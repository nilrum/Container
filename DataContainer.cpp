//
// Created by user on 08.10.2019.
//

#include "DataContainer.h"
INIT_PROPERTYS(TDataBase)

TContainer::TContainer(const TPtrHeader &value):header(value)
{

}

TString TContainer::Version() const
{
    return header->Version();
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
    return LoadData(header->LoadableData(path), TPtrProgress());//по умолчанию загружаем все кривые
}

TResult TContainer::LoadData(const TVecData &value, const TPtrProgress& progress)
{
    childData = value;
    if(weak_from_this().expired() == false)
        for(const auto& child : childData)
            child->SetParent(std::dynamic_pointer_cast<TBaseContainer>(shared_from_this()));
    return header->LoadData(childData, progress);
}

TPtrHeader TContainer::HeaderFromFile(const TString& path)
{
    for(const TPtrRegHeader& h : Headers())
        if (h->CheckFile(path).IsNoError()) return h->Clone();
    return TPtrHeader();
}

TResult TContainer::HeaderFromFile(const TString &path, TPtrHeader &hdr)
{
    TResult res(TContainerResult::InvHeader);
    for(const TPtrRegHeader& h : Headers())
    {
        res = h->CheckFile(path);
        if (res.IsNoError())
        {
            hdr = h->Clone();
            break;
        }
    }
    return res;
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

TString TContainer::Info(size_t index) const
{
    return header->Info(index);
}

void TContainer::SetInfo(size_t index, const TString &value)
{
    header->SetInfo(index, value);
}

double TContainer::InfoDouble(size_t index) const
{
    return header->Info(index);
}

void TContainer::SetInfoDouble(size_t index, double value)
{
    header->SetInfo(index, value);
}

bool TContainer::IsUp() const
{
    if(childData.size() && childData.front()->CountValue())
        return childData.front()->FirstKey() > childData.front()->LastKey();
    return false;
}

void TContainer::SetVersion(const TString &value)
{
    header->SetVersion(value);
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
    SetComment(value->Comment());
    SetTag(value->Tag());
    SetCategory(value->Category());
    SetIndUnit(value->IndUnit());
    SetCoefs(value->Coefs());
    SetKeyDelta(value->KeyDelta());
}

int TDataBase::IndUnit() const
{
    return indUnit;
}

void TDataBase::SetIndUnit(int value)
{
    indUnit = value;
}

TVariable TDataBase::Unit() const
{
    if(category == ucNone || indUnit == 0)
        return unit;
    else
        return Single<TUnitProfile>().FromCategory(category).SetIndex(indUnit);
}

void TDataBase::SetUnit(const TVariable &value)
{
    if(category == ucNone)
        unit = value.ToString();
    else
        indUnit = int(value.GetEnum().Index());
}

TString TDataBase::Title() const
{
    TString u = Unit();
    return Name() + (u.empty() ? TString() : ("," + u));
}

TFindResultIter TDataBase::FindLowerKey(double value)
{
    TFindResultIter res {BeginKey(), EndKey()};
    res.res = std::lower_bound(res.begin, res.end, value);
    if(res.res != res.begin) res.res--;
    return res;
}

TFindResultIter TDataBase::FindUpperKey(double value)
{
    TFindResultIter res {BeginKey(), EndKey()};
    res.res = std::upper_bound(res.begin, res.end, value);
    return res;
}

void TDataBase::ApplyScaleDeltaKey(double scale, double delta, TTypeEdit typeEdit, double start, double stop,
                                   double oldDelta, double oldScale)
{
    ScaleDeltaKey(scale, delta, typeEdit,   std::isnan(start) ? -1 : FindLowerKey(start).Index(),
                                            std::isnan(stop) ? -1 : FindUpperKey(stop).Index(),
                                            oldDelta, oldScale);
}

//---------------------------------------------------------------------------------------------------------------
TString TBaseContainer::FullName() const
{
    TPtrBaseContainer p = parent.lock();
    if(p != nullptr) return p->FullName() + "/" + Name();
    else return Name();
}

void TBaseContainer::SetName(const TString &value)
{
    bool chg = name != value;
    TPropertyClass::SetName(value);
    if(chg) OnNameChanged();
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

TPtrBaseContainer TBaseContainer::ChildContainer(size_t index) const
{
    return ChildData(index);
}


const TPtrData &TBaseContainer::AddChildData(const TPtrData &value)
{
    childData.emplace_back(value);
    if(weak_from_this().expired() == false)
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
    TVecString path = Split(pathData, '/');
    return FindDataPath(path, 0, path.size() > 1);
}

TPtrData TBaseContainer::FindDataPath(const TVecString &path, size_t pos, bool isThis)
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

TString TBaseContainer::Comment() const
{
    return comment;
}

void TBaseContainer::SetComment(const TString &value)
{
    comment = value;
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

    while (depthInd < newCount)
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
            if(i >= depth.size()) break;
            db = depth[i - 1];    //начальная глубина для точки
            de = depth[i];        //конечная глубина
        }
    }

    if(depthInd < newCount)//если данных не хватило для нормировки удалим конец
        for(auto& norm : dataNorm)
            norm.erase(norm.begin() + depthInd, norm.end());

    return dataNorm;
}

