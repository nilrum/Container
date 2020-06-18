//
// Created by user on 05.06.2020.
//

#ifndef NEO_LASCONTAINER_H
#define NEO_LASCONTAINER_H

#include "DataContainer.h"


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


enum class TResultLas { Ok, FileNotOpen, EmptyFile, LasSecNotFound, NotSupportVersion, BadSintax, ErrCountReadData };

class TLas{
public:
    TLas();
    void Clear();
    TResult Read(const TString& path, bool andValues = false);
    TResult ReadData(const TString& path);

    inline bool IsWrap() const { return isWrap; }
    inline double NullValue() const { return nullVal; }

    inline size_t CountCurves() const { return lasCurves.size(); };
    inline const TVecDouble LasCurve(int index) const { return lasCurves[index]; }

    inline double LasVersion() const { return lasVersion; }
protected:
    double lasVersion = 2.0;
    bool isWrap = false;
    double nullVal = -9999.25;

    TVecVecDouble lasCurves;
    int64_t offsetAsciiData = 0;

    using TReadSection = std::function<TResult(std::ifstream& stream, TString& line)>;
    struct TReadInfo{
        bool isReaded = false;
        TString id;
        TReadSection fun;
    };
    static const int CountInfo = 5;
    TReadInfo readInfos[CountInfo];



    virtual void AddWellInfo(const TVecString& info);
    virtual void AddCurveInfo(const TVecString& info);

    TString ReadLine(std::ifstream& stream);
    TResult ReadParam(const TString& line, TVecString& params, bool readVal);

    TResult ReadVersion(std::ifstream& stream, TString& line);
    TResult ReadWell(std::ifstream& stream, TString& line);
    TResult ReadCurves(std::ifstream& stream, TString& line);
    TResult ReadParams(std::ifstream& stream, TString& line);
    TResult ReadOther(std::ifstream& stream, TString& line);
    TResult ReadAscii(std::ifstream& stream, TString& line);
    TResult ReadAsciiWrap(std::ifstream& stream, TString& line);
    TResult ReadAsciiNoWrap(std::ifstream& stream, TString& line);
};

using TPtrDepthVector = std::shared_ptr<TVecDouble>;

class THeaderLas : public THeaderBase, public TLas {
public:
    THeaderLas();
    virtual TPtrHeader Clone();
    virtual TString Version() const override;

    virtual TResult CheckFile(const TString &path) override;
    virtual TVecData LoadableData(const TString &path) override; //получаем список кривых которые доступны для загрузки из файла
    virtual TResult LoadData(const TVecData &datas) override;     //загружаем список кривых

    virtual TString TitleInfo(int index) const;
    void SetTitleInfo(int index, const TString& value);
    virtual TVariable Info(int index) const;
    virtual void SetInfo(int index, const TVariable& value);
    virtual size_t CountInfo() const;

    virtual TDepthUnit DepthUnit() const { return depthUnit; }
protected:
    TDepthUnit depthUnit = duNone;
    TVecString othTitle;
    std::vector<TVariable> headerInfo;
    bool isCheckingFile = false;
    TString loadablePath;
    TPtrDepthVector depthVec = std::make_shared<TVecDouble>();
    TVecData datas;
    int LasHeaderToInd(const TString& value);

    virtual void AddWellInfo(const TVecString& info) override;
    virtual void AddCurveInfo(const TVecString& info) override;

    STATIC_ARG(TVecString, LasHeaderNames, InitLasHeaderNames());
    static TVecString InitLasHeaderNames();
};

class TDataLas : public TDataBase{
public:
    TDataLas(const TString& nameCurve, const TString& unitCurve, const TPtrDepthVector& depthVector);

    virtual TString Unit() const;
    virtual void SetUnit(const TString& value);

    virtual double Key(int index) const;
    virtual void SetKey(int index, double value);
    virtual double Value(int index, int array = 0) const;
    virtual void SetValue(int index, double value, int array = 0);
    virtual size_t CountValue() const;

    virtual const double* PtrKey();
    virtual const double* PtrValue(int array = 0);
    virtual void SwapValue(TVecDouble& value);
protected:
    TString unit;
    TPtrDepthVector depth;
    TVecDouble values;
};
#endif //NEO_LASCONTAINER_H
