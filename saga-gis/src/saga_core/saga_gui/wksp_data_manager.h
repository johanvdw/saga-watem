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
//                 WKSP_Data_Manager.h                   //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Data_Manager_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Data_Manager_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>

#include "wksp_base_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Data_Manager : public CWKSP_Base_Manager
{
public:
	CWKSP_Data_Manager(void);
	virtual ~CWKSP_Data_Manager(void);

	bool							Initialise			(void);
	bool							Finalise			(void);

	virtual TWKSP_Item				Get_Type			(void)		{	return( WKSP_ITEM_Data_Manager );	}

	virtual wxString				Get_Name			(void);
	virtual wxString				Get_Description		(void);

	virtual wxMenu *				Get_Menu			(void);

	virtual bool					On_Command			(int Cmd_ID);
	virtual bool					On_Command_UI		(wxUpdateUIEvent &event);

	virtual CSG_Parameters *		Get_Parameters		(void);
	virtual void					Parameters_Changed	(void);
	virtual int						On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags);

	CWKSP_Base_Manager *			Get_Manager			(TSG_Data_Object_Type Type, bool bAdd = false);
	CWKSP_Base_Manager *			Get_Manager			(int i)		{	return( (CWKSP_Base_Manager *)Get_Item(i) );	}
	void							Del_Manager			(CWKSP_Base_Item *pItem);

	class CWKSP_Table_Manager *		Get_Tables			(void)		{	return( m_pTables      );	}
	class CWKSP_Shapes_Manager *	Get_Shapes			(void)		{	return( m_pShapes      );	}
	class CWKSP_TIN_Manager *		Get_TINs			(void)		{	return( m_pTINs        );	}
	class CWKSP_PointCloud_Manager *Get_PointClouds		(void)		{	return( m_pPointClouds );	}
	class CWKSP_Grid_Manager *		Get_Grids			(void)		{	return( m_pGrids       );	}

	class CWKSP_Layer *				Get_Layer			(CSG_Data_Object *pObject);

	class CWKSP_Data_Menu_Files *	Get_Menu_Files		(void)		{	return( m_pMenu_Files );	}

	CWKSP_Base_Item *				Open				(const wxString &File, int DataType);
	bool							Open				(const wxString &File);
	bool							Open				(int DataType);
	bool							Open_CMD			(int Cmd_ID);
	bool							Open_Browser		(void);
	bool							Open_Browser		(wxArrayString &Projects, const wxString &Directory);

	bool							Save_Modified		(class CWKSP_Base_Item *pItem = NULL, bool bSelections = false);
	bool							Save_Modified_Sel	(void);
	bool							Close				(bool bSilent);

	class CWKSP_Data_Item *			Add					(class CSG_Data_Object *pObject);
	class CWKSP_Data_Item *			Get					(class CSG_Data_Object *pObject);
	bool							Update				(class CSG_Data_Object *pObject, class CSG_Parameters *pParameters);
	bool							Update_Views		(class CSG_Data_Object *pObject);
	bool							Show				(class CSG_Data_Object *pObject, int Flags = 0);
	bool							asImage				(class CSG_Data_Object *pObject, class CSG_Grid *pImage);

	bool							Get_Colors			(class CSG_Data_Object *pObject, class CSG_Colors *pColors);
	bool							Set_Colors			(class CSG_Data_Object *pObject, class CSG_Colors *pColors);
	bool							Get_Parameters		(class CSG_Data_Object *pObject, class CSG_Parameters *pParameters);
	bool							Set_Parameters		(class CSG_Data_Object *pObject, class CSG_Parameters *pParameters);

	int								Get_Numbering		(void)	const	{	return( m_Numbering );	}

	size_t							MultiSelect_Count	(void);
	bool							MultiSelect_Check	(void);
	bool							MultiSelect_Update	(void);


private:

	int								m_Numbering;

	wxArrayTreeItemIds				m_Sel_Items;

	CSG_Parameters					m_Sel_Parms[2];

	class CWKSP_Project				*m_pProject;

	class CWKSP_Data_Menu_Files		*m_pMenu_Files;

	class CWKSP_Table_Manager		*m_pTables;

	class CWKSP_Shapes_Manager		*m_pShapes;

	class CWKSP_TIN_Manager			*m_pTINs;

	class CWKSP_PointCloud_Manager	*m_pPointClouds;

	class CWKSP_Grid_Manager		*m_pGrids;

	static int						_Modified_Changed	(class CSG_Parameter  *pParameter, int Flags);
	bool							_Modified_Get		(class CSG_Parameters *pParameters, class CWKSP_Base_Item *pItem, const wxString &Directory, bool bSelections);
	bool							_Modified_Get		(class CSG_Parameters *pParameters, class CWKSP_Base_Item *pItem, const wxString &Directory, class CSG_Data_Object *pObject);
	bool							_Modified_Save		(class CSG_Parameters *pParameters);


};

//---------------------------------------------------------
extern CWKSP_Data_Manager			*g_pData;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Data_Manager_H
