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
//                 WKSP_Base_Manager.cpp                 //
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
#include <wx/window.h>
#include <wx/debug.h>

#include "active.h"
#include "active_parameters.h"

#include "wksp_tool_control.h"
#include "wksp_data_control.h"
#include "wksp_data_manager.h"
#include "wksp_map_control.h"

#include "wksp_base_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Base_Manager::CWKSP_Base_Manager(void)
{
	m_bManager	= true;
	m_nItems	= 0;
	m_Items		= NULL;
	m_Item_ID	= 0;
}

//---------------------------------------------------------
CWKSP_Base_Manager::~CWKSP_Base_Manager(void)
{
	wxASSERT_MSG(m_nItems == 0, wxT("CWKSP_Base_Manager: Could not kill all items on destruction!"));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Base_Manager::Add_Item(CWKSP_Base_Item *pItem)
{
	if( pItem )
	{
		m_Items				= (CWKSP_Base_Item **)realloc(m_Items, (m_nItems + 1) * sizeof(CWKSP_Base_Item *));
		m_Items[m_nItems++]	= pItem;
		pItem->m_pManager	= this;
		pItem->m_ID			= m_Item_ID++;

		//-------------------------------------------------
		switch( Get_Type() )
		{
		default:
			break;

		case WKSP_ITEM_Tool_Manager:
			g_pTool_Ctrl->Add_Group((CWKSP_Tool_Group *)pItem);
			break;

		case WKSP_ITEM_Tool_Group:
			g_pTool_Ctrl->Add_Library(GetId(), (CWKSP_Tool_Library *)pItem);
			break;

		case WKSP_ITEM_Data_Manager:
		case WKSP_ITEM_Table_Manager:
		case WKSP_ITEM_Shapes_Manager:
		case WKSP_ITEM_Shapes_Type:
		case WKSP_ITEM_TIN_Manager:
		case WKSP_ITEM_PointCloud_Manager:
		case WKSP_ITEM_Grid_Manager:
		case WKSP_ITEM_Grid_System:
			g_pData_Ctrl->Add_Item(this, pItem);
			g_pActive->Update_DataObjects();
			break;

		case WKSP_ITEM_Map_Manager:
		case WKSP_ITEM_Map:
			g_pMap_Ctrl->Add_Item(this, pItem);
			break;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Base_Manager::Del_Item(int iItem)
{
	return( Del_Item(Get_Item(iItem)) );
}

bool CWKSP_Base_Manager::Del_Item(CWKSP_Base_Item *pItem)
{
	int	iItem	= pItem ? pItem->Get_Index() : -1;

	if( iItem >= 0 && iItem < m_nItems )
	{
		m_nItems--;

		for( ; iItem<m_nItems; iItem++)
		{
			m_Items[iItem]	= m_Items[iItem + 1];
		}

		m_Items	= (CWKSP_Base_Item **)realloc(m_Items, m_nItems * sizeof(CWKSP_Base_Item *));

		//-------------------------------------------------
		switch( Get_Type() )
		{
		default:
			break;

		case WKSP_ITEM_Data_Manager:
			g_pData->Del_Manager(pItem);
			break;

		case WKSP_ITEM_Table_Manager:
		case WKSP_ITEM_Shapes_Type:
		case WKSP_ITEM_TIN_Manager:
		case WKSP_ITEM_PointCloud_Manager:
		case WKSP_ITEM_Grid_Manager:
		case WKSP_ITEM_Grid_System:
			if( g_pActive )
			{
				g_pActive->Update_DataObjects();
			}
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
bool CWKSP_Base_Manager::On_Data_Deletion(CSG_Data_Object *pObject)
{
	CWKSP_Base_Item::On_Data_Deletion(pObject);

	for(int iItem=0; iItem<m_nItems; iItem++)
	{
		m_Items[iItem]->On_Data_Deletion(pObject);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Base_Manager::Move_Top(CWKSP_Base_Item *pItem)
{
	int		Index, i;

	if( pItem && (Index = pItem->Get_Index()) > 0 )
	{
		for(i=Index; i>0; i--)
		{
			m_Items[i]	= m_Items[i - 1];
		}

		m_Items[0]		= pItem;

		Get_Control()->SortChildren(GetId());

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Base_Manager::Move_Bottom(CWKSP_Base_Item *pItem)
{
	int		Index, i;

	if( pItem && (Index = pItem->Get_Index()) < Get_Count() - 1 )
	{
		for(i=Index; i<Get_Count()-1; i++)
		{
			m_Items[i]	= m_Items[i + 1];
		}

		m_Items[Get_Count() - 1]	= pItem;

		Get_Control()->SortChildren(GetId());

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Base_Manager::Move_Up(CWKSP_Base_Item *pItem)
{
	int		Index;

	if( pItem && (Index = pItem->Get_Index()) > 0 )
	{
		m_Items[Index]		= m_Items[Index - 1];
		m_Items[Index - 1]	= pItem;

		Get_Control()->SortChildren(GetId());

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Base_Manager::Move_Down(CWKSP_Base_Item *pItem)
{
	int		Index;

	if( pItem && (Index = pItem->Get_Index()) < Get_Count() - 1 )
	{
		m_Items[Index]		= m_Items[Index + 1];
		m_Items[Index + 1]	= pItem;

		Get_Control()->SortChildren(GetId());

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Base_Manager::Move_To(CWKSP_Base_Item *pItem, CWKSP_Base_Item *pItem_Dst)
{
	if( pItem && pItem_Dst && pItem != pItem_Dst && pItem->Get_Manager() == this && pItem_Dst->Get_Manager() == this )
	{
		int		Index, Index_Dst, i;

		Index		= pItem		->Get_Index();
		Index_Dst	= pItem_Dst	->Get_Index();

		if( Index < Index_Dst )
		{
			for(i=Index; i<Index_Dst; i++)
			{
				m_Items[i]	= m_Items[i + 1];
			}
		}
		else
		{
			for(i=Index; i>Index_Dst; i--)
			{
				m_Items[i]	= m_Items[i - 1];
			}
		}

		m_Items[Index_Dst]	= pItem;

		Get_Control()->SortChildren(GetId());

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Base_Manager::Get_Items_Count(void)
{
	int	nCount	= 0;

	for(int iItem=0; iItem<m_nItems; iItem++)
	{
		if( m_Items[iItem]->m_bManager )
		{
			nCount	+= ((CWKSP_Base_Manager *)m_Items[iItem])->Get_Count();
		}
	}

	return( nCount );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
