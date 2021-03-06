//
// Created by user on 09.10.2019.
//

#ifndef TESTAPP_BINCONTAINER_H
#define TESTAPP_BINCONTAINER_H

#include "DataContainer.h"
#include "BinFile.h"

class THeaderBin : public THeaderBase{
public:
    THeaderBin(std::function<TPtrBinFile()> fun):createFile(std::move(fun)){}
    TString Version() const override;

    TResult CheckFile(const TString& path) override;
    TVecData LoadableData(const TString& path) override; //получаем список кривых которые доступны для загрузки из файла
    TResult LoadData(const TVecData& datas, const TPtrProgress& progress) override;       //загружаем список кривых

protected:
    std::map<int, TVariable> otherHeaderValue;
    std::function<TPtrBinFile()> createFile;
    template <typename T, size_t Size>
    void Copy(T (&desc)[Size], const TVariable& value)
    {
        SafeCopyChar(desc, value.ToString());
    }
    void Copy(float& ptr, const TVariable& value);
    template<typename TCont, typename T>
    const T& Add(TCont& rez, const T& value, size_t offsetKey, const TPtrBinFile& file)
    {
        rez.emplace_back(value);
        value->SetOffsetKey(offsetKey);
        value->SetFile(file);
        return value;
    }
};

class TDataBinProp : public TDataBase{
    PROPERTIES(TDataBinProp, TDataBase,
            PROPERTY(double, coefA, CoefA, SetCoefA);
            PROPERTY(double, coefB, CoefB, SetCoefB);
            )
    PROPERTY_FUN(double, coefA, CoefA, SetCoefA);
    PROPERTY_FUN(double, coefB, CoefB, SetCoefB);
protected:
    double coefA = 1.;      //калибровачные коэффициенты А и
    double coefB = 0.;     //В
};

template <typename TValue, typename TKey>
class TDataBin : public TDataBinProp {
public:
    TDataBin(const TString &n, const TString &u, const TString& c, size_t offsetValue):offset(offsetValue)
    { name = n; unit = u; comment = c; }

    inline auto SetOffsetKey(size_t value) { offsetKey = value; return this; }
    inline auto SetFile(const TPtrBinFile& value) { file = value; return this; }

    double Key(size_t index) const override
    {
        return KeyImpl(index);
    }

    double Value(size_t index, int array) const override
    {
        return ValImpl(index, array);
    }

    void SetValue(size_t index, double value, int array, TTypeEdit typeEdit) override
    {
        *(ValImpl(index) + array) = TValue((value - TDataBinProp::coefB) / TDataBinProp::coefA);
    }

    size_t CountValue() const override
    {
        return file->CountData();
    }

    TDataBin<TValue, TKey> *SetClb(double a, double b, TUnitCategory cat)
    {
        if (a != 0.)
        {
            coefA = a;
            coefB = b;
        }
        category = cat;
        return this;
    }

    double KeyDelta() const override { return keyDelta; }
    void SetKeyDelta(double value) override { keyDelta = value;}

    TVecDouble Coefs() const override { return {coefB, coefA}; }

    void SetCatAndInd(int cat, int ind)
    {
        category = cat;
        indUnit = ind;
    }

    double CalcLinValue(size_t index, int array, int first, int last, const TVecUInt &indx) override
    {
        if(index == first)
            return ValImpl(indx[index + 1], array);
        else
        if(index + 1 == last)
            return ValImpl(indx[index - 1], array);
        else
        {
            return LinValue(indx.begin() + index, ValImpl(indx[index - 1], array), ValImpl(indx[index + 1], array));
        }
    }
protected:
    size_t offset = 0;      //смещение относительно начало структуры
    size_t offsetKey = 0;   //смещение для глубины
    TPtrBinFile file;   //данные файла

    double coefKey = 1.;    //коэффициент для глубины до целых единиц(м. фт.)
    double keyDelta = 0.;   //точка записи для данных

    inline TKey KeyImpl(int index) const   { return  *(reinterpret_cast<TKey*>(file->PtrData(index) + offsetKey)); }
    inline TValue* ValImpl(int index) const { return  reinterpret_cast<TValue*>(file->PtrData(index) + offset); }
    inline double ValImpl(int index, int array) const { return (*(ValImpl(index) + array)) * coefA + coefB; }
    inline double LinValue(const TVecUInt::const_iterator& it, double vb, double ve)
    {
        double kb = KeyImpl(*(it - 1));
        double kc = KeyImpl(*(it));
        double ke = KeyImpl(*(it + 1));
        double scale = (kc - kb) / (ke - kb);
        return vb + (ve - vb) * scale;
    }
};

//класс проверки на Null value в бинарных данных

