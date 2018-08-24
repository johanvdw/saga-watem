/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  VIEW_Map_Control.h                   //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Map_Control_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Map_Control_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/panel.h>
#include <wx/bitmap.h>

#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	MAP_MODE_NONE	= 0,
	MAP_MODE_SELECT,
	MAP_MODE_DISTANCE,
	MAP_MODE_ZOOM,
	MAP_MODE_PAN,
	MAP_MODE_PAN_DOWN
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Map_Control : public wxPanel
{
public:
	CVIEW_Map_Control(class CVIEW_Map *pParent, class CWKSP_Map *pMap);
	virtual ~CVIEW_Map_Control(void);

	void						On_Paint			(wxPaintEvent &event);
	void						On_Size				(wxSizeEvent  &event);
	void						On_Key_Down			(wxKeyEvent   &event);

	void						On_Mouse_LDown		(wxMouseEvent &event);
	void						On_Mouse_LUp		(wxMouseEvent &event);
	void						On_Mouse_LDClick	(wxMouseEvent &event);
	void						On_Mouse_RDown		(wxMouseEvent &event);
	void						On_Mouse_RUp		(wxMouseEvent &event);
	void						On_Mouse_RDClick	(wxMouseEvent &event);
	void						On_Mouse_MDown		(wxMouseEvent &event);
	void						On_Mouse_MUp		(wxMouseEvent &event);
	void						On_Mouse_Motion		(wxMouseEvent &event);
	void						On_Mouse_Wheel		(wxMouseEvent &event);

	void						On_Mouse_Lost		(wxMouseCaptureLostEvent &event);

	void						Set_CrossHair		(const TSG_Point &Point);
	void						Set_CrossHair_Off	(void);

	bool						Set_Mode			(int Mode);
	int							Get_Mode			(void)	{	return( m_Mode );	}

	void						Refresh_Map			(void);


private:

	int							m_Mode, m_Drag_Mode;

	double						m_Distance, m_Distance_Move;

	CSG_Points					m_Distance_Pts;

	wxPoint						m_Mouse_Down, m_Mouse_Move, m_CrossHair;
	
	wxBitmap					m_Bitmap;

	class CVIEW_Map				*m_pParent;

	class CWKSP_Map				*m_pMap;


	void						_Set_StatusBar		(const TSG_Point &Point);

	wxPoint						_Get_World2Client	(const TSG_Point &Point);
	CSG_Point					_Get_Client2World	(const   wxPoint &Point);
	double						_Get_Client2World	(double Length);

	bool						_Draw_CrossHair		(const wxPoint &Point);

	void						_Draw_Inverse		(const wxPoint &A, const wxPoint &B);
	void						_Draw_Inverse		(const wxPoint &A, const wxPoint &B_Old, const wxPoint &B_New);

	bool						_Zoom				(const wxPoint &A, const wxPoint &B);
	bool						_Zoom				(const CSG_Point &Center, bool bZoomIn);
	bool						_Pan				(const wxPoint &A, const wxPoint &B);
	bool						_Move				(      wxPoint &A, const wxPoint &B);
	bool						_Move				(const wxPoint &Move);

	void						_Distance_Reset		(void);
	void						_Distance_Add		(const wxPoint & Point);
	void						_Distance_Draw		(wxDC &dc);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Map_Control_H
