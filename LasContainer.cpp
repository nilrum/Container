//
// Created by user on 05.06.2020.
//

#include "LasContainer.h"
#include <fstream>

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

TResult THeaderLas::LoadData(const TVecData &loadable)
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
    TResult res = ReadData(loadablePath);
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
    TLas::AddWellInfo(info);
    int index = LasHeaderToInd(info[0]);//ищим по названию индекс для шапки
    SetTitleInfo(index, info[0]);
    SetInfo(index, info[2]);
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
        datas.emplace_back(std::make_shared<TDataLas>(info[0], info[1], depthVec));
    TLas::AddCurveInfo(info);
}

TString THeaderLas::TitleInfo(int index) const
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

void THeaderLas::SetTitleInfo(int index, const TString &value)
{
    if(index >= iiCountInfo)
    {
        int newIndex = index - iiCountInfo;
        if(newIndex >= othTitle.size())
            othTitle.resize(newIndex + 1);
        othTitle[newIndex] = value;
    }
}

TVariable THeaderLas::Info(int index) const
{
    if(index < headerInfo.size())
        return headerInfo[index];
    return TVariable();
}

void THeaderLas::SetInfo(int index, const TVariable &value)
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

TVecString THeaderLas::InitLasHeaderNames()
{
    //iiArea = 0, iiWell, iiDate, iiTime, iiBegin, iiEnd, iiStep, iiCompany, iiServComp
    return {"FLD", "WELL", "DATE", "", "STRT", "STOP", "STEP", "COMP", "SRVC"};
}


//----------------------------------------------------------------------------------------------------------------------
TLas::TLas():
    readInfos{
            {false, "~V", [this](std::ifstream& stream, TString& line) { return ReadVersion(stream, line); }},
            {false, "~W", [this](std::ifstream& stream, TString& line) { return ReadWell(stream, line); }},
            {false, "~C", [this](std::ifstream& stream, TString& line) { return ReadCurves(stream, line); }},
            {false, "~P", [this](std::ifstream& stream, TString& line) { return ReadParams(stream, line); }},
            {false, "~O", [this](std::ifstream& stream, TString& line) { return ReadOther(stream, line); }},
    }
{

}

void TLas::Clear()
{
    for(auto& info : readInfos)
        info.isReaded = false;//секция не прочитана
    lasCurves.clear();
    offsetAsciiData = 0;
}

TResult TLas::Read(const TString &path, bool andValues)
{
    Clear();
    std::ifstream stream(path);
    if(stream.is_open() == false) return TResultLas::FileNotOpen;


    TString line = ReadLine(stream);
    if(line.empty()) return TResultLas::EmptyFile;

    while(stream.eof() == false)            //пока файл не закончится
    {
        bool isRead = false;
        for(auto& info : readInfos)
        {
            if(line.empty()) break;
            if(info.isReaded == false && line.find(info.id) != -1)
            {
                TResult res = info.fun(stream, line);
                if(res.IsError()) return res;
                isRead = info.isReaded = true;
                break;
            }
        }
        if(isRead == false) break;//если не одна секция не прочитала выходим на проверку
    }

    //проверка что все секции были прочитаны
    for(auto info : readInfos)
        if(info.isReaded == false) return TResultLas::LasSecNotFound;
    if(andValues)
        return ReadAscii(stream, line);
    return TResult();
}

TResult TLas::ReadData(const TString& path)
{
    std::ifstream stream(path);
    if(stream.is_open() == false) return TResultLas::FileNotOpen;
    stream.seekg(offsetAsciiData);
    TString line = ReadLine(stream);
    return ReadAscii(stream, line);
}

TString TLas::ReadLine(std::ifstream &stream)
{
    TString res;
    while(stream.eof() == false)
    {
        offsetAsciiData = stream.tellg();
        std::getline(stream, res);
        if(res.empty() == false && res[0] != '#') return res;
    }
    return res;
}

