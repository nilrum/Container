//
// Created by user on 19.11.2020.
//

#ifndef NEO_HISTORYCONTAINER_H
#define NEO_HISTORYCONTAINER_H

#include "DataContainer.h"
#include "History.h"


class THistoryAddData : public THistoryItem {
public:
    THistoryAddData(const TPtrData& value, size_t indexChg, const TVecDouble& ins);
    void Back() override;
    void Next() override;
private:
    TWPtrData data;
    size_t indexChg;
    TVecDouble insVals;
};

class THistoryDeleteData : public THistoryItem{
public:
    THistoryDeleteData(const TPtrData& value, size_t index, size_t count);
    ~THistoryDeleteData();
    void Back() override;
    void Next() override;

private:
    TWPtrData data;
    size_t indexChg;
    size_t countChg;
    TString pathHistory;
};

class THistoryDataEdit : public THistoryItem{
public:
    THistoryDataEdit(const TPtrData& value, size_t index, size_t array = -1);
    void Back() override;
    void Next() override;

    void SetNewIndex(size_t value){ indexChg = value; }
private:
    TWPtrData data;
    double oldValue;
    size_t indArray = -1;
    size_t indexChg;
    inline bool IsKey() const { return indArray == -1; }
};



#endif //NEO_HISTORYCONTAINER_H
