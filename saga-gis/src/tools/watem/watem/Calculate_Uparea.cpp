#include "Calculate_Uparea.h"

CCalculate_Uparea::CCalculate_Uparea()
{
	//-----------------------------------------------------
	// Give some information about your tool...

	Set_Name(_TL("02: Uparea berekening"));

	Set_Author(_TL("Copyrights (c) 2016 by Johan Van de Wauw"));

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
		NULL, "PIT", "Pit",
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "UPSLOPE_AREA", _TL("Upslope Length Factor"),
		"This contains the output Upslope Area",
		PARAMETER_OUTPUT
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
	m_pUp_Area = Parameters("UPSLOPE_AREA")->asGrid();
	CalculateUparea();

	//-----------------------------------------------------
	// Check for valid parameter settings...


	//-----------------------------------------------------
	// Execute calculation...


	return true;
}

CCalculate_Uparea::~CCalculate_Uparea()
{
	

}



void CCalculate_Uparea::CalculateUparea()

{
	long i, j, teller, vlag, nvlag, rivvlag;
	double OPPCOR, AREA, massbalance;
	long Nextsegmentid;
	std::vector<long> rivsegmentlatinputcheck;
	std::vector<long> rivsegmentupcheck;
	long maxorder = 0;
	long a, b, c, e, f;
	double d;
	

	/* gedeelte rivdata verwijderd*/

	int nrow = Get_NY();
	int ncol = Get_NX();
	double res = Get_Cellsize();

	for (i = 0; i < nrow; i++)
		for (j = 0; j < ncol; j++)
		{
			if (m_pPRC->asInt(i, j) == 0) {
				continue;
			}

			//OPPCOR = Get_Cellarea()* (1 - PTEF[i][j] / 100.0); -- voorlopig geen PTEF
			OPPCOR = Get_Cellarea();

			AREA = m_pUp_Area->asDouble(i, j) + OPPCOR;

			// Dit is de volgende op onder handen te nemen.
			/*DistributeTilDirEvent(i, j, &AREA, UPAREA, &FINISH, &massbalance, true,
				&Rivdat);
				*/
			m_pUp_Area->Set_Value(i, j, OPPCOR);

			/*if (m_pPRC->asInt(i, j) == -1) {
				rivvlag = 1;
				Rivdat[rivvlag].outuparea = Rivdat[rivvlag].latuparea;
				Rivdat[rivvlag].check = 1;

			}*/
		}

	//(tijdelijk om hier iets in nvlag te hebben: )
	nvlag = 128;

	for (i = 0; i < nvlag; i++) {
		rivsegmentlatinputcheck[i] = 0;
		/* p2c: unitSurfacetildir.pas, line 656:
		* Warning: Index on a non-array variable [287] */
		rivsegmentupcheck[i] = 0;
		/* p2c: unitSurfacetildir.pas, line 657:
		* Warning: Index on a non-array variable [287] */
	}

	for (i = 1; i <= pitnum; i++) {
		a = PitDat[i].outr;
		b = PitDat[i].outc;
		if (m_pPRC->asInt(a,b)== -1) {

			vlag = 1;
			RivDat[vlag].latuparea += PitDat[i].input;

			RivDat[vlag].outuparea = RivDat[vlag].latuparea;
		}
	}


	for (j = 1; j <= maxorder; j++) {
		for (i = 1; i < nvlag; i++) {
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
	for (i = 1; i <= nrow; i++) {
		for (j = 1; j <= ncol; j++) {
			if (m_pPit->asInt(i, j) != 0) {
				vlag = m_pPit->asInt(i, j);
				m_pUp_Area->Set_Value(i, j, PitDat[vlag].input);
			}
		}
	}
	for (i = 1; i <= nrow; i++) {
		#pragma omp parallel for
		for (j = 1; j <= ncol; j++) {
			if (m_pPRC->asInt(i, j) == -1) {
				rivvlag = 1;
				m_pUp_Area->Set_Value(i, j, RivDat[rivvlag].outuparea);
			}
		}
	}
	

}
