#include "Create_Parcel_Grid.h"



Create_Parcel_Grid::Create_Parcel_Grid()
{
	Set_Name(_TL("1. Prepares input parcel files for watem module."));

	Set_Author("Johan Van de Wauw (2017)");

	Set_Description(_TW(
		"Prepares input parcel file for the watem module."
	));

	Parameters.Add_Shapes(NULL, "PARCEL_SHAPES", "Percelen", "Percelen", PARAMETER_INPUT, SHAPE_TYPE_Polygon);
	Parameters.Add_Grid(NULL, "LANDUSE", "Landgebruik", "Landgebruik", PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, "PRC", "Parcel grid", "Parcel grid",PARAMETER_OUTPUT, true, SG_DATATYPE_Short);

	//Parameters.Add_Shapes(NULL, "INFRA", "Infrastructuur (polygoon)", "Infrastructuur", PARAMETER_INPUT, SHAPE_TYPE_Polygon);

	Parameters.Add_Shapes(NULL, "VHA_POL", "VHA (polygoon)", "VHA (polygoon)", PARAMETER_INPUT, SHAPE_TYPE_Polygon);
	Parameters.Add_Shapes(NULL, "VHA_LINE", "VHA (lijnen)", "VHA (lijnen) - Wlas", PARAMETER_INPUT, SHAPE_TYPE_Line);

	Parameters.Add_Shapes(NULL, "SBN", "GRB Sbn (spoorbaan)", "GRB Sbn (spoorbaan)", PARAMETER_INPUT, SHAPE_TYPE_Polygon);
	Parameters.Add_Shapes(NULL, "WBN", "GRB Wbn (wegbaan)", "GRB Wbn (wegbaan)", PARAMETER_INPUT, SHAPE_TYPE_Polygon);

	Parameters.Add_Shapes(NULL, "WGA", "GRB Wga (wegaanhorigheid)", "GRB Wga (wegaanhorigheid)", PARAMETER_INPUT, SHAPE_TYPE_Polygon);
	
	Parameters.Add_Shapes(NULL, "GBG", "GRB Gbg (gebouw aan de grond)", "GRB Gbg (gebouw aan de grond)", PARAMETER_INPUT, SHAPE_TYPE_Polygon);
	Parameters.Add_Shapes(NULL, "GBA", "GRB Gbg (gebouwaanhorigheid)", "GRB Gba (gebouwaanhorigheid)", PARAMETER_INPUT, SHAPE_TYPE_Polygon);
	Parameters.Add_Shapes(NULL, "TRN", "GRB Trn (terrein)", "GRB Trn (Terrein) - enkel bepaalde klassen worden gebruikt", PARAMETER_INPUT, SHAPE_TYPE_Polygon);
	Parameters.Add_Shapes(NULL, "KNW", "GRB Knw (Kunstwerk)", "GRB Knw (Kunstwerk)", PARAMETER_INPUT, SHAPE_TYPE_Polygon);


	
}


Create_Parcel_Grid::~Create_Parcel_Grid()
{
}


CSG_Grid * BinaryShapetoGrid(CSG_Parameter* shape, const CSG_Grid_System &system, const int poly_type)
{
	CSG_Grid *  grid = new CSG_Grid(system, SG_DATATYPE_Bit);
	SG_RUN_TOOL_ExitOnError("grid_gridding", 0,
		SG_TOOL_PARAMETER_SET("INPUT", shape)
		&& SG_TOOL_PARAMETER_SET("TARGET_DEFINITION", 1)	// grid or grid system
		&& SG_TOOL_PARAMETER_SET("GRID", grid)
		&& SG_TOOL_PARAMETER_SET("OUTPUT", 0) // data / no data
		&& SG_TOOL_PARAMETER_SET("MULTIPLE", 1)
		&& SG_TOOL_PARAMETER_SET("POLY_TYPE", poly_type)//cell: als er een pixels overlapt met infrastructuur moet hele pixels infrastructuur  worden
	);
	return grid;
}

