//
// Created by user on 08.10.2019.
//

#ifndef TESTAPP_DATACONTAINER_H
#define TESTAPP_DATACONTAINER_H

#include "PropertyClass.h"
class TDataBase;
using TPtrData = std::shared_ptr<TDataBase>;
using TVecData = std::vector<TPtrData>;

class TUsedClass;
using TPtrUsedClass = std::shared_ptr<TUsedClass>;
using TWPtrUsedClass = std::weak_ptr<TUsedClass>;

class TUsedClass : public TPropertyClass{
public:
    virtual TString FullName() const;
    virtual void CallUsed(const TPtrData& value);
    void SetParent(const TPtrUsedClass& value);
protected:
    TWPtrUsedClass parent;
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

    PROPERTIES_CREATE(TDataBase, TPropertyClass, NO_CREATE(),
        PROPERTY(TString, unit, Unit, SetUnit);
        PROPERTY_READ(size_t, countArray, CountArray);
        PROPERTY(int, tag, Tag, SetTag);
        PROPERTY(bool, isUsed, IsUsed, SetIsUsed).NoSerialization();
    )
    PROPERTY_FUN(int, tag, Tag, SetTag);
    bool IsUsed() const;
    virtual void SetIsUsed(bool value);
    void SetIsUsedNoCall(bool value);
protected:
    int isUsed = false;
    int tag = 0;
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

    virtual TRezult CheckFile(const TString& path) = 0;        //проверка файла на соответствие формату
    virtual TVecData LoadableData(const TString& path) = 0; //получаем список кривых которые доступны для загрузки из файла
    virtual TRezult LoadData(const TVecData& datas) = 0;       //загружаем список кривых

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

    TRezult LoadFile(const TString& path, bool isCheck);
    virtual TRezult LoadData(const TVecData& value);

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
    virtual TRezult Select()
    {
        TVecData rez;
        for(int i = 0; i < selected.size(); i++)
            if(selected[i]) rez.push_back(loadable[i]);
        return cont->LoadData(rez);
    }
    std::vector<bool>& Selected() { return selected; }
protected:
    TContainer* cont = nullptr;//TODO подумать над этим
    TVecData loadable;
    std::vector<bool> selected;
};

using TPtrLoader = std::shared_ptr<TSelectLoader>;

#endif //TESTAPP_DATACONTAINER_H
