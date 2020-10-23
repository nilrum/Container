//
// Created by user on 14.10.2020.
//

#include "Las.h"

REGISTER_CODES(TResultLas,
               TEXT_CODE(FileNotOpen, "Error opening file");
               TEXT_CODE(EmptyFile, "File is empty");
               TEXT_CODE(LasSecNotFound, "Las section not found");
               TEXT_CODE(NotSupportVersion, "Error version file");
               TEXT_CODE(BadSintax, "Error las sintax");
               TEXT_CODE(ErrCountReadData, "Error count read data");
               TEXT_CODE(ErrCountWriteData, "Error count write data");
)

TVecString TLas::InitLasHeaderNames()
{
    //iiArea = 0, iiWell, iiDate, iiTime, iiBegin, iiEnd, iiStep, iiCompany, iiServComp
    return {"FLD", "WELL", "DATE", "", "STRT", "STOP", "STEP", "COMP", "SRVC"};
}
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
TLasReader::TLasReader():
    readInfos{
            {false, "~V", [this](TStream& stream, TString& line) { return ReadVersion(stream, line); }},
            {false, "~W", [this](TStream& stream, TString& line) { return ReadWell(stream, line); }},
            {false, "~C", [this](TStream& stream, TString& line) { return ReadCurves(stream, line); }},
            {false, "~P", [this](TStream& stream, TString& line) { return ReadParams(stream, line); }},
            {false, "~O", [this](TStream& stream, TString& line) { return ReadOther(stream, line); }},
            }
{

}

void TLasReader::Clear()
{
    for(auto& info : readInfos)
        info.isReaded = false;//секция не прочитана
    lasCurves.clear();
    offsetAsciiData = 0;
}

TResult TLasReader::Read(const TString &path, bool andValues)
{
    Clear();
    //TStream stream(path.c_str(), std::ifstream::binary);
    TStream stream(path);
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
    offsetAsciiData = stream.tellg();
    if(andValues)
        return ReadAscii(stream, line);
    return TResult();
}

TResult TLasReader::ReadData(const TString& path, const TPtrProgress& progress)
{
    TStream stream(path);
    if(stream.is_open() == false) return TResultLas::FileNotOpen;
    stream.SetProgress(progress);
    stream.seekg(offsetAsciiData);
    TString line = ReadLine(stream);
    return ReadAscii(stream, line);
}


TString TLasReader::ReadLine(TStream& stream)
{
    TString res;
    while(stream.eof() == false)
    {
        //std::getline(stream, res);
        stream.read_line(res);
        if(res.empty() == false && res[0] != '#') return res;
    }
    return res;
}

TResult TLasReader::ReadVersion(TStream& stream, TString& line)
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

TResult TLasReader::ReadWell(TStream& stream, TString& line)
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

TResult TLasReader::ReadCurves(TStream& stream, TString& line)
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

TResult TLasReader::ReadParams(TStream& stream, TString& line)
{
    do
    {
        line = ReadLine(stream);//пропускаем ~P
    } while(line.empty() == false && line[0] != '~');
    return TResult();
}

TResult TLasReader::ReadOther(TStream& stream, TString& line)
{
    do
    {
        line = ReadLine(stream);//пропускаем ~O
    } while(line.empty() == false && line[0] != '~');
    return TResult();
}

TResult TLasReader::ReadParam(const TString &line, TVecString& params, bool readVal)
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

TResult TLasReader::ReadAscii(TStream& stream, TString &line)
{
    if(line[0] == '~')
        line = ReadLine(stream);//пропускаем ~A
    if(line.empty()) return TResultLas::BadSintax;
    if(isWrap)
        return ReadAsciiWrap(stream, line);
    else
        return ReadAsciiNoWrap(stream, line);
}

TResult TLasReader::ReadAsciiWrap(TStream& stream, TString &line)
{
    TParser pars(line.data());
    size_t i = 0;
    while(true)
    {
        auto& d = lasCurves[i];

        if(d.empty())
        {
            if (pars.IgnoreNumber() == 0) return TResultLas::ErrCountReadData;
        }
        else
        {
            if (pars.IsNumber(d.back()) == false) return TResultLas::ErrCountReadData;//нехватило данных для считывания
            if(d.back() == nullVal) d.back() = NAN;
        }
        if(pars.IsEnd())
        {
            line = ReadLine(stream);
            if(line.empty()) break;
            pars.Set(line.data());
        }
        i++;
        if(i >= lasCurves.size())
        {
            i = 0;
            for (auto &d : lasCurves)
                if (d.empty() == false) d.emplace_back(0);
        }
    }
    return TResult();
}

