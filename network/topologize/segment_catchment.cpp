#include "segment_catchment.h"
#include <queue>
#include <algorithm>
#include <map>

segment_catchment::segment_catchment()
{
    Set_Name(_TL("Segment to subcatchments"));

    Set_Author(_TL("Copyrights (c) 2018 by Johan Van de Wauw"));

    Set_Description(_TL(
        "Routing and segments to subcatchment")
    );

    //----------------------------------------------------

    Parameters.Add_Table(
        NULL, "ROUTING"	, _TL("Routing Table"),
        _TL(""),
        PARAMETER_INPUT
    );
    Parameters.Add_Grid(
        NULL, "SEGMENTS", _TL("Segment Map"),
        _TL(""), PARAMETER_INPUT);
    Parameters.Add_Grid(
        NULL, "CATCH", _TL("Subcatchments"),
        _TL(""), PARAMETER_OUTPUT);


}

segment_catchment::~segment_catchment(void)
{}

inline sLong segment_catchment::pos(const int row, const int col)
{
    // convert the position in col en row to saga grid position
    int x = col -1;
    int y = Get_NY() - row;
    sLong res = y * Get_NX() +x;

    return (res);
}

bool segment_catchment::On_Execute(void)
{
    CSG_Grid * subcatch = Parameters("CATCH")->asGrid();
    CSG_Grid * segments = Parameters("SEGMENTS")->asGrid();
    CSG_Table * routing = Parameters("ROUTING")->asTable();

    subcatch->Assign_NoData();

    // routing will be stored as a map cell -> upstream cells[]
    std::map<int, std::vector<int>> upstream;

    int col_field = routing->Get_Field("col");
    int row_field = routing->Get_Field("row");
    int t1col = routing->Get_Field("target1col");
    int t1row = routing->Get_Field("target1row");
    int t2col = routing->Get_Field("target2col");
    int t2row = routing->Get_Field("target2row");
    int part1 = routing->Get_Field("part1");

    int from, to;

    for (int i=0; i< routing->Get_Count(); i++)
    {
        CSG_Table_Record * rec = routing->Get_Record(i);
        if (rec->asInt(row_field)<=0 | rec->asInt(row_field)<=0)
            continue;
        from = pos(rec->asInt(row_field), rec->asInt(col_field));
        to=rec->asDouble(part1)>=0.5?pos(rec->asInt(t1row), rec->asInt(t1col)):pos(rec->asInt(t2row), rec->asInt(t2col));
        if (to >0 && to < Get_NCells() && to!=from)
            upstream[to].push_back(from);

    }
    // x*Get_NX() + y

    // allocate map: segment_id -> todo (deque)
    // strictly speaking we could use vector but that requires that
    // all segment_ids follow eachother.
    std::map<int, std::queue<int>> todo;

    // fill initial todo values
    for (int i=0; i<segments->Get_NCells(); i++)
    {
        int seg = segments->asInt(i);
        if (seg >0){
            subcatch->Set_Value(i,seg);
            todo[seg].push(i);
        }
    }

    for (auto & iSeg:todo)
    {
        int segment = iSeg.first;
        auto iTodo = iSeg.second;
        while (iTodo.size()>0)
        {
            int cell = iTodo.front();
            iTodo.pop();
            subcatch->Set_Value(cell, segment);

            for (auto &iUp :upstream[cell])
            {
                if (subcatch->asInt(iUp)<=0)
                    iTodo.push(iUp);
            }
        }
    }

    return true;
}
