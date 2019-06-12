
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
//                     VIEW_Map.cpp                      //
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
#include <wx/dcclient.h>

#include "res_commands.h"
#include "res_controls.h"
#include "res_images.h"

#include "helper.h"

#include "active.h"

#include "wksp_layer.h"
#include "wksp_map.h"
#include "wksp_map_manager.h"

#include "view_ruler.h"
#include "view_map.h"
#include "view_map_control.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CVIEW_Map, CVIEW_Base);

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_Map, CVIEW_Base)
	EVT_PAINT			(CVIEW_Map::On_Paint)
	EVT_SIZE			(CVIEW_Map::On_Size)
	EVT_KEY_DOWN		(CVIEW_Map::On_Key_Down)

	EVT_MENU			(ID_CMD_MAP_3D_SHOW						, CVIEW_Map::On_Map_3D_Show)
	EVT_MENU			(ID_CMD_MAP_LAYOUT_SHOW					, CVIEW_Map::On_Map_Layout_Show)
	EVT_MENU			(ID_CMD_MAP_SCALEBAR					, CVIEW_Map::On_Map_ScaleBar)
	EVT_MENU			(ID_CMD_MAP_NORTH_ARROW					, CVIEW_Map::On_Map_North_Arrow)
	EVT_MENU			(ID_CMD_MAP_GRATICULE_ADD				, CVIEW_Map::On_Map_Graticule)
	EVT_MENU			(ID_CMD_MAP_BASEMAP_ADD					, CVIEW_Map::On_Map_BaseMap)
	EVT_MENU			(ID_CMD_MAP_SAVE_IMAGE					, CVIEW_Map::On_Map_Save_Image)
	EVT_MENU			(ID_CMD_MAP_SAVE_IMAGE_ON_CHANGE		, CVIEW_Map::On_Map_Save_Image_On_Change)
	EVT_MENU			(ID_CMD_MAPS_SAVE_IMAGE_TO_KMZ			, CVIEW_Map::On_Map_Save_Image_To_KMZ)
	EVT_MENU			(ID_CMD_MAPS_SAVE_IMAGE_TO_MEMORY		, CVIEW_Map::On_Map_Save_Image_To_Memory)
	EVT_MENU			(ID_CMD_MAPS_SAVE_TO_CLIPBOARD			, CVIEW_Map::On_Map_Save_Image_Clipboard)
	EVT_MENU			(ID_CMD_MAPS_SAVE_TO_CLIPBOARD_LEGEND	, CVIEW_Map::On_Map_Save_Image_ClipboardL)

	EVT_MENU			(ID_CMD_MAP_ZOOM_FULL					, CVIEW_Map::On_Map_Zoom_Full)
	EVT_MENU			(ID_CMD_MAP_ZOOM_BACK					, CVIEW_Map::On_Map_Zoom_Back)
	EVT_MENU			(ID_CMD_MAP_ZOOM_FORWARD				, CVIEW_Map::On_Map_Zoom_Forward)
	EVT_MENU			(ID_CMD_MAP_ZOOM_ACTIVE					, CVIEW_Map::On_Map_Zoom_Layer)
	EVT_MENU			(ID_CMD_MAP_ZOOM_SELECTION				, CVIEW_Map::On_Map_Zoom_Selection)
	EVT_MENU			(ID_CMD_MAP_ZOOM_EXTENT					, CVIEW_Map::On_Map_Zoom_Extent)
	EVT_MENU			(ID_CMD_MAP_SYNCHRONIZE					, CVIEW_Map::On_Map_Zoom_Synchronize)
	EVT_MENU			(ID_CMD_MAP_CROSSHAIR					, CVIEW_Map::On_Map_CrossHair)

	EVT_MENU			(ID_CMD_MAP_MODE_ZOOM					, CVIEW_Map::On_Map_Mode_Zoom)
	EVT_MENU			(ID_CMD_MAP_MODE_PAN					, CVIEW_Map::On_Map_Mode_Pan)
	EVT_MENU			(ID_CMD_MAP_MODE_SELECT					, CVIEW_Map::On_Map_Mode_Select)
	EVT_MENU			(ID_CMD_MAP_MODE_DISTANCE				, CVIEW_Map::On_Map_Mode_Distance)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Map::CVIEW_Map(CWKSP_Map *pMap, int Frame_Width)
	: CVIEW_Base(pMap, ID_VIEW_MAP, pMap->Get_Name(), ID_IMG_WND_MAP, false)
{
	SYS_Set_Color_BG_Window(this);

	m_pMap			= pMap;

	m_pControl		= new CVIEW_Map_Control(this, m_pMap);

	m_pRuler_X1		= new CVIEW_Ruler(this, RULER_HORIZONTAL|RULER_MODE_NORMAL|RULER_TICKATBOTTOM);
	m_pRuler_X2		= new CVIEW_Ruler(this, RULER_HORIZONTAL|RULER_MODE_SCALE );
	m_pRuler_Y1		= new CVIEW_Ruler(this, RULER_VERTICAL  |RULER_MODE_NORMAL|RULER_DESCENDING|RULER_TICKATBOTTOM);
	m_pRuler_Y2		= new CVIEW_Ruler(this, RULER_VERTICAL  |RULER_MODE_SCALE |RULER_DESCENDING);

	m_Ruler_Size	= 20;

	Ruler_Set_Width(Frame_Width);

	Do_Show();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CVIEW_Map::_Create_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu;

//	CMD_Menu_Add_Item(pMenu, true , ID_CMD_MAP_TOOLBAR);
//	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, true , ID_CMD_MAP_3D_SHOW);
	CMD_Menu_Add_Item(pMenu, true , ID_CMD_MAP_LAYOUT_SHOW);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, true , ID_CMD_MAP_SCALEBAR);
	CMD_Menu_Add_Item(pMenu, true , ID_CMD_MAP_NORTH_ARROW);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAP_GRATICULE_ADD);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAP_BASEMAP_ADD);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAP_SAVE_IMAGE);
