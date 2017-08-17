#include "Flow_Fields.h"
#include <map>


using std::map;
using std::pair;

CFlow_Fields::CFlow_Fields()
{
	Set_Name("Flow between fields");
	Set_Author("Johan Van de Wauw, 2017");
	Set_Description("Flow between fields (identified by ID)");

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "FIELDS", _TL("FIELDS"),
		_TL(""),
		PARAMETER_INPUT
	);


	/*Parameters.Add_Shapes(
		NULL, "FIELDS", _TL("Fields"),
		_TL("Fields for which you want to calculate the in/outflow"),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);*/

	Parameters.Add_Table(
		NULL, "FLOW", _TL("Flow table"),
		_TL("Table containing the flow from and to each field"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL, "UPAREA", "Uparea", "uparea", PARAMETER_OUTPUT
	);

	Parameters.Add_Bool(
		NULL, "STOP", "Stop at edge", "Stop flow at the edge of a field", PARAMETER_INPUT);

}


CFlow_Fields::~CFlow_Fields()
{
}

bool CFlow_Fields::On_Execute(void)
{
	DEM = Parameters("ELEVATION")->asGrid();
	fields = Parameters("FIELDS")->asGrid();
	CSG_Table * flow = Parameters("FLOW")->asTable();
	UpArea = Parameters("UPAREA")->asGrid();
	m_bStopAtEdge = Parameters("STOP")->asBool();

	map<pair<int, int>, double> flow_map;

	double dzSum, dz[8];

	UpArea->Assign(0.0);

	//todo: convert fields to a (temporary) grid - now already input

	if (!DEM->Set_Index())	// create index ...
	{
		return(false);
	}

	for (sLong n = 0; n < Get_NCells() && Set_Progress_NCells(n); n++)
	{
		int x, y;
		double Slope, Aspect;

		if (DEM->Get_Sorted(n, x, y) && !fields->is_NoData(x, y) && DEM->Get_Gradient(x, y, Slope, Aspect))
		{
			double	Up_Area = UpArea->asDouble(x, y) + Get_Cellarea();

			//---------------------------------------------
			if ((dzSum = Get_Flow(x, y, dz)) > 0.0)
			{
				for (int i = 0; i < 8; i++)
				{
					if (dz[i] > 0.0)
					{
						int	ix = Get_xTo(i, x);
						int	iy = Get_yTo(i, y);


						if (fields->asInt(x,y) != fields->asInt(ix, iy))
						{
							if (!m_bStopAtEdge) UpArea->Add_Value(ix, iy, Up_Area   * dz[i] / dzSum);
							flow_map[std::make_pair(fields->asInt(x,y), fields->asInt(ix, iy))] += Up_Area   * dz[i] / dzSum;
						}
						else
						{
							UpArea->Add_Value(ix, iy, Up_Area   * dz[i] / dzSum);
						}
					}
				}
			}

		}
	}

	if (flow->Get_Field("from") == -1)
		flow->Add_Field("from", SG_DATATYPE_Long);
	if (flow->Get_Field("to") == -1)
		flow->Add_Field("to", SG_DATATYPE_Long);
	if (flow->Get_Field("area") == -1)
		flow->Add_Field("area", SG_DATATYPE_Double);
	flow->Del_Records();
	for (map<pair<int, int>, double>::iterator it = flow_map.begin(); it != flow_map.end(); it++)
	{
		CSG_Table_Record * row = flow->Add_Record();
		row->Set_Value("from", it->first.first);
		row->Set_Value("to", it->first.second);
		row->Set_Value("area", it->second);
	}

	return true;
};


double CFlow_Fields::Get_Flow(int x, int y, double dz[8])
{
	if (fields->is_NoData(x, y))
	{
		return(0.0);
	}

	double	d, z = DEM->asDouble(x, y), dzSum = 0.0;

	for (int i = 0; i<8; i++)
	{
		dz[i] = 0.0;

		int	ix = Get_xTo(i, x);
		int	iy = Get_yTo(i, y);

		if (DEM->is_InGrid(ix, iy) && (d = z - DEM->asDouble(ix, iy)) > 0.0)
		{
			dzSum += (dz[i] = pow(d / Get_Length(i), 1.1));
		}
	}

	return(dzSum);
}

