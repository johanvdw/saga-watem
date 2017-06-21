/**********************************************************
 * Version $Id: table.h 1246 2011-11-25 13:42:38Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       db_pgsql                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       Table.h                         //
//                                                       //
//                 Copyright (C) 2013 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Table_H
#define HEADER_INCLUDED__Table_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_List : public CSG_PG_Tool
{
public:
	CTable_List(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Tools") );	}


protected:

	virtual bool				On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Info : public CSG_PG_Tool
{
public:
	CTable_Info(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Tools") );	}


protected:

	virtual void				On_Connection_Changed	(CSG_Parameters *pParameters);

	virtual bool				On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Load : public CSG_PG_Tool
{
public:
	CTable_Load(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Tables") );	}


protected:

	virtual void				On_Connection_Changed	(CSG_Parameters *pParameters);

	virtual bool				On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Save : public CSG_PG_Tool
{
public:
	CTable_Save(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Tables") );	}


protected:

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Drop : public CSG_PG_Tool
{
public:
	CTable_Drop(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Tables") );	}


protected:

	virtual void				On_Connection_Changed	(CSG_Parameters *pParameters);

	virtual bool				On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Query : public CSG_PG_Tool
{
public:
	CTable_Query(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Tables") );	}


protected:

	virtual bool				On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Query_GUI : public CSG_PG_Tool
{
public:
	CTable_Query_GUI(void);

	virtual bool				needs_GUI				(void)	{	return( true );	}

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Tables") );	}


protected:

	virtual void				On_Connection_Changed	(CSG_Parameters *pParameters);

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	CSG_String					Get_Selection			(const CSG_String &Parameter);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Table_H
