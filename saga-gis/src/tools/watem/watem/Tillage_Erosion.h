#pragma once
#include "C:\Users\jvdak06\git\saga-gis-code\saga-gis\src\saga_core\saga_api\tool.h"
class Tillage_Erosion :
	public CSG_Tool_Grid
{
public:
	Tillage_Erosion();
	bool On_Execute();
	~Tillage_Erosion();
};

