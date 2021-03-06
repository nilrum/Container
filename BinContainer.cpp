//
// Created by user on 09.10.2019.
//

#include "BinContainer.h"

INIT_PROPERTYS(TDataBinProp)

TString THeaderBin::Version() const
{
    return createFile()->Version();
}

TResult THeaderBin::CheckFile(const TString &path)
{
    return createFile()->CheckVersion(path);
}

void THeaderBin::Copy(float& ptr, const TVariable& value)
{
    ptr = float(value.ToDouble());
}

TVecData THeaderBin::LoadableData(const TString &path)
{
    return TVecData();
}

TResult THeaderBin::LoadData(const TVecData &datas, const TPtrProgress& progress)
{
    return TResult();
}


TString DosToUtf8(const TString& dos)
{
    return Cp1251ToUt8(Cp866ToCp1251(dos));
}

TString Cp866ToCp1251(const TString& dos)
{//производит перевод строки из DOS кодировки в ANSI
    TString rez(dos.length(), ' ');
    for(size_t i = 0; i < dos.size(); i++)//пройдемся по всем символам в строке
    {
        unsigned char curChar = dos[i];//получаем очередной символ
        if ((curChar >= 0x80) && (curChar < 0xB0))
            curChar += 0x40;
        else
        if ((curChar >= 0xE0) && (curChar < 0xF0)) curChar += 0x10;
        rez[i] = curChar;
    }
    return rez;
}

TString Cp1251ToUt8(const TString& val)
{
    TString rez;
    for(size_t i = 0; i < val.length(); i++)
    {
        const unsigned char& v = val[i];
        if (v < 0xC0) {
            rez.push_back(v);
        } else {
            unsigned short unic = 0x0410 + v - 0xC0;//начало алфавита с буквы А в юникоде ( А = 0x0410 - Я = 0x042F, а = 0x0430 - я = 0x044F); 0xC0 начало алфавита в cp1251
            char one = 0xC0 | (unic >> 6);
            char two = 0x80 | (0x3F & unic);//берем последние 6 разрядов
            rez.push_back(one);
            rez.push_back(two);
        }
    }
    return rez;
}