//
// Created by user on 08.10.2019.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;
#include "DataContainer.h"
#include "FileSystem.h"

enum class TTypeErrors{Ok, Err1, Err2};
REGISTER_CODES(TTypeErrors, Ok, "No errors")
REGISTER_CODES(TTypeErrors, Err1, "Text for Err1")

TEST(Rezult, Init)
{
    TRezult r;
    EXPECT_TRUE(r.IsNoError());
    EXPECT_TRUE(r.Is(0));

    TRezult r2(TTypeErrors::Ok);
    EXPECT_TRUE(r2.IsNoError());
    EXPECT_FALSE(r2.Is(0));
    EXPECT_TRUE(r2.Is(TTypeErrors::Ok));

    TRezult r3(TTypeErrors::Err1);
    EXPECT_FALSE(r3.IsNoError());
    EXPECT_FALSE(r3.Is(0));
    EXPECT_TRUE(r3.Is(TTypeErrors::Err1));

    EXPECT_EQ(TRezult::TextError(r3), "Text for Err1");

    TRezult i(3);
    EXPECT_EQ(i.Code(), 3);
    EXPECT_FALSE(i.IsNoError());
}

TEST(DataContainer, Init)
{
    TContainer cont;
    EXPECT_FALSE(cont.IsValid());
    EXPECT_FALSE(cont.Header());
    EXPECT_EQ(cont.CountData(), 0);
}

/*class TMockHeader : public THeaderBase{
public:
    MOCK_METHOD(TString, Version, (), (const override));

    MOCK_METHOD(TString, Info, (int index), (const override));
    MOCK_METHOD(void, SetInfo, (int index, const TString& value), ());
    MOCK_METHOD(size_t, CountInfo, (), (const override));

    MOCK_METHOD(bool, CheckFile,(const TString& path), (override));
    MOCK_METHOD(TVecData, LoadedData, (), (override));
};*/

TEST(DataContainer, RegisterHeader)
{
    ASSERT_TRUE(TContainer::CountRegHeaders() > 0);

    TContainer cont(TContainer::RegHeader(0)->Clone());
    EXPECT_TRUE(cont.IsValid());

}

