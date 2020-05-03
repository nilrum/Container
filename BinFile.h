//
// Created by user on 09.10.2019.
//

#ifndef TESTAPP_BINFILE_H
#define TESTAPP_BINFILE_H

#include "Types.h"
#include <cstdio>
#include <memory>
#include <cstring>
enum class TBinFileRezult{ Ok, ErrOpen, ErrVersion };

class TBinFile{
public:
    virtual TString Version() const = 0;
    virtual TResult CheckVersion(const TString& path) = 0;
    virtual TResult LoadFile(const TString& path, bool isCheck = true) = 0;

    virtual unsigned char* PtrHeader() = 0;
    virtual unsigned char* PtrData(int index) = 0;
    virtual size_t CountData() const = 0;
};

using TPtrBinFile = std::shared_ptr<TBinFile>;

template <typename THeader>
bool CheckHeader(const THeader& value, const TString& check)
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
    virtual TString Version() const override
    {
        return version;
    }
    TResult CheckVersion(const TString& path) override
    {
        auto file = OpenFile(path);
        if(file.get() == nullptr) return TBinFileRezult::ErrOpen;
        THeader h;
        std::fread(&h, sizeof(THeader), 1, file.get());
        return CheckHeader(h)? TBinFileRezult::Ok : TBinFileRezult::ErrVersion;
    }
    virtual TResult LoadFile(const TString& path, bool isCheck = true) override
    {
        auto file = OpenFile(path);
        if(file.get() == nullptr) return TBinFileRezult::ErrOpen;
        fread(&header, sizeof(THeader), 1, file.get());
        if(isCheck && CheckHeader(header) == false) return TBinFileRezult::ErrVersion;
        size_t count1 = std::ftell(file.get());
        std::fseek(file.get(), 0, SEEK_END);
        size_t count = std::ftell(file.get());
        data.resize((count - sizeof(THeader))/ sizeof(TData));

        std::fseek(file.get(), sizeof(THeader), SEEK_SET);
        for(int i = 0; i < data.size(); ++i)
            std::fread(&data[i], sizeof(TData), 1, file.get());
        return TBinFileRezult::Ok;
    }

    virtual unsigned char* PtrHeader() override
    {
        return reinterpret_cast<unsigned char*>(&header);
    }
    virtual unsigned char* PtrData(int index) override
    {
        if(data.size()) return reinterpret_cast<unsigned char*>(&data[index]);
        return nullptr;
    }

    virtual size_t CountData() const override { return data.size(); };

    THeader& Header() { return header; }
    const THeader& Header() const{ return header; }

    TData& Data(int index) { return data[index]; }
    const TData& Data(int index) const { return data[index]; }

protected:
    TString version;
    THeader header;
    std::vector<TData> data;

    std::shared_ptr<FILE> OpenFile(const TString& path)
    {
        return std::shared_ptr<FILE>(std::fopen(path.c_str(), "rb"), [](FILE* file){ std::fclose(file); });
    }

    virtual bool CheckHeader(const THeader& h)
    {
        return ::CheckHeader(h, version);
    }
};

#endif //TESTAPP_BINFILE_H