TVecVecDouble NormDataNan(double begin, double end, double step, const TVecData &dataVec, double null, const TPtrProgress& progress)
{
    size_t normCount = int(1 + (end - begin) / step);

    size_t countArray = 1;//глубина
    for(const auto& d : dataVec)
        countArray += d->CountArray();

    if(progress)
    {
        progress->SetMax(countArray);
        progress->SetTypeProgress(TProgress::tpStep);
    }

    TVecVecDouble normVec(countArray, TVecDouble(normCount));
    TVecDouble& depth = normVec[0];
    size_t i = 1;
    for(const auto& data : dataVec)
    {
        for (size_t array = 0; array < data->CountArray(); array++)
        {
            auto &norm = normVec[i++];

            auto beginKey = data->BeginKey();
            auto endKey = data->EndKey();

            auto beginVal = data->BeginValue(array);

            double b = begin;
            size_t numNorm = 0;
            size_t numData = 0;
            for (; numNorm < normCount; numNorm++)
            {
                if (beginKey + numData == endKey || b < beginKey[numData])
                {
                    norm[numNorm] = null;
                    depth[numNorm] = b;
                }
                else
                {
                    numData++;//переходим к следующей точке
                    break;
                }
                b += step;
            }
            while (numNorm < normCount && beginKey + numData != endKey)
            {
                if (b > beginKey[numData])//если b больше текущих данных
                {
                    numData++;//переходим к следующим
                }
                else
                {
                    double scale = (b - beginKey[numData - 1]) / (beginKey[numData] - beginKey[numData - 1]);
                    norm[numNorm] = beginVal[numData - 1] + (beginVal[numData] - beginVal[numData - 1]) * scale;
                    depth[numNorm] = b;
                    numNorm++;
                    b += step;
                }
            }
            for (; numNorm < normCount; numNorm++)
            {
                norm[numNorm] = null;
                depth[numNorm] = b;
                b += step;
            }
        }
        if(progress) progress->Progress(1);
    }
    if(progress) progress->Finish();
    return normVec;
}

//----------------------------------------------------------------------------------------------------------------------
void TSimpleData::Set(TVecDouble *keys, TVecDouble *vals, int countArray, bool isSwap, TTypeEdit typeEdit)
{
    key = *keys;
    arrays.resize(countArray);
    for(auto& a : arrays)
    {
        a = *vals;
        vals++;
    }
}

TVecDouble::const_iterator TSimpleData::BeginKey() const
{
    return key.begin();
}

TVecDouble::const_iterator TSimpleData::EndKey() const
{
    return key.end();
}

TVecDouble::const_iterator TSimpleData::BeginValue(size_t array) const
{
    if(array < arrays.size())
        return arrays[array].begin();
    return TVecDouble::const_iterator();
}

TVecDouble::const_iterator TSimpleData::EndValue(size_t array) const
{
    if(array < arrays.size())
        return arrays[array].end();
    return TVecDouble::const_iterator();
}
