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

THistoryDeleteData::THistoryDeleteData(const TPtrData &value, size_t index, size_t count):
        data(value), indexChg(index), countChg(count)
{
    name = "Interval data removed";
    pathHistory = "D:/History/" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    auto file = OpenFile(pathHistory, TOpenFileMode::Write);
    value->Save(file.get(), index, count);
}

THistoryDeleteData::~THistoryDeleteData()
{
    std::remove(STR(pathHistory));//файл истории удалить после себя
}

void THistoryDeleteData::Back()
{
    if(data.expired()) return;
    auto lock = data.lock();
    auto file = OpenFile(pathHistory, TOpenFileMode::Read);
    lock->Load(file.get(), indexChg, countChg);
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

