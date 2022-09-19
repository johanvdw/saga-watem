#include "Parcel_C_Grid.h"

Parcel_C_Grid::Parcel_C_Grid()
{

	//-----------------------------------------------------
	// Give some information about your tool...

	Set_Name(_TL("3.3. C berekening attribuut"));

	Set_Author(_TL("Copyright (c) 2022, Johan Van de Wauw"));

	Set_Version("1.8.0");

	Set_Description(_TW(
		"Berekening van de C kaart op basis van percelen met C attribuut. "
	)
	);


	//-----------------------------------------------------
	// Define your parameters list...


	CSG_Parameter * shapes = Parameters.Add_Shapes(NULL, "PARCEL_SHAPES", "Percelen", "Percelen (shapefile)", PARAMETER_INPUT, SHAPE_TYPE_Polygon);
	Parameters.Add_Table_Field(shapes, "PARCEL_SHAPE_C_FIELD", "C Field", "Field containing C value");
	Parameters.Add_Grid(
		NULL, "LANDUSE", "Land use grid",
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "C", "C Grid", "C Grid", PARAMETER_OUTPUT, true, SG_DATATYPE_Short); // we use a short to save memory
}

bool Parcel_C_Grid::On_Execute()
{
	CSG_Grid* LANDUSE = Parameters("LANDUSE")->asGrid();
	CSG_Grid* C = Parameters("C")->asGrid();
	// set scaling because short is used 
	C->Set_Scaling(0.001);

	CSG_Shapes* shapes = Parameters("PARCEL_SHAPES")->asShapes();
	int field = Parameters("PARCEL_SHAPE_C_FIELD")->asInt();

	// first convert shapes to grid
	
	bool result;
	SG_RUN_TOOL(result, "grid_gridding", 0, // 0: shapes to grid
		SG_TOOL_PARAMETER_SET("INPUT", shapes)
		&& SG_TOOL_PARAMETER_SET("FIELD", field)
		&& SG_TOOL_PARAMETER_SET("TARGET_DEFINITION", 1)	// grid or grid system
		&& SG_TOOL_PARAMETER_SET("GRID", C)
		&& SG_TOOL_PARAMETER_SET("OUTPUT", 2) // attribute
		&& SG_TOOL_PARAMETER_SET("MULTIPLE", 1) // last value 
		&& SG_TOOL_PARAMETER_SET("POLY_TYPE", 1)//cell: als er een pixels overlapt met infrastructuur moet hele pixels infrastructuur  worden
	);
	

	// fill gaps with reclassified landuse
	#pragma omp parallel for
	for (int i = 0; i < C->Get_NCells(); i++) {
		if (C->is_NoData(i))
		{
			switch (LANDUSE->asInt(i))
			{
			case -9999:
			case -1:
			case -2:
			case 0: break;
			case -3: C->Set_Value(i, 0.001); break;
			default: C->Set_Value(i, 0.37); break;
			}
		}

	}


	return true;
}

Parcel_C_Grid::~Parcel_C_Grid()
{
}