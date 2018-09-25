#include "upstream_edges.h"
#include <queue>
#include <algorithm>

Upstream_Edges::Upstream_Edges(void)
{
    Set_Name(_TL("Upstream Edges"));

    Set_Author(_TL("Copyrights (c) 2018 by Johan Van de Wauw"));

    Set_Description(_TL(
        "Upstream Edges.")
    );

    //----------------------------------------------------

    Parameters.Add_Table(
        NULL, "INPUTLINES"	, _TL("Input Topology"),
        _TL(""),
        PARAMETER_INPUT
    );

    Parameters.Add_Table(
        NULL, "ADJECTANT_EDGES", _TL("adjectant edges"),
        "",
        PARAMETER_OUTPUT);

    Parameters.Add_Table(
        NULL, "UPSTREAM_EDGES", _TL("upstream edges"),
        "",
        PARAMETER_OUTPUT);

    Parameters.Add_Bool(
        NULL, "ZERO_BASED", _TL("Zero based index"), _TL(""), true);

}

Upstream_Edges::~Upstream_Edges(void)
{}


void Upstream_Edges::break_cycles(int edge_id, std::vector<int> upstream, int depth)
{
    // check if any upstream nodes from this node are in upstream
    for (auto up_edge_it = edges[edge_id].from.begin(); up_edge_it !=edges[edge_id].from.end();  )
    {
        int up_edge_id = * up_edge_it;
        auto found = std::find(upstream.begin(), upstream.end(), up_edge_id);
        if (found != upstream.end())
        {
            // We have found a place where we should break: remove from to
            up_edge_it=edges[edge_id].from.erase(up_edge_it);

            // and remove in the corresponding other from
            auto &to = edges[up_edge_id].to;;

            auto found_to =  std::find(to.begin(), to.end(), edge_id);
            if (found_to != to.end())
            {
                to.erase(found_to);
            }
            // we could adjust the nodes as well - but since we don't use them anyway I didn't do that
        }
        else
        {
            upstream.push_back(edge_id);

            if (depth < 5)
            {
                break_cycles(up_edge_id, upstream, depth +1);
            }
            up_edge_it++;
        }

    }
}

