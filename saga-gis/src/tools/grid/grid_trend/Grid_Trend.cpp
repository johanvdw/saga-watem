/**********************************************************
 * Version $Id: Exercise_01.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Lectures                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Exercise_01.cpp                     //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
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
#include "Grid_Trend.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Trend::CGrid_Trend(void)
{
	//-----------------------------------------------------
	// Give some information about your tool...

	Set_Name(_TL("Grid function xy"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TW(
		"(c) 2003 by Olaf Conrad, Goettingen\n"
		"email: oconrad@gwdg.de")
	);


	//-----------------------------------------------------
	// Define your parameters list...

	Parameters.Add_Grid(
		NULL, "INPUT", _TL("Input"),
		_TL("This must be your input data of type grid."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "OUTPUT", _TL("Output"),
		_TL("This will contain your output data of type grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL, "INDEX", _TL("index"),
		_TL("This is the index by which will be grouped."),
		PARAMETER_INPUT
	);

}

//---------------------------------------------------------
CGrid_Trend::~CGrid_Trend(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Trend::On_Execute(void)
{
	CSG_Grid	*pInput, *pOutput, *pIndex;


	//-----------------------------------------------------
	// Get parameter settings...

	pInput = Parameters("INPUT")->asGrid();
	pOutput = Parameters("OUTPUT")->asGrid();
	pIndex = Parameters("INDEX")->asGrid();


	//-----------------------------------------------------
	// Execute calculation...

	// first count how many values we have per type - so we can preallocate the matrices
	std::vector<int> index_count(pIndex->Get_ZMax() + 1);
	for (int y = 0; y < Get_NY() && Set_Progress(y); y++)
	{
		for (int x = 0; x < Get_NX(); x++)
		{
			int nr = pIndex->asInt(x, y);
			if (nr > 0) index_count[nr] ++;
		}
	}

	std::vector <CSG_Matrix*> xyz_matrix(pIndex->Get_ZMax() + 1);

	//allocate matrices
	for (std::vector<int>::size_type i = 0; i != xyz_matrix.size(); i++)
	{
		xyz_matrix[i] = new CSG_Matrix(index_count[i], 3);
	}
	
	std::vector<int>index_position(pIndex->Get_ZMax() + 1);
	
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			int i = pIndex->asInt(x,y);
			CSG_Vector row(3);
			row[0] = pInput->asDouble(x, y); //dependent variable z
			row[1] = x;
			row[2] = y;

			if (i >= 0) {
				bool success = xyz_matrix[i]->Set_Row(index_position[i], row);
				index_position[i]++;
			}
		}
	}



	CSG_Strings pNames;
	pNames.Add("Z");
	pNames.Add("X");
	pNames.Add("Y");

	// calculate stats for every grid
	std::vector <CSG_Regression_Multiple> xyzstat(pIndex->Get_ZMax()+1);
	for (std::vector<int>::size_type i = 0; i != xyz_matrix.size(); i++) {
		xyzstat[i].Set_Data(*xyz_matrix[i], &pNames);
		bool success = xyzstat[i].Get_Model();
		Message_Add(xyzstat[i].Get_Info(), false);
	}
	// calculate new value for every grid cell
	for (int y = 0; y<Get_NY() && Set_Progress(y); y++)
	{
		for (int x = 0; x<Get_NX(); x++)
		{
			int i = pIndex->asInt(x, y);
			CSG_Vector pred(2);
			pred[0] = x;
			pred[1] = y;
			
			try {
				if (i > 0) pOutput->Set_Value(x, y, xyzstat[i].Get_Value(pred));
			}
			catch (std::exception ex)
			{}
			;
			//xystat[i]
		}
	}
	//-----------------------------------------------------
	// Return 'true' if everything went okay...

	for (std::vector<int>::size_type i = 0; i != xyz_matrix.size(); i++)
	{
		delete xyz_matrix[i];
	}
	xyz_matrix.clear();
	xyzstat.clear();

	return( true );
}
