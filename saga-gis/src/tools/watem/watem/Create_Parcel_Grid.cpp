#include "Create_Parcel_Grid.h"



Create_Parcel_Grid::Create_Parcel_Grid()
{
	Set_Name(_TL("Prepares input parcel files for watem module."));

	Set_Author("Johan Van de Wauw (2017)");

	Set_Description(_TW(
		"Prepares input parcel file for watem."
	));

	Parameters.Add_Shapes(NULL, "PARCEL_SHAPES", "Percelen", "Percelen", PARAMETER_INPUT, SHAPE_TYPE_Polygon);
	Parameters.Add_Grid(NULL, "PARCEL_GRID", "Parcel grid", "Parcel grid",PARAMETER_OUTPUT, true, SG_DATATYPE_Short);

	Parameters.Add_Shapes(NULL, "INFRA", "Infrastructuur (polygoon)", "Infrastructuur", PARAMETER_INPUT, SHAPE_TYPE_Polygon);
	Parameters.Add_Shapes(NULL, "VHA_POL", "VHA (polygoon)", "VHA (polygoon)", PARAMETER_INPUT, SHAPE_TYPE_Polygon);
	Parameters.Add_Shapes(NULL, "VHA_LINE", "VHA (lijnen)", "VHA (lijnen) - Wlas", PARAMETER_INPUT, SHAPE_TYPE_Line);
	
}


Create_Parcel_Grid::~Create_Parcel_Grid()
{
}

bool Create_Parcel_Grid::On_Execute()
{
	CSG_Grid * parcel_grid = Parameters("PARCEL_GRID")->asGrid();

	SG_RUN_TOOL_ExitOnError("grid_gridding", 0, 
		SG_TOOL_PARAMETER_SET("INPUT", Parameters("PARCEL_SHAPES"))
		&& SG_TOOL_PARAMETER_SET("TARGET_DEFINITION", 1)	// grid or grid system
		&& SG_TOOL_PARAMETER_SET("GRID", parcel_grid)
		&& SG_TOOL_PARAMETER_SET("OUTPUT", 1)
		&& SG_TOOL_PARAMETER_SET("MULTIPLE", 1)
		&& SG_TOOL_PARAMETER_SET("POLY_TYPE", 0)
		&& SG_TOOL_PARAMETER_SET("GRID_TYPE", 1) //integer 2 byte
	);

	// vervolgens worden deze percelen hernummerd

	#pragma omp parallel for
	for (int i = 0; i < parcel_grid->Get_NCells(); i++) {
		parcel_grid->Set_Value(i, (parcel_grid->asInt(i) % 9997) + 2);
	}

	// grid maken van infrastructuur
	CSG_Grid * infra;
	infra = new CSG_Grid(parcel_grid->Get_System(), SG_DATATYPE_Bit);

	SG_RUN_TOOL_ExitOnError("grid_gridding", 0,
		SG_TOOL_PARAMETER_SET("INPUT", Parameters("INFRA"))
		&& SG_TOOL_PARAMETER_SET("TARGET_DEFINITION", 1)	// grid or grid system
		&& SG_TOOL_PARAMETER_SET("GRID", infra)
		&& SG_TOOL_PARAMETER_SET("OUTPUT", 0) // data / no data
		&& SG_TOOL_PARAMETER_SET("MULTIPLE", 1)
		&& SG_TOOL_PARAMETER_SET("POLY_TYPE", 0)
	);

	// grids maken van waterwegen
	CSG_Grid * vha_pol;
	vha_pol = new CSG_Grid(parcel_grid->Get_System(), SG_DATATYPE_Bit);

	SG_RUN_TOOL_ExitOnError("grid_gridding", 0,
		SG_TOOL_PARAMETER_SET("INPUT", Parameters("VHA_POL"))
		&& SG_TOOL_PARAMETER_SET("TARGET_DEFINITION", 1)	// grid or grid system
		&& SG_TOOL_PARAMETER_SET("GRID", vha_pol)
		&& SG_TOOL_PARAMETER_SET("OUTPUT", 0) // data / no data
		&& SG_TOOL_PARAMETER_SET("MULTIPLE", 1)
		&& SG_TOOL_PARAMETER_SET("POLY_TYPE", 0)
	);

	CSG_Grid * vha_line;
	vha_line = new CSG_Grid(parcel_grid->Get_System(), SG_DATATYPE_Bit);

	SG_RUN_TOOL_ExitOnError("grid_gridding", 0,
		SG_TOOL_PARAMETER_SET("INPUT", Parameters("VHA_LINE"))
		&& SG_TOOL_PARAMETER_SET("TARGET_DEFINITION", 1)	// grid or grid system
		&& SG_TOOL_PARAMETER_SET("GRID", vha_line)
		&& SG_TOOL_PARAMETER_SET("OUTPUT", 0) // data / no data
		&& SG_TOOL_PARAMETER_SET("MULTIPLE", 1)
		&& SG_TOOL_PARAMETER_SET("POLY_TYPE", 0)
	);

	// verschillende grids combineren
#pragma omp parallel for
	for (int i = 0; i < parcel_grid->Get_NCells(); i++) {
		if (vha_line->asInt(i) == 1 || vha_pol->asInt(i) == 1)
			parcel_grid->Set_Value(i, -1);
		else if (infra->asInt(i) == 1)
			parcel_grid->Set_Value(i, -2);
	}

	delete infra;
	delete vha_pol;
	delete vha_line;

	return true;
}