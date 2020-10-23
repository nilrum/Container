//
// Created by user on 05.06.2020.
//

#include "LasContainer.h"
#include <fstream>
#include <cstring>


INIT_PROPERTYS(TDataLas)

namespace {
    const bool addHeader = TContainer::RegisterHeader(std::make_unique<THeaderLas>());
}

THeaderLas::THeaderLas()
{
    headerInfo.resize(iiCountInfo);
}

TString THeaderLas::Version() const
{
    return "LAS";
}

TResult THeaderLas::CheckFile(const TString &path)
{
    isCheckingFile = true;
    TResult res = Read(path);
    isCheckingFile = false;
    if(res.IsError())
        return res;
    if(lasVersion > 2.1)
        return TResultLas::NotSupportVersion;
    return TResult();
}

TVecData THeaderLas::LoadableData(const TString &path)
{
    TResult res = Read(path);
    if(res.IsError())
        return TVecData();
    loadablePath = path;
    return datas;
}

TResult THeaderLas::LoadData(const TVecData &loadable, const TPtrProgress& progress)
{
    if(loadablePath.empty())
        return TResult();
    for(size_t i = 0; i < datas.size(); i++)
    {
        bool isFound = false;
        for (size_t j = 0; j < loadable.size(); j++)//ищим загружаемая ли кривая
            if (datas[i] == loadable[j])
            {
                lasCurves[i + 1].resize(1);//изменяя размер говорим что будет грузится
                isFound = true;
                break;
            }
        if(isFound == false)
            lasCurves[i + 1].clear();//нулевой размер говорит что кривая не грузится
    }
    TResult res = ReadData(loadablePath, progress);
    if(res.IsError()) return res;
    depthVec->swap(lasCurves[0]);//меняем данные
    for(size_t  i = 0; i < datas.size(); i++)
        datas[i]->SwapValue(lasCurves[1 + i]);
    return TResult();
}

TPtrHeader THeaderLas::Clone()
{
    return std::make_shared<THeaderLas>();
}

void THeaderLas::AddWellInfo(const TVecString &info)
{
    TLasReader::AddWellInfo(info);
    int index = LasHeaderToInd(info[0]);//ищим по названию индекс для шапки
    SetTitleInfo(index, info[0]);
    if(index != iiStep)
        SetInfo(index, info[2]);
    else
        SetInfo(index, std::fabs(TVariable(info[2]).ToDouble()));
    if(index == iiBegin)//получаем ед измерения глубины
    {
        TString unit = ToLowerCase(info[1]);
        if(unit == ShortUnitDepth(duMeter))
            depthUnit = duMeter;
        else
            if(unit == ShortUnitDepth(duFoot))
                depthUnit = duFoot;
            else
                depthUnit = duNone;
    }
}

void THeaderLas::AddCurveInfo(const TVecString &info)
{
    if(isCheckingFile) return;//при проверке файла не добавляем кривые

    if(lasCurves.size())
        datas.emplace_back(std::make_shared<TDataLas>(info[0], info[1], info[2], depthVec));
    TLasReader::AddCurveInfo(info);
}

TString THeaderLas::TitleInfo(size_t  index) const
{
    if(index < iiCountInfo)
        return THeaderBase::TitleInfo(index);
    else
    {
        if(index - iiCountInfo < othTitle.size())
            return othTitle[index - iiCountInfo];
        return TString();
    }
}

void THeaderLas::SetTitleInfo(size_t  index, const TString &value)
{
    if(index >= iiCountInfo)
    {
        size_t newIndex = index - iiCountInfo;
        if(newIndex >= othTitle.size())
            othTitle.resize(newIndex + 1);
        othTitle[newIndex] = value;
    }
}

TVariable THeaderLas::Info(size_t  index) const
{
    if(index < headerInfo.size())
        return headerInfo[index];
    return TVariable();
}

void THeaderLas::SetInfo(size_t  index, const TVariable &value)
{
    if(index >= headerInfo.size())
        headerInfo.resize(index + 1);
    headerInfo[index] = value;
}

size_t THeaderLas::CountInfo() const
{
    return headerInfo.size();
}

int THeaderLas::LasHeaderToInd(const TString &value)
{
    for(size_t i = 0; i < LasHeaderNames().size(); i++)
        if(ToUpperCase(value) == LasHeaderNames()[i])
            return i;
    return headerInfo.size();//если не нашли возвращаем индекс на добавление
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
TDataLas::TDataLas(const TString &nameCurve, const TString &unitCurve, const TString& com, const TPtrDepthVector& depthVector)
{
    name = nameCurve;
    unit = unitCurve;
    comment = com;
    depth = depthVector;
}

double TDataLas::Key(size_t index) const
{
    return depth->at(index);
}

size_t TDataLas::SetKey(size_t index, double value, bool isSort, TTypeEdit typeEdit)
{
    return TDataBase::SetKey(index, value, isSort, typeEdit);
}

double TDataLas::Value(size_t index, int array) const
{
    return values[index];
}

void TDataLas::SetValue(size_t index, double value, int array, TTypeEdit typeEdit)
{
    values[index] = value;
}

size_t TDataLas::CountValue() const
{
    return values.size();
}

const double *TDataLas::PtrKey()
{
    return depth->data();
}

const double *TDataLas::PtrValue(int array)
{
    if(values.empty()) return nullptr;
    return values.data();
}

void TDataLas::SwapValue(TVecDouble &value)
{
    values.swap(value);
}