bool Upstream_Edges::On_Execute(void)
{
    auto pInLines	= Parameters("INPUTLINES")->asShapes();
    CSG_Table * pUpstreamEdges = Parameters("UPSTREAM_EDGES")->asTable();
    CSG_Table * pAdjectantEdges = Parameters("ADJECTANT_EDGES")->asTable();

    int start_field = pInLines->Get_Field("start_id");
    int end_field = pInLines->Get_Field("end_id");

    bool zero_based = Parameters("ZERO_BASED")->asBool();

    if ((start_field == -1)||(end_field == -1))
    {
        SG_UI_Msg_Add("Source does not contain start_id and end_id columns", SG_UI_MSG_STYLE_FAILURE);
        return false;
    }

    pUpstreamEdges->Del_Records();
    pAdjectantEdges->Del_Records();

    if (pUpstreamEdges->Get_Field("edge")==-1)
        pUpstreamEdges->Add_Field("edge", SG_DATATYPE_Int);
    if (pUpstreamEdges->Get_Field("upstream_edge")==-1)
        pUpstreamEdges->Add_Field("upstream_edge", SG_DATATYPE_Int);
    if (pUpstreamEdges->Get_Field("proportion")==-1)
        pUpstreamEdges->Add_Field("proportion", SG_DATATYPE_Double);

    if (pAdjectantEdges->Get_Field("from")==-1)
        pAdjectantEdges->Add_Field("from", SG_DATATYPE_Int);
    int from_field = pAdjectantEdges->Get_Field("from");
    if (pAdjectantEdges->Get_Field("to")==-1)
        pAdjectantEdges->Add_Field("to", SG_DATATYPE_Int);
    int to_field = pAdjectantEdges->Get_Field("to");

    for (int iLine = 0; iLine < pInLines->Get_Count() && SG_UI_Process_Set_Progress(iLine, pInLines->Get_Count()); iLine++)
    {
        CSG_Shape * pLine = pInLines->Get_Shape(iLine);
        int start_id = pLine->Get_Value(start_field)->asInt();
        int end_id = pLine->Get_Value(end_field)->asInt();

        // skip lines which connect to themselve (eg when using a large tolerance)
        if (start_id == end_id) continue;

        edges[iLine].start_node = start_id;
        edges[iLine].end_node = end_id;

        nodes[start_id].to.push_back(end_id);
        nodes[end_id].from.push_back(start_id);
        nodes[start_id].to_edge.push_back(iLine);
        nodes[end_id].from_edge.push_back(iLine);
    }

    // fill graph for edges
    for (auto const& it : nodes)
    {
        int node_id = it.first;
        const Node node  = it.second;
        for (auto start_edge_id: node.from_edge)
        {
            for (auto end_edge_id: node.to_edge)
            {
                edges[start_edge_id].to.push_back(end_edge_id);
                edges[end_edge_id].from.push_back(start_edge_id);
            }
        }
    }

    nodes.clear();

    // search for and break circular edges
   for (auto const& it : edges)
   {
       int edge_id = it.first;
       std::vector<int> upstream;
       upstream.push_back(edge_id);
       this->break_cycles(edge_id, upstream, 1);
   }


    // all edges without upper edges to todo

    std::queue<int> todo;
    for (auto & it : edges)
    {
        int edge_id = it.first;
        Edge &edge = it.second;
        if (edge.from.size() == 0)
        {
            edge.shreve_order = 1;
            edge.sort_order = 1;
            todo.push(edge_id);
        }
    }

    while (todo.size()>0)
    {
        int edge_id = todo.front();
        todo.pop();
        Edge &edge = edges[edge_id];

        if (edge.finished)
            continue;

        // add the edges and their upstream_edges
        edge.sort_order = 1;
        for (auto const& from_id : edge.from)
        {
            const Edge from_edge = edges[from_id];

            edge.proportion[from_id] += 1.0 / (from_edge.to.size());
            edge.shreve_order += from_edge.shreve_order;
            edge.sort_order = std::max(edge.sort_order, from_edge.sort_order +1);
            // copy above proportions
            for (auto const& prop : from_edge.proportion)
            {
                edge.proportion[prop.first] += prop.second /(from_edge.to.size());
            }
        }
        edge.finished = true;

        // add nodes below to todo if all their upstream edges are finished

        for (auto const& to_id : edge.to)
        {
            // only add to todo

            bool finished = true;
            for (auto const& from_id : edges[to_id].from)
            {
                const Edge from_edge = edges[from_id];
                finished = from_edge.finished && finished; // if one is not finished -> we will not continue
            }

            if (finished)
                todo.push(to_id);
        }


    }



    int add_one = !zero_based;
    // convert results to a nice table

    for (auto const& it : edges)
    {
        const auto edge_id = it.first;
        const auto &edge = it.second;
        for (auto const& prop : edge.proportion)
        {
            auto *pRecord = pUpstreamEdges->Add_Record();
            pRecord->Set_Value("edge", edge_id + add_one);
            pRecord->Set_Value("upstream_edge", prop.first + add_one);
            pRecord->Set_Value("proportion", prop.second);
        }
    }

    // add shreve order to input table
    if (pInLines->Get_Field("shreve_order")==-1)
        pInLines->Add_Field("shreve_order", SG_DATATYPE_Int);
    if (pInLines->Get_Field("sort_order")==-1)
        pInLines->Add_Field("sort_order", SG_DATATYPE_Int);

    int shreve_field  = pInLines->Get_Field("shreve_order");
    int sort_field  = pInLines->Get_Field("sort_order");

    for (int iLine = 0; iLine < pInLines->Get_Count() && SG_UI_Process_Set_Progress(iLine, pInLines->Get_Count()); iLine++)
    {
        pInLines->Set_Value(iLine, shreve_field, edges[iLine].shreve_order);
        pInLines->Set_Value(iLine, sort_field, edges[iLine].sort_order);
    }

    for (auto const& it : edges)
    {
        const auto edge_id = it.first;
        const auto &edge = it.second;
        for (auto const & to: edge.to)
        {
            auto *pRecord = pAdjectantEdges->Add_Record();
            pRecord->Set_Value(from_field, edge_id + add_one);
            pRecord->Set_Value(to_field, to + add_one);
        }
    }

    edges.clear();

    return true;
}