TResult TLas::ReadVersion(std::ifstream& stream, TString& line)
{
    line = ReadLine(stream);//пропускаем ~V
    if(line.empty()) return TResultLas::BadSintax;

    TParser pars(line, true);
    if(pars.IsId("VERS") == false) return TResultLas::BadSintax;
    if(pars.IsChar('.') == false) return TResultLas::BadSintax;
    if(pars.IsNumber(lasVersion) == false) return TResultLas::BadSintax;

    line = ReadLine(stream);
    if(line.empty()) return TResultLas::BadSintax;

    pars.Set(line, true);
    if(pars.IsId("WRAP") == false) return TResultLas::BadSintax;
    if(pars.IsChar('.') == false) return TResultLas::BadSintax;
    TString wrap;
    if(pars.ReadId(wrap) == false) return TResultLas::BadSintax;
    isWrap = wrap == "YES";

    line = ReadLine(stream);
    return TResult();
}

TResult TLas::ReadWell(std::ifstream& stream, TString& line)
{
    line = ReadLine(stream);//пропускаем ~W
    if(line.empty()) return TResultLas::BadSintax;

    TVecString params(4);
    while(line.empty() == false && line[0] != '~')
    {
        TResult res = ReadParam(line, params, true);
        if(res.IsError()) return res;
        AddWellInfo(params);
        line = ReadLine(stream);
    }

    return TResult();
}

TResult TLas::ReadCurves(std::ifstream& stream, TString& line)
{
    line = ReadLine(stream);//пропускаем ~C
    if(line.empty()) return TResultLas::BadSintax;

    TVecString params(3);
    while(line.empty() == false && line[0] != '~')
    {
        TResult res = ReadParam(line, params, false);
        if(res.IsError()) return res;
        AddCurveInfo(params);
        line = ReadLine(stream);
    }

    return TResult();
}

TResult TLas::ReadParams(std::ifstream& stream, TString& line)
{
    do
    {
        line = ReadLine(stream);//пропускаем ~P
    } while(line.empty() == false && line[0] != '~');
    return TResult();
}

TResult TLas::ReadOther(std::ifstream& stream, TString& line)
{
    do
    {
        line = ReadLine(stream);//пропускаем ~O
    } while(line.empty() == false && line[0] != '~');
    return TResult();
}

TResult TLas::ReadParam(const TString &line, TVecString& params, bool readVal)
{
    TParser pars(line.data());
    if(pars.ReadUntil('.', params[0]) == false) return TResultLas::BadSintax;//читаем все до точки - это название
    params[0] = Trim(params[0]);
    if(pars.IsChar('.') == false) return TResultLas::BadSintax;
    pars.ReadUntil(' ', params[1]);//читаем после точки до пробела это может быть ед изм
    if(readVal)
        pars.ReadUntil(':', params[2]);//читаем после ед изм до двоеточия это может быть значение
    if(pars.IsChar(':') == false) return TResultLas::BadSintax;
    params[readVal ? 3 : 2] = pars.Last();//остаток это коментарий
    return TResult();
}

TResult TLas::ReadAscii(std::ifstream &stream, TString &line)
{
    if(line[0] == '~')
        line = ReadLine(stream);//пропускаем ~A
    if(line.empty()) return TResultLas::BadSintax;
    if(isWrap)
        return ReadAsciiWrap(stream, line);
    else
        return ReadAsciiNoWrap(stream, line);
}

TResult TLas::ReadAsciiWrap(std::ifstream &stream, TString &line)
{
    return TResult();
}

TResult TLas::ReadAsciiNoWrap(std::ifstream &stream, TString &line)
{
    TParser pars(line.data());

    while(true)
    {
        for(auto& d : lasCurves)
        {
            if(d.empty())
            {
                if (pars.IgnoreNumber() == 0) return TResultLas::ErrCountReadData;
            }
            else
            {
                if (pars.IsNumber(d.back()) == false) return TResultLas::ErrCountReadData;//нехватило данных для считывания
                if(d.back() == nullVal) d.back() = NAN;
            }
        }
        line = ReadLine(stream);
        if(line.empty())
            break;
        else
        {
            for (auto &d : lasCurves)
                if (d.empty() == false) d.emplace_back(0);
            pars.Set(line.data());
        }
    }
    return TResult();
}

