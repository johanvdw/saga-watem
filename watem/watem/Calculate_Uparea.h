#include "MLB_Interface.h"
#include <vector>

typedef struct {
	int outr, outc, aantal, r, c = 0;//outr: outrow, outc:outcolumn
	double input = 0;

}  TPitData;


class CCalculate_Uparea :
	public CSG_Tool_Grid
{
public:
	
	CCalculate_Uparea();
	virtual ~CCalculate_Uparea();

	bool On_Execute();

private:
	CSG_Grid *DEM, *Up_Area, *PRC, *Pit, *FINISH, *m_pAspect;

	void CalculateUparea();
	void CalculatePitStuff();
	void DistributeTilDirEvent (int i, int j, double *AREA);
	std::vector<TPitData> PitDat;
	int pitnum = 0;
	int pitwin = 200;
	double TFCAtoCropLand;
	double TFCAtoForestOrPasture = 100;
	double TFCAtoRoad = 100;
	double Abis; 
	bool wrong = false;

	bool pit_flow = true;
	int pit_radius = 4;

	CSG_Table *pPitDataTable;

};

