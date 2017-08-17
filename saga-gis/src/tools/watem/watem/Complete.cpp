#include "Complete.h"

Complete::Complete()
{
	Set_Name(_TL("Complete Berekening Erosiekaart"));

	Set_Author("Based on Watem (KULeuven). Converted to SAGA by Johan Van de Wauw (2016)");

	Set_Description(_TW(
		"Volledige berekening erosiekaart in 1 stap."
	));

	Parameters.Add_Grid(NULL, "DEM", _TL("DEM"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "PRC", _TL("Percelen"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "K", _TL("K: bodemerosiegevoeligheidsfactor (ton ha MJ-1 mm-1)"), _TL("K: bodemerosiegevoeligheidsfactor (ton ha MJ-1 mm-1)"), PARAMETER_INPUT);
	//Parameters.Add_Grid(NULL, "C", _TL("C: de gewas- en bedrijfsvoeringsfactor (dimensieloos)"), _TL("C: de gewas- en bedrijfsvoeringsfactor (dimensieloos)"), PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, "PIT", _TL("Pit"), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_DWord);
	Parameters.Add_Grid(NULL, "UPSLOPE_AREA", _TL("UPAREA"), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "LS", _TL("LS"), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "TILL", _TL("Tillage Erosion"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Value(
		NULL, "R", "R",
		"regenerosiviteitsfactor (MJ mm ha-1 jaar-1)", PARAMETER_TYPE_Double, 880, 0, 1000
	);

	Parameters.Add_Value(
		NULL, "P", "P",
		"de erosiebeheersingsfactor (dimensieloos) ", PARAMETER_TYPE_Double, 1, 0, 1
	);

	Parameters.Add_Value(
		NULL, "CORR", "correctiefactor grid",
		"Correctiefactor omdat de berekeningen bepaald werden op een standaard plot van 22.1 meter. 1.4 voor grids van 5x5", PARAMETER_TYPE_Double, 1.4, 0, 20
	);


	Parameters.Add_Grid(
		NULL, "WATER_EROSION", _TL("Water erosion"),
		"A: gemiddeld bodemverlies als gevolg van geul- en intergeulerosie (ton ha-1 jaar-1)",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL, "PCTOCROP", "Parcel Connectivity to cropland",
		"", PARAMETER_TYPE_Double, 70, 0, 100
	);

	Parameters.Add_Value(
		NULL, "PCTOFOREST", "Parcel Connectivity to forest",
		"", PARAMETER_TYPE_Double, 100, 0, 100
	);

	Parameters.Add_Value(
		NULL, "SAVE_MEMORY", "Save memory", "save memory", PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL, "PIT_FLOW", "Flow from pits into closeby cells (within radius)", "", PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		"PIT_FLOW", "PIT_RADIUS", "Search radius from pit.", "Maximum radius from a pit to which upstream water can flow", PARAMETER_TYPE_Int, 4, 0);

};

bool Complete::On_Execute(void)
{
	bool savememory = Parameters("SAVE_MEMORY")->asBool();
	if (savememory)
		return false; //optio not working correctly


	CSG_Grid * K = Parameters("K")->asGrid();
	CSG_Grid * PRC = Parameters("PRC")->asGrid();
	CSG_Grid * DEM = Parameters("DEM")->asGrid();
	CSG_Grid * UPSLOPE_AREA = Parameters("UPSLOPE_AREA")->asGrid();
	CSG_Grid * PIT = Parameters("PIT")->asGrid();

	if (savememory)
		K->Set_Compression(true);

	SG_RUN_TOOL_ExitOnError("watem", 1, //uparea,
		SG_TOOL_PARAMETER_SET("DEM", Parameters("DEM"))
		&& SG_TOOL_PARAMETER_SET("PRC", Parameters("PRC"))
		&& SG_TOOL_PARAMETER_SET("PIT", Parameters("PIT"))
		&& SG_TOOL_PARAMETER_SET("UPSLOPE_AREA", Parameters("UPSLOPE_AREA"))
		&& SG_TOOL_PARAMETER_SET("PCTOCROP", Parameters("PCTOCROP"))
		&& SG_TOOL_PARAMETER_SET("PCTOFOREST", Parameters("PCTOFOREST"))
		&& SG_TOOL_PARAMETER_SET("PCTOROAD", Parameters("PCTOCROP"))
		&& SG_TOOL_PARAMETER_SET("PIT_FLOW", Parameters("PIT_FLOW"))
		&& SG_TOOL_PARAMETER_SET("PIT_RADIUS", Parameters("PIT_RADIUS"))
	);// als waarde voor PCTOROAD wordt hier (bewust) ook de waarde van crop gebruikt

	if (savememory) {
		DEM->Set_Index(false);
		DEM->Set_Compression(true);
		PRC->Set_Compression(true);
		PIT->Set_Compression(true);
		UPSLOPE_AREA->Set_Compression(true);
	}


	SG_RUN_TOOL_ExitOnError("watem", 2, //LS calculation,
		SG_TOOL_PARAMETER_SET("DEM", Parameters("DEM"))
		&& SG_TOOL_PARAMETER_SET("UPSLOPE_AREA", Parameters("UPSLOPE_AREA"))
		&& SG_TOOL_PARAMETER_SET("LS", Parameters("LS"))
	);

	// include connectivity in ls area name

	int conn = 100 - Parameters("PCTOCROP")->asDouble();
	CSG_String connectivity_string = CSG_String::Format("%d", conn);

	CSG_Grid * ls;
	ls = Parameters("LS")->asGrid();
	ls->Set_Name("LS_" + connectivity_string);

	if (savememory) {
		ls->Set_Compression(true);
	}

	// C grid genereren op basis van percelengrid
	CSG_Grid * C = new CSG_Grid(*Get_System(), SG_DATATYPE_Float);



	#pragma omp parallel for
	for (int i = 0; i < C->Get_NCells(); i++) {
		switch (PRC->asInt(i))
		{
			case 10000: C->Set_Value(i, 0.001); break;
			case -1:
			case -2:
			case 0: C->Set_Value(i, 0); break;
			default: C->Set_Value(i, 0.37); break;
		}
			

		
	}

	SG_RUN_TOOL_ExitOnError("watem", 3, //watererosie op basis LS,
		SG_TOOL_PARAMETER_SET("LS", Parameters("LS"))
		&& SG_TOOL_PARAMETER_SET("K", Parameters("K"))
		&& SG_TOOL_PARAMETER_SET("C", C)
		&& SG_TOOL_PARAMETER_SET("WATER_EROSION", Parameters("WATER_EROSION"))
		&& SG_TOOL_PARAMETER_SET("R", Parameters("R"))
		&& SG_TOOL_PARAMETER_SET("P", Parameters("P"))
		&& SG_TOOL_PARAMETER_SET("CORR", Parameters("CORR"))
		);

	delete C;


	CSG_Grid * water_erosion;
	water_erosion = Parameters("WATER_EROSION")->asGrid();
	water_erosion->Set_Name("water_erosion_" + connectivity_string);

	if (Parameters("TILL")->asGrid() != NULL)
	{
		SG_RUN_TOOL_ExitOnError("watem", 4, //tillage erosion op basis LS,
			SG_TOOL_PARAMETER_SET("DEM", Parameters("DEM"))
			&& SG_TOOL_PARAMETER_SET("PRC", Parameters("PRC"))
			&& SG_TOOL_PARAMETER_SET("TILL", Parameters("TILL"))
		);
	}
	return true;
}

Complete::~Complete()
{
}
