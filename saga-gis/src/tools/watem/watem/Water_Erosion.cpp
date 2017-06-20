#include "Water_Erosion.h"



Water_Erosion::Water_Erosion()
{
	//-----------------------------------------------------
	// Give some information about your tool...

	Set_Name(_TL("03: Watererosie berekening op basis van LS"));

	Set_Author(_TL("Copyrights (c) 2016 by KULeuven. Converted to SAGA/C++ by Johan Van de Wauw"));

	Set_Description(_TW(
		"(c) 2016")
	);


	//-----------------------------------------------------
	// Define your parameters list...


	Parameters.Add_Grid(
		NULL, "LS", "LS",
		"LS: de topografische hellings- en lengtefactor (dimensieloos)",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "K", "K: bodemerosiegevoeligheidsfactor (ton ha MJ-1 mm-1)",
		"K: bodemerosiegevoeligheidsfactor (ton ha MJ-1 mm-1)",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "C", "C: de gewas- en bedrijfsvoeringsfactor (dimensieloos)",
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Value(
		NULL, "R", "regenerosiviteitsfactor (MJ mm ha-1 jaar-1)",
		"", PARAMETER_TYPE_Double, 880, 0, 1000
	);

	Parameters.Add_Value(
		NULL, "P", "de erosiebeheersingsfactor (dimensieloos) ",
		"de erosiebeheersingsfactor (dimensieloos)", PARAMETER_TYPE_Double, 1, 0, 1
	);

	Parameters.Add_Value(
		NULL, "CORR", "correctiefactor grid",
		"Correctiefactor omdat de berekeningen bepaald werden op een standaard plot van 22.1 meter. 1.4 voor grids van 5x5", PARAMETER_TYPE_Double, 1.4, 0, 20
	);

	Parameters.Add_Grid(
		NULL, "WATER_EROSION", _TL("Water erosion"),
		"Gemiddeld bodemverlies als gevolg van geul- en intergeulerosie (ton ha-1 jaar-1)",
		PARAMETER_OUTPUT
	);
}


bool Water_Erosion::On_Execute()
{
	//-----------------------------------------------------
	// Get parameter settings...
	K = Parameters("K")->asGrid();
	C = Parameters("C")->asGrid();
	LS = Parameters("LS")->asGrid();


	water_erosion = Parameters("WATER_EROSION")->asGrid();

	R = Parameters("R")->asDouble();
	P = Parameters("P")->asDouble();
	double corr = Parameters("CORR")->asDouble();
#pragma omp parallel for
	for (int i = 0; i < Get_NCells(); i++){
			double v = R * K->asDouble(i) * LS->asDouble(i) * C->asDouble(i) * P / corr; //correctiefactor voor 5x5 grid ipv 25x25 grid
			if (v > 150)
				v = 150;
			if (v < -1) v = -99;
			water_erosion->Set_Value(i, v);
		}

	water_erosion->Set_NoData_Value(-99);
	
	return true;
}

Water_Erosion::~Water_Erosion()
{
}
