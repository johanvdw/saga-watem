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
//                   pointcloud_tools                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 pc_cluster_analysis.h                 //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                    Volker Wichmann                    //
//                                                       //
//   This tool is a port of the Grid_Cluster_Analysis    //
//   tool from the Grid_Discretisation module library,   //
//                 Copyright (C) 2003 by                 //
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
//    e-mail:     wichmann@laserdata                     //
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
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__PC_Cluster_Analysis_H
#define HEADER_INCLUDED__PC_Cluster_Analysis_H

//---------------------------------------------------------
#include "MLB_Interface.h"

#include <vector>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPC_Cluster_Analysis : public CSG_Tool
{
public:
	CPC_Cluster_Analysis(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("Classification") );	}


protected:

	virtual bool			On_Execute		(void);


private:

	int						*m_Features, m_nFeatures, clustField, *nMembers;

	double					**Centroids, *Variances;

	CSG_PointCloud			*pPC_in, *pPC_out, PC_out;

	std::vector< std::vector<double> >	vValues;	

	void					Write_Result	(CSG_Table *pTable, long nElements, int nCluster, double SP);

	double					MinimumDistance	(long &nElements, int nCluster);
	double					HillClimbing	(long &nElements, int nCluster);

	virtual bool			On_After_Execution(void);

};

#endif // #ifndef HEADER_INCLUDED__PC_Cluster_Analysis_H
