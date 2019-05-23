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
//                    table_calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               table_field_statistics.cpp              //
//                                                       //
//                 Copyright (C) 2014 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata.at                  //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "table_field_statistics.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Field_Statistics::CTable_Field_Statistics(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Field Statistics"));

	Set_Author		("V. Wichmann (c) 2014");

	Set_Description	(_TW(
		"The tools allows one to calculate statistics (n, min, max, range, sum, "
		"mean, variance and standard deviation) for attribute fields of tables, "
		"shapefiles or point clouds."
	));

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"		, _TL("Table"),
		_TL("The input table."),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Fields("TABLE",
		"FIELDS"	, _TL("Attributes"),
		_TL("The (numeric) fields to calculate the statistics for.")
	);

	Parameters.Add_Table("",
		"STATISTICS", _TL("Statistics"),
		_TL("The calculated statistics."),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Field_Statistics::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("TABLE")->asTable();

	int	*Features	= (int *)Parameters("FIELDS")->asPointer();
	int	nFeatures	=        Parameters("FIELDS")->asInt    ();

	if( !Features || nFeatures <= 0 )
	{
		Error_Set(_TL("No attribute fields selected!"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pStatistics	= Parameters("STATISTICS")->asTable();

	pStatistics->Destroy();
	pStatistics->Fmt_Name("%s [%s]", pTable->Get_Name(), _TL("Statistics"));
	
	pStatistics->Add_Field("field"   , SG_DATATYPE_String);
	pStatistics->Add_Field("n"       , SG_DATATYPE_Long  );
	pStatistics->Add_Field("min"     , SG_DATATYPE_Double);
	pStatistics->Add_Field("max"     , SG_DATATYPE_Double);
	pStatistics->Add_Field("range"   , SG_DATATYPE_Double);
	pStatistics->Add_Field("sum"     , SG_DATATYPE_Double);
	pStatistics->Add_Field("mean"    , SG_DATATYPE_Double);
	pStatistics->Add_Field("variance", SG_DATATYPE_Double);
	pStatistics->Add_Field("stddev"  , SG_DATATYPE_Double);

	//-----------------------------------------------------
	for(int iFeature=0; iFeature<nFeatures; iFeature++)
	{
		if( !SG_Data_Type_is_Numeric(pTable->Get_Field_Type(Features[iFeature])) )
		{
			Message_Fmt("\n%s: %s [%s]", _TL("Warning"), _TL("skipping non-numeric field"), pTable->Get_Field_Name(Features[iFeature]));
		}
		else
		{
			CSG_Table_Record *pRecord = pStatistics->Add_Record();

			pRecord->Set_Value(0, pTable->Get_Field_Name(Features[iFeature]));
			pRecord->Set_Value(1, pTable->Get_N         (Features[iFeature]));
			pRecord->Set_Value(2, pTable->Get_Minimum   (Features[iFeature]));
			pRecord->Set_Value(3, pTable->Get_Maximum   (Features[iFeature]));
			pRecord->Set_Value(4, pTable->Get_Range     (Features[iFeature]));
			pRecord->Set_Value(5, pTable->Get_Sum       (Features[iFeature]));
			pRecord->Set_Value(6, pTable->Get_Mean      (Features[iFeature]));
			pRecord->Set_Value(7, pTable->Get_Variance  (Features[iFeature]));
			pRecord->Set_Value(8, pTable->Get_StdDev    (Features[iFeature]));
		}
	}

	//-----------------------------------------------------
	return( pStatistics->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