TResult TLasReader::ReadAsciiNoWrap(TStream& stream, TString &line)
{
    TParser pars(line.data());

    while(true)
    {
        for(auto& d : lasCurves)
        {
            if(d.empty())
            {
                if (pars.IgnoreNumber() == 0)
                    return TResultLas::ErrCountReadData;
            }
            else
            {
                if (pars.IsNumber(d.back()) == false)
                    return TResultLas::ErrCountReadData;//нехватило данных для считывания
                if(d.back() == nullVal)
                    d.back() = NAN;
            }
        }
        line = ReadLine(stream);
        if(line.empty() == false)
        {
            for (auto &d : lasCurves)
                if (d.empty() == false) d.emplace_back(0);
            pars.Set(line.data());
        }
        else
            break;
    }
    return TResult();
}

void TLasReader::AddWellInfo(const TVecString &info)
{
    if(ToUpperCase(info[0]) == "NULL")
    {
        try{
            nullVal = std::stod(info[2]);
        }
        catch (std::invalid_argument){};
    }
}

void TLasReader::AddCurveInfo(const TVecString &info)
{
    lasCurves.push_back(TVecDouble(1));//по умолчанию добавляем одно поле на считывание
}

//----------------------------------------------------------------------------------------------------------------------
TLasWriter::TLasWriter():wellInfo(TWellInfoInd::wiCount)
{
    Clear();
    wellInfo[wiNULL] = nullVal;
}

void TLasWriter::Clear()
{
    curveInfos.clear();
    curveInfos.emplace_back(TCurveInfo {"DEPT", unitDepth, "Depth curve", ""});
    SetDepthCountPoint(depthCountPoint);
}

void TLasWriter::SetUnitDepth(const TString &value)
{
    unitDepth = value;
    curveInfos[0].unit = unitDepth;
}

void TLasWriter::SetDepthCountPoint(size_t value)
{
    depthCountPoint = value;
    curveInfos[0].frmt = TString("%") + STDFORMAT("9.%df", depthCountPoint);
}