//	CMD_Menu_Add_Item(pMenu, true , ID_CMD_MAP_SAVE_IMAGE_ON_CHANGE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SAVE_IMAGE_TO_MEMORY);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SAVE_IMAGE_TO_KMZ);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SAVE_TO_CLIPBOARD);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SAVE_TO_CLIPBOARD_LEGEND);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAP_ZOOM_BACK);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAP_ZOOM_FORWARD);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAP_ZOOM_FULL);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAP_ZOOM_ACTIVE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAP_ZOOM_SELECTION);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAP_ZOOM_EXTENT);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, true , ID_CMD_MAP_SYNCHRONIZE);
	CMD_Menu_Add_Item(pMenu, true , ID_CMD_MAP_CROSSHAIR);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, true , ID_CMD_MAP_MODE_SELECT);
	CMD_Menu_Add_Item(pMenu, true , ID_CMD_MAP_MODE_ZOOM);
	CMD_Menu_Add_Item(pMenu, true , ID_CMD_MAP_MODE_PAN);
	CMD_Menu_Add_Item(pMenu, true , ID_CMD_MAP_MODE_DISTANCE);

	return( pMenu );
}

//---------------------------------------------------------
wxToolBarBase * CVIEW_Map::_Create_ToolBar(void)
{
	wxToolBarBase	*pToolBar	= CMD_ToolBar_Create(ID_TB_VIEW_MAP);

	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP_ZOOM_BACK);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP_ZOOM_FORWARD);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP_ZOOM_FULL);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP_ZOOM_ACTIVE);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP_ZOOM_SELECTION);
