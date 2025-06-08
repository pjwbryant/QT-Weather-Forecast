#ifndef CITYCODEUTILS_H
#define CITYCODEUTILS_H

#include<QString>
#include<QMap>

class citycodeUtils
{
public:
    citycodeUtils();
    QMap<QString,QString> citycodeMap;//存储cityid和cityname的映射
    QString getCitycodeFromName(QString name);//根据name找cityid
private:
    void citycodeMapinit();//读入Json文件，写到citycodeMap中
};
#endif // CITYCODEUTILS_H
