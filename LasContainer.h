//
// Created by user on 05.06.2020.
//

#ifndef NEO_LASCONTAINER_H
#define NEO_LASCONTAINER_H

#include "DataContainer.h"
#include "Las.h"

class THeaderLas : public THeaderBase, public TLasReader {
public:
    THeaderLas();
    TPtrHeader Clone() override;
    TString Version() const override;

    TResult CheckFile(const TString &path) override;
    TVecData LoadableData(const TString &path) override; //получаем список кривых которые доступны для загрузки из файла
    TResult LoadData(const TVecData &datas, const TPtrProgress& progress) override;     //загружаем список кривых

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
};

class TDataLas : public TDataBase{
public:
    TDataLas() = default;
    TDataLas(const TString& nameCurve, const TString& unitCurve, const TString& com, const TPtrDepthVector& depthVector);

    double Key(size_t index) const override;
    size_t SetKey(size_t index, double value, bool isSort, TTypeEdit typeEdit) override;
    double Value(size_t index, int array) const override;
    void SetValue(size_t index, double value, int array, TTypeEdit typeEdit) override;
    size_t CountValue() const override;

    TVecDouble::const_iterator BeginKey() const override;
    TVecDouble::const_iterator EndKey() const override;

    TVecDouble::const_iterator BeginValue(size_t array) const override;
    TVecDouble::const_iterator EndValue(size_t array) const override;

    void SwapValue(TVecDouble& value) override;

    PROPERTIES(TDataLas, TDataBase,
        PROPERTY(TString, comment, Comment, SetComment);
    )

    PROPERTY_FUN(TString, comment, Comment, SetComment);
protected:
    TString comment;
    TPtrDepthVector depth;
    TVecDouble values;
};
#endif //NEO_LASCONTAINER_H
