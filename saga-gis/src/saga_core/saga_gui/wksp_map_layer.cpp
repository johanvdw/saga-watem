
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
//                  WKSP_Map_Layer.cpp                   //
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
#include <wx/window.h>

#include <saga_api/saga_api.h>

#include "res_commands.h"

#include "wksp_data_manager.h"
#include "wksp_layer.h"
#include "wksp_layer_classify.h"
#include "wksp_map.h"
#include "wksp_map_layer.h"

#include "wksp_grid.h"
#include "wksp_grids.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Map_Layer::CWKSP_Map_Layer(CWKSP_Layer *pLayer)
{
	m_pLayer		= pLayer;

	m_bShow			= true;
	m_bProject		= false;
	m_bFitColors	= false;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Map_Layer::Get_Name(void)
{
	return( m_bShow ? m_pLayer->Get_Name() : wxString::Format("[%s]", m_pLayer->Get_Name().c_str()) );
}

//---------------------------------------------------------
wxString CWKSP_Map_Layer::Get_Description(void)
{
	return( m_pLayer->Get_Description() );
}

//---------------------------------------------------------
wxMenu * CWKSP_Map_Layer::Get_Menu(void)
{
	wxMenu	*pMenu, *pMenu_Edit;

	pMenu	= new wxMenu(m_pLayer->Get_Name());

	//-----------------------------------------------------
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_LAYER_SHOW);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_TOP);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_UP);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_DOWN);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_BOTTOM);

	//-----------------------------------------------------
	switch( m_pLayer->Get_Type() )
	{
	default:
		break;

	case WKSP_ITEM_Grid:
	case WKSP_ITEM_Grids:
		pMenu->AppendSeparator();
		CMD_Menu_Add_Item(pMenu, true, ID_CMD_MAPS_GRID_FITCOLORS);
		break;
	}

	//-----------------------------------------------------
	CSG_Projection	prj_Layer, prj_Map;

	if( _Get_Projections(prj_Layer, prj_Map) )
	{
		CMD_Menu_Add_Item(pMenu, true, ID_CMD_MAPS_PROJECT);
	}

	//-----------------------------------------------------
	if( (pMenu_Edit = m_pLayer->Edit_Get_Menu()) != NULL )
	{
		pMenu->AppendSeparator();
		pMenu->Append(ID_CMD_WKSP_FIRST, _TL("Edit"), pMenu_Edit);
	}

	//-----------------------------------------------------
	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Layer::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Item::On_Command(Cmd_ID) );

	case ID_CMD_SHAPES_EDIT_SHAPE:
	case ID_CMD_SHAPES_EDIT_ADD_SHAPE:
	case ID_CMD_SHAPES_EDIT_DEL_SHAPE:
	case ID_CMD_SHAPES_EDIT_ADD_PART:
	case ID_CMD_SHAPES_EDIT_DEL_PART:
	case ID_CMD_SHAPES_EDIT_DEL_POINT:
	case ID_CMD_SHAPES_EDIT_SEL_CLEAR:
	case ID_CMD_SHAPES_EDIT_SEL_INVERT:
		return( m_pLayer->On_Command(Cmd_ID) );

	case ID_CMD_WKSP_ITEM_RETURN:
	case ID_CMD_MAPS_LAYER_SHOW:
		m_bShow	= !m_bShow;
		((wxTreeCtrl *)Get_Control())->SetItemText(GetId(), Get_Name());
		((CWKSP_Map *)Get_Manager())->View_Refresh(true);
		break;

	case ID_CMD_MAPS_MOVE_TOP:
		if( Get_Manager()->Move_Top(this) )
			((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		break;

	case ID_CMD_MAPS_MOVE_BOTTOM:
		if( Get_Manager()->Move_Bottom(this) )
			((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		break;

	case ID_CMD_MAPS_MOVE_UP:
		if( Get_Manager()->Move_Up(this) )
			((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		break;

	case ID_CMD_MAPS_MOVE_DOWN:
		if( Get_Manager()->Move_Down(this) )
			((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		break;

	case ID_CMD_MAPS_GRID_FITCOLORS:
		if( (m_bFitColors = !m_bFitColors) == true )
		{
			Fit_Colors(((CWKSP_Map *)Get_Manager())->Get_Extent());
		}
		break;

	case ID_CMD_MAPS_PROJECT:
		if( (m_bProject = !m_bProject) == true )
		{
			((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		}
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Map_Layer::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Base_Item::On_Command_UI(event) );

	case ID_CMD_SHAPES_EDIT_SHAPE:
	case ID_CMD_SHAPES_EDIT_ADD_SHAPE:
	case ID_CMD_SHAPES_EDIT_DEL_SHAPE:
	case ID_CMD_SHAPES_EDIT_ADD_PART:
	case ID_CMD_SHAPES_EDIT_DEL_PART:
	case ID_CMD_SHAPES_EDIT_DEL_POINT:
	case ID_CMD_SHAPES_EDIT_SEL_CLEAR:
	case ID_CMD_SHAPES_EDIT_SEL_INVERT:
		return( m_pLayer->On_Command_UI(event) );

	case ID_CMD_MAPS_LAYER_SHOW:
		event.Check(m_bShow);
		break;

	case ID_CMD_MAPS_MOVE_TOP:
	case ID_CMD_MAPS_MOVE_UP:
		event.Enable(Get_Index() > 0);
		break;

	case ID_CMD_MAPS_MOVE_DOWN:
	case ID_CMD_MAPS_MOVE_BOTTOM:
		event.Enable(Get_Index() < Get_Manager()->Get_Count() - 1);
		break;

	case ID_CMD_MAPS_GRID_FITCOLORS:
		event.Check(m_bFitColors);
		break;

	case ID_CMD_MAPS_PROJECT:
		event.Check(m_bProject);
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Layer::Load_Settings(CSG_MetaData *pEntry)
{
	if( pEntry )
	{
		m_bShow      = !(*pEntry)("SHOW"     ) || (*pEntry)["SHOW"     ].Get_Content().CmpNoCase("true") == 0;
		m_bProject   =  (*pEntry)("PROJECT"  ) && (*pEntry)["PROJECT"  ].Get_Content().CmpNoCase("true") == 0;
		m_bFitColors =  (*pEntry)("FITCOLORS") && (*pEntry)["FITCOLORS"].Get_Content().CmpNoCase("true") == 0;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Map_Layer::Save_Settings(CSG_MetaData *pEntry)
{
	if( pEntry )
	{
		pEntry->Add_Child("SHOW"     , m_bShow      ? "true" : "false");
		pEntry->Add_Child("PROJECT"  , m_bProject   ? "true" : "false");
		pEntry->Add_Child("FITCOLORS", m_bFitColors ? "true" : "false");

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameters * CWKSP_Map_Layer::Get_Parameters(void)
{
	return( m_pLayer->Get_Parameters() );
}

//---------------------------------------------------------
void CWKSP_Map_Layer::Parameters_Changed(void)
{
	m_pLayer->Parameters_Changed();

	CWKSP_Base_Item::Parameters_Changed();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define	FIT_OVERLAY_GRID_COLORS(band, extent)	{\
	CWKSP_Grid	*pGrid	= (CWKSP_Grid *)g_pData->Get(m_pLayer->Get_Parameter(band)->asGrid());\
	if( pGrid && m_pLayer->Get_Parameter(band)->is_Enabled() )\
	{	pGrid->Fit_Colors(extent);	}\
}

//---------------------------------------------------------
bool CWKSP_Map_Layer::Fit_Colors(const CSG_Rect &rWorld)
{
	if( m_bFitColors )
	{
		switch( m_pLayer->Get_Type() )
		{
		default:
			return( false );

		case WKSP_ITEM_Grid :
			((CWKSP_Grid  *)m_pLayer)->Fit_Colors(rWorld);

			if( m_pLayer->Get_Parameter("COLORS_TYPE")->asInt() == CLASSIFY_OVERLAY )
			{
				FIT_OVERLAY_GRID_COLORS("OVERLAY_R", rWorld);
				FIT_OVERLAY_GRID_COLORS("OVERLAY_G", rWorld);
				FIT_OVERLAY_GRID_COLORS("OVERLAY_B", rWorld);
			}
			break;

		case WKSP_ITEM_Grids:
			((CWKSP_Grids *)m_pLayer)->Fit_Colors(rWorld);
			break;
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Layer::_Get_Projections(CSG_Projection &prj_Layer, CSG_Projection &prj_Map)
{
	prj_Layer	= m_pLayer    ->Get_Object () ->Get_Projection();
	prj_Map		= ((CWKSP_Map *)Get_Manager())->Get_Projection();

	return( prj_Layer.is_Okay() && prj_Map.is_Okay() && prj_Layer.is_Equal(prj_Map) == false );
}

//---------------------------------------------------------
CSG_Rect CWKSP_Map_Layer::Get_Extent(void)
{
	CSG_Projection	prj_Layer, prj_Map;

	if( !m_bProject || !_Get_Projections(prj_Layer, prj_Map) )
	{
		return( m_pLayer->Get_Extent() );
	}

	//-----------------------------------------------------
	TSG_Rect	rMap(m_pLayer->Get_Extent());

	SG_Get_Projected(prj_Layer, prj_Map, rMap);

	return( rMap );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Layer::Draw(CWKSP_Map_DC &dc_Map, int Flags)
{
	CSG_Projection	prj_Layer, prj_Map;

	if( !m_bProject || !_Get_Projections(prj_Layer, prj_Map) )
	{
		if( m_pLayer->do_Show(dc_Map.m_rWorld) )
		{
			m_pLayer->Draw(dc_Map, Flags);
		}

		return( true );
	}

	//-----------------------------------------------------
	CSG_Rect	rLayer(dc_Map.m_rWorld);

	SG_Get_Projected(prj_Map, prj_Layer, rLayer.m_rect);

	if( !m_pLayer->do_Show(rLayer) )
	{
		return( true );
	}

	switch( m_pLayer->Get_Object()->Get_ObjectType() )
	{
	//-----------------------------------------------------
	case SG_DATAOBJECT_TYPE_Shapes: {
		CSG_Shapes	*pShapes	= m_pLayer->Get_Object()->asShapes();
		CSG_Shapes	  Shapes(pShapes->Get_Type(), SG_T(""), pShapes);

		Shapes.Get_Projection().Create(pShapes->Get_Projection());

		for(int i=0; i<pShapes->Get_Count(); i++)
		{
			if( pShapes->Get_Shape(i)->Intersects(rLayer) )
			{
				Shapes.Add_Shape(pShapes->Get_Shape(i));
			}
		}

		SG_Get_Projected(&Shapes, NULL, prj_Map);

		m_pLayer->Draw(dc_Map, Flags, &Shapes);

	}	break;

	//-----------------------------------------------------
	case SG_DATAOBJECT_TYPE_PointCloud: {
		CSG_PointCloud	*pPoints	= m_pLayer->Get_Object()->asPointCloud();
		CSG_PointCloud	 Points(pPoints);

		Points.Get_Projection().Create(pPoints->Get_Projection());

		for(int i=0; i<pPoints->Get_Count(); i++)
		{
			if( rLayer.Contains(pPoints->Get_X(i), pPoints->Get_Y(i)) )
			{
				Points.Add_Shape(pPoints->Get_Record(i));
			}
		}

		CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 2);	// Coordinate Transformation (Shapes)

		if( !pTool )
		{
			return( false );
		}

		SG_UI_ProgressAndMsg_Lock(true);

		pTool->Set_Manager(NULL);

		bool	bResult	=
			pTool->Set_Parameter("CRS_PROJ4", prj_Map.Get_Proj4())
		&&  pTool->Set_Parameter("SOURCE"   , &Points)
		&&  pTool->Set_Parameter("COPY"     , false)
		&&  pTool->Execute();

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

		SG_UI_ProgressAndMsg_Lock(false);

		m_pLayer->Draw(dc_Map, Flags, &Points);

	}	break;

	//-----------------------------------------------------
	case SG_DATAOBJECT_TYPE_Grid: {
		CSG_Grid	Grid, *pGrid = m_pLayer->Get_Object()->asGrid();

		if( !pGrid->Get_Extent().Intersects(rLayer) )	// == INTERSECTION_None
		{
			return( true );
		}

		CSG_Grid_System	System(dc_Map.m_DC2World, dc_Map.m_rWorld.Get_XMin(), dc_Map.m_rWorld.Get_YMin(), dc_Map.dc.GetSize().GetWidth(), dc_Map.dc.GetSize().GetHeight());

		if( !System.is_Valid() || !Grid.Create(System, pGrid->Get_Type()) )
		{
			return( false );
		}

		CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 4);	// Coordinate Transformation (Grid)

		if(	pTool == NULL )
		{
			return( false );
		}

		pTool->Set_Manager(NULL);

		SG_UI_ProgressAndMsg_Lock(true);

		if( pTool->Set_Parameter("CRS_PROJ4"        , prj_Map.Get_Proj4())
		&&  pTool->Set_Parameter("RESAMPLING"       , m_pLayer->Get_Parameter("DISPLAY_RESAMPLING")->asInt() ? 3 : 0)
		&&  pTool->Set_Parameter("TARGET_DEFINITION", 1)
		&&  pTool->Set_Parameter("KEEP_TYPE"        , true)
		&&  pTool->Set_Parameter("SOURCE"           , pGrid)
		&&  pTool->Set_Parameter("GRID"             , &Grid)
		&&  pTool->Execute() )
		{
			m_pLayer->Draw(dc_Map, Flags, &Grid);
		}

		SG_UI_ProgressAndMsg_Lock(false);

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	}	break;

	//-----------------------------------------------------
	case SG_DATAOBJECT_TYPE_Grids: {
		CSG_Grids	*pGrids = m_pLayer->Get_Object()->asGrids();

		if( !pGrids->Get_Extent().Intersects(rLayer) )	// == INTERSECTION_None
		{
			return( true );
		}

		CSG_Grid_System	System(dc_Map.m_DC2World, dc_Map.m_rWorld.Get_XMin(), dc_Map.m_rWorld.Get_YMin(), dc_Map.dc.GetSize().GetWidth(), dc_Map.dc.GetSize().GetHeight());

		if( !System.is_Valid() )
		{
			return( false );
		}

		CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 3);	// Coordinate Transformation (Grid List)

		if(	pTool == NULL )
		{
			return( false );
		}

		pTool->Set_Manager(NULL);

		SG_UI_ProgressAndMsg_Lock(true);

		if( pTool->Set_Parameter("CRS_PROJ4"        , prj_Map.Get_Proj4())
		&&  pTool->Set_Parameter("RESAMPLING"       , m_pLayer->Get_Parameter("DISPLAY_RESAMPLING")->asInt() ? 3 : 0)
		&&  pTool->Set_Parameter("KEEP_TYPE"        , true)
		&&  pTool->Set_Parameter("TARGET_DEFINITION", 0)
		&&  pTool->Set_Parameter("TARGET_USER_SIZE" , System.Get_Cellsize())
		&&  pTool->Set_Parameter("TARGET_USER_XMIN" , System.Get_XMin())
		&&  pTool->Set_Parameter("TARGET_USER_YMIN" , System.Get_YMin())
		&&  pTool->Set_Parameter("TARGET_USER_XMAX" , System.Get_XMax())
		&&  pTool->Set_Parameter("TARGET_USER_YMAX" , System.Get_YMax())
		&&  pTool->Get_Parameter("SOURCE")->asList()->Add_Item(pGrids)
		&&  pTool->Execute() && (pGrids = (CSG_Grids *)pTool->Get_Parameter("GRIDS")->asList()->Get_Item(0)) != NULL )
		{
			m_pLayer->Draw(dc_Map, Flags, pGrids);

			delete(pGrids);
		}

		SG_UI_ProgressAndMsg_Lock(false);

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	}	break;

	//-----------------------------------------------------
	default:
		return( false );
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
