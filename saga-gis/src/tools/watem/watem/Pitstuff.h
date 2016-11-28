#pragma once
#include "MLB_Interface.h"
#include <vector>


typedef enum {
	sdra, fdra, fdpra, mfra
} TRoutingAlgorithm;

 typedef struct  {
	long outr, outc, aantal, r, c =0;//outr: outrow, outc:outcolumn
	double input=0;

 }  TPitData;



class CPitstuff :
	public CSG_Tool_Grid
{
public:
	bool On_Execute(void);
	CPitstuff();
	~CPitstuff();
private:
	CSG_Grid *m_pDEM, *m_pPRC, *m_pPit;
	void CalculatePitStuff();
	std::vector<TPitData> PitDat;
	int pitnum = 0;
};

