//
// Created by user on 08.10.2019.
//

#ifndef TESTAPP_DATACONTAINER_H
#define TESTAPP_DATACONTAINER_H

#include "PropertyClass.h"
#include "Units.h"

#include <climits>

class THeaderBase;
using TPtrHeader = std::shared_ptr<THeaderBase>;

class TDataBase;
using TPtrData = std::shared_ptr<TDataBase>;
using TVecData = std::vector<TPtrData>;

enum class TTypeEdit{NoUpdate, UpdateValues, UpdateViews, FullUpdate};

constexpr bool IsEditValues(TTypeEdit value) { return value == TTypeEdit::UpdateValues || value == TTypeEdit::FullUpdate; }
constexpr bool IsEditViews(TTypeEdit value) { return value == TTypeEdit::UpdateViews || value == TTypeEdit::FullUpdate; }

using TOnDataEdit = sigslot::signal<TTypeEdit>;
using TOnNameChanged = sigslot::signal<>;

class TBaseContainer;
using TPtrBaseContainer = std::shared_ptr<TBaseContainer>;
using TWPtrBaseContainer = std::weak_ptr<TBaseContainer>;
using TVecBaseContainer = std::vector<TPtrBaseContainer>;
using TFindPred = std::function<bool(const TPtrData&)>;

class TBaseContainer : public TPropertyClass{
public:
    TBaseContainer() = default;
    TBaseContainer(const TBaseContainer& oth):parent(oth.parent), childData(oth.childData){}

    TString FullName() const;   //полный путь к данном элементу
    void SetName(const TString &value) override;

    virtual const TPtrHeader& Header() const;

    const TWPtrBaseContainer& Parent() const;
    void SetParent(const TWPtrBaseContainer& value);

    TPtrBaseContainer LockParent() const;

    virtual size_t CountChildData() const;
    virtual const TPtrData& ChildData(size_t index) const;
    virtual TPtrBaseContainer ChildContainer(size_t index) const;

    virtual const TPtrData& AddChildData(const TPtrData& value);
    virtual void DelChildData(const TPtrData& value);

    virtual TPtrData CreateChildData(int t) { return std::make_shared<TDataBase>(); }
    inline TPtrData AddDefChildData(int t = 0) { return AddChildData(CreateChildData(t)); };

    TPtrData FindData(const TString& pathData);
    virtual TPtrData FindDataPath(const TVecString &path, size_t pos, bool isThis);
    virtual TVecData FindDataPred(const TFindPred& pred);

    TOnNameChanged OnNameChanged;
protected:
    TWPtrBaseContainer parent;
    TVecData childData;

    virtual void CallUsed(const TPtrData& value);

    template<typename TCont>
    TVecData FindDataPredThis(const TCont &cont, const TFindPred& pred)
    {
        TVecData res;
        for(const auto& child : cont)
        {
            if (pred(child))
                res.emplace_back(child);
            FindDataPredChild(child, pred, res);
        }
        return res;
    }

    template<typename TChild>
    void FindDataPredChild(const TChild &child, const TFindPred& pred, TVecData& res)
    {
        if(child->CountChildData() == 0) return;
        TVecData childRes = child->FindDataPred(pred);
        if(childRes.size())
            res.insert(res.end(), childRes.begin(), childRes.end());
    }
    template<typename TCont>
    TPtrData FindDataThis(const TVecString &path, size_t pos, const TCont &cont)
    {
        if(pos < path.size() && name == path[pos])
            return FindDataChild(path, pos + 1, cont);
        return TPtrData();
    }
    template<typename TCont>
    TPtrData FindDataChild(const TVecString &path, size_t pos, const TCont &cont)
    {
        if(pos < path.size())
            for(const auto& child : cont)
            {
                TPtrData res = child->FindDataPath(path, pos, true);
                if(res) return res;
            }
        return std::dynamic_pointer_cast<TDataBase>(shared_from_this());
    }
};

class TDataBase: public TBaseContainer{
public:
    virtual ~TDataBase() = default;

    virtual TString Unit() const{ return TString(); };
    virtual void SetUnit(const TString& value){};

    virtual void Set(TVecDouble* keys, TVecDouble* vals, int countArray, bool isSwap, TTypeEdit typeEdit){};

    virtual double Key(size_t index) const{ return 0; };
    virtual size_t SetKey(size_t index, double value, bool isSort, TTypeEdit typeEdit){ return index; };
    virtual double Value(size_t index, int array) const { return 0; };
    virtual void SetValue(size_t index, double value, int array){};
    virtual size_t CountValue() const { return 0; };
    virtual size_t CountArray() const { return 1; };

    inline size_t SetKey(size_t index, double value){ return SetKey(index, value, true, TTypeEdit::FullUpdate); }
    inline double Value(size_t index) const { return Value(index, 0); }
    inline void SetValue(size_t index, double value){ SetValue(index, value, 0); };

    TString Title() const { return Name() + (Unit().empty() ? TString() : ("," + Unit())); }

    inline double FirstKey() const { return Key(0); }
    inline double LastKey() const { return Key(CountValue() - 1); }

