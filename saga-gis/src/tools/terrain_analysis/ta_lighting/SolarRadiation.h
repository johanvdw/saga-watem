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
//                      ta_lighting                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   SolarRadiation.h                    //
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
#ifndef HEADER_INCLUDED__SolarRadiation_H
#define HEADER_INCLUDED__SolarRadiation_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class ta_lighting_EXPORT CSolarRadiation : public CSG_Tool_Grid
{
public:
	CSolarRadiation(void);


protected:

	virtual int				On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);


private:

	bool					m_bLocalSVF;

	int						m_Method, m_Location;

	double					m_Solar_Const, m_Latitude, m_Linke, m_Vapour, m_Atmosphere, m_Transmittance, m_Pressure, m_Water, m_Dust;

	CSG_Grid				*m_pDEM, *m_pSVF, *m_pLinke, *m_pVapour, *m_pDirect, *m_pDiffus, *m_pTotal, *m_pDuration, *m_pSunrise, *m_pSunset,
							m_Slope, m_Aspect, m_Shade, m_Lat, m_Lon, m_Sun_Height, m_Sun_Azimuth;


	bool					Finalize				(void);

	bool					Get_Insolation			(void);
	bool					Get_Insolation			(CSG_DateTime Date);
	bool					Get_Insolation			(CSG_DateTime Date, double Hour);
	bool					Get_Insolation			(double Sun_Height, double Sun_Azimuth, double Hour);

	double					Get_Air_Mass			(double Sun_Height);
	bool					Get_Irradiance			(int x, int y, double Sun_Height, double Sun_Azimuth, double &Direct, double &Diffus);

	bool					is_Shaded				(int x, int y);
	double					Get_Slope				(int x, int y);
	double					Get_Aspect				(int x, int y);

	bool					Get_Slope_Gradient		(void);

	bool					Get_Shade_Params		(double Sun_Height, double Sun_Azimuth, double &dx, double &dy, double &dz, int &Shadowing);
	bool					Get_Shade				(double Sun_Height, double Sun_Azimuth);
	void					Set_Shade				(double x, double y, double z, double dx, double dy, double dz, int Shadowing);
	void					Set_Shade_Bended		(double x, double y, double z                                 , int Shadowing);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SolarRadiation_H
