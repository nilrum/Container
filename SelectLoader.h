//
// Created by user on 20.10.2020.
//

#ifndef NEO_SELECTLOADER_H
#define NEO_SELECTLOADER_H

#include "DataContainer.h"

class TPropertyLoader;

class TSelectLoader{
public:
    virtual ~TSelectLoader(){}

    virtual void SetContainer(const TPtrContainer& value, const TString& path);
    virtual TResult Select(const TPtrProgress& progress);

    inline TVecData& Loadable() { return loadable; }

    inline TPtrContainer Cont() const { return cont; }
    inline TDepthUnit FromConvert() const { return fromConvert; }

    inline double Step() const { return step; };
    inline double Begin() const{ return begin; };
    inline double End() const { return end; };

    TVecBool EditableVector();

    bool IsSelected() const;
    void SetIsSelected(bool value);

    bool IsStepChanged() const;

    using TCreateView = std::function<std::shared_ptr<TSelectLoader>()>;
    STATIC_ARG(TCreateView, CreateFunc, [](){ return std::make_shared<TSelectLoader>();})
protected:
    TPtrContainer cont;
    TVecData loadable;
    TDepthUnit fromConvert = duNone; //из чего конвертировать
    double begin = NAN;
    double end = NAN;
    double step = NAN;
    double beforeStep = 0.;
    friend class TPropertyLoader;
};

using TPtrLoader = std::shared_ptr<TSelectLoader>;

//класс обертка, используется для вывода загружаемых кривых диалога
class TPropertyLoader : public TBaseContainer{
public:
    TPropertyLoader(TSelectLoader* load = nullptr);

    PROPERTIES(TPropertyLoader, TBaseContainer,
        PROPERTY_ARRAY_READ(TDataBase, loadable, CountLoadable, Loadable);
    )
    void Init();//инициализация ед изм по умолчанию если необходимо
protected:
    TSelectLoader* loader;

    size_t CountLoadable() const;
    const TPtrData& Loadable(int index) const;

    void CallUsed(const TPtrData& value) override;
};
#endif //NEO_SELECTLOADER_H
