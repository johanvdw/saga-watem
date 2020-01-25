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
//                     table_pca.cpp                     //
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
#include "table_pca.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_PCA::CTable_PCA(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Principal Component Analysis"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Principal Component Analysis (PCA) for tables. "
	));

	Add_Reference("Bahrenberg, G., Giese, E., Nipper, J.", "1992",
		"Statistische Methoden in der Geographie 2 - Multivariate Statistik", "pp.198-277."
	);

	Add_Reference("http://lib.stat.cmu.edu/multi/pca.c", SG_T("C-code by F.Murtagh"));
	Add_Reference("http://lib.stat.cmu.edu"            , SG_T("StatLib Web Site"   ));

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Parameters("TABLE",
		"FIELDS"	, _TL("Attributes"),
		_TL("")
	);

	Parameters.Add_Table("",
		"PCA"		, _TL("Principal Components"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("correlation matrix"),
			_TL("variance-covariance matrix"),
			_TL("sums-of-squares-and-cross-products matrix")
		), 1
	);

	Parameters.Add_Int("",
		"NFIRST"	, _TL("Number of Components"),
		_TL("maximum number of calculated first components; set to zero to get all"),
		3, 0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_PCA::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("TABLE") )
	{
		CSG_Table		*pTable		= pParameter->asTable();
		CSG_Parameters	*pFields	= (*pParameters)("FIELDS")->asParameters();

		pFields->Del_Parameters();

		if( pTable && pTable->Get_Field_Count() > 0 )
		{
			for(int i=0; i<pTable->Get_Field_Count(); i++)
			{
				if( SG_Data_Type_is_Numeric(pTable->Get_Field_Type(i)) )
				{
					pFields->Add_Bool("", CSG_String::Format("%d", i), pTable->Get_Field_Name(i), _TL(""), false);
				}
			}
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_PCA::On_Execute(void)
{
	CSG_Vector	Eigen_Values;
	CSG_Matrix	Eigen_Vectors, Matrix;

	//-----------------------------------------------------
	m_pTable	= Parameters("TABLE" )->asTable();
	m_Method	= Parameters("METHOD")->asInt();

	//-----------------------------------------------------
	if( !Get_Fields() )
	{
		Error_Set(_TL("invalid field selection"));

		SG_FREE_SAFE(m_Features);

		return( false );
	}

	if( !Get_Matrix(Matrix) )
	{
		Error_Set(_TL("matrix initialisation failed"));

		SG_FREE_SAFE(m_Features);

		return( false );
	}

	if( !SG_Matrix_Eigen_Reduction(Matrix, Eigen_Vectors, Eigen_Values) )
	{
		Error_Set(_TL("Eigen reduction failed"));

		SG_FREE_SAFE(m_Features);

		return( false );
	}

	//-----------------------------------------------------
	Get_Components(Eigen_Vectors, Eigen_Values);

	//-----------------------------------------------------
	SG_FREE_SAFE(m_Features);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_PCA::Get_Fields(void)
{
	CSG_Parameters	*pFields	= Parameters("FIELDS")->asParameters();

	m_Features	= (int *)SG_Calloc(pFields->Get_Count(), sizeof(int));
	m_nFeatures	= 0;

	for(int iFeature=0; iFeature<pFields->Get_Count(); iFeature++)
	{
		if( pFields->Get_Parameter(iFeature)->asBool() )
		{
			CSG_String	s(pFields->Get_Parameter(iFeature)->Get_Identifier());

			m_Features[m_nFeatures++]	= s.asInt();
		}
	}

	return( m_nFeatures > 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CTable_PCA::is_NoData(int iElement)
{
	CSG_Table_Record	*pElement	= m_pTable->Get_Record(iElement);

	for(int iFeature=0; iFeature<m_nFeatures; iFeature++)
	{
		if( pElement->is_NoData(m_Features[iFeature]) )
		{
			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
inline double CTable_PCA::Get_Value(int iFeature, int iElement)
{
	CSG_Table_Record	*pElement	= m_pTable->Get_Record(iElement);

	switch( m_Method )
	{
	default:
	case 0:	// Correlation matrix: Center and reduce the column vectors.
		return( (pElement->asDouble(iFeature) - m_pTable->Get_Mean(iFeature)) / (sqrt(m_pTable->Get_Count() * m_pTable->Get_Variance(iFeature))) );

	case 1:	// Variance-covariance matrix: Center the column vectors.
		return( (pElement->asDouble(iFeature) - m_pTable->Get_Mean(iFeature)) );

	case 2:	// Sums-of-squares-and-cross-products matrix
		return( (pElement->asDouble(iFeature)) );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_PCA::Get_Matrix(CSG_Matrix &Matrix)
{
	int		i, j1, j2;

	Matrix.Create(m_nFeatures, m_nFeatures);
	Matrix.Set_Zero();

	switch( m_Method )
	{
	//-----------------------------------------------------
	default:
	case 0:	// Correlation matrix: Center and reduce the column vectors.
		for(j1=0; j1<m_nFeatures; j1++)
		{
			Matrix[j1][j1] = 1.0;
		}

		for(i=0; i<m_pTable->Get_Count() && Set_Progress(i, m_pTable->Get_Count()); i++)
		{
			if( !is_NoData(i) )
			{
				for(j1=0; j1<m_nFeatures-1; j1++)
				{
					for(j2=j1+1; j2<m_nFeatures; j2++)
					{
						Matrix[j1][j2]	+= Get_Value(j1, i) * Get_Value(j2, i);
					}
				}
			}
		}
		break;

	//-----------------------------------------------------
	case 1:	// Variance-covariance matrix: Center the column vectors.
	case 2:	// Sums-of-squares-and-cross-products matrix
		for(i=0; i<m_pTable->Get_Count() && Set_Progress(i, m_pTable->Get_Count()); i++)
		{
			if( !is_NoData(i) )
			{
				for(j1=0; j1<m_nFeatures; j1++)
				{
					for(j2=j1; j2<m_nFeatures; j2++)
					{
						Matrix[j1][j2]	+= Get_Value(j1, i) * Get_Value(j2, i);
					}
				}
			}
		}
		break;
	}

	//-----------------------------------------------------
	for(j1=0; j1<m_nFeatures; j1++)
	{
		for(j2=j1; j2<m_nFeatures; j2++)
		{
			Matrix[j2][j1] = Matrix[j1][j2];
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_PCA::Get_Components(CSG_Matrix &Eigen_Vectors, CSG_Vector &Eigen_Values)
{
	int		i, j, n, field0;
	double	Sum, Cum;

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	for(i=0, Sum=0.0; i<m_nFeatures; i++)
	{
		Sum	+= Eigen_Values[i];
	}

	Sum	= Sum > 0.0 ? 100.0 / Sum : 0.0;

	Message_Fmt("\n\n%s, %s, %s", _TL("explained variance"), _TL("explained cumulative variance"), _TL("Eigenvalue"));

	for(j=m_nFeatures-1, Cum=0.0; j>=0; j--)
	{
		Cum	+= Eigen_Values[j] * Sum;

		Message_Fmt("\n%6.2f\t%6.2f\t%18.5f", Eigen_Values[j] * Sum, Cum, Eigen_Values[j]);
	}

	Message_Fmt("\n\n%s:", _TL("Eigenvectors"));

	for(j=0; j<m_nFeatures; j++)
	{
		for(i=0; i<m_nFeatures; i++)
		{
			Message_Fmt("\n%12.4f", Eigen_Vectors[j][m_nFeatures - 1 - i]);
		}

		Message_Add("\n", false);
	}

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	n	= Parameters("NFIRST")->asInt();

	if( n <= 0 || n > m_nFeatures )
	{
		n	= m_nFeatures;
	}

	//-----------------------------------------------------
	CSG_Table	*pPCA	= Parameters("PCA")->asTable();

	if( pPCA == NULL )
	{
		pPCA	= m_pTable;
	}

	if( pPCA != m_pTable )
	{
		pPCA->Destroy();
		pPCA->Fmt_Name("%s [%s]", m_pTable->Get_Name(), _TL("Principal Components"));
	}

	//-----------------------------------------------------
	field0	= pPCA->Get_Field_Count();

	for(i=0; i<n; i++)
	{
		pPCA->Add_Field(CSG_String::Format("%s %d", _TL("Component"), i + 1), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	for(int iElement=0; iElement<m_pTable->Get_Count() && Set_Progress(iElement, m_pTable->Get_Count()); iElement++)
	{
		if( !is_NoData(iElement) )
		{
			CSG_Table_Record	*pElement	= pPCA == m_pTable ? pPCA->Get_Record(iElement) : pPCA->Add_Record();

			for(i=0, j=m_nFeatures-1; i<n; i++, j--)
			{
				double	d	= 0.0;

				for(int k=0; k<m_nFeatures; k++)
				{
					d	+= Get_Value(k, iElement) * Eigen_Vectors[k][j];
				}

				pElement->Set_Value(field0 + i, d);
			}
		}
	}

	//-----------------------------------------------------
	if( pPCA == m_pTable )
	{
		DataObject_Update(m_pTable);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
