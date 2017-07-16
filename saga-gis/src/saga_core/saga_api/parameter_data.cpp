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
//                  parameter_data.cpp                   //
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
#include "parameters.h"
#include "data_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_MD_PARM_DATAOBJECT_CREATE		SG_T("CREATE")


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String SG_Parameter_Type_Get_Name(TSG_Parameter_Type Type)
{
	switch( Type )
	{
	default                              :	return( _TL("Parameter"       ) );

	case PARAMETER_TYPE_Node             :	return( _TL("Node"            ) );

	case PARAMETER_TYPE_Bool             :	return( _TL("Boolean"         ) );
	case PARAMETER_TYPE_Int              :	return( _TL("Integer"         ) );
	case PARAMETER_TYPE_Double           :	return( _TL("Floating point"  ) );
	case PARAMETER_TYPE_Degree           :	return( _TL("Degree"          ) );
	case PARAMETER_TYPE_Date             :	return( _TL("Date"            ) );
	case PARAMETER_TYPE_Range            :	return( _TL("Value range"     ) );
	case PARAMETER_TYPE_Choice           :	return( _TL("Choice"          ) );
	case PARAMETER_TYPE_Choices          :	return( _TL("Choices"         ) );

	case PARAMETER_TYPE_String           :	return( _TL("Text"            ) );
	case PARAMETER_TYPE_Text             :	return( _TL("Long text"       ) );
	case PARAMETER_TYPE_FilePath         :	return( _TL("File path"       ) );

	case PARAMETER_TYPE_Font             :	return( _TL("Font"            ) );
	case PARAMETER_TYPE_Color            :	return( _TL("Color"           ) );
	case PARAMETER_TYPE_Colors           :	return( _TL("Colors"          ) );
	case PARAMETER_TYPE_FixedTable       :	return( _TL("Static table"    ) );
	case PARAMETER_TYPE_Grid_System      :	return( _TL("Grid system"     ) );
	case PARAMETER_TYPE_Table_Field      :	return( _TL("Table field"     ) );
	case PARAMETER_TYPE_Table_Fields     :	return( _TL("Table fields"    ) );

	case PARAMETER_TYPE_DataObject_Output:	return( _TL("Data Object"     ) );
	case PARAMETER_TYPE_Grid             :	return( _TL("Grid"            ) );
	case PARAMETER_TYPE_Grids            :	return( _TL("Grids"           ) );
	case PARAMETER_TYPE_Table            :	return( _TL("Table"           ) );
	case PARAMETER_TYPE_Shapes           :	return( _TL("Shapes"          ) );
	case PARAMETER_TYPE_TIN              :	return( _TL("TIN"             ) );
	case PARAMETER_TYPE_PointCloud       :	return( _TL("Point Cloud"     ) );

	case PARAMETER_TYPE_Grid_List        :	return( _TL("Grid list"       ) );
	case PARAMETER_TYPE_Grids_List       :	return( _TL("Grids list"      ) );
	case PARAMETER_TYPE_Table_List       :	return( _TL("Table list"      ) );
	case PARAMETER_TYPE_Shapes_List      :	return( _TL("Shapes list"     ) );
	case PARAMETER_TYPE_TIN_List         :	return( _TL("TIN list"        ) );
	case PARAMETER_TYPE_PointCloud_List  :	return( _TL("Point Cloud list") );

	case PARAMETER_TYPE_Parameters       :	return( _TL("Parameters"      ) );
	}
}

//---------------------------------------------------------
CSG_String SG_Parameter_Type_Get_Identifier(TSG_Parameter_Type Type)
{
	switch( Type )
	{
	default                              :	return( "parameter"    );

	case PARAMETER_TYPE_Node             :	return( "node"         );

	case PARAMETER_TYPE_Bool             :	return( "boolean"      );
	case PARAMETER_TYPE_Int              :	return( "integer"      );
	case PARAMETER_TYPE_Double           :	return( "double"       );
	case PARAMETER_TYPE_Degree           :	return( "degree"       );
	case PARAMETER_TYPE_Date             :	return( "date"         );
	case PARAMETER_TYPE_Range            :	return( "range"        );
	case PARAMETER_TYPE_Choice           :	return( "choice"       );
	case PARAMETER_TYPE_Choices          :	return( "choices"      );

	case PARAMETER_TYPE_String           :	return( "text"         );
	case PARAMETER_TYPE_Text             :	return( "long_text"    );
	case PARAMETER_TYPE_FilePath         :	return( "file"         );

	case PARAMETER_TYPE_Font             :	return( "font"         );
	case PARAMETER_TYPE_Color            :	return( "color"        );
	case PARAMETER_TYPE_Colors           :	return( "colors"       );
	case PARAMETER_TYPE_FixedTable       :	return( "static_table" );
	case PARAMETER_TYPE_Grid_System      :	return( "grid_system"  );
	case PARAMETER_TYPE_Table_Field      :	return( "table_field"  );
	case PARAMETER_TYPE_Table_Fields     :	return( "table_fields" );

	case PARAMETER_TYPE_DataObject_Output:	return( "data_object"  );
	case PARAMETER_TYPE_Grid             :	return( "grid"         );
	case PARAMETER_TYPE_Grids            :	return( "grids"        );
	case PARAMETER_TYPE_Table            :	return( "table"        );
	case PARAMETER_TYPE_Shapes           :	return( "shapes"       );
	case PARAMETER_TYPE_TIN              :	return( "tin"          );
	case PARAMETER_TYPE_PointCloud       :	return( "points"       );

	case PARAMETER_TYPE_Grid_List        :	return( "grid_list"    );
	case PARAMETER_TYPE_Grids_List       :	return( "grids_list"    );
	case PARAMETER_TYPE_Table_List       :	return( "table_list"   );
	case PARAMETER_TYPE_Shapes_List      :	return( "shapes_list"  );
	case PARAMETER_TYPE_TIN_List         :	return( "tin_list"     );
	case PARAMETER_TYPE_PointCloud_List  :	return( "points_list"  );

	case PARAMETER_TYPE_Parameters       :	return( "parameters"   );
	}
}

//---------------------------------------------------------
TSG_Parameter_Type SG_Parameter_Type_Get_Type(const CSG_String &Identifier)
{
	if( !Identifier.Cmp("node"        ) )	{	return( PARAMETER_TYPE_Node             );	}
	if( !Identifier.Cmp("boolean"     ) )	{	return( PARAMETER_TYPE_Bool             );	}
	if( !Identifier.Cmp("integer"     ) )	{	return( PARAMETER_TYPE_Int              );	}
	if( !Identifier.Cmp("double"      ) )	{	return( PARAMETER_TYPE_Double           );	}
	if( !Identifier.Cmp("degree"      ) )	{	return( PARAMETER_TYPE_Degree           );	}
	if( !Identifier.Cmp("date"        ) )	{	return( PARAMETER_TYPE_Date             );	}
	if( !Identifier.Cmp("range"       ) )	{	return( PARAMETER_TYPE_Range            );	}
	if( !Identifier.Cmp("choice"      ) )	{	return( PARAMETER_TYPE_Choice           );	}
	if( !Identifier.Cmp("choices"     ) )	{	return( PARAMETER_TYPE_Choices          );	}

	if( !Identifier.Cmp("text"        ) )	{	return( PARAMETER_TYPE_String           );	}
	if( !Identifier.Cmp("long_text"   ) )	{	return( PARAMETER_TYPE_Text             );	}
	if( !Identifier.Cmp("file"        ) )	{	return( PARAMETER_TYPE_FilePath         );	}

	if( !Identifier.Cmp("font"        ) )	{	return( PARAMETER_TYPE_Font             );	}
	if( !Identifier.Cmp("color"       ) )	{	return( PARAMETER_TYPE_Color            );	}
	if( !Identifier.Cmp("colors"      ) )	{	return( PARAMETER_TYPE_Colors           );	}
	if( !Identifier.Cmp("static_table") )	{	return( PARAMETER_TYPE_FixedTable       );	}
	if( !Identifier.Cmp("grid_system" ) )	{	return( PARAMETER_TYPE_Grid_System      );	}
	if( !Identifier.Cmp("table_field" ) )	{	return( PARAMETER_TYPE_Table_Field      );	}
	if( !Identifier.Cmp("table_fields") )	{	return( PARAMETER_TYPE_Table_Fields     );	}

	if( !Identifier.Cmp("data_object" ) )	{	return( PARAMETER_TYPE_DataObject_Output);	}
	if( !Identifier.Cmp("grid"        ) )	{	return( PARAMETER_TYPE_Grid             );	}
	if( !Identifier.Cmp("grids"       ) )	{	return( PARAMETER_TYPE_Grids            );	}
	if( !Identifier.Cmp("table"       ) )	{	return( PARAMETER_TYPE_Table            );	}
	if( !Identifier.Cmp("shapes"      ) )	{	return( PARAMETER_TYPE_Shapes           );	}
	if( !Identifier.Cmp("tin"         ) )	{	return( PARAMETER_TYPE_TIN              );	}
	if( !Identifier.Cmp("points"      ) )	{	return( PARAMETER_TYPE_PointCloud       );	}

	if( !Identifier.Cmp("grid_list"   ) )	{	return( PARAMETER_TYPE_Grid_List        );	}
	if( !Identifier.Cmp("grids_list"  ) )	{	return( PARAMETER_TYPE_Grids_List       );	}
	if( !Identifier.Cmp("table_list"  ) )	{	return( PARAMETER_TYPE_Table_List       );	}
	if( !Identifier.Cmp("shapes_list" ) )	{	return( PARAMETER_TYPE_Shapes_List      );	}
	if( !Identifier.Cmp("tin_list"    ) )	{	return( PARAMETER_TYPE_TIN_List         );	}
	if( !Identifier.Cmp("points_list" ) )	{	return( PARAMETER_TYPE_PointCloud_List  );	}

	if( !Identifier.Cmp("parameters"  ) )	{	return( PARAMETER_TYPE_Parameters       );	}

	return( PARAMETER_TYPE_Undefined );
}