template<typename T, size_t Offset>
class TNullChecker{
protected:
    bool IsNull(const unsigned char* ptr) const
    {
        return *reinterpret_cast<const T*>(ptr + Offset) == 0;
    }
};

template <typename TValue, typename TKey, size_t N>
struct TDataBinN : public TDataBin<TValue, TKey> {
public:
    TDataBinN(const TString &n, const TString &u, const TString& c, size_t offsetValue)
        :TDataBin<TValue, TKey>(n, u, c, offsetValue){}

    size_t CountArray() const override { return N; }
};

template <typename TBase, typename TChecker>
struct TDataNull : public TBase, public TChecker {
public:
    template<typename... TArgs>
        TDataNull(TArgs&&... args):TBase(args...){}
    double Value(size_t index, int array) const override
    {
        if(TChecker::IsNull(TBase::file->PtrData(index)))
            return NAN;
        return TBase::Value(index, array);
    }
};


using TCountCoef = std::vector<std::tuple<size_t, double>>;

template <typename TValue, typename TKey, size_t N>
struct TDataBinOver : public TDataBinN<TValue, TKey, N>{
public:
    TDataBinOver(const TString &n, const TString &u, const TString& c, size_t offsetValue):
            TDataBinN<TValue, TKey, N>(n, u, c, offsetValue) {}
    auto SetCoef(const TCountCoef& arrayCoef)
    {
        coef = TCountCoef(arrayCoef.size());
        for(size_t i = 0; i < coef.size(); i++)
        {
            size_t off = 0;
            for(size_t j = 0; j < i; j++)
                off += std::get<0>(arrayCoef[j]);
            coef[i] = { off, std::get<1>(arrayCoef[i])};
        }
        return this;
    }
    double Value(size_t index, int array) const override
    {
        if(index != cashIndex) CalcCash(index);
        return cashValue[array];
    }

