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
//              tool_interactive_base.cpp                //
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
#include "tool.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Interactive_Base::CSG_Tool_Interactive_Base(void)
{
	m_pTool		= NULL;

	m_Keys			= 0;
	m_Drag_Mode		= TOOL_INTERACTIVE_DRAG_BOX;

	m_Point			.Assign(0.0, 0.0);
	m_Point_Last	.Assign(0.0, 0.0);
}

//---------------------------------------------------------
CSG_Tool_Interactive_Base::~CSG_Tool_Interactive_Base(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Interactive_Base::Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode, int Keys)
{
	if( !m_pTool || m_pTool->m_bExecutes )
	{
		return( false );
	}

	m_pTool->m_bExecutes		= true;
	m_pTool->m_bError_Ignore	= false;

	m_Point_Last	= m_Point;
	m_Point			= ptWorld;

	m_Keys			= Keys;

	bool	bResult	= On_Execute_Position(m_Point, Mode);

	m_Keys			= 0;

	if( bResult )
	{
		m_pTool->_Synchronize_DataObjects();
	}

	m_pTool->m_bExecutes		= false;

	SG_UI_Process_Set_Okay();

	return( bResult );
}

bool CSG_Tool_Interactive_Base::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	return( false );
}

//---------------------------------------------------------
bool CSG_Tool_Interactive_Base::Execute_Keyboard(int Character, int Keys)
{
	bool	bResult	= false;

	if( m_pTool && !m_pTool->m_bExecutes )
	{
		m_pTool->m_bExecutes		= true;
		m_pTool->m_bError_Ignore	= false;

		m_Keys						= Keys;

		bResult						= On_Execute_Keyboard(Character);

		m_Keys						= 0;

		m_pTool->_Synchronize_DataObjects();

		m_pTool->m_bExecutes		= false;

		SG_UI_Process_Set_Okay();
	}

	return( bResult );
}

bool CSG_Tool_Interactive_Base::On_Execute_Keyboard(int Character)
{
	return( false );
}

//---------------------------------------------------------
bool CSG_Tool_Interactive_Base::Execute_Finish(void)
{
	bool	bResult	= false;

	if( m_pTool && !m_pTool->m_bExecutes )
	{
		m_pTool->m_bExecutes		= true;
		m_pTool->m_bError_Ignore	= false;

		bResult						= On_Execute_Finish();

		m_pTool->_Synchronize_DataObjects();

		m_pTool->m_bExecutes		= false;

		SG_UI_Process_Set_Okay();
	}

	return( bResult );
}

bool CSG_Tool_Interactive_Base::On_Execute_Finish(void)
{
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Tool_Interactive_Base::Set_Drag_Mode(int Drag_Mode)
{
	m_Drag_Mode	= Drag_Mode;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
