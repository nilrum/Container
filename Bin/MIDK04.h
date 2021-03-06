//
// Created by user on 27.02.2020.
//

#ifndef NEO_MIDK04_H
#define NEO_MIDK04_H

#include "MIDHeader.h"

#pragma pack (push, 1) //byte alignment

struct THeaderMIDK04{
    char Text[508];			//Text description
    char EndAscHead[4];		//Must be equal to: ~ASC
    char Ver[12];			//Format version, must be equal to: MIDK04
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

const int CountOneKus_MIDK_04 = 15;
const int CountOthKus_MIDK_04 = 54;
const int CountSpad_MIDK_04 = 6;

struct TDataMIDK04{//Мид-К для определения третьей колонны
    float Depth;            //глубина в метрах
    float Speed;            //скорость каротажа в данной точке
    float Time;             //время между кадрами в миллисекундах
    unsigned short MM;      //магнитная метка
    unsigned short NumMid;  //номер модуля МИД
    unsigned short GR;      //данные ГК
    short IS;               //ток генераторной катушки короткого зонда
    short IL;               //ток длинного зонда
    short TOut;             //внешний термометр
    short TIn;              //внутренний термометр
    short Press;		    //манометр
    short TN;               //натяжение кабеля
    unsigned short Rezerv[10];  //резерв
    short ZS1[CountOneKus_MIDK_04];           //продольный зонд ZS1 (КУС=1)
    short ZS2[CountOthKus_MIDK_04];           //продольный зонд ZS2 (КУС=25)
    short ZS3[CountOthKus_MIDK_04];           //продольный зонд ZS3 (КУС=250)
    short ZL1[CountOneKus_MIDK_04];           //продольный зонд ZL1 (КУС=1)
    short ZL2[CountOthKus_MIDK_04];           //продольный зонд ZL2 (КУС=25)
    short ZL3[CountOthKus_MIDK_04];           //продольный зонд ZL3 (КУС=250)
};

#pragma pack (pop)

class TMIDK04Format : public TMIDFileTemp<THeaderMIDK04, TDataMIDK04>{
public:
    TMIDK04Format():TMIDFileTemp<THeaderMIDK04, TDataMIDK04>("MIDK04"){}

};

#endif //NEO_MIDK04_H
