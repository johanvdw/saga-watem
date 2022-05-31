
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    io_webservices                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    geocoding.cpp                      //
//                                                       //
//                 Copyrights (C) 2018                   //
//                     Olaf Conrad                       //
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
#include "geocoding.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGeoCoding::CGeoCoding(void)
{
	Set_Name		(_TL("Geocoding"));

	Set_Author		("O.Conrad (c) 2018");

	Set_Description	(_TW(
		"Geocoding of addresses using geocoding services. "
	));

	Add_Reference("https://wiki.openstreetmap.org/wiki/Nominatim",
		SG_T("Nominatim at OpenStreetMap Wiki")
	);

	Add_Reference("http://www.datasciencetoolkit.org",
		SG_T("The Data Science Toolkit")
	);

	Add_Reference("https://developers.google.com/maps/documentation/geocoding/start",
		SG_T("Google Maps Platform, Geocoding API")
	);

	Add_Reference("https://msdn.microsoft.com/en-us/library/ff701714.aspx",
		SG_T("Bing Maps Rest Services, Find a Location by Address")
	);

	Add_Reference("https://developer.mapquest.com/documentation/geocoding-api/",
		SG_T("MapQuest Developer, Geocoding API")
	);

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"LOCATIONS"	, _TL("Locations"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table("",
		"ADDRESSES"	, _TL("Address List"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Table_Field("ADDRESSES",
		"FIELD"		, _TL("Address Field"),
		_TL("")
	);

	Parameters.Add_String("ADDRESSES",
		"ADDRESS"	, _TL("Single Address"),
		_TL(""),
		"Bundesstrasse 55, Hamburg, Germany"
	);

	Parameters.Add_Choice("",
		"PROVIDER"	, _TL("Service Provider"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s",
			SG_T("Nominatim (OpenStreetMap)"),
			SG_T("The Data Science Toolkit"),
			SG_T("Google"),
			SG_T("Bing"),
			SG_T("MapQuest")
		), 0
	);

	Parameters.Add_String("PROVIDER",
		"API_KEY"	, _TL("API Key"),
		_TL(""),
		""
	);

	Parameters.Add_Bool("",
		"METADATA"	, _TL("Store Metadata"),
		_TL(""),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGeoCoding::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("ADDRESSES") )
	{
		pParameters->Set_Enabled("FIELD"   , pParameter->asTable() != NULL);
		pParameters->Set_Enabled("ADDRESS" , pParameter->asTable() == NULL);
		pParameters->Set_Enabled("METADATA", pParameter->asTable() == NULL || pParameter->asTable()->Get_Count() == 1);
	}

	if( pParameter->Cmp_Identifier("PROVIDER") )
	{
		pParameters->Set_Enabled("API_KEY", pParameter->asInt() >= 2 );
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoCoding::On_Execute(void)
{
	int	Field	= Parameters("FIELD")->asInt();

	CSG_Table	Table, *pTable	= Parameters("ADDRESSES")->asTable();

	if( !pTable )
	{
		Field	= 0;
		pTable	= &Table;
		pTable->Set_Name(_TL("Locations"));
		pTable->Add_Field("ADDRESS", SG_DATATYPE_String);
		pTable->Add_Record()->Set_Value(0, Parameters("ADDRESS")->asString());
	}

	//-----------------------------------------------------
	CSG_Shapes	*pLocations	= Parameters("LOCATIONS")->asShapes();

	pLocations->Create(SHAPE_TYPE_Point, pTable->Get_Name(), pTable);
	pLocations->Get_Projection().Set_GCS_WGS84();

	//-----------------------------------------------------
	int	Provider	= Parameters("PROVIDER")->asInt();

	m_API_Key	= Parameters("API_KEY")->asString();

	CWebClient	Connection;

	switch( Provider )
	{
	default: Connection.Create("https://nominatim.openstreetmap.org"); break;	// Nominatim
	case  1: Connection.Create("http://www.datasciencetoolkit.org"  ); break;	// The Data Science Toolkit
	case  2: Connection.Create("http://maps.googleapis.com"         ); break;	// Google
	case  3: Connection.Create("http://dev.virtualearth.net"        ); break;	// Bing
	case  4: Connection.Create("http://www.mapquestapi.com"         ); break;	// MapQuest
	}

	if( !Connection.is_Connected() )
	{
		Error_Set(_TL("failed to connect to server."));

		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<pTable->Get_Count() && Process_Get_Okay(); i++)
	{
		TSG_Point	Location;

		CSG_String	Address(pTable->Get_Record(i)->asString(Field));

		bool	bOkay;

		switch( Provider )
		{
		default: bOkay = Request_Nominatim(Connection, Location, Address); break;	// Nominatim
		case  1: bOkay = Request_DSTK     (Connection, Location, Address); break;	// The Data Science Toolkit
		case  2: bOkay = Request_Google   (Connection, Location, Address); break;	// Google
		case  3: bOkay = Request_Bing     (Connection, Location, Address); break;	// Bing
		case  4: bOkay = Request_MapQuest (Connection, Location, Address); break;	// MapQuest
		}

		if( bOkay )
		{
			CSG_Shape	*pLocation	= pLocations->Add_Shape(pTable->Get_Record(i));

			pLocation->Add_Point(Location);
		}
	}

	if( pTable->Get_Count() == 1 && Parameters("METADATA")->asBool() )
	{
		pLocations->Get_MetaData().Add_Child(m_Answer);
	}

	m_Answer.Destroy();

	//-----------------------------------------------------
	return( pLocations->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void	Replace_Special_Chars(CSG_String &String)
{
	const CSG_String	UTF8[][2]	= {
		{ "�",	"%c2%a1" },	{ "�",	"%c2%a2" },	{ "�",	"%c2%a3" },	{ "�",	"%c2%a4" },
		{ "�",	"%c2%a5" },	{ "�",	"%c2%a6" },	{ "�",	"%c2%a7" },	{ "�",	"%c2%a8" },
		{ "�",	"%c2%a9" },	{ "�",	"%c2%aa" },	{ "�",	"%c2%ab" },	{ "�",	"%c2%ac" },
		{ "�",	"%c2%ad" },	{ "�",	"%c2%ae" },	{ "�",	"%c2%af" },	{ "�",	"%c2%b0" },
		{ "�",	"%c2%b1" },	{ "�",	"%c2%b2" },	{ "�",	"%c2%b3" },	{ "�",	"%c2%b4" },
		{ "�",	"%c2%b5" },	{ "�",	"%c2%b6" },	{ "�",	"%c2%b7" },	{ "�",	"%c2%b8" },
		{ "�",	"%c2%b9" },	{ "�",	"%c2%ba" },	{ "�",	"%c2%bb" },	{ "�",	"%c2%bc" },
		{ "�",	"%c2%bd" },	{ "�",	"%c2%be" },	{ "�",	"%c2%bf" },	{ "�",	"%c3%80" },
		{ "�",	"%c3%81" },	{ "�",	"%c3%82" },	{ "�",	"%c3%83" },	{ "�",	"%c3%84" },
		{ "�",	"%c3%85" },	{ "�",	"%c3%86" },	{ "�",	"%c3%87" },	{ "�",	"%c3%88" },
		{ "�",	"%c3%89" },	{ "�",	"%c3%8a" },	{ "�",	"%c3%8b" },	{ "�",	"%c3%8c" },
		{ "�",	"%c3%8d" },	{ "�",	"%c3%8e" },	{ "�",	"%c3%8f" },	{ "�",	"%c3%90" },
		{ "�",	"%c3%91" },	{ "�",	"%c3%92" },	{ "�",	"%c3%93" },	{ "�",	"%c3%94" },
		{ "�",	"%c3%95" },	{ "�",	"%c3%96" },	{ "�",	"%c3%97" },	{ "�",	"%c3%98" },
		{ "�",	"%c3%99" },	{ "�",	"%c3%9a" },	{ "�",	"%c3%9b" },	{ "�",	"%c3%9c" },
		{ "�",	"%c3%9d" },	{ "�",	"%c3%9e" },	{ "�",	"%c3%9f" },	{ "�",	"%c3%a0" },
		{ "�",	"%c3%a1" },	{ "�",	"%c3%a2" },	{ "�",	"%c3%a3" },	{ "�",	"%c3%a4" },
		{ "�",	"%c3%a5" },	{ "�",	"%c3%a6" },	{ "�",	"%c3%a7" },	{ "�",	"%c3%a8" },
		{ "�",	"%c3%a9" },	{ "�",	"%c3%aa" },	{ "�",	"%c3%ab" },	{ "�",	"%c3%ac" },
		{ "�",	"%c3%ad" },	{ "�",	"%c3%ae" },	{ "�",	"%c3%af" },	{ "�",	"%c3%b0" },
		{ "�",	"%c3%b1" },	{ "�",	"%c3%b2" },	{ "�",	"%c3%b3" },	{ "�",	"%c3%b4" },
		{ "�",	"%c3%b5" },	{ "�",	"%c3%b6" },	{ "�",	"%c3%b7" },	{ "�",	"%c3%b8" },
		{ "�",	"%c3%b9" },	{ "�",	"%c3%ba" },	{ "�",	"%c3%bb" },	{ "�",	"%c3%bc" },
		{ "�",	"%c3%bd" },	{ "�",	"%c3%be" },	{ "�",	"%c3%bf" }, { " ",	  "%%20" },	// String.Replace(" ", "+");
		{ "" ,	""       }
	};

	for(int i=0; !UTF8[i][1].is_Empty(); i++)
	{
		String.Replace(UTF8[i][0], UTF8[i][1]);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoCoding::Request_Nominatim(CWebClient &Connection, TSG_Point &Location, CSG_String &Address)
{
	CSG_String	Request(Address);

	if( Request.Find('=') < 0 )
	{
		Replace_Special_Chars(Request);

		Request	= "search?q=" + Request;
	}
	else
	{
		CSG_Strings	query	= SG_String_Tokenize(Request, ",;&");

		Request	= "search?";

		for(int i=0; i<query.Get_Count(); i++)
		{
			CSG_String	key	= query[i].BeforeFirst('='); key.Trim(true); key.Trim(false);
			CSG_String	val	= query[i]. AfterFirst('='); val.Trim(true); val.Trim(false);

			Replace_Special_Chars(val);

			if( i > 0 )
			{
				Request	+= "&";
			}

			Request	+= key + "=" + val; 
		}
	}

	Request	+= "&format=xml&polygon=1&addressdetails=1";

	if( !Connection.Request(Request, m_Answer) )
	{
		Message_Fmt("\n%s [%s]", _TL("Request failed."), Request.c_str());

		return( false );
	}

	//-----------------------------------------------------
	if( m_Answer.Get_Name().CmpNoCase("searchresults") )
	{
		Message_Fmt("\n%s [%s]", _TL("Warning"), m_Answer.Get_Name().c_str());
	}

	if( !m_Answer("place") )
	{
		Message_Fmt("\n%s [%s: %s]", _TL("Error"), SG_T("place"), Address.c_str());

		return( false );
	}

	const CSG_MetaData	&Place	= m_Answer["place"];

	if( !Place.Get_Property("lon", Location.x)
	||  !Place.Get_Property("lat", Location.y) )
	{
		Message_Fmt("\n%s [%s]", _TL("Error"), SG_T("location"));

		return( false );
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoCoding::Request_DSTK(CWebClient &Connection, TSG_Point &Location, CSG_String &Address)
{
	CSG_String	Request(Address);

	Replace_Special_Chars(Request);

	Request	= "maps/api/geocode/json?address=" + Request;

	//-----------------------------------------------------
	CSG_String	_Answer;

	if( !Connection.Request(Request, _Answer) )
	{
		Message_Fmt("\n%s [%s]", _TL("Request failed."), Request.c_str());

		return( false );
	}

	m_Answer.from_JSON(_Answer);

	Message_Add("\n\n" + _Answer + "\n", false);

	//-----------------------------------------------------
	if( !m_Answer.Cmp_Name("root") )
	{
		Message_Fmt("\n%s [%s]", _TL("Warning"), m_Answer.Get_Name().c_str());
	}

	if( !m_Answer("status") )
	{
		Message_Fmt("\n%s [%s]", _TL("Warning"), SG_T("status"));
	}
	else if( !m_Answer["status"].Cmp_Content("OK") )
	{
		Message_Fmt("\n%s [%s]", _TL("Error"), m_Answer.Get_Name().c_str());

		if( m_Answer("error_message") )
		{
			Message_Add("\n" + m_Answer["error_message"].Get_Content(), false);
		}

		return( false );
	}

	if( !m_Answer("results") || !m_Answer["results"](0) )
	{
		Message_Fmt("\n%s [%s]", _TL("Error"), SG_T("results"));

		return( false );
	}

	const CSG_MetaData	&Result	= m_Answer["results"][0];

	if( !Result("geometry") || !Result["geometry"]("location")
	||  !Result["geometry"]["location"]("lat")
	||  !Result["geometry"]["location"]("lng") )
	{
		return( false );
	}

	Location.x	= Result["geometry"]["location"]["lng"].Get_Content().asDouble();
	Location.y	= Result["geometry"]["location"]["lat"].Get_Content().asDouble();

	if( Result("formatted_address") )
	{
		Address	= Result["formatted_address"].Get_Content();
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoCoding::Request_Google(CWebClient &Connection, TSG_Point &Location, CSG_String &Address)
{
	CSG_String	Request(Address);

	Replace_Special_Chars(Request);

	Request	= "/maps/api/geocode/xml?address=" + Request;

	if( !m_API_Key.is_Empty() )
	{
		Request	+= "&key=" + m_API_Key;
	}

	//-----------------------------------------------------
	if( !Connection.Request(Request, m_Answer) )
	{
		Message_Fmt("\n%s [%s]", _TL("Request failed."), Request.c_str());

		return( false );
	}

	//-----------------------------------------------------
	if( m_Answer.Get_Name().CmpNoCase("GeocodeResponse") )
	{
		Message_Fmt("\n%s [%s]", _TL("Warning"), m_Answer.Get_Name().c_str());
	}

	if( !m_Answer("status") )
	{
		Message_Fmt("\n%s [%s]", _TL("Warning"), SG_T("status"));
	}
	else if( !m_Answer["status"].Cmp_Content("OK") )
	{
		Message_Fmt("\n%s [%s]", _TL("Error"), m_Answer.Get_Name().c_str());

		if( m_Answer("error_message") )
		{
			Message_Add("\n" + m_Answer["error_message"].Get_Content(), false);
		}

		return( false );
	}

	if( !m_Answer("result") )
	{
		Message_Fmt("\n%s [%s]", _TL("Error"), SG_T("result"));

		return( false );
	}

	const CSG_MetaData	&Result	= m_Answer["result"];

	if( !Result("geometry") || !Result["geometry"]("location")
	||  !Result["geometry"]["location"]("lat")
	||  !Result["geometry"]["location"]("lng") )
	{
		return( false );
	}

	Location.x	= Result["geometry"]["location"]["lng"].Get_Content().asDouble();
	Location.y	= Result["geometry"]["location"]["lat"].Get_Content().asDouble();

	if( Result("formatted_address") )
	{
		Address	= Result["formatted_address"].Get_Content();
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoCoding::Request_Bing(CWebClient &Connection, TSG_Point &Location, CSG_String &Address)
{
	CSG_String	Request(Address);

//	http://dev.virtualearth.net/REST/v1/Locations?q=1%20Microsoft%20Way%20Redmond%20WA%2098052&o=xml&key=BingMapsKey

	Request	= "/REST/v1/Locations?o=xml&maxResults=1&q=" + Address;

	Replace_Special_Chars(Request);

	if( !m_API_Key.is_Empty() )
	{
		Request	+= "&key=" + m_API_Key;
	}

	if( !Connection.Request(Request, m_Answer) )
	{
		Message_Fmt("\n%s [%s]", _TL("Request failed."), Request.c_str());

		return( false );
	}

	//-----------------------------------------------------
	if( m_Answer.Get_Name().CmpNoCase("Response") )
	{
		Message_Fmt("\n%s [%s]", _TL("Warning"), m_Answer.Get_Name().c_str());
	}

	if( !m_Answer("StatusDescription") || !m_Answer["StatusDescription"].Cmp_Content("OK") )
	{
		Message_Fmt("\n%s [%s]", _TL("Error"), m_Answer.Get_Name().c_str());

		if( m_Answer("ErrorDetails") )
		{
			Message_Add("\n" + m_Answer["ErrorDetails"].Get_Content(), false);
		}
	}

	if( !m_Answer("ResourceSets")
	||  !m_Answer["ResourceSets"]("ResourceSet")
	||  !m_Answer["ResourceSets"]["ResourceSet"]("Resources")
	||  !m_Answer["ResourceSets"]["ResourceSet"]["Resources"]("Location")
	||  !m_Answer["ResourceSets"]["ResourceSet"]["Resources"]["Location"]("Point") )
	{
		return( false );
	}

	const CSG_MetaData	&Point	= m_Answer["ResourceSets"]["ResourceSet"]["Resources"]["Location"]["Point"];

	if( !Point("Longitude") || !Point["Longitude"].Get_Content().asDouble(Location.x)
	||  !Point("Latitude" ) || !Point["Latitude" ].Get_Content().asDouble(Location.y) )
	{
		Message_Fmt("\n%s [%s]", _TL("Error"), SG_T("location"));

		return( false );
	}

	if( !m_Answer["ResourceSets"]["ResourceSet"]["Resources"]["Location"]("Name") )
	{
		Address	= m_Answer["ResourceSets"]["ResourceSet"]["Resources"]["Location"]["Name"].Get_Content();
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoCoding::Request_MapQuest(CWebClient &Connection, TSG_Point &Location, CSG_String &Address)
{
	CSG_String	Request(Address);

	Replace_Special_Chars(Request);

	CSG_String	APIKey	= "KEY";

	Request	= "geocoding/v1/address?key=" + APIKey + "&location=" + Address;
//www.mapquestapi.com/geocoding/v1/address?key=KEY&location=1600+Pennsylvania+Ave+NW,Washington,DC,20500

	if( !Connection.Request(Request, m_Answer) )
	{
		Message_Fmt("\n%s [%s]", _TL("Request failed."), Request.c_str());

		return( false );
	}

	//-----------------------------------------------------
	if( m_Answer.Get_Name().CmpNoCase("ResultSet") )
	{
		Message_Fmt("\n%s [%s]", _TL("Warning"), m_Answer.Get_Name().c_str());
	}

	if( !m_Answer("Longitude") || !m_Answer["Longitude"].Get_Content().asDouble(Location.x)
	||  !m_Answer("Latitude" ) || !m_Answer["Latitude" ].Get_Content().asDouble(Location.y) )
	{
		Message_Fmt("\n%s [%s]", _TL("Error"), SG_T("location"));

		return( false );
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
