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
//                     Grid_Gridding                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//          Interpolation_NaturalNeighbour.cpp           //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "Interpolation_NaturalNeighbour.h"

//---------------------------------------------------------
extern "C"
{
	#include "nn/nn.h"
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CInterpolation_NaturalNeighbour::CInterpolation_NaturalNeighbour(void)
	: CInterpolation(false)
{
	Set_Name		(_TL("Natural Neighbour"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"Natural Neighbour method for grid interpolation from irregular distributed points. "
		"This tool makes use of the 'nn - Natural Neighbours interpolation library' created "
		"and maintained by Pavel Sakov, CSIRO Marine Research. "
		"Find more information about this library at:\n"
		"<a href=\"http://github.com/sakov/nn-c\">github.com/sakov/nn-c</a>."
	));

	Parameters.Add_Choice(NULL,
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("Linear"),
			_TL("Sibson"),
			_TL("Non-Sibsonian")
		), 1
	);

	Parameters.Add_Double(NULL,
		"WEIGHT"	, _TL("Minimum Weight"),
		_TL("restricts extrapolation by assigning minimal allowed weight for a vertex (normally \"-1\" or so; lower values correspond to lower reliability; \"0\" means no extrapolation)"),
		0.0, 0.0, false, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation_NaturalNeighbour::Interpolate(void)
{
	//-----------------------------------------------------
	// initialize points

	CSG_Shapes	*pPoints	= Get_Points();

	int		 nn_nPoints	= 0;
	point	*nn_pPoints	= (point *)SG_Malloc(pPoints->Get_Count() * sizeof(point));

	for(int iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
	{
		CSG_Shape	*pShape	= pPoints->Get_Shape(iPoint);

		if( !pShape->is_NoData(Get_Field()) )
		{
			nn_pPoints[nn_nPoints].x	= pShape->Get_Point(0).x;
			nn_pPoints[nn_nPoints].y	= pShape->Get_Point(0).y;
			nn_pPoints[nn_nPoints].z	= pShape->asDouble(Get_Field());

			nn_nPoints++;
		}
	}

	if( nn_nPoints < 3 )
	{
		SG_FREE_SAFE(nn_pPoints);

		Error_Set(_TL("less than 3 valid points"));

		return( false );
	}

	//-----------------------------------------------------
	// initialize grid

	CSG_Grid	*pGrid	= Get_Grid();

	int		 nn_nCells;
	point	*nn_pCells	= NULL;

	points_generate(
		pGrid->Get_XMin(), pGrid->Get_XMax(),
		pGrid->Get_YMin(), pGrid->Get_YMax(),
		pGrid->Get_NX  (), pGrid->Get_NY  (),
		&nn_nCells, &nn_pCells
	);

	if( nn_nCells != pGrid->Get_NCells() )
	{
		SG_FREE_SAFE(nn_pPoints);
		SG_FREE_SAFE(nn_pCells );

		Error_Set(_TL("grid cells array creation"));

		return( false );
	}

	//-----------------------------------------------------
    Process_Set_Text(_TL("interpolating"));

	double	Weight	= Parameters("WEIGHT")->asDouble();

	switch( Parameters("METHOD")->asInt() )
	{
	case  0:
        lpi_interpolate_points (nn_nPoints, nn_pPoints        , nn_nCells, nn_pCells);
		break;

	default:
		nn_rule	= SIBSON;
        nnpi_interpolate_points(nn_nPoints, nn_pPoints, Weight, nn_nCells, nn_pCells);
		break;

	case  2:
		nn_rule	= NON_SIBSONIAN;
        nnpi_interpolate_points(nn_nPoints, nn_pPoints, Weight, nn_nCells, nn_pCells);
		break;
	}

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int iCell=0; iCell<pGrid->Get_NCells(); iCell++)
	{
		double	z	= nn_pCells[iCell].z;

		if( SG_is_NaN(z) )
		{
			pGrid->Set_NoData(iCell);
		}
		else
		{
			pGrid->Set_Value(iCell, z);
		}
	}

	//-----------------------------------------------------
	SG_FREE_SAFE(nn_pPoints);
	SG_FREE_SAFE(nn_pCells );

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
