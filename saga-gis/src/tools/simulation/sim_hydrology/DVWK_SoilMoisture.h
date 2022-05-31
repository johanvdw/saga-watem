
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     sim_hydrology                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  DVWK_SoilMoisture.h                  //
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
#ifndef HEADER_INCLUDED__DVWK_SoilMoisture_H
#define HEADER_INCLUDED__DVWK_SoilMoisture_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CDVWK_SoilMoisture : public CSG_Tool_Grid  
{
public:
	CDVWK_SoilMoisture(void);

	virtual CSG_String	Get_MenuPath	(void)	{	return( _TL("A:Simulation|Soil Hydrology") );	}


protected:

	virtual bool		On_Execute		(void);


private:

	double				m_FK, m_PWP;

	CSG_Grid			*m_pWi, *m_pFK, *m_pPWP, m_LandUse;

	CSG_Table			*m_pClimate, *m_pCropCoeff;


	int					Get_Month		(int Day);
	double				Get_kc			(int Bestand, int Day);

	double				Get_ETP_Haude	(int Day);
	double				Get_Pi			(int Day);
	double				Get_Wi			(double Wi, double Pi, double ETP, double kc, double FK, double PWP);

	void				Step_Day		(int Day);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__DVWK_SoilMoisture_H
