/**********************************************************
 * Version $Id: grid_extent.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Shapes                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    grid_extent.cpp                    //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
#include "grid_extent.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Extent::CGrid_Extent(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Grid System Extent"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Creates a polygon (rectangle) from a grid system's extent."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		"", "SHAPES", _TL("Extent"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice(
		"", "CELLS"	, _TL("Border"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("grid cells"),
			_TL("grid nodes")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Extent::On_Execute(void)
{
	//-----------------------------------------------------
	const CSG_Grid_System	&System	= Get_System();

	if(	!System.is_Valid() )
	{
		Error_Set(_TL("invalid grid system"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	*pShapes	= Parameters("SHAPES")->asShapes();

	pShapes->Create(SHAPE_TYPE_Polygon, _TL("Grid System Extent"));

	pShapes->Add_Field("NX"      , SG_DATATYPE_Int   );
	pShapes->Add_Field("NY"      , SG_DATATYPE_Int   );
	pShapes->Add_Field("CELLSIZE", SG_DATATYPE_Double);

	CSG_Shape	*pExtent	= pShapes->Add_Shape();

	pExtent->Set_Value(0, System.Get_NX());
	pExtent->Set_Value(1, System.Get_NY());
	pExtent->Set_Value(2, System.Get_Cellsize());

	bool	bCells	= Parameters("CELLS")->asInt() == 0;

	pExtent->Add_Point(System.Get_XMin(bCells), System.Get_YMin(bCells));
	pExtent->Add_Point(System.Get_XMin(bCells), System.Get_YMax(bCells));
	pExtent->Add_Point(System.Get_XMax(bCells), System.Get_YMax(bCells));
	pExtent->Add_Point(System.Get_XMax(bCells), System.Get_YMin(bCells));
	pExtent->Add_Point(System.Get_XMin(bCells), System.Get_YMin(bCells));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
