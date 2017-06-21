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
//                     Grid_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     FuzzyOR.cpp                       //
//                                                       //
//                 Copyright (C) 2004 by                 //
//           Antonio Boggia and Gianluca Massei          //
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
//    e-mail:     boggia@unipg.it						 //
//				  g_massa@libero.it				     	 //
//                                                       //
//    contact:    Antonio Boggia                         //
//                Gianluca Massei                        //
//                Department of Economics and Appraisal  //
//                University of Perugia - Italy			 //
//                www.unipg.it                           //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "FuzzyOR.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFuzzyOR::CFuzzyOR(void)
{
	Set_Name		(_TL("Fuzzy Union (OR)"));

	Set_Author		(SG_T("Antonio Boggia and Gianluca Massei (c) 2004"));

	Set_Description	(_TW(
		"Calculates the union (max operator) for each grid cell of the selected grids.\n "
		"e-mail Gianluca Massei: g_massa@libero.it \n"
		"e-mail Antonio Boggia: boggia@unipg.it \n")
	);

	Parameters.Add_Grid_List(
		NULL, "GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "OR"		, _TL("Union"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL, "TYPE"	, _TL("Operator Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("max(a, b) (non-interactive)"),
			_TL("a + b - a * b"),
			_TL("min(1, a + b)")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFuzzyOR::On_Execute(void)
{
	int						Type;
	CSG_Grid				*pOR;
	CSG_Parameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	pGrids	= Parameters("GRIDS")	->asGridList();
	pOR		= Parameters("OR")		->asGrid();
	Type	= Parameters("TYPE")	->asInt();

	//-----------------------------------------------------
	if( pGrids->Get_Grid_Count() < 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			bool	bNoData	= pGrids->Get_Grid(0)->is_NoData(x, y);
			double	OR		= pGrids->Get_Grid(0)->asDouble (x, y);

			for(int i=1; i<pGrids->Get_Grid_Count() && !bNoData; i++)
			{
				if( !(bNoData = pGrids->Get_Grid(i)->is_NoData(x, y)) )
				{
					double	iz	= pGrids->Get_Grid(i)->asDouble(x, y);
					
					switch( Type )
					{
					case 0:
						if( OR < iz )
						{
							OR	= iz;
						}
						break;

					case 1:
						OR	= OR + iz - OR * iz;
						break;

					case 2:
						if( (OR = OR + iz) > 1.0 )
						{
							OR	= 1.0;
						}
						break;
					}
				}
			}

			if( bNoData )
			{
				pOR->Set_NoData(x, y);
			}
			else
			{
				pOR->Set_Value(x, y, OR);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
