#pragma once
#include "MLB_Interface.h"
#include "Pitstuff.h"
#include <vector>



typedef struct  {
	double latinput, rivinput, rivoutput, bedloadrivinput, bedloadrivoutput,
		bankerosion, gullycontribution, floodplainloss, totalsedimentload,
		latuparea, outuparea, inuparea, length, slope, maf, Qb;
	long segmentid, fnode, tnode, segmentdown, order, check, pondSTE, pondnr_;

} Trivdata;


class CCalculate_Uparea :
	public CSG_Tool_Grid
{
public:
	
	CCalculate_Uparea();
	virtual ~CCalculate_Uparea();

	bool On_Execute();

private:
	CSG_Grid *m_pDEM, *m_pUp_Area, *m_pPRC, *m_Pit;

	void CalculateUparea();
	std::vector<TPitData> PitDat;
	std::vector<Trivdata> RivDat;
	int pitnum = 0;

};

