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
    EXPECT_EQ(cont->FullName(), TString("Root"));
    EXPECT_TRUE(cont->Parent().expired());
    EXPECT_EQ(cont->CountChildData(), 0);
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

TEST(NormDataNan, OneData)
{
    TVecDouble depth{4., 6., 8. };
    TVecDouble vals{8., 12., 16. };
    /*std::shared_ptr<TSimpleData> simple = std::make_shared<TSimpleData>();
    simple->Set(&depth, &vals, 1, false, TTypeEdit::NoUpdate);

    TVecVecDouble res = NormDataNan(1., 10., 1., {simple}, NAN);*/

    TVecNormInfo vec = {{depth.begin(), depth.end(), vals.begin()}};
    TVecVecDouble res = NormLas(vec, 1., 10., 1.);

    ASSERT_EQ(res.size(), 2);//глубина и нормированное значение

    EXPECT_EQ(res[0].size(), 10);
    EXPECT_EQ(res[0][0], 1.);
    EXPECT_EQ(res[0][1], 2.);
    EXPECT_EQ(res[0][2], 3.);
    EXPECT_EQ(res[0][3], 4.);
    EXPECT_EQ(res[0][4], 5.);
    EXPECT_EQ(res[0][5], 6.);
    EXPECT_EQ(res[0][6], 7.);
    EXPECT_EQ(res[0][7], 8.);
    EXPECT_EQ(res[0][8], 9.);
    EXPECT_EQ(res[0][9], 10.);

    EXPECT_EQ(res[1].size(), 10);
    EXPECT_TRUE(std::isnan(res[1][0]));
    EXPECT_TRUE(std::isnan(res[1][1]));
    EXPECT_TRUE(std::isnan(res[1][2]));
    EXPECT_EQ(res[1][3], 8.);
    EXPECT_EQ(res[1][4], 10);
    EXPECT_EQ(res[1][5], 12);
    EXPECT_EQ(res[1][6], 14);
    EXPECT_EQ(res[1][7], 16);
    EXPECT_TRUE(std::isnan(res[1][8]));
    EXPECT_TRUE(std::isnan(res[1][9]));
}



TEST(NormDataNan, OneArrayData)
{
    TVecDouble depth{4., 6., 8. };
    TVecVecDouble vals{{8., 12., 16. }, {16., 24., 32.}};
    /*std::shared_ptr<TSimpleData> simple = std::make_shared<TSimpleData>();
    simple->Set(&depth, &vals[0], 2, false, TTypeEdit::NoUpdate);

    TVecVecDouble res = NormDataNan(1., 10., 1., {simple}, NAN);*/

    TVecNormInfo vec = {
            {depth.begin(), depth.end(), vals[0].begin()},
            {depth.begin(), depth.end(), vals[1].begin()}
    };
    TVecVecDouble res = NormLas(vec, 1., 10., 1.);

    ASSERT_EQ(res.size(), 3);//глубина и нормированные значения

    EXPECT_EQ(res[0].size(), 10);
    EXPECT_EQ(res[0][0], 1.);
    EXPECT_EQ(res[0][1], 2.);
    EXPECT_EQ(res[0][2], 3.);
    EXPECT_EQ(res[0][3], 4.);
    EXPECT_EQ(res[0][4], 5.);
    EXPECT_EQ(res[0][5], 6.);
    EXPECT_EQ(res[0][6], 7.);
    EXPECT_EQ(res[0][7], 8.);
    EXPECT_EQ(res[0][8], 9.);
    EXPECT_EQ(res[0][9], 10.);

    EXPECT_EQ(res[1].size(), 10);
    EXPECT_TRUE(std::isnan(res[1][0]));
    EXPECT_TRUE(std::isnan(res[1][1]));
    EXPECT_TRUE(std::isnan(res[1][2]));
    EXPECT_EQ(res[1][3], 8.);
    EXPECT_EQ(res[1][4], 10);
    EXPECT_EQ(res[1][5], 12);
    EXPECT_EQ(res[1][6], 14);
    EXPECT_EQ(res[1][7], 16);
    EXPECT_TRUE(std::isnan(res[1][8]));
    EXPECT_TRUE(std::isnan(res[1][9]));

    EXPECT_EQ(res[1].size(), 10);
    EXPECT_TRUE(std::isnan(res[2][0]));
    EXPECT_TRUE(std::isnan(res[2][1]));
    EXPECT_TRUE(std::isnan(res[2][2]));
    EXPECT_EQ(res[2][3], 16.);
    EXPECT_EQ(res[2][4], 20);
    EXPECT_EQ(res[2][5], 24);
    EXPECT_EQ(res[2][6], 28);
    EXPECT_EQ(res[2][7], 32);
    EXPECT_TRUE(std::isnan(res[2][8]));
    EXPECT_TRUE(std::isnan(res[2][9]));
}



