#include "Calculate_Uparea.h"

CCalculate_Uparea::CCalculate_Uparea()
{
	//-----------------------------------------------------
	// Give some information about your tool...

	Set_Name(_TL("02: Uparea berekening"));

	Set_Author(_TL("Copyrights (c) 2016 by KULeuven. Converted to SAGA/C++ by Johan Van de Wauw"));

	Set_Description(_TW(
		"(c) 2016")
	);


	//-----------------------------------------------------
	// Define your parameters list...


	Parameters.Add_Grid(
		NULL, "DEM", "Elevation",
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "PRC", "Percelen",
		"",
		PARAMETER_INPUT
	);

	/*Parameters.Add_Grid(
		NULL, "ASPECT", "Aspect",
		"",
		PARAMETER_INPUT_OPTIONAL
	);*/

	Parameters.Add_Grid(
		NULL, "PIT", "Pit",
		"",
		PARAMETER_OUTPUT
	);


	Parameters.Add_Grid(
		NULL, "UPSLOPE_AREA", _TL("Upslope Length Factor"),
		"This contains the output Upslope Area",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table(
		NULL, "PITDATA", _TL("Pit data"),
		"",
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL, "PCTOCROP", "Parcel Connectivity to cropland",
		"", PARAMETER_TYPE_Double, 70, 0, 100
	);

}

bool CCalculate_Uparea::On_Execute(void)
{
	int x, y;
	double opp;
	//-----------------------------------------------------
	// Get parameter settings...

	m_pDEM = Parameters("DEM")->asGrid();
	m_pPit = Parameters("PIT")->asGrid();
	m_pPRC = Parameters("PRC")->asGrid();
	m_pUp_Area = Parameters("UPSLOPE_AREA")->asGrid();
	pPitDataTable = Parameters("PITDATA")->asTable();

	TFCAtoCropLand = Parameters("PCTOCROP")->asDouble();


	//-----------------------------------------------------
	// Check for valid parameter settings...


	//-----------------------------------------------------
	// Execute calculation...

	// make sure m_pUp_Area is empty (as we only add) -- TODO check if this is really necessary
	for (int i = 0; i < Get_NX(); i++)
#pragma omp parallel for
		for (int j = 0; j < Get_NY(); j++)
			m_pUp_Area->Set_Value(i, j, 0);
	//m_pAspect = Parameters("ASPECT")->asGrid();
	//pitwin = Parameters("PITWIN")->asInt();
	pitwin = 200;
	pitnum = 0;
	PitDat.empty();

	CalculatePitStuff();
	CalculateUparea();

	//export pitdata table if this is defined
	if (pPitDataTable != NULL) {
		pPitDataTable->Del_Records();
		if (pPitDataTable->Get_Field("ID")==-1)
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
		int i = 0;
		for each (TPitData pit in PitDat)
		{
			CSG_Table_Record * row = pPitDataTable->Add_Record();
			row->Add_Value("ID", i++);
			row->Add_Value("OutRow", pit.outr);
			row->Add_Value("OutCol", pit.outc);
			row->Add_Value("Number", pit.aantal);
			row->Add_Value("C", pit.c+1);
			row->Add_Value("R", Get_NY() - pit.r);
		}
	}

	//clean stuff
	m_pFINISH->Delete();
	PitDat.empty();
	pitnum = 0;

	return true;
}

CCalculate_Uparea::~CCalculate_Uparea()
{


}



void CCalculate_Uparea::CalculateUparea()

{
	long i, j, vlag, nvlag, rivvlag;
	double OPPCOR, AREA, massbalance;
	long Nextsegmentid;
	std::vector<long> rivsegmentlatinputcheck;
	std::vector<long> rivsegmentupcheck;
	long maxorder = 1;
	long a, b, c, e, f;
	double d;


	nvlag = 1;

	rivsegmentlatinputcheck.resize(nvlag);
	rivsegmentupcheck.resize(nvlag);
	RivDat.resize(nvlag);

	int nrow = Get_NY();
	int ncol = Get_NX();
	double res = Get_Cellsize();

	m_pFINISH = new CSG_Grid(*Get_System(), SG_DATATYPE_Int);


	//eerst sorteren, dan van hoogste naar laagste pixel gaan.
	for (int t = 0; t < ncol*nrow; t++)
	{
		int i, j;
		m_pDEM->Get_Sorted(t, i, j);
		if (m_pPRC->asInt(i, j) == 0) {
			continue;
		}

		double ptef = 0; 
		if (m_pPRC->asInt(i, j) == 10000) ptef = 0.75;
		OPPCOR = Get_Cellarea() * (1-ptef);
		AREA = m_pUp_Area->asDouble(i, j) + OPPCOR;

		DistributeTilDirEvent(i, j, &AREA, &massbalance);
		m_pUp_Area->Add_Value(i, j, OPPCOR);

		if (m_pPRC->asInt(i, j) == -1) {
			rivvlag = 0;
			RivDat[rivvlag].outuparea = RivDat[rivvlag].latuparea;
			RivDat[rivvlag].check = 1;

		}
	}



	for (i = 0; i < nvlag; i++) {
		rivsegmentlatinputcheck[i] = 0;
		/* p2c: unitSurfacetildir.pas, line 656:
		* Warning: Index on a non-array variable [287] */
		rivsegmentupcheck[i] = 0;
		/* p2c: unitSurfacetildir.pas, line 657:
		* Warning: Index on a non-array variable [287] */
	}

	for (i = 0; i < pitnum; i++) {
		a = PitDat[i].outr;
		b = PitDat[i].outc;
		if (m_pPRC->asInt(a, b) == -1) {

			vlag = 0;
			RivDat[vlag].latuparea += PitDat[i].input;
			RivDat[vlag].outuparea = RivDat[vlag].latuparea;
		}
	}




	for (j = 1; j <= maxorder; j++) {
		for (i = 0; i < nvlag; i++) {
			if (RivDat[i].check == 1) {
				if (RivDat[i].order == j) {
					Nextsegmentid = RivDat[i].segmentdown;
					/* p2c: unitSurfacetildir.pas, line 680:
					* Warning: Index on a non-array variable [287] */
					/* p2c: unitSurfacetildir.pas, line 680:
					* Warning: No field called SEGMENTDOWN in that record [288] */
					if (Nextsegmentid == -2) {
						continue;
					}
					else {
						RivDat[Nextsegmentid].inuparea += RivDat[i].outuparea;
						RivDat[Nextsegmentid].outuparea += RivDat[i].outuparea;

					}
				}
			}
		}
	}

	/*     Temporary disabled
	assignfile(output,extractFilepath(DTMFilename)+'\tmp.txt') ;
	reset(output);
	rewrite(output);
	writeln(output,inttostr(nvlag-1));
	for i:=1 to nvlag-1 do
	begin
	writeln(inttostr(Rivdat[i].segmentid),chr(9),inttostr(Rivdat[i].fnode),chr(9),
	inttostr(Rivdat[i].tnode),chr(9),floattostr(Rivdat[i].length),chr(9),inttostr(Rivdat[i].order),chr(9),inttostr(Rivdat[i].segmentdown),char(9),
	floattostr(Rivdat[i].latuparea),chr(9),floattostr(Rivdat[i].inuparea),chr(9),floattostr(Rivdat[i].outuparea),chr(9));
	end;
	closefile(output);*/
	for (i = 0; i < ncol; i++) {
		for (j = 0; j < nrow; j++) {
			if (m_pPit->asInt(i, j) != 0) {
				vlag = m_pPit->asInt(i, j);
				m_pUp_Area->Add_Value(i, j, PitDat[vlag].input);
			}
		}
	}
	for (i = 1; i <= ncol; i++) {
		for (j = 1; j < nrow; j++) {
			if (m_pPRC->asInt(i, j) == -1) {
				rivvlag = 0;
				m_pUp_Area->Add_Value(i, j, RivDat[rivvlag].outuparea);
			}
		}
	}


}

void CCalculate_Uparea::CalculatePitStuff()
{

	long vlag, i, j, k, l, m, n, W;
	long nvlag = -1;
	long hulp;
	bool check;
	double minimum;

	int nrow = Get_NY();
	int ncol = Get_NX();

	// dit kan zéker efficienter - misschien zelfs weggelaten worden
	for (j = 0; j < nrow; j++)
	{
#pragma omp parallel for
		for (i = 0; i < ncol; i++)
		 {
			m_pPit->Set_Value(i, j, 0);
		}
	}

	//vraag: is dit paralleliseerbaar?
	for (j = nrow -2; j >0; j--)
	{
		for (i = 1; i < ncol-1; i++)
		 {
			if (m_pPit->asInt(i, j) != 0 || m_pPRC->asInt(i, j) == 0) {
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
					if (m_pDEM->asDouble(i + k, j + l) < m_pDEM->asDouble(i, j))
						hulp++;
				}
			}
			if (hulp == 0) { // indien er een pit is
				nvlag++; //depressie zoeken
				m_pPit->Set_Value(i, j, nvlag+1);

				//setlength(PitDat, nvlag + 1);
				TPitData p = {};
				p.aantal = 0;
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


							if (m_pDEM->asDouble(i + k, j + l) == m_pDEM->asDouble(i, j)) {

								m_pPit->Set_Value(i + k, j + l, nvlag);

								PitDat[nvlag].aantal++;

								check = true;
							}

						}
					}
					if (check == false)
						goto _Lnopit;
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
									if  (!is_InGrid(i+k+m,j+l+n) || !is_InGrid(i + k, j + l)) {
										continue;
									}
									if (m_pDEM->asDouble(i + k + m, j + l + n) < m_pDEM->asDouble(i + k, j + l) &&
										m_pPit->asInt(i + k + m, j + l + n) != nvlag &&
										m_pPit->asInt(i + k, j + l) == nvlag) {

										if (m_pDEM->asDouble(i + k + m, j + l + n) < minimum) {
											PitDat[nvlag].outc = i + k + m;
											PitDat[nvlag].outr = j + l + n;
											minimum = m_pDEM->asDouble(i + k + m, j + l + n);
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
			k = PitDat[i].outr;
			l = PitDat[i].outc;
			if (m_pPit->asInt(k, l) != 0) {
				vlag = m_pPit->asInt(k, l);
				PitDat[i].outr = PitDat[vlag].outr;
				PitDat[i].outc = PitDat[vlag].outc;
			}
		}



		for (i = 1; i < ncol; i++) {

			for (j = 1; j < nrow; j++) {
				if (m_pPit->asInt(i, j) != 0 && m_pPRC->asInt(i, j) == -1) {
					vlag = m_pPit->asInt(i, j);
					PitDat[vlag].outr = j;
					PitDat[vlag].outc = i;
				}
			}
		}

	}



	//nagaan of rivier door de pit gaat
	for (int i = 1; i < ncol - 1; i++)
	{
		for (int j = 1; j < nrow - 1; j++)
		{
			if ((m_pPit->asDouble(i, j) != 0) && (m_pPRC->asDouble(i, j) == -1))
			{
				vlag = m_pPit->asDouble(i, j);
				PitDat[vlag].outr = j;
				PitDat[vlag].outc = i;

			}

		}

	}
	
	pitnum = nvlag;
}




void CCalculate_Uparea::DistributeTilDirEvent(long i, long j, double *AREA, double * massbalance)

{
	int nrow = Get_NY();
	int ncol = Get_NX();

	double CSN, SN, MINIMUM, MINIMUM2;
	double PART1 = 0.0, PART2 = 0.0;
	int K1 = 0, K2 = 0, L1 = 0, L2 = 0;
	int ROWMIN, COLMIN, ROWMIN2, COLMIN2, K, L;
	bool parequal;
	bool closeriver = false;
	double  Abis = 0;
	double Direction;
	int vlag, rivvlag, v, w;

	for (K = -1; K <= 1; K++) {
		for (L = -1; L <= 1; L++) {
			if (is_InGrid(i + K, j + L) && m_pPRC->asInt(i + K, j + L) == -1) {
				if (m_pDEM->asDouble(i + K, j + L) < m_pDEM->asDouble(i, j))
					closeriver = true;
				else
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

				if ((m_pPRC->asInt(i + K, j + L) == -1) & (m_pDEM->asDouble(i + K, j + L) < m_pDEM->asDouble(i, j))) {
					ROWMIN = K;
					COLMIN = L;
					rivvlag = 0;
					RivDat[rivvlag].latuparea += *AREA;

					*AREA = 0.0;
					m_pFINISH->Set_Value(i, j, 1);

				}

			}
		}

		return;
	}

	// nieuwe code, verbruikt minder geheugen, volgens mij beter - maakt geen gebruik van vooraf berekend aspect
	double localslope;
	m_pDEM->Get_Gradient(i, j, localslope, Direction);
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



	if ((is_InGrid(i + K1, j + L1) && (m_pPit->asInt(i + K1, j + L1) != 0)) ||
		(is_InGrid(i + K2, j + L2) && (m_pPit->asInt(i + K2, j + L2) != 0))) {
		if (is_InGrid(i + K1, j + L1))
			vlag = m_pPit->asInt(i + K1, j + L1);
		else vlag = 0;
		if ((vlag == 0) && (is_InGrid(i + K2, j + L2)))
			vlag = m_pPit->asInt(i + K2, j + L2);

		v = PitDat[vlag].outr;
		/* p2c: unitSurfacetildir.pas, line 264:
		* Warning: Index on a non-array variable [287] */
		/* p2c: unitSurfacetildir.pas, line 264:
		* Warning: No field called OUTR in that record [288] */
		w = PitDat[vlag].outc;
		/* p2c: unitSurfacetildir.pas, line 265:
		* Warning: Index on a non-array variable [287] */
		/* p2c: unitSurfacetildir.pas, line 265:
		* Warning: No field called OUTC in that record [288] */
		if (m_pPit->asInt(v, w) == -1) {
			rivvlag = 1;
			RivDat[rivvlag].latinput += *AREA;
			PitDat[vlag].input += *AREA;

		}
		else {
			m_pUp_Area->Set_Value(PitDat[vlag].outr, PitDat[vlag].outc, *AREA);

			PitDat[vlag].input += *AREA;

		}
		m_pFINISH->Set_Value(i, j, 1);
	}

	else {

		if (is_InGrid(i + K1, j + L1) && (m_pFINISH->asInt(i + K1, j + L1) == 1)) {
			if (is_InGrid(i + K2, j + L2)&& (m_pFINISH->asInt(i + K2, j + L2) == 1)) {

				PART1 = 0.0;

				PART2 = 0.0;
			}
			else {
				PART2 += PART1;

				PART1 = 0.0;
			}
		}


		else {
			if (is_InGrid(i + K2, j + L2) && (m_pFINISH->asInt(i + K2, j + L2) == 1)) {
				/* p2c: unitSurfacetildir.pas, line 305:
				* Warning: Index on a non-array variable [287] */
				/* p2c: unitSurfacetildir.pas, line 305:
				* Warning: Index on a non-array variable [287] */
				PART1 += PART2;

				PART2 = 0.0;
			}
		}



		if (is_InGrid(i + K1, j + L1) && (m_pDEM->asDouble(i + K1, j + L1) > m_pDEM->asDouble(i, j))) {
			if (is_InGrid(i + K2, j + L2) && (m_pDEM->asDouble(i + K2, j + L2) > m_pDEM->asDouble(i, j))) {
				PART1 = 0.0;
				PART2 = 0.0;
			}
			else {
				if (is_InGrid(i + K2, j + L2) && (m_pPRC->asInt(i + K2, j + L2) != m_pPRC->asInt(i, j))) {
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

			if (is_InGrid(i + K2, j + L2) && (m_pDEM->asDouble(i + K2, j + L2) > m_pDEM->asDouble(i, j))) {
				if (is_InGrid(i + K1, j + L1) && (m_pPRC->asInt(i + K1, j + L1) != m_pPRC->asInt(i, j))) {
					PART2 = 0.0;
					PART1 = 0.0;
				}
				else {
					PART1 += PART2;
					PART2 = 0.0;
				}
			}
			else {
				if (is_InGrid(i + K1, j + L1) && (m_pPRC->asInt(i + K1, j + L1) != m_pPRC->asInt(i, j))) {
					if (is_InGrid(i + K2, j + L2) && (m_pPRC->asInt(i + K2, j + L2) != m_pPRC->asInt(i, j))) {
						PART1 = 0.0;
						PART2 = 0.0;
					}
					else {
						PART2 += PART1;
						PART1 = 0.0;
					}
				}
				else {
					if (is_InGrid(i + K2, j + L2) && (m_pPRC->asInt(i + K2, j + L2) != m_pPRC->asInt(i, j))) {
						PART1 += PART2;
						PART2 = 0.0;
					}
				}
			}
		}


		if (PART1 == 0.0 && PART2 == 0.0) {
			/* p2c: unitSurfacetildir.pas:
			* Note: Eliminated unused assignment statement [338] */
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
					if ((((m_pDEM->asDouble(i + K, j + L) < MINIMUM) && (m_pDEM->asDouble(i + K, j + L) < m_pDEM->asDouble(i,
						j))) && m_pFINISH->asInt(i + K, j + L) == 0) && (m_pPRC->asInt(i + K, j + L) == m_pPRC->asInt(i, j))) {

						MINIMUM = m_pDEM->asDouble(i + K, j + L);
						ROWMIN = K;
						COLMIN = L;
						parequal = true;
					}

					if (((m_pDEM->asDouble(i + K, j + L) < MINIMUM2) & (m_pDEM->asDouble(i + K, j + L) <
						m_pDEM->asDouble(i, j))) &&
						m_pFINISH->asInt(i + K, j + L) == 0) {
						/* p2c: unitSurfacetildir.pas, line 412:
						* Warning: Index on a non-array variable [287] */
						/* p2c: unitSurfacetildir.pas, line 412:
						* Warning: Index on a non-array variable [287] */
						MINIMUM2 = m_pDEM->asDouble(i + K, j + L);
						ROWMIN2 = K;
						COLMIN2 = L;
					}
				}
			}
			if (parequal) {

				if (m_pPit->asInt(i + ROWMIN, j + COLMIN) != 0) {

					vlag = m_pPit->asInt(i + ROWMIN, j + COLMIN);
					v = PitDat[vlag].outr;
					w = PitDat[vlag].outc;
					if (m_pPRC->asInt(v, w) == -1) {
						rivvlag = 0;
						RivDat[rivvlag].latinput += *AREA;
						/* p2c: unitSurfacetildir.pas, line 445:
						* Warning: Index on a non-array variable [287] */
						/* p2c: unitSurfacetildir.pas, line 445:
						* Warning: No field called LATINPUT in that record [288] */
						/* p2c: unitSurfacetildir.pas, line 445:
						* Warning: Index on a non-array variable [287] */
						/* p2c: unitSurfacetildir.pas, line 445:
						* Warning: No field called LATINPUT in that record [288] */

						*AREA = 0.0;
					}
					else {

						m_pUp_Area->Add_Value(PitDat[vlag].outr, PitDat[vlag].outc, *AREA);

						PitDat[vlag].input += *AREA;

					}
					m_pFINISH->Set_Value(i, j, 1);

				}


				else {

					m_pUp_Area->Add_Value(i + ROWMIN, j + COLMIN, *AREA);//line 462
					//(*UPAREA)[i + ROWMIN][j + COLMIN] += *AREA;
					/* p2c: unitSurfacetildir.pas, line 462:
					* Warning: Index on a non-array variable [287] */

				}
			}
			else {
				if (m_pPit->asInt(i + ROWMIN2, j + COLMIN2) != 0) {
					vlag = m_pPit->asInt(i + ROWMIN2, j + COLMIN2);
					v = PitDat[vlag].outr;
					w = PitDat[vlag].outc;
					if (m_pPRC->asInt(v, w) == -1) {
						rivvlag = 0;
						RivDat[rivvlag].latinput += *AREA;
						*AREA = 0.0;
					}
					else {

						m_pUp_Area->Add_Value(PitDat[vlag].outr, PitDat[vlag].outc, *AREA);

						PitDat[vlag].input += *AREA;

					}
					m_pFINISH->Set_Value(i, j, 1);


				}
				else {
					if (m_pPRC->asInt(i + ROWMIN2, j + COLMIN2) > 0) {
						if (m_pPRC->asInt(i + ROWMIN2, j + COLMIN2) < 10000) {

							Abis = *AREA * (100 - TFCAtoCropLand) / 100.0;

						}
						else {
							Abis = *AREA * (100 - TFCAtoForestOrPasture) / 100.0;

						}
					}
					m_pUp_Area->Add_Value(i + ROWMIN2, j + COLMIN2, Abis);

					if (m_pPRC->asInt(i + ROWMIN2, j + COLMIN2) == 9999) {

						*massbalance += *AREA;
					}
					if ((i + ROWMIN2 < 1 || i + ROWMIN2 > Get_NY() || j + COLMIN2 < 1 ||
						j + COLMIN2 > Get_NX()) | (m_pPRC->asInt(i + ROWMIN2, j + COLMIN2) == 0)) {
						*massbalance += *AREA;

					}
				}
			}
		}


		else {


			if (!is_InGrid(i + K1, j + L1) ||
				(m_pPRC->asInt(i + K1, j + L1) == 0) | (m_pPRC->asInt(i + K1, j + L1) == 9999)) {

				*massbalance += PART1;
			}
			if (!is_InGrid(i + K2, j + L2) || (m_pPRC->asInt(i + K2, j + L2) == 9999)) {

				*massbalance += PART2;
			}
			if (is_InGrid(i + K1, j + L1))
				m_pUp_Area->Add_Value(i + K1, j + L1, PART1);
			if (is_InGrid(i + K2, j + L2))
				m_pUp_Area->Add_Value(i + K2, j + L2, PART2);
		}
	}

m_pFINISH->Set_Value(i, j, 1.0);

}