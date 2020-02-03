//
// Created by user on 08.10.2019.
//

#include "DataContainer.h"

TContainer::TContainer(const TPtrHeader &value):header(value)
{

}

bool TContainer::IsValid() const
{
    return static_cast<bool>(header);
}

TPtrHeader TContainer::Header() const
{
    return header;
}

TPtrData TContainer::Data(int index)
{
    return data[index];
}

const TPtrData& TContainer::Data(int index) const
{
    return data[index];
}

size_t TContainer::CountData() const
{
    return data.size();
}

TRezult TContainer::LoadFile(const TString &path, bool isCheck)
{
    if(IsValid() == false) return false;
    if(isCheck)
    {
        TRezult r = header->CheckFile(path);
        if(r.IsError()) return r;
    }
    return LoadData(header->LoadableData(path));//по умолчанию загружаем все кривые
}

TRezult TContainer::LoadData(const TVecData &value)
{
    data = value;
    return header->LoadData(data);
}

TPtrHeader TContainer::HeaderFromFile(const TString& path)
{
    for(const TPtrRegHeader& h : Headers())
        if (h->CheckFile(path).IsNoError()) return h->Clone();
    return TPtrHeader();
}

TContainer TContainer::CreateFromFile(const TString &path)
{
    return TContainer(HeaderFromFile(path));
}

TContainer TContainer::LoadFromFile(const TString &path)
{
    TPtrHeader h = HeaderFromFile(path);
    if(h)
    {
        TContainer c(h);
        c.LoadFile(path, false);
        return c;
    }

    return TContainer();
}

TContainer TContainer::CreateFromVersion(const TString &value)
{
    TPtrHeader h = FindHeader(value);
    if(h) return TContainer(h);
    else return TContainer();
}

bool TContainer::RegisterHeader(TPtrRegHeader&& value)
{
    Headers().emplace_back(std::move(value));
    return true;
}

size_t TContainer::CountRegHeaders()
{
    return Headers().size();
}

const TPtrRegHeader &TContainer::RegHeader(int index)
{
    return Headers()[index];
}

TPtrHeader TContainer::FindHeader(const TString &version)
{
    for(const TPtrRegHeader& val : Headers())
        if(val->Version() == version) return val->Clone();
    return TPtrHeader();
}

TString TContainer::Info(int index) const
{
    return header->Info(index);
}

void TContainer::SetInfo(int index, const TString &value) 
{
    header->SetInfo(index, value);
}

double TContainer::InfoDouble(int index) const
{
    return header->Info(index);
}

void TContainer::SetInfoDouble(int index, double value)
{
    header->SetInfo(index, value);
}

TVecString THeaderBase::InitDefTitle()
{
    return {"Area", "Well", "Date", "Time", "Begin depth", "End depth", "Step depth", "Company", "Service company"};
}


TString THeaderBase::TitleInfo(int index) const
{
    if(index < iiCountInfo)
        return DefTitle()[index];
    return TString();
}

size_t THeaderBase::CountInfo() const
{
    return iiCountInfo;
}