    void SetValue(size_t index, double value, int array, TTypeEdit typeEdit) override
    {

    }

protected:
    mutable int cashIndex = -1;
    mutable double cashValue[N];
    TCountCoef coef;//количество данных для коэффициента и сам коэффициент
    void CalcCash(int index) const
    {
        cashIndex = index;
        int indSpad = coef.size() - 1;

        TValue* ptrSpad = TDataBin<TValue, TKey>::ValImpl(index);

        size_t offsetSpad = std::get<0>(coef[indSpad]);
        double kus = std::get<1>(coef[indSpad]);

        for(int i = N - 1; i >= 0; i--)
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

template <typename TValue, typename TKey, size_t N>
struct TDataBinMark : public TDataBinN<TValue, TKey, N>{
public:
    TDataBinMark(const TString &n, const TString &u, const TString& c, size_t offsetValue):
            TDataBinN<TValue, TKey, N>(n, u, c, offsetValue){}

    auto SetCoef(const TVecDouble & arrayCoef)
    {
        coef.resize(arrayCoef.size());
        for(size_t i = 0; i < coef.size(); i++)
            coef[i] = arrayCoef.back() / arrayCoef[i];
        return this;
    }
    double Value(size_t index, int array) const override
    {
        if(index != cashIndex)
        {
            cashIndex = index;
            CalcCash(index, cashValue);
        }
        return cashValue[array];
    }
protected:
    mutable int cashIndex = -1;
    mutable double cashValue[N];
    TVecDouble coef;
    void CalcCash(int index, double* cash) const
    {
        size_t indKus = 0;
        size_t count = N + coef.size() - 1;
        TValue* ptrSpad = TDataBin<TValue, TKey>::ValImpl(index);
        for(size_t i = 0, ir = 0; (i < count) && (ir < N); i++)
        {
            if(ptrSpad[i] == -32768)//если дошли до маркера меняем коэффициент
            {
                if(indKus < coef.size() - 1)
                    indKus++;
                continue;
            }
            cash[ir] = ptrSpad[i] * coef[indKus];
            ir++;
        }
    }
};

template <typename TValue, typename TKey, size_t N, typename TChecker>
struct TDataBinMarkNull : public TDataBinMark<TValue, TKey, N>, public TChecker{
public:
    TDataBinMarkNull(const TString &n, const TString &u, const TString& c, size_t offsetValue):
            TDataBinMark<TValue, TKey, N>(n, u, c, offsetValue){};
    double Value(size_t index, int array) const override
    {
        if(TChecker::IsNull(this->file->PtrData(index)))
            return NAN;
        double* ptr = this->cashValue;
        if(index != this->cashIndex)
        {
            if(index == cashIndexAfter)
                ptr = cashAfter;
            else
                if(index == cashIndexBefore)
                    ptr = cashBefore;
                else
                {
                    this->cashIndex = index;
                    this->CalcCash(this->cashIndex, this->cashValue);
                }
        }
        return ptr[array];
    }

    double CalcLinValue(size_t index, int array, int first, int last, const TVecUInt &indx) override
    {
        if(index == first)
        {
            CheckAfter(indx[index + 1]);
            return cashAfter[array];
        }
        else
        if(index + 1 == last)
        {
            CheckBefore(indx[index - 1]);
            return cashBefore[array];
        }
        else
        {
            CheckBefore(indx[index - 1]);
            CheckAfter(indx[index + 1]);
            return this->LinValue(indx.begin() + index, cashBefore[array], cashAfter[array]);
        }
    }
protected:
    mutable int cashIndexBefore = -1;
    mutable int cashIndexAfter = -1;
    mutable double cashBefore[N];
    mutable double cashAfter[N];

    inline void CheckBefore(int index)
    {
        if(index != cashIndexBefore)
        {
            cashIndexBefore = index;
            this->CalcCash(cashIndexBefore, cashBefore);
        }
    }
    inline void CheckAfter(int index)
    {
        if(index != cashIndexAfter)
        {
            cashIndexAfter = index;
            this->CalcCash(cashIndexAfter, cashAfter);
        }
    }
};

TString DosToUtf8(const TString& dos);
TString Cp866ToCp1251(const TString& dos);
TString Cp1251ToUt8(const TString& val);

#define OFFSET(NAME) offsetof(TFormat::TDataType, NAME)

#define KEY_LINE(NAME, TYPE)\
    using TKey = TYPE; \
    offsetKey = OFFSET(NAME);

#define ADD(VALUE) Add(res, VALUE, offsetKey, file)

#define DOUBLE_LINE_T(NAME, UNIT, COMM, DATATYPE, TYPE)\
    std::make_shared<DATATYPE<TYPE, TKey>>(#NAME, #UNIT, COMM, OFFSET(NAME))

#define DOUBLE_LINE(NAME, UNIT, COMM, TYPE) DOUBLE_LINE_T(NAME, UNIT, COMM, TDataBin, TYPE)

#define DOUBLE_LINE_ARRAY(NAME, OFF_NAME, UNIT, COMM, TYPE, ALG, COUNT)\
    std::make_shared<ALG<TYPE, TKey, COUNT>>(#NAME, #UNIT, COMM, OFFSET(OFF_NAME))


#define HEADER_CLB(TYPE, NAME)\
    (*reinterpret_cast<TYPE*>(file->PtrHeader() + offsetof(TFormat::THeaderType, NAME)))

#define CLB(TYPE, KA, KB, ENUM) SetClb(HEADER_CLB(TYPE, KA), HEADER_CLB(TYPE, KB), TUnitCategory::ENUM)


#define HEADER_BIN(TYPE_HEADER, BASE_HEADER, FORMAT, INFO, LOADABLE)\
    class TYPE_HEADER : public BASE_HEADER{\
    public:                                                                   \
        using TFormat = FORMAT;                                                \
        TYPE_HEADER():BASE_HEADER([](){ return std::make_shared<TFormat>(); }){};\
        TPtrHeader Clone() override { return std::make_shared<TYPE_HEADER>(); };\
        TVariable Info(size_t  index) const override\
        {\
            if(file.get() == nullptr) return TVariable();\
            const TFormat::THeaderType& h = *((TFormat::THeaderType*)(file->PtrHeader()));\
            switch (index){\
                INFO\
                default: return otherHeaderValue.count(index) ? otherHeaderValue.at(index) : TVariable();\
            }\
        }\
        void SetInfo(size_t  index, const TVariable& value) override\
        {\
            otherHeaderValue[index] = value;\
        }\
        TVecData LoadableData(const TString& path) override\
        {\
            file = createFile();                                    \
            size_t offsetKey = 0;                                   \
            if (file->LoadFile(path).IsError()) return TVecData();  \
            TVecData res;                                           \
            LOADABLE                                                \
            return res;                                             \
        }\
    protected:\
        TPtrBinFile file;\
    };\
    namespace {\
        const bool addHeader = TContainer::RegisterHeader(std::make_unique<TYPE_HEADER>()); \
        const bool addBinFile = TBinFile::RegisterBinFile(#FORMAT, [](){ return std::make_shared<FORMAT>(); });                                                                      \
    }


#define VAR_LIST(...) __VA_ARGS__
#define INFO(INDEX, RET) case INDEX: { if(otherHeaderValue.count(INDEX) == 0) return RET; return otherHeaderValue.at(INDEX); }
#define SET_INFO(INDEX, VAL) case INDEX: { otherHeaderValue[INDEX] = value; break; }



#endif //TESTAPP_BINCONTAINER_H
