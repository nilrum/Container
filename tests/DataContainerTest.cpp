//
// Created by user on 08.10.2019.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;
#include "DataContainer.h"

enum class TTypeErrors{Ok, Err1, Err2};
REGISTER_CODES(TTypeErrors, Ok, "No errors")
REGISTER_CODES(TTypeErrors, Err1, "Text for Err1")

TEST(Rezult, Init)
{
    TResult r;
    EXPECT_TRUE(r.IsNoError());
    EXPECT_TRUE(r.Is(0));

    TResult r2(TTypeErrors::Ok);
    EXPECT_TRUE(r2.IsNoError());
    EXPECT_FALSE(r2.Is(0));
    EXPECT_TRUE(r2.Is(TTypeErrors::Ok));

    TResult r3(TTypeErrors::Err1);
    EXPECT_FALSE(r3.IsNoError());
    EXPECT_FALSE(r3.Is(0));
    EXPECT_TRUE(r3.Is(TTypeErrors::Err1));

    EXPECT_EQ(TResult::TextError(r3), "Text for Err1");

    TResult i(3);
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

TEST(DataContainer, RegisterHeader)
{
    ASSERT_TRUE(TContainer::CountRegHeaders() > 0);

    TContainer cont(TContainer::RegHeader(0)->Clone());
    EXPECT_TRUE(cont.IsValid());
}

TEST(FindIndex, FindTryIndexes)
{
    TVecDouble depthUp = { 200.1, 190.2, 180.3, 185.4, 170.5};
    TVecDouble depthDown = { 170.1, 180.2, 175.3, 174.4, 200.5};
    int rezult = 0;
    TVecUInt rezUp = FindTryIndex(true, &depthUp, 170., 200., 1, rezult);
    ASSERT_EQ(rezult, -1);
    rezult *= -1;
    EXPECT_EQ(rezUp[rezult + 0], 4);
    EXPECT_EQ(rezUp[rezult + 1], 2);
    EXPECT_EQ(rezUp[rezult + 2], 1);
    EXPECT_EQ(rezUp[rezult + 3], 0);

    TVecUInt rezDown = FindTryIndex(false, &depthDown, 170., 200., 1, rezult);
    ASSERT_EQ(rezult, 3);
    EXPECT_EQ(rezDown[0], 0);
    EXPECT_EQ(rezDown[1], 1);
    EXPECT_EQ(rezDown[2], 4);
}
