/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//           The Tool Link Library Interface             //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include "crs_transform.h"


//---------------------------------------------------------
// 2. Place general tool library informations here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("Proj.4") );

	case TLB_INFO_Category:
		return( _TL("Projection") );

	case TLB_INFO_Author:
		return( SG_T("O. Conrad (c) 2004-14") );

	case TLB_INFO_Description:
		return( CSG_CRSProjector::Get_Description() );

	case TLB_INFO_Version:
		return( _TL("2.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("Projection") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "crs_assign.h"

#include "crs_transform_shapes.h"
#include "crs_transform_grid.h"
#include "crs_transform_pointcloud.h"
#include "crs_transform_utm.h"

#include "PROJ4_Shapes.h"
#include "PROJ4_Grid.h"

#include "gcs_lon_range.h"

#include "gcs_graticule.h"

#include "crs_indicatrix.h"
#include "crs_grid_geogcoords.h"

#include "crs_distance.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0:	return( new CCRS_Assign() );

	case  1:	return( new CCRS_Transform_Shapes        (true ) );
	case  2:	return( new CCRS_Transform_Shapes        (false) );
	case  3:	return( new CCRS_Transform_Grid          (true ) );
	case  4:	return( new CCRS_Transform_Grid          (false) );
	case 18:	return( new CCRS_Transform_PointCloud    (true ) );
	case 19:	return( new CCRS_Transform_PointCloud    (false) );

	case 23:	return( new CCRS_Transform_UTM_Grids     (true ) );
	case 24:	return( new CCRS_Transform_UTM_Grids     (false) );
	case 25:	return( new CCRS_Transform_UTM_Shapes    (true ) );
	case 26:	return( new CCRS_Transform_UTM_Shapes    (false) );
	case 27:	return( new CCRS_Transform_UTM_PointCloud(true ) );
	case 28:	return( new CCRS_Transform_UTM_PointCloud(false) );

	case 15:	return( new CCRS_Picker() );

	case 13:	return( new CGCS_Grid_Longitude_Range() );

	case 14:	return( new CGCS_Graticule() );

	case 16:	return( new CCRS_Indicatrix() );
	case 17:	return( new CCRS_Grid_GeogCoords() );
	case 20:	return( new CCRS_Distance_Lines() );
	case 21:	return( new CCRS_Distance_Points() );
	case 22:	return( new CCRS_Distance_Interactive() );

	case  5:	return( new CPROJ4_Shapes(PROJ4_INTERFACE_SIMPLE, false) );
	case  6:	return( new CPROJ4_Shapes(PROJ4_INTERFACE_DIALOG, false) );
	case  7:	return( new CPROJ4_Grid  (PROJ4_INTERFACE_SIMPLE, false) );
	case  8:	return( new CPROJ4_Grid  (PROJ4_INTERFACE_DIALOG, false) );
	case  9:	return( new CPROJ4_Shapes(PROJ4_INTERFACE_SIMPLE, true ) );
	case 10:	return( new CPROJ4_Shapes(PROJ4_INTERFACE_DIALOG, true ) );
	case 11:	return( new CPROJ4_Grid  (PROJ4_INTERFACE_SIMPLE, true ) );
	case 12:	return( new CPROJ4_Grid  (PROJ4_INTERFACE_DIALOG, true ) );

	case 29:	return( NULL );
	default:	return( TLB_INTERFACE_SKIP_TOOL );
	}
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
