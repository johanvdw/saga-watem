#include "upstream_edges.h"
#include <queue>
#include <algorithm>
#include <iostream>

Upstream_Edges::Upstream_Edges(void)
{
    Set_Name(_TL("Upstream Edges"));

    Set_Author(_TL("Copyrights (c) 2018 by Johan Van de Wauw"));

    Set_Description(_TL(
        "Upstream Edges.")
    );

    //----------------------------------------------------

    Parameters.Add_Shapes(
        NULL, "INPUTLINES"	, _TL("Input Topology"),
        _TL(""),
        PARAMETER_INPUT,SHAPE_TYPE_Line
    );

    Parameters.Add_Table(
        NULL, "UPSTREAM_EDGES", _TL("upstream edges"),
        "",
        PARAMETER_OUTPUT);

}

Upstream_Edges::~Upstream_Edges(void)
{}


void Upstream_Edges::break_cycles(int edge_id, std::vector<int> upstream, int depth)
{
    // check if any upstream nodes from this node are in upstream
    bool contains = false;
    for (auto up_edge_it = edges[edge_id].from.begin(); up_edge_it !=edges[edge_id].from.end(); up_edge_it++ )
    {
        int edge_id = *up_edge_it;
        auto found = std::find(upstream.begin(), upstream.end(), edge_id);
        if (found != upstream.end())
        {
            std::cout << *found;
            // We have found a place where we should break:
            up_edge_it=edges[edge_id].from.erase(up_edge_it);
            //upstream.push_back(edge_id);
            // we could adjust the nodes as well - but since we don't use them anyway I didn't do that
        }
        else
        {
            upstream.push_back(edge_id);

            if (depth <5)
            {
                break_cycles(edge_id, upstream, depth +1);
            }
        }

    }
}

bool Upstream_Edges::On_Execute(void)
{
    auto pInLines	= Parameters("INPUTLINES")->asShapes();
    auto pUpstreamEdges = Parameters("UPSTREAM_EDGES")->asTable();

    pUpstreamEdges->Add_Field("edge", SG_DATATYPE_Int);
    pUpstreamEdges->Add_Field("upstream_edge", SG_DATATYPE_Int);
    pUpstreamEdges->Add_Field("proportion", SG_DATATYPE_Double);

    int start_field = pInLines->Get_Field("start_id");
    int end_field = pInLines->Get_Field("end_id");

    for (int iLine = 0; iLine < pInLines->Get_Count() && SG_UI_Process_Set_Progress(iLine, pInLines->Get_Count()); iLine++)
    {
        CSG_Shape * pLine = pInLines->Get_Shape(iLine);
        int start_id = pLine->Get_Value(start_field)->asInt();
        int end_id = pLine->Get_Value(end_field)->asInt();
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

    // search for and break circular edges
   for (auto const& it : edges)
   {
       int edge_id = it.first;
       std::vector<int> upstream;
       this->break_cycles(edge_id, upstream, 1);
   }


    // all edges without upper edges to todo

    std::queue<int> todo;
    for (auto const& it : edges)
    {
        int edge_id = it.first;
        const Edge edge = it.second;
        if (edge.from.size() == 0)
        {
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

        // check from is empty or finished

        bool finished = true;
        for (auto const& from_id : edge.from)
        {
            const Edge from_edge = edges[from_id];
            finished = from_edge.finished && finished; // if one is not finished -> we will not continue
        }
        if (finished)
        {
            edge.finished = true;
            // all nodes above are finished
            // add the edges and their upstream_edges
            for (auto const& from_id : edge.from)
            {
                const Edge from_edge = edges[from_id];

                edge.proportion[from_id] += 1 / (from_edge.to.size());

                // copy above proportions
                for (auto const& prop : from_edge.proportion)
                {
                    edge.proportion[prop.first] += prop.second /(from_edge.to.size());
                }
            }
            edge.finished = true;

            // add nodes below to todo

            for (auto const& to_id : edge.to)
            {
                todo.push(to_id);
            }

        }
        else
        {
            todo.push(edge_id);
        }

    }


    // convert results to a nice table

    for (auto const& it : edges)
    {
        const auto edge_id = it.first;
        const auto &edge = it.second;
        for (auto const& prop : edge.proportion)
        {
            auto *pRecord = pUpstreamEdges->Add_Record();
            pRecord->Set_Value("edge", edge_id);
            pRecord->Set_Value("upstream_edge", prop.first);
            pRecord->Set_Value("proportion", prop.second);
        }
    }
    return true;
}
