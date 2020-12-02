//
// Created by user on 15.06.2020.
//

#include "MIDK05.h"
#include "BinContainer.h"
#include "Units.h"

HEADER_BIN(THeaderBinMIDK05, THeaderBin, TMIDK05Format,
VAR_LIST(
    INFO(iiArea, DosToUtf8(h.Area))
    INFO(iiWell, DosToUtf8(h.Well))
    INFO(iiDate, h.Date)
    INFO(iiTime, h.TimeBeg)
    INFO(iiBegin, h.DepthBeg)
    INFO(iiEnd, h.DepthEnd)
    INFO(iiStep, 0.0)
    INFO(iiServComp, DosToUtf8(h.Operator))
),
VAR_LIST(
    SET_INFO(iiArea,    Copy(h.Area, value))
    SET_INFO(iiWell,    Copy(h.Well, value))
    SET_INFO(iiDate,    Copy(h.Date, value))
    SET_INFO(iiTime,    Copy(h.TimeBeg, value))
    SET_INFO(iiBegin,   Copy(h.DepthBeg, value))
    SET_INFO(iiEnd,     Copy(h.DepthEnd, value))
    SET_INFO(iiServComp, Copy(h.Operator, value))
    ),
VAR_LIST(
    KEY_LINE(Depth, float);
    ADD(DOUBLE_LINE(GR, API, "", unsigned short))->CLB(float, CoefGRExtA, CoefGRExtB, ucGR)
        ->SetKeyDelta(0.945);

    ADD(DOUBLE_LINE(IM, mA, "", short))->CLB(float, CoefIA, CoefIB, ucCurrent)
        ->SetKeyDelta(1.870);

    ADD(DOUBLE_LINE(IL, mA, "", short))->CLB(float, CoefIA, CoefIB, ucCurrent)
        ->SetKeyDelta(2.290);

    ADD(DOUBLE_LINE(IS, mA, "", short))->CLB(float, CoefIA, CoefIB, ucCurrent)
        ->SetKeyDelta(1.670);

    ADD(DOUBLE_LINE(TOut, C, "", short))->CLB(float, CoefTOutA, CoefTOutB, ucTemp)
        ->SetKeyDelta(2.800);

    ADD(DOUBLE_LINE(TIn, C, "", short))->CLB(float, CoefTInA, CoefTInB, ucTemp)
        ->SetKeyDelta(1.679);

    ADD(DOUBLE_LINE(Press, atm, "", short))->CLB(float, CoefPressA, CoefPressB, ucPress)
        ->SetKeyDelta(1.330);

    ADD(DOUBLE_LINE_ARRAY(ZM, ZS1, raw, "", short, TDataBinOver, 54))
        ->SetCoef({{15, 250.}, {54, 10.}, {54, 1.}})
        ->SetKeyDelta(1.870);

    ADD(DOUBLE_LINE_ARRAY(ZL, ZL1, raw, "", short, TDataBinOver, 54))
        ->SetCoef({{30, 250.}, {54, 10.}, {54, 1.}})
        ->SetKeyDelta(2.290);

    ADD(DOUBLE_LINE_ARRAY(ZS, ZSm1, raw, "", short, TDataBinOver, 30))
        ->SetCoef({{30, 250.}, {30, 25.}})
        ->SetKeyDelta(1.670);
    )
)
