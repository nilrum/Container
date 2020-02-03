//
// Created by user on 08.10.2019.
//

#ifndef TESTAPP_DATACONTAINER_H
#define TESTAPP_DATACONTAINER_H

#include "PropertyClass.h"

class TDataBase: public TPropertyClass{
public:
    virtual ~TDataBase() = default;

    virtual TString Unit() const = 0;
    virtual void SetUnit(const TString& value){};

    virtual double Value(int index) const = 0;
    virtual void SetValue(int index, double value){};
    virtual size_t CountValue() const = 0;

    TString Title() const { return Name() + (Unit().size() ? ("," + Unit()) : TString()); }

    PROPERTIES_CREATE(TDataBase, TPropertyClass, NO_CREATE(),
        PROPERTY(TString, unit, Unit, SetUnit);
    )
};
using TPtrData = std::shared_ptr<TDataBase>;
using TVecData = std::vector<TPtrData>;

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

    virtual TRezult CheckFile(const TString& path) = 0;        //проверка файла на соответствие формату
    virtual TVecData LoadableData(const TString& path) = 0; //получаем список кривых которые доступны для загрузки из файла
    virtual TRezult LoadData(const TVecData& datas) = 0;       //загружаем список кривых

    static TVecString InitDefTitle();
    static const TVecString& DefTitle(){ static TVecString defTitle = InitDefTitle(); return defTitle; }

};
using TPtrRegHeader = std::unique_ptr<THeaderBase>;
using TVecHeader = std::vector<TPtrRegHeader>;


class TContainer{
public:
    TContainer() = default;
    TContainer(const TPtrHeader& value);

    bool IsValid() const;

    TPtrHeader Header() const;
    TPtrData Data(int index);
    const TPtrData& Data(int index) const;
    size_t CountData() const;

    TRezult LoadFile(const TString& path, bool isCheck);
    TRezult LoadData(const TVecData& value);

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
    TContainer* cont = nullptr;
    TVecData loadable;
    std::vector<bool> selected;
};

using TPtrLoader = std::shared_ptr<TSelectLoader>;
#endif //TESTAPP_DATACONTAINER_H
