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
    childData = value;
    for(const auto& child : childData)
        child->SetParent(std::dynamic_pointer_cast<TBaseContainer>(shared_from_this()));
    return header->LoadData(childData);
}

TPtrHeader TContainer::HeaderFromFile(const TString& path)
{
    for(const TPtrRegHeader& h : Headers())
        if (h->CheckFile(path).IsNoError()) return h->Clone();//TODO нужно возвращать результат открытия
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
    if(childData.size() && childData.front()->CountValue())
        return childData.front()->FirstKey() > childData.front()->LastKey();
    return false;
}

TVecString THeaderBase::InitDefTitle()
{
    return {"Area", "Well", "Date", "Time", "Begin depth", "End depth", "Step depth", "Company", "Service company"};
}


TString THeaderBase::TitleInfo(size_t index) const
{
    if(index < iiCountInfo)
        return DefTitle()[index];
    return TString();
}

size_t THeaderBase::CountInfo() const
{
    return iiCountInfo;
}
//---------------------------------------------------------------------------------------------------------------
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
    OnChanged();
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

TEnum TDataBase::IndUnit() const
{
    return UNIT->FromCategory(category).SetIndex(indUnit);
}

void TDataBase::SetIndUnitEn(const TEnum &value)
{
    indUnit = value;
    SetUnit(UNIT->FromCategory(category).Names()[indUnit]);
}

void TDataBase::SetIndUnit(int value)
{
    indUnit = value;
}

void TDataBase::SetName(const TString &value)
{
    bool chg = name != value;
    TPropertyClass::SetName(value);
    if(chg) OnNameChanged();
}

//---------------------------------------------------------------------------------------------------------------
TString TBaseContainer::FullName() const
{
    TPtrBaseContainer p = parent.lock();
    if(p != nullptr) return p->FullName() + "/" + Name();
    else return Name();
}

const TWPtrBaseContainer &TBaseContainer::Parent() const
{
    return parent;
}

void TBaseContainer::SetParent(const TWPtrBaseContainer &value)
{
    parent = value;
}

size_t TBaseContainer::CountChildData() const
{
    return childData.size();
}

const TPtrData &TBaseContainer::ChildData(size_t index) const
{
    if(index < childData.size())
        return childData[index];
    return Single<TPtrData>();
}

const TPtrData &TBaseContainer::AddChildData(const TPtrData &value)
{
    childData.emplace_back(value);
    value->SetParent(std::dynamic_pointer_cast<TBaseContainer>(shared_from_this()));
    OnChanged();
    return childData.back();
}

void TBaseContainer::DelChildData(const TPtrData &value)
{
    if(RemoveValFor(childData, value))
    {
        value->SetParent(TWPtrBaseContainer());
        OnChanged();
    }
}

void TBaseContainer::CallUsed(const TPtrData &value)
{
    TPtrBaseContainer p = parent.lock();
    if(p) p->CallUsed(value);
}

TPtrData TBaseContainer::FindData(const TString &pathData)
{
    return FindDataPath(Split(pathData, '/'), 0, true);
}

TPtrData TBaseContainer::FindDataPath(const TVecString &path, int pos, bool isThis)
{
    if(isThis)
        return FindDataThis(path, pos, childData);
    else
        return FindDataChild(path, pos, childData);
}

TPtrBaseContainer TBaseContainer::LockParent() const
{
    return parent.lock();
}

TVecData TBaseContainer::FindDataPred(const TFindPred &pred)
{
    return FindDataPredThis(childData, pred);
}

const TPtrHeader &TBaseContainer::Header() const
{
    TPtrBaseContainer par = parent.lock();
    if(par) return par->Header();
    return Single<TPtrHeader>();
}

TVecVecDouble NormData(double begin, double end, double step, const TVecVecDouble& data)
{
    size_t newCount = int(1 + (end - begin) / step);
    TVecVecDouble dataNorm(data.size(), TVecDouble(newCount));
    size_t depthInd = 0;
    size_t i = 1;
    const TVecDouble& depth = data[0];
    double db = depth[i - 1];    //начальная глубина для точки
    double de = depth[i];        //конечная глубина

    while (i < depth.size() && depthInd < newCount)
    {
        if (TDoubleCheck::Less(begin, de))
        {
            double scale = (begin - db) / (de - db);
            dataNorm[0][depthInd] = begin;
            for (size_t j = 1; j < dataNorm.size(); j++)
                dataNorm[j][depthInd] = data[j][i - 1] + (data[j][i] - data[j][i - 1]) * scale;
            depthInd++;
            begin = begin + step;
            if (TDoubleCheck::GreatEq(begin, end)) break;
        }
        else
        {
            i++;
            db = depth[i - 1];    //начальная глубина для точки
            de = depth[i];        //конечная глубина
        }
    }
    return dataNorm;
}