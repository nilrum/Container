//
// Created by user on 08.10.2019.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;
#include "DataContainer.h"
#include "LasContainer.h"


TEST(TBaseContainer, Init)
{
    TBaseContainer cont;
    EXPECT_EQ(cont.FullName(), TString());
    EXPECT_TRUE(cont.Parent().expired());
    EXPECT_EQ(cont.CountChildData(), 0);
}

TEST(TBaseContainer, Childs)
{
    TPtrBaseContainer cont = std::make_shared<TBaseContainer>();
    cont->SetName("Root");
    EXPECT_EQ(cont.FullName(), TString("Root"));
    EXPECT_TRUE(cont.Parent().expired());
    EXPECT_EQ(cont.CountChildData(), 0);


}


TEST(DataContainer, Init)
{
    TContainer cont;
    EXPECT_FALSE(cont.IsValid());
    EXPECT_FALSE(cont.Header());
    EXPECT_EQ(cont.CountChildData(), 0);
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
    int first = 0;
    int last = 0;
    TVecUInt rezUp = FindTryIndex(true, &depthUp, 170., 200., 1, first, last);
    ASSERT_EQ(first, 1);
    ASSERT_EQ(last, 5);
    EXPECT_EQ(rezUp[first + 0], 4);
    EXPECT_EQ(rezUp[first + 1], 2);
    EXPECT_EQ(rezUp[first + 2], 1);
    EXPECT_EQ(rezUp[first + 3], 0);

    TVecUInt rezDown = FindTryIndex(false, &depthDown, 170., 200., 1, first, last);
    EXPECT_EQ(rezDown[first + 0], 0);
    EXPECT_EQ(rezDown[first + 1], 1);
    EXPECT_EQ(rezDown[first + 2], 4);
    ASSERT_EQ(first, 0);
    ASSERT_EQ(last, 3);
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
    EXPECT_EQ(cont.CountChildData(), 5);


    TPtrData data0 = cont.ChildData(0);
    EXPECT_EQ(data0->Name(), "8 5/8 Casing_Th");

    EXPECT_EQ(data0->FirstKey(), -14.59);
    EXPECT_TRUE(isnan(data0->FirstValue()));

    EXPECT_EQ(data0->Key(1), -14.39);
    EXPECT_EQ(data0->Value(1), 0.398);

    EXPECT_EQ(data0->LastKey(), 5281.21);
    EXPECT_EQ(data0->LastValue(), 0.398);

    TPtrData data4 = cont.ChildData(4);
    EXPECT_EQ(data4->Name(), "8 5/8 Casing_ML");

    EXPECT_EQ(data4->FirstKey(), -14.59);
    EXPECT_TRUE(isnan(data4->FirstValue()));

    EXPECT_EQ(data4->Key(1), -14.39);
    EXPECT_EQ(data4->Value(1), 0.);

    EXPECT_EQ(data4->LastKey(), 5281.21);
    EXPECT_EQ(data4->LastValue(), 0.390);
}

TEST(TLasHeader, LoadFile2)
{
    TContainer cont = TContainer::CreateFromFile("2.las");
    EXPECT_TRUE(cont.IsValid());

    TVecData loadable = cont.Header()->LoadableData("2.las");
    EXPECT_EQ(loadable.size(), 15);
    EXPECT_EQ(cont.Header()->DepthUnit(), duMeter);
    EXPECT_TRUE(cont.LoadData(TVecData(loadable.begin() + 2, loadable.end() - 5)).IsNoError());
    EXPECT_EQ(cont.CountChildData(), 8);


    TPtrData data0 = cont.ChildData(0);
    EXPECT_EQ(data0->Name(), "GRC");

    EXPECT_EQ(data0->FirstKey(), 6700.77);
    EXPECT_EQ(data0->FirstValue(), 2.);

    EXPECT_EQ(data0->Key(1), 6700.77);
    EXPECT_EQ(data0->Value(1), 1.);

    EXPECT_EQ(data0->LastKey(), -0.42);
    EXPECT_EQ(data0->LastValue(), 4.);

    TPtrData data7 = cont.ChildData(7);
    EXPECT_EQ(data7->Name(), "ZS10");

    EXPECT_EQ(data7->FirstKey(), 6700.77);
    EXPECT_EQ(data7->FirstValue(), 24998.);

    EXPECT_EQ(data7->Key(1), 6700.77);
    EXPECT_EQ(data7->Value(1), 24999.);

    EXPECT_EQ(data7->LastKey(), -0.42);
    EXPECT_EQ(data7->LastValue(), 26749.);
}