///////////////////////////////////////////////////////////
//														 //
//						Base Class						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Data::CSG_Parameter_Data(CSG_Parameter *pOwner, long Constraint)
{
	m_pOwner		= pOwner;
	m_Constraint	= Constraint;
}

//---------------------------------------------------------
CSG_String CSG_Parameter_Data::Get_Type_Identifier(void)	const
{
	return( SG_Parameter_Type_Get_Identifier(Get_Type()) );
}

//---------------------------------------------------------
CSG_String CSG_Parameter_Data::Get_Type_Name(void)	const
{
	return( SG_Parameter_Type_Get_Name(Get_Type()) );
}

//---------------------------------------------------------
bool CSG_Parameter_Data::Set_Value(int Value)
{
	return( false );
}

bool CSG_Parameter_Data::Set_Value(double Value)
{
	return( false );
}

bool CSG_Parameter_Data::Set_Value(const CSG_String &Value)
{
	return( false );
}

bool CSG_Parameter_Data::Set_Value(void *Value)
{
	return( false );
}

//---------------------------------------------------------
int CSG_Parameter_Data::asInt(void)	const
{
	return( 0 );
}

double CSG_Parameter_Data::asDouble(void)	const
{
	return( 0.0 );
}

void * CSG_Parameter_Data::asPointer(void)	const
{
	return( NULL );
}

const SG_Char * CSG_Parameter_Data::asString(void)
{
	return( NULL );
}

//---------------------------------------------------------
void CSG_Parameter_Data::Set_Default(int Value)
{
	m_Default.Printf("%d", Value);
}

void CSG_Parameter_Data::Set_Default(double Value)
{
	m_Default.Printf("%f", Value);
}

void CSG_Parameter_Data::Set_Default(const CSG_String &Value)
{
	m_Default	= Value;
}

//---------------------------------------------------------
bool CSG_Parameter_Data::Assign(CSG_Parameter_Data *pSource)
{
	if( pSource && Get_Type() == pSource->Get_Type() )
	{
		m_Default	= pSource->m_Default;

		On_Assign(pSource);

		return( true );
	}

	return( false );
}

void CSG_Parameter_Data::On_Assign(CSG_Parameter_Data *pSource)
{}

//---------------------------------------------------------
bool CSG_Parameter_Data::Serialize(CSG_MetaData &Entry, bool bSave)
{
	return( On_Serialize(Entry, bSave) );
}

bool CSG_Parameter_Data::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	Entry.Set_Content(SG_T("-"));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Node							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Node::CSG_Parameter_Node(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{}


///////////////////////////////////////////////////////////
//														 //
//						Bool							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Bool::CSG_Parameter_Bool(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_Value		= false;
}

//---------------------------------------------------------
bool CSG_Parameter_Bool::Set_Value(int Value)
{
	bool	bValue = Value != 0;

	if( m_Value != bValue )
	{
		m_Value		= bValue;

		return( true );
	}

	return( false );
}

bool CSG_Parameter_Bool::Set_Value(double Value)
{
	return( Set_Value((int)Value) );
}

