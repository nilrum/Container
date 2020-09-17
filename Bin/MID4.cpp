//
// Created by user on 09.10.2019.
//

#include "MID4.h"

#include "BinContainer.h"

using TNullCheckerMID4 = TNullChecker<unsigned short, offsetof(TDataMID4, Num2)>;

template<typename TValue, typename TKey>
    using TDataBinChecker = TDataBinNull<TValue, TKey, TNullCheckerMID4>;
template<typename TValue, typename TKey, size_t N>
    using TDataBinMarkChecker = TDataBinMarkNull<TValue, TKey, N, TNullCheckerMID4>;

HEADER_BIN(THeaderBinMID4, THeaderBin, TMID4Format,
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
                   KEY_LINE(Depth, float, TMID4Format);
                   Add(res, DOUBLE_LINE(GR, API, unsigned short))->CLB(float, CoefGRExtA, CoefGRExtB, ucGR)->SetKeyDelta(3.945);
                   Add(res, DOUBLE_LINE(IM, mA, short))->CLB(float, CoefIA, CoefIB, ucCurrent)->SetKeyDelta(5.435);
                   Add(res, DOUBLE_LINE(IS, mA, short))->CLB(float, CoefIA, CoefIB, ucCurrent)->SetKeyDelta(5.225);
                   Add(res, DOUBLE_LINE(TIn1, C, short))->CLB(float, CoefTInA, CoefTInB, ucTemp)->SetKeyDelta(5.225);//TODO check
                   Add(res, DOUBLE_LINE(Press, atm, short))->CLB(float, CoefPressA, CoefPressB, ucPress)->SetKeyDelta(4.330);
                   Add(res, DOUBLE_LINE_ARRAY(ZM, ZM, raw, short, TDataBinMark, 42, COEF(1., 25., 250.)))->SetKeyDelta(5.435);//коэффициенты как есть, потом автоматом пересчитывается
                   Add(res, DOUBLE_LINE_ARRAY(ZS, ZS, raw, short, TDataBinMark, 30, COEF(25, 250.)))->SetKeyDelta(5.225);

                   Add(res, DOUBLE_LINE_T(IL, mA, TDataBinChecker, short))->CLB(float, CoefIA, CoefIB, ucCurrent)->SetKeyDelta(1.540);
                   Add(res, DOUBLE_LINE_T(TOut, C, TDataBinChecker, short))->CLB(float, CoefTOutA, CoefTOutB, ucTemp)->SetKeyDelta(2.645);
                   Add(res, DOUBLE_LINE_T(TIn2, C, TDataBinChecker, short))->CLB(float, CoefTInA, CoefTInB, ucTemp)->SetKeyDelta(1.540);//TODO check
                   Add(res, DOUBLE_LINE_ARRAY(ZL, ZL, raw, short, TDataBinMarkChecker, 54, COEF(1., 25., 250.)))->SetKeyDelta(1.540);
                   )
)

