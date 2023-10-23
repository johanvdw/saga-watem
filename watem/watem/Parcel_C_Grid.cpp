#include "Parcel_C_Grid.h"
#include "Calculate_C_Grid.h"
#include <config.h>

Parcel_C_Grid::Parcel_C_Grid()
{

	//-----------------------------------------------------
	// Give some information about your tool...

	Set_Name(_TL("3.6. C berekening attribuut"));

	Set_Author(_TL("Copyright (c) 2022, Johan Van de Wauw"));

	Set_Version(VERSION);

	Set_Description(_TW(
		"Berekening van de C kaart op basis van percelen met C attribuut. "
	)
	);


	//-----------------------------------------------------
	// Define your parameters list...


	CSG_Parameter * shapes = Parameters.Add_Shapes(NULL, "PARCEL_SHAPES", "Percelen", "Percelen (shapefile)", PARAMETER_INPUT, SHAPE_TYPE_Polygon);
	Parameters.Add_Table_Field(shapes, "PARCEL_SHAPE_C_FIELD", "C Field", "Field containing C value");
	Parameters.Add_Grid(
		NULL, "LANDUSE", "Land use grid",
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "C", "C Grid", "C Grid", PARAMETER_OUTPUT, true, SG_DATATYPE_Short); // we use a short to save memory

	Parameters.Add_Bool(NULL, "GRB_VHA", "Use GRB and VHA as extra landuse", "", false);
	// Optional shapes
	Parameters.Add_Shapes("GRB_VHA", "WTZ", "GRB WTZ (VHA-polygoon)", "VHA (polygoon)", PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Polygon);
	Parameters.Add_Shapes("GRB_VHA", "WLAS", "VHA Lijnen", "VHA (lijnen). Eventueel kan de laag GRB Wlas gebruikt worden, deze loopt echter soms achter op de VHA.", PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Line);

	Parameters.Add_Shapes("GRB_VHA", "SBN", "GRB Sbn (spoorbaan)", "GRB Sbn (spoorbaan)", PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Polygon);
	Parameters.Add_Shapes("GRB_VHA", "WBN", "GRB Wbn (wegbaan)", "GRB Wbn (wegbaan)", PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Polygon);

	Parameters.Add_Shapes("GRB_VHA", "WGA", "GRB Wga (wegaanhorigheid)", "GRB Wga (wegaanhorigheid)", PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Polygon);

	Parameters.Add_Shapes("GRB_VHA", "GBG", "GRB Gbg (gebouw aan de grond)", "GRB Gbg (gebouw aan de grond)", PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Polygon);
	Parameters.Add_Shapes("GRB_VHA", "GBA", "GRB Gba (gebouwaanhorigheid)", "GRB Gba (gebouwaanhorigheid)", PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Polygon);
	Parameters.Add_Shapes("GRB_VHA", "TRN", "GRB Trn (terrein)", "GRB Trn (Terrein) - enkel bepaalde klassen worden gebruikt", PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Polygon);
	Parameters.Add_Shapes("GRB_VHA", "KNW", "GRB Knw (Kunstwerk)", "GRB Knw (Kunstwerk)", PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Polygon);

}


bool Parcel_C_Grid::On_Execute()
{
	CSG_Grid* LANDUSE = Parameters("LANDUSE")->asGrid();
	CSG_Grid* C = Parameters("C")->asGrid();
	// set scaling because short is used 
	C->Set_Scaling(0.001);

	CSG_Shapes* shapes = Parameters("PARCEL_SHAPES")->asShapes();
	int field = Parameters("PARCEL_SHAPE_C_FIELD")->asInt();

	// first convert shapes to grid
	
	bool result;
	SG_RUN_TOOL(result, "grid_gridding", 0, // 0: shapes to grid
		SG_TOOL_PARAMETER_SET("INPUT", shapes)
		&& SG_TOOL_PARAMETER_SET("FIELD", field)
		&& SG_TOOL_PARAMETER_SET("TARGET_DEFINITION", 1)	// grid or grid system
		&& SG_TOOL_PARAMETER_SET("GRID", C)
		&& SG_TOOL_PARAMETER_SET("OUTPUT", 2) // attribute
		&& SG_TOOL_PARAMETER_SET("MULTIPLE", 1) // last value 
		&& SG_TOOL_PARAMETER_SET("POLY_TYPE", 1)//cell: als er een pixels overlapt met infrastructuur moet hele pixels infrastructuur  worden
	);
	if (!result) {
		return false;
	}


	CSG_Grid* gbg, * gba, * wga, * knw, * wlas, * wtz;

    const bool grb_vha=Parameters("GRB_VHA")->asBool();

	if (grb_vha)
	{
		// grid maken van GRB achtergrondlagen (gebouwen, kunstwerken)
		
		gbg = BinaryShapetoGrid(Parameters("GBG"), this->Get_System(), 1);
		gba = BinaryShapetoGrid(Parameters("GBA"), this->Get_System(), 1);
		wga = BinaryShapetoGrid(Parameters("WGA"), this->Get_System(), 1);
		knw = BinaryShapetoGrid(Parameters("KNW"), this->Get_System(), 1);
		wlas = BinaryShapetoGrid(Parameters("WLAS"), this->Get_System(), 1);
		wtz = BinaryShapetoGrid(Parameters("WTZ"), this->Get_System(), 1);


	}

	// fill gaps with reclassified landuse
	#pragma omp parallel for
	for (int i = 0; i < C->Get_NCells(); i++) {
		if (C->is_NoData(i))
		{
			if ((grb_vha) && (gbg->asInt(i) || gba->asInt(i) || wga->asInt(i) || knw->asInt(i) || wlas->asInt(i) || wtz->asInt(i)))
			{
				C->Set_Value(i, 0);
			}
			else
			{
				switch (LANDUSE->asInt(i))
				{
				case -9999:
				case 0:
					break; // remains nodata
				case -1:
				case -2:
				case -5:
					C->Set_Value(i, 0);  break;
				case -3:
					C->Set_Value(i, 0.001); break;
				case -4:
				case -6:
					C->Set_Value(i, 0.01); break;
				default: C->Set_Value(i, 0.37); break;
				}
			}
		}

	}

	// C->Set_Name(SG_T("C Grid") + C->Get_Name());
	if (Parameters("GRB_VHA")->asBool())
	{
		delete gbg;
        delete gba;
        delete wga;
        delete knw;
        delete wlas;
        delete wtz;
	}

	return true;
}

Parcel_C_Grid::~Parcel_C_Grid()
{
}
