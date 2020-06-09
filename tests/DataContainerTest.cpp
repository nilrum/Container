//
// Created by user on 08.10.2019.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;
#include "DataContainer.h"
#include "LasContainer.h"

enum class TTypeErrors{Ok, Err1, Err2};
REGISTER_CODES(TTypeErrors, Ok, "No errors")
REGISTER_CODES(TTypeErrors, Err1, "Text for Err1")

TEST(Result, Init)
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
    int result = 0;
    TVecUInt rezUp = FindTryIndex(true, &depthUp, 170., 200., 1, result);
    ASSERT_EQ(result, -1);
    result *= -1;
    EXPECT_EQ(rezUp[result + 0], 4);
    EXPECT_EQ(rezUp[result + 1], 2);
    EXPECT_EQ(rezUp[result + 2], 1);
    EXPECT_EQ(rezUp[result + 3], 0);

    TVecUInt rezDown = FindTryIndex(false, &depthDown, 170., 200., 1, result);
    ASSERT_EQ(result, 3);
    EXPECT_EQ(rezDown[0], 0);
    EXPECT_EQ(rezDown[1], 1);
    EXPECT_EQ(rezDown[2], 4);
}


TEST(TLas, InitLas)
{
    TLas las;
    EXPECT_EQ(las.LasVersion(), 2.0);
}

TEST(TLas, ReadHeader)
{
    TLas las;
    EXPECT_TRUE(las.Read("0.las").IsNoError());
    EXPECT_EQ(las.IsWrap(), false);
    EXPECT_EQ(las.NullValue(), 9999.99);
    EXPECT_EQ(las.CountCurves(), 8);
    EXPECT_EQ(las.LasCurve(0).size(), 1);
    EXPECT_TRUE(las.Read("0.las", true).IsNoError());
    EXPECT_EQ(las.CountCurves(), 8);
    EXPECT_TRUE(las.LasCurve(0).size() > 1);
}

TEST(TLasHeader, CheckFile)
{
    THeaderLas header;
    EXPECT_TRUE(header.CheckFile("0.las").IsNoError());
    EXPECT_TRUE(header.CountInfo() > iiCountInfo);
    EXPECT_EQ(header.Info(iiBegin).ToDouble(), 68.);
}

TEST(TLasHeader, LoadFile0)
{
    THeaderLas header;
    TVecData loadable = header.LoadableData("0.las");
    EXPECT_EQ(loadable.size(), 7);
    EXPECT_EQ(loadable[0]->Name(), "MM");
    EXPECT_EQ(header.DepthUnit(), duMeter);
    EXPECT_TRUE(header.LoadData(loadable).IsNoError());
}

TEST(TLasHeader, LoadFile1)
{
    TContainer cont = TContainer::CreateFromFile("1.las");
    EXPECT_TRUE(cont.IsValid());

    TVecData loadable = cont.Header()->LoadableData("1.las");
    EXPECT_EQ(loadable.size(), 8);
    EXPECT_EQ(cont.Header()->DepthUnit(), duFoot);
    EXPECT_TRUE(cont.LoadData(TVecData(loadable.begin() + 2, loadable.end() - 1)).IsNoError());
    EXPECT_EQ(cont.CountData(), 5);


    TPtrData data0 = cont.Data(0);
    EXPECT_EQ(data0->Name(), "8 5/8 Casing_Th");

    EXPECT_EQ(data0->FirstKey(), -14.59);
    EXPECT_TRUE(isnan(data0->FirstValue()));

    EXPECT_EQ(data0->Key(1), -14.39);
    EXPECT_EQ(data0->Value(1), 0.398);

    EXPECT_EQ(data0->LastKey(), 5281.21);
    EXPECT_EQ(data0->LastValue(), 0.398);

    TPtrData data4 = cont.Data(4);
    EXPECT_EQ(data4->Name(), "8 5/8 Casing_ML");

    EXPECT_EQ(data4->FirstKey(), -14.59);
    EXPECT_TRUE(isnan(data4->FirstValue()));

    EXPECT_EQ(data4->Key(1), -14.39);
    EXPECT_EQ(data4->Value(1), 0.);

    EXPECT_EQ(data4->LastKey(), 5281.21);
    EXPECT_EQ(data4->LastValue(), 0.390);
}