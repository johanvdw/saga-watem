#include "Complete.h"



Complete::Complete()
{
	Set_Name(_TL("Complete Watem"));

	Set_Author("Based on Watem (KULeuven). Converted to SAGA by Johan Van de Wauw (2016)");

	Set_Description(_TW(
		"All steps of Watem in one go"
	));

	Parameters.Add_Grid(NULL, "DEM", _TL("DEM"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "PRC", _TL("Percelen"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "K", _TL("K: bodemerosiegevoeligheidsfactor (ton ha MJ-1 mm-1)"), _TL("K: bodemerosiegevoeligheidsfactor (ton ha MJ-1 mm-1)"), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "C", _TL("C: de gewas- en bedrijfsvoeringsfactor (dimensieloos)"), _TL("C: de gewas- en bedrijfsvoeringsfactor (dimensieloos)"), PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, "UPSLOPE_AREA", _TL("UPAREA"), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "PIT", _TL("Pit"), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "LS", _TL("LS"), _TL(""), PARAMETER_OUTPUT);
	//Parameters.Add_Grid(NULL, "TILL_EROSION", _TL("Tillage Erosion"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Value(
		NULL, "R", "regenerosiviteitsfactor (MJ mm ha-1 jaar-1)",
		"", PARAMETER_TYPE_Double, 880, 0, 1000
	);

	Parameters.Add_Value(
		NULL, "P", "de erosiebeheersingsfactor (dimensieloos) ",
		"", PARAMETER_TYPE_Double, 1, 0, 1
	);

	Parameters.Add_Value(
		NULL, "CORR", "correctiefactor grid",
		"", PARAMETER_TYPE_Double, 1.4, 0, 20
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
		NULL, "PCTOROAD", "Parcel Connectivity to road/built-up areas",
		"", PARAMETER_TYPE_Double, 70, 0, 100
	);


}

bool Complete::On_Execute(void)
{

	SG_RUN_TOOL_ExitOnError("watem", 1, //uparea,
		SG_TOOL_PARAMETER_SET("DEM", Parameters("DEM"))
		&& SG_TOOL_PARAMETER_SET("PRC", Parameters("PRC"))
		&& SG_TOOL_PARAMETER_SET("PIT", Parameters("PIT"))
		&& SG_TOOL_PARAMETER_SET("UPSLOPE_AREA", Parameters("UPSLOPE_AREA"))
		&& SG_TOOL_PARAMETER_SET("PCTOCROP", Parameters("PCTOCROP"))
		&& SG_TOOL_PARAMETER_SET("PCTOFOREST", Parameters("PCTOFOREST"))
		&& SG_TOOL_PARAMETER_SET("PCTOROAD", Parameters("PCTOROAD"))
	);
	return true;
}

Complete::~Complete()
{
}
