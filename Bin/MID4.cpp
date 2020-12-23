//
// Created by user on 09.10.2019.
//

#include "MID4.h"

#include "BinContainer.h"
using TNullCheckerMID4 = TNullChecker<unsigned short, offsetof(TDataMID4, Num2)>;

template<typename TValue, typename TKey>
    using TDataBinNullMID4 = TDataNull<TDataBin<TValue, TKey>, TNullCheckerMID4>;

template<typename TValue, typename TKey, size_t N, typename... TArgs>
    using TDataBinNNullMID4 = TDataNull<TDataBinN<TValue, TKey, N>, TNullCheckerMID4>;

template<typename TValue, typename TKey, size_t N>
    using TDataBinMarkChecker = TDataBinMarkNull<TValue, TKey, N, TNullCheckerMID4>;

HEADER_BIN(THeaderBinMID4, THeaderBin, TMID4Format,
VAR_LIST(
           INFO(iiArea, DosToUtf8(SafeCharString(h.Area)))
           INFO(iiWell, DosToUtf8(SafeCharString(h.Well)))
           INFO(iiDate, SafeCharString(h.Date))
           INFO(iiTime, SafeCharString(h.TimeBeg))
           INFO(iiBegin, h.DepthBeg)
           INFO(iiEnd, h.DepthEnd)
           INFO(iiCompany, DosToUtf8(SafeCharString(h.Customer)))
           INFO(iiServComp, DosToUtf8(SafeCharString(h.Operator)))
           ),
VAR_LIST(
       KEY_LINE(Depth, float);
       ADD(DOUBLE_LINE(GR, API, "GR", unsigned short))
            ->CLB(float, CoefGRExtA, CoefGRExtB, ucGR)->SetKeyDelta(3.945);

       ADD(DOUBLE_LINE(IM, mA, "Generator coil current medium electromagnetic sonde", short))
            ->CLB(float, CoefIA, CoefIB, ucCurrent)->SetKeyDelta(5.435);

       ADD(DOUBLE_LINE(IS, mA, "Generator coil current short electromagnetic sonde", short))
            ->CLB(float, CoefIA, CoefIB, ucCurrent)->SetKeyDelta(5.225);

       ADD(DOUBLE_LINE(TIn1, C, "Internal thermometer 1", short))
            ->CLB(float, CoefTInA, CoefTInB, ucTemp)->SetKeyDelta(5.225);//TODO check

       ADD(DOUBLE_LINE(Press, atm, "Pressure", short))
            ->CLB(float, CoefPressA, CoefPressB, ucPress)->SetKeyDelta(4.330);

       ADD(DOUBLE_LINE_ARRAY(ZM, ZM, raw, "Medium electromagnetic sonde decay", short, TDataBinMark, 42))
            ->SetCoef({1., 25., 250.})//коэффициенты как есть, потом автоматом пересчитывается
            ->SetKeyDelta(5.435);

       ADD(DOUBLE_LINE_ARRAY(ZS, ZS, raw, "Short electromagnetic sonde decay", short, TDataBinMark, 30))
            ->SetCoef({25, 250.})
            ->SetKeyDelta(5.225);

       ADD(DOUBLE_LINE_ARRAY(Scan1, Scan[0], raw, "Scan electromagnetic sonde 1 decay", short,
           TDataBinNNullMID4, CountScanData));

       ADD(DOUBLE_LINE_ARRAY(Scan2, Scan[1], raw, "Scan electromagnetic sonde 2 decay", short,
           TDataBinNNullMID4, CountScanData));

       ADD(DOUBLE_LINE_ARRAY(Scan3, Scan[2], raw, "Scan electromagnetic sonde 3 decay", short,
           TDataBinNNullMID4, CountScanData));

       ADD(DOUBLE_LINE_ARRAY(Scan4, Scan[3], raw, "Scan electromagnetic sonde 4 decay", short,
           TDataBinNNullMID4, CountScanData));

       ADD(DOUBLE_LINE_ARRAY(Scan5, Scan[4], raw, "Scan electromagnetic sonde 5 decay", short,
           TDataBinNNullMID4, CountScanData));

       ADD(DOUBLE_LINE_ARRAY(Scan6, Scan[5], raw, "Scan electromagnetic sonde 6 decay", short,
           TDataBinNNullMID4, CountScanData));

       ADD(DOUBLE_LINE_T(IL, mA, "Generator coil current long electromagnetic sonde",
             TDataBinNullMID4, short))
            ->CLB(float, CoefIA, CoefIB, ucCurrent)->SetKeyDelta(1.540);

       ADD(DOUBLE_LINE_T(TOut, C, "External thermometer", TDataBinNullMID4, short))
            ->CLB(float, CoefTOutA, CoefTOutB, ucTemp)->SetKeyDelta(2.645);

       ADD(DOUBLE_LINE_T(TIn2, C, "Internal thermometer 2", TDataBinNullMID4, short))
            ->CLB(float, CoefTInA, CoefTInB, ucTemp)->SetKeyDelta(1.540);//TODO check

       ADD(DOUBLE_LINE_ARRAY(ZL, ZL, raw, "Long electromagnetic sonde decay", short,
            TDataBinMarkChecker, 54))
           ->SetCoef({1., 25., 250.})
           ->SetKeyDelta(1.540);
       )
)
