//
// Created by user on 20.04.2020.
//

#ifndef NEO_UNITS_H
#define NEO_UNITS_H

#include "PropertyClass.h"

//Категории калибровок используемые в приложении по умолчанию
ENUM(TUnitCategory, ucNone, ucDepth, ucSpeed, ucTension, ucGR, ucTemp, ucCurrent, ucPress, ucDiameter, ucThick);

ENUM(TDepthUnit   , duNone, duMeter, duFoot);
ENUM(TSpeedUnit   , suNone, suInHour, suInMinute);
ENUM(TTensionUnit , tnuNone, tnuKN, tnuLbf, tnuKgf);
ENUM(TGRUnit      , guNone, guMcRH, guAPI);
ENUM(TTempUnit    , tuNone, tuC, tuF);
ENUM(TCurrentUnit , cuNone, cuMA);
ENUM(TPressUnit   , puNone, puAtm, puMPa, puPsi);
ENUM(TWidthUnit   , wuNone, wuMM, wuInch);

//TString EnumNameFromCategory(TUnitCategory value);
//TString EnumNameFromCategory(int value);

//хранит информацию о текущих единицах измерения по умолчанию
class TUnitProfile : public TPropertyClass{
public:
    TUnitProfile(){ name = "Russian"; };

    TEnum FromCategory(TUnitCategory cat);     //возврашает значение property от категории,
                                                // например cat == ucDepth -> либо 1 либо 2
    TString FullName(TUnitCategory cat);     //возвращает в строковом представлении cat == ucDepth -> ucMeter or ucFoot
    TString ShortName(TUnitCategory cat);    //возвращает в строковом представлении короткий вариант cat == ucDepth -> ucMeter_s or ucFoot_s

    void SetFromCategory(TUnitCategory cat, int value);

    PROPERTIES(TUnitProfile, TPropertyClass,
        PROPERTY(TDepthUnit, depth, Depth, SetDepth);
        PROPERTY(TSpeedUnit, speed, Speed, SetSpeed);
        PROPERTY(TTensionUnit, tension, Tension, SetTension);
        PROPERTY(TGRUnit, gr, GR, SetGR);
        PROPERTY(TTempUnit, temp, Temp, SetTemp);
        PROPERTY(TCurrentUnit, current, Current, SetCurrent);
        PROPERTY(TPressUnit, press, Press, SetPress);
        PROPERTY(TWidthUnit, diameter, Diameter, SetDiameter);
        PROPERTY(TWidthUnit, thickness, Thickness, SetThickness);
        PROPERTY(double, defaultStep, DefaultStep, SetDefaultStep);
    );

    PROPERTY_FUN(TDepthUnit, depth, Depth, SetDepth);
    PROPERTY_FUN(TSpeedUnit, speed, Speed, SetSpeed);
    PROPERTY_FUN(TTensionUnit, tension, Tension, SetTension);
    PROPERTY_FUN(TGRUnit, gr, GR, SetGR);
    PROPERTY_FUN(TTempUnit, temp, Temp, SetTemp);
    PROPERTY_FUN(TCurrentUnit, current, Current, SetCurrent);
    PROPERTY_FUN(TPressUnit, press, Press, SetPress);
    PROPERTY_FUN(TWidthUnit, diameter, Diameter, SetDiameter);
    PROPERTY_FUN(TWidthUnit, thickness, Thickness, SetThickness);
    PROPERTY_FUN(double, defaultStep, DefaultStep, SetDefaultStep);
private:
    TDepthUnit depth = TDepthUnit::duMeter;
    TSpeedUnit speed = TSpeedUnit::suInHour;
    TTensionUnit tension = TTensionUnit::tnuKN;
    TGRUnit gr = TGRUnit::guMcRH;
    TTempUnit temp = TTempUnit::tuC;
    TCurrentUnit current = TCurrentUnit::cuMA;
    TPressUnit press = TPressUnit::puAtm;
    TWidthUnit diameter = TWidthUnit::wuMM;
    TWidthUnit thickness = TWidthUnit::wuMM;

    double defaultStep = 0.02;
};

using TPtrUnitProfile = std::shared_ptr<TUnitProfile>;

class TUnitCustoms : public TPropertyClass{
public:
    TUnitCustoms(){ name = "UnitCustoms"; profiles.emplace_back(std::make_shared<TUnitProfile>()); }
    PROPERTIES(TUnitCustoms, TPropertyClass,
        PROPERTY_ARRAY(TUnitProfile, profils, CountProfiles, Profile, AddProfile, DelProfile);
        PROPERTY(int, indProfile, IndProfile, SetIndProfile);
    )
    PROPERTY_ARRAY_FUN(TPtrUnitProfile, profiles, CountProfiles, Profile, AddProfile, DelProfile);
    PROPERTY_FUN(int, indProfile, IndProfile, SetIndProfile);

    const TPtrUnitProfile& CurProfile() const { return profiles[indProfile]; }
private:
    int indProfile = 0;
    std::vector<TPtrUnitProfile> profiles;
};

using TPtrUnitCustoms = std::shared_ptr<TUnitCustoms>;

class TUnits : public TUnitProfile{
public:
    TUnits();
    ~TUnits();

    void SetNone();//устанавливает не выбранные значения
    void SetDefault(const std::map<TUnitCategory, int>& set);//применяет настройки по умолчанию или переданное значение

    STATIC_ARG(TUnits*, Single, nullptr);                                 //текущие ед измерения
    STATIC_ARG(TPtrUnitCustoms, Customs, std::make_shared<TUnitCustoms>());//настройки по умолчанию для ед измерения
    PROPERTIES(TUnits, TUnitProfile, );
};

using TPtrUnits = std::shared_ptr<TUnits>;

TString FullUnitDepth(TDepthUnit value);
TString ShortUnitDepth(TDepthUnit value);

template<typename  T>
TString FullUnit(T value)
{
    return TEnum(value).Name();
}

template<typename T>
TString ShortUnit(T value)
{
    return FullUnit(value) + "_s";
}

double ConvertUnit(TUnitCategory cat, int from, int to, double value);

//Depth
double MeterToFoot(double value);
double FootToMeter(double value);

double ConvertDepth(TDepthUnit from, TDepthUnit to, double value);
double ConvertToCur(TDepthUnit from, double value);
double ConvertCoef(TDepthUnit from, TDepthUnit to);

//Diameter, Thickness
double MmToInch(double value);
double InchToMm(double value);

double ConvertWidth(TWidthUnit from, TWidthUnit to, double value);

#define UNIT TUnits::Single()

#endif //NEO_UNITS_H