TEST(NormDataNan, VecData)
{
    TVecDouble depth{4., 6., 8. };
    TVecDouble vals{8., 12., 16. };

    TVecDouble depth2{1., 3., 5. };
    TVecDouble vals2{8., 12., 16. };

    TVecDouble depth3{6., 8., 10. };
    TVecDouble vals3{8., 12., 16. };
    /*
    std::shared_ptr<TSimpleData> simple = std::make_shared<TSimpleData>();
    simple->Set(&depth, &vals, 1, false, TTypeEdit::NoUpdate);

    std::shared_ptr<TSimpleData> simple2 = std::make_shared<TSimpleData>();
    simple2->Set(&depth2, &vals, 1, false, TTypeEdit::NoUpdate);

    std::shared_ptr<TSimpleData> simple3 = std::make_shared<TSimpleData>();
    simple3->Set(&depth3, &vals, 1, false, TTypeEdit::NoUpdate);

    TVecVecDouble res = NormDataNan(1., 10., 1., {simple, simple2, simple3}, NAN);
*/
    TVecNormInfo vec = {
            {depth.begin(), depth.end(), vals.begin()},
            {depth2.begin(), depth2.end(), vals2.begin()},
            {depth3.begin(), depth3.end(), vals3.begin()}
    };
    TVecVecDouble res = NormLas(vec, 1., 10., 1.);

    ASSERT_EQ(res.size(), 4);//глубина и нормированное значение

    EXPECT_EQ(res[0].size(), 10);
    EXPECT_EQ(res[0][0], 1.);
    EXPECT_EQ(res[0][1], 2.);
    EXPECT_EQ(res[0][2], 3.);
    EXPECT_EQ(res[0][3], 4.);
    EXPECT_EQ(res[0][4], 5.);
    EXPECT_EQ(res[0][5], 6.);
    EXPECT_EQ(res[0][6], 7.);
    EXPECT_EQ(res[0][7], 8.);
    EXPECT_EQ(res[0][8], 9.);
    EXPECT_EQ(res[0][9], 10.);

    EXPECT_EQ(res[1].size(), 10);
    EXPECT_TRUE(std::isnan(res[1][0]));
    EXPECT_TRUE(std::isnan(res[1][1]));
    EXPECT_TRUE(std::isnan(res[1][2]));
    EXPECT_EQ(res[1][3], 8.);
    EXPECT_EQ(res[1][4], 10);
    EXPECT_EQ(res[1][5], 12);
    EXPECT_EQ(res[1][6], 14);
    EXPECT_EQ(res[1][7], 16);
    EXPECT_TRUE(std::isnan(res[1][8]));
    EXPECT_TRUE(std::isnan(res[1][9]));

    EXPECT_EQ(res[2].size(), 10);
    EXPECT_EQ(res[2][0], 8.);
    EXPECT_EQ(res[2][1], 10);
    EXPECT_EQ(res[2][2], 12);
    EXPECT_EQ(res[2][3], 14);
    EXPECT_EQ(res[2][4], 16);
    EXPECT_TRUE(std::isnan(res[2][5]));
    EXPECT_TRUE(std::isnan(res[2][6]));
    EXPECT_TRUE(std::isnan(res[2][7]));
    EXPECT_TRUE(std::isnan(res[2][8]));
    EXPECT_TRUE(std::isnan(res[2][9]));

    EXPECT_EQ(res[3].size(), 10);
    EXPECT_TRUE(std::isnan(res[3][0]));
    EXPECT_TRUE(std::isnan(res[3][1]));
    EXPECT_TRUE(std::isnan(res[3][2]));
    EXPECT_TRUE(std::isnan(res[3][3]));
    EXPECT_TRUE(std::isnan(res[3][4]));
    EXPECT_EQ(res[3][5], 8.);
    EXPECT_EQ(res[3][6], 10);
    EXPECT_EQ(res[3][7], 12);
    EXPECT_EQ(res[3][8], 14);
    EXPECT_EQ(res[3][9], 16);

}

TEST(Translit, Call)
{
    TString input = "АБВГДЕЁЖЗИЙКЛМНОПРСТУ Ф Х Ц Ч Ш Щ ЪЫЬ Э Ю Я абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    TString output = "ABVGDEEOZHZIIKLMNOPRSTU F H TS CH SH SCH Y E YU YA abvgdeeozhziiklmnoprstufhtschshschyeyuya";
    EXPECT_EQ(Transliteration(input), output);
}