    inline double FirstValue(int array = 0) const { return Value(0, array); }
    inline double LastValue(int array = 0) const { return Value(CountValue() - 1, array); }

    virtual void Insert(size_t index, const TVecDouble& keyValues, bool isSort){};
    virtual void Delete(size_t index, size_t count){};

    inline void Insert(size_t index, const TVecDouble& keyValues = TVecDouble()) { Insert(index, keyValues, true); }
    inline void Delete(size_t index){ Delete(index, 1); };
    inline void Add(const TVecDouble& keyValues, bool isSort = false) { Insert(CountValue(), keyValues, isSort); }

    virtual void Load(FILE* file) {};
    virtual void Save(FILE* file) {};

    virtual TVecString DefaultTitles() const{ return TVecString(); }
    virtual TVecString DefaultEnumTypes() const{ return TVecString(); }

    TOnDataEdit OnEdited;

    //vector interface
    inline size_t size() const { return CountValue();};
    inline double at(int index) const { return Key(index); }

    //если есть возможность отдает указатель на данные напрямую
    virtual const double* PtrKey() { return nullptr; };
    virtual const double* PtrValue(int array){ return nullptr; };
    virtual void SwapValue(TVecDouble& value){};//TODO подумать над необходимстью


    virtual void ApplyScaleDeltaKey(double scale, double delta, TTypeEdit typeEdit){};

    void Assign(const TPtrData& value);

    PROPERTIES_CREATE(TDataBase, TPropertyClass, NO_CREATE(),
        PROPERTY(TString, unit, Unit, SetUnit);
        PROPERTY_READ(size_t, countArray, CountArray);
        PROPERTY(int, tag, Tag, SetTag);
        PROPERTY(bool, isUsed, IsUsed, SetIsUsed).NoSerialization();
        PROPERTY(TUnitCategory, category, Category, SetCategory).NoSerialization();
        PROPERTY(TEnum, indUnit, IndUnit, SetIndUnitEn).NoSerialization();
    )
    PROPERTY_FUN(int, tag, Tag, SetTag);
    PROPERTY_FUN_CHG(TUnitCategory, category, Category, SetCategory);

    TEnum IndUnit() const;
    void SetIndUnitEn(const TEnum& value);
    void SetIndUnit(int value);

    virtual TVecDouble Coefs() const { return TVecDouble(); }
    virtual void SetCoefs(const TVecDouble& value) { }

    virtual double KeyDelta() const { return 0.;}
    virtual void SetKeyDelta(double value) {}

    bool IsUsed() const;
    virtual void SetIsUsed(bool value);
    void SetIsUsedNoCall(bool value);

    virtual double CalcLinValue(size_t index, int array, int first, int last, const TVecUInt& indx){ return NAN; };
protected:
    int isUsed = false;
    int tag = 0;
    TUnitCategory category = ucNone;
    int indUnit = 0;
};

int IndexForTag(const TPtrData& value, int tag);

enum TIdInfo{iiArea = 0, iiWell, iiDate, iiTime, iiBegin, iiEnd, iiStep, iiCompany, iiServComp, iiCountInfo};

class THeaderBase{
public:
    virtual ~THeaderBase() = default;

    virtual TPtrHeader Clone() = 0;
    virtual TString Version() const = 0;

    virtual TString TitleInfo(size_t index) const;
    virtual TVariable Info(size_t  index) const = 0;
    virtual void SetInfo(size_t  index, const TVariable& value){};
    virtual size_t CountInfo() const;
    virtual void CopyInfo(const TPtrHeader& src){};

    virtual TResult CheckFile(const TString& path) = 0;        //проверка файла на соответствие формату
    virtual TVecData LoadableData(const TString& path) = 0; //получаем список кривых которые доступны для загрузки из файла
    virtual TResult LoadData(const TVecData& datas) = 0;       //загружаем список кривых

    virtual TDepthUnit DepthUnit() const { return duNone; }

    static TVecString InitDefTitle();
    static const TVecString& DefTitle(){ static TVecString defTitle = InitDefTitle(); return defTitle; }
};

using TPtrRegHeader = std::unique_ptr<THeaderBase>;
using TVecHeader = std::vector<TPtrRegHeader>;

enum class TContainerResult{ Ok, InvHeader};

class TContainer : public TBaseContainer{
public:
    TContainer() = default;
    TContainer(const TPtrHeader& value);
    TContainer(const TContainer& oth):TBaseContainer(oth), header(oth.header){}

    bool IsValid() const;

    const TPtrHeader& Header() const override;
    void SetHeader(const TPtrHeader& value);

    bool IsUp() const;

    TResult LoadFile(const TString& path, bool isCheck);
    virtual TResult LoadData(const TVecData& value);

    TString Info(int index) const;
    virtual void SetInfo(int index, const TString& value);

    double InfoDouble(int index) const;
    virtual void SetInfoDouble(int index, double value);

