//
// Created by user on 20.10.2020.
//

#include "SelectLoader.h"

INIT_PROPERTYS(TPropertyLoader)

void TSelectLoader::SetContainer(const TPtrContainer& value, const TString& path)
{
    cont = value;
    loadable = cont->Header()->LoadableData(path);
    SetIsSelected(true);
    beforeStep = cont->InfoDouble(iiStep);
    if(beforeStep == 0)
        cont->SetInfoDouble(iiStep, TUnits::Customs()->CurProfile()->DefaultStep());
}

TResult TSelectLoader::Select(const TPtrProgress& progress)
{//возвращаем список выбраных кривых
    TVecData res;
    for(const auto& it : loadable)
        if(it->IsUsed()) res.push_back(it);
    return cont->LoadData(res, progress);
}

TVecBool TSelectLoader::EditableVector()
{
    TVecBool rez(loadable.size());
    for(size_t i = 0; i < rez.size(); i++)
        rez[i] = loadable[i]->IsUnitEditable();
    return rez;
}

bool TSelectLoader::IsSelected() const
{//выбрана ли хоть одна кривая
    for(const auto& v : loadable)
        if(v->IsUsed()) return true;
    return false;
}

void TSelectLoader::SetIsSelected(bool value)
{
    for(const auto& v : loadable)
        v->SetIsUsedNoCall(value);
}

bool TSelectLoader::IsStepChanged() const
{
    return beforeStep != cont->InfoDouble(iiStep);
}

//----------------------------------------------------------------------------------------------------------------------
void TPropertyLoader::Init()
{
    const TVecData& loadable = loader->loadable;
    TPtrBaseContainer par = std::dynamic_pointer_cast<TBaseContainer>(shared_from_this());
    for(size_t i = 0; i < loadable.size(); i++)
    {
        const TPtrData& l = loadable[i];
        l->SetParent(par);
        if(l->IsUnitEditable())
            l->SetIndUnit(TUnits::Customs()->CurProfile()->FromCategory(l->Category()));//ставим значение по умолчанию
    }
}

TPropertyLoader::TPropertyLoader(TSelectLoader *load):loader(load)
{

}

void TPropertyLoader::CallUsed(const TPtrData &value)
{
    value->SetIsUsedNoCall(!value->IsUsed());
}

const TPtrData &TPropertyLoader::Loadable(int index) const
{
    if(loader)
        return loader->loadable[index];
    return Single<TPtrData>();
}

size_t TPropertyLoader::CountLoadable() const
{
    if(loader)
        return loader->loadable.size();
    return 0;
}
