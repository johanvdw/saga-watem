
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Lectures                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Exercise_02.cpp                     //
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
#include "Exercise_02.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_02::CExercise_02(void)
{
	Set_Name		(_TL("02: Pixel by pixel operations with two grids"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Pixel by pixel operations with two grids."
	));

	Add_Reference("Conrad, O.", "2007",
		"SAGA - Entwurf, Funktionsumfang und Anwendung eines Systems f�r Automatisierte Geowissenschaftliche Analysen",
		"ediss.uni-goettingen.de.", SG_T("http://hdl.handle.net/11858/00-1735-0000-0006-B26C-6"), SG_T("Online")
	);


	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "INPUT_A"	, _TL("First input grid"),
		_TL("This must be your input data of type grid."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "INPUT_B"	, _TL("Second input grid"),
		_TL("This must be your input data of type grid."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "OUTPUT"	, _TL("Output"),
		_TL("This will contain your output data of type grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		"", "METHOD"	, _TL("Method"),
		_TL("Choose a method"),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Addition"),
			_TL("Subtraction"),
			_TL("Multiplication"),
			_TL("Division")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_02::On_Execute(void)
{
	int		x, y, Method;
	double	a, b;
	CSG_Grid	*pInput_A, *pInput_B, *pOutput;

	//-----------------------------------------------------
	// Get parameter settings...

	pInput_A	= Parameters("INPUT_A")->asGrid();
	pInput_B	= Parameters("INPUT_B")->asGrid();
	pOutput		= Parameters("OUTPUT" )->asGrid();
	Method		= Parameters("METHOD" )->asInt ();


	//-----------------------------------------------------
	// Execute calculation...

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pInput_A->is_NoData(x, y) || pInput_B->is_NoData(x, y) )	// don't work with 'no data'...
			{
				pOutput->Set_NoData(x, y);
			}
			else
			{
				a	= pInput_A->asDouble(x, y);
				b	= pInput_B->asDouble(x, y);

				switch( Method )
				{
				case 0:	// Addition...
					pOutput->Set_Value(x, y, a + b);
					break;

				case 1:	// Subtraction...
					pOutput->Set_Value(x, y, a - b);
					break;

				case 2:	// Multiplication...
					pOutput->Set_Value(x, y, a * b);
					break;

				case 3:	// Division...
					if( b != 0.0 )	// prevent division by zero...
					{
						pOutput->Set_Value(x, y, a / b);
					}
					else
					{
						pOutput->Set_NoData(x, y);
					}
					break;
				}
			}
		}
	}


	//-----------------------------------------------------
	// Return 'true' if everything went okay...

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
