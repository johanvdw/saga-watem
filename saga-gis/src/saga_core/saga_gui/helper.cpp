/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      Helper.cpp                       //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
#include <wx/config.h>
#include <wx/cursor.h>
#include <wx/utils.h>
#include <wx/mimetype.h>
#include <wx/filename.h>
#include <wx/protocol/http.h>
#include <wx/gdicmn.h>
#include <wx/colour.h>
#include <wx/settings.h>

#include "helper.h"

#include "res_dialogs.h"

#include "saga.h"
#include "saga_frame.h"

#include "info.h"
#include "info_messages.h"

#include "wksp_tool.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString	Get_SignificantDecimals_String(double Value, int maxDecimals)
{
	return( wxString::Format("%.*f", SG_Get_Significant_Decimals(Value, maxDecimals), Value) );
}

//---------------------------------------------------------
wxString	Get_nBytes_asString(double nBytes, int Precision)
{
	if( nBytes < 1024 )
	{
		return( wxString::Format("%.0f %s", nBytes, _TL("bytes")) );
	}

	double	dSize	= nBytes / 1024.0;

	if( dSize < 1024 )
	{
		return( wxString::Format("%.*f %s", Precision < 0 ? SG_Get_Significant_Decimals(dSize, 20) : Precision, dSize, _TL("kB")) );
	}

	dSize	/= 1024.0;

	if( dSize < 1024 )
	{
		return( wxString::Format("%.*f %s", Precision < 0 ? SG_Get_Significant_Decimals(dSize, 20) : Precision, dSize, _TL("MB")) );
	}

	dSize	/= 1024.0;

	return( wxString::Format("%.*f %s", Precision < 0 ? SG_Get_Significant_Decimals(dSize, 20) : Precision, dSize, _TL("GB")) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString	Get_FilePath_Relative(const wxString &Directory, const wxString &FileName)
{
	if( FileName.Find(Directory) != 0 )
	{
		return( FileName );
	}

	wxFileName	fn(FileName);

	fn.MakeRelativeTo(Directory);

	return( fn.GetFullPath() );
}

//---------------------------------------------------------
wxString	Get_FilePath_Absolute(const wxString &Directory, const wxString &FileName)
{
	wxFileName	fn(FileName);

	if( !fn.IsAbsolute() )
	{
		fn.MakeAbsolute(Directory);
	}

	return( fn.GetFullPath() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString		Get_TableInfo_asHTML(const CSG_Table *pTable)
{
	wxString	s;

	if( pTable && pTable->is_Valid() )
	{
		s	+= wxString::Format("<hr><h4>%s</h4>", _TL("Table Description"));

		s	+= wxString::Format("<table border=\"1\"><tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th>",
			_TL("Field"),
			_TL("Name"),
			_TL("Type"),
			_TL("Minimum"),
			_TL("Maximum"),
			_TL("Mean"),
			_TL("Standard Deviation")
		);

		for(int i=0; i<pTable->Get_Field_Count(); i++)
		{
			s	+= wxString::Format("<tr><td>%d</td><td>%s</td><td>%s</td>",
				i + 1,
				pTable->Get_Field_Name(i),
				SG_Data_Type_Get_Name(pTable->Get_Field_Type(i)).c_str()
			);

			if( SG_Data_Type_is_Numeric(pTable->Get_Field_Type(i)) )
			{
				s	+= wxString::Format("<td align=\"right\">%s</td><td align=\"right\">%s</td><td align=\"right\">%s</td><td align=\"right\">%s</td></tr>",
					SG_Get_String(pTable->Get_Minimum(i), -6).c_str(),
					SG_Get_String(pTable->Get_Maximum(i), -6).c_str(),
					SG_Get_String(pTable->Get_Mean   (i), -6).c_str(),
					SG_Get_String(pTable->Get_StdDev (i), -6).c_str()
				);
			}
			else
			{
				s	+= wxString::Format("<td></td><td></td><td></td><td></td></tr>");
			}
		}

		s	+= "</table>";
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	Set_Font(CSG_Parameter *pFont, wxFont &Font, wxColour &Colour)
{
	if( !pFont )
	{
		return( false );
	}

	Colour.Set(
		SG_GET_R(pFont->asColor()),
		SG_GET_G(pFont->asColor()),
		SG_GET_B(pFont->asColor())
	);

	Font.SetNativeFontInfo(pFont->asFont());

	return( true );
}

//---------------------------------------------------------
bool	Set_Font(const wxFont &Font, wxColour Colour, CSG_Parameter *pFont)
{
	if( !pFont )
	{
		return( false );
	}

	pFont->Set_Value((int)SG_GET_RGB(Colour.Red(), Colour.Green(), Colour.Blue()));
	pFont->Set_Value(Font.GetNativeFontInfoDesc().wx_str());

	return( true );
}

//---------------------------------------------------------
wxFont	Get_Font(CSG_Parameter *pFont)
{
	wxColour	Colour;
	wxFont		Font;

	Set_Font(pFont, Font, Colour);

	return( Font );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxColour	Get_Color_asWX(int Color)
{
	return( wxColour(SG_GET_R(Color), SG_GET_G(Color), SG_GET_B(Color)) );
}

//---------------------------------------------------------
int			Get_Color_asInt(wxColour Color)
{
	return( SG_GET_RGB(Color.Red(), Color.Green(), Color.Blue()) );
}

//---------------------------------------------------------
wxColour	SYS_Get_Color(wxSystemColour index)
{
	return( wxSystemSettings::GetColour(index) );
}

//---------------------------------------------------------
void		SYS_Set_Color_BG(wxWindow *pWindow, wxSystemColour index)
{
	if( pWindow )
	{
		pWindow->SetBackgroundColour(SYS_Get_Color(index));
	}
}

//---------------------------------------------------------
void		SYS_Set_Color_BG_Window(wxWindow *pWindow)
{
	if( pWindow )
	{
#if defined(__WXMSW__)
		pWindow->SetBackgroundColour(SYS_Get_Color(wxSYS_COLOUR_WINDOW));
#else
		pWindow->SetBackgroundColour(*wxWHITE);
#endif
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxWindow *	MDI_Get_Frame(void)
{
	return( g_pSAGA_Frame );
}

//---------------------------------------------------------
wxPoint		MDI_Get_Def_Position(void)
{
	static int	n	= 0;

	int		Height	= wxSystemSettings::GetMetric(wxSYS_CAPTION_Y);

	wxPoint	p(n * Height, n * Height);

	n	= n < 10 ? n + 1 : 0;

	return( p );
}

//---------------------------------------------------------
wxSize		MDI_Get_Def_Size(void)
{
	return( wxSize(400, 300) );
}

//---------------------------------------------------------
void		MDI_Top_Window_Push(wxWindow *pWindow)
{
	if( g_pSAGA_Frame )
	{
		g_pSAGA_Frame->Top_Window_Push(pWindow);
	}
}

//---------------------------------------------------------
void		MDI_Top_Window_Pop(wxWindow *pWindow)
{
	if( g_pSAGA_Frame )
	{
		g_pSAGA_Frame->Top_Window_Pop(pWindow);
	}
}

//---------------------------------------------------------
wxWindow *	MDI_Get_Top_Window(void)
{
	return( g_pSAGA_Frame ? g_pSAGA_Frame->Top_Window_Get() : NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Set_Buisy_Cursor(bool bOn)
{
	if( g_pSAGA_Frame )
	{
		g_pSAGA_Frame->SetCursor(bOn ? *wxHOURGLASS_CURSOR : *wxSTANDARD_CURSOR);
	}
}

//---------------------------------------------------------
void		Do_Beep(int Style)
{
	switch( Style )
	{
	case -1:	// no noise...
		break;

	case 0:	default:
#if defined(__WXMSW__)
		Beep(330, 100);
		Beep(440, 100);
		Beep(550, 100);
#else
//		wxBell();
#endif
		break;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		MSG_General_Add_Line(void)
{
	if( g_pINFO )
	{
		g_pINFO->Get_General()->Add_Line();
	}
}

//---------------------------------------------------------
void		MSG_General_Add(const wxString &Message, bool bNewLine, bool bTime, TSG_UI_MSG_STYLE Style)
{
	if( g_pINFO )
	{
		g_pINFO->Get_General()->Add_String(Message, bNewLine, bTime, Style);
	}
}

//---------------------------------------------------------
void		MSG_Error_Add_Line(void)
{
	if( g_pINFO )
	{
		g_pINFO->Get_Errors()->Add_Line();
	}
}

//---------------------------------------------------------
void		MSG_Error_Add(const wxString &Message, bool bNewLine, bool bTime, TSG_UI_MSG_STYLE Style)
{
	if( g_pINFO )
	{
		g_pINFO->Get_Errors()->Add_String(Message, bNewLine, bTime, Style);
	}
}

//---------------------------------------------------------
void		MSG_Execution_Add_Line(void)
{
	if( g_pINFO )
	{
		g_pINFO->Get_Execution()->Add_Line();
	}
}

//---------------------------------------------------------
void		MSG_Execution_Add(const wxString &Message, bool bNewLine, bool bTime, TSG_UI_MSG_STYLE Style)
{
	if( g_pINFO )
	{
		g_pINFO->Get_Execution()->Add_String(Message, bNewLine, bTime, Style);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		g_CONFIG_bSave	= true;

//---------------------------------------------------------
bool		CONFIG_Do_Save(bool bOn)
{
	if( g_CONFIG_bSave != bOn )
	{
		g_CONFIG_bSave	= bOn;

		if( g_CONFIG_bSave )
		{
			wxConfigBase::Get()->Flush();
		}
	}

	return( g_CONFIG_bSave );
}

//---------------------------------------------------------
bool		CONFIG_Read(const wxString &Group, const wxString &Entry, wxString &Value)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format("/%s", Group));

	return( pConfig->Read(Entry, &Value) );
}

//---------------------------------------------------------
bool		CONFIG_Read(const wxString &Group, const wxString &Entry, long &Value)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format("/%s", Group));

	return( pConfig->Read(Entry, &Value) );
}

//---------------------------------------------------------
bool		CONFIG_Read(const wxString &Group, const wxString &Entry, double &Value)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format("/%s", Group));

	return( pConfig->Read(Entry, &Value) );
}

//---------------------------------------------------------
bool		CONFIG_Read(const wxString &Group, const wxString &Entry, bool &Value)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format("/%s", Group));

	return( pConfig->Read(Entry, &Value) );
}

//---------------------------------------------------------
bool		CONFIG_Write(const wxString &Group, const wxString &Entry, const wxString &Value)
{
	if( g_CONFIG_bSave )
	{
		wxConfigBase	*pConfig	= wxConfigBase::Get();

		pConfig->SetPath(wxString::Format("/%s", Group));

		return( pConfig->Write(Entry, Value) ? pConfig->Flush() : false );
	}

	return( true );
}

bool		CONFIG_Write(const wxString &Group, const wxString &Entry, const char *Value)
{
	return( CONFIG_Write(Group, Entry, wxString(Value)) );
}

bool		CONFIG_Write(const wxString &Group, const wxString &Entry, const wchar_t *Value)
{
	return( CONFIG_Write(Group, Entry, wxString(Value)) );
}

//---------------------------------------------------------
bool		CONFIG_Write(const wxString &Group, const wxString &Entry, long Value)
{
	if( g_CONFIG_bSave )
	{
		wxConfigBase	*pConfig	= wxConfigBase::Get();

		pConfig->SetPath(wxString::Format("/%s", Group));

		return( pConfig->Write(Entry, Value) ? pConfig->Flush() : false );
	}

	return( true );
}

//---------------------------------------------------------
bool		CONFIG_Write(const wxString &Group, const wxString &Entry, double Value)
{
	if( g_CONFIG_bSave )
	{
		wxConfigBase	*pConfig	= wxConfigBase::Get();

		pConfig->SetPath(wxString::Format("/%s", Group));

		return( pConfig->Write(Entry, Value) ? pConfig->Flush() : false );
	}

	return( true );
}

//---------------------------------------------------------
bool		CONFIG_Write(const wxString &Group, const wxString &Entry, bool Value)
{
	if( g_CONFIG_bSave )
	{
		wxConfigBase	*pConfig	= wxConfigBase::Get();

		pConfig->SetPath(wxString::Format("/%s", Group));

		return( pConfig->Write(Entry, Value) ? pConfig->Flush() : false );
	}

	return( true );
}

//---------------------------------------------------------
bool		CONFIG_Delete(const wxString &Group)
{
	if( g_CONFIG_bSave )
	{
		wxConfigBase	*pConfig	= wxConfigBase::Get();

		return( pConfig->DeleteGroup(Group) ? pConfig->Flush() : false );
	}

	return( true );
}

bool		CONFIG_Delete(const wxString &Group, const wxString &Entry)
{
	if( g_CONFIG_bSave )
	{
		wxConfigBase	*pConfig	= wxConfigBase::Get();

		pConfig->SetPath(wxString::Format("/%s", Group));

		return( pConfig->DeleteEntry(Entry) ? pConfig->Flush() : false );
	}

	return( true );
}

//---------------------------------------------------------
bool		CONFIG_Read(wxConfigBase *pConfig, CSG_Parameter *pParameter)
{
	long		l;
	double		d;
	wxString	s, Entry(pParameter->Get_Identifier());

	switch( pParameter->Get_Type() )
	{
	case PARAMETER_TYPE_Bool    :
	case PARAMETER_TYPE_Int     :
	case PARAMETER_TYPE_Choice  :
	case PARAMETER_TYPE_Color   :
		return( pConfig->Read(Entry, &l) && pParameter->Set_Value((int)l) );

	case PARAMETER_TYPE_Double  :
	case PARAMETER_TYPE_Degree  :
		return( pConfig->Read(Entry, &d) && pParameter->Set_Value(d) );

	case PARAMETER_TYPE_Date    :
	case PARAMETER_TYPE_String  :
	case PARAMETER_TYPE_Text    :
	case PARAMETER_TYPE_FilePath:
	case PARAMETER_TYPE_Choices :
		return( pConfig->Read(Entry, &s) && pParameter->Set_Value((const SG_Char *)s) );

	case PARAMETER_TYPE_Range   :
		return(
			pConfig->Read(Entry + "_LO", &d) && pParameter->asRange()->Set_Min(d)
		&&	pConfig->Read(Entry + "_HI", &d) && pParameter->asRange()->Set_Max(d)
		);

	case PARAMETER_TYPE_Font    :
		return(
			pConfig->Read(Entry + "_FONT" , &s) && pParameter->Set_Value((const SG_Char *)s)
		&&	pConfig->Read(Entry + "_COLOR", &l) && pParameter->Set_Value((int)l)
		);

	case PARAMETER_TYPE_Colors  :
		return( pConfig->Read(Entry, &s) && pParameter->asColors()->from_Text(&s) );

	case PARAMETER_TYPE_Parameters:
		return( CONFIG_Read(Entry + "/" + pParameter->Get_Identifier(), pParameter->asParameters()) );

	default:
		return( false );
	}
}

//---------------------------------------------------------
bool		CONFIG_Read(const wxString &Group, CSG_Parameters *pParameters)
{
	wxConfigBase	*pConfig	= wxConfigBase::Get();

	pConfig->SetPath(wxString::Format("/%s", Group));

	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		CONFIG_Read(pConfig, pParameters->Get_Parameter(i));
	}

	return( pConfig->Flush() );
}

//---------------------------------------------------------
bool		CONFIG_Write(wxConfigBase *pConfig, CSG_Parameter *pParameter)
{
	if( g_CONFIG_bSave )
	{
		wxString	Entry(pParameter->Get_Identifier());

		switch( pParameter->Get_Type() )
		{
		case PARAMETER_TYPE_Bool    :
		case PARAMETER_TYPE_Int     :
		case PARAMETER_TYPE_Choice  :
		case PARAMETER_TYPE_Color   :
			return( pConfig->Write(Entry, pParameter->asInt()) );

		case PARAMETER_TYPE_Double  :
		case PARAMETER_TYPE_Degree  :
			return( pConfig->Write(Entry, pParameter->asDouble()) );

		case PARAMETER_TYPE_Date    :
		case PARAMETER_TYPE_String  :
		case PARAMETER_TYPE_Text    :
		case PARAMETER_TYPE_FilePath:
		case PARAMETER_TYPE_Choices :
			return( pConfig->Write(Entry, pParameter->asString()) );

		case PARAMETER_TYPE_Range   :
			return(
				pConfig->Write(Entry + "_LO", pParameter->asRange()->Get_Min())
			&&	pConfig->Write(Entry + "_HI", pParameter->asRange()->Get_Max())
			);

		case PARAMETER_TYPE_Font    :
			return(
				pConfig->Write(Entry + "_FONT" , (const SG_Char *)pParameter->asPointer())
			&&	pConfig->Write(Entry + "_COLOR", pParameter->asInt())
			);

		case PARAMETER_TYPE_Colors  :
			{
				CSG_String	s;

				return( pParameter->asColors()->to_Text(s) && pConfig->Write(Entry, s.c_str()) );
			}

		case PARAMETER_TYPE_Parameters:
			return( CONFIG_Write(Entry + "/" + pParameter->Get_Identifier(), pParameter->asParameters()) );

		default:
			return( false );
		}
	}

	return( true );
}

//---------------------------------------------------------
bool		CONFIG_Write(const wxString &Group, CSG_Parameters *pParameters)
{
	if( g_CONFIG_bSave )
	{
		wxConfigBase	*pConfig	= wxConfigBase::Get();

		pConfig->SetPath(wxString::Format("/%s", Group));

		for(int i=0; i<pParameters->Get_Count(); i++)
		{
			CONFIG_Write(pConfig, pParameters->Get_Parameter(i));
		}

		return( pConfig->Flush() );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		PROCESS_is_Executing(void)
{
	return( g_pTool != NULL );
}

//---------------------------------------------------------
bool		PROCESS_Wait(void)
{
	if( g_pSAGA )
	{
		return( g_pSAGA->Process_Wait() );
	}

	return( true );
}

//---------------------------------------------------------
bool		PROCESS_Get_Okay(bool bBlink)
{
	if( g_pSAGA_Frame )
	{
		return( g_pSAGA_Frame->Process_Get_Okay(bBlink) );
	}

	return( true );
}

//---------------------------------------------------------
bool		PROCESS_Set_Okay(bool bOkay)
{
	if( g_pSAGA_Frame )
	{
		return( g_pSAGA_Frame->Process_Set_Okay(bOkay) );
	}

	return( true );
}

//---------------------------------------------------------
bool		PROGRESSBAR_Set_Position(int Position)
{
	if( g_pSAGA_Frame )
	{
		return( g_pSAGA_Frame->ProgressBar_Set_Position(Position) );
	}

	return( true );
}

//---------------------------------------------------------
bool		PROGRESSBAR_Set_Position(double Position, double Range)
{
	if( g_pSAGA_Frame )
	{
		return( g_pSAGA_Frame->ProgressBar_Set_Position(Position, Range) );
	}

	return( true );
}

//---------------------------------------------------------
void		STATUSBAR_Set_Text(const wxString &Text, int iPane)
{
	if( g_pSAGA_Frame )
	{
		g_pSAGA_Frame->StatusBar_Set_Text(Text, iPane);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		Open_Application(const wxString &Reference, const wxString &Mime_Extension)
{
	if( Reference.IsEmpty() )
	{
		return( false );
	}

	if(0&& Reference.Find("ftp:"   ) == 0 )
	{
		wxString	Directory;

		if( DLG_Directory(Directory, _TL("Save to Directory...")) )
		{
			wxBusyCursor	BusyCursor;

			if( SG_FTP_Download(&Directory, &Reference) )
			{
				DLG_Message_Show_Error(Reference, _TL("FTP download finished..."));

				return( true );
			}

			DLG_Message_Show_Error(Reference, _TL("FTP download failed..."));

			return( false );
		}
	}

	if( Reference.Find("ftp:"   ) == 0
	||  Reference.Find("file:"  ) == 0
	||  Reference.Find("http:"  ) == 0
	||  Reference.Find("https:" ) == 0
	||  Reference.Find("mailto:") == 0 )
	{
		return( wxLaunchDefaultBrowser(Reference) );
	}

	//-----------------------------------------------------
	bool	bResult	= false;

	wxFileName	FileName(Reference);

	if( FileName.IsRelative() )	// very likely that this will not work
	{
		FileName.MakeAbsolute(g_pSAGA->Get_App_Path());	// this might not work either, but give it a try
	}

	//-----------------------------------------------------
	if( Mime_Extension.IsEmpty() )
	{
		if( (bResult = wxLaunchDefaultApplication(FileName.GetFullPath())) == false )
		{
			MSG_Error_Add(wxString::Format("%s:\n%s\n", _TL("failed to launch default application"), FileName.GetFullPath().c_str()));
		}
	}

	//-----------------------------------------------------
	else
	{
		wxFileType	*pFileType	= wxTheMimeTypesManager->GetFileTypeFromExtension(Mime_Extension);

		if( pFileType )
		{
			wxString	Command;

			if( !pFileType->GetOpenCommand(&Command, wxFileType::MessageParameters(FileName.GetFullPath(), ""))
			||  !(bResult = wxExecute(Command) == 0) )
			{
				MSG_Error_Add(wxString::Format("%s:\n%s\n", _TL("command execution failed"), Command.c_str()));
			}

			delete(pFileType);
		}
	}

	return( bResult );
}

//---------------------------------------------------------
bool		Open_WebBrowser(const wxString &Reference)
{
	return( wxLaunchDefaultBrowser(Reference) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString Get_Online_Tool_Description(const wxString &Library, const wxString &ID)
{
	static bool	bBuisy	= false;

	wxString	Description;

	if( !bBuisy )
	{
		bBuisy	= true;

		wxInputStream	*pStream;
		wxHTTP			Server;

		wxString	sServer	= "sourceforge.net";
		wxString	sPath	= SG_File_Get_Name(&Library, false).c_str();

		if( sPath.Find("lib") == 0 )	// remove linux prefix 'lib'
		{
			sPath.Remove(0, 3);
		}

		if( !ID.IsEmpty() )
		{
			sPath	+= "_" + ID;
		}

		sPath	= "/p/saga-gis/wiki/" + sPath + "/";

		if( Server.Connect(sServer) && (pStream = Server.GetInputStream(sPath)) != NULL )
		{
			while( !pStream->Eof() )
			{
				Description	+= pStream->GetC();
			}

			int		n;

			if( (n = Description.Find("<div class=\"markdown_content\">")) < 0 )
			{
				Description.Clear();
			}
			else
			{
				Description.Remove(0, n);

				if( (n = Description.Find("</div>")) > 0 )
				{
					Description.Truncate(n + 6);
				}

				Description.Replace("./attachment"      ,        "http://" + sServer + sPath + "attachment");
				Description.Replace("href=\"/p/saga-gis", "href=\"http://" + sServer + "/p/saga-gis");
				Description.Replace("\n", "");
			}

			delete(pStream);
		}

		bBuisy	= false;
	}

	return( Description );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool QGIS_Styles_Import(const CSG_String &File, CSG_Table &Classes)
{
	CSG_String	Attribute;

	return( QGIS_Styles_Import(File, Classes, Attribute) );
}

bool QGIS_Styles_Import(const CSG_String &File, CSG_Table &Classes, CSG_String &Attribute)
{
	CSG_MetaData	QML;

	if( !QML.Load(File) || !QML.Cmp_Name("QGIS") )
	{
		SG_UI_Dlg_Error(CSG_String::Format("[%s]: %s", SG_File_Get_Name(File, true).c_str(), _TL("failed to load file")), _TL("QGIS Styles Import"));

		return( false );
	}

	//-----------------------------------------------------
	if( QML("renderer-v2")
	&&  QML["renderer-v2"]("categories")
	&&  QML["renderer-v2"]("symbols"   ) )
	{
		QML["renderer-v2"].Get_Property("attr", Attribute);

		//-----------------------------------------------------
		Classes.Destroy();

		Classes.Add_Field(_TL("COLOR"      ), SG_DATATYPE_Color );
		Classes.Add_Field(_TL("NAME"       ), SG_DATATYPE_String);
		Classes.Add_Field(_TL("DESCRIPTION"), SG_DATATYPE_String);
		Classes.Add_Field(_TL("MINIMUM"    ), SG_DATATYPE_String);
		Classes.Add_Field(_TL("MAXIMUM"    ), SG_DATATYPE_String);

		//-----------------------------------------------------
		const CSG_MetaData	&Categories	= QML["renderer-v2"]["categories"];
		const CSG_MetaData	&Symbols	= QML["renderer-v2"]["symbols"   ];

		for(int i=0; i<Categories.Get_Children_Count(); i++)
		{
			CSG_String	Value, Label, Symbol;

			if( Categories[i].Get_Property("value" , Value )
			&&  Categories[i].Get_Property("label" , Label )
			&&  Categories[i].Get_Property("symbol", Symbol) )
			{
				CSG_Table_Record	&Class	= *Classes.Add_Record();

				int	Color	= SG_Color_Get_Random();

				for(int j=0; j<Symbols.Get_Children_Count(); j++)
				{
					if( Symbols[j].Cmp_Property("name", Symbol) && Symbols[j]("layer") )
					{
						const CSG_MetaData	&Props	= Symbols[j]["layer"];

						for(int k=0; k<Props.Get_Children_Count(); k++)
						{
							if( Props[k].Cmp_Property("k", "color") )
							{
								CSG_String	Value	= Props[k].Get_Property("v");

								int	r	= Value.asInt(); Value = Value.AfterFirst(',');
								int	g	= Value.asInt(); Value = Value.AfterFirst(',');
								int	b	= Value.asInt(); Value = Value.AfterFirst(',');

								Color	= SG_GET_RGB(r, g, b);
							}
						}

						break;
					}
				}

				Class.Set_Value(0, Color);	// Color
				Class.Set_Value(1, Label);	// Name
				Class.Set_Value(2, Label);	// Description
				Class.Set_Value(3, Value);	// Minimum
				Class.Set_Value(4, Value);	// Maximum
			}
		}

		return( Classes.Get_Count() > 0 );
	}

	//-----------------------------------------------------
	if( QML("pipe")
	&&  QML["pipe"]("rasterrenderer")
	&&  QML["pipe"]["rasterrenderer"]("rastershader")
	&&  QML["pipe"]["rasterrenderer"]["rastershader"]("colorrampshader")
	&&  QML["pipe"]["rasterrenderer"]["rastershader"]["colorrampshader"].Get_Children_Count() > 0 )
	{
		//-------------------------------------------------
		const CSG_MetaData	&ColorRamp	= QML["pipe"]["rasterrenderer"]["rastershader"]["colorrampshader"];

		bool	bExact	= ColorRamp.Cmp_Property("colorRampType", "EXACT");	// "DISCRETE"

		Classes.Destroy();

		Classes.Add_Field(_TL("COLOR"      ), SG_DATATYPE_Color );
		Classes.Add_Field(_TL("NAME"       ), SG_DATATYPE_String);
		Classes.Add_Field(_TL("DESCRIPTION"), SG_DATATYPE_String);
		Classes.Add_Field(_TL("MINIMUM"    ), SG_DATATYPE_Double);
		Classes.Add_Field(_TL("MAXIMUM"    ), SG_DATATYPE_Double);

		CSG_String	Value("-99999");

		//-------------------------------------------------
		for(int i=0; i<ColorRamp.Get_Children_Count(); i++)
		{
			CSG_String	minVal(Value); Value = ColorRamp[i].Get_Property("value");

			wxColour	Color; Color.Set(ColorRamp[i].Get_Property("color"));

			CSG_Table_Record	&Class	= *Classes.Add_Record();

			Class.Set_Value(0, Get_Color_asInt(Color));	// Color
			Class.Set_Value(1, ColorRamp[i].Get_Property("label"));	// Name
			Class.Set_Value(3, bExact ? Value : minVal);	// Minimum
			Class.Set_Value(4, Value);	// Maximum
		}

		//-------------------------------------------------
		return( Classes.Get_Count() > 0 );
	}

	//-----------------------------------------------------
	if( QML("rasterproperties")
	&&  QML["rasterproperties"]("customColorRamp")
	&&  QML["rasterproperties"]["customColorRamp"]("colorRampType")
//	&&  QML["rasterproperties"]["customColorRamp"]["colorRampType"].Cmp_Content("DISCRETE")
	&&  QML["rasterproperties"]["customColorRamp"].Get_Children_Count() > 1 )
	{
		//-------------------------------------------------
		const CSG_MetaData	&ColorRamp	= QML["rasterproperties"]["customColorRamp"];

		bool	bExact	= ColorRamp("colorRampType") && ColorRamp["colorRampType"].Cmp_Content("DISCRETE");

		Classes.Destroy();

		Classes.Add_Field(_TL("COLOR"      ), SG_DATATYPE_Color );
		Classes.Add_Field(_TL("NAME"       ), SG_DATATYPE_String);
		Classes.Add_Field(_TL("DESCRIPTION"), SG_DATATYPE_String);
		Classes.Add_Field(_TL("MINIMUM"    ), SG_DATATYPE_Double);
		Classes.Add_Field(_TL("MAXIMUM"    ), SG_DATATYPE_Double);

		CSG_String	Value("-99999");

		//-------------------------------------------------
		for(int i=0; i<ColorRamp.Get_Children_Count(); i++)
		{
			if( ColorRamp[i].Cmp_Name("colorRampEntry") )
			{
				CSG_Table_Record &Class = *Classes.Add_Record();

				CSG_String	minVal(Value); Value = ColorRamp[i].Get_Property("value");

				Class.Set_Value(1, ColorRamp[i].Get_Property("label"));	// Name
				Class.Set_Value(3, bExact ? Value : minVal);	// Minimum
				Class.Set_Value(4, Value);	// Maximum

				if( ColorRamp[i].Get_Property("color", Value) )
				{
					wxColour Color; Color.Set(ColorRamp[i].Get_Property("color"));

					Class.Set_Value(0, Get_Color_asInt(Color));	// Color
				}
				else
				{
					int	r, g, b;

					ColorRamp[i].Get_Property("red"  , r);
					ColorRamp[i].Get_Property("green", g);
					ColorRamp[i].Get_Property("blue" , b);

					Class.Set_Value(0, SG_GET_RGB(r, g, b));	// Color
				}
			}
		}

		//-------------------------------------------------
		return( Classes.Get_Count() > 0 );
	}

	//-----------------------------------------------------
	SG_UI_Dlg_Error(CSG_String::Format("[%s]: %s", SG_File_Get_Name(File, true).c_str(), _TL("unsupported format")), _TL("QGIS Styles Import"));

	return( false );
}

//---------------------------------------------------------
bool QGIS_Styles_Export(const CSG_String &File, const CSG_Table &Classes, const CSG_String &Attribute)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
