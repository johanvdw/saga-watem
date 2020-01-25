
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    table_calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                table_fill_record_gaps.h               //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
//    e-mail:     oconrad@saga-gis.de                    //
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
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__table_fill_record_gaps_H
#define HEADER_INCLUDED__table_fill_record_gaps_H


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Fill_Record_Gaps : public CSG_Tool
{
public:
	CTable_Fill_Record_Gaps(void);

//	virtual CSG_String		Get_MenuPath	(void)		{	return( _TL("Import") );	}


protected:

	virtual bool			On_Execute		(void);


private:

	int						m_fOrder;

	CSG_Table				*m_pTable;

	bool					Get_Neighbours	(int iRecord, int Order, int Field, CSG_Vector &Y, CSG_Vector &X, int Number);

	double					Get_Nearest		(const double Y[2], const double X[2], double y);
	double					Get_Linear		(const double Y[2], const double X[2], double y);
	double					Get_Spline		(const double Y[4], const double X[4], double y);

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Insert_Records : public CSG_Tool
{
public:
	CTable_Insert_Records(void);

//	virtual CSG_String		Get_MenuPath	(void)		{	return( _TL("Import") );	}


protected:

	virtual bool			On_Execute		(void);


private:

	int						m_fOrder;

	CSG_Table				*m_pNoGaps;


	bool					Set_Nearest		(int iOffset, int iField, CSG_Table_Record *pA, CSG_Table_Record *pB);
	bool					Set_Linear		(int iOffset, int iField, CSG_Table_Record *pA, CSG_Table_Record *pB);
	bool					Set_Spline		(int iOffset, int iField, CSG_Table_Record *pA, CSG_Table_Record *pB, CSG_Table_Record *pC, CSG_Table_Record *pD);

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__table_fill_record_gaps_H
