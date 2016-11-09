#include "Calculate_Uparea.h"

CCalculate_Uparea::CCalculate_Uparea()
{
	//-----------------------------------------------------
	// Give some information about your tool...

	Set_Name(_TL("02: Uparea berekening"));

	Set_Author(_TL("Copyrights (c) 2016 by Johan Van de Wauw"));

	Set_Description(_TW(
		"(c) 2016")
	);


	//-----------------------------------------------------
	// Define your parameters list...


	Parameters.Add_Grid(
		NULL, "DEM", "Elevation",
		"",
		PARAMETER_INPUT
	);


	Parameters.Add_Grid(
		NULL, "UPSLOPE_AREA", _TL("Upslope Length Factor"),
		"This contains the output Upslope Area",
		PARAMETER_OUTPUT
	);



}

bool CCalculate_Uparea::On_Execute(void)
{
	return true;
}

CCalculate_Uparea::~CCalculate_Uparea()
{
	int x, y;
	double opp;
	//-----------------------------------------------------
	// Get parameter settings...

	m_pDEM = Parameters("DEM")->asGrid();
	m_pUp_Area = Parameters("UPSLOPE_AREA")->asGrid();
	

	//-----------------------------------------------------
	// Check for valid parameter settings...


	//-----------------------------------------------------
	// Execute calculation...

	//opm zie pascal CalculateUparea
	for (y = 0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for (x = 0; x<Get_NX(); x++)
		{
			opp = Get_Cellarea(); // eventueel min ptef


		}
	}


}


