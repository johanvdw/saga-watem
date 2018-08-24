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
//                    tool_chain.cpp                     //
//                                                       //
//          Copyright (C) 2014 by Olaf Conrad            //
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
//                University of Hamburg                  //
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
#include "saga_api.h"

#include "tool_chain.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_XML_CONTENT(XML, ID, DEFAULT, TRANSLATE)	(!XML(ID) ? CSG_String(DEFAULT) : !TRANSLATE ? XML[ID].Get_Content() : CSG_String(SG_Translate(XML[ID].Get_Content())))

#define IS_TRUE_STRING(String)				(!String.CmpNoCase("true") || !String.CmpNoCase("1"))
#define IS_TRUE_PROPERTY(Item, Property)	(Item.Cmp_Property(Property, "true", true) || Item.Cmp_Property(Property, "1"))


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Chain::CSG_Tool_Chain(void)
{}

//---------------------------------------------------------
CSG_Tool_Chain::CSG_Tool_Chain(const CSG_String &File)
{
	Create(File);
}

//---------------------------------------------------------
CSG_Tool_Chain::~CSG_Tool_Chain(void)
{
	Reset();
}

//---------------------------------------------------------
void CSG_Tool_Chain::Reset(void)
{
	Parameters.Create(this, SG_T(""), SG_T(""));

	m_Chain.Destroy();

	m_Conditions.Destroy();

	m_Menu.Clear();
}

