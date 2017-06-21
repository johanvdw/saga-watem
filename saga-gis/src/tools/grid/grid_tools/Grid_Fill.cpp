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
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Grid_Fill.cpp                      //
//                                                       //
//                 Copyright (C) 2005 by                 //
//                    Andre Ringeler                     //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
#include "Grid_Fill.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Fill::CGrid_Fill(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Change Grid Values - Flood Fill"));

	Set_Author		("A.Ringeler (c) 2005, O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Interactively use the flood fill method to replace a grid's cell values. "
		"If the target is not set, the changes will be stored to the original grid. ")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"GRID_IN"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"GRID_OUT"	, _TL("Changed Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Double("",
		"FILL"		, _TL("Fill Value"),
		_TL(""),
		1
	); 

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Value to be replaced"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("value at mouse position"),
			_TL("fixed value"),
			_TL("tolerance as absolute values")
		), 0
	);

	Parameters.Add_Double("",
		"ZFIXED"	, _TL("Fixed value to be replaced"),
		_TL(""),
		0.0
	); 

	Parameters.Add_Double("",
		"DZMAX"		, _TL("Upper Tolerance"),
		_TL(""),
		1.0
	); 

	Parameters.Add_Double("",
		"DZMIN"		, _TL("Lower Tolerance"),
		_TL(""),
		-1.0
	);

	Parameters.Add_Bool("",
		"NODATA"	, _TL("Fill NoData"),
		_TL(""),
		false
	); 
}

//---------------------------------------------------------
CGrid_Fill::~CGrid_Fill(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Fill::On_Execute(void)
{
	if( Parameters("GRID_OUT")->asGrid() == NULL || Parameters("GRID_IN")->asGrid() == Parameters("GRID_OUT")->asGrid() )
	{
		m_pGrid	= Parameters("GRID_IN")	->asGrid();
	}
	else
	{
		m_pGrid	= Parameters("GRID_OUT")->asGrid();
		m_pGrid->Assign(Parameters("GRID_IN")->asGrid());
	}

	m_Method			= Parameters("METHOD")	->asInt();
	m_zFill				= Parameters("FILL")	->asDouble();
	m_zFixed			= Parameters("ZFIXED")	->asDouble();
	m_zTolerance_Min	= Parameters("DZMIN")	->asDouble();
	m_zTolerance_Max	= Parameters("DZMAX")	->asDouble();
	m_bNoData			= Parameters("NODATA")	->asBool();

	if( m_zTolerance_Min > m_zTolerance_Max )
	{
		double	z			= m_zTolerance_Min;
		m_zTolerance_Min	= m_zTolerance_Min;
		m_zTolerance_Max	= z;
	}

	if (m_bNoData == true)
		m_bNoData = false;
	else
		m_bNoData = true;

	return( true );	
}

//---------------------------------------------------------
bool CGrid_Fill::On_Execute_Finish(void)
{
	m_Stack.Clear();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CGrid_Fill::Push(int x, int y)
{
	if( m_iStack >= m_Stack.Get_Count() )
	{
		m_Stack.Set_Count(m_Stack.Get_Count() + 1000);
	}

	m_Stack[m_iStack].x	= x;
	m_Stack[m_iStack].y	= y;

	m_iStack++;
}

//---------------------------------------------------------
inline void CGrid_Fill::Pop(int &x, int &y)
{
	m_iStack--;

	x	= m_Stack[m_iStack].x;
	y	= m_Stack[m_iStack].y;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Fill::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	//-----------------------------------------------------
	if(  Mode == TOOL_INTERACTIVE_LDOWN )
	{
		int		x, y, i, ix, iy, nReplaced;
		double	z, zMin, zMax;

		x	= Get_System()->Get_xWorld_to_Grid(ptWorld.Get_X());
		y	= Get_System()->Get_yWorld_to_Grid(ptWorld.Get_Y());

		if( m_pGrid && m_pGrid->is_InGrid(x, y, m_bNoData) )
		{
			Message_Add(_TL("Starting flood fill..."));

			switch( m_Method )
			{
			case 0:	z	= m_pGrid->asDouble(x, y);	break;	// value at mouse position
			case 1:	z	= m_zFixed;					break;	// fixed value
			case 2:	z	= 0.0;						break;	// tolerance as absolute values
			}

			zMin		= z + m_zTolerance_Min;
			zMax		= z + m_zTolerance_Max;

			m_iStack	= 0;
			nReplaced	= 1;

			Push(x, y);

			//---------------------------------------------
			while( m_iStack > 0 && Set_Progress_NCells(nReplaced) )
			{
				Pop(x, y);

				for(i=0; i<8; i+=2)
				{
					ix	= Get_xTo(i, x);
					iy	= Get_yTo(i, y);

					if(	m_pGrid->is_InGrid(ix, iy, m_bNoData) )
					{
						z	= m_pGrid->asDouble(ix, iy);

						if( z != m_zFill && z >= zMin && z <= zMax )
						{
							Push(ix, iy);

							m_pGrid->Set_Value(ix, iy, m_zFill);

							nReplaced++;
						}
					}
				}
			}

			//---------------------------------------------
			Message_Add(_TL("ready"), false);
			Message_Add(CSG_String::Format("%d %s", nReplaced, _TL("replacements")));

			DataObject_Update(m_pGrid, m_pGrid->Get_Min(), m_pGrid->Get_Max());

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
