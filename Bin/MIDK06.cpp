//
// Created by user on 30.11.2020.
//

#include "MIDK06.h"

#include "BinContainer.h"
#include "Units.h"

template <typename TValue, typename TKey, size_t N>
struct TDataBinMerged : public TDataBinN<TValue, TKey, N> {
public:
    TDataBinMerged(const TString &n, const TString &u, const TString &c, size_t offsetValue):
        TDataBinN<TValue, TKey, N>(n, u, c, offsetValue){}

    auto SetIndMerged(size_t value){ indInMerged = value; return this; }

    double Value(size_t index, int array) const override
    {
        TValue* ptrMerged = TDataBinN<TValue, TKey, N>::ValImpl(index);
        return ptrMerged[array * Count_MIDK_06_Zond + indInMerged];
    }

private:
    size_t indInMerged = 0;
};

template <typename TValue, typename TKey, size_t N>
struct TDataBinMergedOver : public TDataBinN<TValue, TKey, N>  {
public:
    TDataBinMergedOver(const TString &n, const TString &u, const TString &c, size_t offsetValue):
            TDataBinN<TValue, TKey, N>(n, u, c, offsetValue){}

    auto SetCoef(const TCountCoef& arrayCoef)
    {
        coef = arrayCoef;
        return this;
    }

    double Value(size_t index, int array) const override
    {
        if(index != cashIndex) CalcCash(index);
        return cashValue[array];
    }

private:
    mutable int cashIndex = -1;
    mutable double cashValue[N];
    TCountCoef coef;//количество данных для коэффициента и сам коэффициент
    void CalcCash(int index) const
    {
        cashIndex = index;
        auto coefIt = coef.rbegin();

        TValue* ptrMerged = TDataBinN<TValue, TKey, N>::ValImpl(index);

        double kus = std::get<1>(coef.back());

        for(int i = N - 1; i >= 0; i--)
        {
            TValue v = ptrMerged[i * Count_MIDK_06_Zond + std::get<0>(*coefIt)];
            if(v >= 32767 && coefIt != (coef.rend() - 1))
            {
                coefIt++;
                kus = std::get<1>(*coefIt);
                v = ptrMerged[i * Count_MIDK_06_Zond + std::get<0>(*coefIt)];
            }
            cashValue[i] = v * kus;
        }
    }
};

HEADER_BIN(THeaderBinMIDK06, THeaderBin, TMIDK06Format,
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
                   ADD(DOUBLE_LINE(GR, API, "GR", unsigned short))
                           ->CLB(float, CoefGRExtA, CoefGRExtB, ucGR)->SetKeyDelta(0.9);

                   ADD(DOUBLE_LINE(IGenKat, mA, "", short))
                           ->CLB(float, CoefIA, CoefIB, ucCurrent)->SetKeyDelta(1.948);

                   ADD(DOUBLE_LINE(TOut, C, "", short))
                           ->CLB(float, CoefTOutA, CoefTOutB, ucTemp)->SetKeyDelta(2.845);

                   ADD(DOUBLE_LINE(TIn, C, "", short))
                           ->CLB(float, CoefTInA, CoefTInB, ucTemp)->SetKeyDelta(1.620);

                   ADD(DOUBLE_LINE_ARRAY(ZL, Zonds, "raw", "", short, TDataBinMergedOver, 54))
                           ->SetCoef({{0, 250.}, {1, 10.}, {2, 1.}});

                   ADD(DOUBLE_LINE_ARRAY(TX, Zonds, "raw", "", short, TDataBinMerged, 54))
                        ->SetIndMerged(3);
                   ADD(DOUBLE_LINE_ARRAY(TY, Zonds, "raw", "", short, TDataBinMerged, 54))
                           ->SetIndMerged(3);
           )
)