bool CSG_Parameter_Bool::Set_Value(const CSG_String &Value)
{
	int		i;

	if( !Value.CmpNoCase("true") )
	{
		return( Set_Value(1) );
	}

	if( !Value.CmpNoCase("false") )
	{
		return( Set_Value(0) );
	}

	if( Value.asInt(i) )
	{
		return( Set_Value(i) );
	}

	return( false );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Bool::asString(void)
{
	m_String	= m_Value ? _TL("yes") : _TL("no");

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Bool::On_Assign(CSG_Parameter_Data *pSource)
{
	m_Value		= ((CSG_Parameter_Bool *)pSource)->m_Value;
}

//---------------------------------------------------------
bool CSG_Parameter_Bool::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Set_Content(m_Value ? SG_T("TRUE") : SG_T("FALSE"));
	}
	else
	{
		m_Value	= Entry.Cmp_Content(SG_T("TRUE"), true);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Value							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Value::CSG_Parameter_Value(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_Minimum	= 0.0;
	m_bMinimum	= false;

	m_Maximum	= 0.0;
	m_bMaximum	= false;
}

//---------------------------------------------------------
bool CSG_Parameter_Value::Set_Range(double Minimum, double Maximum)
{
	if( 1 )
	{
		m_Minimum		= Minimum;
		m_Maximum		= Maximum;

		switch( Get_Type() )
		{
		default:
			return( false );

		case PARAMETER_TYPE_Int:
			Set_Value(asInt());
			break;

		case PARAMETER_TYPE_Double:
		case PARAMETER_TYPE_Degree:
			Set_Value(asDouble());
			break;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Parameter_Value::Set_Minimum(double Minimum, bool bOn)
{
	if( bOn == false || (m_bMaximum && Minimum >= m_Maximum) )
	{
		m_bMinimum	= false;
	}
	else
	{
		m_bMinimum	= true;

		Set_Range(Minimum, m_Maximum);
	}
}

void CSG_Parameter_Value::Set_Maximum(double Maximum, bool bOn)
{
	if( bOn == false || (m_bMaximum && Maximum <= m_Minimum) )
	{
		m_bMaximum	= false;
	}
	else
	{
		m_bMaximum	= true;

		Set_Range(m_Minimum, Maximum);
	}
}

//---------------------------------------------------------
void CSG_Parameter_Value::On_Assign(CSG_Parameter_Data *pSource)
{
	m_Minimum	= ((CSG_Parameter_Value *)pSource)->m_Minimum;
	m_bMinimum	= ((CSG_Parameter_Value *)pSource)->m_bMinimum;

	m_Maximum	= ((CSG_Parameter_Value *)pSource)->m_Maximum;
	m_bMaximum	= ((CSG_Parameter_Value *)pSource)->m_bMaximum;
}


///////////////////////////////////////////////////////////
//														 //
//						Int								 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Int::CSG_Parameter_Int(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Value(pOwner, Constraint)
{
	m_Value		= 0;
}

//---------------------------------------------------------
bool CSG_Parameter_Int::Set_Value(int Value)
{
	if( m_bMinimum && Value < m_Minimum )
	{
		return( Set_Value((int)m_Minimum) );
	}

	if( m_bMaximum && Value > m_Maximum )
	{
		return( Set_Value((int)m_Maximum) );
	}

	if( m_Value != Value )
	{
		m_Value	= Value;

		return( true );
	}

	return( false );
}

bool CSG_Parameter_Int::Set_Value(double Value)
{
	return( Set_Value((int)Value) );
}

bool CSG_Parameter_Int::Set_Value(const CSG_String &Value)
{
	int		i;

	if( Value.asInt(i) )
	{
		return( Set_Value(i) );
	}

	return( false );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Int::asString(void)
{
	m_String.Printf(SG_T("%d"), m_Value);

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Int::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_Value::On_Assign(pSource);

	Set_Value(((CSG_Parameter_Value *)pSource)->asInt());
}

//---------------------------------------------------------
bool CSG_Parameter_Int::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Set_Content(asString());
	}
	else
	{
		return( Entry.Get_Content().asInt(m_Value) );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Double							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Double::CSG_Parameter_Double(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Value(pOwner, Constraint)
{
	m_Value		= 0.0;
}

//---------------------------------------------------------
bool CSG_Parameter_Double::Set_Value(int Value)
{
	return( Set_Value((double)Value) );
}

bool CSG_Parameter_Double::Set_Value(double Value)
{
	if( m_bMinimum && Value < m_Minimum )
	{
		return( Set_Value(m_Minimum) );
	}

	if( m_bMaximum && Value > m_Maximum )
	{
		return( Set_Value(m_Maximum) );
	}

	if( m_Value != Value )
	{
		m_Value	= Value;

		return( true );
	}

	return( false );
}

bool CSG_Parameter_Double::Set_Value(const CSG_String &Value)
{
	double	d;

	if( Value.asDouble(d) )
	{
		return( Set_Value(d) );
	}

	return( false );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Double::asString(void)
{
	m_String.Printf(SG_T("%f"), m_Value);

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Double::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_Value::On_Assign(pSource);

	Set_Value(((CSG_Parameter_Value *)pSource)->asDouble());
}

//---------------------------------------------------------
bool CSG_Parameter_Double::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Set_Content(asString());
	}
	else
	{
		return( Entry.Get_Content().asDouble(m_Value) );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Degree							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Degree::CSG_Parameter_Degree(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Double(pOwner, Constraint)
{}

//---------------------------------------------------------
bool CSG_Parameter_Degree::Set_Value(const CSG_String &Value)
{
	return( CSG_Parameter_Double::Set_Value(SG_Degree_To_Double(Value)) );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Degree::asString(void)
{
	m_String	= SG_Double_To_Degree(asDouble());

	return( m_String );
}


///////////////////////////////////////////////////////////
//														 //
//						Date							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Date::CSG_Parameter_Date(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	Set_Value(CSG_DateTime::Now().Get_JDN());
}

CSG_Parameter_Date::~CSG_Parameter_Date(void)
{}

//---------------------------------------------------------
bool CSG_Parameter_Date::Restore_Default(void)
{
	return( Set_Value(m_Default) );
}

//---------------------------------------------------------
bool CSG_Parameter_Date::Set_Value(int Value)
{
	return( Set_Value((double)Value) );
}

//---------------------------------------------------------
bool CSG_Parameter_Date::Set_Value(double Value)
{
	m_Date.Set(floor(Value) + 0.5);	// always adjust to high noon, prevents rounding problems (we're not intested in time, just date!)

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Date::Set_Value(const CSG_String &Value)
{
	return( m_Date.Parse_Date(Value) && Set_Value(m_Date.Get_JDN()) );
}

//---------------------------------------------------------
int CSG_Parameter_Date::asInt(void)	const
{
	return( (int)asDouble() );
}

//---------------------------------------------------------
double CSG_Parameter_Date::asDouble(void)	const
{
	return( m_Date.Get_JDN() );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Date::asString(void)
{
	m_String	= m_Date.Format_ISODate();

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Date::Set_Date(const CSG_DateTime &Date)
{
	m_Date	= Date;
}

//---------------------------------------------------------
void CSG_Parameter_Date::On_Assign(CSG_Parameter_Data *pSource)
{
	m_Date	= ((CSG_Parameter_Date *)pSource)->m_Date;
}

//---------------------------------------------------------
bool CSG_Parameter_Date::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Set_Content(asString());
	}
	else
	{
		Set_Value(Entry.Get_Content());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Range							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Range::CSG_Parameter_Range(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_pRange	= new CSG_Parameters;

	if( (m_Constraint & PARAMETER_INFORMATION) != 0 )
	{
		m_pLo	= m_pRange->Add_Info_Value(m_pOwner->Get_Identifier(), "MIN", "Minimum", m_pOwner->Get_Description(), PARAMETER_TYPE_Double);
		m_pHi	= m_pRange->Add_Info_Value(m_pOwner->Get_Identifier(), "MAX", "Maximum", m_pOwner->Get_Description(), PARAMETER_TYPE_Double);
	}
	else
	{
		m_pLo	= m_pRange->Add_Value     (m_pOwner->Get_Identifier(), "MIN", "Minimum", m_pOwner->Get_Description(), PARAMETER_TYPE_Double);
		m_pHi	= m_pRange->Add_Value     (m_pOwner->Get_Identifier(), "MAX", "Maximum", m_pOwner->Get_Description(), PARAMETER_TYPE_Double);
	}
}

CSG_Parameter_Range::~CSG_Parameter_Range(void)
{
	delete(m_pRange);
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Range::asString(void)
{
	m_String.Printf("%f; %f", Get_LoVal(), Get_HiVal());

	return( m_String );
}

//---------------------------------------------------------
bool CSG_Parameter_Range::Set_Value(const CSG_String &Value)
{
	return( Set_Range(Value.BeforeFirst(';').asDouble(), Value.AfterFirst(';').asDouble()) );
}

//---------------------------------------------------------
bool CSG_Parameter_Range::Set_Range(double loVal, double hiVal)
{
	bool	bResult;

	if( loVal > hiVal )
	{
		bResult	 = m_pLo->Set_Value(hiVal);
		bResult	|= m_pHi->Set_Value(loVal);
	}
	else
	{
		bResult	 = m_pLo->Set_Value(loVal);
		bResult	|= m_pHi->Set_Value(hiVal);
	}

	return( bResult );
}

//---------------------------------------------------------
bool CSG_Parameter_Range::Set_LoVal(double newValue)
{
	return( m_pLo->Set_Value(newValue) );
}

double CSG_Parameter_Range::Get_LoVal(void)
{
	return( m_pLo->asDouble() );
}

//---------------------------------------------------------
bool CSG_Parameter_Range::Set_HiVal(double newValue)
{
	return( m_pHi->Set_Value(newValue) );
}

double CSG_Parameter_Range::Get_HiVal(void)
{
	return( m_pHi->asDouble() );
}

//---------------------------------------------------------
bool CSG_Parameter_Range::Restore_Default(void)
{
	return( m_pLo->Restore_Default() && m_pHi->Restore_Default() );
}

//---------------------------------------------------------
void CSG_Parameter_Range::On_Assign(CSG_Parameter_Data *pSource)
{
	m_pLo->Assign(((CSG_Parameter_Range *)pSource)->m_pLo);
	m_pHi->Assign(((CSG_Parameter_Range *)pSource)->m_pHi);
}

//---------------------------------------------------------
bool CSG_Parameter_Range::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Fmt_Content(SG_T("%f; %f"), Get_LoVal(), Get_HiVal());

		return( true );
	}
	else
	{
		double		loVal, hiVal;
		CSG_String	s(Entry.Get_Content());

		if( s.BeforeFirst(SG_T(';')).asDouble(loVal) && s.AfterFirst(SG_T(';')).asDouble(hiVal) )
		{
			return( Set_Range(loVal, hiVal) );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Choice							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Choice::CSG_Parameter_Choice(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Int(pOwner, Constraint)
{}

//---------------------------------------------------------
bool CSG_Parameter_Choice::Set_Value(const CSG_String &Value)
{
	int		i;

	for(i=0; i<m_Items.Get_Count(); i++)
	{
		if( m_Items[i].Cmp(Value) == 0 )
		{
			m_Value	= i;

			return( true );
		}
	}

	if( Value.asInt(i) )
	{
		m_Value	= i;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Choice::asString(void)
{
	m_String	= Get_Item(m_Value) ? Get_Item(m_Value) : _TL("<no choice available>");

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Choice::Set_Items(const SG_Char *String)
{
	m_Items.Clear();

	if( String && *String != '\0' )
	{
		CSG_String	Items(String);

		while( Items.Length() > 0 )
		{
			CSG_String	Item(Items.BeforeFirst('|'));

			if( Item.Length() )
			{
				m_Items	+= Item;
			}

			Items	= Items.AfterFirst('|');
		}
	}

	if( m_Items.Get_Count() <= 0 )
	{
		m_Items	+= _TL("<not set>");

		Set_Minimum(              0, true);
		Set_Maximum(Get_Count() - 1, true);

		CSG_Parameter_Int::Set_Value(0);
	}
	else
	{
		Set_Minimum(              0, true);
		Set_Maximum(Get_Count() - 1, true);

		CSG_Parameter_Int::Set_Value(m_Value);
	}
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Choice::Get_Item(int Index)	const
{
	if( Index >= 0 && Index < m_Items.Get_Count() )
	{
		const SG_Char	*Item	= m_Items[Index].c_str();

		if( *Item == SG_T('{') )
		{
			do	{	Item++;	}	while( *Item != SG_T('\0') && *Item != SG_T('}') );

			if( *Item == SG_T('\0') )
			{
				return( m_Items[Index].c_str() );
			}

			Item++;
		}

		return( Item );
	}

	return( NULL );
}

//---------------------------------------------------------
CSG_String CSG_Parameter_Choice::Get_Item_Data(int Index)	const
{
	CSG_String	Value;

	if( Index >= 0 && Index < m_Items.Get_Count() )
	{
		const SG_Char	*Item	= m_Items[Index].c_str();

		if( *Item == SG_T('{') )
		{
			Item++;

			do
			{
				Value	+= *(Item++);
			}
			while( *Item && *Item != SG_T('}') );
		}
	}

	return( Value );
}

//---------------------------------------------------------
bool CSG_Parameter_Choice::Get_Data(int        &Value)	const
{
	CSG_String	sValue;

	if( Get_Data(sValue) )
	{
		return( sValue.asInt(Value) );
	}

	return( false );
}

bool CSG_Parameter_Choice::Get_Data(double     &Value)	const
{
	CSG_String	sValue;

	if( Get_Data(sValue) )
	{
		return( sValue.asDouble(Value) );
	}

	return( false );
}

bool CSG_Parameter_Choice::Get_Data(CSG_String &Value)	const
{
	if( m_Value >= 0 && m_Value < m_Items.Get_Count() )
	{
		const SG_Char	*Item	= m_Items[m_Value].c_str();

		if( *Item == SG_T('{') )
		{
			Item++;

			Value.Clear();

			do
			{
				Value	+= *(Item++);
			}
			while( *Item && *Item != SG_T('}') );

			return( Value.Length() > 0 );
		}
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Parameter_Choice::On_Assign(CSG_Parameter_Data *pSource)
{
	m_Items	= ((CSG_Parameter_Choice *)pSource)->m_Items;

	CSG_Parameter_Int::On_Assign(pSource);
}

//---------------------------------------------------------
bool CSG_Parameter_Choice::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Add_Property("index", m_Value);

		Entry.Set_Content(asString());

		return( true );
	}
	else
	{
		int	Index;

		return( (Entry.Get_Property("index", Index) || Entry.Get_Content().asInt(Index)) && CSG_Parameter_Int::Set_Value(Index) );
	}
}


///////////////////////////////////////////////////////////
//														 //
//						Choice							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Choices::CSG_Parameter_Choices(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{}

//---------------------------------------------------------
bool CSG_Parameter_Choices::Set_Value(const CSG_String &Value)
{
	CSG_String_Tokenizer	Tokens(Value, ";");

	m_Selection.Destroy();

	while( Tokens.Has_More_Tokens() )
	{
		int	Index;

		if( Tokens.Get_Next_Token().asInt(Index) )
		{
			Select(Index);
		}
	}

	return( true );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Choices::asString(void)
{
	m_String.Clear();

	for(size_t i=0; i<m_Selection.Get_Size(); i++)
	{
		m_String	+= CSG_String::Format("%d;", m_Selection[i]);
	}

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Choices::Set_Items(const CSG_String &Items)
{
	Del_Items();

	CSG_String_Tokenizer	Tokens(Items, "|");

	while( Tokens.Has_More_Tokens() )
	{
		Add_Item(Tokens.Get_Next_Token());
	}
}

//---------------------------------------------------------
void CSG_Parameter_Choices::Set_Items(const CSG_Strings &Items)
{
	Del_Items();

	for(int i=0; i<Items.Get_Count(); i++)
	{
		Add_Item(Items[i]);
	}
}

//---------------------------------------------------------
void CSG_Parameter_Choices::Del_Items(void)
{
	m_Items[0].Clear();
	m_Items[1].Clear();

	m_Selection.Destroy();
}

//---------------------------------------------------------
void CSG_Parameter_Choices::Add_Item(const CSG_String &Item, const CSG_String &Data)
{
	m_Items[0]	+= Item;
	m_Items[1]	+= Data;
}

//---------------------------------------------------------
bool CSG_Parameter_Choices::is_Selected(int Index)
{
	for(size_t i=0; i<m_Selection.Get_Size(); i++)
	{
		if( Index == m_Selection[i] )
		{
			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameter_Choices::Select(int Index, bool bSelect)
{
	if( Index >= 0 && Index < Get_Item_Count() )
	{
		if( bSelect && !is_Selected(Index) )
		{
			m_Selection	+= Index;
		}
		else if( !bSelect )
		{
			for(size_t i=0; i<m_Selection.Get_Size(); i++)
			{
				if( Index == m_Selection[i] )
				{
					for(size_t j=i+1; j<m_Selection.Get_Size(); i++, j++)
					{
						m_Selection[i]	= m_Selection[j];
					}

					m_Selection.Dec_Array();
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameter_Choices::Clr_Selection(void)
{
	m_Selection.Destroy();

	return( true );
}

//---------------------------------------------------------
void CSG_Parameter_Choices::On_Assign(CSG_Parameter_Data *pSource)
{
	m_Items[0]	= ((CSG_Parameter_Choices *)pSource)->m_Items[0];
	m_Items[1]	= ((CSG_Parameter_Choices *)pSource)->m_Items[1];

	m_Selection	= ((CSG_Parameter_Choices *)pSource)->m_Selection;

	CSG_Parameter_Data::On_Assign(pSource);
}

//---------------------------------------------------------
bool CSG_Parameter_Choices::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Set_Content(asString());

		return( true );
	}
	else
	{
		return( Set_Value(Entry.Get_Content()) );
	}
}


///////////////////////////////////////////////////////////
//														 //
//						String							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_String::CSG_Parameter_String(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_bPassword	= false;
}

//---------------------------------------------------------
bool CSG_Parameter_String::is_Valid(void)	const
{
	return( m_String.Length() > 0 );
}

//---------------------------------------------------------
bool CSG_Parameter_String::Set_Value(const CSG_String &Value)
{
	if( m_String.Cmp(Value) )
	{
		m_String	= Value;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Parameter_String::On_Assign(CSG_Parameter_Data *pSource)
{
	m_String	= ((CSG_Parameter_String *)pSource)->m_String.c_str();

	m_bPassword	= ((CSG_Parameter_String *)pSource)->m_bPassword;
}

//---------------------------------------------------------
bool CSG_Parameter_String::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Set_Content(m_String);
	}
	else
	{
		m_String	= Entry.Get_Content();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Text							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Text::CSG_Parameter_Text(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_String(pOwner, Constraint)
{}


///////////////////////////////////////////////////////////
//														 //
//						FilePath						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_File_Name::CSG_Parameter_File_Name(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_String(pOwner, Constraint)
{
	m_Filter.Printf(SG_T("%s|*.*"), _TL("All Files"));

	m_bSave			= false;
	m_bMultiple		= false;
	m_bDirectory	= false;
}

//---------------------------------------------------------
void CSG_Parameter_File_Name::Set_Filter(const SG_Char *Filter)
{
	if( Filter )
	{
		m_Filter	= Filter;
	}
	else
	{
		m_Filter.Printf(SG_T("%s|*.*"), _TL("All Files"));
	}
}

const SG_Char *  CSG_Parameter_File_Name::Get_Filter(void)	const
{
	return( m_Filter.c_str() );
}

//---------------------------------------------------------
void CSG_Parameter_File_Name::Set_Flag_Save(bool bFlag)
{
	m_bSave			= bFlag;
}

void CSG_Parameter_File_Name::Set_Flag_Multiple(bool bFlag)
{
	m_bMultiple		= bFlag;
}

void CSG_Parameter_File_Name::Set_Flag_Directory(bool bFlag)
{
	m_bDirectory	= bFlag;
}

//---------------------------------------------------------
bool CSG_Parameter_File_Name::Get_FilePaths(CSG_Strings &FilePaths)	const
{
	FilePaths.Clear();

	if( m_String.Length() > 0 )
	{
		if( !m_bMultiple )
		{
			FilePaths.Add(m_String);
		}
		else if( m_String[0] != '\"' )
		{
			FilePaths.Add(m_String);
		}
		else
		{
			CSG_String	s(m_String);

			while( s.Length() > 2 )
			{
				s	= s.AfterFirst('\"');
				FilePaths.Add(s.BeforeFirst('\"'));
				s	= s.AfterFirst('\"');
			}
		}
	}

	return( FilePaths.Get_Count() > 0 );
}

//---------------------------------------------------------
void CSG_Parameter_File_Name::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_String::On_Assign(pSource);

	Set_Filter(((CSG_Parameter_File_Name *)pSource)->m_Filter.c_str());

	m_bSave			= ((CSG_Parameter_File_Name *)pSource)->m_bSave;
	m_bMultiple		= ((CSG_Parameter_File_Name *)pSource)->m_bMultiple;
	m_bDirectory	= ((CSG_Parameter_File_Name *)pSource)->m_bDirectory;
}


///////////////////////////////////////////////////////////
//														 //
//						Font							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Font::CSG_Parameter_Font(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	Restore_Default();
}

//---------------------------------------------------------
bool CSG_Parameter_Font::Restore_Default(void)
{
	m_Color		= SG_GET_RGB(0, 0, 0);
	m_Font		= SG_T("0;-13;0;0;0;400;0;0;0;0;3;2;1;34;Arial");
	m_String	= SG_T("Arial");

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Font::Set_Value(int Value)
{
	m_Color	= Value;

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Font::Set_Value(const CSG_String &Value)
{
	if( Value.Length() > 0 )
	{
		m_Font		= Value;
		m_String	= m_Font.AfterLast(SG_T(';'));
	}
	else
	{
		Restore_Default();
	}

	return( true );
}

//---------------------------------------------------------
void CSG_Parameter_Font::On_Assign(CSG_Parameter_Data *pSource)
{
	if( pSource && pSource->Get_Type() == Get_Type() )
	{
		m_Color		= ((CSG_Parameter_Font *)pSource)->m_Color;
		m_Font		= ((CSG_Parameter_Font *)pSource)->m_Font;
		m_String	= ((CSG_Parameter_Font *)pSource)->m_String;
	}
}

//---------------------------------------------------------
bool CSG_Parameter_Font::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Add_Child(SG_T("COLOR")	, CSG_String::Format(SG_T("R%03d G%03d B%03d"),
			SG_GET_R(m_Color),
			SG_GET_G(m_Color),
			SG_GET_B(m_Color)
		));

		Entry.Add_Child(SG_T("FONT")	, m_Font);
	}
	else
	{
		CSG_MetaData	*pEntry;

		if( (pEntry = Entry.Get_Child(SG_T("COLOR"))) != NULL )
		{
			Set_Value((int)SG_GET_RGB(
				pEntry->Get_Content().AfterFirst(SG_T('R')).asInt(),
				pEntry->Get_Content().AfterFirst(SG_T('G')).asInt(),
				pEntry->Get_Content().AfterFirst(SG_T('B')).asInt()
			));
		}

		if( (pEntry = Entry.Get_Child(SG_T("FONT"))) != NULL )
		{
			Set_Value(pEntry->Get_Content());
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Color							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Color::CSG_Parameter_Color(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Int(pOwner, Constraint)
{}

//---------------------------------------------------------
bool CSG_Parameter_Color::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Fmt_Content(SG_T("R%03d G%03d B%03d"), SG_GET_R(m_Value), SG_GET_G(m_Value), SG_GET_B(m_Value));
	}
	else
	{
		m_Value	= SG_GET_RGB(
			Entry.Get_Content().AfterFirst(SG_T('R')).asInt(),
			Entry.Get_Content().AfterFirst(SG_T('G')).asInt(),
			Entry.Get_Content().AfterFirst(SG_T('B')).asInt()
		);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Colors							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Colors::CSG_Parameter_Colors(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Colors::asString(void)
{
	m_String.Printf(SG_T("%d %s"), m_Colors.Get_Count(), _TL("colors"));

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Colors::On_Assign(CSG_Parameter_Data *pSource)
{
	m_Colors.Assign(&((CSG_Parameter_Colors *)pSource)->m_Colors);
}

//---------------------------------------------------------
bool CSG_Parameter_Colors::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		for(int i=0; i<m_Colors.Get_Count(); i++)
		{
			Entry.Add_Child(SG_T("COLOR"), CSG_String::Format(SG_T("R%03d G%03d B%03d"), m_Colors.Get_Red(i), m_Colors.Get_Green(i), m_Colors.Get_Blue(i)));
		}
	}
	else
	{
		if( Entry.Get_Children_Count() <= 1 )
		{
			return( false );
		}

		m_Colors.Set_Count(Entry.Get_Children_Count());

		for(int i=0; i<m_Colors.Get_Count(); i++)
		{
			CSG_String	s(Entry.Get_Child(i)->Get_Content());

			m_Colors.Set_Red  (i, s.AfterFirst(SG_T('R')).asInt());
			m_Colors.Set_Green(i, s.AfterFirst(SG_T('G')).asInt());
			m_Colors.Set_Blue (i, s.AfterFirst(SG_T('B')).asInt());
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						FixedTable						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Fixed_Table::CSG_Parameter_Fixed_Table(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_Table.Set_Name(_TL("Table"));
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Fixed_Table::asString(void)
{
	m_String.Printf(SG_T("%s (%s: %d, %s: %d)"), m_Table.Get_Name(), _TL("columns"), m_Table.Get_Field_Count(), _TL("rows"), m_Table.Get_Record_Count());

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Fixed_Table::On_Assign(CSG_Parameter_Data *pSource)
{
	m_Table.Assign(&((CSG_Parameter_Fixed_Table *)pSource)->m_Table);
}

//---------------------------------------------------------
bool CSG_Parameter_Fixed_Table::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	int				iField, iRecord;
	CSG_MetaData	*pNode, *pEntry;

	if( bSave )
	{
		pNode	= Entry.Add_Child("FIELDS");

		for(iField=0; iField<m_Table.Get_Field_Count(); iField++)
		{
			pEntry	= pNode->Add_Child("FIELD", m_Table.Get_Field_Name(iField));
			pEntry	->Set_Property("type" , gSG_Data_Type_Identifier[m_Table.Get_Field_Type(iField)]);
		}

		pNode	= Entry.Add_Child("RECORDS");

		for(iRecord=0; iRecord<m_Table.Get_Count(); iRecord++)
		{
			pEntry	= pNode->Add_Child("RECORD");

			CSG_Table_Record	*pRecord	= m_Table.Get_Record(iRecord);

			for(iField=0; iField<m_Table.Get_Field_Count(); iField++)
			{
				pEntry->Add_Child("FIELD", pRecord->asString(iField));
			}
		}
	}
	else
	{
		CSG_Table	t;

		if( (pNode = Entry.Get_Child("FIELDS")) == NULL )
		{
			return( false );
		}

		for(iField=0; iField<pNode->Get_Children_Count(); iField++)
		{
			TSG_Data_Type	type	= SG_DATATYPE_String;

			CSG_String	s;

			if( pNode->Get_Child(iField)->Get_Property("type", s) )
			{
				type	= SG_Data_Type_Get_Type(s);
			}

			t.Add_Field(pNode->Get_Child(iField)->Get_Content(), type == SG_DATATYPE_Undefined ? SG_DATATYPE_String : type);
		}

		if( (pNode = Entry.Get_Child("RECORDS")) == NULL )
		{
			return( false );
		}

		for(iRecord=0; iRecord<pNode->Get_Children_Count(); iRecord++)
		{
			pEntry	= pNode->Get_Child(iRecord);

			CSG_Table_Record	*pRecord	= t.Add_Record();

			for(iField=0; iField<pEntry->Get_Children_Count(); iField++)
			{
				pRecord->Set_Value(iField, pEntry->Get_Child(iField)->Get_Content());
			}
		}

		return( m_Table.Assign_Values(&t) );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Grid_System						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Grid_System::CSG_Parameter_Grid_System(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{}

//---------------------------------------------------------
bool CSG_Parameter_Grid_System::Set_Value(void *Value)
{
	//-----------------------------------------------------
	CSG_Grid_System	Invalid;

	if( Value == NULL )
	{
		Value	= &Invalid;
	}

	//-----------------------------------------------------
	if( !m_System.is_Equal(*((CSG_Grid_System *)Value)) )
	{
		m_System.Assign(*((CSG_Grid_System *)Value));

		CSG_Data_Manager *pManager    = m_pOwner->Get_Manager();

		for(int i=0; i<m_pOwner->Get_Children_Count(); i++)
		{
			CSG_Parameter	*pParameter	= m_pOwner->Get_Child(i);

			//--------------------------------------------
			if( pParameter->is_DataObject() )
			{
				CSG_Data_Object	*pObject	= pParameter->asDataObject();

				bool	bInvalid	= !m_System.is_Valid() || !(pManager && pManager->Exists(pObject));

				if( !bInvalid && pObject != DATAOBJECT_NOTSET && pObject != DATAOBJECT_CREATE )
				{
					switch( pObject->Get_ObjectType() )
					{
					case SG_DATAOBJECT_TYPE_Grid : bInvalid = !m_System.is_Equal(((CSG_Grid  *)pObject)->Get_System()); break;
					case SG_DATAOBJECT_TYPE_Grids: bInvalid = !m_System.is_Equal(((CSG_Grids *)pObject)->Get_System()); break;
					default: break;
					}
				}

				if( bInvalid )
				{
					pParameter->Set_Value(DATAOBJECT_NOTSET);
				}
			}

			//--------------------------------------------
			else if( pParameter->is_DataObject_List() )
			{
				CSG_Parameter_List	*pList	= pParameter->asList();

				for(int j=pList->Get_Item_Count()-1; j>=0; j--)
				{
					CSG_Data_Object	*pObject	= pList->Get_Item(j);

					bool	bInvalid	= !m_System.is_Valid() || !(pManager && pManager->Exists(pObject));

					if( !bInvalid && pObject != DATAOBJECT_NOTSET && pObject != DATAOBJECT_CREATE )
					{
						switch( pObject->Get_ObjectType() )
						{
						case SG_DATAOBJECT_TYPE_Grid : bInvalid = !m_System.is_Equal(((CSG_Grid  *)pObject)->Get_System()); break;
						case SG_DATAOBJECT_TYPE_Grids: bInvalid = !m_System.is_Equal(((CSG_Grids *)pObject)->Get_System()); break;
						default: break;
						}
					}

					if( bInvalid )
					{
						pList->Del_Item(j);
					}
				}
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Grid_System::asString(void)
{
	m_String	= m_System.Get_Name();

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Grid_System::On_Assign(CSG_Parameter_Data *pSource)
{
	m_System	= ((CSG_Parameter_Grid_System *)pSource)->m_System;
}

//---------------------------------------------------------
bool CSG_Parameter_Grid_System::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Add_Child(SG_T("CELLSIZE"), m_System.Get_Cellsize());
		Entry.Add_Child(SG_T("XMIN")    , m_System.Get_Extent().Get_XMin());
		Entry.Add_Child(SG_T("XMAX")    , m_System.Get_Extent().Get_XMax());
		Entry.Add_Child(SG_T("YMIN")    , m_System.Get_Extent().Get_YMin());
		Entry.Add_Child(SG_T("YMAX")    , m_System.Get_Extent().Get_YMax());
	}
	else
	{
		double		Cellsize;
		TSG_Rect	Extent;

		Cellsize	= Entry.Get_Child(SG_T("CELLSIZE"))->Get_Content().asDouble();
		Extent.xMin	= Entry.Get_Child(SG_T("XMIN"))    ->Get_Content().asDouble();
		Extent.xMax	= Entry.Get_Child(SG_T("XMAX"))    ->Get_Content().asDouble();
		Extent.yMin	= Entry.Get_Child(SG_T("YMIN"))    ->Get_Content().asDouble();
		Extent.yMax	= Entry.Get_Child(SG_T("YMAX"))    ->Get_Content().asDouble();

		m_System.Assign(Cellsize, CSG_Rect(Extent));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//					Table Field							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Table_Field::CSG_Parameter_Table_Field(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Int(pOwner, Constraint)
{
	m_Default		= -1;
}

//---------------------------------------------------------
bool CSG_Parameter_Table_Field::Add_Default(double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	if( m_Default < 0 && (m_Constraint & PARAMETER_OPTIONAL) != 0 )
	{
		m_Default	= m_pOwner->Get_Children_Count();

		m_pOwner->Get_Owner()->Add_Double(
			m_pOwner->Get_Identifier(),
			CSG_String::Format("%s_DEFAULT", m_pOwner->Get_Identifier()),
			_TL("Default"), _TL("default value if no attribute has been selected"),
			Value, Minimum, bMinimum, Maximum, bMaximum
		);
	}

	return( m_Default >= 0 );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Table_Field::asString(void)
{
	CSG_Table	*pTable;

	if( (pTable = Get_Table()) != NULL )
	{
		if( m_Value >= 0 && m_Value < pTable->Get_Field_Count() )
		{
			return( m_String = pTable->Get_Field_Name(m_Value) );
		}

		return( m_String = _TL("<not set>") );
	}

	return( m_String = _TL("<no attributes>") );
}

//---------------------------------------------------------
double CSG_Parameter_Table_Field::asDouble(void) const
{
	return( m_pOwner->Get_Child(m_Default) ? m_pOwner->Get_Child(m_Default)->asDouble() : CSG_Parameter_Int::asDouble() );
}

//---------------------------------------------------------
bool CSG_Parameter_Table_Field::Set_Value(int Value)
{
	CSG_Table	*pTable	= Get_Table();

	m_Value	= Value;

	if( pTable && pTable->Get_Field_Count() > 0 && m_Value >= 0 )
	{
		if( m_Value >= pTable->Get_Field_Count() )
		{
			m_Value	= !m_pOwner->is_Optional() ? pTable->Get_Field_Count() - 1 : -1;
		}
	}
	else
	{
		m_Value	= -1;
	}

	if( m_pOwner->Get_Child(m_Default) )
	{
		m_pOwner->Get_Child(m_Default)->Set_Enabled(m_Value < 0);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Table_Field::Set_Value(const CSG_String &Value)
{
	CSG_Table	*pTable;

	if( Value.Length() > 0 && (pTable = Get_Table()) != NULL )
	{
		for(int i=0; i<pTable->Get_Field_Count(); i++)
		{
			if( Value.CmpNoCase(pTable->Get_Field_Name(i)) == 0 )
			{
				m_Value	= i;

				return( true );
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
CSG_Table * CSG_Parameter_Table_Field::Get_Table(void)	const
{
	CSG_Table		*pTable;
	CSG_Parameter	*pParent;

	pTable		= NULL;

	if( (pParent = m_pOwner->Get_Parent()) != NULL )
	{
		switch( m_pOwner->Get_Parent()->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Table:
		case PARAMETER_TYPE_Shapes:
		case PARAMETER_TYPE_TIN:
		case PARAMETER_TYPE_PointCloud:
			pTable	= pParent->asTable();
			break;
		}
	}

	return( pTable && pTable != DATAOBJECT_CREATE && pTable->Get_Field_Count() > 0 ? pTable : NULL );
}

//---------------------------------------------------------
void CSG_Parameter_Table_Field::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_Int::On_Assign(pSource);

	m_Default	= ((CSG_Parameter_Table_Field *)pSource)->m_Default;
}

//---------------------------------------------------------
bool CSG_Parameter_Table_Field::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Set_Property("index", asInt());
		Entry.Set_Content(asString());
	}
	else
	{
		int	idx;

		if( Entry.Get_Property("index", idx) )	// we require this check for backward compatibility, "index" was first introduced with SAGA 2.2.3 (r2671)
		{
			return( Set_Value(idx) );
		}
		else
		{
			return( Set_Value(Entry.Get_Content()) );
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//					Table Fields						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Table_Fields::CSG_Parameter_Table_Fields(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_nFields	= 0;
	m_Fields	= NULL;
}

//---------------------------------------------------------
CSG_Parameter_Table_Fields::~CSG_Parameter_Table_Fields(void)
{
	SG_FREE_SAFE(m_Fields);
}

//---------------------------------------------------------
bool CSG_Parameter_Table_Fields::Set_Value(const CSG_String &Value)
{
	CSG_Table	*pTable	= Get_Table();

	if( !pTable || pTable->Get_Field_Count() <= 0 )
	{
		SG_FREE_SAFE(m_Fields);
		m_nFields	= 0;
		m_String	= _TL("<no attributes>");

		return( false );
	}

	m_Fields	= (int *)SG_Realloc(m_Fields, pTable->Get_Field_Count() * sizeof(int));

	memset(m_Fields, 0, pTable->Get_Field_Count() * sizeof(int));

	//-----------------------------------------------------
	int		iField;

	CSG_String	List(Value);	List.Replace(";", ",");

	while( List.Length() > 0 )
	{
		CSG_String	sValue	= List.BeforeFirst(',');

		if( sValue.asInt(iField) && iField >= 0 && iField < pTable->Get_Field_Count() )
		{
			m_Fields[iField]	= 1;
		}
		else if( sValue.Length() > 0 )
		{
			sValue.Trim();

			if( sValue[0] == '[' )
			{
				sValue	= sValue.AfterFirst('[').BeforeLast(']');
			}

			for(iField=0; iField<pTable->Get_Field_Count(); iField++)
			{
				if( sValue.CmpNoCase(pTable->Get_Field_Name(iField)) == 0 )
				{
					m_Fields[iField]	= 1;

					break;
				}
			}
		}

		List	= List.AfterFirst(',');
	}

	//-----------------------------------------------------
	m_String.Clear();

	for(iField=0, m_nFields=0; iField<pTable->Get_Field_Count(); iField++)
	{
		if( m_Fields[iField] != 0 )
		{
			m_Fields[m_nFields++]	= iField;

			m_String	+= CSG_String::Format(m_String.Length() ? SG_T(",%d") : SG_T("%d"), iField);
		}
	}

	if( m_nFields <= 0 )
	{
		m_String	= _TL("<no attributes>");
	}

	return( true );
}

//---------------------------------------------------------
CSG_Table * CSG_Parameter_Table_Fields::Get_Table(void)	const
{
	CSG_Table		*pTable		= NULL;
	CSG_Parameter	*pParent	= m_pOwner->Get_Parent();

	if( pParent )
	{
		switch( m_pOwner->Get_Parent()->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Table:
		case PARAMETER_TYPE_Shapes:
		case PARAMETER_TYPE_TIN:
		case PARAMETER_TYPE_PointCloud:
			pTable	= pParent->asTable();
			break;
		}
	}

	return( pTable && pTable != DATAOBJECT_CREATE && pTable->Get_Field_Count() > 0 ? pTable : NULL );
}

//---------------------------------------------------------
void CSG_Parameter_Table_Fields::On_Assign(CSG_Parameter_Data *pSource)
{
	Set_Value(pSource->asString());
}

//---------------------------------------------------------
bool CSG_Parameter_Table_Fields::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Set_Content(m_String);
	}
	else
	{
		m_String	= Entry.Get_Content();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						DataObject						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Data_Object::CSG_Parameter_Data_Object(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_pDataObject	= NULL;
}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object::is_Valid(void)	const
{
	return(	m_pOwner->is_Optional() || (m_pDataObject && m_pDataObject->is_Valid()) );
}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object::Set_Value(void *Value)
{
	if( m_pDataObject != Value )
	{
		m_pDataObject	= (CSG_Data_Object *)Value;
	}

	return( true );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Data_Object::asString(void)
{
	if( m_pDataObject == DATAOBJECT_NOTSET )
	{
		m_String	= m_pOwner->is_Output() && !m_pOwner->is_Optional()
					? _TL("<create>")
					: _TL("<not set>");
	}
	else if( m_pDataObject == DATAOBJECT_CREATE )
	{
		m_String	= _TL("<create>");
	}
	else
	{
		m_String	= m_pDataObject->Get_Name();
	}

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Data_Object::On_Assign(CSG_Parameter_Data *pSource)
{
	m_pDataObject	= ((CSG_Parameter_Data_Object *)pSource)->m_pDataObject;
}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		if(	m_pDataObject == DATAOBJECT_CREATE )
		{
			Entry.Set_Content("CREATE");
		}
		else if( m_pDataObject == DATAOBJECT_NOTSET )//|| !SG_File_Exists(m_pDataObject->Get_File_Name(false)) )
		{
			Entry.Set_Content("NOT SET");
		}
		else
		{
			Entry.Set_Content(m_pDataObject->Get_File_Name(false));
		}
	}
	else
	{
		if( Entry.Cmp_Content("CREATE") )
		{
			Set_Value(DATAOBJECT_CREATE);
		}
		else if( Entry.Cmp_Content("NOT SET") )
		{
			Set_Value(DATAOBJECT_NOTSET);
		}
		else
		{
			Set_Value(m_pOwner->Get_Manager() ? m_pOwner->Get_Manager()->Find(Entry.Get_Content(), false) : NULL);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//					DataObject_Output					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Data_Object_Output::CSG_Parameter_Data_Object_Output(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data_Object(pOwner, Constraint)
{
	m_Type	= SG_DATAOBJECT_TYPE_Undefined;
}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object_Output::Set_Value(void *Value)
{
	CSG_Data_Object	*pDataObject	= (CSG_Data_Object *)Value;

	if( pDataObject == DATAOBJECT_CREATE )
	{
		pDataObject	= NULL;
	}

	if( m_pDataObject != pDataObject && (pDataObject == NULL || pDataObject->Get_ObjectType() == m_Type) )
	{
		m_pDataObject	= pDataObject;

		if( m_pOwner->Get_Manager() )
		{
			m_pOwner->Get_Manager()->Add(m_pDataObject);

			if( m_pOwner->Get_Manager() == &SG_Get_Data_Manager() )	// prevent that local data manager send their data objects to gui
			{
				SG_UI_DataObject_Add(m_pDataObject, false);
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object_Output::Set_DataObject_Type(TSG_Data_Object_Type Type)
{
	if( m_Type == SG_DATAOBJECT_TYPE_Undefined )	// m_Type should not be changed after set once!!!...
	{
		switch( Type )
		{
		case SG_DATAOBJECT_TYPE_Grid      :
		case SG_DATAOBJECT_TYPE_Grids     :
		case SG_DATAOBJECT_TYPE_Table     :
		case SG_DATAOBJECT_TYPE_Shapes    :
		case SG_DATAOBJECT_TYPE_TIN       :
		case SG_DATAOBJECT_TYPE_PointCloud:
			m_Type	= Type;

			return( true );

		default:
			return( false );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Grid							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Grid::CSG_Parameter_Grid(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data_Object(pOwner, Constraint)
{
	m_Type		= SG_DATATYPE_Undefined;

	m_Default	= -1;
}

//---------------------------------------------------------
void CSG_Parameter_Grid::Set_Preferred_Type(TSG_Data_Type Type)
{
	m_Type	= Type;
}

//---------------------------------------------------------
CSG_Grid_System * CSG_Parameter_Grid::Get_System(void)	const
{
	if( m_pOwner->Get_Parent() && m_pOwner->Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		return( m_pOwner->Get_Parent()->asGrid_System() );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Parameter_Grid::Add_Default(double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	if( m_Default < 0 && (Get_Constraint() & PARAMETER_INPUT) && (Get_Constraint() & PARAMETER_OPTIONAL) )
	{
		m_Default	= m_pOwner->Get_Children_Count();

		m_pOwner->Get_Owner()->Add_Double(
			m_pOwner->Get_Identifier(),
			CSG_String::Format("%s_DEFAULT", m_pOwner->Get_Identifier()),
			_TL("Default"), _TL("default value if no grid has been selected"),
			Value, Minimum, bMinimum, Maximum, bMaximum
		);
	}

	return( m_Default >= 0 );
}

//---------------------------------------------------------
bool CSG_Parameter_Grid::Set_Value(void *Value)
{
	if( Value == m_pDataObject )	// nothing to do
	{
		return( true );
	}

	//-----------------------------------------------------
	if( Value != DATAOBJECT_NOTSET && Value != DATAOBJECT_CREATE && Get_System() )// && m_pOwner->Get_Manager()
	{
		CSG_Grid_System	System	= Get_Type() == PARAMETER_TYPE_Grid
			? ((CSG_Grid  *)Value)->Get_System()
			: ((CSG_Grids *)Value)->Get_System();

		if( !Get_System()->is_Equal(System) )
		{
			for(int i=0; i<m_pOwner->Get_Parent()->Get_Children_Count(); i++)
			{
				CSG_Parameter	*pChild	= m_pOwner->Get_Parent()->Get_Child(i);

				if( pChild->Get_Type() == PARAMETER_TYPE_Grid
				||  pChild->Get_Type() == PARAMETER_TYPE_Grids )
				{
					if( pChild->asDataObject() != DATAOBJECT_NOTSET
					&&  pChild->asDataObject() != DATAOBJECT_CREATE
					&&  pChild->asDataObject() != m_pDataObject )
					{
						return( false );
					}
				}

				if( pChild->is_DataObject_List() && pChild->asList()->Get_Item_Count() > 0 )
				{
					if( (pChild->Get_Type() == PARAMETER_TYPE_Grid_List  && pChild->asGridList ()->Get_System())
					||  (pChild->Get_Type() == PARAMETER_TYPE_Grids_List && pChild->asGridsList()->Get_System()) )
					{
						return( false );
					}
				}
			}

			Get_System()->Assign(((CSG_Grid *)Value)->Get_System());
		}
	}

	//-----------------------------------------------------
	m_pDataObject	= (CSG_Data_Object *)Value;

	if( m_pOwner->Get_Child(m_Default) )
	{
		m_pOwner->Get_Child(m_Default)->Set_Enabled(m_pDataObject == DATAOBJECT_NOTSET);
	}

	return( true );
}

//---------------------------------------------------------
int	CSG_Parameter_Grid::asInt(void) const
{
	return( m_pOwner->Get_Child(m_Default) ? m_pOwner->Get_Child(m_Default)->asInt() : CSG_Parameter_Data_Object::asInt() );
}

//---------------------------------------------------------
double CSG_Parameter_Grid::asDouble(void) const
{
	return( m_pOwner->Get_Child(m_Default) ? m_pOwner->Get_Child(m_Default)->asDouble() : CSG_Parameter_Data_Object::asDouble() );
}

//---------------------------------------------------------
void CSG_Parameter_Grid::On_Assign(CSG_Parameter_Data *pSource)
{
	m_Type		= ((CSG_Parameter_Grid *)pSource)->m_Type;

	m_Default	= ((CSG_Parameter_Grid *)pSource)->m_Default;

	Set_Value(pSource->asPointer());
}


///////////////////////////////////////////////////////////
//														 //
//						Grids							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Grids::CSG_Parameter_Grids(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Grid(pOwner, Constraint)
{}


///////////////////////////////////////////////////////////
//														 //
//						Table							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Table::CSG_Parameter_Table(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data_Object(pOwner, Constraint)
{}

//---------------------------------------------------------
bool CSG_Parameter_Table::Set_Value(void *Value)
{
	if( m_pDataObject == Value )
	{
		return( true );
	}

	m_pDataObject	= (CSG_Data_Object *)Value;

	for(int i=0; i<m_pOwner->Get_Children_Count(); i++)
	{
		CSG_Parameter	*pChild	= m_pOwner->Get_Child(i);

		if( pChild->Get_Type() == PARAMETER_TYPE_Table_Field )
		{
			pChild->Set_Value(m_pDataObject && pChild->is_Optional() ? ((CSG_Table *)m_pDataObject)->Get_Field_Count() : 0);
		}
		else if( pChild->Get_Type() == PARAMETER_TYPE_Table_Fields )
		{
			pChild->Set_Value(CSG_String(""));
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Shapes							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Shapes::CSG_Parameter_Shapes(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data_Object(pOwner, Constraint)
{
	m_Type	= SHAPE_TYPE_Undefined;
}

//---------------------------------------------------------
bool CSG_Parameter_Shapes::Set_Value(void *Value)
{
	if(	Value != DATAOBJECT_NOTSET && Value != DATAOBJECT_CREATE
	&&	m_Type != SHAPE_TYPE_Undefined && m_Type != ((CSG_Shapes *)Value)->Get_Type() )
	{
		return( false );
	}

	if( m_pDataObject == Value )
	{
		return( true );
	}

	m_pDataObject	= (CSG_Data_Object *)Value;

	for(int i=0; i<m_pOwner->Get_Children_Count(); i++)
	{
		CSG_Parameter	*pChild	= m_pOwner->Get_Child(i);

		if(	pChild->Get_Type() == PARAMETER_TYPE_Table_Field )
		{
			pChild->Set_Value(m_pDataObject && m_pDataObject != DATAOBJECT_CREATE && pChild->is_Optional() ? ((CSG_Table *)m_pDataObject)->Get_Field_Count() : 0);
		}
		else if( pChild->Get_Type() == PARAMETER_TYPE_Table_Fields )
		{
			pChild->Set_Value("");
		}
	}

	return( true );
}

//---------------------------------------------------------
void CSG_Parameter_Shapes::Set_Shape_Type(TSG_Shape_Type Type)
{
	m_Type	= Type;
}

//---------------------------------------------------------
void CSG_Parameter_Shapes::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_Data_Object::On_Assign(pSource);

	m_Type	= ((CSG_Parameter_Shapes *)pSource)->m_Type;
}


///////////////////////////////////////////////////////////
//														 //
//						TIN								 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_TIN::CSG_Parameter_TIN(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data_Object(pOwner, Constraint)
{}

//---------------------------------------------------------
bool CSG_Parameter_TIN::Set_Value(void *Value)
{
	if( m_pDataObject == Value )
	{
		return( true );
	}

	m_pDataObject	= (CSG_Data_Object *)Value;

	for(int i=0; i<m_pOwner->Get_Children_Count(); i++)
	{
		CSG_Parameter	*pChild	= m_pOwner->Get_Child(i);

		if( pChild->Get_Type() == PARAMETER_TYPE_Table_Field )
		{
			pChild->Set_Value(m_pDataObject && pChild->is_Optional() ? ((CSG_Table *)m_pDataObject)->Get_Field_Count() : 0);
		}
		else if( pChild->Get_Type() == PARAMETER_TYPE_Table_Fields )
		{
			pChild->Set_Value("");
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						PointCloud						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_PointCloud::CSG_Parameter_PointCloud(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data_Object(pOwner, Constraint)
{}

//---------------------------------------------------------
void CSG_Parameter_PointCloud::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_Data_Object::On_Assign(pSource);
}

//---------------------------------------------------------
bool CSG_Parameter_PointCloud::Set_Value(void *Value)
{
	if( m_pDataObject == Value )
	{
		return( true );
	}

	m_pDataObject	= (CSG_Data_Object *)Value;

	for(int i=0; i<m_pOwner->Get_Children_Count(); i++)
	{
		CSG_Parameter	*pChild	= m_pOwner->Get_Child(i);

		if( pChild->Get_Type() == PARAMETER_TYPE_Table_Field )
		{
			pChild->Set_Value(m_pDataObject && pChild->is_Optional() ? ((CSG_Table *)m_pDataObject)->Get_Field_Count() : 0);
		}
		else if( pChild->Get_Type() == PARAMETER_TYPE_Table_Fields )
		{
			pChild->Set_Value("");
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						List							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_List::CSG_Parameter_List(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{}

CSG_Parameter_List::~CSG_Parameter_List(void)
{}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_List::asString(void)
{
	if( Get_Item_Count() > 0 )
	{
		m_String.Printf("%d %s (", Get_Item_Count(), Get_Item_Count() == 1 ? _TL("object") : _TL("objects"));

		for(int i=0; i<Get_Item_Count(); i++)
		{
			if( i > 0 )
			{
				m_String	+= ", ";
			}

			m_String	+= Get_Item(i)->Get_Name();
		}

		m_String	+= ")";
	}
	else
	{
		m_String	= _TL("No objects");
	}

	return( m_String );
}

//---------------------------------------------------------
bool CSG_Parameter_List::Add_Item(CSG_Data_Object *pObject)
{
	return( pObject != DATAOBJECT_NOTSET && pObject != DATAOBJECT_CREATE && m_Objects.Add(pObject) );
}

bool CSG_Parameter_List::Del_Item(CSG_Data_Object *pObject, bool bUpdateData)
{
	return( m_Objects.Del(pObject) > 0 );
}

bool CSG_Parameter_List::Del_Item(int Index, bool bUpdateData)
{
	return( m_Objects.Del(Index) );
}

bool CSG_Parameter_List::Del_Items(void)
{
	return( m_Objects.Set_Array(0) );
}

//---------------------------------------------------------
void CSG_Parameter_List::On_Assign(CSG_Parameter_Data *pSource)
{
	Del_Items();

	for(int i=0; i<((CSG_Parameter_List *)pSource)->Get_Item_Count(); i++)
	{
		if( m_pOwner->Get_Manager() != &SG_Get_Data_Manager() || SG_Get_Data_Manager().Exists(((CSG_Parameter_List *)pSource)->Get_Item(i)) )
		{
			Add_Item(((CSG_Parameter_List *)pSource)->Get_Item(i));
		}
	}
}

//---------------------------------------------------------
bool CSG_Parameter_List::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		for(int i=0; i<Get_Item_Count(); i++)
		{
			CSG_String	File	= Get_Item(i)->Get_File_Name(false);

			if( File.BeforeFirst(':').Cmp("PGSQL") || SG_File_Exists(File) )
			{
				Entry.Add_Child("DATA", File);
			}
		}
	}
	else
	{
		Del_Items();

		for(int i=0; i<Entry.Get_Children_Count(); i++)
		{
			CSG_Data_Object	*pObject	= m_pOwner->Get_Manager() ? m_pOwner->Get_Manager()->Find(Entry.Get_Content(i), false) : NULL;

			if( pObject )
			{
				Add_Item(pObject);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Grid_List						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Grid_List::CSG_Parameter_Grid_List(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_List(pOwner, Constraint)
{}

//---------------------------------------------------------
CSG_Grid_System * CSG_Parameter_Grid_List::Get_System(void) const
{
	if( m_pOwner->Get_Parent() && m_pOwner->Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		return( m_pOwner->Get_Parent()->asGrid_System() );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Parameter_Grid_List::Add_Item(CSG_Data_Object *pObject)
{
	if( pObject == DATAOBJECT_NOTSET || pObject == DATAOBJECT_CREATE
	|| (pObject->Get_ObjectType() != SG_DATAOBJECT_TYPE_Grid
	&&  pObject->Get_ObjectType() != SG_DATAOBJECT_TYPE_Grids)  )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Get_System() )	// check grid system compatibility
	{
		CSG_Grid_System	System	= pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grid
			? ((CSG_Grid  *)pObject)->Get_System()
			: ((CSG_Grids *)pObject)->Get_System();

		if( !Get_System()->is_Equal(System) )
		{
			for(int i=0; i<m_pOwner->Get_Parent()->Get_Children_Count(); i++)
			{
				CSG_Parameter	*pChild	= m_pOwner->Get_Parent()->Get_Child(i);

				if( pChild->Get_Type() == PARAMETER_TYPE_Grid
				||  pChild->Get_Type() == PARAMETER_TYPE_Grids )
				{
					if( pChild->asDataObject() != DATAOBJECT_NOTSET
					&&  pChild->asDataObject() != DATAOBJECT_CREATE )
					{
						return( false );
					}
				}

				if( pChild->is_DataObject_List() && pChild->asList()->Get_Item_Count() > 0 )
				{
					if( (pChild->Get_Type() == PARAMETER_TYPE_Grid_List  && pChild->asGridList ()->Get_System())
					||  (pChild->Get_Type() == PARAMETER_TYPE_Grids_List && pChild->asGridsList()->Get_System()) )
					{
						return( false );
					}
				}
			}

			Get_System()->Assign(System);
		}
	}

	return( CSG_Parameter_List::Add_Item(pObject) && Update_Data() );
}

//---------------------------------------------------------
bool CSG_Parameter_Grid_List::Del_Item(CSG_Data_Object *pItem, bool bUpdateData)
{
	return( CSG_Parameter_List::Del_Item(pItem) && (!bUpdateData || Update_Data()) );
}

//---------------------------------------------------------
bool CSG_Parameter_Grid_List::Del_Item(int Index, bool bUpdateData)
{
	return( CSG_Parameter_List::Del_Item(Index) && (!bUpdateData || Update_Data()) );
}

//---------------------------------------------------------
bool CSG_Parameter_Grid_List::Del_Items(void)
{
	return( CSG_Parameter_List::Del_Items() && Update_Data() );
}

//---------------------------------------------------------
bool CSG_Parameter_Grid_List::Update_Data(void)
{
	m_Grids.Set_Array(0);

	for(int i=0; i<Get_Item_Count(); i++)
	{
		switch( Get_Item(i)->Get_ObjectType() )
		{
		case SG_DATAOBJECT_TYPE_Grid:
			m_Grids.Add(Get_Item(i));
			break;

		case SG_DATAOBJECT_TYPE_Grids:
			{
				CSG_Grids	*pGrids	= (CSG_Grids *)Get_Item(i);

				for(int j=0; j<pGrids->Get_Grid_Count(); j++)
				{
					m_Grids.Add(pGrids->Get_Grid_Ptr(j));
				}
			}
			break;

		default:
			break;
		}

	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Grids_List						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Grids_List::CSG_Parameter_Grids_List(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_List(pOwner, Constraint)
{}

//---------------------------------------------------------
CSG_Grid_System * CSG_Parameter_Grids_List::Get_System(void) const
{
	if( m_pOwner->Get_Parent() && m_pOwner->Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		return( m_pOwner->Get_Parent()->asGrid_System() );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Parameter_Grids_List::Add_Item(CSG_Data_Object *pObject)
{
	if( pObject == NULL || pObject->Get_ObjectType() != SG_DATAOBJECT_TYPE_Grids )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Get_System() )	// check grid system compatibility
	{
		CSG_Grid_System	System	= Get_Type() == PARAMETER_TYPE_Grid_List
			? ((CSG_Grid  *)pObject)->Get_System()
			: ((CSG_Grids *)pObject)->Get_System();

		if( !Get_System()->is_Equal(System) )
		{
			for(int i=0; i<m_pOwner->Get_Parent()->Get_Children_Count(); i++)
			{
				CSG_Parameter	*pChild	= m_pOwner->Get_Parent()->Get_Child(i);

				if( pChild->Get_Type() == PARAMETER_TYPE_Grids )
				{
					if( pChild->asDataObject() != DATAOBJECT_NOTSET
					&&  pChild->asDataObject() != DATAOBJECT_CREATE )
					{
						return( false );
					}
				}

				if( pChild->is_DataObject_List() && pChild->asList()->Get_Item_Count() > 0 )
				{
					if( (pChild->Get_Type() == PARAMETER_TYPE_Grids_List && pChild->asGridsList()->Get_System()) )
					{
						return( false );
					}
				}
			}

			Get_System()->Assign(((CSG_Grids *)pObject)->Get_System());
		}
	}

	return( CSG_Parameter_List::Add_Item(pObject) );
}


///////////////////////////////////////////////////////////
//														 //
//						Table_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CSG_Parameter_Table_List::CSG_Parameter_Table_List(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_List(pOwner, Constraint)
{}


///////////////////////////////////////////////////////////
//														 //
//						Shapes_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CSG_Parameter_Shapes_List::CSG_Parameter_Shapes_List(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_List(pOwner, Constraint)
{
	m_Type	= SHAPE_TYPE_Undefined;
}

//---------------------------------------------------------
void CSG_Parameter_Shapes_List::Set_Shape_Type(TSG_Shape_Type Type)
{
	m_Type	= Type;
}

//---------------------------------------------------------
void CSG_Parameter_Shapes_List::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_List::On_Assign(pSource);

	m_Type	= ((CSG_Parameter_Shapes_List *)pSource)->m_Type;
}


///////////////////////////////////////////////////////////
//														 //
//						TIN_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CSG_Parameter_TIN_List::CSG_Parameter_TIN_List(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_List(pOwner, Constraint)
{}


///////////////////////////////////////////////////////////
//														 //
//					PointCloud_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CSG_Parameter_PointCloud_List::CSG_Parameter_PointCloud_List(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_List(pOwner, Constraint)
{}


///////////////////////////////////////////////////////////
//														 //
//						Parameters						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Parameters::CSG_Parameter_Parameters(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_pParameters	= new CSG_Parameters(pOwner->Get_Owner()->Get_Owner(), pOwner->Get_Name(), pOwner->Get_Description(), pOwner->Get_Identifier());
}

CSG_Parameter_Parameters::~CSG_Parameter_Parameters(void)
{
	delete(m_pParameters);
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Parameters::asString(void)
{
	m_String.Printf(SG_T("%d %s"), m_pParameters->Get_Count(), _TL("parameters"));

	return( m_String );
}

//---------------------------------------------------------
bool CSG_Parameter_Parameters::Restore_Default(void)
{
	return( m_pParameters->Restore_Defaults() );
}

//---------------------------------------------------------
void CSG_Parameter_Parameters::On_Assign(CSG_Parameter_Data *pSource)
{
	m_pParameters->Assign(((CSG_Parameter_Parameters *)pSource)->m_pParameters);
}

//---------------------------------------------------------
bool CSG_Parameter_Parameters::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( m_pParameters->Serialize(Entry, bSave) )
	{
		if( bSave )
		{
			Entry.Set_Property(SG_T("id")  , m_pOwner->Get_Identifier());
			Entry.Set_Property(SG_T("type"), m_pOwner->Get_Type_Identifier());
		}

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
