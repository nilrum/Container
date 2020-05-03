//
// Created by user on 09.10.2019.
//

#ifndef TESTAPP_BINCONTAINER_H
#define TESTAPP_BINCONTAINER_H

#include "DataContainer.h"
#include "BinFile.h"


class THeaderBin : public THeaderBase{
public:
    THeaderBin(std::function<TPtrBinFile()> fun):createFile(fun){}
    virtual TString Version() const override;

    virtual TResult CheckFile(const TString& path) override;
    virtual TVecData LoadableData(const TString& path) override; //получаем список кривых которые доступны для загрузки из файла
    virtual TResult LoadData(const TVecData& datas) override;       //загружаем список кривых

protected:
    std::function<TPtrBinFile()> createFile;
    void Copy(char* ptr, const TVariable& value);
    void Copy(float& ptr, const TVariable& value);
    template<typename TCont, typename T>
    const T& Add(TCont& rez, const T& value)
    {
        rez.emplace_back(value);
        return value;
    }
};

template <typename TValue, typename TKey>
class TDataBin : public TDataBase{
public:
    TDataBin(const TString &n, const TString &u, size_t ov, size_t ok, TPtrBinFile f):
        unit(u), offset(ov), offsetKey(ok), file(f){ name = n; }

    virtual TString Unit() const override
    {
        return unit;
    }

    virtual void SetUnit(const TString& value) override
    {
        unit = value;
    }

    virtual double Key(int index) const override
    {
        return  *(reinterpret_cast<TKey*>(file->PtrData(index) + offsetKey));
    }

    virtual double Value(int index, int array = 0) const override
    {
        TValue v = *(reinterpret_cast<TValue*>(file->PtrData(index) + offset));
        return  v * coefA + coefB;
    }

    virtual void SetValue(int index, double value, int array = 0) override
    {
        *(reinterpret_cast<TValue*>(file->PtrData(index) + offset)) = (value - coefB) / coefA;
    }

    virtual size_t CountValue() const override
    {
        return file->CountData();
    }

    TDataBin<TValue, TKey>* SetClb(double a, double b, TUnitCategory cat)
    {
        if(a != 0.)
        {
            coefA = a;
            coefB = b;
        }
        category = cat;
        return this;
    }

    virtual double KeyDelta() const { return keyDelta; }
    virtual void SetKeyDelta(double value) { keyDelta = value; }

    virtual TVecDouble Coefs() const override { return {coefB, coefA}; }
    void SetCatAndInd(int cat, int ind)
    {
        category = cat;
        indUnit = ind;
    }
protected:
    TString title;          //название кривой
    TString unit;           //ед. измерения кривой
    size_t offset = 0;      //смещение относительно начало структуры
    size_t offsetKey = 0;   //смещение для глубины
    double coefKey = 1.;    //коэффициент для глубины до целых единиц(м. фт.)
    double coefA = 1.;      //калибровачные коэффициенты А и
    double coefB = 0.;     //В
    double keyDelta = 0.;   //точка записи для данных
    TPtrBinFile file;   //данные файла
};

using TCountCoef = std::vector<std::tuple<size_t, double>>;
template <typename TValue, typename TKey, int size>
struct TDataBinOver : public TDataBin<TValue, TKey>{
public:
    TDataBinOver(const TString &n, const TString &u, size_t ov, size_t ok, TPtrBinFile f, const TCountCoef& arrayCoef):
            TDataBin<TValue, TKey>(n, u, ov, ok, f)
    {
        coef = TCountCoef(arrayCoef.size());
        for(size_t i = 0; i < coef.size(); i++)
        {
            size_t off = 0;
            for(size_t j = 0; j < i; j++)
                off += std::get<0>(arrayCoef[j]);
            coef[i] = { off, std::get<1>(arrayCoef[i])};
        }
    }
    virtual double Value(int index, int array = 0) const override
    {
        if(index != cashIndex) CalcCash(index);
        return cashValue[array];
    }

    virtual void SetValue(int index, double value, int array = 0) override
    {

    }

    virtual size_t CountArray() const override { return size; }

protected:
    mutable int cashIndex = -1;
    mutable double cashValue[size];
    TCountCoef coef;//количество данных для коэффициента и сам коэффициент
    void CalcCash(int index) const
    {
        cashIndex = index;
        int indSpad = coef.size() - 1;

        TValue* ptrSpad = reinterpret_cast<TValue*>(this->file->PtrData(index) + this->offset);

        size_t offsetSpad = std::get<0>(coef[indSpad]);
        double kus = std::get<1>(coef[indSpad]);

        for(int i = size - 1; i >= 0; i--)
        {
            TValue v = ptrSpad[offsetSpad + i];
            if(v >= 32767 && indSpad != 0)
            {
                indSpad--;
                offsetSpad = std::get<0>(coef[indSpad]);
                kus = std::get<1>(coef[indSpad]);
                v = ptrSpad[offsetSpad + i];
            }
            cashValue[i] = v * kus;
        }
    }
};

TString DosToUtf8(const TString& dos);
TString Cp866ToCp1251(const TString& dos);
TString Cp1251ToUt8(const TString& val);

#define KEY_LINE(NAME, TYPE, STRUCT)\
    using TKey = TYPE; \
    using TStruct = STRUCT;\
    size_t ok = offsetof(TStruct::TDataType, NAME);\
    double coefKey = 1.;//TODO как лучше сделать

#define DOUBLE_LINE(NAME, UNIT, TYPE)\
    std::make_shared<TDataBin<TYPE, TKey>>(#NAME, #UNIT, offsetof(TStruct::TDataType, NAME), ok, file)

#define DOUBLE_LINE_ARRAY(NAME, OFF_NAME, UNIT, TYPE, ALG, COUNT, COEF)\
    std::make_shared<ALG<TYPE, TKey, COUNT>>(#NAME, #UNIT, offsetof(TStruct::TDataType, OFF_NAME), ok, file, COEF)

#define COUNTCOEF(...) TCountCoef{  __VA_ARGS__ }

#define HEADER_CLB(TYPE, NAME)\
    (*reinterpret_cast<TYPE*>(file->PtrHeader() + offsetof(TStruct::THeaderType, NAME)))

#define CLB(TYPE, KA, KB, ENUM) SetClb(HEADER_CLB(TYPE, KA), HEADER_CLB(TYPE, KB), TUnitCategory::ENUM)

#endif //TESTAPP_BINCONTAINER_H
