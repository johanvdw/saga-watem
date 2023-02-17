#include "MLB_Interface.h"

class Calculate_C_Grid :
	public CSG_Tool_Grid
{
public:
	Calculate_C_Grid();
	bool On_Execute();
	~Calculate_C_Grid();
};

CSG_Grid* BinaryShapetoGrid(CSG_Parameter* shape, const CSG_Grid_System& system, const int poly_type);