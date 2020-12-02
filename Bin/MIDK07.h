//
// Created by user on 30.11.2020.
//

#ifndef NEO_MIDK07_H
#define NEO_MIDK07_H

#include "MIDHeader.h"

#pragma pack (push, 1) //byte alignment

struct THeaderMIDK07{
    char Text[508];			//Text description
    char EndAscHead[4];		//Must be equal to: ~ASC
    char Ver[12];			//Format version, must be equal to: MIDK07 MID-V1.1
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

const int Count_MIDK_06_Zond = 5;
const int Count_MIDK_06_Zader = 54;

struct TDataMIDK07{//Мид-К 134 слова 150 градусов
    float Depth;            //глубина в метрах
    float Speed;            //скорость каротажа в данной точке в м/сек
    float Time;             //время между кадрами в миллисекундах
    unsigned short MM;   	//магнитная метка
    unsigned short NumMid;  //номер модуля МИД
    unsigned short GR;      //данные ГК
    short IGenKat;          //ток генераторной катушки
    short TOut;             //внешний термометр
    short TIn;              //внутренний термометр
    short TN;               //натяжение кабеля
    unsigned short Rezerv[10];  //резерв
    short TX[15];           //быстрые каналы поперечные зонды X
    short TY[15];           //быстрые каналы поперечные зонды Y
    short Z1[15];           //продольный зонд Z1 (КУС=1)
    short Z2[30];           //продольный зонд Z2 (КУС=25)
    short Z3[54];           //продольный зонд Z3 (КУС=250)
};

#pragma pack (pop)

class TMIDK07Format : public TMIDFileTemp<THeaderMIDK07, TDataMIDK07>{
public:
    TMIDK07Format():TMIDFileTemp<THeaderMIDK07, TDataMIDK07>("MIDK07"){}

protected:
    virtual bool CheckTextVersionHeader(const THeaderMIDK07& h)
    {
        return ::CheckTextVersion(h, version) || TString(h.Ver) == TString("MID-V1.1");
    }
};


#endif //NEO_MIDK07_H
