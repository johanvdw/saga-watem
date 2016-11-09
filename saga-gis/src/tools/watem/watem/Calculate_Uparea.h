#pragma once
#include "MLB_Interface.h"

class CCalculate_Uparea :
	public CSG_Tool_Grid
{
public:
	
	CCalculate_Uparea();
	virtual ~CCalculate_Uparea();

	bool On_Execute();

private:
	CSG_Grid *m_pDEM, m_pUp_Area;

	

};

