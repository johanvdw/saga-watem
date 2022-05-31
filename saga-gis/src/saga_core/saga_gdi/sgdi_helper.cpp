
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_GDI                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    sgdi_helper.cpp                    //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/wxprec.h>
#include <wx/settings.h>
#include <wx/dc.h>

//---------------------------------------------------------
#include "sgdi_helper.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Draw_Edge(wxDC &dc, int Edge_Style, int ax, int ay, int bx, int by)
{
	wxPen	oldPen(dc.GetPen());

	switch( Edge_Style )
	{
	//-----------------------------------------------------
	case EDGE_STYLE_SIMPLE:
		dc.DrawLine(bx, ay, bx, by);
		dc.DrawLine(bx, by, ax, by);
		dc.DrawLine(ax, ay, bx, ay);
		dc.DrawLine(ax, by, ax, ay);
		break;

	//-----------------------------------------------------
	case EDGE_STYLE_STATIC:
		Draw_Edge(dc, EDGE_STYLE_SUNKEN, ax + 0, ay + 0, bx - 0, by - 0);
		Draw_Edge(dc, EDGE_STYLE_RAISED, ax + 1, ay + 1, bx - 1, by - 1);
		break;

	//-----------------------------------------------------
	case EDGE_STYLE_SUNKEN:
		oldPen	= dc.GetPen();

		dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT), 0, wxPENSTYLE_SOLID));
		dc.DrawLine(bx, ay, bx, by);
		dc.DrawLine(bx, by, ax, by);

		dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW   ), 0, wxPENSTYLE_SOLID));
		dc.DrawLine(ax, by, ax, ay);
		dc.DrawLine(ax, ay, bx, ay);

		dc.SetPen(oldPen);
		break;

	//-----------------------------------------------------
	case EDGE_STYLE_RAISED:
		dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW   ), 0, wxPENSTYLE_SOLID));
		dc.DrawLine(bx, ay, bx, by);
		dc.DrawLine(bx, by, ax, by);

		dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT), 0, wxPENSTYLE_SOLID));
		dc.DrawLine(ax, by, ax, ay);
		dc.DrawLine(ax, ay, bx, ay);
		break;
	}

	dc.SetPen(oldPen);
}

