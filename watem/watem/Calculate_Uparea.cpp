#include "Calculate_Uparea.h"

#if SAGA_MAJOR_VERSION >=9
#define PROGRESS Set_Progress_Cells
#else
# define PROGRESS Set_Progress_NCells
#endif
CCalculate_Uparea::CCalculate_Uparea()
{
	Set_Name(_TL("3.1. Uparea berekening"));

	Set_Version(VERSION);

	Set_Author(_TL("Copyright (c) 2006 - 2016 by KULeuven. Converted to SAGA/C++ by Johan Van de Wauw"));

	Set_Description(_TW(
		"This module converts a digital elevation model grid and a parcel grid and converts it to an upslope area.\n\n"
		"The method takes into account parcel borders, rivers (grid value -1) and landuse (forest has value 10000).\n"
		"Pits are taken into account. Optionally a grid with Pits and a table with pit information can be generated. "
		)
	);

	Parameters.Add_Grid(
		NULL, "DEM", "Digitaal hoogtemodel",
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "PRC", "Percelen",
		"Percelengrid met unieke identifier per perceel. Bossen krijgen waarde 10000. Bebouwde gebieden en wegen -2 en rivieren -1.",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "PIT", "Pit",
		"Gridfile met pits",
		PARAMETER_OUTPUT,
		true,
		SG_DATATYPE_DWord
	);

	Parameters.Add_Grid(
		NULL, "UPSLOPE_AREA", _TL("Upslope Area"),
		"Upslope Area: oppervlakte dat afstroomt naar een pixel",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table(
		NULL, "PITDATA", _TL("Pit data"),
		"Tabel met gegevens per gevonden pit. Coordinaten volgens het grid in watem.",
		PARAMETER_OUTPUT_OPTIONAL
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

	Parameters.Add_Value(
		NULL, "WRONG", "Use old (wrong) calculation near roads and rivers",
		"De oorspronkelijke berekening gebruiken voor wegen en rivieren of de gecorrigeerde versie.", 
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL, "PIT_FLOW", "Flow from pits into closeby cells (within radius)", 
		"Geeft aan of in de buurt van pits gezocht moet worden naar lagergelegen cellen waar dan nog water naar kan stromen. Gebruikt de maximale zoekstraal die hieronder bepaald wordt. ",
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		"PIT_FLOW", "PIT_RADIUS", "Search radius from pit.", 
		"Maximale zoekstraal waarin naar pixels gezocht wordt die lager liggen dan gevonden pits. Enkel van toepassing als ook PIT_FLOW geselecterd is.",
		PARAMETER_TYPE_Int, 4, 0);
}

bool CCalculate_Uparea::On_Execute(void)
{

	//-----------------------------------------------------
	// Get parameter settings...

	DEM = Parameters("DEM")->asGrid();
	Pit = Parameters("PIT")->asGrid();
	PRC = Parameters("PRC")->asGrid();
	Up_Area = Parameters("UPSLOPE_AREA")->asGrid();
	DataObject_Set_Colors(Up_Area, 11, SG_COLORS_WHITE_BLUE, false);

	pPitDataTable = Parameters("PITDATA")->asTable();

	TFCAtoCropLand = Parameters("PCTOCROP")->asDouble();
	TFCAtoForestOrPasture = Parameters("PCTOFOREST")->asDouble();
	TFCAtoRoad = Parameters("PCTOROAD")->asDouble();

	wrong = Parameters("WRONG")->asBool();

	pit_flow = Parameters("PIT_FLOW")->asBool();
	pit_radius = Parameters("PIT_RADIUS")->asInt();

	// include connectivity in upslope area name
	int conn = 100 - TFCAtoCropLand;
	CSG_String connectivity_string = CSG_String::Format("%d", conn);
	Up_Area->Set_Name("Upslope_area_" + connectivity_string);

	//-----------------------------------------------------
	// Execute calculation...

	// make sure m_pUp_Area is empty
	Up_Area->Assign(0.0);
	for (int i = 0; i < Get_NX(); i++)


	pitwin = 200;
	pitnum = 0;
	PitDat.empty();

	CalculatePitStuff();
	CalculateUparea();

	//export pitdata table if this is defined
	if (pPitDataTable != NULL) {
		pPitDataTable->Del_Records();
		if (pPitDataTable->Get_Field("ID") == -1)
			pPitDataTable->Add_Field("ID", SG_DATATYPE_Int);
		if (pPitDataTable->Get_Field("OutRow") == -1)
			pPitDataTable->Add_Field("OutRow", SG_DATATYPE_Int);
		if (pPitDataTable->Get_Field("OutCol") == -1)
			pPitDataTable->Add_Field("OutCol", SG_DATATYPE_Int);
		if (pPitDataTable->Get_Field("Number") == -1)
			pPitDataTable->Add_Field("Number", SG_DATATYPE_Int);
		if (pPitDataTable->Get_Field("C") == -1)
			pPitDataTable->Add_Field("C", SG_DATATYPE_Int);
		if (pPitDataTable->Get_Field("R") == -1)
			pPitDataTable->Add_Field("R", SG_DATATYPE_Int);
		if (pPitDataTable->Get_Field("OutX") == -1)
			pPitDataTable->Add_Field("OutX", SG_DATATYPE_Double);
		if (pPitDataTable->Get_Field("OutY") == -1)
			pPitDataTable->Add_Field("OutY", SG_DATATYPE_Double);
		if (pPitDataTable->Get_Field("input") == -1)
			pPitDataTable->Add_Field("input", SG_DATATYPE_Double);
		int i = 1; // initial ID
		for (TPitData& pit : PitDat)
		{
			CSG_Table_Record * row = pPitDataTable->Add_Record();
			row->Add_Value("ID", i++);
			row->Add_Value("OutRow", pit.outr == -1 ? 0 : Get_NY() - pit.outr);
			row->Add_Value("OutCol", pit.outc == -1 ? 0 : pit.outc + 1);
			row->Add_Value("Number", pit.aantal);
			row->Add_Value("C", pit.c + 1);
			row->Add_Value("R", Get_NY() - pit.r);
			TSG_Point xy = Get_System().Get_Grid_to_World(pit.outr, pit.outc);
			row->Add_Value("OutX", xy.x);
			row->Add_Value("OutY", xy.y);
			row->Add_Value("input", pit.input);
		}
	}

	//clean stuff
	FINISH->Delete();
	PitDat.empty();
	pitnum = 0;

	return true;
}

CCalculate_Uparea::~CCalculate_Uparea()
{


}

void CCalculate_Uparea::CalculateUparea()
{
	int i, j, vlag, nvlag, rivvlag;
	double OPPCOR, AREA;
	int maxorder = 1;
	int a, b;

	nvlag = 1;

	int nrow = Get_NY();
	int ncol = Get_NX();
	double res = Get_Cellsize();

	FINISH = new CSG_Grid(Get_System(), SG_DATATYPE_Bit);

	SG_UI_Process_Set_Text(_TL("Calculating Uparea"));
	//eerst sorteren, dan van hoogste naar laagste pixel gaan.
	DEM->Set_Index();
	SG_UI_Process_Set_Text(_TL("Indexing finished"));
	for (int t = 0; t < ncol*nrow; t++)
	{
		if (t%ncol == 0) PROGRESS(t);
		int i, j;
		bool ret = DEM->Get_Sorted(t, i, j);
		
		if (!ret) continue;

		if (PRC->asInt(i, j) == 0 || DEM->is_NoData(i,j)) {
			Up_Area->Set_Value(i, j, 0);
			continue;
		}

		double ptef = 0;
		if (PRC->asInt(i, j) == 10000) ptef = 0.75;
		OPPCOR = Get_Cellarea() * (1 - ptef);
		AREA = Up_Area->asDouble(i, j) + OPPCOR;

		DistributeTilDirEvent(i, j, &AREA);
		Up_Area->Add_Value(i, j, OPPCOR);
	}

	// zet de waarde van de uparea binnen in de pits 
	for (i = 0; i < ncol; i++) {
		for (j = 0; j < nrow; j++) {
			if (Pit->asInt(i, j) != 0) {
				vlag = Pit->asInt(i, j);
				Up_Area->Add_Value(i, j, PitDat[vlag].input);
			}
		}
	}
}

void CCalculate_Uparea::CalculatePitStuff()
{
	int vlag, i, j, k, l, m, n, W;
	int nvlag = -1;
	int hulp;
	bool check;
	double minimum;

	int nrow = Get_NY();
	int ncol = Get_NX();

	Pit->Assign(0.0);
	SG_UI_Process_Set_Text(_TL("Creating pit index"));
	for (j = nrow - 2; j > 0; j-- && Set_Progress(nrow - j)) // buitenste rand niet meenemen. Van beneden naar boven zoals watem
	{
		for (i = 1; i < ncol - 1; i++)
		{
			if (Pit->asInt(i, j) != 0 || PRC->asInt(i, j) == 0 || DEM->is_NoData(i,j)) {
				continue;
			}
			hulp = 0;

			for (k = -1; k <= 1; k++) {
				for (l = -1; l <= 1; l++) {
					if (!is_InGrid(i + k, j + l))
						continue;

					if (k == 0 && l == 0) {
						continue;
					}
					if (DEM->asDouble(i + k, j + l) < DEM->asDouble(i, j))
						hulp++;
				}
			}
			if (hulp == 0) { // indien er een pit is
				nvlag++; //depressie zoeken
				Pit->Set_Value(i, j, nvlag + 1);

				TPitData p;
				p.aantal = 0;
				p.outr = -1;
				p.outc = -1;
				PitDat.resize(nvlag + 1);
				PitDat[nvlag] = p;
				PitDat[nvlag].c = i;
				PitDat[nvlag].r = j;
				W = 0;
				minimum = 999999999.9;

				do {
					check = false;
					for (k = -W; k <= W; k++) {
						for (l = -W; l <= W; l++) {
							if (labs(k) != W && labs(l) != W) {
								continue;
							}
							if (!is_InGrid(i + k, j + l)) {
								continue;
							}

							if (DEM->asDouble(i + k, j + l) == DEM->asDouble(i, j)) {
								Pit->Set_Value(i + k, j + l, nvlag);
								PitDat[nvlag].aantal++;
								check = true;
							}
						}
					}
					if (check == false)
					{
						// additional code Johan: search near a pit for lower cells with a maximum radius of pit_radius if PIT_FLOW is selected
						if (pit_flow && PitDat[nvlag].outc == -1 && PitDat[nvlag].outr == -1) {
							for (int searchradius = 1; searchradius <= pit_radius; searchradius++)
							{
								double minvalue = DEM->asDouble(i, j);
								for (k = -searchradius; k <= searchradius; k++) {
									for (l = -searchradius; l <= searchradius; l++) {
										if (labs(k) != searchradius && labs(l) != searchradius) {
											continue;
										}
										if (is_InGrid(i + k, j + l) && DEM->asDouble(i + k, j + l) < minvalue)
										{
											minvalue = DEM->asDouble(i + k, j + l);
											PitDat[nvlag].outc = i + k;
											PitDat[nvlag].outr = j + l;
										}
									}
								}
								if (minvalue < DEM->asDouble(i, j))
									break;
							}
						}
						goto _Lnopit;
					}

					for (k = -W; k <= W; k++) {
						for (l = -W; l <= W; l++) {
							if (labs(k) != W && labs(l) != W) {
								continue;

							}
							for (m = -1; m <= 1; m++) {
								for (n = -1; n <= 1; n++) {
									if (m == 0 && n == 0) {
										continue;
									}
									if (!is_InGrid(i + k + m, j + l + n) || !is_InGrid(i + k, j + l)) {
										continue;
									}
									if (DEM->asDouble(i + k + m, j + l + n) < DEM->asDouble(i + k, j + l) &&
										Pit->asInt(i + k + m, j + l + n) != nvlag &&
										Pit->asInt(i + k, j + l) == nvlag) {

										if (DEM->asDouble(i + k + m, j + l + n) < minimum) {
											PitDat[nvlag].outc = i + k + m;
											PitDat[nvlag].outr = j + l + n;
											minimum = DEM->asDouble(i + k + m, j + l + n);
											PitDat[nvlag].c = i + k;
											PitDat[nvlag].r = j + l;
										}
									}
								}
							}
						}
					}
					W++;
				} while (W <= pitwin);

			_Lnopit:;
			}
		}
	}
	if (nvlag != 0) {
		for (i = 1; i <= nvlag; i++) {
			int row = PitDat[i].outr;
			int col = PitDat[i].outc;
			if (row != -1 && col != -1 && Pit->asInt(col, row) != 0) {
				vlag = Pit->asInt(k, l);
				PitDat[i].outr = PitDat[vlag].outr;
				PitDat[i].outc = PitDat[vlag].outc;
			}
		}
	}

	//nagaan of rivier door de pit gaat - dan is outlet pit zelf
	for (int i = 1; i < ncol - 1; i++)
	{
		for (int j = 1; j < nrow - 1; j++)
		{
			if ((Pit->asInt(i, j) > 0) && (PRC->asInt(i, j) == -1))
			{
				vlag = Pit->asInt(i, j);
				PitDat[vlag].outr = j;
				PitDat[vlag].outc = i;
			}
		}
	}

	pitnum = nvlag;
}

void CCalculate_Uparea::DistributeTilDirEvent(int i, int j, double *AREA)
{
	int nrow = Get_NY();
	int ncol = Get_NX();

	double CSN, SN, MINIMUM, MINIMUM2;
	double PART1 = 0.0, PART2 = 0.0;
	int K1 = 0, K2 = 0, L1 = 0, L2 = 0;
	int ROWMIN, COLMIN, ROWMIN2, COLMIN2, K, L;
	bool parequal;
	bool closeriver = false;

	double Direction;
	int vlag, rivvlag, v, w;

	for (K = -1; K <= 1; K++) {
		for (L = 1; L >= -1; L--) {
			if (is_InGrid(i + K, j + L) && PRC->asInt(i + K, j + L) == -1) {
				if (DEM->asDouble(i + K, j + L) < DEM->asDouble(i, j))
					closeriver = true;
				if (wrong)
					//bug: the else clause should be removed because it is a bug - only the topright pixel is analysed if a river is found in other cells the result is overwritten
					closeriver = false;
			}
		}
	}

	if (closeriver) {
		for (K = -1; K <= 1; K++) {
			for (L = -1; L <= 1; L++) {
				if (!is_InGrid(i + K, j + L) || (K == 0 && L == 0)) {
					continue;
				}

				if ((PRC->asInt(i + K, j + L) == -1) & (DEM->asDouble(i + K, j + L) < DEM->asDouble(i, j))) {
					ROWMIN = K;
					COLMIN = L;
					*AREA = 0.0;
					FINISH->Set_Value(i, j, 1);
				}
			}
		}
		return;
	}

	double localslope;
	DEM->Get_Gradient(i, j, localslope, Direction);

	CSN = fabs(cos(Direction)) / (fabs(sin(Direction)) + fabs(cos(Direction)));
	SN = 1 - CSN;
	if (Direction <= M_PI / 2) {
		PART1 = *AREA * SN;
		PART2 = *AREA * CSN;
		K1 = 1;
		L1 = 0;
		K2 = 0;
		L2 = 1;
	}
	else {
		if (Direction > M_PI / 2 && Direction < M_PI) {
			PART1 = *AREA * SN;
			PART2 = *AREA * CSN;
			K1 = 1;
			L1 = 0;
			K2 = 0;
			L2 = -1;
		}
		else {
			if (Direction >= M_PI && Direction <= M_PI * 1.5) {
				PART1 = *AREA * CSN;
				PART2 = *AREA * SN;
				K1 = 0;
				L1 = -1;
				K2 = -1;
				L2 = 0;
			}
			else {
				if (Direction > M_PI * 1.5) {
					PART1 = *AREA * SN;
					PART2 = *AREA * CSN;
					K1 = -1;
					L1 = 0;
					K2 = 0;
					L2 = 1;
				}

			}
		}
	}

	// check if receiving cell is a pit
	if ((is_InGrid(i + K1, j + L1) && (Pit->asInt(i + K1, j + L1) > 0)) ||
		(is_InGrid(i + K2, j + L2) && (Pit->asInt(i + K2, j + L2) > 0))) {
		if (is_InGrid(i + K1, j + L1))
			vlag = Pit->asInt(i + K1, j + L1);
		else vlag = 0;
		if ((vlag == 0) && (is_InGrid(i + K2, j + L2)))
			vlag = Pit->asInt(i + K2, j + L2);

		v = PitDat[vlag].outr;
		w = PitDat[vlag].outc;

		if (is_InGrid(w, v)) {
			if (Pit->asInt(w, v) == -1) {
				PitDat[vlag].input += *AREA;
			}
			else {
				double area_out;
				if (PRC->asInt(w, v) != PRC->asInt(i,j))
				{
					if (PRC->asInt(w,v) == 10000)
						area_out = *AREA * (100 - TFCAtoForestOrPasture) / 100.0;
					else if (PRC->asInt(w, v) > 0)
					{

						area_out = *AREA * (100 - TFCAtoCropLand) / 100.0;
					}
					else if (PRC->asInt(w, v) == -2)
					{
						area_out = *AREA * (100 - TFCAtoRoad) / 100.0;
					}
					else area_out = 0;
				}
				else
				{
					area_out = *AREA;
				}

				Up_Area->Add_Value(w, v, area_out);
				PitDat[vlag].input += *AREA;
			}
		}
		FINISH->Set_Value(i, j, 1);
	}

	else {
		//goed checken wanneer en of deze code eigenlijk gebruikt wordt: kan enkel als dem gelijk denk ik. Of misschien bij raar aspect tov pixelwaarden (zadelachtig)
		if (is_InGrid(i + K1, j + L1) && (FINISH->asInt(i + K1, j + L1) == 1)) {
			if (is_InGrid(i + K2, j + L2) && (FINISH->asInt(i + K2, j + L2) == 1)) {
				PART1 = 0.0;
				PART2 = 0.0;
			}
			else {
				PART2 += PART1;
				PART1 = 0.0;
			}
		}
		else {
			if (is_InGrid(i + K2, j + L2) && (FINISH->asInt(i + K2, j + L2) == 1)) {
				PART1 += PART2;
				PART2 = 0.0;
			}
		}

		if (is_InGrid(i + K1, j + L1) && (DEM->asDouble(i + K1, j + L1) > DEM->asDouble(i, j))) {
			if (is_InGrid(i + K2, j + L2) && (DEM->asDouble(i + K2, j + L2) > DEM->asDouble(i, j))) {
				PART1 = 0.0;
				PART2 = 0.0;
			}
			else {
				if (is_InGrid(i + K2, j + L2) && (PRC->asInt(i + K2, j + L2) != PRC->asInt(i, j))) {
					PART1 = 0.0;
					PART2 = 0.0;
				}
				else {
					PART2 += PART1;
					PART1 = 0.0;
				}
			}
		}
		else {
			if (is_InGrid(i + K2, j + L2) && (DEM->asDouble(i + K2, j + L2) > DEM->asDouble(i, j))) {
				if (is_InGrid(i + K1, j + L1) && (PRC->asInt(i + K1, j + L1) != PRC->asInt(i, j))) {
					PART2 = 0.0;
					PART1 = 0.0;
				}
				else {
					PART1 += PART2;
					PART2 = 0.0;
				}
			}
			else {
				if (is_InGrid(i + K1, j + L1) && (PRC->asInt(i + K1, j + L1) != PRC->asInt(i, j))) {
					if (is_InGrid(i + K2, j + L2) && (PRC->asInt(i + K2, j + L2) != PRC->asInt(i, j))) {
						PART1 = 0.0;
						PART2 = 0.0;
					}
					else {
						PART2 += PART1;
						PART1 = 0.0;
					}
				}
				else {
					if (is_InGrid(i + K2, j + L2) && (PRC->asInt(i + K2, j + L2) != PRC->asInt(i, j))) {
						PART1 += PART2;
						PART2 = 0.0;
					}
				}
			}
		}

		if (PART1 == 0.0 && PART2 == 0.0) {
			// no receiving cells were found (lagergelegen of ander perceel)         
			parequal = false;
			ROWMIN = 0;
			ROWMIN2 = 0;
			COLMIN = 0;
			COLMIN2 = 0;
			MINIMUM = 99999999.9;
			MINIMUM2 = 99999999.9;
			for (K = -1; K <= 1; K++) {
				for (L = -1; L <= 1; L++) {
					if (K == 0 && L == 0) {
						continue;
					}
					if (!is_InGrid(i + K, j + L))
						continue;
					if ((((DEM->asDouble(i + K, j + L) < MINIMUM) && (DEM->asDouble(i + K, j + L) < DEM->asDouble(i,
						j))) && FINISH->asInt(i + K, j + L) == 0) && (PRC->asInt(i + K, j + L) == PRC->asInt(i, j))) {

						MINIMUM = DEM->asDouble(i + K, j + L);
						ROWMIN = K;
						COLMIN = L;
						parequal = true;
					}
					if (((DEM->asDouble(i + K, j + L) < MINIMUM2) & (DEM->asDouble(i + K, j + L) <
						DEM->asDouble(i, j))) &&
						FINISH->asInt(i + K, j + L) == 0) {
						MINIMUM2 = DEM->asDouble(i + K, j + L);
						ROWMIN2 = K;
						COLMIN2 = L;
					}
				}
			}
			if (parequal) {
				if (Pit->asInt(i + ROWMIN, j + COLMIN) != 0) {
					vlag = Pit->asInt(i + ROWMIN, j + COLMIN);
					int row = PitDat[vlag].outr;
					int col = PitDat[vlag].outc;
					if (is_InGrid(col, row)) {
						if (PRC->asInt(col, row) == -1) {
							*AREA = 0.0;
						}
						else {
							Up_Area->Add_Value(PitDat[vlag].outc, PitDat[vlag].outr, *AREA);
							PitDat[vlag].input += *AREA;
						}
					}

					FINISH->Set_Value(i, j, 1);
				}
				else {
					Up_Area->Add_Value(i + ROWMIN, j + COLMIN, *AREA);
				}
			}
			else { //receiving cell belongs to a different parcel
				if (!wrong)
				{
					if (PRC->asInt(i + ROWMIN2, j + COLMIN2) == 10000)
						Abis = *AREA * (100 - TFCAtoForestOrPasture) / 100.0;
					else if (PRC->asInt(i + ROWMIN2, j + COLMIN2) > 0)
					{

						Abis = *AREA * (100 - TFCAtoCropLand) / 100.0;
					}
					else if (PRC->asInt(i + ROWMIN2, j + COLMIN2) == -2)
					{
						Abis = *AREA * (100 - TFCAtoRoad) / 100.0;
					}
					else Abis = 0;
				}
				else //use wrong calulation
				{
					/* WRONG calculation - but the same as the original version of WATEM
					if grid cell is -1 or - 2 then there is no result. The previous value of abis is used. */
					if (PRC->asInt(i + ROWMIN2, j + COLMIN2) == 10000)
						Abis = *AREA * (100 - TFCAtoForestOrPasture) / 100.0;
					else if (PRC->asInt(i + ROWMIN2, j + COLMIN2) > 0)
					{
						Abis = *AREA * (100 - TFCAtoCropLand) / 100.0;
					}
				}

				if (Pit->asInt(i + ROWMIN2, j + COLMIN2) != 0) {
					vlag = Pit->asInt(i + ROWMIN2, j + COLMIN2);
					int row = PitDat[vlag].outr;
					int col = PitDat[vlag].outc;

					if (is_InGrid(col, row) && PRC->asInt(col, row) == -1) {
						*AREA = 0.0;
					}
					else {
						if (is_InGrid(col, row))
							Up_Area->Add_Value(col, row, Abis);
							PitDat[vlag].input += *AREA;
					}
					FINISH->Set_Value(i, j, 1);
				}
				else {
					Up_Area->Add_Value(i + ROWMIN2, j + COLMIN2, Abis);
				}
			}
		}

		else { //normal case: part1 and part 2 !=0

			if (is_InGrid(i + K1, j + L1))
				Up_Area->Add_Value(i + K1, j + L1, PART1);
			if (is_InGrid(i + K2, j + L2))
				Up_Area->Add_Value(i + K2, j + L2, PART2);
		}
	}
	FINISH->Set_Value(i, j, 1.0);
}
