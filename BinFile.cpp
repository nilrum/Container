//
// Created by user on 09.10.2019.
//

#include "BinFile.h"

REGISTER_CODES(TBinFileResult,
    TEXT_CODE(ErrOpen, "Error opening file");
    TEXT_CODE(ErrVersion, "Error version file");
)

TInspectInfo::TInspectInfo(const TString &text)
{
    if(text.empty() == false) info.push_back(text);
}

TVecString TInspectInfo::Result()
{
    TVecString res;
    if(info.empty() == false)
        res.insert(res.end(), info.begin(), info.end());
    if(errors.size())
    {
        res.push_back("Errors:");
        res.insert(res.end(), errors.begin(), errors.end());
    }
    if(warrnings.size())
    {
        res.push_back("Warrnings:");
        res.insert(res.end(), warrnings.begin(), warrnings.end());
    }
    res.push_back("");
    return res;
}

void TInspectInfo::AddInfo(const TString &text)
{
    info.push_back(text);
}

void TInspectInfo::AddError(const TString &text)
{
    errors.push_back(text);
}

void TInspectInfo::AddWarrning(const TString &text)
{
    warrnings.push_back(text);
}

bool TInspectInfo::Status() const
{
    return status;
}

void TInspectInfo::SetStatus(bool value)
{
    status = value;
}
