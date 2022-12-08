//////////////////////////////////////////////////////////
//														 //
//			The Module Link Library Interface			 //
//														 //
///////////////////////////////////////////////////////////


#include "TLB_Interface.h"

CSG_String Get_Info(int i)
{
	switch( i )
	{
    case TLB_INFO_Name:	default:
        return( _TL("Topologize") );

    case TLB_INFO_Author:
        return( _TL("Johan Van de Wauw (c) 2018") );

    case TLB_INFO_Description:
        return( _TL("Generate a network from a line shapefile; Based on Microcity") );

    case TLB_INFO_Version:
		return( SG_T("0.905") );

    case TLB_INFO_Menu_Path:

        return( _TL("Network") );
	}
}

#include "topologize.h"
#include "upstream_edges.h"
#include "routing_shape.h"
#include "segment_catchment.h"

CSG_Tool *		Create_Tool(int i)
{

	CSG_Tool	*pModule;
	
	switch( i )
	{
	case 0:
		pModule = new CTopologize;
		break;
    case 1:
        pModule = new Upstream_Edges;
        break;
    case 2:
        pModule = new routing_shape;
        break;
    case 3:
        pModule = new segment_catchment;
        break;
    default:
		pModule	= NULL;
		break;
	}

	return( pModule );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//{{AFX_SAGA

    TLB_INTERFACE

//}}AFX_SAGA
