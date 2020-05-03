//
// Created by user on 08.10.2019.
//

#ifndef TESTAPP_DATACONTAINER_H
#define TESTAPP_DATACONTAINER_H

#include "PropertyClass.h"
#include "Units.h"

class TDataBase;
using TPtrData = std::shared_ptr<TDataBase>;
using TVecData = std::vector<TPtrData>;

class TUsedClass;
using TPtrUsedClass = std::shared_ptr<TUsedClass>;
using TWPtrUsedClass = std::weak_ptr<TUsedClass>;

using TOnDataEdit = sigslot::signal<>;

class TUsedClass : public TPropertyClass{
public:
    virtual TString FullName() const;
    void SetParent(const TPtrUsedClass& value);
protected:
    TWPtrUsedClass parent;
    virtual void CallUsed(const TPtrData& value);
};

class TDataBase: public TUsedClass{
public:
    virtual ~TDataBase() = default;

    virtual TString Unit() const{ return TString(); };
    virtual void SetUnit(const TString& value){};

    virtual double Key(int index) const{ return 0; };
    virtual void SetKey(int index, double value){};
    virtual double Value(int index, int array = 0) const { return 0; };
    virtual void SetValue(int index, double value, int array = 0){};
    virtual size_t CountValue() const { return 0; };
    virtual size_t CountArray() const { return 1; };

    TString Title() const { return Name() + (Unit().size() ? ("," + Unit()) : TString()); }

    inline double FirstKey() const { return Key(0); }
    inline double LastKey() const { return Key(CountValue() - 1); }

    //TODO удалить методы
    virtual void NormValue( size_t index, double depth, size_t ib, size_t ie, double scale, const TDataBase* data){};
    virtual void ShrinkCount(int value){};

    virtual void Insert(size_t index, const TVecDouble& keyValues = TVecDouble()){};
    virtual void Delete(size_t index, size_t count = 1){};

    virtual void Load(FILE* file) {};
    virtual void Save(FILE* file) {};

    virtual size_t CountOther() const { return 0; };
    virtual const TPtrData& Other(int index) const;
    virtual const TPtrData& AddOther(const TPtrData& value){ return value; };
    virtual void DelOther(const TPtrData& value){};
    virtual const TPtrData& AddDefOther(){ return AddOther(std::make_shared<TDataBase>()); };

    virtual TVecString DefaultTitles() const{ return TVecString(); }
    virtual TVecString DefaultEnumTypes() const{ return TVecString(); }

    TOnDataEdit Edited;

    //vector interface
    inline size_t size() const { return CountValue();};
    inline double at(int index) const { return Key(index); }

    //если есть возможность отдает указатель на данные напрямую
    virtual const double* PtrKey() { return nullptr; };
    virtual const double* PtrValue(int array = 0){ return nullptr; };

    void Assign(const TPtrData& value);

    PROPERTIES_CREATE(TDataBase, TPropertyClass, NO_CREATE(),
        PROPERTY(TString, unit, Unit, SetUnit);
        PROPERTY_READ(size_t, countArray, CountArray);
        PROPERTY(int, tag, Tag, SetTag);
        PROPERTY(bool, isUsed, IsUsed, SetIsUsed).NoSerialization();
    )
    PROPERTY_FUN(int, tag, Tag, SetTag);
    PROPERTY_FUN(TUnitCategory, category, Category, SetCategory);
    PROPERTY_FUN(int, indUnit, IndUnit, SetIndUnit);

    virtual TVecDouble Coefs() const { return TVecDouble(); }
    virtual void SetCoefs(const TVecDouble& value) { }

    virtual double KeyDelta() const { return 0.;}
    virtual void SetKeyDelta(double value) {}

    bool IsUsed() const;
    virtual void SetIsUsed(bool value);
    void SetIsUsedNoCall(bool value);
protected:
    int isUsed = false;
    int tag = 0;
    TUnitCategory category = ucNone;
    int indUnit = 0;
};

enum TIdInfo{iiArea = 0, iiWell, iiDate, iiTime, iiBegin, iiEnd, iiStep, iiCompany, iiServComp, iiCountInfo};

class THeaderBase;
using TPtrHeader = std::shared_ptr<THeaderBase>;

class THeaderBase{
public:
    virtual ~THeaderBase() = default;

    virtual TPtrHeader Clone() = 0;
    virtual TString Version() const = 0;