//	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP_ZOOM_EXTENT);
	CMD_ToolBar_Add_Separator(pToolBar);
	CMD_ToolBar_Add_Item(pToolBar, true , ID_CMD_MAP_SYNCHRONIZE);
	CMD_ToolBar_Add_Item(pToolBar, true , ID_CMD_MAP_CROSSHAIR);
	CMD_ToolBar_Add_Separator(pToolBar);
	CMD_ToolBar_Add_Item(pToolBar, true , ID_CMD_MAP_MODE_SELECT);
	CMD_ToolBar_Add_Item(pToolBar, true , ID_CMD_MAP_MODE_ZOOM);
	CMD_ToolBar_Add_Item(pToolBar, true , ID_CMD_MAP_MODE_PAN);
	CMD_ToolBar_Add_Item(pToolBar, true , ID_CMD_MAP_MODE_DISTANCE);
	CMD_ToolBar_Add_Separator(pToolBar);
	CMD_ToolBar_Add_Item(pToolBar, true , ID_CMD_MAP_3D_SHOW);
	CMD_ToolBar_Add_Item(pToolBar, true , ID_CMD_MAP_LAYOUT_SHOW);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAPS_SAVE_TO_CLIPBOARD);
	CMD_ToolBar_Add_Separator(pToolBar);
	CMD_ToolBar_Add_Item(pToolBar, true , ID_CMD_MAP_SCALEBAR);
	CMD_ToolBar_Add_Item(pToolBar, true , ID_CMD_MAP_NORTH_ARROW);

	CMD_ToolBar_Add(pToolBar, _TL("Map"));

	return( pToolBar );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map::Do_Update(void)
{
	m_pControl->Refresh_Map();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map::On_Paint(wxPaintEvent &event)
{
	if( m_Ruler_Size > 1 )
	{
		wxPaintDC	dc(this);
		wxRect		rClient(wxPoint(0, 0), GetClientSize()), rMap(rClient);

		rMap.Deflate(m_Ruler_Size - 1);

		dc.DrawLine(rMap   .GetLeft (), rClient.GetTop   (), rMap   .GetLeft (), rClient.GetBottom());
		dc.DrawLine(rMap   .GetRight(), rClient.GetTop   (), rMap   .GetRight(), rClient.GetBottom());
		dc.DrawLine(rClient.GetLeft (), rMap   .GetTop   (), rClient.GetRight(), rMap   .GetTop   ());
		dc.DrawLine(rClient.GetLeft (), rMap   .GetBottom(), rClient.GetRight(), rMap   .GetBottom());
	}

	event.Skip();
}

//---------------------------------------------------------
void CVIEW_Map::On_Size(wxSizeEvent &event)
{
	int		minSize	= 2 * m_Ruler_Size + 10;
	wxSize	Size(GetClientSize()), fSize(GetSize().x - Size.x, GetSize().y - Size.y);

	if( Size.x >= minSize && Size.y >= minSize )
	{
		_Set_Positions();

		event.Skip();
	}
}

//---------------------------------------------------------
void CVIEW_Map::_Set_Positions(void)
{
	wxRect	r(wxPoint(0, 0), GetClientSize());

	if( m_Ruler_Size > 0 )
	{
		r.Deflate(m_Ruler_Size);

		m_pRuler_X1	->SetSize(wxRect(r.GetLeft()     , 0                , r.GetWidth(), m_Ruler_Size - 1));
		m_pRuler_X2	->SetSize(wxRect(r.GetLeft()     , 2 + r.GetBottom(), r.GetWidth(), m_Ruler_Size - 1));

		m_pRuler_Y1	->SetSize(wxRect(0               , r.GetTop()       , m_Ruler_Size - 1, r.GetHeight()));
		m_pRuler_Y2	->SetSize(wxRect(2 + r.GetRight(), r.GetTop()       , m_Ruler_Size - 1, r.GetHeight()));

		Refresh();
	}

	m_pControl	->SetSize(r);
}

//---------------------------------------------------------
void CVIEW_Map::On_Key_Down(wxKeyEvent &event)
{
	if( m_pControl )
	{
		m_pControl->On_Key_Down(event);
	}
	else
	{
		event.Skip();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map::Ruler_Set_Width(int Width)
{
	if( m_Ruler_Size != Width )
	{
		Set_Size_Min(10 + 2 * Width, 10 + 2 * Width);

		m_Ruler_Size	= Width;

		if( m_Ruler_Size > 0 )
		{
			m_pRuler_X1->Show(true);
			m_pRuler_X2->Show(true);
			m_pRuler_Y1->Show(true);
			m_pRuler_Y2->Show(true);

			m_pControl->SetWindowStyleFlag(wxFULL_REPAINT_ON_RESIZE);
		}
		else
		{
			m_pRuler_X1->Show(false);
			m_pRuler_X2->Show(false);
			m_pRuler_Y1->Show(false);
			m_pRuler_Y2->Show(false);

			m_pControl->SetWindowStyleFlag(wxSUNKEN_BORDER|wxFULL_REPAINT_ON_RESIZE);
		}

		_Set_Positions();
	}
}

//---------------------------------------------------------
void CVIEW_Map::Ruler_Set_Position(int x, int y)
{
	if( m_Ruler_Size > 0 )
	{
		m_pRuler_X1->Set_Position(x);
		m_pRuler_X2->Set_Position(x);
		m_pRuler_Y1->Set_Position(y);
		m_pRuler_Y2->Set_Position(y);
	}
}

//---------------------------------------------------------
void CVIEW_Map::Ruler_Refresh(void)
{
	if( m_Ruler_Size > 0 )
	{
		CSG_Rect	rWorld(m_pMap->Get_World(m_pControl->GetRect()));

		m_pRuler_X2->Set_Mode(m_pMap->is_ScaleBar() ? RULER_MODE_NORMAL : RULER_MODE_SCALE);
		m_pRuler_Y2->Set_Mode(m_pMap->is_ScaleBar() ? RULER_MODE_NORMAL : RULER_MODE_SCALE);

		m_pRuler_X1->Set_Range(rWorld.Get_XMin(), rWorld.Get_XMax());
		m_pRuler_X2->Set_Range(rWorld.Get_XMin(), rWorld.Get_XMax());
		m_pRuler_Y1->Set_Range(rWorld.Get_YMin(), rWorld.Get_YMax());
		m_pRuler_Y2->Set_Range(rWorld.Get_YMin(), rWorld.Get_YMax());
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	case ID_CMD_MAP_3D_SHOW:
		event.Check(m_pMap->View_3D_Get() != NULL);
		break;

	case ID_CMD_MAP_LAYOUT_SHOW:
		event.Check(m_pMap->View_Layout_Get() != NULL);
		break;

	case ID_CMD_MAP_SCALEBAR:
		event.Check(m_pMap->is_ScaleBar());
		break;

	case ID_CMD_MAP_NORTH_ARROW:
		event.Check(m_pMap->is_North_Arrow());
		break;

	case ID_CMD_MAP_SYNCHRONIZE:
		event.Enable(m_pMap->Get_Manager()->Get_Count() > 1);
		event.Check (m_pMap->is_Synchronising());
		break;

	case ID_CMD_MAP_SAVE_IMAGE_ON_CHANGE:
		event.Check(m_pMap->is_Image_Save_Mode());
		break;

	case ID_CMD_MAP_MODE_ZOOM:
		event.Check(m_pControl->Get_Mode() == MAP_MODE_ZOOM);
		break;

	case ID_CMD_MAP_MODE_PAN:
		event.Check(m_pControl->Get_Mode() == MAP_MODE_PAN || m_pControl->Get_Mode() == MAP_MODE_PAN_DOWN);
		break;

	case ID_CMD_MAP_MODE_SELECT:
		event.Check(m_pControl->Get_Mode() == MAP_MODE_SELECT);
		break;

	case ID_CMD_MAP_MODE_DISTANCE:
		event.Check(m_pControl->Get_Mode() == MAP_MODE_DISTANCE);
		break;

	case ID_CMD_MAP_ZOOM_ACTIVE:
		event.Enable(g_pActive->Get_Active_Layer() != NULL);
		break;

	case ID_CMD_MAP_ZOOM_SELECTION:
		event.Enable(g_pActive->Get_Active_Layer()
			&& g_pActive->Get_Active_Layer()->Get_Object()->asShapes()
			&& g_pActive->Get_Active_Layer()->Get_Object()->asShapes()->Get_Selection_Count() > 0
		);
		break;

	case ID_CMD_MAP_ZOOM_BACK:
		event.Enable(m_pMap->Set_Extent_Back(true));
		break;

	case ID_CMD_MAP_ZOOM_FORWARD:
		event.Enable(m_pMap->Set_Extent_Forward(true));
		break;

	case ID_CMD_MAP_CROSSHAIR:
		event.Check(g_pMaps->is_CrossHair());
		break;

	case ID_CMD_MAP_GRATICULE_ADD:
	case ID_CMD_MAP_BASEMAP_ADD:
		event.Enable(m_pMap->Get_Count() > 0 && m_pMap->Get_Projection().is_Okay());
		break;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map::On_Map_3D_Show(wxCommandEvent &event)
{
	m_pMap->View_3D_Toggle();
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_Layout_Show(wxCommandEvent &event)
{
	m_pMap->View_Layout_Toggle();
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_Save_Image(wxCommandEvent &event)
{
	m_pMap->SaveAs_Image();
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_Save_Image_On_Change(wxCommandEvent &event)
{
	m_pMap->SaveAs_Image_On_Change();
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_Save_Image_To_KMZ(wxCommandEvent &event)
{
	m_pMap->SaveAs_Image_To_KMZ(m_pControl->GetClientSize().x, m_pControl->GetClientSize().y);
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_Save_Image_To_Memory(wxCommandEvent &event)
{
	m_pMap->SaveAs_Image_To_Memory(m_pControl->GetClientSize().x, m_pControl->GetClientSize().y);
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_Save_Image_Clipboard(wxCommandEvent &event)
{
	m_pMap->SaveAs_Image_Clipboard(false);
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_Save_Image_ClipboardL(wxCommandEvent &event)
{
	m_pMap->SaveAs_Image_Clipboard(true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map::On_Map_Zoom_Full(wxCommandEvent &event)
{
	m_pMap->Set_Extent_Full();
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_Zoom_Back(wxCommandEvent &event)
{
	m_pMap->Set_Extent_Back();
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_Zoom_Forward(wxCommandEvent &event)
{
	m_pMap->Set_Extent_Forward();
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_Zoom_Layer(wxCommandEvent &event)
{
	m_pMap->Set_Extent_Active();
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_Zoom_Selection(wxCommandEvent &event)
{
	m_pMap->Set_Extent_Selection();
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_Zoom_Extent(wxCommandEvent &event)
{
	m_pMap->Set_Extent();
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_ScaleBar(wxCommandEvent &event)
{
	m_pMap->Set_ScaleBar(!m_pMap->is_ScaleBar());
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_North_Arrow(wxCommandEvent &event)
{
	m_pMap->Set_North_Arrow(!m_pMap->is_North_Arrow());
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_Graticule(wxCommandEvent &event)
{
	m_pMap->Add_Graticule();
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_BaseMap(wxCommandEvent &event)
{
	m_pMap->Add_BaseMap();
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_Zoom_Synchronize(wxCommandEvent &event)
{
	m_pMap->Set_Synchronising(!m_pMap->is_Synchronising());
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_CrossHair(wxCommandEvent &event)
{
	g_pMaps->Toggle_CrossHair();
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_Mode_Zoom(wxCommandEvent &event)
{
	m_pControl->Set_Mode(MAP_MODE_ZOOM);
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_Mode_Pan(wxCommandEvent &event)
{
	m_pControl->Set_Mode(MAP_MODE_PAN);
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_Mode_Select(wxCommandEvent &event)
{
	m_pControl->Set_Mode(MAP_MODE_SELECT);
}

//---------------------------------------------------------
void CVIEW_Map::On_Map_Mode_Distance(wxCommandEvent &event)
{
	m_pControl->Set_Mode(MAP_MODE_DISTANCE);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
