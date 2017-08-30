/*
	TODO: licentie bepalen
	copyright holders
	naam module etc
*/
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        Watem                          //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Exercise_01.h                      //
//                                                       //
//                 Copyright (C) 2016 by                 //
//                 Johan Van de Wauw                     //
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
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Exercise_01_H
#define HEADER_INCLUDED__Exercise_01_H

//---------------------------------------------------------
#include "MLB_Interface.h"

//---------------------------------------------------------
class CCalculate_LS_Watem : public CSG_Tool_Grid
{
public: ////// public members and functions: //////////////

	CCalculate_LS_Watem 
	(void);							// constructor
	virtual ~CCalculate_LS_Watem 
	(void);				// destructor


protected: /// protected members and functions: ///////////

	virtual bool		On_Execute(void);		// always override this function


private: ///// private members and functions: /////////////
	double					Get_LS(int x, int y);
	CSG_Grid				*m_pDEM, *m_pUp_Area, *PRC;
	bool					use_prc;
	double					m_Erosivity, m_Stability;
	int						m_Method;

};


//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Exercise_01_H
