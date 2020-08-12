
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
//                topographic_openness.cpp               //
//                                                       //
//                 Copyright (C) 2012 by                 //
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
#include "topographic_openness.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTopographic_Openness::CTopographic_Openness(void)
{
	Set_Name		(_TL("Topographic Openness"));

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		"Topographic openness expresses the dominance (positive) or enclosure (negative) "
		"of a landscape location. See Yokoyama et al. (2002) for a precise definition. "
		"Openness has been related to how wide a landscape can be viewed from any position. "
		"It has been proven to be a meaningful input for computer aided geomorphological mapping. "
	));

	Add_Reference("Anders, N. S. / Seijmonsbergen, A. C. / Bouten, W.", "2009",
		"Multi-Scale and Object-Oriented Image Analysis of High-Res LiDAR Data for Geomorphological Mapping in Alpine Mountains",
		"Proceedings of Geomorphometry 2009.",
		SG_T("http://geomorphometry.org/system/files/anders2009geomorphometry.pdf"), SG_T("pdf at geomorphometry.org")
	);

	Add_Reference("Prima, O.D.A / Echigo, A. / Yokoyama, R. / Yoshida, T.", "2006",
		"Supervised landform classification of Northeast Honshu from DEM-derived thematic maps",
		"Geomorphology, vol.78, pp.373-386."
	);

	Add_Reference("Yokoyama, R. / Shirasawa, M. / Pike, R.J.", "2002",
		"Visualizing topography by openness: A new application of image processing to digital elevation models",
		"Photogrammetric Engineering and Remote Sensing, Vol.68, pp.251-266",
		SG_T("http://www.asprs.org/a/publications/pers/2002journal/march/2002_mar_257-265.pdf"), SG_T("pdf at ASPRS")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "POS"		, _TL("Positive Openness"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "NEG"		, _TL("Negative Openness"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double(
		"", "RADIUS"	, _TL("Radial Limit"),
		_TL(""),
		10000., 0., true
	);

	Parameters.Add_Choice(
		"", "METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("multi scale"),
			_TL("line tracing")
		), 1
	);

	Parameters.Add_Double(
		"", "DLEVEL"	, _TL("Multi Scale Factor"),
		_TL(""),
		3., 1.25, true
	);

	Parameters.Add_Int(
		"", "NDIRS"		, _TL("Number of Sectors"),
		_TL(""),
		8, 2, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTopographic_Openness::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("DLEVEL", pParameter->asInt() == 0);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTopographic_Openness::On_Execute(void)
{
	CSG_Grid	*pPos, *pNeg;

	m_pDEM		= Parameters("DEM"   )->asGrid();
	pPos		= Parameters("POS"   )->asGrid();
	pNeg		= Parameters("NEG"   )->asGrid();

	m_Radius	= Parameters("RADIUS")->asDouble();
	m_Method	= Parameters("METHOD")->asInt();

	DataObject_Set_Colors(pPos, 11, SG_COLORS_RED_GREY_BLUE,  true);
	DataObject_Set_Colors(pNeg, 11, SG_COLORS_RED_GREY_BLUE, false);

	//-----------------------------------------------------
	if( m_Method == 0 )	// multi scale
	{
		if( !m_Pyramid.Create(m_pDEM, Parameters("DLEVEL")->asDouble(), GRID_PYRAMID_Mean) )
		{
			Error_Set(_TL("failed to create pyramids."));

			return( false );
		}

		m_nLevels	= m_Pyramid.Get_Count();

		if( m_Radius > 0.0 )
		{
			while( m_nLevels > 0 && m_Pyramid.Get_Grid(m_nLevels - 1)->Get_Cellsize() > m_Radius )
			{
				m_nLevels--;
			}
		}
	}
	else if( m_Radius <= 0.0 )
	{
		m_Radius	= Get_Cellsize() * M_GET_LENGTH(Get_NX(), Get_NY());
	}

	//-----------------------------------------------------
	bool	bResult	= Initialise(Parameters("NDIRS")->asInt());

	if( bResult )
	{
		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				double	Pos, Neg;

				if( !m_pDEM->is_NoData(x, y) && Get_Openness(x, y, Pos, Neg) )
				{
					if( pPos )	pPos->Set_Value(x, y, Pos);
					if( pNeg )	pNeg->Set_Value(x, y, Neg);
				}
				else
				{
					if( pPos )	pPos->Set_NoData(x, y);
					if( pNeg )	pNeg->Set_NoData(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	m_Pyramid  .Destroy();
	m_Direction.Clear  ();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTopographic_Openness::Initialise(int nDirections)
{
	m_Direction.Set_Count(nDirections);

	for(int i=0; i<nDirections; i++)
	{
		m_Direction[i].z	= (M_PI_360 * i) / nDirections;
		m_Direction[i].x	= sin(m_Direction[i].z);
		m_Direction[i].y	= cos(m_Direction[i].z);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTopographic_Openness::Get_Openness(int x, int y, double &Pos, double &Neg)
{
	CSG_Vector	Max(m_Direction.Get_Count()), Min(m_Direction.Get_Count());

	switch( m_Method )
	{
	case  0: if( !Get_Angles_Multi_Scale(x, y, Max, Min) ) return( false ); break;
	default: if( !Get_Angles_Sectoral   (x, y, Max, Min) ) return( false ); break;
	}

	//-----------------------------------------------------
	Pos	= 0.0;
	Neg	= 0.0;

	for(int i=0; i<m_Direction.Get_Count(); i++)
	{
		Pos	+= M_PI_090 - atan(Max[i]);
		Neg	+= M_PI_090 + atan(Min[i]);
	}

	Pos	/= m_Direction.Get_Count();
	Neg	/= m_Direction.Get_Count();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTopographic_Openness::Get_Angles_Multi_Scale(int x, int y, CSG_Vector &Max, CSG_Vector &Min)
{
	double		z, d;
	TSG_Point	p, q;

	z	= m_pDEM->asDouble(x, y);
	p	= Get_System().Get_Grid_to_World(x, y);

	//-----------------------------------------------------
	for(int iGrid=-1; iGrid<m_nLevels; iGrid++)
	{
		bool		bOkay	= false;
		CSG_Grid	*pGrid	= m_Pyramid.Get_Grid(iGrid);

		for(int i=0; i<m_Direction.Get_Count(); i++)
		{
			q.x	= p.x + pGrid->Get_Cellsize() * m_Direction[i].x;
			q.y	= p.y + pGrid->Get_Cellsize() * m_Direction[i].y;

			if( pGrid->Get_Value(q, d) )
			{
				d	= (d - z) / pGrid->Get_Cellsize();

				if( bOkay == false )
				{
					bOkay	= true;
					Max[i]	= Min[i]	= d;
				}
				else if( Max[i] < d )
				{
					Max[i]	= d;
				}
				else if( Min[i] > d )
				{
					Min[i]	= d;
				}
			}
		}

		if(0|| bOkay == false )
		{
			return( false );
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTopographic_Openness::Get_Angles_Sectoral(int x, int y, CSG_Vector &Max, CSG_Vector &Min)
{
	for(int i=0; i<m_Direction.Get_Count(); i++)
	{
		if(0|| Get_Angle_Sectoral(x, y, i, Max[i], Min[i]) == false )
		{
			return( false );
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CTopographic_Openness::Get_Angle_Sectoral(int x, int y, int i, double &Max, double &Min)
{
	double	iDistance, dDistance, dx, dy, ix, iy, d, z;

	z			= m_pDEM->asDouble(x, y);
	dx			= m_Direction[i].x;
	dy			= m_Direction[i].y;
	ix			= x;
	iy			= y;
	iDistance	= 0.0;
	dDistance	= Get_Cellsize() * M_GET_LENGTH(dx, dy);
	Max			= 0.0;
	Min			= 0.0;

	bool	bOkay	= false;

	while( is_InGrid(x, y) && iDistance <= m_Radius )
	{
		ix	+= dx;	x	= (int)(0.5 + ix);
		iy	+= dy;	y	= (int)(0.5 + iy);
		iDistance	+= dDistance;

		if( m_pDEM->is_InGrid(x, y) )
		{
			d	= (m_pDEM->asDouble(x, y) - z) / iDistance;

			if( bOkay == false )
			{
				bOkay		= true;
				Max	= Min	= d;
			}
			else if( Max < d )
			{
				Max	= d;
			}
			else if( Min > d )
			{
				Min	= d;
			}
		}
	}

	return( bOkay );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
