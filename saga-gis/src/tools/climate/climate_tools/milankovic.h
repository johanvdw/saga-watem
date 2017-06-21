/**********************************************************
 * Version $Id: milankovic.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Lectures                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     milankovic.h                      //
//                                                       //
//                 Copyright (C) 2012 by                 //
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
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__milankovic_H
#define HEADER_INCLUDED__milankovic_H

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CMilankovic : public CSG_Tool
{
public:
	CMilankovic(void);

	virtual CSG_String	Get_MenuPath	(void)	{	return( _TL("Earth's Orbital Parameters") );	}


protected:

	virtual bool		On_Execute		(void);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CMilankovic_SR_Location : public CSG_Tool
{
public:
	CMilankovic_SR_Location(void);

	virtual CSG_String	Get_MenuPath	(void)	{	return( _TL("Earth's Orbital Parameters") );	}


protected:

	virtual bool		On_Execute		(void);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CMilankovic_SR_Day_Location : public CSG_Tool
{
public:
	CMilankovic_SR_Day_Location(void);

	virtual CSG_String	Get_MenuPath	(void)	{	return( _TL("Earth's Orbital Parameters") );	}


protected:

	virtual bool		On_Execute		(void);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CMilankovic_SR_Monthly_Global : public CSG_Tool
{
public:
	CMilankovic_SR_Monthly_Global(void);

	virtual CSG_String	Get_MenuPath	(void)	{	return( _TL("Earth's Orbital Parameters") );	}


protected:

	virtual bool		On_Execute		(void);

};


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__milankovic_H
