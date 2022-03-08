#include "routing_shape.h"

routing_shape::routing_shape(void)
{
    Set_Name(_TL("Routing to shape"));

    Set_Author(_TL("Copyrights (c) 2018 by Johan Van de Wauw"));

    Set_Description(_TL(
        "Routing to shape")
    );

    //----------------------------------------------------

    Parameters.Add_Table(
        NULL, "ROUTING"	, _TL("Routing Table"),
        _TL(""),
        PARAMETER_INPUT
    );
    Parameters.Add_Grid(
        NULL, "LANDUSE", _TL("Landuse"),
        _TL(""), PARAMETER_INPUT);
    Parameters.Add_Shapes(
        NULL, "OUTPUTLINES"	, _TL("Output Lines"),
        _TL(""),
        PARAMETER_OUTPUT, SHAPE_TYPE_Line
    );

}

routing_shape::~routing_shape(void)
{}

TSG_Point routing_shape::to_world(int col, int row)
{
    // note: CN/WS counts the opposite way for rows + zero based;
    return system.Get_Grid_to_World(col-1, system.Get_NY() - row);
}


void routing_shape::Add_Line(int col, int row, int t_col, int t_row, double dist, double part)
{
    CSG_Shape * line = lines->Add_Shape();
    TSG_Point orig = to_world(col, row);
    TSG_Point to = to_world(t_col, t_row);

    line->Add_Point(orig);
    line->Add_Point(to);
    line->Set_Value("col", col);
    line->Set_Value("row", row);
    line->Set_Value("targetcol", t_col);
    line->Set_Value("targetrow", t_row);
    line->Set_Value("part", part);
    line->Set_Value("distance", dist);

    line->Set_Value("sourceX", orig.x);
    line->Set_Value("sourceY", orig.y);
    line->Set_Value("targetX", to.x);
    line->Set_Value("targetY", to.y);
    line->Set_Value("jump", dist > 1.1 * system.Get_Cellsize()*sqrt(2));

    // switch to saga grid definition for overlay of landuse
    row = system.Get_NY() - row;
    t_row = system.Get_NY() - t_row;
    col = col -1;
    t_col = t_col - 1;
	if (landuse->is_InGrid(col, row))
		line->Set_Value("lnduSource", landuse->asDouble(col, row));
	if (landuse->is_InGrid(t_col, t_row))
		line->Set_Value("lnduTarg", landuse->asDouble(t_col, t_row));
}


bool routing_shape::On_Execute(void)
{
    CSG_Table * routing = Parameters("ROUTING")->asTable();
    landuse = Parameters("LANDUSE")->asGrid();
    lines = Parameters("OUTPUTLINES")->asShapes();

    lines->Del_Records();
    for (int i=0; i<lines->Get_Field_Count(); i++)
        lines->Del_Field(i);

    lines->Add_Field("col", SG_DATATYPE_Int);
    lines->Add_Field("row", SG_DATATYPE_Int);
    lines->Add_Field("targetcol", SG_DATATYPE_Int);
    lines->Add_Field("targetrow", SG_DATATYPE_Int);
    lines->Add_Field("part", SG_DATATYPE_Double);
    lines->Add_Field("distance", SG_DATATYPE_Double);
    lines->Add_Field("lnduSource", landuse->Get_Type());
    lines->Add_Field("lnduTarg", landuse->Get_Type());
    lines->Add_Field("jump", SG_DATATYPE_Int);
    lines->Add_Field("targetX", SG_DATATYPE_Double);
    lines->Add_Field("targetY", SG_DATATYPE_Double);
    lines->Add_Field("sourceX", SG_DATATYPE_Double);
    lines->Add_Field("sourceY", SG_DATATYPE_Double);

    system = landuse->Get_System();

    int col_field = routing->Get_Field("col");
    int row_field = routing->Get_Field("row");
    int t1col = routing->Get_Field("target1col");
    int t1row = routing->Get_Field("target1row");
    int t2col = routing->Get_Field("target2col");
    int t2row = routing->Get_Field("target2row");

    for (int i=0; i< routing->Get_Count(); i++)
    {
        CSG_Table_Record * row = routing->Get_Record(i);

        if (row->asInt(t1col)>0 )
            Add_Line(row->asInt(col_field), row->asInt(row_field), row->asInt(t1col), row->asInt(t1row), row->asDouble("distance1"), row->asDouble("part1"));


        if (row->asInt(t2col)>0)
            Add_Line(row->asInt(col_field), row->asInt(row_field), row->asInt(t2col), row->asInt(t2row), row->asDouble("distance2"), row->asDouble("part2"));

    }

    return true;
}

