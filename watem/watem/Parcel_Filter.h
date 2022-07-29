#include "MLB_Interface.h"

class Parcel_Filter :
	public CSG_Tool_Grid
{
public:
	Parcel_Filter();
	bool On_Execute();
	~Parcel_Filter();
};

