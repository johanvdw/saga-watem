#include "Calculate_C_Grid.h"


Calculate_C_Grid::Calculate_C_Grid()
{

	//-----------------------------------------------------
	// Give some information about your tool...

	Set_Name(_TL("3.3. C berekening"));

	Set_Author(_TL("Copyright (c) 2017, Johan Van de Wauw"));

	Set_Version(VERSION);

	Set_Description(_TW(
		"Berekening van de C kaart op basis van de percelenkaart. "
	)
	);


	//-----------------------------------------------------
	// Define your parameters list...


	Parameters.Add_Grid(
		NULL, "PRC", "Percelengrid",
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "C", "C Grid", "C Grid", PARAMETER_OUTPUT, true,SG_DATATYPE_Short); // we use a short to save memory
}

bool Calculate_C_Grid::On_Execute()
{
	CSG_Grid * PRC = Parameters("PRC")->asGrid();
	CSG_Grid * C = Parameters("C")->asGrid();
	C->Set_Scaling(0.001);


#pragma omp parallel for
	for (int i = 0; i < C->Get_NCells(); i++) {
		switch (PRC->asInt(i))
		{
		case 10000: 
			C->Set_Value(i, 0.001); break;
		case -9999:
		case 0: 
			C->Set_NoData(i); break;
		case -1:
		case -2:
			C->Set_Value(i, 0 ); break;
		default: 
			C->Set_Value(i, 0.37); break;
		}
	}

	return true;
}

Calculate_C_Grid::~Calculate_C_Grid()
{
}
