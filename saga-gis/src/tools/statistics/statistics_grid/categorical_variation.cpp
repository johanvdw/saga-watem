/**********************************************************
 * Version $Id: categorical_variation.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    statistics_grid                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               categorical_variation.cpp               //
//                                                       //
//                 Copyright (C) 2016 by                 //
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
#include "categorical_variation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCategorical_Variation::CCategorical_Variation(void)
{
	Set_Name		(_TL("Categorical Coincidence"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"Calculates for each cell the categorical coincidence, "
		"which can be useful to compare different classifications."
	));

	Parameters.Add_Grid_List(
		"", "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "CATEGORIES"	, _TL("Number of Categories"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);

	Parameters.Add_Grid(
		"", "COINCIDENCE"	, _TL("Coincidence"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "MAJ_COUNT"	, _TL("Dominance of Majority"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "MAJ_VALUE"	, _TL("Value of Majority"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Int(
		"", "RADIUS"		, _TL("Radius [Cells]"),
		_TL(""),
		0, 0, true
	);

//	m_Cells.Get_Weighting().Create_Parameters(&Parameters, false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCategorical_Variation::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
//	m_Cells.Get_Weighting().Enable_Parameters(pParameters);

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCategorical_Variation::On_Execute(void)
{
	//-----------------------------------------------------
	m_pGrids		= Parameters("GRIDS"      )->asGridList();

	m_pCategories	= Parameters("CATEGORIES" )->asGrid();
	m_pCoincidence	= Parameters("COINCIDENCE")->asGrid();
	m_pMaj_Count	= Parameters("MAJ_COUNT"  )->asGrid();
	m_pMaj_Value	= Parameters("MAJ_VALUE"  )->asGrid();

	DataObject_Set_Colors(m_pMaj_Count, 11, SG_COLORS_DEFAULT, true);

	//-----------------------------------------------------
	if( m_pGrids->Get_Grid_Count() < 1 )
	{
		Error_Set(_TL("no input"));

		return( false );
	}

	//-----------------------------------------------------
//	m_Cells.Get_Weighting().Set_Parameters(&Parameters);
	m_Cells.Set_Radius(Parameters("RADIUS")->asInt());

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			Get_Variation(x, y);
		}
	}

	//-----------------------------------------------------
	m_Cells.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCategorical_Variation::Get_Variation(int x, int y)
{
	CSG_Unique_Number_Statistics	s;

	//-----------------------------------------------------
	if( m_Cells.Get_Count() > 1 )
	{
		for(int iCell=0, ix, iy; iCell<m_Cells.Get_Count(); iCell++)
		{
			if( is_InGrid(ix = m_Cells.Get_X(iCell, x), iy = m_Cells.Get_Y(iCell, y)) )
			{
				for(int iBand=0; iBand<m_pGrids->Get_Grid_Count(); iBand++)
				{
					if( !m_pGrids->Get_Grid(iBand)->is_NoData(ix, iy) )
					{
						s.Add_Value(m_pGrids->Get_Grid(iBand)->asDouble(ix, iy));
					}
				}
			}
		}
	}
	else
	{
		for(int iBand=0; iBand<m_pGrids->Get_Grid_Count(); iBand++)
		{
			if( !m_pGrids->Get_Grid(iBand)->is_NoData(x, y) )
			{
				s.Add_Value(m_pGrids->Get_Grid(iBand)->asDouble(x, y));
			}
		}
	}

	//-----------------------------------------------------
	if( s.Get_Count() > 0 )
	{
		#define GET_COINCIDENCE(x)	((x * (x - 1)) / 2)

		int		nValues		= 0;
		double	Coincidence	= 0.0;

		for(int iClass=0; iClass<s.Get_Count(); iClass++)
		{
			int	nClass	= s.Get_Count(iClass);

			nValues		+= nClass;
			Coincidence	+= GET_COINCIDENCE(nClass);
		}

		int		maj_Count;
		double	maj_Value;

		s.Get_Majority(maj_Value, maj_Count);

		SG_GRID_PTR_SAFE_SET_VALUE(m_pCategories , x, y, s.Get_Count());
		SG_GRID_PTR_SAFE_SET_VALUE(m_pCoincidence, x, y, Coincidence / GET_COINCIDENCE(nValues));
		SG_GRID_PTR_SAFE_SET_VALUE(m_pMaj_Count  , x, y, maj_Count   / (double)nValues);
		SG_GRID_PTR_SAFE_SET_VALUE(m_pMaj_Value  , x, y, maj_Value);

		return( true );
	}

	SG_GRID_PTR_SAFE_SET_NODATA(m_pCategories , x, y);
	SG_GRID_PTR_SAFE_SET_NODATA(m_pCoincidence, x, y);
	SG_GRID_PTR_SAFE_SET_NODATA(m_pMaj_Count  , x, y);
	SG_GRID_PTR_SAFE_SET_NODATA(m_pMaj_Value  , x, y);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
