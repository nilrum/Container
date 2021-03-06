//
// Created by user on 20.04.2020.
//

#include "Units.h"
#include "GlobalCustom.h"

INIT_PROPERTYS(TUnits)
INIT_PROPERTYS(TUnitProfile)
INIT_PROPERTYS(TUnitCustoms)

REGISTER_CUSTOM(TUnits::Customs())

TUnits::TUnits()
{
    name = "Units";

    if(Single() == nullptr)
        Single() = this;
}

TUnits::~TUnits()
{
    if(Single() == this)
        Single() = nullptr;
}

void TUnits::SetNone()
{
    int count = TEnum(ucDepth).Names().size();
    for(int i = ucDepth; i <= count; i++)
        WriteProperty(i, 0);
}

void TUnits::SetDefault(const TMapCategory& set)
{//делаем значения не выбранными по умолчанию
    const TPtrUnitProfile& def = Customs()->CurProfile();
    int count = TEnum(ucDepth).Names().size();
    for(int i = ucDepth; i <= count; i++)
    {
        if(ReadProperty(i).ToInt() == 0)
        {
            auto it = set.find(TUnitCategory(i));
            if(it == set.end())
                WriteProperty(i, def->ReadProperty(i));
            else
                WriteProperty(i, it->second);
        }
    }
}

TString EnumNameFromCategory(TUnitCategory value)
{
    switch (value)
    {
        case TUnitCategory::ucDepth:      return TypeEnum<TDepthUnit>();
        case TUnitCategory::ucSpeed:      return TypeEnum<TSpeedUnit>();
        case TUnitCategory::ucTension:    return TypeEnum<TTensionUnit>();
        case TUnitCategory::ucGR:         return TypeEnum<TGRUnit>();
        case TUnitCategory::ucTemp:       return TypeEnum<TTempUnit>();
        case TUnitCategory::ucCurrent:    return TypeEnum<TCurrentUnit>();
        case TUnitCategory::ucPress:      return TypeEnum<TPressUnit>();
        default: return TString();
    }
}
#define PRESTR(VALUE) #VALUE

TString FullUnitDepth(TDepthUnit value)
{
    if(value == duMeter)
        return PRESTR(duMeter);
    else
        return PRESTR(duFoot);
}

TString ShortUnitDepth(TDepthUnit value)
{
    return FullUnitDepth(value) + "_s";
}

TString FullUnitDepth()
{
    return FullUnitDepth(UNIT->Depth());
}

TString ShortUnitDepth()
{
    return ShortUnitDepth(UNIT->Depth());
}


TEnum TUnitProfile::FromCategory(TUnitCategory cat)
{
    return ReadProperty(cat).GetEnum();//пропускаем property Name
}

TString TUnitProfile::FullName(TUnitCategory cat)
{
    return FromCategory(cat).Name();
}

TString TUnitProfile::ShortName(TUnitCategory cat)
{
    return FullName(cat) + "_s";
}

void TUnitProfile::SetFromCategory(TUnitCategory cat, int value)
{
    WriteProperty(cat , value);
}

double MeterToFoot(double value)
{
    return value * 3.28084;
}

double FootToMeter(double value)
{
    return value / 3.28084;
}

double ConvertDepth(TDepthUnit from, TDepthUnit to, double value)
{
    if(from == to) return value;
    else if(from == duMeter) return MeterToFoot(value);
    return FootToMeter(value);
}

double ConvertToCur(TDepthUnit from, double value)
{
    if(UNIT->Depth() == duNone || from == duNone) return value;
    return ConvertDepth(from, UNIT->Depth(), value);
}

double ConvertCoef(TDepthUnit from, TDepthUnit to)
{
    if(from == duNone || to == duNone || from == to) return 1.;
    if(from == duMeter) return 3.28084;
    else return 1. / 3.28084;
}

double MmToInch(double value)
{
    return value / 25.4;
}

double InchToMm(double value)
{
    return value * 25.4;
}

double ConvertWidth(TWidthUnit from, TWidthUnit to, double value)
{
    if(from == to) return value;
    if(from == wuMM) return MmToInch(value);
    else InchToMm(value);
    return 0;
}

double ConvertUnit(TUnitCategory cat, int from, int to, double value)
{
    if(from == to || cat == 0) return value;
    switch(cat)
    {
        case ucDepth:       return ConvertDepth(TDepthUnit(from), TDepthUnit(to), value);
        case ucDiameter:
        case ucThick:       return ConvertWidth(TWidthUnit(from), TWidthUnit(to), value);
        default:            return value;
    }
}