/*

Procedure DistributeTilDirEvent(i,j:integer;Var AREA:double;Var UPAREA:RRaster;Var FINISH:
                                GsmallintRaster;Var massbalance:double;Topo:boolean; Var Rivdat:
                                Trivdatarray);

Var 
  CSN,SN,MINIMUM,MINIMUM2,PART1,PART2 : extended;
  K1,K2,l1,L2,ROWMIN,COLMIN,ROWMIN2,COLMIN2,K,L,z : integer;
  pit1,pit2,parequal,closeriver : boolean;
  Direction,Abis : single;
  teller,vlag,rivvlag,v,w,id,STE : integer;
Begin
  closeriver := false;
  //nagaan of een buurcel een rivier is en deze een lagere hoogte heeft
  For K := -1 To 1 Do
    For L := -1 To 1 Do
      Begin
        If (PRC[i+k,j+l]=-1)Then
          Begin
            If (DTM[i+k,j+l]<DTM[i,j]) Then
              closeriver := true // end if
            Else closeriver := false;
          End;
      End;
  //end loop

  //wanneer er een buurcel is die rivier is dan...
  If closeriver Then
    Begin
      For K := -1 To 1 Do
        For L := -1 To 1 Do
          Begin
            If ((K=0) And(L=0)) Then
              continue;

            If (PRC[i+k,j+l]=-1) And(DTM[i+k,j+l]<DTM[i,j]) Then
              Begin
                ROWMIN := K;
                COLMIN := L;
                rivvlag := 1;
                Rivdat[rivvlag].latuparea := Rivdat[rivvlag].latuparea+AREA;
                AREA := 0;
                FINISH[i,j] := 1
              End;
            //end if
          End;
      //end loop
    End

    //indien er geen buurcel een rivier is dan ...
  Else
    Begin
      PART1 := 0.0;
      PART2 := 0.0;
      k1 := 0;
      l1 := 0;
      k2 := 0;
      l2 := 0;

      If (LogReg(i,j)<0.5)And(Not(Topo)) Then
        Begin
          Direction := degtorad(TilDir[i,j]);
          If SlopeDir(Direction,i,j,DTM)>0.0 Then
            Direction := Direction+Pi;
          //end if
          If direction>Pi*2.0 Then
            direction := direction-Pi*2.0;
          //end if
        End
      Else
        Direction := aspect[i,j];
      // end if

      CSN := (ABS(cos(Direction)))/(ABS(SIN(Direction))+ABS(COS(Direction)));
      SN := (ABS(sin(Direction)))/(ABS(SIN(Direction))+ABS(COS(Direction)));
      If (Direction >= 0.0) And (Direction <= (PI/2)) Then
        Begin
          PART1 := AREA*CSN;
          PART2 := AREA*SN;
          K1 := -1;
          L1 := 0 ;
          K2 := 0 ;
          L2 := 1 ;
        End
      Else
        Begin
          If (Direction > (PI/2)) And (Direction < PI) Then
            Begin
              PART1 := AREA*SN;
              PART2 := AREA*CSN;
              K1 := 0;
              L1 := 1;
              K2 := 1;
              L2 := 0;
            End
          Else
            Begin
              If (Direction >= PI)And (Direction<= (PI*1.5)) Then
                Begin
                  PART1 := AREA*CSN;
                  PART2 := AREA*SN;
                  K1 := 1;
                  L1 := 0;
                  K2 := 0;
                  L2 := -1;
                End
              Else
                Begin
                  If (Direction>(PI*1.5))Then
                    Begin
                      PART1 := AREA*SN;
                      PART2 := AREA*CSN;
                      K1 := 0;
                      L1 := -1;
                      K2 := -1;
                      L2 := 0;
                    End;
                  //end if (Direction>(PI*1.5))
                End;
              //end IF (Direction >= PI)AND (Direction<= (PI*1.5))
            End;
          //end IF (Direction > (PI/2)) AND (Direction < PI)
        End;
      //if Direction >= 0.0) AND (Direction <= (PI/2))

      pit1 := false;
      pit2 := false;
      If (PIT[i+K1,j+L1]<>0)Or(PIT[i+K2,j+L2]<>0)Or(Pondnr<>0)Or(Pondnr<>0) Then
        // if one of receiving cells is a pit
        Begin
          If (Pondnr<>0)Or(Pondnr<>0) Then
            Begin
              vlag := Pondnr;
              If vlag=0 Then vlag := Pondnr;
              STE := PondDat[vlag].ptef;
              UPAREA[PondDat[vlag].outr,PondDat[vlag].outc] := 
                                                               UPAREA[PondDat[vlag].outr,PondDat[
                                                               vlag].outc]+AREA*(1-(STE/100));
              PondDat[vlag].input := PondDat[vlag].input+AREA;
              FINISH[i,j] := 1;
            End
          Else
            Begin
              vlag := Pit[i+K1,j+L1];
              If vlag=0 Then
                vlag := Pit[i+k2,j+l2];

              v := Pitdat[vlag].outr;
              w := Pitdat[vlag].outc;
              If PRC[v,w]=-1 Then
                Begin
                  rivvlag := 1;
                  Rivdat[rivvlag].latinput := Rivdat[rivvlag].latinput+AREA;
                  PitDat[vlag].input := PitDat[vlag].input+AREA;
                  //AREA:=0;
                End
              Else
                Begin
                  UPAREA[PitDat[vlag].outr,PitDat[vlag].outc] := 
                                                                 UPAREA[PitDat[vlag].outr,PitDat[
                                                                 vlag].outc]+AREA;
                  PitDat[vlag].input := PitDat[vlag].input+AREA;
                End;
              FINISH[i,j] := 1;
            End;

        End
        // end if receiving cell is a pit
      Else  // wanneer er geen pits zijn dan...
        Begin
          If FINISH[i+K1,j+L1]=1 Then
          // check if receiving cell is already treated  (bv wanneer het een cel is afwaarts een pit
            Begin
              If FINISH[i+K2,j+L2]=1 Then
                Begin
                  part1 := 0.0;
                  //beide cellen zijn al behandeld en ontvangen dus niets meer
                  part2 := 0.0;
                End
              Else
                Begin
                  part2 := part2+part1;
                  //één cel is behandeld, de andere ontvangt alles
                  part1 := 0;
                End;
            End
          Else
            Begin
              If FINISH[i+K2,j+L2]=1 Then
                Begin
                  part1 := part1+part2;
                  //één cel is behandeld, de andere ontvangt alles
                  part2 := 0;
                End;
            End;
          //einde checken of cel al behandeld is

//nu volgt deel waarbij part1 en part2 worden ingevuld of niet naargelang de ontvangende cellen hoger liggen tot een ander perceel behoren
          If DTM[i+k1,j+l1]>DTM[i,j] Then
            Begin
              If DTM[i+k2,j+l2] > DTM[i,j] Then
                Begin
                  part1 := 0.0;
                  //twee ontvangende cellen liggen hoger dan huidige cel en krijgen dus niets meer
                  part2 := 0.0;
                End
              Else                 //slechts één cel is lagergelegen
                Begin
                  If PRC[i+k2,j+l2]<>PRC[i,j] Then
                    //indien deze ene cel tot een ander perceel behoort dan..
                    Begin
                      part1 := 0.0;
                      //...ontvangt deze cel niets
                      part2 := 0.0;
                    End
                  Else            //als ze echter tot hetzelfde perceel behoort dan...
                    Begin
                      PART2 := PART2+PART1;
                      //...ontvangt ze alles      (routing langsheen perceelsgrens)
                      PART1 := 0.0;
                    End;
                  //end if
                End;
              //end if PRC[i+k2,j+l2]<>PRC[i,j]
            End
          Else  //IF DTM[i+k1,j+l1]>DTM[i,j]..ELSE
            Begin
              If DTM[i+k2,j+l2]>DTM[i,j] Then
                Begin
                  If PRC[i+k1,j+l1] <> PRC[i,j] Then
                    Begin
                      part2 := 0.0;
                      part1 := 0.0;
                    End
                  Else
                    Begin
                      part1 := part1+part2;
                      part2 := 0.0;
                    End;
                End
              Else
                Begin
                  If PRC[i+k1,j+l1]<>PRC[i,j] Then
                    Begin
                      If PRC[i+k2,j+l2]<>PRC[i,j] Then
                        Begin
                          PART1 := 0.0;
                          PART2 := 0.0;
                        End
                      Else
                        Begin
                          PART2 := PART2+PART1;
                          PART1 := 0.0;
                        End;
                    End
                  Else
                    Begin
                      If PRC[i+k2,j+l2]<>PRC[i,j] Then
                        Begin
                          PART1 := PART1+PART2;
                          PART2 := 0.0;
                        End
                      Else
                        Begin
                        End;
                    End;
                End;
            End;
          //einde deel toewijzing part1 en part2

          If ((PART1=0.0)And(PART2=0.0)) Then
            // no receiving cells were found (lagergelegen of ander perceel)
            Begin
              PART1 := AREA;
              parequal := false;
              ROWMIN := 0;
              ROWMIN2 := 0;
              COLMIN := 0;
              COLMIN2 := 0;
              MINIMUM := 99999999.9;
              MINIMUM2 := 99999999.9;
              For K := -1 To 1 Do
                For L := -1 To 1 Do
                  Begin
                    If ((K=0)And(L=0)) Then CONTINUE;
                    If ((DTM[I+K,J+L]<MINIMUM)And(DTM[I+K,J+L]<DTM[I,J])
                       And(FINISH[I+K,J+L]=0)And(PRC[I+K,J+L]=PRC[I,J]))Then
                      Begin
                        MINIMUM := DTM[I+K,J+L];
                        ROWMIN := K;
                        COLMIN := L;
                        parequal := true;
                      End;
                    //second loop voor wanneer er geen lagere cel is in hetzelfde perceel
                    If ((DTM[I+K,J+L]<MINIMUM2)And(DTM[I+K,J+L]<DTM[I,J])
                       And(FINISH[I+K,J+L]=0))Then
                      Begin
                        MINIMUM2 := DTM[I+K,J+L];
                        ROWMIN2 := K;
                        COLMIN2 := L;
                      End;
                  End;
              If parequal Then    // If receiving cell is in same parcel
                Begin
                  pit1 := false;
                  pit2 := false;
                  If (PIT[i+ROWMIN,j+COLMIN]<>0)Or(Pondnr<>0) Then
                    // if one of receiving cells is a pit or pond
                    Begin
                      If (Pondnr<>0) Then
                        Begin
                          vlag := Pondnr;
                          STE := PondDat[vlag].PTEF;
                          UPAREA[PondDat[vlag].outr,PondDat[vlag].outc] := 
                                                                           UPAREA[PondDat[vlag].outr
                                                                           ,PondDat[vlag].outc]+AREA
                                                                           *(1-(STE/100));
                          PondDat[vlag].input := PondDat[vlag].input+AREA;
                          FINISH[i,j] := 1;
                        End
                      Else
                        Begin
                          vlag := Pit[i+ROWMIN,j+COLMIN];
                          v := Pitdat[vlag].outr;
                          w := Pitdat[vlag].outc;
                          If PRC[v,w]=-1 Then
                            Begin
                              rivvlag := 1;
                              Rivdat[rivvlag].latinput := Rivdat[rivvlag].latinput+AREA;
                              // PitDat[vlag].input:=PitDat[vlag].input+AREA;
                              AREA := 0;
                            End
                          Else
                            Begin
                              UPAREA[PitDat[vlag].outr,PitDat[vlag].outc] := 
                                                                             UPAREA[PitDat[vlag].
                                                                             outr,PitDat[vlag].outc]
                                                                             +AREA;
                              PitDat[vlag].input := PitDat[vlag].input+AREA;
                            End;
                          FINISH[i,j] := 1;
                        End;
                    End
                    // end if receiving cell is a pit or pond
                  Else// normal case, no pits
                    UPAREA[I+ROWMIN,J+COLMIN] := UPAREA[I+ROWMIN,J+COLMIN]+AREA;
                End
              Else           // if receiving cell belongs to a different parcel
                //IF ((ROWMIN<>0)OR(COLMIN<>0)) THEN
                Begin
                  pit1 := false;
                  pit2 := false;
                  If (PIT[i+ROWMIN2,j+COLMIN2]<>0)Or(Pondnr<>0) Then
                    // if one of receiving cells is a pit or pond
                    Begin
                      If (Pondnr<>0) Then
                        Begin
                          vlag := Pondnr;
                          UPAREA[PondDat[vlag].outr,PondDat[vlag].outc] := 
                                                                           UPAREA[PondDat[vlag].outr
                                                                           ,PondDat[vlag].outc]+AREA
                          ;
                          PondDat[vlag].input := PondDat[vlag].input+AREA;
                          FINISH[i,j] := 1;
                        End
                      Else
                        Begin
                          vlag := Pit[i+ROWMIN2,j+COLMIN2];
                          v := Pitdat[vlag].outr;
                          w := Pitdat[vlag].outc;
                          If PRC[v,w]=-1 Then
                            Begin
                              rivvlag := 1;
                              Rivdat[rivvlag].latinput := Rivdat[rivvlag].latinput+AREA;
                              //  PitDat[vlag].input:=PitDat[vlag].input+AREA;
                              AREA := 0;
                            End
                          Else
                            Begin
                              UPAREA[PitDat[vlag].outr,PitDat[vlag].outc] := 
                                                                             UPAREA[PitDat[vlag].
                                                                             outr,PitDat[vlag].outc]
                                                                             +AREA;
                              PitDat[vlag].input := PitDat[vlag].input+AREA;
                            End;
                          FINISH[i,j] := 1;
                        End;
                    End
                    // end if receiving cell is a pit or pond
                  Else// normal case, no pits, no ponds
                    Begin
                      If (PRC[i+ROWMIN2,j+COLMIN2] > 0) Then
                        If (PRC[i+ROWMIN2,j+COLMIN2] < 10000) Then
                          Abis := AREA * (100-TFCAtoCropLand)/100.0
                      Else
                        Abis := AREA * (100-TFCAtoForestOrPasture)/100.0;
                      UPAREA[I+ROWMIN2,J+COLMIN2] := UPAREA[I+ROWMIN2,J+COLMIN2]+Abis;
                      If PRC[I+ROWMIN2,J+COLMIN2]=9999 Then // case of sediment export
                        Begin
                          massbalance := massbalance+AREA;
                        End;
                      If (I+ROWMIN2<1) Or (I+ROWMIN2>nrow) Or (J+COLMIN2<1) Or (J+COLMIN2>ncol)Or(
                         PRC[i+rowmin2,j+colmin2]=0) Then
                        massbalance := massbalance+AREA;
                    End;

                End;

            End
            // end if no cells were found
          Else
            Begin
              // normal case part1 or part2 verschillen van 0

              If (I+k1<1) Or (I+k1>nrow) Or (J+l1<1) Or (J+l1>ncol)
                 Or (PRC[i+k1,j+l1]=0) Or (PRC[i+k1,j+l1]=9999) Then
                Begin
                  massbalance := massbalance+PART1;
                End;
              If (I+k2<1) Or (I+k2>nrow) Or (J+l2<1) Or (J+l2>ncol)
                 Or (PRC[i+k2,j+l2]=0) Or (PRC[i+k2,j+l2]=9999) Then
                Begin
                  massbalance := massbalance+PART2;
                End;

              UPAREA[i+k1,j+l1] := UPAREA[i+k1,j+l1]+PART1;
              UPAREA[i+k2,j+l2] := UPAREA[i+k2,j+l2]+PART2;
            End;

        End;
      // end normal case no pits
      FINISH[I,J] := 1;

    End;

End;
// end procedure DistributeTilDirEvent                            

*/