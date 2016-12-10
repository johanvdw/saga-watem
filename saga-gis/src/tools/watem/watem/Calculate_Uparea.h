#include "MLB_Interface.h"
#include <vector>

typedef struct  {
	double latinput, rivinput, rivoutput, bedloadrivinput, bedloadrivoutput,
		bankerosion, gullycontribution, floodplainloss, totalsedimentload,
		latuparea, outuparea, inuparea, length, slope, maf, Qb;
	int segmentid, fnode, tnode, segmentdown, order, check, pondSTE, pondnr_;

} Trivdata;

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
	void DistributeTilDirEvent (int i, int j, double *AREA,  double *massbalance
		);
	std::vector<TPitData> PitDat;
	std::vector<Trivdata> RivDat;
	int pitnum = 0;
	int pitwin = 200;
	double TFCAtoCropLand;
	double TFCAtoForestOrPasture = 100;
	double Abis;
	CSG_Table *pPitDataTable;

};

