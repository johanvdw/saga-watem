#include "MLB_Interface.h"
class Water_Erosion :
	public CSG_Tool_Grid
{
public:
	Water_Erosion();
	bool On_Execute();
	~Water_Erosion();


private:
	CSG_Grid *C, *K, *LS, *water_erosion;
	double R, P;
};

