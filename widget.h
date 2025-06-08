#ifndef WIDGET_H
#define WIDGET_H

#include <QMenu>
#include <QNetworkReply>
#include <QWidget>
#include <QMap>
#include "citycodeutils.h"
#include "day.h"
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    // void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::Widget *ui;
    QMenu *menuQuit;//右键的菜单
    QPoint offset;//移动时鼠标位置相对于窗口的右上角的偏移
    QNetworkAccessManager *manager;//http的管理类
    citycodeUtils citycodeutils;//查询城市id的工具类
    QMap<QString,QString> weatherTypeIconMap;//天气类型和图片路径的映射
    Day day[7];
    QList<QLabel *> weeklist;
    QList<QLabel *> datelist;
    QList<QLabel *> weatherIconlist;
    QList<QLabel *> weatherTypelist;
    QList<QLabel *> airQualitylist;
    QList<QLabel *> windDirectinlist;
    QList<QLabel *> windLevellist;

    void parseWeatherJsonData(QByteArray weatherdata);//将传入的Json数据解析填入ui中
    void parseJsonDataFor7days(QByteArray weatherdata);//解析7天的天气数据
    void UpdateUI();

    void drawTempLineHigh();
    void drawTempLineLow();

public slots:
    void readhttpReply(QNetworkReply *reply);//访问网站，请求数据的槽函数

private slots:
    void on_btn_search_clicked();
    void on_lineEdit_search_returnPressed();
};
#endif // WIDGET_H
