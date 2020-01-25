
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        VIGRA                          //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 vigra_random_forest.h                 //
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
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__vigra_random_forest_H
#define HEADER_INCLUDED__vigra_random_forest_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "vigra.h"

//---------------------------------------------------------
#if defined(VIGRA_VERSION_MAJOR) && VIGRA_VERSION_MAJOR >= 1 && VIGRA_VERSION_MINOR >= 10


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CViGrA_Random_Forest : public CSG_Tool_Grid
{
public:
	CViGrA_Random_Forest(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("A:Imagery|Classification|Machine Learning") );	}


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	int							m_nFeatures;

	CSG_Grid					**m_pFeatures;


	CSG_Grid *					Get_Class_Grid			(void);
	void						Set_Classification		(CSG_Table &Classes);

	CSG_Parameter_Grid_List *	Get_Propability_Grids	(CSG_Table &Classes);

	bool						Get_Training			(CSG_Matrix &Data, CSG_Table &Classes);
	int							Get_Training			(CSG_Matrix &Data, int ID, CSG_Shape_Polygon *pArea);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CViGrA_RF_Presence : public CSG_Tool_Grid
{
public:
	CViGrA_RF_Presence(void);

	virtual CSG_String			Get_MenuPath	(void)	{	return( _TL("A:Grid|Analysis|Macroecology") );	}


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	int							m_nFeatures;

	CSG_Grid					**m_pFeatures;


	bool						Get_Training			(CSG_Matrix &Data);
	bool						Get_Training			(CSG_Matrix &Data, int x, int y, int ID);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CViGrA_RF_Table : public CSG_Tool
{
public:
	CViGrA_RF_Table(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("A:Table|Tools") );	}


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define new_CViGrA_Random_Forest	new CViGrA_Random_Forest
#define new_CViGrA_RF_Presence		new CViGrA_RF_Presence
#define new_CViGrA_RF_Table			new CViGrA_RF_Table

#else // defined(VIGRA_VERSION_MAJOR) && VIGRA_VERSION_MAJOR >= 1 && VIGRA_VERSION_MINOR >= 10

#define new_CViGrA_Random_Forest	TLB_INTERFACE_SKIP_TOOL
#define new_CViGrA_RF_Presence		TLB_INTERFACE_SKIP_TOOL
#define new_CViGrA_RF_Table			TLB_INTERFACE_SKIP_TOOL
#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__vigra_random_forest_H
