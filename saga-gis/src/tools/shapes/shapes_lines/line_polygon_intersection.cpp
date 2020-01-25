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
//                     shapes_lines                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             line_polygon_intersection.cpp             //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "line_polygon_intersection.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLine_Polygon_Intersection::CLine_Polygon_Intersection(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Line-Polygon Intersection"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Line-polygon intersection."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		"", "LINES"		, _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		"", "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		"", "INTERSECT"	, _TL("Intersection"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		"", "DIFFERENCE", _TL("Difference"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Choice(
		"", "ATTRIBUTES", _TL("Attributes"),
		_TL("attributes inherited to intersection result"),
		CSG_String::Format("%s|%s|%s",
			_TL("polygon"),
			_TL("line"),
			_TL("line and polygon")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Polygon_Intersection::On_Execute(void)
{
	CSG_Shapes	*pLines			= Parameters("LINES"     )->asShapes();
	CSG_Shapes	*pPolygons		= Parameters("POLYGONS"  )->asShapes();
	CSG_Shapes	*pIntersection	= Parameters("INTERSECT" )->asShapes();
	CSG_Shapes	*pDifference	= Parameters("DIFFERENCE")->asShapes();

	if(	pLines->Get_Count() < 1 || pPolygons->Get_Count() < 1 )
	{
		Error_Set(_TL("empty input layer"));

		return( false );
	}

	//-----------------------------------------------------
	int	Attributes	= Parameters("ATTRIBUTES")->asInt();

	pIntersection->Create(SHAPE_TYPE_Line, NULL, Attributes == 0 ? pPolygons : pLines);
	pDifference  ->Create(SHAPE_TYPE_Line, NULL,                               pLines);

	pIntersection->Fmt_Name("%s [%s: %s]", pLines->Get_Name(), _TL("Intersection"), pPolygons->Get_Name());
	pDifference  ->Fmt_Name("%s [%s: %s]", pLines->Get_Name(), _TL("Difference"  ), pPolygons->Get_Name());

	if( Attributes == 2 )
	{
		for(int iField=0; iField<pPolygons->Get_Field_Count(); iField++)
		{
			pIntersection->Add_Field(pPolygons->Get_Field_Name(iField), pPolygons->Get_Field_Type(iField));
		}
	}

	if( !pLines->Get_Extent().Intersects(pPolygons->Get_Extent()) )	// no intersection >> difference == lines
	{
		pDifference->Assign(pLines);

		return( true );
	}

	//-----------------------------------------------------
	for(int iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		CSG_Shapes	Intersection(SHAPE_TYPE_Line, NULL, pLines);
		CSG_Shapes	Difference  (SHAPE_TYPE_Line, NULL, pLines);

		CSG_Shape	*pLine	= Difference.Add_Shape(pLines->Get_Shape(iLine));

		for(int iPolygon=0; pLine->Get_Part_Count() > 0 && iPolygon<pPolygons->Get_Count(); iPolygon++)
		{
			CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon);

			if( Get_Intersection(pPolygon, pLine, Intersection) )
			{
				if( Attributes == 0 )	// inherit attributes from polygon, create one polyline from intersection segments
				{
					CSG_Shape	*pNew	= pIntersection->Add_Shape(pPolygon, SHAPE_COPY_ATTR);

					for(int iSegment=0; iSegment<Intersection.Get_Count(); iSegment++)
					{
						CSG_Shape_Line	*pParts	= (CSG_Shape_Line *)Intersection.Get_Shape(iSegment);

						for(int iPart=0; iPart<pParts->Get_Part_Count(); iPart++)
						{
							pNew->Add_Part(pParts->Get_Part(iPart));
						}
					}
				}
				else					// keep original line attributes
				{
					for(int iSegment=0; iSegment<Intersection.Get_Count(); iSegment++)
					{
						CSG_Shape	*pNew	= pIntersection->Add_Shape(Intersection.Get_Shape(iSegment));

						if( Attributes == 2 )
						{
							for(int iField=0, jField=pLines->Get_Field_Count(); iField<pPolygons->Get_Field_Count(); iField++, jField++)
							{
								*pNew->Get_Value(jField)	= *pPolygon->Get_Value(iField);
							}
						}
					}
				}
			}
		}

		if( pLine->is_Valid() )
		{
			pDifference->Add_Shape(pLine);
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Polygon_Intersection::Get_Intersection(CSG_Shape_Polygon *pPolygon, CSG_Shape *pLine, CSG_Shapes &Intersection)
{
	if( !pLine->Intersects(pPolygon) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	Segments(SHAPE_TYPE_Line);

	CSG_Table	Crossings;

	Crossings.Add_Field("X", SG_DATATYPE_Double);
	Crossings.Add_Field("Y", SG_DATATYPE_Double);
	Crossings.Add_Field("D", SG_DATATYPE_Double);

	//-----------------------------------------------------
	for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
	{
		if( pLine->Get_Point_Count(iPart) < 2 )
		{
			continue;
		}

		TSG_Point	A	= pLine->Get_Point(0, iPart);

		CSG_Shape	*pSegment	= Segments.Add_Shape();
		
		pSegment->Add_Point(A);

		for(int iPoint=1; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
		{
			TSG_Point	B = A; A = pLine->Get_Point(iPoint, iPart);

			if( A.x == B.x && A.y == B.y )
			{
				continue;
			}

			if( Get_Crossings(pPolygon, A, B, Crossings) > 0 )
			{
				for(int iCrossing=0; iCrossing<Crossings.Get_Count(); iCrossing++)
				{
					B.x	= Crossings[iCrossing].asDouble(0);
					B.y	= Crossings[iCrossing].asDouble(1);

					pSegment->Add_Point(B);
					pSegment = Segments.Add_Shape();
					pSegment->Add_Point(B);
				}
			}

			pSegment->Add_Point(A);
		}
	}

	//-----------------------------------------------------
	Intersection.Del_Records();

	pLine->Del_Parts();

	for(int iSegment=0; iSegment<Segments.Get_Count(); iSegment++)
	{
		CSG_Shape_Line	*pSegment	= (CSG_Shape_Line *)Segments.Get_Shape(iSegment);

		if( pSegment->Get_Length(0) > 0 )
		{
			TSG_Point	A	= pSegment->Get_Point(0);

			if( pPolygon->is_OnEdge(A) )
			{
				TSG_Point	B	= pSegment->Get_Point(1);

				A.x	+= B.x - A.x;
				A.y	+= B.y - A.y;
			}

			if( pPolygon->Contains(A) )
			{
				Intersection.Add_Shape(pLine, SHAPE_COPY_ATTR)->Add_Part(pSegment->Get_Part(0));
			}
			else
			{
				pLine->Add_Part(pSegment->Get_Part(0));
			}
		}
	}

	//-----------------------------------------------------
	return( Intersection.Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLine_Polygon_Intersection::Get_Crossings(CSG_Shape_Polygon *pPolygon, const TSG_Point &a, const TSG_Point &b, CSG_Table &Crossings)
{
	Crossings.Del_Records();

	for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
	{
		TSG_Point	A	= pPolygon->Get_Point(pPolygon->Get_Point_Count(iPart) - 1, iPart);

		for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
		{
			TSG_Point	C, B = A; A = pPolygon->Get_Point(iPoint, iPart);

			if( SG_Get_Crossing(C, A, B, a, b) )
			{
				CSG_Table_Record	*pCrossing	= Crossings.Add_Record();

				pCrossing->Set_Value(0, C.x);
				pCrossing->Set_Value(1, C.y);
				pCrossing->Set_Value(2, SG_Get_Distance(b, C));
			}
		}
	}

	Crossings.Set_Index(2, TABLE_INDEX_Ascending);

	return( Crossings.Get_Count() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
