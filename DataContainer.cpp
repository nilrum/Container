//
// Created by user on 08.10.2019.
//

#include "DataContainer.h"
INIT_PROPERTYS(TDataBase)

TContainer::TContainer(const TPtrHeader &value):header(value)
{

}

bool TContainer::IsValid() const
{
    return static_cast<bool>(header);
}

const TPtrHeader& TContainer::Header() const
{
    return header;
}

void TContainer::SetHeader(const TPtrHeader &value)
{
    header = value;
}

const TPtrData& TContainer::Data(int index)
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

TResult TContainer::LoadFile(const TString &path, bool isCheck)
{
    if(IsValid() == false) return TContainerResult::InvHeader;
    if(isCheck)
    {
        TResult r = header->CheckFile(path);
        if(r.IsError()) return r;
    }
    return LoadData(header->LoadableData(path));//по умолчанию загружаем все кривые
}

TResult TContainer::LoadData(const TVecData &value)
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
        if(c.LoadFile(path, false).IsNoError())
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

bool TContainer::IsUp() const
{
    if(data.size() && data[0]->CountValue())
        return data[0]->Key(0) > data[0]->Key(data[0]->CountValue() - 1);
    return false;
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

const TPtrData &TDataBase::Other(int index) const
{
    return Single<TPtrData>();
}

bool TDataBase::IsUsed() const
{
    return isUsed;
}

void TDataBase::SetIsUsed(bool value)
{
    CallUsed(std::dynamic_pointer_cast<TDataBase>(shared_from_this()));
}

void TDataBase::SetIsUsedNoCall(bool value)
{
    isUsed = value;
    Change();
}

void TDataBase::Assign(const TPtrData &value)
{
    SetName(value->Name());
    SetUnit(value->Unit());
    SetCategory(value->Category());
    SetIndUnit(value->IndUnit());
    SetCoefs(value->Coefs());
    SetKeyDelta(value->KeyDelta());
}

//----------------------------------------------------------------------------------------------------------------------
void TUsedClass::SetParent(const TPtrUsedClass &value)
{
    parent = value;
}

TString TUsedClass::FullName() const
{
    TPtrUsedClass p = parent.lock();
    if(p != nullptr) return p->FullName() + "/" + Name();
    else return Name();
}

void TUsedClass::CallUsed(const TPtrData &value)
{
    TPtrUsedClass p = parent.lock();
    if(p) p->CallUsed(value);
}