TEST(TLas, InitLas)
{
    TLas las;
    EXPECT_EQ(las.LasVersion(), 2.0);
}

TEST(TLas, ReadHeader)
{
    TLasReader las;
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
    EXPECT_TRUE(header.LoadData(loadable, TPtrProgress()).IsNoError());
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

TEST(TLasWriter, Write)
{
    TLasWriter writer;
    writer.SetWellInfo(TWellInfoInd::wiSTRT, 1);
    writer.SetWellInfo(TWellInfoInd::wiSTOP, 10);
    writer.SetWellInfo(TWellInfoInd::wiSTEP, 0.2);
    writer.SetWellInfo(TWellInfoInd::wiWELL, "Well");

    TVecDouble depth{4., 6., 8. };
    TVecDouble vals{8., 12., 16. };

    TVecDouble depth2{1., 3., 5. };
    TVecDouble vals2{8., 12., 16. };

    writer.AddCurveInfo("GR", "API", "GR curve") =
            {depth.begin(), depth.end(), vals.begin(), 0, writer.WellInfoDbl(wiNULL)};
    writer.AddCurveInfo("IS", "mA", "Current curve", "%10.2f") =
            {depth2.begin(), depth2.end(), vals2.begin(), 0, writer.WellInfoDbl(wiNULL)};

    EXPECT_TRUE(writer.Write("writer.las").IsNoError());

}

#include <iostream>

#include <complex>

#include <cmath>
#include <math.h>
#include <iterator>
#include <limits>
#include <algorithm>

unsigned int bitReverse(size_t x, int log2n)
{
    int n = 0;
    for (int i = 0; i < log2n; i++)
    {
        n <<= 1;
        n |= (x & 1);
        x >>= 1;
    }
    return n;
}
using comp = std::complex<double>;

template<class Iter>
void fft(Iter a, Iter b, int log2n)
{
    const double PI = std::atan(1.0)*4;
    const comp J(0, 1);
    size_t n = 1 << log2n;

    for (size_t i = 0; i < n; ++i)
        b[bitReverse(i, log2n)] = a[i];

    for (int s = 1; s <= log2n; ++s)
    {
        int m = 1 << s;
        int m2 = m >> 1;
        comp w(1, 0);
        comp wm = std::exp(-J * (PI / m2));

        for (int j = 0; j < m2; ++j)
        {
            for (int k = j; k < n; k += m)
            {
                std::complex t = w * b[k + m2];
                std::complex u = b[k];
                b[k] = u + t;
                b[k + m2] = u - t;
            }
            w *= wm;
        }
    }
}
using TVecComp = std::vector<comp>;
const double PI = std::atan(1.0)*4;

void fft2(TVecComp& v, int invert)
{
    size_t n = v.size();
    if(n == 1) return;
    TVecComp v1(n / 2);
    TVecComp v2(v1.size());
    for(size_t i = 0; i < v1.size(); i++)
    {
        v1[i] = v[i * 2];
        v2[i] = v[i * 2 + 1];
    }
    fft2(v1, invert);
    fft2(v2, invert);

    double ang = 2 * PI  / double(n) * invert;
    comp w(1);
    comp wn(std::cos(ang), std::sin(ang));
    for(size_t i = 0; i < v1.size(); i++)
    {
        v[i] = v1[i] + w * v2[i];
        v[i + v1.size()] = v1[i] - w * v2[i];
        if(invert == -1)
        {
            v[i] = v[i] / 2.;
            v[i + v1.size()] = v[i + v1.size()] / 2.;
        }
        w = w * wn;
    }
}

TEST(FFFT, Calc)
{
    TVecComp a = {
            {0, 0}, {1, 0}, {3, 0}, {4, 0},
            {4, 0}, {3, 0}, {1, 0}, {0, 0},
            {0, 0}, {0, 0}, {0, 0}, {0, 0},
            {0, 0}, {0, 0}, {0, 0}, {0, 0}
    };
    TVecComp b(8);
    TVecComp c(8);

    fft(a.begin(), b.begin(), 3);
    fft(b.begin(), c.begin(), 3);

    fft2(a, 1);
    EXPECT_NE(a[0].real(), 0.);
    fft2(a, -1);
    EXPECT_EQ(a[0].real(), 0.);
}

/*
int main()
{

    typedef complex<double> cx;
    cx a[] = {
            cx(0, 0), cx(1, 1), cx(3, 3), cx(4, 4),
            cx(4, 4), cx(3, 3), cx(1, 1), cx(0, 0)
    };

    cx b[8];
    fft(a, b, 3);
    for (int i=0; i<8; ++i)
        cout << b[i] << " ";

}*/