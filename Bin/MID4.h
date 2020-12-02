//
// Created by user on 09.10.2019.
//

#ifndef TESTAPP_MID4_H
#define TESTAPP_MID4_H

#include "MIDHeader.h"

#pragma pack (push, 1) //byte alignment

struct THeaderMID4{
    char Text[508];			//Text description
    char EndAscHead[4];		//Must be equal to: ~ASC
    char Ver[12];			//Format version, must be equal to: MID4BK
    char Customer[40];		//Customer
    char Area[40];			//Area
    char Well[10];			//Well
    char Date[11];			//Date logging, dd.mm.yyyy
    char Operator[16];		//Operator
    char TimeBeg[9];		//Start time, hh:mm
    char Reserve1[9];
    float DepthBeg;			//Begin depth, ft. or m.
    float DepthEnd;			//End depth, ft. or m.
    float Reserve2;
    short NumTool;			//Tool number 1
    short NumTool2;			//Tool number 2
    short LenRec;			//Lenght record, sizeof(stData_MIDK_4_Baker)
    short Reserve3[3];
    float CountOK;			//Count record
    float Reserve4;
    float CoefIB;			//Сoefficient B for I (Y = A * X + B): B = 0
    float CoefIA;			//Сoefficient A for I (Y = A * X + B): A = 0.0715
    float CoefTInB;			//Сoefficient B for internal thermometer (Y = A * X + B)
    float CoefTInA;			//Сoefficient A for internal thermometer (Y = A * X + B)
    float CoefGR;			//Сoefficient A for GR (Y = A * X + B)
    short Reserve5;
    float CoefTNB;			//Сoefficient B for tension(Y = A * X + B)
    float CoefTNA;			//Сoefficient A for tension(Y = A * X + B)
    float CoefTOutA;		//Сoefficient A for outside thermometer(Y = A * X + B)
    float CoefTOutB;		//Сoefficient B for outside thermometer(Y = A * X + B)
    short Reserve6;
    float CoefGRExtA;		//Сoefficient A for GR (Y = A * X + B)
    float CoefGRExtB;		//Сoefficient B for GR (Y = A * X + B)
    float CoefPressA;		//Сoefficient A for pressure (Y = A * X + B)
    float CoefPressB;		//Сoefficient B for pressure (Y = A * X + B)
    float CoefIExtA;		//Сoefficient A for I (Y = A * X + B): A = 0.0715
    float CoefIExtB;		//Сoefficient B for I (Y = A * X + B): B = 0
    char Reserve7[265];
    char EndBinHead[4];		//Must be equal to: ~BIN
};

const int CountZMData = 42;
const int CountZMMark = 2;
const int CountZSData = 30;
const int CountZSMark = 1;
const int CountScan = 6;
const int CountScanData = 8;

const int CountZLData = 54;
const int CountZLMark = 2;

struct TDataMID4{
    float Depth;						//Depth, ft or m
    float Speed;						//Speed, ft/min, ft/h, m/min, m/h
    float Time;							//Time between data, millisecond
    unsigned short MM;					//Magnetic mark, == 0 -> false, != 0 -> true
    short TN;							//Tension, code

    unsigned short Rezerv1[10];			//Reserve

    unsigned short Num2;				//Number tool 2
    unsigned short Index2;				//Data index in order tool 2
    short IL;							//Current tool 2, code
    short TOut;							//Outside thermometer tool 2, code
    short TIn2;							//Internal thermometer tool 2, code
    short ZL[CountZLData + CountZLMark];//Decay ZL tool 2 KUS: 1, 25, 250
    unsigned char Code2[8];				//Service data tool 2

    unsigned short Num1;				//Number tool 1
    unsigned short Index1;				//Data index in order tool 1
    unsigned short GR;					//GR tool 1, code
    short IM;							//Current for ZM tool 1, code
    short IS;							//Current for ZS tool 1, code
    short ResT;							//Reserve for outside thermometer tool 1
    short TIn1;							//Internal thermometer tool 1, code
    short Press;						//Pressure tool 1, code
    short ZM[CountZMData + CountZMMark];//Decay ZM tool 1(42 data + 2 mark) KUS: 1, 25, 250
    short ZS[CountZSData + CountZSMark];//Decay ZS tool 1(30 data + 1 mark) KUS: 25, 250
    short Scan[CountScan][CountScanData];//Decay Scan tool 1
    unsigned char Code1[8];				//Service data tool 1

    unsigned short Rezerv2[10];			//Reserve
};
#pragma pack (pop)

class TMID4Format : public TMIDFileTemp<THeaderMID4, TDataMID4>{
public:
    TMID4Format():TMIDFileTemp<THeaderMID4, TDataMID4>("MID4BK"){}
};


#endif //TESTAPP_MID4_H
