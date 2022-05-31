
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
//              active_attributes_control.h              //
//                                                       //
//          Copyright (C) 2015 by Olaf Conrad            //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef _HEADER_INCLUDED__SAGA_GUI__active_attributes_control_H
#define _HEADER_INCLUDED__SAGA_GUI__active_attributes_control_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/grid.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CActive_Attributes_Control : public wxGrid
{
	DECLARE_CLASS(CActive_Attributes_Control)

public:
	CActive_Attributes_Control(wxWindow *pParent);
	virtual ~CActive_Attributes_Control(void);

	CSG_Table &					Get_Table			(void)	{	return( m_Table );	}

	void						Set_Row_Labeling	(bool bOn);

	bool						Update_Table		(void);


protected:

	void						On_Key				(wxKeyEvent      &event);

	void						On_Edit_Start		(wxGridEvent     &event);
	void						On_Edit_Stop		(wxGridEvent     &event);
	void						On_Changed			(wxGridEvent     &event);

	void						On_LClick			(wxGridEvent     &event);
	void						On_LDClick			(wxGridEvent     &event);
	void						On_LClick_Label		(wxGridEvent     &event);
	void						On_RClick			(wxGridEvent     &event);
	void						On_RClick_Label		(wxGridEvent     &event);

	void						On_Autosize_Cols	(wxCommandEvent  &event);
	void						On_Autosize_Rows	(wxCommandEvent  &event);

	void						On_Cell_Open		(wxCommandEvent  &event);

	void						On_Field_Add		(wxCommandEvent  &event);
	void						On_Field_Del		(wxCommandEvent  &event);
	void						On_Field_Rename		(wxCommandEvent  &event);
	void						On_Field_Type		(wxCommandEvent  &event);

	void						On_ToClipboard		(wxCommandEvent  &event);


private:

	bool						m_bEditing;

	CSG_Table					m_Table;

	class CVIEW_Table_Data		*m_pData;


	bool						_Update_Records		(void);

	CSG_Grids *					_Get_Grids			(void);


	DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__active_attributes_control_H