bool Create_Parcel_Grid::On_Execute()
{
	CSG_Grid * prc = Parameters("PRC")->asGrid();
	CSG_Grid * landuse = Parameters("LANDUSE")->asGrid();

	// grid maken van GRB achtergrondlagen (gebouwen, kunstwerken)
	CSG_Grid * gbg, *gba, *wga, *knw;
	gbg = BinaryShapetoGrid(Parameters("GBG"), prc->Get_System(), 1);
	gba = BinaryShapetoGrid(Parameters("GBA"), prc->Get_System(), 1);
	wga = BinaryShapetoGrid(Parameters("WGA"), prc->Get_System(), 1);
	knw = BinaryShapetoGrid(Parameters("KNW"), prc->Get_System(), 1);

	// landgebruik combineren met GRB achtergrondlagen
#pragma omp parallel for
	for (int i = 0; i < prc->Get_NCells(); i++) {
		if (gbg->asInt(i) == 1 || gba->asInt(i) == 1 || wga->asInt(i) == 1 || knw->asInt(i) == 1)
			prc->Set_Value(i, -2);
		else 
			prc->Set_Value(i, landuse->asInt(i));
	}

	delete gbg, gba, wga, knw;

	CSG_Grid * parcel_grid = new CSG_Grid(prc->Get_System(), SG_DATATYPE_Long);

	SG_RUN_TOOL_ExitOnError("grid_gridding", 0, 
		SG_TOOL_PARAMETER_SET("INPUT", Parameters("PARCEL_SHAPES"))
		&& SG_TOOL_PARAMETER_SET("TARGET_DEFINITION", 1)	// grid or grid system
		&& SG_TOOL_PARAMETER_SET("GRID", parcel_grid)
		&& SG_TOOL_PARAMETER_SET("OUTPUT", 1)
		&& SG_TOOL_PARAMETER_SET("MULTIPLE", 1)
		&& SG_TOOL_PARAMETER_SET("POLY_TYPE", 0)
		&& SG_TOOL_PARAMETER_SET("GRID_TYPE", 2) //integer 4 byte
	);

	// vervolgens worden deze percelen hernummerd en over het landgebruik gelegd

	#pragma omp parallel for
	for (int i = 0; i < prc->Get_NCells(); i++) {
		if (!parcel_grid->is_NoData(i) && parcel_grid->asInt(i) >= 0)
		{
			prc->Set_Value(i, (parcel_grid->asInt(i) % 9997) + 2);
		}
	}
	delete parcel_grid;

	
	// grids maken van wegen en waterwegen

	// todo : wegenregister agiv (lijnen)
	CSG_Grid *wbn, *sbn;
	wbn = BinaryShapetoGrid(Parameters("WBN"), prc->Get_System(), 1);
	sbn = BinaryShapetoGrid(Parameters("SBN"), prc->Get_System(), 1);


	CSG_Grid * vha_pol;
	vha_pol = BinaryShapetoGrid(Parameters("VHA_POL"), prc->Get_System(), 1);


	CSG_Grid * vha_line;
	vha_line = new CSG_Grid(prc->Get_System(), SG_DATATYPE_Bit);

	SG_RUN_TOOL_ExitOnError("grid_gridding", 0,
		SG_TOOL_PARAMETER_SET("INPUT", Parameters("VHA_LINE"))
		&& SG_TOOL_PARAMETER_SET("TARGET_DEFINITION", 1)	// grid or grid system
		&& SG_TOOL_PARAMETER_SET("GRID", vha_line)
		&& SG_TOOL_PARAMETER_SET("OUTPUT", 0) // data / no data
		&& SG_TOOL_PARAMETER_SET("MULTIPLE", 1)
		&& SG_TOOL_PARAMETER_SET("LINE_TYPE", 1) //thick lines
	);

	// verschillende grids combineren
	#pragma omp parallel for
	for (int i = 0; i < prc->Get_NCells(); i++) {
		if (vha_line->asInt(i) == 1 || vha_pol->asInt(i) == 1)
			prc->Set_Value(i, -1);
		else if (wbn->asInt(i) == 1 || sbn->asInt(i) == 1)
			prc->Set_Value(i, -2);
	}


	delete vha_pol, vha_line, wbn, sbn;
	//todo: no data op 0 zetten!


	return true;
}