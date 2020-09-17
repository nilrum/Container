//
// Created by user on 27.02.2020.
//

#ifndef NEO_MIDK04_H
#define NEO_MIDK04_H

#include "BinFile.h"

#pragma pack (push, 1) //byte alignment
struct THeaderMIDK04{//шапка МИД-К 2006,2007 но расширена в техже размерах
    char Data[508];
    char EndAscHead[4];     //~ASC
    char Ver[12];           //версия формата записи
    char Zakaz[40];         //заказчик
    char Area[40];         //площадь
    char Well[10];          //скважина
    char Date[11];          //дата каротажа
    char Operator[16];      //оператор
    char TimeBeg[9];        //время начала каротажа
    char TimeEnd[9];        //время окончания каротажа
    float DepthBeg;         //начальная глубина
    float DepthEnd;         //конечная глубина
    float Speed;            //средняя скорость каротажа
    short NumPrMid;         //номер модуля дефектоскопа
    short NumPrGK;          //номер модуля ГК, либо 0, если не подключен
    short LenRec;           //длина одного кадра, записываемого на диск
    short Rezerv2[3];
    float CountOK;          //кол-во нормальных кадров
    float CountBad;         //кол-во сбойных кадров не записанных в файл
    float KoefTokB;       	//коэффициент линейной зависимости тока
    float KoefTokA;       	//коэффициент линейной зависимости тока
    float KoefTinB;      	//коэффициент линейной зависимости внутреннего термометра
    float KoefTinA;      	//коэффициент линейной зависимости внутреннего термометра
    float KoefGR;           //коэффициент линейной зависимости ГК, по результатам базовой калибровки ГК y=ax+b, где b=0, KoefGR=a
    short Rezerv3;
    float KoefTNB;        	//коэффициент калибровки натяжения
    float KoefTNA;        	//коэффициент калибровки натяжения
    float KoefTOutA;      	//коэффициент калибровки внешнего термометра
    float KoefTOutB;      	//коэффициент калибровки внешнего термометра
    short Rezerv4;
    float KoefGRExtA;     	//полный коэф ГК
    float KoefGRExtB;     	//полный коэф ГК
    float KoefManA;       //калибровка манометра
    float KoefManB;       //калибровка манометра
    float KoefTokExtA;    //дополнительная калибровка тока
    float KoefTokExtB;    //дополнительная калибровка тока
    char Rezerv5[265];       //резерв
    char EndBinHead[4];     //~BIN
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

class TMIDK04Format : public TBinFileTemp<THeaderMIDK04, TDataMIDK04>{
public:
    TMIDK04Format():TBinFileTemp<THeaderMIDK04, TDataMIDK04>("MIDK04"){}
};

template <>
bool CheckHeader<THeaderMIDK04>(const THeaderMIDK04& value, const TString& check)
{
    return TString(value.Ver) == check && value.LenRec == sizeof(TDataMIDK04);
}


#endif //NEO_MIDK04_H
