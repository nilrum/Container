//
// Created by user on 27.02.2020.
//

#include "MIDK04.h"

#include "BinContainer.h"
#include "Units.h"

HEADER_BIN(THeaderBinMIDK04, THeaderBin, TMIDK04Format,
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
    KEY_LINE(Depth, float, TMIDK04Format);
    Add(res, DOUBLE_LINE(GR, API, unsigned short))->CLB(float, KoefGRExtA, KoefGRExtB, ucGR)->SetKeyDelta(0.9);
    Add(res, DOUBLE_LINE(IS, mA, short))->CLB(float, KoefTokA, KoefTokB, ucCurrent)->SetKeyDelta(1.948);
    Add(res, DOUBLE_LINE(IL, mA, short))->CLB(float, KoefTokA, KoefTokB, ucCurrent)->SetKeyDelta(2.340);
    Add(res, DOUBLE_LINE(TOut, C, short))->CLB(float, KoefTOutA, KoefTOutB, ucTemp)->SetKeyDelta(2.845);
    Add(res, DOUBLE_LINE(TIn, C, short))->CLB(float, KoefTinA, KoefTinB, ucTemp)->SetKeyDelta(1.620);
    Add(res, DOUBLE_LINE(Press, atm, short))->CLB(float, KoefManA, KoefManB, ucPress)->SetKeyDelta(1.330);
    Add(res, DOUBLE_LINE_ARRAY(ZS, ZS1, raw, short, TDataBinOver, 54, COUNTCOEF({15, 250.}, {54, 10.}, {54, 1.})))->SetKeyDelta(1.948);
    Add(res, DOUBLE_LINE_ARRAY(ZL, ZL1, raw, short, TDataBinOver, 54, COUNTCOEF({15, 250.}, {54, 10.}, {54, 1.})))->SetKeyDelta(2.340);
    )
)