    virtual TString TitleInfo(int index) const;
    virtual TVariable Info(int index) const = 0;
    virtual void SetInfo(int index, const TVariable& value){};
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

enum class TContainerRezult{ Ok, InvHeader};

class TContainer{
public:
    TContainer() = default;
    TContainer(const TPtrHeader& value);

    bool IsValid() const;

    const TPtrHeader& Header() const;
    void SetHeader(const TPtrHeader& value);
    const TPtrData& Data(int index);
    const TPtrData& Data(int index) const;
    size_t CountData() const;

    bool IsUp() const;

    TResult LoadFile(const TString& path, bool isCheck);
    virtual TResult LoadData(const TVecData& value);

    TString Info(int index) const;
    void SetInfo(int index, const TString& value);

    double InfoDouble(int index) const;
    void SetInfoDouble(int index, double value);

    static TPtrHeader HeaderFromFile(const TString& path);  //ишет шапку по файлу
    static TContainer CreateFromFile(const TString& path);      //создает контейнер по файлу
    static TContainer LoadFromFile(const TString& path);        //создает и загружает контейнер по файлу
    static TContainer CreateFromVersion(const TString& value);//создает контейнер по версии шапки(если такая версия есть)

    static bool RegisterHeader(TPtrRegHeader&& value);
    static size_t CountRegHeaders();
    static const TPtrRegHeader& RegHeader(int index);

    static TPtrHeader FindHeader(const TString& version);
protected:
    TPtrHeader header;
    TVecData data;
    static TVecHeader& Headers(){ static TVecHeader headers; return headers; }
};

using TPtrContainer = std::shared_ptr<TContainer>;

class TSelectLoader{
public:
    virtual void SetContainer(TContainer* value, const TString& path)
    {
        cont = value;
        loadable = cont->Header()->LoadableData(path);
        selected.resize(loadable.size(), true);
    }
    virtual TResult Select()
    {//возвращаем список выбраных кривых
        TVecData rez;
        for(int i = 0; i < selected.size(); i++)
            if(selected[i]) rez.push_back(loadable[i]);
        return cont->LoadData(rez);
    }
    TVecData& Loadable() { return loadable; }
    TVecBool& Selected() { return selected; }

    TDepthUnit FromConvert() const { return fromConvert; }

    TVecBool IsEditable()
    {
        TVecBool rez(selected.size());
        for(size_t i = 0; i < rez.size(); i++)
            rez[i] = loadable[i]->Category() != ucNone && loadable[i]->IndUnit() == 0;
    }

    bool IsSelected() const
    {//выбрана ли хоть одна кривая
        for(auto v : selected)
            if(v == true) return true;
        return false;
    }

protected:
    TContainer* cont = nullptr;//TODO подумать над этим
    TVecData loadable;
    TVecBool selected;
    TDepthUnit fromConvert = duNone; //из чего конвертировать
};

using TPtrLoader = std::shared_ptr<TSelectLoader>;

template <bool>
struct TRange{
    using TPredFirst = TDoubleCheck::TDoubleGreater;
    using TPredSecond= TDoubleCheck::TDoubleGreaterEq;
    static double StartVal(double begin, double end){ return begin; }
    static double StopVal(double begin, double end){ return end; }
    static const int Last = INT_MIN;
    static int RealInc() { return 1; }
};

template <>
struct TRange<true>{
    using TPredFirst = TDoubleCheck::TDoubleLess;
    using TPredSecond= TDoubleCheck::TDoubleLessEq;
    static double StartVal(double begin, double end){ return end; }
    static double StopVal(double begin, double end){ return begin; }
    static const int Last = INT_MAX;
    static int RealInc() { return -1; }
};

template <typename TPtr, bool isDepthUp>
TVecUInt FindTryIndexes(const TPtr &ptr, double begin, double end, double convCoef, int& rezult)
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
        rezult = (realIndex + 1) * realInc;
    else
        rezult = realIndex;//если положительное то реальное количество данных, если отриц то c какого начать
    return indx;
}
template <typename T>
TVecUInt FindTryIndex(bool isUp, T* ptr, double begin, double end, double convCoef, int& rezult)
{
    if(isUp)
        return FindTryIndexes<T*, true>(ptr, begin, end, convCoef, rezult);
    else
        return FindTryIndexes<T*, false>(ptr, begin, end, convCoef, rezult);
}


#endif //TESTAPP_DATACONTAINER_H