TResult TLasWriter::Write(const TString &path, const TPtrProgress& progress)
{
    if(curveInfos.size() < 2) return TResultLas::ErrCountWriteData;
    TStream stream(path, TOpenFileMode::Write);
    if(stream.is_open() == false) return TResultLas::FileNotOpen;
    FILE* f = stream.get();
    fprintf(f, "~Version information\n");
    fprintf(f, "  VERS. %31.1f: CWLS LAS  -  VERSION\n", lasVersion);
    if(isWrap)
        fprintf(f, "  WRAP. %31s: Multiple lines per depth step\n", "YES");
    else
        fprintf(f, "  WRAP. %31s: One Line per Depth Step\n", "NO");
    fprintf(f, "~Well information\n");
    fprintf(f, "# MNEM.UNIT       DATA TYPE INFORMATION\n");
    fprintf(f, "# ====.================================: ===================\n");

    double begin = WellInfoDbl(wiSTRT);
    double end = WellInfoDbl(wiSTOP);
    double step =  WellInfoDbl(wiSTEP);
    if(step == 0) step = 0.1;
    end = begin + int((end - begin) / step) * step;

    SetWellInfoDbl(wiSTRT, begin);
    SetWellInfoDbl(wiSTOP, end);
    SetWellInfoDbl(wiSTEP, step);

    for(size_t i = wiSTRT; i <= wiSTEP; i++)
        fprintf(f, "  %s.%s %30.*f: %s\n", STR(WellInfoTitle(i)), STR(unitDepth),
                depthCountPoint, wellInfo[i].ToDouble(), STR(lasWellInfoComments[i]));

    fprintf(f, "  %s. %31.*f: %s\n", STR(WellInfoTitle(wiNULL)), depthCountPoint,
            wellInfo[wiNULL].ToDouble(), STR(lasWellInfoComments[wiNULL]));

    for(size_t i = wiCOMP; i <= wiAPI; i++)
        fprintf(f, "  %-4s. %31s: %s\n", STR(WellInfoTitle(i)),
                STR(Transliteration(wellInfo[i].ToString())), STR(lasWellInfoComments[i]));
    fprintf(f, "~Curve information\n");
    fprintf(f, "# MNEM.UNIT                    API CODE  CURVE DESCRIPTION\n");
    fprintf(f, "# ====.================================:====================\n");
    for(const auto c : curveInfos)
    {
        TString nameUnit = STDFORMAT("%-4s.%s", STR(c.name), STR(c.unit));
        fprintf(f, "  %-36s : %s\n", STR(nameUnit), STR(c.comment));
    }
    fprintf(f, "~Parameter information block\n");
    fprintf(f, "# MNEM.UNIT                     VALUE       DESCRIPTION\n");
    fprintf(f, "# ====.================================:====================\n");
    fprintf(f, "~Other information\n");
    fprintf(f, "# ------------------- REMARKS AREA ------------------------\n");
    fprintf(f, "# ==========================================================\n");
    fprintf(f, "~ASCII Log Data\n");
    if(progress)
    {
        progress->SetMaxAndBorderCoef((end - begin) / step, 1. / 20.);
        progress->SetTypeProgress(TProgress::tpStep);
    }
    if(isWrap == false)
    {
        while(begin <= end)
        {
            fprintf(f, STR(curveInfos[0].frmt), begin);//записываем глубину
            for(size_t i = 1; i < curveInfos.size(); i++)
                fprintf(f, STR(curveInfos[i].frmt), curveInfos[i].norm.CalcValue(begin));
            fprintf(f, "\n");
            begin += step;
            if(progress) progress->Progress(1.);
        }
    }
    else
    {
        int num = 0;
        while(begin <= end)
        {
            fprintf(f, STR(curveInfos[0].frmt), begin);//записываем глубину
            fprintf(f, "\n");
            num = 0;
            for(size_t i = 1; i < curveInfos.size(); i++)
            {
                fprintf(f, STR(curveInfos[i].frmt), curveInfos[i].norm.CalcValue(begin));
                num++;
                if(num == numWrap)
                {
                    num = 0;
                    fprintf(f, "\n");
                }
            }
            fprintf(f, "\n");
            begin += step;
            if(progress) progress->Progress(1.);
        }
    }
    if(progress) progress->Finish();
    return TResult();
}


TVecString TLasWriter::InitLasWellInfoComments()
{
    return {"First depth in file", "Last  depth in file", "Depth increment", "Null  values",
            "COMPANY", "WELL", "FIELD", "LOCATION", "COUNTY", "STATE", "COUNTRY",
            "SERVICE COMPANY", "LOG DATE", "API NUMBER"
            };
}

TString TLasWriter::WellInfoTitle(size_t index) const
{
    return EnumName<TWellInfoInd>(index).substr(2);//пропускаем wi
}

void TLasWriter::SetWellInfo(size_t index, const TVariable &value)
{
    if(index < wellInfo.size())
        wellInfo[index] = value;
}

TVariable TLasWriter::WellInfo(size_t index) const
{
    if(index < wellInfo.size())
        return wellInfo[index];
    return TVariable();
}

TNormInfo& TLasWriter::AddCurveInfo(const TString &name, const TString &unit, const TString comment, const TString &format)
{
    curveInfos.emplace_back(TCurveInfo {name, unit, comment, (format.empty() ? defaultValueFrmt : format)});
    return curveInfos.back().norm;
}

TString TLasWriter::WellInfoStr(size_t index) const
{
    return WellInfo(index).ToString();
}

double TLasWriter::WellInfoDbl(size_t index) const
{
    return WellInfo(index).ToDouble();
}

void TLasWriter::SetWellInfoStr(size_t index, const TString &value)
{
    SetWellInfo(index, value);
}

void TLasWriter::SetWellInfoDbl(size_t index, double value)
{
    SetWellInfo(index, value);
}