void TLas::AddWellInfo(const TVecString &info)
{
    if(ToUpperCase(info[0]) == "NULL")
    {
        try{
            nullVal = std::stod(info[2]);
        }
        catch (std::invalid_argument){};
    }
}

void TLas::AddCurveInfo(const TVecString &info)
{
    lasCurves.push_back(TVecDouble(1));//по умолчанию добавляем одно поле на считывание
}

//----------------------------------------------------------------------------------------------------------------------
TParser::TParser(const std::string &t, bool isUpper)
{
    Set(t, isUpper);
}

void TParser::Set(const std::string &t, bool isUpper)
{

    if(isUpper)
        buffer = ToUpperCase(t);
    else
        buffer = t;
    Set(buffer.data());
}

bool TParser::IsId(const char* id)
{
    IsSpace();
    char const* it = id;
    while(*it != 0)
    {
        if(*text == 0) return false;
        if(*text != *it) return false;
        text++;
        it++;
    }
    return true;
}

bool TParser::ReadId(TString &value)
{
    IsSpace();
    const char* beg = text;
    if(isalpha(*text))
    {
        do{
            text++;
        }
        while(*text && isalnum(*text));
        value = TString(beg, text - beg);
        return true;
    }
    return false;
}


bool TParser::IsSpace()
{
    const char* beg = text;
    while(*text)
    {
        if(std::isspace(*text)) text++;
        else break;
    }
    return text != beg;//был ли найден пробельный символ
}

bool TParser::IsChar(int value)
{
    IsSpace();
    if(*text && *text == value)
    {
        text++;
        return true;
    }
    text++;
    return false;
}

bool TParser::IsNumber(double &val)
{
    int count = IgnoreNumber();
    if(count > 0)
    {
        const char* beg = text - count;
        if(count < 3)
        {
            if(count == 1 && std::isdigit(*beg) == false) return false;
            if(count == 2 && std::isdigit(*(beg + 1)) == false) return false;
        }
        val = std::stod(TString(beg, text - beg));
        return true;
    }
    return false;
}

int TParser::IgnoreNumber()
{
    IsSpace();
    const char* beg = text;
    if(*text && (*text == '+' || *text == '-')) text++;
    bool bePoint = false;
    while(*text && (std::isdigit(*text) || *text == '.'))
    {
        if(*text == '.')
        {
            if(bePoint) break;
            else bePoint = true;
        }
        text++;
    }
    return text - beg;
}

bool TParser::ReadUntil(int stop, TString &value)
{
    if(std::isspace(stop) == false) IsSpace();
    const char* beg = text;
    while(*text && *text != stop)
    {
        text++;
    }
    if(beg != text)
    {
        value = TString(beg, text - beg);
        return true;
    }
    value.clear();
    return false;
}

TString TParser::Last()
{
    IsSpace();
    if(*text) return TString(text);
    return TString();
}


TDataLas::TDataLas(const TString &nameCurve, const TString &unitCurve, const TPtrDepthVector& depthVector)
{
    name = nameCurve;
    unit = unitCurve;
    depth = depthVector;
}

TString TDataLas::Unit() const
{
    return unit;
}

void TDataLas::SetUnit(const TString &value)
{
    unit  = value;
}

double TDataLas::Key(int index) const
{
    return depth->at(index);
}

void TDataLas::SetKey(int index, double value)
{
    TDataBase::SetKey(index, value);
}

double TDataLas::Value(int index, int array) const
{
    return values[index];
}

void TDataLas::SetValue(int index, double value, int array)
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


