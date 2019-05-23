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
//                     grid_analysis                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Fragmentation_Classify.cpp              //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
#include "fragmentation_classify.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFragmentation_Classify::CFragmentation_Classify(void)
{
	Parameters.Create(this, SG_T(""), SG_T(""), SG_T(""), true);

	//-----------------------------------------------------
	Set_Name		(_TL("Fragmentation Classes from Density and Connectivity"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"Fragmentation classes:\n"
		"(1) interior, if Density = 1.0\n"
		"(2) undetermined, if Density > 0.6 and Density = Connectivity\n"
		"(3) perforated, if Density > 0.6 and Density - Connectivity > 0\n"
		"(4) edge, if Density > 0.6 and Density - Connectivity < 0\n"
		"(5) transitional, if 0.4 < Density < 0.6\n"
		"(6) patch, if Density < 0.4\n"
	));

	Add_Reference("Riitters, K., Wickham, J., O'Neill, R., Jones, B., Smith, E.", "2000",
		"Global-scale patterns of forest fragmentation",
		"Conservation Ecology 4(2):3.",
		SG_T("http://www.ecologyandsociety.org/vol4/iss2/art3/")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "DENSITY"			, _TL("Density [Percent]"),
		_TL("Density Index (Pf)."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "CONNECTIVITY"	, _TL("Connectivity [Percent]"),
		_TL("Connectivity Index (Pff)."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "FRAGMENTATION"	, _TL("Fragmentation"),
		_TL("Fragmentation Index"),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte
	);

	Parameters.Add_Bool(
		"", "BORDER"		, _TL("Add Border"),
		_TL(""),
		false
	);

	Parameters.Add_Double(
		"", "WEIGHT"		, _TL("Connectivity Weighting"),
		_TL(""),
		1.1, 0.0, true
	);

	Parameters.Add_Double(
		"", "DENSITY_MIN"	, _TL("Minimum Density [Percent]"),
		_TL(""),
		10.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Double(
		"", "DENSITY_INT"	, _TL("Minimum Density for Interior Forest [Percent]"),
		_TL("if less than 100, it is distinguished between interior and core forest"),
		99.0, 0.0, true, 100.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFragmentation_Classify::On_Execute(void)
{
	CSG_Grid	*pDensity			= Parameters("DENSITY"      )->asGrid();
	CSG_Grid	*pConnectivity		= Parameters("CONNECTIVITY" )->asGrid();
	CSG_Grid	*pFragmentation		= Parameters("FRAGMENTATION")->asGrid();

	Set_Classification(pFragmentation);

	m_Weight			= Parameters("WEIGHT"       )->asDouble();
	m_Density_Min		= Parameters("DENSITY_MIN"  )->asDouble() / 100.0;
	m_Density_Interior	= Parameters("DENSITY_INT"  )->asDouble() / 100.0;

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pDensity->is_NoData(x, y) && !pConnectivity->is_NoData(x, y) )
			{
				double	Density      = pDensity     ->asDouble(x, y) / 100.0;
				double	Connectivity = pConnectivity->asDouble(x, y) / 100.0;

			//	pFragmentation->Set_Value (x, y, 100.0 * Density * Connectivity);
				pFragmentation->Set_Value (x, y, Get_Classification(Density, Connectivity));
			}
			else
			{
				pFragmentation->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	if( Parameters("BORDER")->asBool() )
	{
		Add_Border(pFragmentation);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
