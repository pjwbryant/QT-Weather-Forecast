#include "citycodeutils.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>

citycodeUtils::citycodeUtils() {}

QString citycodeUtils::getCitycodeFromName(QString name)
{
    //初次访问文件
    if(citycodeMap.isEmpty()){
        citycodeMapinit();
    }
    //查询城市id
    QMap<QString,QString>::Iterator it=citycodeMap.find(name);
    if(it==citycodeMap.end()){
        return NULL;
    }
    return it.value();
}

//读取Json文件，将cityname和citycode键值对放入Map中
void citycodeUtils::citycodeMapinit()
{
    QFile file(":/cn_weather_citylist.json");
    file.open(QFile::ReadOnly);
    QByteArray arr=file.readAll();
    file.close();
    QJsonDocument jsondoc=QJsonDocument::fromJson(arr);
    if(!jsondoc.isNull()&&jsondoc.isArray()){ //json文件中是一个数组离包含众多jsonobject
        QJsonArray citysjsonarr=jsondoc.array();
        for(QJsonValue city:citysjsonarr){//遍历Jsonarr中的每个Jsonobj
            if(city.isObject()){
                QString cityname=city["cityName"].toString();
                QString citycode=city["cityCode"].toString();
                citycodeMap.insert(cityname,citycode);
            }
        }
    }
}