//---------------------------------------------------------
void CSG_Tool_Chain::Set_Library_Menu(const CSG_String &Menu)
{
	m_Library_Menu	= Menu;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Create(const CSG_String &File)
{
	bool	bReload	= is_Okay();

	Reset();

	//-----------------------------------------------------
	if( File.Right(sizeof(".pyt.xml")).Make_Lower().Find(".pyt.xml") >= 0 )
	{
		return( false );
	}

	if( !m_Chain.Load(File) )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("%s: %s", _TL("failed to load or parse xml file"), File.c_str()));

		Reset();	return( false );
	}

	if( m_Chain.Cmp_Name("toolchains") )	// don't report any error, this xml-file provides info for a category of tool chains
	{
		Reset();	return( false );
	}

	if( !m_Chain.Cmp_Name("toolchain") || !m_Chain("identifier") || !m_Chain("parameters") )
	{
		Reset();	return( false );
	}

	if( SG_Compare_Version(m_Chain.Get_Property("saga-version"), "2.1.3") < 0 )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("%s %s: %s", _TL("WARNING"), _TL("unsupported tool chain version"), File.c_str()));
	}

	SG_UI_Msg_Add(CSG_String::Format("%s: %s...", bReload ? _TL("Reload tool chain") : _TL("Load tool chain"), File.c_str()), true);

	//-----------------------------------------------------
	m_File_Name		= File;

	m_ID			= GET_XML_CONTENT(m_Chain, "identifier" ,     ""               , false) ;
	m_Library		= GET_XML_CONTENT(m_Chain, "group"      ,     "toolchains"     , false) ;
	m_Menu			= GET_XML_CONTENT(m_Chain, "menu"       ,     ""               ,  true) ;
	Set_Name         (GET_XML_CONTENT(m_Chain, "name"       , _TL("Not Named"     ),  true));
	Set_Author       (GET_XML_CONTENT(m_Chain, "author"     , _TL("unknown"       ), false));
	Set_Description  (GET_XML_CONTENT(m_Chain, "description", _TL("no description"),  true));

	Add_References();

	CSG_String	Description	= Get_Description();
	Description.Replace("[[", "<");	// support for xml/html tags
	Description.Replace("]]", ">");
	Set_Description(Description);

	if( !m_Menu.is_Empty() && (m_Menu.Length() < 2 || m_Menu[1] != ':') )
	{
		if( IS_TRUE_PROPERTY(m_Chain["menu"], "absolute") )
			m_Menu.Prepend("A:");	// absolute path
		else
			m_Menu.Prepend("R:");	// relative path
	}

	//-----------------------------------------------------
	for(int i=0; i<m_Chain["parameters"].Get_Children_Count(); i++)
	{
		const CSG_MetaData	&Parameter	= m_Chain["parameters"][i];

		CSG_String	ID	= Parameter.Get_Property("varname");

		if( ID.is_Empty() || Parameters(ID) )
		{
			continue;
		}

		if( Parameter.Get_Property("with_gui") && !IS_TRUE_PROPERTY(Parameter, "with_gui") && SG_UI_Get_Window_Main() == NULL )
		{
			continue;
		}

		if( Parameter.Get_Property("with_cmd") && !IS_TRUE_PROPERTY(Parameter, "with_cmd") && SG_UI_Get_Window_Main() != NULL )
		{
			continue;
		}

		//-------------------------------------------------
		int			Constraint	= 0;
		CSG_String	Value;
		bool		bMin = false, bMax = false;
		double		 Min = 0.0  ,  Max = 0.0;

		if( Parameter.Cmp_Name("input") )
		{
			Constraint	= IS_TRUE_PROPERTY(Parameter, "optional") ? PARAMETER_INPUT_OPTIONAL : PARAMETER_INPUT;
		}
		else if( Parameter.Cmp_Name("output") )
		{
			Constraint	= IS_TRUE_PROPERTY(Parameter, "optional") ? PARAMETER_OUTPUT_OPTIONAL : PARAMETER_OUTPUT;
		}
		else if( Parameter.Cmp_Name("option") && Parameter("value") )
		{
			Value	= Parameter.Get_Content("value");

			bMin	= Parameter["value"].Get_Property("min", Min);
			bMax	= Parameter["value"].Get_Property("max", Max);
		}

		//-------------------------------------------------
		if( Parameter("condition") )
		{
			CSG_MetaData	&Conditions	= *m_Conditions.Add_Child(ID);

			for(int j=0; j<Parameter.Get_Children_Count(); j++)	// there might be more than one condition to be checked
			{
				if( Parameter[j].Cmp_Name("condition") )
				{
					Conditions.Add_Child(Parameter[j]);
				}
			}
		}

		//-------------------------------------------------
		CSG_String	Name	= SG_Translate(Parameter.Get_Content("name"       ));
		CSG_String	Desc	= SG_Translate(Parameter.Get_Content("description"));

		CSG_Parameter	*pParent	= Parameters(Parameter.Get_Property("parent"));
		CSG_String		ParentID(pParent ? pParent->Get_Identifier() : SG_T(""));

		//-------------------------------------------------
		switch( SG_Parameter_Type_Get_Type(Parameter.Get_Property("type")) )
		{
		case PARAMETER_TYPE_Node             : Parameters.Add_Node           (ParentID, ID, Name, Desc);	break;

		case PARAMETER_TYPE_Bool             : Parameters.Add_Value          (ParentID, ID, Name, Desc, PARAMETER_TYPE_Bool  , IS_TRUE_STRING(Value));	break;
		case PARAMETER_TYPE_Int              : Parameters.Add_Value          (ParentID, ID, Name, Desc, PARAMETER_TYPE_Int   , Value.asInt   (), Min, bMin, Max, bMax);	break;
		case PARAMETER_TYPE_Double           : Parameters.Add_Value          (ParentID, ID, Name, Desc, PARAMETER_TYPE_Double, Value.asDouble(), Min, bMin, Max, bMax);	break;
		case PARAMETER_TYPE_Degree           : Parameters.Add_Value          (ParentID, ID, Name, Desc, PARAMETER_TYPE_Degree, Value.asDouble(), Min, bMin, Max, bMax);	break;

		case PARAMETER_TYPE_Date             : Parameters.Add_Date           (ParentID, ID, Name, Desc, 0.0)->Set_Value(Value);	break;

		case PARAMETER_TYPE_Range            : Parameters.Add_Range          (ParentID, ID, Name, Desc, Value.BeforeFirst(';').asDouble(), Value.AfterFirst(';').asDouble(), Min, bMin, Max, bMax);	break;
		case PARAMETER_TYPE_Choice           : Parameters.Add_Choice         (ParentID, ID, Name, Desc, Parameter.Get_Content("choices"))->Set_Value(Value);	break;
		case PARAMETER_TYPE_Choices          : Parameters.Add_Choices        (ParentID, ID, Name, Desc, Parameter.Get_Content("choices"))->Set_Value(Value);	break;

		case PARAMETER_TYPE_String           : Parameters.Add_String         (ParentID, ID, Name, Desc, Value, false);	break;
		case PARAMETER_TYPE_Text             : Parameters.Add_String         (ParentID, ID, Name, Desc, Value,  true);	break;

		case PARAMETER_TYPE_FilePath         : Parameters.Add_FilePath       (ParentID, ID, Name, Desc, Parameter.Get_Content("filter"), Value,
												IS_TRUE_PROPERTY(Parameter, "save"     ),
												IS_TRUE_PROPERTY(Parameter, "directory"),
												IS_TRUE_PROPERTY(Parameter, "multiple" ));	break;

		case PARAMETER_TYPE_Font             : break;
		case PARAMETER_TYPE_Color            : Parameters.Add_Value          (ParentID, ID, Name, Desc, PARAMETER_TYPE_Color, Value.asInt());	break;
		case PARAMETER_TYPE_Colors           : Parameters.Add_Colors         (ParentID, ID, Name, Desc);	break;
		case PARAMETER_TYPE_FixedTable       : break;	// to do ?

		case PARAMETER_TYPE_Grid_System      : Parameters.Add_Grid_System    (ParentID, ID, Name, Desc);	break;

		case PARAMETER_TYPE_Table_Field      : Parameters.Add_Table_Field    (ParentID, ID, Name, Desc, (!Value.CmpNoCase("true") || !Value.CmpNoCase("1")));	break;
		case PARAMETER_TYPE_Table_Fields     : Parameters.Add_Table_Fields   (ParentID, ID, Name, Desc);	break;

		case PARAMETER_TYPE_Grid             : Parameter.Cmp_Property("target", "none") 
			                                 ? Parameters.Add_Grid_Output    (      "", ID, Name, Desc)
			                                 : Parameters.Add_Grid           (ParentID, ID, Name, Desc, Constraint);	break;

		case PARAMETER_TYPE_Grids            : Parameter.Cmp_Property("target", "none") 
			                                 ? Parameters.Add_Grids_Output   (      "", ID, Name, Desc)
			                                 : Parameters.Add_Grids          (ParentID, ID, Name, Desc, Constraint);	break;

		case PARAMETER_TYPE_Table            : Parameters.Add_Table          (ParentID, ID, Name, Desc, Constraint);	break;

		case PARAMETER_TYPE_Shapes           : Parameters.Add_Shapes         (ParentID, ID, Name, Desc, Constraint,
												   Parameter.Cmp_Property("feature_type", "point"  ) ? SHAPE_TYPE_Point   :
												   Parameter.Cmp_Property("feature_type", "points" ) ? SHAPE_TYPE_Points  :
												   Parameter.Cmp_Property("feature_type", "line"   ) ? SHAPE_TYPE_Line    :
												   Parameter.Cmp_Property("feature_type", "polygon") ? SHAPE_TYPE_Polygon : SHAPE_TYPE_Undefined);	break;

		case PARAMETER_TYPE_TIN              : Parameters.Add_TIN            (ParentID, ID, Name, Desc, Constraint);	break;
		case PARAMETER_TYPE_PointCloud       : Parameters.Add_PointCloud     (ParentID, ID, Name, Desc, Constraint);	break;

		case PARAMETER_TYPE_Grid_List        : Parameters.Add_Grid_List      (ParentID, ID, Name, Desc, Constraint, !IS_TRUE_PROPERTY(Parameter, "no_system"));	break;
		case PARAMETER_TYPE_Grids_List       : Parameters.Add_Grids_List     (ParentID, ID, Name, Desc, Constraint, !IS_TRUE_PROPERTY(Parameter, "no_system"));	break;
		case PARAMETER_TYPE_Table_List       : Parameters.Add_Table_List     (ParentID, ID, Name, Desc, Constraint);	break;
		case PARAMETER_TYPE_Shapes_List      : Parameters.Add_Shapes_List    (ParentID, ID, Name, Desc, Constraint);	break;
		case PARAMETER_TYPE_TIN_List         : Parameters.Add_TIN_List       (ParentID, ID, Name, Desc, Constraint);	break;
		case PARAMETER_TYPE_PointCloud_List  : Parameters.Add_PointCloud_List(ParentID, ID, Name, Desc, Constraint);	break;

		case PARAMETER_TYPE_DataObject_Output: break;
		case PARAMETER_TYPE_Parameters       : break;	// to do ?

		default: break;
		}
	}

	//-----------------------------------------------------
	if( is_Okay() )
	{
		SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);

		return( true );
	}

	SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Tool_Chain::Add_References(void)
{
	for(int i=0; i<m_Chain.Get_Children_Count(); i++)
	{
		if( !m_Chain[i].Get_Name().CmpNoCase("REFERENCE") )
		{
			CSG_String	Authors, Year, Title, Where, Link, Link_Text;

			if( m_Chain[i]("AUTHORS"  ) ) Authors   = m_Chain[i].Get_Content("AUTHORS"  );
			if( m_Chain[i]("YEAR"     ) ) Year      = m_Chain[i].Get_Content("YEAR"     );
			if( m_Chain[i]("TITLE"    ) ) Title     = m_Chain[i].Get_Content("TITLE"    );
			if( m_Chain[i]("WHERE"    ) ) Where     = m_Chain[i].Get_Content("WHERE"    );
			if( m_Chain[i]("LINK"     ) ) Link      = m_Chain[i].Get_Content("LINK"     );
			if( m_Chain[i]("LINK_TEXT") ) Link_Text = m_Chain[i].Get_Content("LINK_TEXT");

			if( !Authors.is_Empty() && !Year.is_Empty() && !Title.is_Empty() )
			{
				Add_Reference(Authors, Year, Title, Where, Link.c_str(), Link_Text.c_str());
			}
			else if( !Link.is_Empty() )
			{
				Add_Reference(Link, Link_Text.c_str());
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Tool_Chain::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	for(int iParameter=0; iParameter<m_Conditions.Get_Children_Count(); iParameter++)
	{
		const CSG_MetaData	&Conditions	= m_Conditions[iParameter];

		if( pParameters->Get_Parameter(Conditions.Get_Name()) )
		{
			bool	bEnable	= true;

			for(int iCondition=0; bEnable && iCondition<Conditions.Get_Children_Count(); iCondition++)
			{
				bEnable	= Check_Condition(Conditions[iCondition], pParameters);
			}

			pParameters->Get_Parameter(Conditions.Get_Name())->Set_Enabled(bEnable);
		}
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::On_Execute(void)
{
	bool	bResult	= Data_Initialize();

	if( !bResult )
	{
		Error_Set(_TL("no data objects"));
	}

	for(int i=0; bResult && i<m_Chain["tools"].Get_Children_Count(); i++)
	{
		bResult	= Tool_Run(m_Chain["tools"][i]);
	}

	Data_Finalize();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Data_Add(const CSG_String &ID, CSG_Parameter *pData)
{
	if( !pData )
	{
		return( false );
	}

	CSG_Parameter	*pParameter	= m_Data(ID);

	if( pParameter )	// don't add twice with same identifier
	{
		if(0&& pParameter->Get_Type() != pData->Get_Type() )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	else switch( pData->Get_Type() )
	{
	case PARAMETER_TYPE_PointCloud     : pParameter	= m_Data.Add_PointCloud     ("", ID, "", "", 0       );	break;
	case PARAMETER_TYPE_Grid           : pParameter	= m_Data.Add_Grid           ("", ID, "", "", 0       );	break;
	case PARAMETER_TYPE_Grids          : pParameter	= m_Data.Add_Grids          ("", ID, "", "", 0       );	break;
	case PARAMETER_TYPE_Table          : pParameter	= m_Data.Add_Table          ("", ID, "", "", 0       );	break;
	case PARAMETER_TYPE_Shapes         : pParameter	= m_Data.Add_Shapes         ("", ID, "", "", 0       );	break;
	case PARAMETER_TYPE_TIN            : pParameter	= m_Data.Add_TIN            ("", ID, "", "", 0       );	break;

	case PARAMETER_TYPE_PointCloud_List: pParameter	= m_Data.Add_PointCloud_List("", ID, "", "", 0       );	break;
	case PARAMETER_TYPE_Grid_List      : pParameter	= m_Data.Add_Grid_List      ("", ID, "", "", 0, false);	break;
	case PARAMETER_TYPE_Grids_List     : pParameter	= m_Data.Add_Grids_List     ("", ID, "", "", 0, false);	break;
	case PARAMETER_TYPE_Table_List     : pParameter	= m_Data.Add_Table_List     ("", ID, "", "", 0       );	break;
	case PARAMETER_TYPE_Shapes_List    : pParameter	= m_Data.Add_Shapes_List    ("", ID, "", "", 0       );	break;
	case PARAMETER_TYPE_TIN_List       : pParameter	= m_Data.Add_TIN_List       ("", ID, "", "", 0       );	break;

	case PARAMETER_TYPE_DataObject_Output:
		switch( pData->Get_DataObject_Type() )
		{
		case SG_DATAOBJECT_TYPE_PointCloud: pParameter	= m_Data.Add_PointCloud     ("", ID, "", "", 0       );	break;
		case SG_DATAOBJECT_TYPE_Grid      : pParameter	= m_Data.Add_Grid           ("", ID, "", "", 0       );	break;
		case SG_DATAOBJECT_TYPE_Grids     : pParameter	= m_Data.Add_Grids          ("", ID, "", "", 0       );	break;
		case SG_DATAOBJECT_TYPE_Table     : pParameter	= m_Data.Add_Table          ("", ID, "", "", 0       );	break;
		case SG_DATAOBJECT_TYPE_Shapes    : pParameter	= m_Data.Add_Shapes         ("", ID, "", "", 0       );	break;
		case SG_DATAOBJECT_TYPE_TIN       : pParameter	= m_Data.Add_TIN            ("", ID, "", "", 0       );	break;
		default:
			return( true );
		}
		break;

	default:
		return( true );
	}

	//-----------------------------------------------------
	if( pData->is_DataObject() )
	{
		if( pParameter->is_DataObject() )
		{
			pParameter->Set_Value(pData->asDataObject());
		}
		else
		{
			pParameter->asList()->Add_Item(pData->asDataObject());
		}

		m_Data_Manager.Add(pData->asDataObject());
	}
	else if( pData->is_DataObject_List() && pParameter->is_DataObject_List() )
	{
		for(int i=0; i<pData->asList()->Get_Data_Count(); i++)
		{
			pParameter->asList()->Add_Item(pData->asList()->Get_Data(i));
			m_Data_Manager.Add            (pData->asList()->Get_Data(i));
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::Data_Add_TempList(const CSG_String &ID, const CSG_String &Type)
{
	if( !m_Data(ID) )
	{
		switch( SG_Parameter_Type_Get_Type(Type) )
		{
		case PARAMETER_TYPE_PointCloud_List: m_Data.Add_PointCloud_List("", ID, "", "", 0       );	break;
		case PARAMETER_TYPE_Grid_List      : m_Data.Add_Grid_List      ("", ID, "", "", 0, false);	break;
		case PARAMETER_TYPE_Grids_List     : m_Data.Add_Grids_List     ("", ID, "", "", 0, false);	break;
		case PARAMETER_TYPE_Table_List     : m_Data.Add_Table_List     ("", ID, "", "", 0       );	break;
		case PARAMETER_TYPE_Shapes_List    : m_Data.Add_Shapes_List    ("", ID, "", "", 0       );	break;
		case PARAMETER_TYPE_TIN_List       : m_Data.Add_TIN_List       ("", ID, "", "", 0       );	break;

		default:
			Error_Fmt("%s: %s [%s|%s]", SG_T("datalist"), _TL("unsupported data list type"), ID.c_str(), Type.c_str());

			return( false );
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Data_Exists(CSG_Data_Object *pData)
{
	for(int i=0; i<m_Data.Get_Count(); i++)
	{
		if( m_Data(i)->is_DataObject() )
		{
			if( pData == m_Data(i)->asDataObject() )
			{
				return( true );
			}
		}
		else if( m_Data(i)->is_DataObject_List() )
		{
			for(int j=0; j<m_Data(i)->asList()->Get_Data_Count(); j++)
			{
				if( pData == m_Data(i)->asList()->Get_Data(j) )
				{
					return( true );
				}
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::Data_Initialize(void)
{
	m_Data.Set_Manager(NULL);

	for(int i=0; i<Parameters.Get_Count(); i++)
	{
		CSG_Parameter	*pParameter	= Parameters(i);

		if( !(pParameter->is_DataObject() && !pParameter->asDataObject()) )
		{
			if( !Data_Add(pParameter->Get_Identifier(), pParameter) )
			{
				return( false );
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::Data_Finalize(void)
{
	int		i;

	//-----------------------------------------------------
	for(i=0; i<Parameters.Get_Count(); i++)	// detach non temporary data before freeing the local data manager !!!
	{
		if( Parameters(i)->is_DataObject() )
		{
			if( Parameters(i)->Get_Type() == PARAMETER_TYPE_DataObject_Output && m_Data(Parameters(i)->Get_Identifier()) )
			{
				Parameters(i)->Set_Value(m_Data(Parameters(i)->Get_Identifier())->asDataObject());
			}

			m_Data_Manager.Delete(Parameters(i)->asDataObject(), true);
		}
		else if( Parameters(i)->is_DataObject_List() )
		{
			if( Parameters(i)->is_Output() && m_Data(Parameters(i)->Get_Identifier()) )	// output lists cannot be up-to-date yet
			{
				CSG_Parameter	*pData	= m_Data(Parameters(i)->Get_Identifier());

				for(int j=0; j<pData->asList()->Get_Data_Count(); j++)	// csg_parameter::assign() will not work, if parameters data manager is the standard data manager because it checks for existing data sets
				{
					Parameters(i)->asList()->Add_Item(pData->asList()->Get_Data(j));
				}
			}

			for(int j=0; j<Parameters(i)->asList()->Get_Data_Count(); j++)
			{
				m_Data_Manager.Delete(Parameters(i)->asList()->Get_Data(j), true);
			}
		}
	}

	m_Data_Manager.Delete_All();

	m_Data.Destroy();

	//-----------------------------------------------------
	for(i=0; i<m_Chain["parameters"].Get_Children_Count(); i++)
	{
		const CSG_MetaData	&Parameter	= m_Chain["parameters"][i];

		if( Parameter.Cmp_Name("output") )
		{
			CSG_Parameter	*pParameter	= Parameters(Parameter.Get_Property("varname"));

			if( pParameter && pParameter->is_DataObject() && pParameter->asDataObject() )
			{
				if( Parameter("output_name") )
				{
					if( IS_TRUE_PROPERTY(Parameter["output_name"], "input") )
					{
						CSG_Parameter	*pInput	= Parameters(Parameter["output_name"].Get_Content());

						if( pInput && pInput->is_DataObject() && pInput->asDataObject() )
						{
							CSG_String	Suffix;

							if( Parameter["output_name"].Get_Property("suffix", Suffix) && !Suffix.is_Empty() )
							{
								Suffix	= " [" + Suffix + "]";
							}

							pParameter->asDataObject()->Set_Name(pInput->asDataObject()->Get_Name() + Suffix);
						}
					}
					else if( !Parameter["output_name"].Get_Content().is_Empty() )
					{
						pParameter->asDataObject()->Set_Name(Parameter["output_name"].Get_Content());
					}
				}

				if( Parameter("colours") )
				{
					DataObject_Set_Colors(pParameter->asDataObject(), 11,
						Parameter["colours"].Get_Content().asInt(), IS_TRUE_PROPERTY(Parameter["colours"], "revert")
					);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Parameter_isCompatible(TSG_Parameter_Type A, TSG_Parameter_Type B)
{
	if( A == B )
	{
		return( true );
	}

	switch( A )
	{
	default:
		return( false );

	case PARAMETER_TYPE_Table:
		return( B == PARAMETER_TYPE_Shapes
			||  B == PARAMETER_TYPE_PointCloud
			||  B == PARAMETER_TYPE_TIN );

	case PARAMETER_TYPE_Shapes:
		return( B == PARAMETER_TYPE_PointCloud );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Check_Condition(const CSG_MetaData &Condition, CSG_Parameters *pData)
{
	//-----------------------------------------------------
	CSG_String	Type;

	if( !Condition.Cmp_Name("condition") || !Condition.Get_Property("type", Type) )
	{
		return( true );
	}

	//-----------------------------------------------------
	CSG_String	Variable;

	if( !Condition.Get_Property("varname", Variable) && !Condition.Get_Property("variable", Variable) )
	{
		Variable	= Condition.Get_Content();
	}

	//-----------------------------------------------------
	if( !Type.CmpNoCase("has_gui"   ) )	// executed from saga_gui ? (tool might offer different parameters if called from saga_cmd, python etc.)
	{
		return( IS_TRUE_STRING(Variable) ? SG_UI_Get_Window_Main() != NULL : SG_UI_Get_Window_Main() == NULL );
	}

	//-----------------------------------------------------
	if( !Type.CmpNoCase("exists"    ) )	// data object exists
	{
		CSG_Parameter	*pParameter	= pData->Get_Parameter(Variable);

		return( pParameter && ((pParameter->is_DataObject() && pParameter->asDataObject()) || (pParameter->is_DataObject_List() && pParameter->asList()->Get_Data_Count())) );
	}

	if( !Type.CmpNoCase("not_exists") )	// data object does not exist
	{
		return( pData->Get_Parameter(Variable) == NULL || pData->Get_Parameter(Variable)->asDataObject() == NULL );
	}

	//-----------------------------------------------------
	CSG_Parameter	*pOption	= pData->Get_Parameter(Variable);

	if( pOption == NULL )
	{
		return( true );
	}

	switch( pOption->Get_Type() )
	{
	//-----------------------------------------------------
	case PARAMETER_TYPE_Bool    :
		{
			CSG_String	Value;

			if( Condition.Get_Property("value", Value) )
			{
				if(      !Type.CmpNoCase("=") || !Type.CmpNoCase("equal"    ) )	{	return( (IS_TRUE_STRING(Value) ?  pOption->asBool() : !pOption->asBool()) );	}
				else if( !Type.CmpNoCase("!") || !Type.CmpNoCase("not_equal") )	{	return( (IS_TRUE_STRING(Value) ? !pOption->asBool() :  pOption->asBool()) );	}
			}
		}
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_Int     :
	case PARAMETER_TYPE_Color   :
	case PARAMETER_TYPE_Choice  :
		{
			int		Value;

			if( Condition.Get_Property("value", Value) )
			{
				if(      !Type.CmpNoCase("=") || !Type.CmpNoCase("equal"    ) )	{	if( Value != pOption->asInt() )	{	return( false );	}	}
				else if( !Type.CmpNoCase("!") || !Type.CmpNoCase("not_equal") )	{	if( Value == pOption->asInt() )	{	return( false );	}	}
				else if( !Type.CmpNoCase("<") || !Type.CmpNoCase("less"     ) )	{	if( Value >= pOption->asInt() )	{	return( false );	}	}
				else if( !Type.CmpNoCase(">") || !Type.CmpNoCase("greater"  ) )	{	if( Value <= pOption->asInt() )	{	return( false );	}	}
			}
		}
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_Double  :
	case PARAMETER_TYPE_Degree  :
		{
			double	Value;

			if( Condition.Get_Property("value", Value) )
			{
				if(      !Type.CmpNoCase("=") || !Type.CmpNoCase("equal"    ) )	{	if( Value != pOption->asDouble() )	{	return( false );	}	}
				else if( !Type.CmpNoCase("!") || !Type.CmpNoCase("not_equal") )	{	if( Value == pOption->asDouble() )	{	return( false );	}	}
				else if( !Type.CmpNoCase("<") || !Type.CmpNoCase("less"     ) )	{	if( Value >= pOption->asDouble() )	{	return( false );	}	}
				else if( !Type.CmpNoCase(">") || !Type.CmpNoCase("greater"  ) )	{	if( Value <= pOption->asDouble() )	{	return( false );	}	}
			}
		}
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_String  :
	case PARAMETER_TYPE_Text    :
	case PARAMETER_TYPE_FilePath:
		{
			CSG_String	Value;

			Condition.Get_Property("value", Value);	// no 'if', bcos empty string would return false !!

			{
				if(      !Type.CmpNoCase("=") || !Type.CmpNoCase("equal"    ) )	{	if( Value.Cmp(pOption->asString()) != 0 )	{	return( false );	}	}
				else if( !Type.CmpNoCase("!") || !Type.CmpNoCase("not_equal") )	{	if( Value.Cmp(pOption->asString()) == 0 )	{	return( false );	}	}
				else if( !Type.CmpNoCase("<") || !Type.CmpNoCase("less"     ) )	{	if( Value.Cmp(pOption->asString()) >= 0 )	{	return( false );	}	}
				else if( !Type.CmpNoCase(">") || !Type.CmpNoCase("greater"  ) )	{	if( Value.Cmp(pOption->asString()) <= 0 )	{	return( false );	}	}
			}
		}
		break;

	//-----------------------------------------------------
	default:
		// nop
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::ForEach(const CSG_MetaData &Commands)
{
	CSG_String	ListVarName;
	
	if( !Commands.Get_Property("input", ListVarName) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<Commands.Get_Children_Count(); i++)	// add internal target lists, if any..
	{
		if( Commands[i].Cmp_Name("output") || Commands[i].Cmp_Name("datalist") )
		{
			Data_Add_TempList(Commands[i].Get_Content(), Commands[i].Get_Property("type"));
		}
	}

	//-----------------------------------------------------
	return( ForEach_Object(Commands, ListVarName)
		||  ForEach_File  (Commands, ListVarName) );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::ForEach_Object(const CSG_MetaData &Commands, const CSG_String &ListVarName)
{
	bool	bResult	= false;

	CSG_Parameter	*pList	= m_Data(ListVarName);

	if( pList )
	{
		bResult	= true;

		//-------------------------------------------------
		if( pList->is_DataObject_List() )
		{
			for(int iObject=0; bResult && iObject<pList->asList()->Get_Data_Count(); iObject++)
			{
				for(int iTool=0; bResult && iTool<Commands.Get_Children_Count(); iTool++)
				{
					const CSG_MetaData	&Tool	= Commands[iTool];

					if( Tool.Cmp_Name("tool") )
					{
						for(int j=0; j<Tool.Get_Children_Count(); j++)
						{
							if( Tool[j].Cmp_Name("input") && Tool[j].Get_Content().Find(ListVarName) == 0 )
							{
								Tool(j)->Set_Content(ListVarName + CSG_String::Format("[%d]", iObject));
							}
						}

						bResult	= Tool_Run(Tool);
					}
				}
			}
		}

		//-------------------------------------------------
		else if( pList->Get_Type() == PARAMETER_TYPE_Grids )
		{
			for(int iObject=0; bResult && iObject<pList->asGrids()->Get_Grid_Count(); iObject++)
			{
				for(int iTool=0; bResult && iTool<Commands.Get_Children_Count(); iTool++)
				{
					const CSG_MetaData	&Tool	= Commands[iTool];

					if( Tool.Cmp_Name("tool") )
					{
						for(int j=0; j<Tool.Get_Children_Count(); j++)
						{
							if( Tool[j].Cmp_Name("input") && Tool[j].Get_Content().Find(ListVarName) == 0 )
							{
								Tool(j)->Set_Content(ListVarName + CSG_String::Format("[%d]", iObject));
							}
						}

						bResult	= Tool_Run(Tool);
					}
				}
			}
		}
	}

	return( bResult );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::ForEach_File(const CSG_MetaData &Commands, const CSG_String &ListVarName)
{
	CSG_Parameter	*pList	= Parameters(ListVarName);

	if( !pList || pList->Get_Type() != PARAMETER_TYPE_FilePath )
	{
		return( false );
	}

	CSG_Strings	Files;

	pList->asFilePath()->Get_FilePaths(Files);

	//-----------------------------------------------------
	bool	bResult	= true;

	for(int iFile=0; bResult && iFile<Files.Get_Count(); iFile++)
	{
		for(int iTool=0; bResult && iTool<Commands.Get_Children_Count(); iTool++)
		{
			const CSG_MetaData	&Tool	= Commands[iTool];

			if( Tool.Cmp_Name("tool") )
			{
				CSG_Array_Int	Input;

				for(int j=0; j<Tool.Get_Children_Count(); j++)
				{
					if( Tool[j].Cmp_Name("option") && Tool[j].Get_Content().Find(ListVarName) == 0 && IS_TRUE_PROPERTY(Tool[j], "varname") )
					{
						Tool(j)->Set_Content(Files[iFile]);
						Tool(j)->Set_Property("varname", "false");

						Input	+= j;
					}
				}

				bResult	= Tool_Run(Tool);

				for(size_t i=0; i<Input.Get_Size(); i++)
				{
					Tool(Input[i])->Set_Content(ListVarName);
					Tool(Input[i])->Set_Property("varname", "true");
				}
			}
		}
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Tool_Run(const CSG_MetaData &Tool)
{
	//-----------------------------------------------------
	if( Tool.Cmp_Name("comment") )
	{
		return( true );
	}

	//-----------------------------------------------------
	if( Tool.Cmp_Name("datalist") )
	{
		return( Data_Add_TempList(Tool.Get_Content(), Tool.Get_Property("type")) );
	}

	//-----------------------------------------------------
	if( Tool.Cmp_Name("condition") )
	{
		if( !Check_Condition(Tool, &m_Data) || !Check_Condition(Tool, &Parameters) )
		{
			return( true );
		}

		bool	bResult	= true;

		for(int i=0; bResult && i<Tool.Get_Children_Count(); i++)
		{
			bResult	= Tool_Run(Tool[i]);
		}

		return( bResult );
	}

	//-----------------------------------------------------
	if( Tool.Cmp_Name("foreach") )
	{
		return( ForEach(Tool) );
	}

	//-----------------------------------------------------
	if( !Tool.Cmp_Name("tool") || !Tool.Get_Property("library") || !(Tool.Get_Property("tool") || Tool.Get_Property("module")) )
	{
		Error_Set(_TL("invalid tool definition"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_String	Name(Tool.Get_Property("tool") ? Tool.Get_Property("tool") : Tool.Get_Property("module"));

	CSG_Tool	*pTool;

	if(	!(pTool = SG_Get_Tool_Library_Manager().Get_Tool(Tool.Get_Property("library"), Name)) )
	{
		Error_Fmt("%s [%s].[%s]", _TL("could not find tool"), Tool.Get_Property("library"), Name.c_str());

		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(pTool->Get_Name());
	Message_Add(CSG_String::Format("\n%s: %s", _TL("Run Tool"), pTool->Get_Name().c_str()), false);

	pTool->Settings_Push(&m_Data_Manager);

	bool	bResult	= false;

	if( !pTool->On_Before_Execution() )
	{
		Error_Fmt("%s [%s].[%s]", _TL("before tool execution check failed"), pTool->Get_Library().c_str(), pTool->Get_Name().c_str());
	}
	else if( !Tool_Initialize(Tool, pTool) )
	{
		Error_Fmt("%s [%s].[%s]", _TL("tool initialization failed"        ), pTool->Get_Library().c_str(), pTool->Get_Name().c_str());
	}
	else if( !(bResult = pTool->Execute()) )
	{
		Error_Fmt("%s [%s].[%s]", _TL("tool execution failed"             ), pTool->Get_Library().c_str(), pTool->Get_Name().c_str());
	}

	if( bResult )
	{
		pTool->On_After_Execution();
	}

	Tool_Finalize(Tool, pTool);

	pTool->Settings_Pop();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Tool_Check_Condition(const CSG_MetaData &Tool)
{
	if( Tool("condition") )
	{
		for(int i=0; i<Tool.Get_Children_Count(); i++)	// there might be more than one condition to be checked
		{
			if( !Check_Condition(Tool[i], &m_Data) )
			{
				return( false );
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Tool_Get_Parameter(const CSG_MetaData &Parameter, CSG_Tool *pTool, CSG_Parameter **ppParameter, CSG_Parameter **ppParameters)
{
	CSG_String	ID	= Parameter.Get_Property("id");

	*ppParameters	= NULL;

	CSG_Parameter	*pParameters	= pTool->Get_Parameters()->Get_Parameter(Parameter.Get_Property("parms"));

	if( pParameters && pParameters->Get_Type() == PARAMETER_TYPE_Parameters && pParameters->asParameters()->Get_Parameter(ID) )
	{
		*ppParameters	= pParameters;
		*ppParameter	= pParameters->asParameters()->Get_Parameter(ID);
	}
	else if( pTool->Get_Parameters(Parameter.Get_Property("parms")) )
	{
		*ppParameter	= pTool->Get_Parameters(Parameter.Get_Property("parms"))->Get_Parameter(ID);
	}
	else
	{
		*ppParameter	= pTool->Get_Parameters()->Get_Parameter(ID);
	}

	return( *ppParameter != NULL );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::Tool_Initialize(const CSG_MetaData &Tool, CSG_Tool *pTool)
{
	//-----------------------------------------------------
	int		i;

	//-----------------------------------------------------
	for(i=0; i<Tool.Get_Children_Count(); i++)	// set data variables first
	{
		const CSG_MetaData	&Parameter	= Tool[i];

		CSG_Parameter	*pParameter, *pOwner;

		if( Parameter.Cmp_Name("comment") )
		{
			continue;
		}
		else if( !Tool_Get_Parameter(Parameter, pTool, &pParameter, &pOwner) )
		{
			Error_Set(CSG_String::Format("%s: %s", _TL("parameter not found"), Parameter.Get_Property("id")));

			return( false );
		}
		else if( Parameter.Cmp_Name("option") )
		{
			if( IS_TRUE_PROPERTY(Parameter, "varname") )
			{	// does option want a value from tool chain parameters and do these provide one ?
				pParameter->Set_Value(Parameters(Parameter.Get_Content()));
			}
			else switch( pParameter->Get_Type() )
			{
			default:
				pParameter->Set_Value(Parameter.Get_Content());
				break;

			case PARAMETER_TYPE_FixedTable:
				if( Parameter("OPTION") )
				{
					pParameter->Serialize(*Parameter("OPTION"), false);
				}
				break;
			}
		}
		else if( Parameter.Cmp_Name("input") )
		{
			bool	bResult	= false;

			int	Index;

			if( Parameter.Get_Content().Find('[') < 1 || !Parameter.Get_Content().AfterFirst('[').asInt(Index) )
			{
				Index	= -1;
			}

			CSG_Parameter	*pData	= m_Data(Index < 0 ? Parameter.Get_Content() : Parameter.Get_Content().BeforeFirst('['));

			if( pData && (pParameter->is_DataObject() || pParameter->is_DataObject_List()) )
			{
				if( pParameter->Get_Type() == pData->Get_Type() )
				{
					bResult	= pParameter->Assign(pData);
				}
				else if( Parameter_isCompatible(pParameter->Get_Type(), pData->Get_Type()) )
				{
					bResult = pParameter->Set_Value(pData->asDataObject());
				}
				else if( pParameter->is_DataObject_List() && pData->is_DataObject() )
				{
					bResult	= pParameter->asList()->Add_Item(pData->asDataObject());
				}
				else if( pParameter->is_DataObject() && pData->is_DataObject_List() && Index >= 0 )
				{
					bResult	= pParameter->Set_Value(pData->asList()->Get_Data(Index));
				}
				else if( pParameter->is_DataObject() && pData->asGrids() && Index >= 0 )
				{
					bResult	= pParameter->Set_Value(pData->asGrids()->Get_Grid_Ptr(Index));
				}
			}

			if( !bResult )
			{
				Error_Set(CSG_String::Format("%s: %s", _TL("set input"), Parameter.Get_Property("id")));

				return( false );
			}

			pParameter->has_Changed();

			if( pOwner )
			{
				pOwner->has_Changed();
			}
		}
		else if( Parameter.Cmp_Name("output") )
		{
			if( !pParameter->Assign(m_Data(Parameter.Get_Content())) )
			{
				if( pParameter->is_DataObject() )
				{
					pParameter->Set_Value(DATAOBJECT_CREATE);
				}
				else if( pParameter->is_DataObject_List() )
				{
					pParameter->asList()->Del_Items();
				}
				else if( pParameter->asGrids() )
				{
					pParameter->asGrids()->Del_Grids();
				}
			}
		}
	}

	//-----------------------------------------------------
	for(i=0; i<Tool.Get_Children_Count(); i++)	// now set options
	{
		const CSG_MetaData	&Parameter	= Tool[i];

		CSG_Parameter	*pParameter, *pOwner;

		if( Parameter.Cmp_Name("comment") )
		{
			continue;
		}
		else if( !Tool_Get_Parameter(Parameter, pTool, &pParameter, &pOwner) )
		{
			return( false );
		}
		else if( Parameter.Cmp_Name("option") )
		{
			if( IS_TRUE_PROPERTY(Parameter, "varname") )
			{	// does option want a value from tool chain parameters and do these provide one ?
				pParameter->Set_Value(Parameters(Parameter.Get_Content()));
			}
			else switch( pParameter->Get_Type() )
			{
			default:
				pParameter->Set_Value(Parameter.Get_Content());
				break;

			case PARAMETER_TYPE_FixedTable:
				if( Parameter("OPTION") )
				{
					pParameter->Serialize(*Parameter("OPTION"), false);
				}
				break;

			case PARAMETER_TYPE_String:
				{
					CSG_String	Value(Parameter.Get_Content());

					for(int j=0; j<Parameters.Get_Count(); j++)
					{
						CSG_String	Var; Var.Printf("$(%s)", Parameters(j)->Get_Identifier());

						if( Value.Find(Var) >= 0 )
						{
							Value.Replace(Var, Parameters(j)->asString());
						}
					}

					pParameter->Set_Value(Value);
				}
				break;
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::Tool_Finalize(const CSG_MetaData &Tool, CSG_Tool *pTool)
{
	int		i;

	//-----------------------------------------------------
	for(i=0; i<Tool.Get_Children_Count(); i++)	// add all data objects declared as output to variable list
	{
		const CSG_MetaData	&Parameter	= Tool[i];

		if( Parameter.Cmp_Name("output") )
		{
			CSG_String	ID	= Parameter.Get_Property("id");

			CSG_Parameter	*pParameter	= pTool->Get_Parameters(Parameter.Get_Property("parms"))
				? pTool->Get_Parameters(Parameter.Get_Property("parms"))->Get_Parameter(ID)
				: pTool->Get_Parameters()->Get_Parameter(ID);

			if( !pParameter || !Data_Add(Parameter.Get_Content(), pParameter) )
			{
				return( false );
			}
		}
	}

	//-----------------------------------------------------
	for(i=-1; i<pTool->Get_Parameters_Count(); i++)	// save memory: free all data objects that have not been added to variable list
	{
		CSG_Parameters	*pParameters	= i < 0 ? pTool->Get_Parameters() : pTool->Get_Parameters(i);

		for(int j=0; j<pParameters->Get_Count(); j++)
		{
			CSG_Parameter	*pParameter	= pParameters->Get_Parameter(j);

			if( pParameter->is_Output() )
			{
				if( pParameter->is_DataObject() )
				{
					if( !Data_Exists(pParameter->asDataObject()) )
					{
						m_Data_Manager.Delete(pParameter->asDataObject());
					}
				}
				else if( pParameter->is_DataObject_List() )
				{
					for(int k=0; k<pParameter->asList()->Get_Data_Count(); k++)
					{
						if( !Data_Exists(pParameter->asList()->Get_Data(k)) )
						{
							m_Data_Manager.Delete(pParameter->asList()->Get_Data(k));
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Chains::CSG_Tool_Chains(const CSG_String &Library_Name, const CSG_String &Path)
{
	m_Library_Name	= Library_Name;

	if( m_Library_Name.is_Empty() )
	{
		m_Library_Name	= "toolchains";
		m_Name			= _TL("Tool Chains");
		m_Description	= _TL("Unsorted tool chains");
		m_Menu			= _TL("Tool Chains");
	}
	else
	{
		CSG_MetaData	XML(SG_File_Make_Path(Path, Library_Name, SG_T("xml")));

		if( !XML.Cmp_Name("toolchains") )
		{
			XML.Destroy();
		}

		m_Name			= GET_XML_CONTENT(XML, "name"       , m_Library_Name       , true);
		m_Description	= GET_XML_CONTENT(XML, "description", _TL("no description"), true);
		m_Menu			= GET_XML_CONTENT(XML, "menu"       , _TL("Tool Chains"   ), true);

		m_Description.Replace("[[", "<");	// support for xml/html tags
		m_Description.Replace("]]", ">");
	}

	//-----------------------------------------------------
	m_nTools	= 0;
	m_pTools	= NULL;
}

//---------------------------------------------------------
CSG_Tool_Chains::~CSG_Tool_Chains(void)
{
	for(int i=0; i<m_nTools; i++)
	{
		delete(m_pTools[i]);
	}

	SG_FREE_SAFE(m_pTools);

	m_nTools	= 0;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Tool_Chains::Get_Info(int Type) const
{
	switch( Type )
	{
	case TLB_INFO_Name       :	return( m_Name );
	case TLB_INFO_Description:	return( m_Description );
	case TLB_INFO_Menu_Path  :	return( m_Menu );
	case TLB_INFO_Category   :	return( _TL("Tool Chains") );
	}

	return( "" );
}

//---------------------------------------------------------
bool CSG_Tool_Chains::Add_Tool(CSG_Tool_Chain *pTool)
{
	m_pTools	= (CSG_Tool_Chain **)SG_Realloc(m_pTools, (m_nTools + 1) * sizeof(CSG_Tool_Chain *));
	m_pTools[m_nTools++]	= pTool;

	pTool->Set_Library_Menu(Get_Info(TLB_INFO_Menu_Path));

	return( true );
}

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Chains::Get_Tool(int Index, TSG_Tool_Type Type) const
{
	CSG_Tool	*pTool	= Index >= 0 && Index < m_nTools ? m_pTools[Index] : NULL;

	return(	pTool && (Type == TOOL_TYPE_Base || Type == pTool->Get_Type()) ? pTool : NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Save_History_to_Model(const CSG_MetaData &History, const CSG_String &File)
{
	if( SG_Compare_Version(History.Get_Property("saga-version"), "2.1.3") < 0 || !(History("TOOL") || History("MODULE")) )
	{
		return( false );
	}

	const CSG_MetaData	&Tool(History("TOOL") ? History["TOOL"] : History["MODULE"]);

	if( !Tool("OUTPUT") )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_MetaData	Chain;

	Chain.Set_Name    ("toolchain"  );
	Chain.Add_Property("saga-version", SAGA_VERSION);

	Chain.Add_Child   ("group"      , "toolchains");
	Chain.Add_Child   ("identifier" , SG_File_Get_Name(File, false));
	Chain.Add_Child   ("name"       , SG_File_Get_Name(File, false));
	Chain.Add_Child   ("description", _TL("created from history"));

	Chain.Add_Child   ("parameters" );
	Chain.Add_Child   ("tools"      );

	_Save_History_Add_Tool(Tool, *Chain("parameters"), *Chain("tools"), true);

	for(int i=0; i<Chain["tools"].Get_Children_Count(); i++)
	{
		Chain["tools"](i)->Del_Property("id");
	}

	return( Chain.Save(File) );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::_Save_History_Add_Tool(const CSG_MetaData &History, CSG_MetaData &Parms, CSG_MetaData &Tools, bool bAddOutput)
{
	if( !History("OUTPUT") || !History["OUTPUT"].Get_Property("id") )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_MetaData	*pParameter, &Tool	= *Tools.Ins_Child("tool", 0);

	CSG_String		Tool_ID(CSG_String::Format("tool_%02d", Tools.Get_Children_Count()));

	Tool.Add_Property("id"     , Tool_ID);
	Tool.Add_Property("library", History.Get_Property("library"));
	Tool.Add_Property("tool"   , History.Get_Property("id"     ));
	Tool.Add_Property("name"   , History.Get_Property("name"   ));

	//-----------------------------------------------------
	const CSG_MetaData	&Output	= History["OUTPUT"];

	CSG_String	VarName	= CSG_String::Format("%s__%s", Tool_ID.c_str(), Output.Get_Property("id"));

	pParameter	= Tool.Add_Child("output", VarName);
	pParameter->Add_Property("parms", Output.Get_Property("parms"));
	pParameter->Add_Property("id"   , Output.Get_Property("id"   ));

	if( bAddOutput )
	{
		pParameter	= Parms.Add_Child("output");

		pParameter->Add_Property("varname"    , VarName);
		pParameter->Add_Property("type"       , Output.Get_Property("type"));
		pParameter->Add_Child   ("name"       , Output.Get_Property("name"));
	//	pParameter->Add_Child   ("description", Output.Get_Property("name"));
	}

	//-----------------------------------------------------
	for(int i=0; i<History.Get_Children_Count(); i++)	// Options and Input
	{
		CSG_MetaData	*pChild	= History.Get_Child(i);

		if( !pChild->Get_Name().Cmp("OPTION") )
		{
			pParameter	= NULL;

			switch( SG_Parameter_Type_Get_Type(pChild->Get_Property("type")) )
			{
			case PARAMETER_TYPE_Bool        :
			case PARAMETER_TYPE_Int         :
			case PARAMETER_TYPE_Double      :
			case PARAMETER_TYPE_Degree      :
			case PARAMETER_TYPE_Date        :
			case PARAMETER_TYPE_Range       :
			case PARAMETER_TYPE_String      :
			case PARAMETER_TYPE_Text        :
			case PARAMETER_TYPE_FilePath    :
			case PARAMETER_TYPE_Table_Field :
			case PARAMETER_TYPE_Table_Fields:
			case PARAMETER_TYPE_Choices     :
				pParameter	= Tool.Add_Child("option", pChild->Get_Content());
				break;

			case PARAMETER_TYPE_Choice      :
				pParameter	= Tool.Add_Child("option", pChild->Get_Property("index"));
				break;

			case PARAMETER_TYPE_Grid_System :
				if( pChild->Get_Children_Count() == 0 )
				{
					pParameter	= Tool.Add_Child("option", pChild->Get_Content());
				}
				break;

			default: break;
			}

			if( pParameter )
			{
				pParameter->Add_Property("parms", pChild->Get_Property("parms"));
				pParameter->Add_Property("id"   , pChild->Get_Property("id"   ));
			}
		}
		else if( !pChild->Get_Name().Cmp("INPUT") )
		{
			_Save_History_Add_Input(*pChild, Parms, Tool);
		}
		else if( !pChild->Get_Name().Cmp("INPUT_LIST") )
		{
			for(int j=0; j<pChild->Get_Children_Count(); j++)
			{
				_Save_History_Add_Input(*pChild->Get_Child(j), Parms, Tool);
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::_Save_History_Add_Input(const CSG_MetaData &History, CSG_MetaData &Parms, CSG_MetaData &Tool)
{
	CSG_MetaData	*pInput	= Tool.Add_Child("input");

	pInput->Add_Property("parms", History.Get_Property("parms"));
	pInput->Add_Property("id"   , History.Get_Property("id"   ));

	if( History("TOOL") || History("MODULE") )
	{
		const CSG_MetaData	&History_Tool(History("TOOL") ? History["TOOL"] : History["MODULE"]);

		if( History_Tool("OUTPUT") && History_Tool["OUTPUT"].Get_Property("id") )
		{
			pInput->Fmt_Content("tool_%02d__%s", Tool.Get_Parent()->Get_Children_Count() + 1, History_Tool["OUTPUT"].Get_Property("id"));

			return( _Save_History_Add_Tool(History_Tool, Parms, *Tool.Get_Parent()) );
		}
	}

	CSG_String	VarName	= CSG_String::Format("%s__%s", Tool.Get_Property("id"), History.Get_Property("id"));

	pInput->Set_Content(VarName);

	CSG_MetaData	*pParameter	= Parms.Ins_Child("input", 0);

	pParameter->Add_Property("varname"    , VarName);
	pParameter->Add_Property("type"       , History.Get_Property("type"));
	pParameter->Add_Child   ("name"       , History.Get_Property("name"));
//	pParameter->Add_Child   ("description", History.Get_Property("name"));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
