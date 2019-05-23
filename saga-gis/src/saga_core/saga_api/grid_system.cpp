/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   grid_system.cpp                     //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University Hamburg                     //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "grid.h"
#include "shapes.h"
#include "parameters.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int		CSG_Grid_System::m_Precision	= 16;	// 16 decimal digits, default precision used for storing cellsize and extent

//---------------------------------------------------------
int CSG_Grid_System::Set_Precision(int Decimals)
{
	if( Decimals >= 0 )
	{
		m_Precision	= Decimals;
	}

	return( m_Precision );
}

//---------------------------------------------------------
int CSG_Grid_System::Get_Precision(void)
{
	return( m_Precision );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(void)
{
	Destroy();
}

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(const CSG_Grid_System &System)
{
	Create(System);
}

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(double Cellsize, const CSG_Rect &Extent)
{
	Create(Cellsize, Extent);
}

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(double Cellsize, double xMin, double yMin, double xMax, double yMax)
{
	Create(Cellsize, xMin, yMin, xMax, yMax);
}

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(double Cellsize, double xMin, double yMin, int NX, int NY)
{
	Create(Cellsize, xMin, yMin, NX, NY);
}

//---------------------------------------------------------
CSG_Grid_System::~CSG_Grid_System(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_System::Create(const CSG_Grid_System &System)
{
	m_NX			= System.m_NX;
	m_NY			= System.m_NY;
	m_NCells		= System.m_NCells;

	m_Cellsize		= System.m_Cellsize;
	m_Cellarea		= System.m_Cellarea;
	m_Diagonal		= System.m_Diagonal;

	m_Extent		= System.m_Extent;
	m_Extent_Cells	= System.m_Extent_Cells;

	return( is_Valid() );
}

//---------------------------------------------------------
bool CSG_Grid_System::Create(double Cellsize, const CSG_Rect &Extent)
{
	if( Cellsize > 0.0 && Extent.Get_XRange() >= 0.0 && Extent.Get_YRange() >= 0.0 )
	{
		int		nx	= 1 + (int)(0.5 + Extent.Get_XRange() / Cellsize);
		int		ny	= 1 + (int)(0.5 + Extent.Get_YRange() / Cellsize);

		double	x	= !fmod(Extent.Get_XRange(), Cellsize) ? Extent.Get_XMin() : Extent.Get_Center().Get_X() - Cellsize * nx / 2.;
		double	y	= !fmod(Extent.Get_YRange(), Cellsize) ? Extent.Get_YMin() : Extent.Get_Center().Get_Y() - Cellsize * ny / 2.;

		return( Create(Cellsize, x, y, nx, ny) );
	}

	Destroy();

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid_System::Create(double Cellsize, double xMin, double yMin, double xMax, double yMax)
{
	return( Create(Cellsize, CSG_Rect(xMin, yMin, xMax, yMax)) );
}

//---------------------------------------------------------
bool CSG_Grid_System::Create(double Cellsize, double xMin, double yMin, int NX, int NY)
{
	if( Cellsize > 0.0 && NX > 0 && NY > 0 )
	{
		Cellsize	= SG_Get_Rounded(Cellsize, m_Precision);
		xMin		= SG_Get_Rounded(xMin    , m_Precision);
		yMin		= SG_Get_Rounded(yMin    , m_Precision);

		if( Cellsize > 0.0 )
		{
			m_NX		= NX;
			m_NY		= NY;
			m_NCells	= (sLong)NY * NX;

			m_Cellsize	= Cellsize;
			m_Cellarea	= Cellsize * Cellsize;
			m_Diagonal	= Cellsize * sqrt(2.0);

			m_Extent.m_rect.xMin	= xMin;
			m_Extent.m_rect.yMin	= yMin;
			m_Extent.m_rect.xMax	= xMin + (NX - 1.0) * Cellsize;
			m_Extent.m_rect.yMax	= yMin + (NY - 1.0) * Cellsize;

			m_Extent_Cells	= m_Extent;
			m_Extent_Cells.Inflate(0.5 * Cellsize, false);

			return( true );
		}
	}

	//-----------------------------------------------------
	m_NX			= 0;
	m_NY			= 0;
	m_NCells		= 0;

	m_Cellsize		= 0.0;
	m_Cellarea		= 0.0;
	m_Diagonal		= 0.0;

	m_Extent		.Assign(0.0, 0.0, 0.0, 0.0);
	m_Extent_Cells	.Assign(0.0, 0.0, 0.0, 0.0);

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid_System::Destroy(void)
{
	Create(0.0, 0.0, 0.0, 0, 0);

	return( true );
}

//---------------------------------------------------------
bool CSG_Grid_System::Assign(const CSG_Grid_System &System)
{	return( Create(System) );	}

bool CSG_Grid_System::Assign(double Cellsize, const CSG_Rect &Extent)
{	return( Create(Cellsize, Extent) );	}

bool CSG_Grid_System::Assign(double Cellsize, double xMin, double yMin, double xMax, double yMax)
{	return( Create(Cellsize, xMin, yMin, xMax, yMax) );	}

bool CSG_Grid_System::Assign(double Cellsize, double xMin, double yMin, int NX, int NY)
{	return( Create(Cellsize, xMin, yMin, NX, NY) );	}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_System::is_Valid(void) const
{
	return( m_Cellsize > 0.0 );
}

//---------------------------------------------------------
const SG_Char * CSG_Grid_System::Get_Name(bool bShort)
{
	if( is_Valid() )
	{
		if( bShort )
		{
			m_Name.Printf("%.*f; %dx %dy; %.*fx %.*fy",
				SG_Get_Significant_Decimals(Get_Cellsize()), Get_Cellsize(),
				Get_NX(), Get_NY(),
				SG_Get_Significant_Decimals(Get_XMin    ()), Get_XMin    (),
				SG_Get_Significant_Decimals(Get_YMin    ()), Get_YMin    ()
			);
		}
		else
		{
			m_Name.Printf("%s: %f, %s: %dx/%dy, %s: %fx/%fy",
				_TL("Cell size"        ), Get_Cellsize(),
				_TL("Number of cells"  ), Get_NX(), Get_NY(),
				_TL("Lower left corner"), Get_XMin(), Get_YMin()
			);
		}
	}
	else
	{
		m_Name	= _TL("<not set>");
	}

	return( m_Name );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_System::operator == (const CSG_Grid_System &System) const
{
	return( is_Equal(System) );
}

//---------------------------------------------------------
void CSG_Grid_System::operator = (const CSG_Grid_System &System)
{
	Create(System);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_System::is_Equal(const CSG_Grid_System &System) const
{
	return( m_Cellsize           == System.m_Cellsize
		&&  m_NX                 == System.m_NX
		&&  m_NY                 == System.m_NY
		&&  m_Extent.m_rect.xMin == System.m_Extent.m_rect.xMin
		&&  m_Extent.m_rect.yMin == System.m_Extent.m_rect.yMin
	);
}

//---------------------------------------------------------
bool CSG_Grid_System::is_Equal(double Cellsize, const TSG_Rect &Extent) const
{
	return( m_Cellsize == Cellsize && m_Extent == Extent );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid_Cell_Addressor::CSG_Grid_Cell_Addressor(void)
{
	m_Cells.Add_Field("X", SG_DATATYPE_Int   );
	m_Cells.Add_Field("Y", SG_DATATYPE_Int   );
	m_Cells.Add_Field("D", SG_DATATYPE_Double);
	m_Cells.Add_Field("W", SG_DATATYPE_Double);
}

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Destroy(void)
{
	m_Cells.Del_Records();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Add_Parameters(CSG_Parameters &Parameters, const SG_Char *Parent, int Style)
{
	Parameters.Add_Choice("", "KERNEL_TYPE", _TL("Kernel Type"),
		_TL("The shape of the filter kernel."),
		"0|1|", 1
	);

	CSG_String	Unit	= (Style & SG_GRIDCELLADDR_PARM_MAPUNIT) == 0
		? _TL("The kernel radius in cells.")
		: _TL("The kernel radius in map units.");

	if( (Style & SG_GRIDCELLADDR_PARM_SIZEDBL) == 0 )
	{
		Parameters.Add_Int   ("", "KERNEL_RADIUS", _TL("Kernel Radius"), Unit, 2  , 1  , true);
	}
	else
	{
		Parameters.Add_Double("", "KERNEL_RADIUS", _TL("Kernel Radius"), Unit, 1.0, 0.0, true);
	}

	//-----------------------------------------------------
	CSG_String	Types;

	if( (Style & SG_GRIDCELLADDR_PARM_SQUARE) != 0 )
	{
		Types	+= CSG_String::Format("{%d}%s|", SG_GRIDCELLADDR_PARM_SQUARE , _TL("Square"));
	}

	if( (Style & SG_GRIDCELLADDR_PARM_CIRCLE) != 0 )
	{
		Types	+= CSG_String::Format("{%d}%s|", SG_GRIDCELLADDR_PARM_CIRCLE , _TL("Circle"));
	}

	if( (Style & SG_GRIDCELLADDR_PARM_ANNULUS) != 0 )
	{
		Types	+= CSG_String::Format("{%d}%s|", SG_GRIDCELLADDR_PARM_ANNULUS, _TL("Annulus"));

		Parameters.Add_Double("", "KERNEL_INNER", _TL("Inner Kernel Radius"), _TL(""));
	}

	if( (Style & SG_GRIDCELLADDR_PARM_SECTOR) != 0 )
	{
		Types	+= CSG_String::Format("{%d}%s|", SG_GRIDCELLADDR_PARM_SECTOR , _TL("Sector"));

		Parameters.Add_Double("", "KERNEL_DIRECTION", _TL("Kernel Direction"), _TL(""));
		Parameters.Add_Double("", "KERNEL_TOLERANCE", _TL("Kernel Tolerance"), _TL(""));
	}

	Parameters("KERNEL_TYPE")->asChoice()->Set_Items(Types);

	return( true );
}

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Set_Parameters(CSG_Parameters &Parameters, int Type)
{
	if( Type == 0 && Parameters("KERNEL_TYPE") )
	{
		Type	= Parameters("KERNEL_TYPE")->asChoice()->Get_Item_Data(Parameters("KERNEL_TYPE")->asInt()).asInt();
	}

	switch( Type )
	{
	case SG_GRIDCELLADDR_PARM_SQUARE:
		return( Set_Radius(
			Parameters("KERNEL_RADIUS"   )->asDouble(),
			true
		));

	case SG_GRIDCELLADDR_PARM_CIRCLE:
		return( Set_Radius(
			Parameters("KERNEL_RADIUS"   )->asDouble(),
			false
		));

	case SG_GRIDCELLADDR_PARM_ANNULUS:
		return( Set_Annulus(
			Parameters("KERNEL_INNER"    )->asDouble(),
			Parameters("KERNEL_RADIUS"   )->asDouble()
		));

	case SG_GRIDCELLADDR_PARM_SECTOR:
		return( Set_Sector(
			Parameters("KERNEL_RADIUS"   )->asDouble(),
			Parameters("KERNEL_DIRECTION")->asDouble(),
			Parameters("KERNEL_TOLERANCE")->asDouble()
		));
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Set_Square (CSG_Parameters &Parameters)	{	return( Set_Parameters(Parameters, SG_GRIDCELLADDR_PARM_SQUARE ) );	}
bool CSG_Grid_Cell_Addressor::Set_Circle (CSG_Parameters &Parameters)	{	return( Set_Parameters(Parameters, SG_GRIDCELLADDR_PARM_CIRCLE ) );	}
bool CSG_Grid_Cell_Addressor::Set_Annulus(CSG_Parameters &Parameters)	{	return( Set_Parameters(Parameters, SG_GRIDCELLADDR_PARM_ANNULUS) );	}
bool CSG_Grid_Cell_Addressor::Set_Sector (CSG_Parameters &Parameters)	{	return( Set_Parameters(Parameters, SG_GRIDCELLADDR_PARM_SECTOR ) );	}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Enable_Parameters(CSG_Parameters &Parameters)
{
	if( Parameters("KERNEL_TYPE") )
	{
		int	Type	= Parameters("KERNEL_TYPE")->asChoice()->Get_Item_Data(Parameters("KERNEL_TYPE")->asInt()).asInt();

		Parameters.Set_Enabled("KERNEL_INNER"    , Type == SG_GRIDCELLADDR_PARM_ANNULUS);
		Parameters.Set_Enabled("KERNEL_DIRECTION", Type == SG_GRIDCELLADDR_PARM_SECTOR );
		Parameters.Set_Enabled("KERNEL_TOLERANCE", Type == SG_GRIDCELLADDR_PARM_SECTOR );
	}

	if( Parameters("DW_WEIGHTING") )
	{
		int	Method	= Parameters("DW_WEIGHTING")->asInt();

		Parameters.Set_Enabled("DW_IDW_OFFSET", Method == 1);
		Parameters.Set_Enabled("DW_IDW_POWER" , Method == 1);
		Parameters.Set_Enabled("DW_BANDWIDTH" , Method >= 2);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define	ADD_CELL(x, y, d)	{\
	CSG_Table_Record	*pRecord	= m_Cells.Add_Record();\
	pRecord->Set_Value(0, x);\
	pRecord->Set_Value(1, y);\
	pRecord->Set_Value(2, d);\
	pRecord->Set_Value(3, m_Weighting.Get_Weight(d));\
}

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Set_Radius(double Radius, bool bSquare)
{
	Destroy();

	m_Type		= bSquare ? 1 : 0;
	m_Parms[0]	= Radius;

	//-----------------------------------------------------
	if( Radius > 0.0 )
	{
		ADD_CELL(0.0, 0.0, 0.0);

		for(double y=1.0; y<=Radius; y++)
		{
			for(double x=0.0; x<=Radius; x++)
			{
				double	d	= SG_Get_Length(x, y);

				if( bSquare || d <= Radius )
				{
					ADD_CELL( x,  y, d);
					ADD_CELL( y, -x, d);
					ADD_CELL(-x, -y, d);
					ADD_CELL(-y,  x, d);
				}
			}
		}

		//-------------------------------------------------
		if( m_Cells.Get_Count() > 0 )
		{
			m_Cells.Set_Index(2, TABLE_INDEX_Ascending);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Set_Annulus(double inner_Radius, double outer_Radius)
{
	Destroy();

	m_Type		= 2;
	m_Parms[0]	= outer_Radius;
	m_Parms[1]	= inner_Radius;

	//-----------------------------------------------------
	if( inner_Radius <= outer_Radius )
	{
		if( inner_Radius <= 0.0 )
		{
			ADD_CELL(0.0, 0.0, 0.0);
		}

		for(double y=1.0; y<=outer_Radius; y++)
		{
			for(double x=0.0; x<=outer_Radius; x++)
			{
				double	d	= SG_Get_Length(x, y);

				if( inner_Radius <= d && d <= outer_Radius )
				{
					ADD_CELL( x,  y, d);
					ADD_CELL( y, -x, d);
					ADD_CELL(-x, -y, d);
					ADD_CELL(-y,  x, d);
				}
			}
		}

		//-------------------------------------------------
		if( m_Cells.Get_Count() > 0 )
		{
			m_Cells.Set_Index(2, TABLE_INDEX_Ascending);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid_Cell_Addressor::Set_Sector(double Radius, double Direction, double Tolerance)
{
	Destroy();

	m_Type		= 3;
	m_Parms[0]	= Radius;
	m_Parms[2]	= Direction;
	m_Parms[3]	= Tolerance;

	//-----------------------------------------------------
	if( Radius > 0.0 )
	{
		TSG_Point	a, b;

		Direction	= fmod(Direction, M_PI_360); if( Direction < 0.0 ) Direction += M_PI_360;

		if( Direction < M_PI_090 )
		{
			a.x	= -0.5;	b.x	=  0.5;
			a.y	=  0.5;	b.y	= -0.5;
		}
		else if( Direction < M_PI_180 )
		{
			a.x	=  0.5;	b.x	= -0.5;
			a.y	=  0.5;	b.y	= -0.5;
		}
		else if( Direction < M_PI_270 )
		{
			a.x	=  0.5;	b.x	= -0.5;
			a.y	= -0.5;	b.y	=  0.5;
		}
		else // if( Direction < M_PI_360 )
		{
			a.x	= -0.5;	b.x	=  0.5;
			a.y	= -0.5;	b.y	=  0.5;
		}

		double	d	= 10.0 * SG_Get_Length(Radius, Radius);

		CSG_Shapes	Polygons(SHAPE_TYPE_Polygon);
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)Polygons.Add_Shape();

		pPolygon->Add_Point(b.x, b.y);
		pPolygon->Add_Point(a.x, a.y);
		pPolygon->Add_Point(a.x + d * sin(Direction - Tolerance), a.y + d * cos(Direction - Tolerance));
		pPolygon->Add_Point(      d * sin(Direction)            ,       d * cos(Direction));
		pPolygon->Add_Point(b.x + d * sin(Direction + Tolerance), a.y + d * cos(Direction + Tolerance));

		//-------------------------------------------------
		for(double y=1.0; y<=Radius; y++)
		{
			for(double x=0.0; x<=Radius; x++)
			{
				if( (d = SG_Get_Length(x, y)) <= Radius )
				{
					if( pPolygon->Contains( x,  y) ) ADD_CELL( x,  y, d);
					if( pPolygon->Contains( y, -x) ) ADD_CELL( y, -x, d);
					if( pPolygon->Contains(-x, -y) ) ADD_CELL(-x, -y, d);
					if( pPolygon->Contains(-y,  x) ) ADD_CELL(-y,  x, d);
				}
			}
		}

		//-------------------------------------------------
		if( m_Cells.Get_Count() > 0 )
		{
			m_Cells.Set_Index(2, TABLE_INDEX_Ascending);

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
