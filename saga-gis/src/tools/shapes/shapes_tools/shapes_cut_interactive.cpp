/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     shapes_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Shapes_Cut_Interactive.cpp              //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "shapes_cut_interactive.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Cut_Interactive::CShapes_Cut_Interactive(void)
{
	Set_Name		(_TL("Copy Shapes from Region"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Copies all shapes that belong to the specified region."
	));

	Set_Drag_Mode	(TOOL_INTERACTIVE_DRAG_BOX);

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		""	, "SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		""	, "CUT"		, _TL("Copy"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes(
		""	, "EXTENT"	, _TL("Extent"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice(
		""	, "METHOD"	, _TL("Method"),
		_TL(""),
		Cut_Methods_Str(), 1
	);

	Parameters.Add_Bool(
		""	, "CONFIRM"	, _TL("Confirm"),
		_TL("Show a confirmation dialog before selection is performed"),
		false
	);

	//-----------------------------------------------------
	CSG_Parameters	*pParameters	= Add_Parameters("CUT", _TL("Extent"), _TL(""));

	pParameters->Add_Double("", "AX", _TL("Left"            ), _TL(""), 0.0);
	pParameters->Add_Double("", "BX", _TL("Right"           ), _TL(""), 1.0);
	pParameters->Add_Double("", "AY", _TL("Bottom"          ), _TL(""), 0.0);
	pParameters->Add_Double("", "BY", _TL("Top"             ), _TL(""), 1.0);
	pParameters->Add_Double("", "DX", _TL("Horizontal Range"), _TL(""), 1.0, 0.0, true);
	pParameters->Add_Double("", "DY", _TL("Vertical Range"  ), _TL(""), 1.0, 0.0, true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CShapes_Cut_Interactive::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameters->Cmp_Identifier("CUT") )
	{
		double	ax	= pParameters->Get_Parameter("AX")->asDouble();
		double	ay	= pParameters->Get_Parameter("AY")->asDouble();
		double	bx	= pParameters->Get_Parameter("BX")->asDouble();
		double	by	= pParameters->Get_Parameter("BY")->asDouble();
		double	dx	= pParameters->Get_Parameter("DX")->asDouble();
		double	dy	= pParameters->Get_Parameter("DY")->asDouble();

		if( ax > bx )	{	double	d	= ax;	ax	= bx;	bx	= d;	}
		if( ay > by )	{	double	d	= ay;	ay	= by;	by	= d;	}

		if     ( pParameter->Cmp_Identifier("DX") )
		{
			bx	= ax + dx;
		}
		else if( pParameter->Cmp_Identifier("AX")
			||   pParameter->Cmp_Identifier("BX") )
		{
			dx	= bx - ax;
		}

		else if( pParameter->Cmp_Identifier("DY") )
		{
			by	= ay + dy;
		}
		else if( pParameter->Cmp_Identifier("AY")
			||   pParameter->Cmp_Identifier("BY") )
		{
			dy	= by - ay;
		}

		pParameters->Set_Parameter("AX", ax);
		pParameters->Set_Parameter("AY", ay);
		pParameters->Set_Parameter("BX", bx);
		pParameters->Set_Parameter("BY", by);
		pParameters->Set_Parameter("DX", dx);
		pParameters->Set_Parameter("DY", dy);
	}

	//-----------------------------------------------------
	else if( pParameter->Cmp_Identifier("SHAPES") && pParameter->asShapes() )
	{
		CSG_Shapes	*pCut	= pParameters->Get_Parameter("CUT")->asShapes();

		if( pCut != DATAOBJECT_NOTSET && pCut != DATAOBJECT_CREATE && pCut->Get_Type() != pParameter->asShapes()->Get_Type() )
		{
			pParameters->Get_Parameter("CUT")->Set_Value(DATAOBJECT_CREATE);
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Cut_Interactive::On_Execute(void)
{
	m_bDown		= false;

	m_pShapes	= Parameters("SHAPES" )->asShapes();
	m_pCut		= Parameters("CUT"    )->asShapes();
	m_pExtent	= Parameters("EXTENT" )->asShapes();
	m_Method	= Parameters("METHOD" )->asInt   ();
	m_bConfirm	= Parameters("CONFIRM")->asBool  ();

	m_pCut->Create(m_pShapes->Get_Type());

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Cut_Interactive::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	switch( Mode )
	{
	//-----------------------------------------------------
	case TOOL_INTERACTIVE_LDOWN:
		if( m_bDown == false )
		{
			m_bDown	= true;
			m_pDown	= ptWorld;
		}
		break;

	//-----------------------------------------------------
	case TOOL_INTERACTIVE_LUP:
		if( m_bDown == true )
		{
			m_bDown	= false;

			CSG_Rect	Extent(m_pDown, ptWorld);

			if( !m_bConfirm || Get_Extent(Extent) )
			{
				if( Cut_Set_Extent(Extent, m_pExtent, true) )
				{
					DataObject_Update(m_pExtent);
				}

				if( Cut_Shapes(Extent, m_Method, m_pShapes, m_pCut) )
				{
					DataObject_Update(m_pCut);
				}
				else
				{
					Message_Add(_TL("No shapes in region"));
				}
			}
		}
		break;
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Cut_Interactive::Get_Extent(CSG_Rect &Extent)
{
	CSG_Parameters	*pParameters	= Get_Parameters("CUT");

	pParameters->Set_Callback(false);
	pParameters->Set_Parameter("AX", Extent.Get_XMin  ());
	pParameters->Set_Parameter("AY", Extent.Get_YMin  ());
	pParameters->Set_Parameter("BX", Extent.Get_XMax  ());
	pParameters->Set_Parameter("BY", Extent.Get_YMax  ());
	pParameters->Set_Parameter("DX", Extent.Get_XRange());
	pParameters->Set_Parameter("DY", Extent.Get_YRange());
	pParameters->Set_Callback(true);

	if( Dlg_Parameters("CUT") )
	{
		Extent.Assign(
			pParameters->Get_Parameter("AX")->asDouble(),
			pParameters->Get_Parameter("AY")->asDouble(),
			pParameters->Get_Parameter("BX")->asDouble(),
			pParameters->Get_Parameter("BY")->asDouble()
		);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
