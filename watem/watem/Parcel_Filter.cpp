#include "Parcel_Filter.h"



Parcel_Filter::Parcel_Filter()
{
	Set_Name(_TL("2. 3x3 filter binnen perceelsgrenzen"));

	Set_Author(_TL("Copyright (c) 2017.  Johan Van de Wauw"));

	Set_Version(VERSION);

	Set_Description(_TW(
		"De tool voert een 3x3 filter uit op een digitaal hoogtemodel die enkel cellen binnen het perceel gebruikt."
	)
	);


	//-----------------------------------------------------
	// Define your parameters list...


	Parameters.Add_Grid(
		NULL, "DEM", "Elevation",
		"Digitaal hoogtemodel. ",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "PRC", "Percelen",
		"Percelengrid met unieke identifier per perceel, kan aangemaakt worden met de tool 'aanmaak percelengrid'",
		PARAMETER_INPUT
	);


	Parameters.Add_Grid(
		NULL, "DEM_FILTER", "Filtered DEM",
		"DEM gefilterd met een 3x3 filter die enkel binnen de perceelsgrenzen actief is.",
		PARAMETER_OUTPUT
	);


}

Parcel_Filter::~Parcel_Filter()
{
}


bool Parcel_Filter::On_Execute()
{

	// Get parameter settings...
	CSG_Grid * DEM = Parameters("DEM")->asGrid();
	CSG_Grid * PRC = Parameters("PRC")->asGrid();
	CSG_Grid * DEM_FILTER = Parameters("DEM_FILTER")->asGrid();


	for (int y = 0; y < Get_NY() && Set_Progress(y); y++) {
#pragma omp parallel for
		for (int x = 0; x < Get_NX(); x++) {
			if (DEM->is_NoData(x, y))
			{
				DEM_FILTER->Set_NoData(x, y);
				continue;
			}
				
			int current_prc = PRC->asInt(x,y);
			double sum = DEM->asDouble(x, y);
			int n = 1;
			for (int i = 0; i < 8; i++)
			{
				int ix = Get_xTo(i, x);
				int iy = Get_yTo(i, y);
				// all surrounding cells are added if they contain data and are in the grid and in the same parcel
				if (is_InGrid(ix, iy) && !(DEM->is_NoData(ix,iy)) && PRC->asInt(ix, iy) == current_prc)
				{
					sum += DEM->asDouble(ix, iy);
					n++;
				}
			}
			DEM_FILTER->Set_Value(x, y, sum / n);
		}
	}
	return true;
}
