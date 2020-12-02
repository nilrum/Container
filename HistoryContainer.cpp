//
// Created by user on 19.11.2020.
//

#include "HistoryContainer.h"
#include <chrono>
#include <cstdio>

THistoryAddData::THistoryAddData(const TPtrData &value, size_t index, const TVecDouble& ins):data(value),
    indexChg(index), insVals(ins)
{
    name = "Data value inserted";
}

void THistoryAddData::Back()
{
    if(data.expired()) return;
    auto lock = data.lock();
    lock->Delete(indexChg);
    lock->OnEdited(TTypeEdit::FullUpdate);
}

void THistoryAddData::Next()
{
    if(data.expired()) return;
    auto lock = data.lock();
    lock->Insert(indexChg, insVals);
    lock->OnEdited(TTypeEdit::FullUpdate);
}
//----------------------------------------------------------------------------------------------------------------------
THistoryFile::THistoryFile()
{
    AddPath();
}

THistoryFile::~THistoryFile()
{
    for(const auto& p : pathHistory)
        std::remove(STR(p));//файл истории удалить после себя
}

TString THistoryFile::PathHistory(size_t index) const
{
    return pathHistory[index];
}

size_t THistoryFile::CountPaths() const
{
    return pathHistory.size();
}

TString THistoryFile::AddPath()
{
    pathHistory.push_back("D:/History/" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    return pathHistory.back();
}

//----------------------------------------------------------------------------------------------------------------------
THistoryDeleteData::THistoryDeleteData(const TPtrData &value, size_t index, size_t count):THistoryFile(),
        data(value), indexChg(index), countChg(count)
{
    name = "Interval data removed";

    auto file = OpenFile(PathHistory(), TOpenFileMode::Write);
    value->Save(file.get(), index, count);
}

void THistoryDeleteData::Back()
{
    if(data.expired()) return;
    auto lock = data.lock();
    auto file = OpenFile(PathHistory(), TOpenFileMode::Read);
    lock->Load(file.get(), indexChg, countChg, false);
    lock->OnEdited(TTypeEdit::UpdateViews);
}

void THistoryDeleteData::Next()
{
    if(data.expired()) return;
    auto lock = data.lock();
    lock->Delete(indexChg, countChg);
    lock->OnEdited(TTypeEdit::UpdateViews);
}
//----------------------------------------------------------------------------------------------------------------------
THistoryValueEditInterval::THistoryValueEditInterval(const TPtrData &value, size_t index, size_t count):
    THistoryDeleteData(value, index, count)
{
    name = "Interval data changed";
}

void THistoryValueEditInterval::Back()
{
    BackNext(0);
}

void THistoryValueEditInterval::Next()
{
    BackNext(1);
}

void THistoryValueEditInterval::BackNext(size_t load)
{
    if(data.expired()) return;
    auto lock = data.lock();
    if(CountPaths() == 1)
    {//сохраняем текущие данные
        auto file = OpenFile(AddPath(), TOpenFileMode::Write);
        lock->Save(file.get(), indexChg, countChg);//TODO проверки сохранения не реализованны
    }
    auto file = OpenFile(PathHistory(load), TOpenFileMode::Read);
    lock->Load(file.get(), indexChg, countChg, true);
    lock->OnEdited(TTypeEdit::UpdateViews);
}

//----------------------------------------------------------------------------------------------------------------------
THistoryDataEdit::THistoryDataEdit(const TPtrData &value, size_t index, size_t array):data(value), indexChg(index), indArray(array)
{
    if(IsKey())
    {
        oldValue = value->Key(index);
        indexChg = -1;//сбрасываем что бы проверить что новый установлен
        name = "Key changed";
    }
    else
    {
        oldValue = value->Value(index, indArray);
        name = "Value changed";
    }

}

void THistoryDataEdit::Back()
{
    if(data.expired()) return;
    auto lock = data.lock();
    if(IsKey())
    {
        if(indexChg == -1) return;//новый индекс для ключа не был указан
        double buffer = lock->Key(indexChg);
        indexChg = lock->SetKey(indexChg, oldValue);
        oldValue = buffer;
    }
    else
    {
        double buffer = lock->Value(indexChg, indArray);
        lock->SetValue(indexChg, oldValue, indArray);
        oldValue = buffer;
    }
}

void THistoryDataEdit::Next()
{
    Back();
}

