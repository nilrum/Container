//
// Created by user on 04.02.2020.
//
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;
#include "DataContainer.h"

#include <filesystem>
namespace fs = std::filesystem;

TEST(DataContainer, ContFromFile)
{
    TString path = "MID_4_4.mdk";
    ASSERT_TRUE(fs::exists(path));

    TContainer cont = TContainer::LoadFromFile(path);
    ASSERT_EQ(cont.IsValid(), true);
    EXPECT_EQ(cont.Header()->Version(), TString("MID4BK"));
    EXPECT_EQ(cont.Info(iiArea), "Площадь");
    EXPECT_EQ(cont.Info(iiWell), TString("МИД-4"));
    EXPECT_EQ(cont.Info(iiDate), "14.08.2019");
    EXPECT_EQ(cont.Info(iiTime), "11:24");
    EXPECT_TRUE(fabs(cont.InfoDouble(iiBegin) - 2154.040039) < 0.0001);
    EXPECT_TRUE(fabs(cont.InfoDouble(iiEnd) - 2151.496826) < 0.0001);
    EXPECT_EQ(cont.InfoDouble(iiStep), 0.0);
    EXPECT_EQ(cont.Info(iiCompany), "");
    EXPECT_EQ(cont.Info(iiServComp), "Оператор");

    ASSERT_EQ(cont.CountChildData(), 8);
    EXPECT_EQ(cont.ChildData(0)->Name(), "IL");
    EXPECT_EQ(cont.ChildData(0)->Unit(), "mA");
    EXPECT_TRUE(cont.ChildData(0)->CountValue());

    EXPECT_EQ(cont.ChildData(1)->Name(), "TOut");
    EXPECT_TRUE(cont.ChildData(1)->CountValue());
}

TEST(DataContainer, StepContFromFile)
{
    TString path = "MID_4_4.mdk";
    ASSERT_TRUE(fs::exists(path));

    TContainer cont = TContainer::CreateFromFile(path);     //пробуем создать контейнер по файлу
    ASSERT_TRUE(cont.IsValid());
    EXPECT_EQ(cont.CountChildData(), 0);

    TVecData loadable = cont.Header()->LoadableData(path);  //загружаем список возможных кривых
    ASSERT_TRUE(loadable.size() > 1);
    loadable.pop_back();                                    //удаляем одну из кривых из загрузки

    ASSERT_TRUE(cont.LoadData(loadable).IsNoError());       //загружаем выраные кривые
    EXPECT_EQ(cont.CountChildData(), loadable.size());

    EXPECT_EQ(cont.ChildData(0)->Name(), "IL");
    EXPECT_TRUE(cont.ChildData(0)->CountValue());
}
