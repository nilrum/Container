//
// Created by user on 30.11.2020.
//

#ifndef NEO_MIDHEADER_H
#define NEO_MIDHEADER_H

#include "BinFile.h"

template <typename THeader, typename TData>
class TMIDFileTemp : public TBinFileTemp<THeader, TData>{
public:
    TMIDFileTemp(const TString& v):TBinFileTemp<THeader, TData>(v){}
    TInspectInfo Inspect(const TString &path, bool isCheckFormat) override;
protected:
    using TBase = TBinFileTemp<THeader, TData>;
    virtual bool CheckHeader(const THeader& h)
    {
        return TBinFileTemp<THeader, TData>::CheckHeader(h) && h.LenRec == sizeof(TData);
    }
};

#define FIELD_MASK "Field \"%s\" must not be null. (Offset: %d bytes, Size: %d bytes)"

#define CHECK_ARRAY_NULL(FIELD)\
    if(h.FIELD[0] == 0)\
        info.AddWarrning(STDFORMAT(FIELD_MASK, #FIELD, offsetof(THeader, FIELD), std::size(h.FIELD)));

#define CHECK_VALUE_NULL(FIELD) \
    if(h.FIELD == 0)\
        info.AddWarrning(STDFORMAT(FIELD_MASK, #FIELD, offsetof(THeader, FIELD), sizeof(THeader::FIELD)));

#define CHECK_EQUAL_INT(FIELD, TRUE, MASK) \
    if(h.FIELD != TRUE) \
        info.AddError(STDFORMAT("Field \"%s\" must be: %d, error value: " MASK ". (Offset: %d bytes, Size: %d bytes)", #FIELD, TRUE, h.FIELD, offsetof(THeader, FIELD), sizeof(THeader::FIELD)));

template <typename THeader, typename TData>
TInspectInfo TMIDFileTemp<THeader, TData>::Inspect(const TString &path, bool isCheckFormat)
{
    TInspectInfo info("Inspect for file format: " + TBase::Version());
    auto res = TBase::LoadFile(path, isCheckFormat);
    if (res.IsError())
    {
        info.AddError(TResult::TextError(res));//ошибка открытия файла
        return info;
    }
    if(TBase::CheckVersion(path).IsNoError())
    {
        info.AddInfo("File can be loaded");
        info.SetStatus(true);
    }
    else
    {
        info.AddInfo("File can not be loaded");
        info.SetStatus(false);
    }
    auto& h = TBase::header;
    if(SafeCharString(h.EndAscHead) != TString("~ASC"))
        info.AddWarrning("Field 'EndAscHead' must be '~ASC'.");

    if(TBase::CheckTextVersionHeader(h) == false)
        info.AddError(STDFORMAT("Field 'Ver' must be: '%s', error value: '%s'", STR(TBase::Version()), STR(
                SafeCharString(h.Ver))));

    CHECK_ARRAY_NULL(Area)
    CHECK_ARRAY_NULL(Well)
    CHECK_ARRAY_NULL(Date)
    CHECK_ARRAY_NULL(TimeBeg)
    CHECK_ARRAY_NULL(Operator)

    CHECK_EQUAL_INT(LenRec, sizeof(TData), "%d")
    CHECK_EQUAL_INT(CountOK, TBase::CountData(), "%.0f")

    CHECK_VALUE_NULL(CoefIA)
    CHECK_VALUE_NULL(CoefTInA)
    CHECK_VALUE_NULL(CoefGR)
    CHECK_VALUE_NULL(CoefTNA)
    CHECK_VALUE_NULL(CoefTOutA)

    return info;
};
#endif //NEO_MIDHEADER_H