double TNormInfo::CalcValue(double key)
{
    if(beginKey + num >= endKey) return nullValue;//данных нет или закончились
    if(TDoubleCheck::Less(key, beginKey[num]) && num == 0) return nullValue;//до данных еще не добрались

    while(beginKey + num < endKey)//пока не дошли до конца и не нашли значение большее переданного
    {
        if(TDoubleCheck::Less(key,beginKey[num]))
        {
            double scale = (key - beginKey[num - 1]) / (beginKey[num] - beginKey[num - 1]);
            return beginVal[num - 1] + (beginVal[num] - beginVal[num - 1]) * scale;
        }
        else if(TDoubleCheck::Equal(key,beginKey[num]))
            return beginVal[num];
        else
            num++;
    }
    return nullValue;
}

TVecVecDouble NormLas(TVecNormInfo vec, double begin, double end, double step)
{
    TVecVecDouble res(vec.size() + 1, TVecDouble(int(1 + (end - begin) / step)));
    int i = 0;
    while(begin <= end)
    {
        res[0][i] = begin;//записываем глубину
        for(size_t j = 1; j < res.size(); j++)
            res[j][i] = vec[j - 1].CalcValue(begin);
        begin += step;
        i++;
    }
    return res;
}
//----------------------------------------------------------------------------------------------------------------------
TStream::TStream(const TString &path, TOpenFileMode mode):file(OpenFile(path, mode))
{

}

int TStream::read_line(TString &value)
{
    value.clear();
    c = std::fgetc(file.get());
    while(c != EOF)
    {
        if(c == '\n' || c == '\r')//если символ конца строки
        {
            if(value.empty() == false)
                break;
        }
        else
            value.push_back(c);

        c = std::fgetc(file.get());
    }
    if(progress) progress->Progress(double(tellg()));
    return c;
}

int64_t TStream::tellg()
{
    fpos_t pos;
    std::fgetpos(file.get(), &pos);
#ifdef _WIN32
    return int64_t(pos);
#else
    int64_t res;
    memcpy(&res, &pos, sizeof(int64_t));
    return res;
#endif
}

void TStream::seekg(int64_t value)
{
#ifdef _WIN32
    fpos_t pos = fpos_t(value);
#else
    fpos_t pos;
    memcpy(&pos, &value, sizeof(int64_t));
#endif
    std::fsetpos(file.get(), &pos);//TODO подумать над fseek
}

void TStream::SetProgress(const TPtrProgress& value)
{
    progress = value;
    if(progress && is_open())
    {
        int64_t oldPos = tellg();
        std::fseek(file.get(), 0, SEEK_END);
        progress->SetMax(double(tellg()));
        progress->SetBorderOfMax(1 / 20.);
        seekg(oldPos);
    }
}

//----------------------------------------------------------------------------------------------------------------------
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
        if(std::isspace((unsigned char)(*text))) text++;
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

bool TParser::IsEnd()
{
    IsSpace();
    return !(*text);
}
static const char* trans[] = {"A", "B", "V", "G", "D", "E", "ZH", "Z", "I", "I", "K", "L", "M", "N", "O",
                  "P", "R", "S", "T", "U", "F", "H", "TS", "CH", "SH", "SCH", "", "Y", "", "E", "YU", "YA",
                  "a", "b", "v", "g", "d", "e", "zh", "z", "i", "i", "k", "l", "m", "n", "o",
                  "p", "r", "s", "t", "u", "f", "h", "ts", "ch", "sh", "sch", "", "y", "", "e", "yu", "ya",
                  };

TString Transliteration(TString utf8text)
{
    TString res;
    for(auto it = utf8text.begin(); it != utf8text.end(); it++)
    {
        unsigned char a = *it;
        if (a < 0xC0)//символ не русский
        {
            res.push_back(a);
        }
        else
        {
            unsigned short unic = (a & 0x3F) << 6;
            it++;
            a = *it;
            unic = unic | (0x7F & a);//получили символ в юникоде
            if(unic >= 0x410 && unic <= 0x044F)//А = 0x0410 - Я = 0x042F, а = 0x0430 - я = 0x044F
                res.insert(res.length(), trans[unic - 0x410]);
            else if(unic == 0x401) res.insert(res.length(), "EO");//Ё буква отдельно
            else if(unic == 0x451) res.insert(res.length(), "eo");//ё
        }
    }
    return res;
}

