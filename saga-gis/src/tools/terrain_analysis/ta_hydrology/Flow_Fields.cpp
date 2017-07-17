#include "Flow_Fields.h"



CFlow_Fields::CFlow_Fields()
{
	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);
}


CFlow_Fields::~CFlow_Fields()
{
}


bool CFlow_Fields::On_Execute(void)
{
	return true;
}