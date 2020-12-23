//
// Created by user on 30.11.2020.
//

#include "MIDK07.h"

#include "BinContainer.h"
#include "Units.h"

HEADER_BIN(THeaderBinMIDK07, THeaderBin, TMIDK07Format,
           VAR_LIST(
                   INFO(iiArea, DosToUtf8(h.Area))
                   INFO(iiWell, DosToUtf8(h.Well))
                   INFO(iiDate, h.Date)
                   INFO(iiTime, h.TimeBeg)
                   INFO(iiBegin, h.DepthBeg)
                   INFO(iiEnd, h.DepthEnd)
                   INFO(iiServComp, DosToUtf8(h.Operator))
           ),
           VAR_LIST(
                   KEY_LINE(Depth, float);
                   ADD(DOUBLE_LINE(GR, API, "GR", unsigned short))
                           ->CLB(float, CoefGRExtA, CoefGRExtB, ucGR)->SetKeyDelta(0.9);

                   ADD(DOUBLE_LINE(IGenKat, mA, "", short))
                           ->CLB(float, CoefIA, CoefIB, ucCurrent)->SetKeyDelta(1.948);

                   ADD(DOUBLE_LINE(TOut, C, "", short))
                           ->CLB(float, CoefTOutA, CoefTOutB, ucTemp)->SetKeyDelta(2.845);

                   ADD(DOUBLE_LINE(TIn, C, "", short))
                           ->CLB(float, CoefTInA, CoefTInB, ucTemp)->SetKeyDelta(1.620);

                   ADD(DOUBLE_LINE_ARRAY(TX, TX, raw, "", short, TDataBinN, 15))
                           ->SetKeyDelta(1.948);

                   ADD(DOUBLE_LINE_ARRAY(TY, TY, raw, "", short, TDataBinN, 15))
                            ->SetKeyDelta(1.948);

                   ADD(DOUBLE_LINE_ARRAY(ZL, Z1, raw, "", short, TDataBinOver, 54))
                            ->SetCoef({{15, 250.}, {30, 10.}, {54, 1.}})
                            ->SetKeyDelta(2.340));

           )