//---------------------------------------------------------
void		Draw_Edge(wxDC &dc, int Edge_Style, wxRect r)
{
	Draw_Edge(dc, Edge_Style, r.GetLeft(), r.GetTop(), r.GetRight(), r.GetBottom());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Draw_Rect(wxDC &dc, wxColour Color, int ax, int ay, int bx, int by)
{
	Draw_FillRect(dc, Color, ax, ay, bx, by);

	Draw_Edge(dc, EDGE_STYLE_SIMPLE, ax, ay, bx, by);
}

//---------------------------------------------------------
void		Draw_Rect(wxDC &dc, wxColour Color, wxRect r)
{
	Draw_Rect(dc, Color, r.GetLeft(), r.GetTop(), r.GetRight(), r.GetBottom());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Draw_FillRect(wxDC &dc, wxColour Color, int ax, int ay, int bx, int by)
{
	if( ax > bx )
	{
		int i = ax; ax = bx; bx = i;
	}

	if( ay > by )
	{
		int i = ay; ay = by; by = i;
	}

	//-----------------------------------------------------
	wxPen oldPen(dc.GetPen()); dc.SetPen(wxPen(Color));

	if( bx - ax < by - ay )
	{
		for(int i=ax; i<bx; i++)
		{
			dc.DrawLine(i, ay, i, by);
		}
	}
	else
	{
		for(int i=ay; i<by; i++)
		{
			dc.DrawLine(ax, i, bx, i);
		}
	}

	//-----------------------------------------------------
	dc.SetPen(oldPen);
}

//---------------------------------------------------------
void		Draw_FillRect(wxDC &dc, wxColour Color, wxRect r)
{
	Draw_FillRect(dc, Color, r.GetLeft(), r.GetTop(), r.GetRight(), r.GetBottom());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Draw_Text(wxDC &dc, int Align, int x, int y, const wxString &Text)
{
	if( Align != TEXTALIGN_TOPLEFT )
	{
		wxCoord	xSize, ySize;

		dc.GetTextExtent(Text, &xSize, &ySize);

		//-------------------------------------------------
		if		( Align & TEXTALIGN_XCENTER )
		{
			x	-= xSize / 2;
		}
		else if	( Align & TEXTALIGN_RIGHT )
		{
			x	-= xSize;
		}

		//-------------------------------------------------
		if		( Align & TEXTALIGN_YCENTER )
		{
			y	-= ySize / 2;
		}
		else if	( Align & TEXTALIGN_BOTTOM )
		{
			y	-= ySize;
		}
	}

	dc.DrawText(Text, x, y);
}

//---------------------------------------------------------
void		Draw_Text(wxDC &dc, int Align, int x, int y, double Angle, const wxString &Text)
{
	if( Angle == 0. )
	{
		Draw_Text(dc, Align, x, y, Text);

		return;
	}

	if( Align != TEXTALIGN_TOPLEFT )
	{
		wxCoord	xSize, ySize;	double	d;

		dc.GetTextExtent(Text, &xSize, &ySize);

		//-------------------------------------------------
		d	 = M_DEG_TO_RAD * Angle;

		if		( Align & TEXTALIGN_XCENTER )
		{
			x	-= (int)(xSize * cos(d) / 2.);
			y	+= (int)(xSize * sin(d) / 2.);
		}
		else if	( Align & TEXTALIGN_RIGHT )
		{
			x	-= (int)(xSize * cos(d));
			y	+= (int)(xSize * sin(d));
		}

		//-------------------------------------------------
		d	 = M_DEG_TO_RAD * (Angle - 90.);

		if		( Align & TEXTALIGN_YCENTER )
		{
			x	-= (int)(ySize * cos(d) / 2.);
			y	+= (int)(ySize * sin(d) / 2.);
		}
		else if	( Align & TEXTALIGN_BOTTOM )
		{
			x	-= (int)(ySize * cos(d));
			y	+= (int)(ySize * sin(d));
		}
	}

	dc.DrawRotatedText(Text, x, y, Angle);
}

//---------------------------------------------------------
void			Draw_Text			(wxDC &dc, int Align, int x, int y, const wxString &Text, int Effect, wxColour Effect_Color, int Effect_Size)
{
	Draw_Text(dc, Align, x, y, 0., Text, Effect, Effect_Color);
}

void			Draw_Text			(wxDC &dc, int Align, int x, int y, double Angle, const wxString &Text, int Effect, wxColour Effect_Color, int Effect_Size)
{
	if( Effect != TEXTEFFECT_NONE )
	{
		wxColour	oldColor	= dc.GetTextForeground();	dc.SetTextForeground(Effect_Color);

		if( Effect_Size <= 1 )
		{
			int	d	= 1;

			if( Effect & TEXTEFFECT_TOP         )	Draw_Text(dc, Align, x    , y - d, Angle, Text);
			if( Effect & TEXTEFFECT_TOPLEFT     )	Draw_Text(dc, Align, x - d, y - d, Angle, Text);
			if( Effect & TEXTEFFECT_LEFT        )	Draw_Text(dc, Align, x - d, y    , Angle, Text);
			if( Effect & TEXTEFFECT_BOTTOMLEFT  )	Draw_Text(dc, Align, x - d, y + d, Angle, Text);
			if( Effect & TEXTEFFECT_BOTTOM      )	Draw_Text(dc, Align, x    , y + d, Angle, Text);
			if( Effect & TEXTEFFECT_BOTTOMRIGHT )	Draw_Text(dc, Align, x + d, y + d, Angle, Text);
			if( Effect & TEXTEFFECT_RIGHT       )	Draw_Text(dc, Align, x + d, y    , Angle, Text);
			if( Effect & TEXTEFFECT_TOPRIGHT    )	Draw_Text(dc, Align, x + d, y - d, Angle, Text);
		}
		else if( Effect != TEXTEFFECT_FRAME )
		{
			for(int d=1; d<=Effect_Size; d++)
			{
				if( Effect & TEXTEFFECT_TOP         )	Draw_Text(dc, Align, x    , y - d, Angle, Text);
				if( Effect & TEXTEFFECT_TOPLEFT     )	Draw_Text(dc, Align, x - d, y - d, Angle, Text);
				if( Effect & TEXTEFFECT_LEFT        )	Draw_Text(dc, Align, x - d, y    , Angle, Text);
				if( Effect & TEXTEFFECT_BOTTOMLEFT  )	Draw_Text(dc, Align, x - d, y + d, Angle, Text);
				if( Effect & TEXTEFFECT_BOTTOM      )	Draw_Text(dc, Align, x    , y + d, Angle, Text);
				if( Effect & TEXTEFFECT_BOTTOMRIGHT )	Draw_Text(dc, Align, x + d, y + d, Angle, Text);
				if( Effect & TEXTEFFECT_RIGHT       )	Draw_Text(dc, Align, x + d, y    , Angle, Text);
				if( Effect & TEXTEFFECT_TOPRIGHT    )	Draw_Text(dc, Align, x + d, y - d, Angle, Text);
			}
		}
		else
		{
			for(int dy=y-Effect_Size; dy<=y+Effect_Size; dy++)
			{
				for(int dx=x-Effect_Size; dx<=x+Effect_Size; dx++)
				{
					Draw_Text(dc, Align, dx, dy, Angle, Text);
				}
			}
		}

		dc.SetTextForeground(oldColor);
	}

	Draw_Text(dc, Align, x, y, Angle, Text);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define TEXTSPACE	6

//---------------------------------------------------------
void		Draw_Scale(wxDC &dc, const wxRect &r, double zMin, double zMax, int Orientation, int Tick, int Style, const wxString &Unit, bool bUseDCFont)
{
	//-----------------------------------------------------
	int	Width	= Orientation != SCALE_VERTICAL ? r.GetWidth() : r.GetHeight();
	int	Height	= Orientation != SCALE_VERTICAL ? r.GetHeight() : r.GetWidth();

	if( zMin >= zMax || Width < 5 || Height < 5 )
	{
		return;
	}

	//-----------------------------------------------------
	if( Style & SCALE_STYLE_GLOOMING )
	{
		Style	^= SCALE_STYLE_GLOOMING;

		wxRect	rTmp;

		dc.SetPen     (wxPen(*wxWHITE));
		dc.SetTextForeground(*wxWHITE);

		rTmp	= r; rTmp.Offset( 0,  1); Draw_Scale(dc, rTmp, zMin, zMax, Orientation, Tick, Style, Unit);
		rTmp	= r; rTmp.Offset( 1,  1); Draw_Scale(dc, rTmp, zMin, zMax, Orientation, Tick, Style, Unit);
		rTmp	= r; rTmp.Offset( 1,  0); Draw_Scale(dc, rTmp, zMin, zMax, Orientation, Tick, Style, Unit);
		rTmp	= r; rTmp.Offset( 1, -1); Draw_Scale(dc, rTmp, zMin, zMax, Orientation, Tick, Style, Unit);
		rTmp	= r; rTmp.Offset( 0, -1); Draw_Scale(dc, rTmp, zMin, zMax, Orientation, Tick, Style, Unit);
		rTmp	= r; rTmp.Offset(-1, -1); Draw_Scale(dc, rTmp, zMin, zMax, Orientation, Tick, Style, Unit);
		rTmp	= r; rTmp.Offset(-1,  0); Draw_Scale(dc, rTmp, zMin, zMax, Orientation, Tick, Style, Unit);
		rTmp	= r; rTmp.Offset(-1,  1); Draw_Scale(dc, rTmp, zMin, zMax, Orientation, Tick, Style, Unit);

		dc.SetPen     (wxPen(*wxBLACK));
		dc.SetTextForeground(*wxBLACK);
	}

	//-----------------------------------------------------
	wxPen	oldPen  (dc.GetPen  ());
	wxBrush	oldBrush(dc.GetBrush());
	wxFont	oldFont (dc.GetFont ());

	if( bUseDCFont )
	{
		wxFont Font(dc.GetFont());
		Font.SetPointSize((int)((Tick == SCALE_TICK_NONE ? 0.60 : 0.45) * (double)Height));
		dc.SetFont(Font);
	}
	else
	{
		dc.SetFont(wxFont((int)((Tick == SCALE_TICK_NONE ? 0.60 : 0.45) * (double)Height),
			wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL
		));
	}

	//-----------------------------------------------------
	double	zToDC		= (double)Width / (zMax - zMin);
	double	dz			= pow(10., floor(log10(zMax - zMin)) - 1.);
	int		Decimals	= dz >= 1. ? 0 : (int)floor(-log10(dz));

	int		h	= dc.GetTextExtent(wxString::Format("%.*f", Decimals, zMax)).GetWidth();

	while( zToDC * dz < h + TEXTSPACE )	{	dz	*= 2;	}

	//-----------------------------------------------------
	int		Align, x[2], y[2];	h	= 0;

	switch( Tick )
	{
	default               :	Align = TEXTALIGN_TOPLEFT     ;	break;
	case SCALE_TICK_TOP   :	Align = TEXTALIGN_TOPCENTER   ;	break;
	case SCALE_TICK_BOTTOM:	Align = TEXTALIGN_BOTTOMCENTER;	break;
	}

	if( Orientation != SCALE_VERTICAL )
	{
		x[0]	= -1;	x[1]	= (int)((Tick == SCALE_TICK_NONE ? 1.00 : 0.30) * (double)Height);
		y[0]	= r.GetTop () + (Tick != SCALE_TICK_BOTTOM ? 0    : Height);
		y[1]	= r.GetTop () + (Tick != SCALE_TICK_BOTTOM ? x[1] : Height - x[1]);
	}
	else // if( Orientation == SCALE_VERTICAL )
	{
		y[0]	= -1;	y[1]	= (int)((Tick == SCALE_TICK_NONE ? 1.00 : 0.30) * (double)Height);
		x[0]	= r.GetLeft() + (Tick != SCALE_TICK_BOTTOM ? 0    : Height);
		x[1]	= r.GetLeft() + (Tick != SCALE_TICK_BOTTOM ? y[1] : Height - y[1]);
	}

	//-----------------------------------------------------
	for(double z=dz*floor(zMin/dz); !std::isinf(z) && z<=zMax; z+=dz)
	{
		if( Tick == SCALE_TICK_NONE || z >= zMin )
		{
			int	zDC	= Style & SCALE_STYLE_DESCENDENT
				? Width - 1 - (int)(zToDC * (z - zMin))
				:             (int)(zToDC * (z - zMin));

			if( Orientation != SCALE_VERTICAL )
			{
				int	xLast	= x[0];	x[0] = x[1] = r.GetLeft() + zDC;

				if( Style & SCALE_STYLE_BLACKWHITE && xLast >= 0 )
				{
					dc.SetBrush(h++ % 2 ? *wxWHITE : *wxBLACK);

					dc.DrawRectangle(xLast, y[0], x[1] - xLast, y[1] - y[0]);
				}
			}
			else // if( Orientation == SCALE_VERTICAL )
			{
				int	yLast	= y[0]; y[0] = y[1] = r.GetTop () + zDC;

				if( Style & SCALE_STYLE_BLACKWHITE && yLast >= 0 )
				{
					dc.SetBrush(h++ % 2 ? *wxWHITE : *wxBLACK);

					dc.DrawRectangle(x[0], yLast, x[1] - x[0], y[1] - yLast);
				}
			}

			if( !(Style & SCALE_STYLE_BLACKWHITE) )
			{
				dc.DrawLine(x[0], y[0], x[1], y[1]);
			}

			Draw_Text(dc, Align, x[1], y[1], Orientation != SCALE_VERTICAL ? 0 : 90,
				wxString::Format("%.*f", Decimals, z)
			);
		}
	}

	//-----------------------------------------------------
	if( Orientation != SCALE_VERTICAL )
	{
		if( Style & SCALE_STYLE_LINECONN )
		{
			dc.DrawLine(r.GetLeft(), y[0], x[1], y[0]);
		}

		if( !Unit.IsEmpty() )
		{
			if( Style & SCALE_STYLE_UNIT_BELOW )
			{
				Draw_Text(dc, TEXTALIGN_TOPCENTER   , r.GetLeft() + r.GetWidth() / 2, r.GetBottom(), Unit);
			}
			else // if( Style & SCALE_STYLE_UNIT_ABOVE )
			{
				Draw_Text(dc, TEXTALIGN_BOTTOMCENTER, r.GetLeft() + r.GetWidth() / 2, r.GetTop   (), Unit);
			}
		}
	}
	else
	{
		if( Style & SCALE_STYLE_LINECONN )
		{
			dc.DrawLine(x[0], r.GetTop(), x[0], y[1]);
		}

		if( !Unit.IsEmpty() )
		{
			if( Style & SCALE_STYLE_UNIT_BELOW )
			{
				Draw_Text(dc, TEXTALIGN_TOPCENTER   , r.GetRight(), r.GetTop() + r.GetHeight() / 2, 90., Unit);
			}
			else // if( Style & SCALE_STYLE_UNIT_ABOVE )
			{
				Draw_Text(dc, TEXTALIGN_BOTTOMCENTER, r.GetLeft (), r.GetTop() + r.GetHeight() / 2, 90., Unit);
			}
		}
	}

	//-----------------------------------------------------
	dc.SetPen  (oldPen  );
	dc.SetBrush(oldBrush);
	dc.SetFont (oldFont );
}

//---------------------------------------------------------
void		Draw_Scale(wxDC &dc, const wxRect &r, double zMin, double zMax, bool bHorizontal, bool bAscendent, bool bTickAtTop)
{
	Draw_Scale(dc, r, zMin, zMax, bHorizontal ? SCALE_HORIZONTAL : SCALE_VERTICAL, bTickAtTop ? SCALE_TICK_TOP : SCALE_TICK_BOTTOM, bAscendent ? SCALE_STYLE_DEFAULT : SCALE_STYLE_DESCENDENT);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define RULER_TEXT_SPACE	4

//---------------------------------------------------------
bool		Draw_Ruler(wxDC &dc, const wxRect &r, bool bHorizontal, double zMin, double zMax, bool bAscendent, int FontSize, const wxColour &Colour)
{
	if( zMin < zMax && r.GetWidth() > 0 && r.GetHeight() > 0 )
	{
		dc.SetPen(wxPen(Colour));
		dc.SetFont(wxFont(7, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

		double	Width	= bHorizontal ? r.GetWidth() : r.GetHeight();

		int		xMin	= r.GetX();
		int		xMax	= r.GetX() + r.GetWidth();
		int		yMin	= r.GetY() + r.GetHeight();
		int		yMax	= r.GetY();

		//-------------------------------------------------
		double	zToDC	= (double)Width / (zMax - zMin);

		double	dz		= pow(10., floor(log10(zMax - zMin)) - 1.);
		int	Decimals	= dz >= 1. ? 0 : (int)fabs(log10(dz));

		wxSize	Extent	= dc.GetTextExtent(wxString::Format("%.*f", Decimals, zMax));
		int		dyFont	= RULER_TEXT_SPACE + Extent.y;
		int		dxFont	= RULER_TEXT_SPACE;

		double	zDC		= 2 * Extent.x;
		while( zToDC * dz < zDC + RULER_TEXT_SPACE )
		{
			dz	*= 2;
		}

		//-------------------------------------------------
		double	z		= dz * floor(zMin / dz);	if( z < zMin )	z	+= dz;

		for(; z<=zMax; z+=dz)
		{
			zDC	= bAscendent ? zToDC * (z - zMin) : Width - zToDC * (z - zMin);

			if( bHorizontal )
			{
				int	zPos	= (int)(xMin + zDC);
				dc.DrawLine(zPos, yMin, zPos, yMax);
				dc.DrawText(wxString::Format("%.*f", Decimals, z), zPos + dxFont, yMin - dyFont);
			}
			else
			{
				int	zPos	= (int)(yMin - zDC);
				dc.DrawLine(xMin, zPos, xMax, zPos);
				dc.DrawText(wxString::Format("%.*f", Decimals, z), xMin + dxFont, zPos - dyFont);
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
