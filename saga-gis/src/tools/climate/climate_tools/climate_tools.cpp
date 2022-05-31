
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   climate_tools.cpp                   //
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
// Free Software Foundation; version >=2 of the License. //
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
#include "climate_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
* Allen R.G., Pereira L.S., Raes D., Smith M. (1998):
* Crop evapotranspiration: guidelines for computing crop water requirements.
* FAO Irrigation and Drainage Paper 56. FAO, Rome
* http://www.fao.org/docrep/X0490E/x0490e07.htm#radiation
*/
//---------------------------------------------------------
double	CT_Get_Radiation_Daily_TopOfAtmosphere	(int DayOfYear, double Latitude, bool bWaterEquivalent)
{
	double	sinLat	= sin(Latitude * M_DEG_TO_RAD);
	double	cosLat	= cos(Latitude * M_DEG_TO_RAD);
	double	tanLat	= tan(Latitude * M_DEG_TO_RAD);

	double	JD		= DayOfYear * M_PI * 2. / 365.;

	double	dR		= 0.033  * cos(JD) + 1.;	// relative distance between sun and earth on any Julian day

	double	SunDec	= 0.4093 * sin(JD - 1.405);	// solar declination

	double	d		= -tanLat * tan(SunDec);	// sunset hour angle
	double	SunSet	= acos(d < -1. ? -1. : d < 1. ? d : 1.);

//	double	R0		= dR * (SunSet * sinLat * sin(SunDec) + cosLat * cos(SunDec) * sin(SunSet)) * Gsc * 24 * 60 / M_PI; // Gsc (solar constant) = 0.0820 [MJ m-2 min-1]
	double	R0		= dR * (SunSet * sinLat * sin(SunDec) + cosLat * cos(SunDec) * sin(SunSet)) * 37.58603136;

	return( bWaterEquivalent ? R0 / 2.45 : R0 );	// water equivalent [mm/day] or radiation [MJ/sqm/day]
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
* DVWK (1996): Ermittlung der Verdunstung von Land- u. Wasserflaechen. Merkblaetter 238/1996.
* Allen R.G., Pereira L.S., Raes D., Smith M. (1998):
* Crop evapotranspiration: guidelines for computing crop water requirements.
* FAO Irrigation and Drainage Paper 56. FAO, Rome
* http://www.fao.org/docrep/X0490E/x0490e07.htm#an%20alternative%20equation%20for%20eto%20when%20weather%20data%20are%20missing
* T, Tmin, Tmax = temperatures [�C]
* R0 = extraterrestrial radiation [MJ/m2/day]
*/
//---------------------------------------------------------
double	CT_Get_ETpot_Hargreave	(double T, double Tmin, double Tmax, double R0)
{
	if( (T + 17.8) <= 0. || Tmin >= Tmax )
	{
		return( 0. );
	}

	double	ETpot	= 0.0023 * R0 * (T + 17.8) * sqrt(Tmax - Tmin);	// reference crop evapotranspiration mm per day

	return( ETpot < 0. ? 0. : ETpot );
}

//---------------------------------------------------------
double	CT_Get_ETpot_Hargreave	(double T, double Tmin, double Tmax, int DayOfYear, double Latitude)
{
	double	R0	= CT_Get_Radiation_Daily_TopOfAtmosphere(DayOfYear, Latitude, false);

	return( CT_Get_ETpot_Hargreave(T, Tmin, Tmax, R0) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
* Daily potential evapotranspiration (ETpot) after Turc:
* T  = daily mean of temperature [�C]
* Rg = daily sum of global radiation [J/cm^2]
* rH = daily mean relative humidity [%]
* DVWK (1996): Ermittlung der Verdunstung von Land- u. Wasserflaechen. Merkblaetter 238/1996.
*/
//---------------------------------------------------------
double	CT_Get_ETpot_Turc	(double T, double Rg, double rH)
{
	if( (T + 15.) <= 0. )
	{
		return( 0. );
	}

	double	ETpot	= 0.0031 * (Rg + 209.) * T / (T + 15.);

	if( rH < 50. )
	{
		ETpot	*= 1. + (50. - rH) / 70.;
	}

	return( ETpot < 0. ? 0. : ETpot );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
* Daily potential evapotranspiration (ETpot) after Penman (simplified):
* T  = daily mean of temperature [�C]
* Rg = daily sum of global radiation [J/cm^2]
* rH = daily mean relative humidity [%]
* V  = daily mean of wind speed at 2m above ground [m/s]
* S0 = day length [h] (= astronomically possible sun shine duration in hours)
* DVWK (1996): Ermittlung der Verdunstung von Land- u. Wasserflaechen. Merkblaetter 238/1996.
* Allen R.G., Pereira L.S., Raes D., Smith M. (1998):
* Crop evapotranspiration: guidelines for computing crop water requirements.
* FAO Irrigation and Drainage Paper 56. FAO, Rome
* http://www.fao.org/3/X0490E/x0490e00.htm
*/
//---------------------------------------------------------
double	CT_Get_ETpot_Penman	(double T, double Rg, double rH, double V, double S0)
{
	if( (T + 22.) <= 0. || S0 <= 0. || Rg <= 0. )
	{
		return( 0. );
	}

	// Latent heat of vaporization. As L varies only slightly over normal
	// temperature ranges a single value of 2.45 MJ/kg is taken in the
	// simplification of the FAO Penman-Monteith equation (T of about 20�C).
	const double	L	= 245.;	// Latent heat of vaporization [J/cm^2]

	double	gT	= 2.3 * (T + 22.) / (T + 123.);

	double	ETpot	= gT * ((0.6 * Rg / L) + 0.66 * (1. + 1.08 * V) * (1. - rH / 100.) * S0 / 12.);

	return( ETpot < 0. ? 0. : ETpot );
}

//---------------------------------------------------------
double	CT_Get_ETpot_Penman	(double T, double Rg, double rH, double V, int DayOfYear, double Latitude)
{
	return( CT_Get_ETpot_Penman(T, Rg, rH, V, SG_Get_Day_Length(DayOfYear, Latitude)) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
* Daily potential FAO grass reference evapotranspiration (ETpot) after Penman & Monteith:
* T    = daily mean of temperature [�C]
* Tmin = daily minimum of temperature [�C]
* Tmax = daily maximum of temperature [�C]
* Rg   = daily sum of global radiation [J/cm^2]
* rH   = daily mean relative humidity [%]
* V    = daily mean of wind speed at 2m above ground [m/s]
* P    = atmospheric pressure [kPa]
* DVWK (1996): Ermittlung der Verdunstung von Land- u. Wasserflaechen. Merkblaetter 238/1996.
* Allen R.G., Pereira L.S., Raes D., Smith M. (1998):
* Crop evapotranspiration: guidelines for computing crop water requirements.
* FAO Irrigation and Drainage Paper 56. FAO, Rome
* http://www.fao.org/3/X0490E/x0490e00.htm
*/
//---------------------------------------------------------
double	CT_Get_ETpot_FAORef	(double T, double Tmin, double Tmax, double Rg, double rH, double V, double P, double dZ)
{
	if( T <= -237.3 || Rg <= 0. )
	{
		return( 0. );
	}

	if( dZ != 0. ) // (3. 1) adjustment of the atmospheric pressure [kPa]
	{
		P	*= pow(1. - (0.0065 * dZ / (273.15 + T)), 5.255);
	}

	// (3. 2) psychrometric constant [kPa/�C]
	double	y	= 0.000664742 * P;

	// (3. 6) slope of saturation vapour pressure curve at air temperature T [kPa/�C]
	double	A	= 4098. * (0.6108 * exp((17.27 * T) / (T + 237.3))) / SG_Get_Square(T + 237.3);

	// (3. 4) saturation vapour pressure at the air temperature T [kPa]
	#define Get_PVapourSat(t)	(0.610 * exp((17.27 * t) / (t + 237.3)))

	// (3. 5) (mean!) saturation vapour pressure [kPa]
	double	es	= 0.5 * (Get_PVapourSat(Tmin) + Get_PVapourSat(Tmax));

	// (3.12) actual vapour pressure (e0(Tmean) * rH / 100.) [kPa]
	double	ea	= Get_PVapourSat(T) * rH / 100.;

	Rg	/= 100.; // [J/cm^2] -> [MJ/m^2] -> 10,000 / 1,000,000

	// (3.28) reference evapotranspiration [mm/day]
	double	ETpot	= (0.408 * A * Rg + y * (900. / (T + 273.15)) * V * (es - ea)) / (A + y * (1. + 0.34 * V));

	return( ETpot < 0. ? 0. : ETpot );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	CT_Get_Daily_Splined(CSG_Vector &Daily, const double Monthly[12])
{
	static const int	MidOfMonth[12]	=
	// JAN  FEB  MAR  APR  MAY  JUN  JUL  AUG  SEP  OCT  NOV  DEC
	//	 0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334
	{	15,  45,  74, 105, 135, 166, 196, 227, 258, 288, 319, 349	};

	CSG_Spline	Spline;

	Spline.Add(MidOfMonth[10] - 365, Monthly[10]);
	Spline.Add(MidOfMonth[11] - 365, Monthly[11]);

	for(int iMonth=0; iMonth<12; iMonth++)
	{
		Spline.Add(MidOfMonth[iMonth], Monthly[iMonth]);
	}

	Spline.Add(MidOfMonth[ 0] + 365, Monthly[ 0]);
	Spline.Add(MidOfMonth[ 1] + 365, Monthly[ 1]);

	Daily.Create(365);

	for(int iDay=0; iDay<365; iDay++)
	{
		Daily[iDay]	= Spline.Get_Value(iDay);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	CT_Get_Daily_Precipitation(CSG_Vector &Daily_P, const double Monthly_P[12])	// linear interpolation
{
	static const int	nDaysOfMonth[13]	=
	// JAN  FEB  MAR  APR  MAY  JUN  JUL  AUG  SEP  OCT  NOV  DEC  JAN
	{	31,  28,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31,  31	};

	static const int	MidOfMonth[13]	=
	// JAN  FEB  MAR  APR  MAY  JUN  JUL  AUG  SEP  OCT  NOV  DEC  JAN
	//	 0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334, 365
	{	15,  45,  74, 105, 135, 166, 196, 227, 258, 288, 319, 349, 380	};

	Daily_P.Create(365);

	for(int iMonth=0, jMonth=1; iMonth<12; iMonth++, jMonth++)
	{
		int	Day0	= MidOfMonth[iMonth     ];
		int	nDays	= MidOfMonth[jMonth     ] - Day0;
		double	P0	= Monthly_P [iMonth     ] / nDaysOfMonth[iMonth];
		double	dP	= Monthly_P [jMonth % 12] / nDaysOfMonth[jMonth] - P0;

		for(int iDay=0; iDay<=nDays; iDay++)
		{
			Daily_P[(Day0 + iDay) % 365]	= P0 + iDay * dP / nDays;
		}
	}

	return( true );
}

//---------------------------------------------------------
bool	CT_Get_Daily_Precipitation(CSG_Vector &Daily_P, const double Monthly_P[12], const double Monthly_T[12])
{
	static const int	nDaysOfMonth[12]	=
	// JAN  FEB  MAR  APR  MAY  JUN  JUL  AUG  SEP  OCT  NOV  DEC
	{	31,  28,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31	};

	Daily_P.Create(365);

	for(int iMonth=0, iDay=0; iMonth<12; iDay+=nDaysOfMonth[iMonth++])
	{
		double	dEvent	= Monthly_T[iMonth] < 5 ? 5 : Monthly_T[iMonth] < 10 ? 10 : 20;
		int		nEvents	= (int)(0.5 + Monthly_P[iMonth] / dEvent);

		if( nEvents < 1 )
		{
			nEvents	= 1;
		}
		else if( nEvents > nDaysOfMonth[iMonth] )
		{
			nEvents	= nDaysOfMonth[iMonth];
		}

		dEvent	= Monthly_P[iMonth] / nEvents;

		int	Step	= nDaysOfMonth[iMonth] / nEvents;

		for(int iEvent=0, jDay=Step/2; iEvent<nEvents; iEvent++, jDay+=Step)
		{
			Daily_P[iDay + jDay]	= dEvent;
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCT_Snow_Accumulation::CCT_Snow_Accumulation(void)
{
	m_Snow.Create(365);
	m_nSnow	= 0;
}

//---------------------------------------------------------
CCT_Snow_Accumulation::CCT_Snow_Accumulation(const CCT_Snow_Accumulation &Snow_Accumulation)
{
	m_Snow.Create(Snow_Accumulation.m_Snow);
	m_nSnow	= Snow_Accumulation.m_nSnow;
}

//---------------------------------------------------------
CCT_Snow_Accumulation::~CCT_Snow_Accumulation(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCT_Snow_Accumulation::Calculate(const double T[365], const double P[365])
{
	//-----------------------------------------------------
	int		iStart	= Get_Start(T);

	if( iStart < 0 )	// no frost/non-frost change
	{
		if( T[0] < 0. )	// no day without frost
		{
			double	Snow	= 0.;

			for(int i=0; i<365; i++)
			{
				Snow	+= P[i];
			}

			m_Snow	= Snow;	// precipitation accumulated over one year
			m_nSnow	= 365;
		}
		else				// no frost at all
		{
			m_Snow	= 0.;
			m_nSnow	= 0;
		}

		return( true );
	}

	//-----------------------------------------------------
	int	nSnow, iPass = 0, maxIter = 64;

	double	Snow	= 0.;

	m_Snow	= 0.;
	m_nSnow	= 0;	// days with snow cover

	do
	{
		nSnow	= m_nSnow;	m_nSnow	= 0;

		for(int iDay=iStart; iDay<iStart+365; iDay++)
		{
			int	i	= iDay % 365;

			if( T[i] < 0. )		// snow accumulation
			{
				Snow	+= P[i];
			}
			else if( Snow > 0. )	// snow melt
			{
				Snow	-= Get_SnowMelt(Snow, T[i], P[i]);
			}

			if( Snow > 0. )
			{
				m_nSnow++;
			}

			m_Snow[i]	= Snow;
		}
	}
	while( m_nSnow != nSnow && m_nSnow < 365 && iPass++ < maxIter );

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCT_Snow_Accumulation::Get_Start(const double *T)
{
	int	iMax = -1, nMax = 0;

	for(int iDay=0; iDay<365; iDay++)
	{
		if( T[iDay] <= 0. )
		{
			int	i = iDay + 1, n = 0;

			while( T[i % 365] > 0. )	{	i++; n++;	}

			if( nMax < n )
			{
				nMax	= n;
				iMax	= i - 1;
			}
		}
	}

	return( iMax % 365 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CCT_Snow_Accumulation::Get_SnowMelt(double Snow, double T, double P)
{
	if( T > 0. && Snow > 0. )
	{
		double	dSnow	= T * (0.84 + 0.125 * P);

		return( dSnow > Snow ? Snow : dSnow );
	}

	return( 0. );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCT_Soil_Water::CCT_Soil_Water(void)
{
	m_SW_Capacity  [0]	=  20;
	m_SW_Capacity  [1]	= 200;

	m_SW_Resistance[0]	= 0.;
	m_SW_Resistance[1]	= 1.;
}

//---------------------------------------------------------
CCT_Soil_Water::CCT_Soil_Water(const CCT_Soil_Water &Soil_Water)
{
	Create(Soil_Water);
}

//---------------------------------------------------------
bool CCT_Soil_Water::Create(const CCT_Soil_Water &Soil_Water)
{
	m_SW_Capacity  [0]	= Soil_Water.m_SW_Capacity  [0];
	m_SW_Capacity  [1]	= Soil_Water.m_SW_Capacity  [1];

	m_SW_Resistance[0]	= Soil_Water.m_SW_Resistance[0];
	m_SW_Resistance[1]	= Soil_Water.m_SW_Resistance[1];

	return( true );
}

//---------------------------------------------------------
CCT_Soil_Water::~CCT_Soil_Water(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCT_Soil_Water::Set_Capacity(int Layer, double Value)
{
	if( Layer >= 0 && Layer <= 1 && Value >= 0. )
	{
		m_SW_Capacity[Layer]	= Value;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CCT_Soil_Water::Set_ET_Resistance(int Layer, double Value)
{
	if( Layer >= 0 && Layer <= 1 && Value > 0. )
	{
		m_SW_Resistance[Layer]	= Value;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCT_Soil_Water::Calculate(const double T[365], const double P[365], const double ETpot[365], const double Snow[365])
{
	//-----------------------------------------------------
	int	iStart	= Get_Start(P, ETpot), iPass = 0, maxIter = 64;

	double	SW[2], SW_Last[2];

	m_SW[0].Create(365);
	m_SW[1].Create(365);

	if( m_SW_Capacity[0] + m_SW_Capacity[1] <= 0. )
	{
		m_SW[0]	= 0.;
		m_SW[1]	= 0.;

		return( true );
	}

	SW[0]	= 0.5 * m_SW_Capacity[0];
	SW[1]	= 0.5 * m_SW_Capacity[1];

	do
	{
		SW_Last[0]	= SW[0];
		SW_Last[1]	= SW[1];

		for(int iDay=iStart; iDay<iStart+365; iDay++)
		{
			int	i	= iDay % 365;

			if( T[i] > 0. )
			{
				//---------------------------------------------
				// upper soil layer

				double	dSW	= P[i];

				if( Snow[i] > 0. )
				{
					dSW	+= CCT_Snow_Accumulation::Get_SnowMelt(Snow[i], T[i], P[i]);
				}
				else
				{
					dSW	-= ETpot[i];
				}

				SW[0]	+= dSW;

				if( SW[0] > m_SW_Capacity[0] )	// more water in upper soil layer than its capacity
				{
					dSW		= SW[0] - m_SW_Capacity[0];
					SW[0]	= m_SW_Capacity[0];
				}
				else if( SW[0] < 0. )	// evapotranspiration exceeds available water
				{
				//	dSW		= m_SW_Capacity[1] > 0. ? (SW[0] > -SW[1] ? SW[0] : -SW[1]) * sqrt(SW[1] / m_SW_Capacity[1]) : 0.;
					dSW		= m_SW_Capacity[1] > 0. ? SW[0] * pow(SW[1] / m_SW_Capacity[1], m_SW_Resistance[1]) : 0.;	// positive: runoff, negative: loss by evapotranspiration not covered by upper layer, with water loss resistance;
					SW[0]	= 0.;
				}
				else
				{
					dSW		= 0.;
				}

				//---------------------------------------------
				// lower soil layer

				SW[1]	+= dSW;

				if( SW[1] > m_SW_Capacity[1] )	// more water in lower soil layer than its capacity
				{
					SW[1]	= m_SW_Capacity[1];
				}
				else if( SW[1] < 0. )	// evapotranspiration exceeds available water
				{
					SW[1]	= 0.;
				}
			}

			//---------------------------------------------
			m_SW[0][i]	= SW[0];
			m_SW[1][i]	= SW[1];
		}
	}
	while( iPass++ <= 1 || (SW_Last[0] != SW[0] && iPass <= maxIter) );

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCT_Soil_Water::Get_Start(const double *P, const double *ETpot)
{
	int	iMax = 0, nMax = 0;

	for(int iDay=0; iDay<365; iDay++)
	{
		if( P[iDay] <= 0. )
		{
			int	i = iDay + 1, n = 0;

			while( P[i % 365] > 0. )	{	i++; n++;	}

			if( nMax < n )
			{
				nMax	= n;
				iMax	= i - 1;
			}
		}
	}

	return( iMax % 365 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCT_Water_Balance::CCT_Water_Balance(void)
{}

//---------------------------------------------------------
CCT_Water_Balance::CCT_Water_Balance(const CCT_Water_Balance &Copy)
{
	m_Soil.Create(Copy.m_Soil);

	for(int i=0; i<MONTHLY_COUNT; i++)
	{
		m_Monthly[i].Create(Copy.m_Monthly[i]);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCT_Water_Balance::Set_Monthly(int Type, const double Values[12])
{
	return( m_Monthly[Type].Create(12, (double *)Values) );
}

//---------------------------------------------------------
bool CCT_Water_Balance::Set_Monthly(int Type, int x, int y, CSG_Parameter_Grid_List *pMonthly, double Default)
{
	m_Monthly[Type].Create(12);

	double *Monthly	= m_Monthly[Type].Get_Data();

	if( pMonthly->Get_Grid_Count() == 12 )
	{
		bool	bOkay	= true;

		for(int iMonth=0; iMonth<12; iMonth++)
		{
			if( pMonthly->Get_Grid(iMonth)->is_NoData(x, y) )
			{
				bOkay	= false;

				Monthly[iMonth]	= Default;
			}
			else
			{
				Monthly[iMonth]	= pMonthly->Get_Grid(iMonth)->asDouble(x, y);
			}
		}

		return( bOkay );
	}

	for(int iMonth=0; iMonth<12; iMonth++)
	{
		Monthly[iMonth]	= Default;
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCT_Water_Balance::Calculate(double SWC, double Latitude)
{
	//-----------------------------------------------------
	Set_Soil_Capacity(SWC);

	//-----------------------------------------------------
	CT_Get_Daily_Splined      (m_Daily[DAILY_T], m_Monthly[MONTHLY_T]);
	CT_Get_Daily_Precipitation(m_Daily[DAILY_P], m_Monthly[MONTHLY_P], m_Monthly[MONTHLY_T]);

	m_Snow.Calculate(m_Daily[DAILY_T], m_Daily[DAILY_P]);

	m_Soil.Calculate(m_Daily[DAILY_T], m_Daily[DAILY_P], Set_ETpot(Latitude, m_Monthly[MONTHLY_Tmin], m_Monthly[MONTHLY_Tmax]), m_Snow);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCT_Water_Balance::Set_Soil_Capacity(double SWC)
{
	if( SWC < m_Soil.Get_Capacity(0) )
	{
		m_Soil.Set_Capacity(0, SWC);
		m_Soil.Set_Capacity(1, 0. );
	}
	else
	{
		m_Soil.Set_Capacity(1, SWC - m_Soil.Get_Capacity(0));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const double * CCT_Water_Balance::Set_ETpot(double Latitude, const double Tmin[12], const double Tmax[12])
{
	CSG_Vector	dTmin;	CT_Get_Daily_Splined(dTmin, Tmin);
	CSG_Vector	dTmax;	CT_Get_Daily_Splined(dTmax, Tmax);

	m_Daily[DAILY_ETpot].Create(365);

	for(int i=0; i<365; i++)
	{
		m_Daily[DAILY_ETpot][i]	= CT_Get_ETpot_Hargreave(m_Daily[DAILY_T][i], dTmin[i], dTmax[i], i + 1, Latitude);
	}

	return( m_Daily[DAILY_ETpot] );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
