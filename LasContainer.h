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

    //using TStream = std::ifstream;

    struct TStream{
        std::shared_ptr<FILE> file;
        int c = 0;
        TStream(const TString& path);
        int read_line(TString& value);
        int64_t tellg();
        void seekg(int64_t value);

        bool is_open() const { return file != nullptr; }
        bool eof() const { return c == EOF; }
    };

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

using TPtrDepthVector = std::shared_ptr<TVecDouble>;

class THeaderLas : public THeaderBase, public TLas {
public:
    THeaderLas();
    TPtrHeader Clone() override;
    TString Version() const override;

    TResult CheckFile(const TString &path) override;
    TVecData LoadableData(const TString &path) override; //получаем список кривых которые доступны для загрузки из файла
    TResult LoadData(const TVecData &datas) override;     //загружаем список кривых

    TString TitleInfo(size_t  index) const override;
    void SetTitleInfo(size_t  index, const TString& value);
    TVariable Info(size_t  index) const override;
    void SetInfo(size_t  index, const TVariable& value) override;
    size_t CountInfo() const override;

    TDepthUnit DepthUnit() const override { return depthUnit; }
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
    TDataLas() = default;
    TDataLas(const TString& nameCurve, const TString& unitCurve, const TString& com, const TPtrDepthVector& depthVector);

    TString Unit() const override;
    void SetUnit(const TString& value) override;

    double Key(size_t index) const override;
    size_t SetKey(size_t index, double value, bool isSort, TTypeEdit typeEdit) override;
    double Value(size_t index, int array) const override;
    void SetValue(size_t index, double value, int array, TTypeEdit typeEdit) override;
    size_t CountValue() const override;

    const double* PtrKey() override;
    const double* PtrValue(int array) override;
    void SwapValue(TVecDouble& value) override;

    PROPERTIES(TDataLas, TDataBase,
        PROPERTY(TString, comment, Comment, SetComment);
    )

    PROPERTY_FUN(TString, comment, Comment, SetComment);
protected:
    TString unit;
    TString comment;
    TPtrDepthVector depth;
    TVecDouble values;
};
#endif //NEO_LASCONTAINER_H
