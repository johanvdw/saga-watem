#include <vector>
#include "Pitstuff.h"


//TPitDataArray = std::List(TPitData);

//voorlopîg even wat variabelen hier zetten die we in klasse moeten hebben:




const int pitwin = 200; //grootte pitwindow



CPitstuff::CPitstuff()
{
	Set_Name(_TL("03: Pit berekening"));

	Set_Author(_TL("Copyrights (c) 2016 by Johan Van de Wauw"));

	Set_Description(_TW(
		"")
	);


	//-----------------------------------------------------
	// Define your parameters list...


	Parameters.Add_Grid(
		NULL, "DEM", "Elevation",
		"",
		PARAMETER_INPUT
	);


	Parameters.Add_Grid(
		NULL, "PRC", _TL("Percelen"),
		"",
		PARAMETER_INPUT
	);


	Parameters.Add_Grid(
		NULL, "PIT", _TL("Pit"),
		_TL("This will contain LS. "),
		PARAMETER_OUTPUT,
		true,
		SG_DATATYPE_Int
	);

}

bool CPitstuff::On_Execute(void)
{
	//-----------------------------------------------------
	// Get parameter settings...

	m_pDEM = Parameters("DEM")->asGrid();
	m_pPRC = Parameters("PRC")->asGrid();
	m_pPit = Parameters("PIT")->asGrid();
 

		  //-----------------------------------------------------
		  // Check for valid parameter settings...


		  //-----------------------------------------------------
		  // Execute calculation...
	CalculatePitStuff();
	return true;
}

void CPitstuff::CalculatePitStuff()
{

	long vlag, i, j, k, l, m, n, W;
	long nvlag = 0;
	long hulp;
	bool check;
	double minimum;

	int nrow = Get_NY();
	int ncol = Get_NX();

	// dit kan zéker efficienter - misschien zelfs weggelaten worden
	for (i = 1; i <= nrow; i++) {
		for (j = 1; j <= ncol; j++) {
			m_pPit->Set_Value(i, j, 0);
		}
	}

	//vraag: is dit paralleliseerbaar?
	for (i = 2; i < nrow; i++) {
		for (j = 2; j < ncol; j++) {
			if (m_pPit->asInt(i,j) != 0 || m_pPRC->asInt(i, j) == 0) {
				continue;
			}
			hulp = 0;

			for (k = -1; k <= 1; k++) {
				for (l = -1; l <= 1; l++) {
					if (k == 0 && l == 0) {
						continue;
					}
					if  (m_pDEM->asDouble(i+k,j+l) < m_pDEM->asDouble(i,j))
						hulp++;
				}
			}
			if (hulp == 0) {
				nvlag++;
				m_pPit->Set_Value(i, j, nvlag);
				
				//setlength(PitDat, nvlag + 1);
				TPitData p = {};
				p.aantal = 0;
				
//				PitDat[nvlag].aantal = 0;
				PitDat.resize(nvlag+1);
				PitDat[nvlag] = p;
				W = 0;
				minimum = 999999999.9;

				do {
					check = false;
					for (k = -W; k <= W; k++) {
						for (l = -W; l <= W; l++) {
							if (labs(k) != W && labs(l) != W) {
								continue;
							}
							if ((unsigned long)(i + k) > nrow ||
								(unsigned long)(j + l) > ncol) {
								continue;
							}




							if (m_pDEM->asDouble(i+k,j+l) == m_pDEM->asDouble(i,j)) {

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
									if (i + k + m < 1 || i + k + m > nrow || j + l + n < 1 ||
										j + l + n > ncol) {

										continue;

									}
									if (m_pDEM->asDouble(i + k + m, j + l + n) < m_pDEM->asDouble(i + k, j + l) &&
										m_pPit->asInt(i + k + m, j + l + n) != nvlag &&
										m_pPit->asInt(i + k, j + l) == nvlag) {

										if (m_pDEM->asDouble(i + k + m,j + l + n) < minimum) {
											PitDat[nvlag].outr = i + k + m;
											PitDat[nvlag].outc = j + l + n;
											minimum = m_pDEM->asDouble(i + k + m, j + l + n);
											PitDat[nvlag].r = i + k;
											PitDat[nvlag].c = j + l;
										
										}
									}
								}
							}
						}
					}
					W++;
					/* p2c: unitSurface.pas, line 231:
					* Warning: Symbol 'PITWIN' is not defined [221] */
				} while (W <= pitwin);

			_Lnopit:;
			}
		}
	}
	if (nvlag != 0) {
		for (i = 1; i <= nvlag; i++) {
			k = PitDat[i].outr;
			l = PitDat[i].outc;
			if (m_pPit->asInt(k,l) != 0) {
				vlag = m_pPit->asInt(k, l);
				PitDat[i].outr = PitDat[vlag].outr;
				PitDat[i].outc = PitDat[vlag].outc;
			}
		}



		for (i = 2; i < nrow; i++) {

			for (j = 2; j < ncol; j++) {
				if (m_pPit->asInt(i,j) != 0 && m_pPRC->asInt(i,j) == -1) {
					vlag = m_pPit->asInt(i,j);
					PitDat[vlag].outr = i;
					PitDat[vlag].outc = j;
				}
			}
		}

	}

	// eventueel de tabel met pitdata nog genereren om te controleren of dit identiek is.
	// kan door een niet verplichte tabel
	
	/*printf("%12ld\n", inttostr(nvlag));
	if (nvlag != 0) {
		for (i = 1; i <= nvlag; i++)
			printf("%12ld\t%12ld\t%12ld\t%12ld\n",
				inttostr(i), inttostr(PitDat[i].outr), inttostr(PitDat[i].outc),
				inttostr(PitDat[i].aantal));
	}


	*/
	pitnum = nvlag;
}





CPitstuff::~CPitstuff()
{
}
