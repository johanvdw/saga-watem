/**********************************************************
 * Version $Id: Exercise_01.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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
//                   Exercise_01.cpp                     //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Calculate_LS_Watem.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCalculate_LS_Watem::CCalculate_LS_Watem(void)
{
	//-----------------------------------------------------
	// Give some information about your tool...

	Set_Name	(_TL("01: LS berekening"));

	Set_Author	(_TL("Copyrights (c) 2016 by Johan Van de Wauw"));

	Set_Description	(_TW(
		"(c) 2003 by Olaf Conrad, Goettingen\n"
		"email: oconrad@gwdg.de")
	);


	//-----------------------------------------------------
	// Define your parameters list...


	Parameters.Add_Grid(
		NULL, "DEM", "Elevation",
		"",
		PARAMETER_INPUT
	);


	Parameters.Add_Grid(
		NULL, "UPSLOPE_AREA", _TL("Upslope Length Factor"),
		"",
		PARAMETER_INPUT
	);


	Parameters.Add_Grid(
		NULL, "LS"	, _TL("Calculated LS"),
		_TL("This will contain LS. "),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL, "FACTOR"	, _TL("Factor"),
		_TL("Type in a value that shall be used for the chosen mathematical operation."),
		PARAMETER_TYPE_Double,
		1.0
	);


}

//---------------------------------------------------------
CCalculate_LS_Watem::~CCalculate_LS_Watem(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCalculate_LS_Watem::On_Execute(void)
{
	int		x, y;
	double	Factor, Value_Input, Value_Output;
	CSG_Grid	*pLS;

	//-----------------------------------------------------
	// Get parameter settings...

	m_pDEM		= Parameters("DEM" )->asGrid();
	m_pUp_Area = Parameters("UPSLOPE_AREA")->asGrid();
	pLS		= Parameters("LS")->asGrid();
	Factor		= Parameters("FACTOR")->asDouble();
	m_Stability = 0;
	m_Erosivity = 1;
	m_Method = 1; //standaard algoritme govers gebruiken

	//-----------------------------------------------------
	// Check for valid parameter settings...


	//-----------------------------------------------------
	// Execute calculation...

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(x=0; x<Get_NX(); x++)
		{
			double ls = Get_LS(x, y);
			pLS->Set_Value(x, y, ls);
		}
	}


	//-----------------------------------------------------
	// Return 'true' if everything went okay...

	return( true );
}


double CCalculate_LS_Watem::Get_LS(int x, int y)
{
	double	LS, Slope, Aspect, Area, sin_Slope;

	//-----------------------------------------------------

	if (!m_pDEM->Get_Gradient(x, y, Slope, Aspect))
	{
		return(-1.0);
	}

	//if (m_Method_Slope == 1)	// distance weighted average up-slope slope
	//{
	//	Slope = m_pUp_Slope->asDouble(x, y);
	//}

	if (Slope <= 0.0)	Slope = 0.000001;
	if (Aspect < 0.0)	Aspect = 0.0;

	sin_Slope = sin(Slope);

	Area = m_pUp_Area->asDouble(x, y);
	
	//-----------------------------------------------------
	switch (m_Method)
	{
		//-----------------------------------------------------
	default:	// Moore and Nieber
	{
		LS = (0.4 + 1) * pow(Area / 22.13, 0.4) * pow(sin_Slope / 0.0896, 1.3);
	}
	break;

	//-----------------------------------------------------
	case 1:		// Desmet and Govers
	{
		double	L, S, m, x;

		m = m_Erosivity * (sin_Slope / 0.0896) / (3.0 * pow(sin_Slope, 0.8) + 0.56);
		m = m / (1.0 + m);

		x = fabs(sin(Aspect)) + fabs(cos(Aspect));

		// x: coefficient that adjusts for width of flow at the center of the cell.
		// It has a value of 1.0 when the flow is toward a side and sqrt(2.0) when
		// the flow is toward a corner (Kinnel 2005).

		L = (pow(Area + Get_Cellarea(), m + 1.0) - pow(Area, m + 1.0))
			/ (pow(Get_Cellsize(), m + 2.0) * pow(22.13, m) * pow(x, m));

		//-----------------------------------------------------

		if (Area / (x * Get_Cellsize()) < 5.0)
		{
		//Sfactor: = 3.0*(power(SIN(slope[i, j]), 0.8)) + 0.56
			S = 3.0 * pow(sin_Slope, 0.8) + 0.56;
		}
		if (Slope < 0.08975817419)		// <  9% (= atan(0.09)), ca. 5 Degree
		{
			S = 10.8 * sin_Slope + 0.03;
		}
		else if (m_Stability == 0)		// >= 9%, stable
		{
			S = 16.8 * sin_Slope - 0.5;
		}
		else							// >= 9%, thawing, unstable
		{
			S = pow(sin_Slope / 0.896, 0.6);
		}

		LS = L * S;
	}
	break;

	//-----------------------------------------------------
	case 2:		// Wischmeier and Smith
	{
		if (Slope > 0.0505)	// >  ca. 3�
		{
			LS = sqrt(Area / 22.13)
				* (65.41 * sin_Slope * sin_Slope + 4.56 * sin_Slope + 0.065);
		}
		else					// <= ca. 3�
		{
			LS = pow(Area / 22.13, 3.0 * pow(Slope, 0.6))
				* (65.41 * sin_Slope * sin_Slope + 4.56 * sin_Slope + 0.065);
		}
	}
	break;

	}

	return(LS);
}
