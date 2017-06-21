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
//                    crs_assign.cpp                     //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "crs_assign.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Assign::CCRS_Assign(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Set Coordinate Reference System"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"The tool allows one to define the Coordinate Reference System (CRS) "
		"of the supplied data sets. The tool applies no transformation to "
		"the data sets, it just updates their CRS metadata.\n"
		"A complete and correct description of the CRS of a dataset is necessary "
		"in order to be able to actually apply a projection with one of the "
		"'Coordinate Transformation' tools later.\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Shapes_List(
		NULL	, "SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

//	Parameters.Add_TIN_List(
//		NULL	, "TINS"	, _TL("TINs"),
//		_TL(""),
//		PARAMETER_INPUT_OPTIONAL
//	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Assign::On_Execute(void)
{
	int	nTotal	= Parameters("GRIDS" )->asList()->Get_Item_Count()
				+ Parameters("SHAPES")->asList()->Get_Item_Count();
	//			+ Parameters("TINS"  )->asList()->Get_Item_Count();

	if( nTotal <= 0 )
	{
		Message_Dlg(_TL("nothing to do: no data in selection"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Projection	Projection;

	if( !Get_Projection(Projection) )
	{
		return( false );
	}

	//-----------------------------------------------------
	int	nProjected	= 0;

	nProjected	+= Set_Projections(Projection, Parameters("GRIDS" )->asList());
	nProjected	+= Set_Projections(Projection, Parameters("SHAPES")->asList());
//	nProjected	+= Set_Projections(Projection, Parameters("TINS")  ->asList());

	//-----------------------------------------------------
	return( nProjected > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCRS_Assign::Set_Projections(const CSG_Projection &Projection, CSG_Parameter_List *pList)
{
	int	nProjected	= 0;

	for(int i=0; i<pList->Get_Item_Count(); i++)
	{
		if( pList->Get_Item(i)->Get_Projection().Create(Projection) )
		{
			pList->Get_Item(i)->Set_Modified();

			DataObject_Update(pList->Get_Item(i));

			nProjected++;
		}
	}

	return( nProjected );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
