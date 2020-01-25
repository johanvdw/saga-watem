/**********************************************************
 * Version $Id: Table_Trend.cpp 1098 2011-06-16 16:06:32Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                 statistics_regression                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   table_trend.cpp                     //
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

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "table_trend.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CTable_Trend_Base::Initialise(void)
{
	//-----------------------------------------------------
	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Table_Field("TABLE",
		"FIELD_X"	, _TL("X Values"),
		_TL("")
	);

	Parameters.Add_Table_Field("TABLE",
		"FIELD_Y"	, _TL("Y Values"),
		_TL("")
	);

	Parameters.Add_String("",
		"FORMULA"	, _TL("Formula"),
		_TL(""),
		"m * x + b"
	);

	Parameters.Add_Choice("",
		"FORMULAS"	, _TL("Pre-defined Formulas"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s",
			SG_T("a + b * x (linear)"),
			SG_T("a + b * x + c * x^2 (quadric)"),
			SG_T("a + b * x + c * x^2 + d * x^3 (cubic)"),
			SG_T("a + b * ln(x) (logarithmic)"),
			SG_T("a + b * x^c (power)"),
			SG_T("a + b / x"),
			SG_T("a + b * (1 - exp(-x / c))"),
			SG_T("a + b * (1 - exp(-(x / c)^2))")
		), 0
	);

	Parameters.Add_Table("",
		"TREND"		, _TL("Table (with Trend)"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);
}

//---------------------------------------------------------
int CTable_Trend_Base::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("FORMULAS") )
	{
		CSG_String	Formula;

		switch( pParameter->asInt() )
		{
		default: Formula = "a + b * x"                    ; break;
		case  1: Formula = "a + b * x + c * x^2"          ; break;
		case  2: Formula = "a + b * x + c * x^2 + d * x^3"; break;
		case  3: Formula = "a + b * ln(x)"                ; break;
		case  4: Formula = "a + b * x^c"                  ; break;
		case  5: Formula = "a + b / x"                    ; break;
		case  6: Formula = "a + b * (1 - exp(-x / c))"    ; break;
		case  7: Formula = "a + b * (1 - exp(-(x / c)^2))"; break;
		}

		pParameters->Set_Parameter("FORMULA", Formula);
	}

	if( pParameter->Cmp_Identifier("FORMULA") )
	{
		if( !m_Trend.Set_Formula(pParameter->asString()) )
		{
			Message_Dlg(m_Trend.Get_Error(), _TL("Error in Formula"));

			return( false );
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CTable_Trend_Base::On_Execute(void)
{
	int					i, j, xField, yField;
	CSG_String			Name;
	CSG_Table_Record	*pRecord;
	CSG_Table			*pTable;

	pTable	= Parameters("TABLE")	->asTable();
	xField	= Parameters("FIELD_X")	->asInt();
	yField	= Parameters("FIELD_Y")	->asInt();

	//-----------------------------------------------------
	if( m_Trend.Set_Formula(Parameters("FORMULA")->asString()) )
	{
		m_Trend.Clr_Data();

		for(i=0; i<pTable->Get_Record_Count(); i++)
		{
			pRecord	= pTable->Get_Record(i);

			m_Trend.Add_Data(pRecord->asDouble(xField), pRecord->asDouble(yField));
		}

		//-------------------------------------------------
		if( m_Trend.Get_Trend() )
		{
			Message_Fmt("\n%s\n%s: %f", m_Trend.Get_Formula().c_str(), SG_T("r\xb2"), 100.0 * m_Trend.Get_R2());

			if( Parameters("TREND")->asTable() == NULL )
			{
				pTable->Add_Field("TREND"	, SG_DATATYPE_Double);

				for(i=0, j=pTable->Get_Field_Count()-1; i<m_Trend.Get_Data_Count(); i++)
				{
					pRecord	= pTable->Get_Record(i);
					pRecord->Set_Value(j, m_Trend.Get_Value(m_Trend.Get_Data_X(i)));
				}
			}
			else
			{
				Name.Printf("%s [%s]", pTable->Get_Name(), _TL("Trend"));
				pTable	= Parameters("TREND")->asTable();
				pTable->Destroy();
				pTable->Set_Name(Name);
				pTable->Add_Field("X"      , SG_DATATYPE_Double);
				pTable->Add_Field("Y"      , SG_DATATYPE_Double);
				pTable->Add_Field("Y_TREND", SG_DATATYPE_Double);

				for(i=0; i<m_Trend.Get_Data_Count(); i++)
				{
					pRecord	= pTable->Add_Record();
					pRecord->Set_Value(0, m_Trend.Get_Data_X(i));
					pRecord->Set_Value(1, m_Trend.Get_Data_Y(i));
					pRecord->Set_Value(2, m_Trend.Get_Value(m_Trend.Get_Data_X(i)));
				}
			}

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Trend::CTable_Trend(void)
	: CTable_Trend_Base()
{
	Set_Name		(_TL("Trend Analysis"));

	Parameters.Add_Table(
		NULL	, "TABLE"	, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Initialise();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Trend_Shapes::CTable_Trend_Shapes(void)
	: CTable_Trend_Base()
{
	Set_Name		(_TL("Trend Analysis (Shapes)"));

	Parameters.Add_Shapes(
		NULL	, "TABLE"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Initialise();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
