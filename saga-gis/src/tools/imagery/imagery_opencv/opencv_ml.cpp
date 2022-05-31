
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        OpenCV                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     opencv_ml.cpp                     //
//                                                       //
//                 Copyright (C) 2016 by                 //
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
#include "opencv_ml.h"

#if CV_MAJOR_VERSION >= 3


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	CLASS_ID	= 0,
	CLASS_NAME,
	CLASS_COUNT,
	CLASS_R,
	CLASS_G,
	CLASS_B
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenCV_ML::COpenCV_ML(bool bProbability)
{
	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"FEATURES"		, _TL("Features"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Bool("FEATURES",
		"NORMALIZE"		, _TL("Normalize"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("FEATURES",
		"RGB_COLORS"	, _TL("Update Colors from Features"),
		_TL("Use the first three features in list to obtain blue, green, red components for class colour in look-up table."),
		false
	)->Set_UseInCMD(false);

	if( bProbability )
	{
		Parameters.Add_Grid("",
			"PROBABILITY"	, _TL("Probability"),
			_TL(""),
			PARAMETER_OUTPUT_OPTIONAL
		);
	}

	Parameters.Add_Grid("",
		"CLASSES"		, _TL("Classification"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);

	Parameters.Add_Table("CLASSES",
		"CLASSES_LUT"	, _TL("Look-up Table"),
		_TL("A reference list of the grid values that have been assigned to the training classes."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_FilePath("",
		"MODEL_LOAD"	, _TL("Load Model"),
		_TL("Use a model previously stored to file."),
		CSG_String::Format("%s (*.xml)|*.xml|%s|*.*",
			_TL("XML Files"),
			_TL("All Files")
		)
	);

	Parameters.Add_Node("",
		"MODEL_TRAIN"	, _TL("Model Training"),
		_TL("")
	);

	Parameters.Add_Shapes("MODEL_TRAIN",
		"TRAIN_AREAS"	, _TL("Training Areas"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TRAIN_AREAS",
		"TRAIN_CLASS"	, _TL("Class Identifier"),
		_TL("")
	);

	Parameters.Add_Double("TRAIN_AREAS",
		"TRAIN_BUFFER"	, _TL("Buffer Size"),
		_TL("For non-polygon type training areas, creates a buffer with a diameter of specified size."),
		1., 0., true
	);

	Parameters.Add_FilePath("MODEL_TRAIN",
		"MODEL_SAVE"	, _TL("Save Model"),
		_TL("Stores model to file to be used for subsequent classifications instead of training areas."),
		CSG_String::Format("%s (*.xml)|*.xml|%s|*.*",
			_TL("XML Files"),
			_TL("All Files")
		), NULL, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int COpenCV_ML::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("PARAMETERS_GRID_SYSTEM") )
	{
		if( pParameter->asGrid_System()->is_Valid() )
		{
			pParameters->Set_Parameter("TRAIN_BUFFER", pParameter->asGrid_System()->Get_Cellsize());
		}
	}

	//-----------------------------------------------------
	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}
	
//---------------------------------------------------------
int COpenCV_ML::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("FEATURES") )
	{
		pParameters->Set_Enabled("RGB_COLORS", pParameter->asGridList()->Get_Grid_Count() >= 3);
	}

	if( pParameter->Cmp_Identifier("MODEL_LOAD") )
	{
		bool	bOkay	= Check_Model_File(pParameter->asString());

		if( !bOkay )
		{
			pParameter->Set_Value("");
		}

		pParameters->Set_Enabled("MODEL_TRAIN", !bOkay);
		pParameters->Set_Enabled("RGB_COLORS" , !bOkay);
		pParameters->Set_Enabled("CLASSES_LUT", !bOkay);
	}

	if( pParameter->Cmp_Identifier("TRAIN_AREAS") )
	{
		pParameters->Set_Enabled("TRAIN_BUFFER", pParameter->asShapes() && pParameter->asShapes()->Get_Type() != SHAPE_TYPE_Polygon);
	}

	//-----------------------------------------------------
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COpenCV_ML::_Initialize(void)
{
	m_pFeatures		= Parameters("FEATURES"   )->asGridList();
	m_pClasses		= Parameters("CLASSES"    )->asGrid();
	m_pProbability	= Parameters("PROBABILITY") ? Parameters("PROBABILITY")->asGrid() : NULL;
	m_bNormalize	= Parameters("NORMALIZE"  )->asBool();

	//-----------------------------------------------------
	m_pClasses->Set_NoData_Value(-1.);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			bool	bNoData	= false;

			for(int i=0; !bNoData && i<m_pFeatures->Get_Grid_Count(); i++)
			{
				bNoData	= m_pFeatures->Get_Grid(i)->is_NoData(x, y);
			}

			m_pClasses->Set_Value(x, y, bNoData ? -1. : 0.);
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool COpenCV_ML::_Finalize(void)
{
	m_pClasses->Set_Name(Get_Name());

	if( m_pProbability )
	{
		m_pProbability->Set_Name(Get_Name() + " [" + _TL("Probability") + "]");
	}

	//-----------------------------------------------------
	if( m_Classes.Get_Count() > 0 )
	{
		CSG_Parameter	*pLUT	= DataObject_Get_Parameter(m_pClasses, "LUT");

		if( pLUT )
		{
			bool	bRGB	= m_pFeatures->Get_Grid_Count() >= 3 && Parameters("RGB_COLORS")->asBool();

			for(int i=0; i<m_Classes.Get_Count(); i++)
			{
				CSG_Table_Record	*pClass	= pLUT->asTable()->Get_Record(i);

				if( !pClass )
				{
					(pClass	= pLUT->asTable()->Add_Record())->Set_Value(0, SG_Color_Get_Random());
				}

				pClass->Set_Value(1, m_Classes[i].asString(1));
				pClass->Set_Value(2, m_Classes[i].asString(1));
				pClass->Set_Value(3, m_Classes[i].asInt   (0));
				pClass->Set_Value(4, m_Classes[i].asInt   (0));

				if( bRGB )
				{
					#define SET_COLOR_COMPONENT(c, i)	c = 127 + 127 * (m_bNormalize ? c : (c - m_pFeatures->Get_Grid(i)->Get_Mean()) / m_pFeatures->Get_Grid(i)->Get_StdDev());\
						if( c < 0 ) c = 0; else if( c > 255 ) c = 255;

					double	r = m_Classes[i].asDouble(CLASS_R) / m_Classes[i].asInt(CLASS_COUNT); SET_COLOR_COMPONENT(r, 2);
					double	g = m_Classes[i].asDouble(CLASS_G) / m_Classes[i].asInt(CLASS_COUNT); SET_COLOR_COMPONENT(g, 1);
					double	b = m_Classes[i].asDouble(CLASS_B) / m_Classes[i].asInt(CLASS_COUNT); SET_COLOR_COMPONENT(b, 0);

					pClass->Set_Value(0, SG_GET_RGB(r, g, b));
				}
			}

			pLUT->asTable()->Set_Record_Count(m_Classes.Get_Count());

			DataObject_Set_Parameter(m_pClasses, pLUT);
			DataObject_Set_Parameter(m_pClasses, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
		}

		//-------------------------------------------------
		if( Parameters("CLASSES_LUT")->asTable() )
		{
			CSG_Table	&LUT	= *Parameters("CLASSES_LUT")->asTable();

			LUT.Destroy();
			LUT.Set_Name(m_pClasses->Get_Name());
			LUT.Add_Field("VALUE", SG_DATATYPE_Int   );
			LUT.Add_Field("CLASS", SG_DATATYPE_String);

			for(int i=0; i<m_Classes.Get_Count(); i++)
			{
				CSG_Table_Record	&Class	= *LUT.Add_Record();

				Class.Set_Value(0, m_Classes[i].asInt   (0));
				Class.Set_Value(1, m_Classes[i].asString(1));
			}
		}

		m_Classes.Destroy();
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COpenCV_ML::Check_Model_File(const CSG_String &File)
{
	if( !SG_File_Exists(File) )
	{
		return( false );
	}
	
	CSG_MetaData	Model(File);

	if( !Model.Load(File) || !Model.Cmp_Name("opencv_storage") || !Model("opencv_ml_" + CSG_String(Get_Model_ID())) )
	{
		Error_Fmt("%s: %s", _TL("failed to load model"), File.c_str());

		return( false);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double COpenCV_ML::_Get_Feature(int x, int y, int iFeature)
{
	CSG_Grid	*pFeature	= m_pFeatures->Get_Grid(iFeature);

	double	Feature	= pFeature->asDouble(x, y);

	if( m_bNormalize )
	{
		return( (Feature - pFeature->Get_Mean()) / pFeature->Get_StdDev() );
	}

	return( Feature );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COpenCV_ML::_Get_Prediction(const Ptr<StatModel> &Model)
{
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pClasses->is_NoData(x, y) )
			{
				Mat	Sample(1, m_pFeatures->Get_Grid_Count(), CV_32FC1);

				for(int i=0; i<m_pFeatures->Get_Grid_Count(); i++)
				{
					Sample.at<float>(i)	= (float)_Get_Feature(x, y, i);
				}

				m_pClasses->Set_Value(x, y, Model->predict(Sample));

				if( m_pProbability )
				{
					m_pProbability->Set_Value(x, y, Get_Probability(Model, Sample));
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COpenCV_ML::_Get_Training(CSG_Matrix &Data)
{
	m_Classes.Destroy();

	m_Classes.Add_Field("ID"   , SG_DATATYPE_Int   );	// CLASS_ID
	m_Classes.Add_Field("NAME" , SG_DATATYPE_String);	// CLASS_NAME
	m_Classes.Add_Field("COUNT", SG_DATATYPE_Int   );	// CLASS_COUNT
	m_Classes.Add_Field("RED"  , SG_DATATYPE_Double);	// CLASS_R
	m_Classes.Add_Field("GREEN", SG_DATATYPE_Double);	// CLASS_G
	m_Classes.Add_Field("BLUE" , SG_DATATYPE_Double);	// CLASS_B

	//-----------------------------------------------------
	int	Field	= Parameters("TRAIN_CLASS")->asInt();

	CSG_Shapes	Polygons, *pPolygons	= Parameters("TRAIN_AREAS")->asShapes();

	if( pPolygons->Get_Type() != SHAPE_TYPE_Polygon )
	{
		double	Buffer	= Parameters("TRAIN_BUFFER")->asDouble() / 2.;	// diameter, not radius!

		if( Buffer <= 0. )
		{
			Error_Set(_TL("buffer size must not be zero"));

			return( false );
		}

		Polygons.Create(SHAPE_TYPE_Polygon);
		Polygons.Add_Field(pPolygons->Get_Field_Name(Field), pPolygons->Get_Field_Type(Field));

		for(int iShape=0; iShape<pPolygons->Get_Count(); iShape++)
		{
			CSG_Shape	*pShape		= pPolygons->Get_Shape(iShape);
			CSG_Shape	*pBuffer	= Polygons.Add_Shape();

			*pBuffer->Get_Value(0)	= *pShape->Get_Value(Field);

			SG_Polygon_Offset(pShape, Buffer, 5 * M_DEG_TO_RAD, pBuffer);
		}

		pPolygons = &Polygons; Field = 0;
	}

	//-----------------------------------------------------
	pPolygons->Set_Index(Field, TABLE_INDEX_Ascending);

	CSG_String			Label;
	CSG_Table_Record	*pClass	= NULL;

	//-----------------------------------------------------
	for(int iPolygon=0, ID=0; iPolygon<pPolygons->Get_Count(); iPolygon++)
	{
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape_byIndex(iPolygon);

		if( !pClass || Label.Cmp(pPolygon->asString(Field)) )
		{
			pClass	= m_Classes.Add_Record();

			pClass->Set_Value(CLASS_ID  , ID++);
			pClass->Set_Value(CLASS_NAME, Label = pPolygon->asString(Field));
		}

		_Get_Training(Data, pClass, pPolygon);
	}

	//-----------------------------------------------------
	for(int iClass=m_Classes.Get_Count()-1; iClass>=0; iClass--)
	{
		if( m_Classes[iClass].asInt(CLASS_COUNT) < 1 )
		{
			m_Classes.Del_Record(iClass);
		}
	}

	return( m_Classes.Get_Count() > 1 );
}

//---------------------------------------------------------
bool COpenCV_ML::_Get_Training(CSG_Matrix &Data, CSG_Table_Record *pClass, CSG_Shape_Polygon *pArea)
{
	int	ID = pClass->asInt(CLASS_ID), n = 0; double	r = 0., g = 0., b = 0.;

	int	xMin	= Get_System().Get_xWorld_to_Grid(pArea->Get_Extent().Get_XMin());	if( xMin <  0        ) xMin = 0;
	int	xMax	= Get_System().Get_xWorld_to_Grid(pArea->Get_Extent().Get_XMax());	if( xMax >= Get_NX() ) xMax = Get_NX() - 1;
	int	yMin	= Get_System().Get_yWorld_to_Grid(pArea->Get_Extent().Get_YMin());	if( yMin <  0        ) yMin = 0;
	int	yMax	= Get_System().Get_yWorld_to_Grid(pArea->Get_Extent().Get_YMax());	if( yMax >= Get_NY() ) yMax = Get_NY() - 1;

	for(int y=yMin; y<=yMax; y++)
	{
		for(int x=xMin; x<=xMax; x++)
		{
			if( !m_pClasses->is_NoData(x, y) && pArea->Contains(Get_System().Get_Grid_to_World(x, y)) )
			{
				CSG_Vector	z(1 + m_pFeatures->Get_Grid_Count());

				z[m_pFeatures->Get_Grid_Count()]	= ID;

				for(int i=0; i<m_pFeatures->Get_Grid_Count(); i++)
				{
					z[i]	= _Get_Feature(x, y, i);
				}

				Data.Add_Row(z);

				if( m_pFeatures->Get_Grid_Count() >= 3 )
				{
					r += z[2]; g += z[1]; b += z[0];
				}

				n++;
			}
		}
	}

	if( n > 0 )
	{
		pClass->Add_Value(CLASS_COUNT, n);
		pClass->Add_Value(CLASS_R    , r);
		pClass->Add_Value(CLASS_G    , g);
		pClass->Add_Value(CLASS_B    , b);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
Ptr<TrainData> COpenCV_ML::Get_Training(const CSG_Matrix &Data)
{
	Mat	Samples (Data.Get_NRows(), Data.Get_NCols() - 1, CV_32F);
	Mat	Response(Data.Get_NRows(),                    1, CV_32S);

	for(int i=0; i<Data.Get_NRows(); i++)
	{
		Response.at<int>(i)	= (int)Data[i][Data.Get_NCols() - 1];

		for(int j=0; j<Samples.cols; j++)
		{
			Samples.at<float>(i, j)	= (float)Data[i][j];
		}
	}

	//-----------------------------------------------------
	return( TrainData::create(Samples, ROW_SAMPLE, Response) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COpenCV_ML::On_Execute(void)
{
	//-----------------------------------------------------
	Process_Set_Text(_TL("initializing"));

	if( !_Initialize() )
	{
		return( false );
	}

	Ptr<StatModel>	Model;

	//-----------------------------------------------------
	if( SG_File_Exists(Parameters("MODEL_LOAD")->asString()) )
	{
		Model	= Get_Model(Parameters("MODEL_LOAD")->asString());
	}
	else
	{
		Process_Set_Text(_TL("preparing training"));

		CSG_Matrix	Data;

		if( !_Get_Training(Data) )
		{
			return( false );
		}

		Ptr<TrainData>	tData	= Get_Training(Data);	Data.Destroy();

		Process_Set_Text(_TL("training"));

		Model	= Get_Model();

		try
		{
			Model->train(tData);
		}
		catch(...)
		{
			Error_Set(_TL("Failed to train the model."));

			return( false );
		}

		if( *Parameters("MODEL_SAVE")->asString() )
		{
			CSG_String	File(Parameters("MODEL_SAVE")->asString());

			Model->save(File.b_str());
		}
	}

	//-----------------------------------------------------
	if( !Model->isTrained() )
	{
		Error_Set(_TL("Model is not trained"));

		return( false );
	}

	Process_Set_Text(_TL("prediction"));

	_Get_Prediction(Model);

	//-----------------------------------------------------
	return( _Finalize() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // CV_MAJOR_VERSION >= 3


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
