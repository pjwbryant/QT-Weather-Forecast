#ifndef DAY_H
#define DAY_H

#include<QString>

class Day
{
public:
    Day();

    QString mdate;
    QString mweek;
    QString mcity;
    QString mtemp;
    QString mweatherType;
    QString mtempLow;
    QString mtempHigh;

    QString mtips;
    QString mfx;
    QString mfl;
    QString mpm25;
    QString mhumidity;
    QString mairq;
};

#endif // DAY_H
