#include "MLB_Interface.h"

class Tillage_Erosion :
	public CSG_Tool_Grid
{
public:
	Tillage_Erosion();
	bool On_Execute();
	~Tillage_Erosion();
};

