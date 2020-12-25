//
// Created by user on 09.10.2019.
//

#ifndef TESTAPP_BINFILE_H
#define TESTAPP_BINFILE_H

#include "Types.h"
#include <cstdio>
#include <memory>
#include <cstring>
#include <functional>

enum class TBinFileResult{ Ok, ErrOpen, ErrVersion };
class TBinFile;
using TPtrBinFile = std::shared_ptr<TBinFile>;
using TCreateBinFile = std::function<TPtrBinFile()>;

class TInspectInfo{
public:
    TInspectInfo(const TString& text = TString());
    void AddInfo(const TString& text);
    void AddError(const TString& text);
    void AddWarrning(const TString& text);
    TVecString Result();
    bool Status() const;
    void SetStatus(bool value);
private:
    bool status = false;
    std::vector<TString> info;
    std::vector<TString> errors;
    std::vector<TString> warrnings;
};

class TBinFile{
public:
    virtual TString Version() const = 0;
    virtual TResult CheckVersion(const TString& path) = 0;
    virtual TResult CheckTextVersion() = 0;
    virtual TResult LoadFile(const TString& path, bool isCheck = true) = 0;

    virtual unsigned char* PtrHeader() = 0;
    virtual unsigned char* PtrData(int index) = 0;
    virtual size_t CountData() const = 0;

    virtual TInspectInfo Inspect(const TString &path, bool isCheckFormat){ return TInspectInfo(); };

    using TMapBinFiles = std::map<TString, TCreateBinFile>;
    static TMapBinFiles& AllBinFiles() { static TMapBinFiles map; return map; }
    static bool RegisterBinFile(const TString& typeName, const TCreateBinFile& fun) { AllBinFiles()[typeName] = fun; return true; }
};

template <typename THeader>
bool CheckTextVersion(const THeader& value, const TString& check)
{
    return TString(value.Ver) == check;
}

template <typename THeader, typename TData>
class TBinFileTemp : public TBinFile{
public:
    using THeaderType = THeader;
    using TDataType = TData;

    TBinFileTemp(const TString& v):version(v)
    {
        std::memset(&header, 0, sizeof(THeader));
    }
    TString Version() const override
    {
        return version;
    }

    TResult CheckVersion(const TString& path) override
    {
        auto file = OpenFile(path);
        if(file.get() == nullptr) return TBinFileResult::ErrOpen;
        THeader h;
        auto read = std::fread(&h, sizeof(THeader), 1, file.get());
        return CheckHeader(h) ? TBinFileResult::Ok : TBinFileResult::ErrVersion;
    }

    TResult CheckTextVersion() override
    {
        return CheckTextVersionHeader(header) ? TBinFileResult::Ok : TBinFileResult::ErrVersion;
    }

    TResult LoadFile(const TString& path, bool isCheck = true) override
    {
        auto file = OpenFile(path);
        if(file.get() == nullptr) return TBinFileResult::ErrOpen;
        auto read = fread(&header, sizeof(THeader), 1, file.get());
        if(isCheck && CheckHeader(header) == false) return TBinFileResult::ErrVersion;

        std::fseek(file.get(), 0, SEEK_END);
        size_t count = std::ftell(file.get());
        data.resize((count - sizeof(THeader))/ sizeof(TData));

        std::fseek(file.get(), sizeof(THeader), SEEK_SET);
        for(size_t i = 0; i < data.size(); ++i)
            read = std::fread(&data[i], sizeof(TData), 1, file.get());
        return TBinFileResult::Ok;
    }

    unsigned char* PtrHeader() override
    {
        return reinterpret_cast<unsigned char*>(&header);
    }

    unsigned char* PtrData(int index) override
    {
        if(data.size()) return reinterpret_cast<unsigned char*>(&data[index]);
        return nullptr;
    }

    size_t CountData() const override { return data.size(); };

    THeader& Header() { return header; }
    const THeader& Header() const{ return header; }

    TData& Data(int index) { return data[index]; }
    const TData& Data(int index) const { return data[index]; }

protected:
    TString version;
    THeader header;
    std::vector<TData> data;

    virtual bool CheckHeader(const THeader& h)
    {
        return CheckTextVersionHeader(h);
    }

    virtual bool CheckTextVersionHeader(const THeader& h)
    {
        return ::CheckTextVersion(h, version);
    }
};

template<typename T>
TString SafeCharString(const T& value)
{
    size_t s = std::size(value);
    TString res(value);
    if(res.size() > s) res.resize(s);
    return res;
};

template <typename T, size_t Size>
void SafeCopyChar(T (&desc)[Size], const TString& src)
{
    size_t s = std::size(desc);
    if(src.size() < s)
        strcpy(desc, src.c_str());
    else
        strncpy(desc, src.c_str(), s - 1);
}


#endif //TESTAPP_BINFILE_H
