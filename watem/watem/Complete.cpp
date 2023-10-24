#include "Complete.h"

Complete::Complete()
{
	Set_Name(_TL("3. Berekening Erosiekaart (compleet)"));

	Set_Author("Based on Watem (KULeuven). Converted to SAGA by Johan Van de Wauw (2016-2017)");

	Set_Version(VERSION);

	Set_Description(_TW(
		"Volledige berekening erosiekaart in één stap. Deze module voert volgende stappen uit: Berekening upslope area, berekening LS factor, berekening C, "
		"berekening watererosie en optioneel de berekening van de bewerkingserosie. Maakt gebruik van standaardwaarden voor de C-factor."
	));

	Parameters.Add_Grid(NULL, "DEM", _TL("DEM"), _TL("Digitaal hoogtemodel. Eventueel met gebruik van filter."), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "PRC", _TL("Percelen"), _TL("Percelengrid zoals aangemaakt met de tool 'aanmaak percelengrid'"), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "K", _TL("K: bodemerosiegevoeligheidsfactor (ton ha MJ-1 mm-1)"), _TL("K: bodemerosiegevoeligheidsfactor (ton ha MJ-1 mm-1)"), PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, "PIT", _TL("Pit"), _TL("Grid met pits zoals bepaald in de uparea berekening"), PARAMETER_OUTPUT, true, SG_DATATYPE_DWord);
	Parameters.Add_Grid(NULL, "UPSLOPE_AREA", _TL("UPAREA"), _TL("Upslope Area: oppervlakte dat afstroomt naar een pixel"), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "LS", _TL("LS"), _TL("LS: de topografische hellings- en lengtefactor (dimensieloos)"), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "TILL", _TL("Tillage Erosion"), _TL("Gemiddeld bodemverlies als gevolg van bodemerosie  (ton ha-1 jaar-1)"), PARAMETER_OUTPUT_OPTIONAL);

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
		"Correctiefactor omdat de berekeningen bepaald werden op een standaard plot van 22.1 meter. 1.4 voor grids van 5x5", 
		PARAMETER_TYPE_Double, 1.4, 0, 20
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
		NULL, "SAVE_MEMORY", "Save memory", 
		"Optie om het geheugengebruik tijdens het uitvoeren te verminderen. Opgelet: kan de doorlooptijd sterk verhogen.", 
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL, "PIT_FLOW", "Flow from pits into closeby cells (within radius)", "", PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		"PIT_FLOW", "PIT_RADIUS", "Search radius from pit.",
		"Maximum radius from a pit to which upstream water can flow", PARAMETER_TYPE_Int, 4, 0);

	Parameters.Add_Value(
		NULL,"LS_USE_PRC", "Perceelsgrenzen gebruiken in LS berekening", 
		"Perceelsgrenzen gebruiken in LS berekening (slope enkel binnen veld). ", PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Choice("",
		"METHOD", _TL("LS Calculation"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Moore & Nieber 1989"),
			_TL("Desmet & Govers 1996 (standaard WATEM)"),
			_TL("Wischmeier & Smith 1978"),
			_TL("Van Oost, 2003")
		), 1
	);

	Parameters.Add_Value(
		NULL, "EROSION_CROP_MAX", "Maximale erosie per pixel aftoppen",
		"", PARAMETER_TYPE_Bool, 1
	);

	Parameters.Add_Value(
		"EROSION_CROP_MAX", "EROSION_MAX", "Maximale erosie, hogere waarden worden afgetopt",
		"", PARAMETER_TYPE_Double, 150, 0, 10000000
	);


};

bool Complete::On_Execute(void)
{
	bool savememory = Parameters("SAVE_MEMORY")->asBool();
	
	CSG_Grid * K = Parameters("K")->asGrid();
	CSG_Grid * PRC = Parameters("PRC")->asGrid();
	CSG_Grid * DEM = Parameters("DEM")->asGrid();
	CSG_Grid * UPSLOPE_AREA = Parameters("UPSLOPE_AREA")->asGrid();
	CSG_Grid * PIT = Parameters("PIT")->asGrid();
	CSG_Grid * water_erosion = Parameters("WATER_EROSION")->asGrid();

	CSG_Grid * ls = Parameters("LS")->asGrid();

	if (savememory) {
		K->Set_Cache(true);
		water_erosion->Set_Cache(true);
		ls->Set_Cache(true);
	}
		
	SG_RUN_TOOL_ExitOnError("watem", 4, //uparea,
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
		//DEM->Set_Cache(true);
		//PRC->Set_Cache(true);
		PIT->Set_Cache(true);
		ls->Set_Cache(false);
	}

	SG_RUN_TOOL_ExitOnError("watem", 5, //LS calculation,
		SG_TOOL_PARAMETER_SET("DEM", Parameters("DEM"))
		&& SG_TOOL_PARAMETER_SET("UPSLOPE_AREA", Parameters("UPSLOPE_AREA"))
		&& SG_TOOL_PARAMETER_SET("USEPRC", Parameters("LS_USE_PRC"))
		&& SG_TOOL_PARAMETER_SET("PRC", Parameters("PRC"))
		&& SG_TOOL_PARAMETER_SET("LS", Parameters("LS"))
		&& SG_TOOL_PARAMETER_SET("METHOD", Parameters("METHOD"))
	);

	if (savememory) {
		DEM->Set_Cache(true);
		UPSLOPE_AREA->Set_Cache(true);
	}

	// include connectivity in ls area name
	int conn = 100 - Parameters("PCTOCROP")->asDouble();
	CSG_String connectivity_string = CSG_String::Format("%d", conn);

	ls->Set_Name("LS_" + connectivity_string);

	// C grid genereren op basis van percelengrid
	CSG_Grid * C = new CSG_Grid(Get_System(), SG_DATATYPE_Short); // we use a short to save memory

	SG_RUN_TOOL_ExitOnError("watem", 8, //watererosie op basis LS,
		SG_TOOL_PARAMETER_SET("PRC", Parameters("PRC"))
		&& SG_TOOL_PARAMETER_SET("C", C)
	);

	if (savememory) {
		PRC->Set_Cache(true);
		water_erosion->Set_Cache(false);
		K->Set_Cache(false);
	}

	SG_RUN_TOOL_ExitOnError("watem", 6, //watererosie op basis LS,
		SG_TOOL_PARAMETER_SET("LS", Parameters("LS"))
		&& SG_TOOL_PARAMETER_SET("K", Parameters("K"))
		&& SG_TOOL_PARAMETER_SET("C", C)
		&& SG_TOOL_PARAMETER_SET("WATER_EROSION", Parameters("WATER_EROSION"))
		&& SG_TOOL_PARAMETER_SET("R", Parameters("R"))
		&& SG_TOOL_PARAMETER_SET("P", Parameters("P"))
		&& SG_TOOL_PARAMETER_SET("CORR", Parameters("CORR"))
		&& SG_TOOL_PARAMETER_SET("EROSION_CROP_MAX", Parameters("EROSION_CROP_MAX"))
		&& SG_TOOL_PARAMETER_SET("EROSION_MAX", Parameters("EROSION_MAX"))
		);

	delete C;

	water_erosion->Set_Name("watero_" + connectivity_string);

	if (savememory) {
		K->Set_Cache(true);
		PIT->Set_Cache(false);
		UPSLOPE_AREA->Set_Cache(false);
		ls->Set_Cache(false);
	}

	if (Parameters("TILL")->asGrid() != NULL)
	{
		SG_RUN_TOOL_ExitOnError("watem", 7, //tillage erosion op basis LS,
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
