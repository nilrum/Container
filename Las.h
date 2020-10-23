//
// Created by user on 14.10.2020.
//

#ifndef NEO_LAS_H
#define NEO_LAS_H

#include "Variable.h"
#include "Progress.h"
#include <functional>

class TLas {
public:
    inline bool IsWrap() const  { return isWrap; }
    inline double NullValue() const { return nullVal; }
    inline double LasVersion() const { return lasVersion; }
protected:
    double lasVersion = 2.0;
    bool isWrap = false;
    double nullVal = -9999.25;

    STATIC_ARG(TVecString, LasHeaderNames, InitLasHeaderNames());
    static TVecString InitLasHeaderNames();
};

class TStream;
enum class TResultLas { Ok, FileNotOpen, EmptyFile, LasSecNotFound, NotSupportVersion, BadSintax, ErrCountReadData, ErrCountWriteData };
using TPtrDepthVector = std::shared_ptr<TVecDouble>;

class TLasReader : public TLas{
public:
    TLasReader();
    void Clear();

    TResult Read(const TString& path, bool andValues = false);
    TResult ReadData(const TString& path, const TPtrProgress& progress);

    inline size_t CountCurves() const { return lasCurves.size(); };
    inline const TVecDouble LasCurve(int index) const { return lasCurves[index]; }

protected:
    TVecVecDouble lasCurves;
    int64_t offsetAsciiData = 0;

    using TReadSection = std::function<TResult(TStream& stream, TString& line)>;
    struct TReadInfo{
        bool isReaded = false;
        TString id;
        TReadSection fun;
    };
    static const int CountInfo = 5;
    TReadInfo readInfos[CountInfo];

    virtual void AddWellInfo(const TVecString& info);
    virtual void AddCurveInfo(const TVecString& info);

    TString ReadLine(TStream& stream);
    TResult ReadParam(const TString& line, TVecString& params, bool readVal);

    TResult ReadVersion(TStream& stream, TString& line);
    TResult ReadWell(TStream& stream, TString& line);
    TResult ReadCurves(TStream& stream, TString& line);
    TResult ReadParams(TStream& stream, TString& line);
    TResult ReadOther(TStream& stream, TString& line);
    TResult ReadAscii(TStream& stream, TString& line);
    TResult ReadAsciiWrap(TStream& stream, TString& line);
    TResult ReadAsciiNoWrap(TStream& stream, TString& line);
};

struct TNormInfo{
    TVecDouble::const_iterator beginKey;
    TVecDouble::const_iterator endKey;
    TVecDouble::const_iterator beginVal;
    size_t num = 0;
    double nullValue = NAN;
    double CalcValue(double key);
};

using TVecNormInfo = std::vector<TNormInfo>;

TVecVecDouble NormLas(TVecNormInfo vec, double begin, double end, double step);

ENUM(TWellInfoInd,  wiSTRT, wiSTOP, wiSTEP, wiNULL, wiCOMP, wiWELL, wiFLD,
     wiLOC, wiCNTY, wiSTAT, wiCTRY,
     wiSRVC, wiDATE, wiAPI, wiCount)

TString Transliteration(TString utf8text);

class TLasWriter : public TLas{
public:
    TLasWriter();

    void Clear();
    void SetIsWrap(bool value){ isWrap = value; };

    size_t DepthCountPoint() const { return depthCountPoint; }
    void SetDepthCountPoint(size_t value);

    TString UnitDepth() const { return unitDepth; }
    void SetUnitDepth(const TString& value);

    TString WellInfoTitle(size_t index) const;

    void SetWellInfo(size_t index, const TVariable& value);
    TVariable WellInfo(size_t index) const;

    TString WellInfoStr(size_t index) const;
    void SetWellInfoStr(size_t index, const TString& value);

    double WellInfoDbl(size_t index) const;
    void SetWellInfoDbl(size_t index, double value);

    TNormInfo& AddCurveInfo(const TString& name, const TString& unit, const TString comment, const TString& format = TString());

    TResult Write(const TString& path, const TPtrProgress& progress = TPtrProgress());
private:
    size_t depthCountPoint = 2;

    TString unitDepth = "M";
    std::vector<TVariable> wellInfo;
    struct TCurveInfo{
        TString name;
        TString unit;
        TString comment;
        TString frmt;
        TNormInfo norm;
    };
    std::vector<TCurveInfo> curveInfos;
    TString defaultValueFrmt = "%11.2f";
    size_t numWrap = 6;

    TVecString lasWellInfoComments = InitLasWellInfoComments();
    static TVecString InitLasWellInfoComments();
};

class TStream{
private:
    std::shared_ptr<FILE> file;
    int c = 0;

    TPtrProgress progress;
public:
    TStream(const TString& path, TOpenFileMode mode = TOpenFileMode::Read);
    void SetProgress(const TPtrProgress& value);

    int read_line(TString& value);

    int64_t tellg();
    void seekg(int64_t value);

    bool is_open() const { return file != nullptr; }
    bool eof() const { return c == EOF; }

    FILE* get() { return file.get(); }
};

class TParser{
public:
    TParser(const char* t):text(t), pos(0){};
    TParser(const std::string& t, bool isUpper = false);
    inline void Set(const char* t) { text = t; pos = 0;}
    void Set(const std::string& t, bool isUpper = false);

    bool IsId(const char* id);    //проверяет и читает что есть идентификатор
    bool IsChar(int value);             //проверяет что есть символ
    bool IsNumber(double& val);
    int IgnoreNumber();
    bool IsSpace();   //проверяет и читает все пробельные символы
    bool IsEnd();

    bool ReadId(TString& value);
    bool ReadUntil(int stop, TString& value);

    TString Last();
private:
    std::string buffer;
    const char* text = nullptr;
    int pos = 0;
};

#endif //NEO_LAS_H