    static TPtrHeader HeaderFromFile(const TString& path);  //ишет шапку по файлу
    static TResult HeaderFromFile(const TString& path, TPtrHeader& hdr);
    static TContainer CreateFromFile(const TString& path);      //создает контейнер по файлу
    static TContainer LoadFromFile(const TString& path);        //создает и загружает контейнер по файлу
    static TContainer CreateFromVersion(const TString& value);//создает контейнер по версии шапки(если такая версия есть)

    static bool RegisterHeader(TPtrRegHeader&& value);
    static size_t CountRegHeaders();
    static const TPtrRegHeader& RegHeader(int index);

    static TPtrHeader FindHeader(const TString& version);
protected:
    TPtrHeader header;
    static TVecHeader& Headers(){ static TVecHeader headers; return headers; }
};

using TPtrContainer = std::shared_ptr<TContainer>;

class TSelectLoader{
public:
    virtual ~TSelectLoader(){}

    virtual void SetContainer(const TPtrContainer& value, const TString& path)
    {
        cont = value;
        loadable = cont->Header()->LoadableData(path);
        SetIsSelected(true);
    }
    virtual TResult Select()
    {//возвращаем список выбраных кривых
        TVecData res;
        for(const auto& it : loadable)
            if(it->IsUsed()) res.push_back(it);
        return cont->LoadData(res);
    }
    TVecData& Loadable() { return loadable; }

    inline TPtrContainer Cont() const { return cont; }
    TDepthUnit FromConvert() const { return fromConvert; }
    inline double Step() const { return step; };
    inline double Begin() const{ return begin; };
    inline double End() const { return end; };


    TVecBool EditableVector()
    {
        TVecBool rez(loadable.size());
        for(size_t i = 0; i < rez.size(); i++)
            rez[i] = loadable[i]->Category() != ucNone && loadable[i]->IndUnit() == 0;
        return rez;
    }

    bool IsSelected() const
    {//выбрана ли хоть одна кривая
        for(const auto& v : loadable)
            if(v->IsUsed()) return true;
        return false;
    }

    void SetIsSelected(bool value)
    {
        for(const auto& v : loadable)
            v->SetIsUsedNoCall(value);
    }

protected:
    TPtrContainer cont;
    TVecData loadable;
    TDepthUnit fromConvert = duNone; //из чего конвертировать
    double begin = NAN;
    double end = NAN;
    double step = NAN;
};

using TPtrLoader = std::shared_ptr<TSelectLoader>;

template <bool>
struct TRange{
    using TPredFirst = TDoubleCheck::TDoubleGreater;
    using TPredSecond= TDoubleCheck::TDoubleGreaterEq;
    static double StartVal(double begin, double){ return begin; }
    static double StopVal(double, double end){ return end; }
    static const int Last = INT_MIN;
    static int RealInc() { return 1; }
};

template <>
struct TRange<true>{
    using TPredFirst = TDoubleCheck::TDoubleLess;
    using TPredSecond= TDoubleCheck::TDoubleLessEq;
    static double StartVal(double, double end){ return end; }
    static double StopVal(double begin, double){ return begin; }
    static const int Last = INT_MAX;
    static int RealInc() { return -1; }
};

template <typename TPtr, bool isDepthUp>
TVecUInt FindTryIndexes(const TPtr &ptr, double begin, double end, double convCoef, int& result)
{
    typename TRange<isDepthUp>::TPredFirst check;
    typename TRange<isDepthUp>::TPredSecond checkEq;
    double start = TRange<isDepthUp>::StartVal(begin, end);
    double stop = TRange<isDepthUp>::StopVal(begin, end);

    int count = ptr->size();
    TVecUInt indx(count);           //инедксы валидные для загрузки

    int bi = 0;
    for (int i = 1; i < count; i++)
        if (check(ptr->at(i) * convCoef, start))
        {
            bi = i - 1;
            break;
        }
    int realInc = TRange<isDepthUp>::RealInc();
    int realIndex = (realInc == 1) ? 0 : (count - 1);
    double last = TRange<isDepthUp>::Last;
    double temp = 0;
    for (int i = bi; i < count; i++)
    {
        temp = ptr->at(i) * convCoef;
        if (check(temp, last))
        {
            indx[realIndex] = i;
            realIndex = realIndex + realInc;
            last = temp;
            if (checkEq(temp, stop))
                break;
        }
    }
    if constexpr (isDepthUp)
        result = (realIndex + 1) * realInc;
    else
        result = realIndex;//если положительное то реальное количество данных, если отриц то c какого начать
    return indx;
}
template <typename T>
TVecUInt FindTryIndex(bool isUp, T* ptr, double begin, double end, double convCoef, int& first, int& last)
{
    TVecUInt res;
    if(isUp)
        res = FindTryIndexes<T*, true>(ptr, begin, end, convCoef, last);
    else
        res = FindTryIndexes<T*, false>(ptr, begin, end, convCoef, last);
    first = 0;
    if (last <= 0)
    {
        first = -last;
        last = res.size();
    }
    return res;
}

TVecVecDouble NormData(double begin, double end, double step, const TVecVecDouble& data);


#endif //TESTAPP_DATACONTAINER_H
