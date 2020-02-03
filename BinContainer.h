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

    virtual TRezult CheckFile(const TString& path) override;
    virtual TVecData LoadableData(const TString& path) override; //получаем список кривых которые доступны для загрузки из файла
    virtual TRezult LoadData(const TVecData& datas) override;       //загружаем список кривых

protected:
    std::function<TPtrBinFile()> createFile;
    void Copy(char* ptr, const TVariable& value);
    void Copy(float& ptr, const TVariable& value);
};
template <typename T>
class TDataBin : public TDataBase{
public:
    TDataBin(const TString &n, const TString &u, size_t o, TPtrBinFile f): unit(u), offset(o), file(f){ name = n; }

    virtual TString Unit() const override
    {
        return unit;
    }

    virtual void SetUnit(const TString& value) override
    {
        unit = value;
    }

    virtual double Value(int index) const override
    {
        return  *(reinterpret_cast<T*>(file->PtrData(index) + offset));
    }

    virtual void SetValue(int index, double value) override
    {
        *(reinterpret_cast<T*>(file->PtrData(index) + offset)) = value;
    }

    virtual size_t CountValue() const override
    {
        return file->CountData();
    }

protected:
    TString title;      //название кривой
    TString unit;       //ед. измерения кривой
    size_t offset = 0;      //смещение относительно начало структуры
    std::function<double(unsigned char*, int)> get;
    std::function<void(unsigned char*, int, double)> set;
    TPtrBinFile file;   //данные файла
};

TString DosToUtf8(const TString& dos);
TString Cp866ToCp1251(const TString& dos);
TString Cp1251ToUt8(const TString& val);

#endif //TESTAPP_BINCONTAINER_H
