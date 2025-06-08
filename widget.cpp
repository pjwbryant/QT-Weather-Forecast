#include "widget.h"
#include "ui_widget.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QPainter>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    // setFixedSize(382,591);
    setWindowFlag(Qt::FramelessWindowHint);//无边框样式

    // 鼠标右键点击事件后显示的菜单
    menuQuit =new QMenu(this);//菜单
    menuQuit->setStyleSheet("QMenu::item{color:red}");
    QPixmap closePix;
    closePix.load(":/images/close");
    QAction *closeAction=new QAction(QIcon(closePix),"关闭",this);//菜单项
    menuQuit->addAction(closeAction);//添加菜单项

    //绑定点击关闭
    connect(menuQuit,&QMenu::triggered,this,[=](){
        this->close();
    });

    //由QNetworkAccessManager中的get方法发起对QNetworkRequest的请求（其中给定QUrl），返回QNetworkReply，用readAll读取数据
    //请求天气数据
    manager=new QNetworkAccessManager(this);
    // QUrl urlItboy("http://t.weather.itboy.net/api/weather/city/101190605");
    // QUrl urlTianqi("http://v1.yiketianqi.com/api?unescape=1&version=v61&appid=34342915&appsecret=FXIZP2jv");//一天的天气数据
    QUrl urlTianqi("http://v1.yiketianqi.com/api?unescape=1&version=v9&appid=34342915&appsecret=FXIZP2jv");
    QNetworkRequest request(urlTianqi);//请求类
    manager->get(request);//得到回复
    //manager在get请求后读取数据（QNetworkAccessManager的finished信号中传输了个QNetworkReply）
    connect(manager,&QNetworkAccessManager::finished,this,&Widget::readhttpReply);

    //安装事件过滤器
    ui->widget0404->installEventFilter(this);
    ui->widget0405->installEventFilter(this);

    //将ui标签放入标签列表,未来6天的天气信息
    weeklist<<ui->label_week1<<ui->label_week2<<ui->label_week3<<ui->label_week4<<ui->label_week5<<ui->label_week6;
    datelist<<ui->label_data1<<ui->label_data2<<ui->label_data3<<ui->label_data4<<ui->label_data5<<ui->label_data6;
    weatherIconlist<<ui->label_weatherIcon1<<ui->label_weatherIcon2<<ui->label_weatherIcon3<<ui->label_weatherIcon4<<ui->label_weatherIcon5<<ui->label_weatherIcon6;
    weatherTypelist<<ui->label_weatherType1<<ui->label_weatherType2<<ui->label_weatherType3<<ui->label_weatherType4<<ui->label_weatherType5<<ui->label_weatherType6;
    airQualitylist<<ui->label_airquality1<<ui->label_airquality2<<ui->label_airquality3<<ui->label_airquality4<<ui->label_airquality5<<ui->label_airquality6;
    windDirectinlist<<ui->label_fx1<<ui->label_fx2<<ui->label_fx3<<ui->label_fx4<<ui->label_fx5<<ui->label_fx6;
    windLevellist<<ui->label_fx1level<<ui->label_fx2level<<ui->label_fx3level<<ui->label_fx4level<<ui->label_fx5level<<ui->label_fx6level;


    weatherTypeIconMap.insert("晴",":/weatherType/weatherType/qingtian.png");
    weatherTypeIconMap.insert("阴",":/weatherType/weatherType/yin.png");
    weatherTypeIconMap.insert("多云",":/weatherType/weatherType/duoyun.png");
    weatherTypeIconMap.insert("小雨",":/weatherType/weatherType/xiaoyu.png");
    weatherTypeIconMap.insert("中雨",":/weatherType/weatherType/zhongyu.png");
    weatherTypeIconMap.insert("大雨",":/weatherType/weatherType/dayu.png");
    weatherTypeIconMap.insert("阵雨",":/weatherType/weatherType/zhenyu.png");
    weatherTypeIconMap.insert("雷阵雨",":/weatherType/weatherType/leizhenyu.png");
    weatherTypeIconMap.insert("小雨转晴",":/weatherType/weatherType/xiaoyuzhuanqing.png");
    weatherTypeIconMap.insert("晴转多云",":/weatherType/weatherType/qingzhuanduoyun.png");
    weatherTypeIconMap.insert("多云转晴",":/weatherType/weatherType/duoyunzhuanqing.png");
    weatherTypeIconMap.insert("多云转小雨",":/weatherType/weatherType/duoyunzhuanxiaoyu.png");
    weatherTypeIconMap.insert("阴转晴",":/weatherType/weatherType/yinzhuanqing.png");
    weatherTypeIconMap.insert("大雾",":/weatherType/weatherType/dawu.png");
    weatherTypeIconMap.insert("沙尘暴",":/weatherType/weatherType/shachenbao.png");
    weatherTypeIconMap.insert("大雪",":/weatherType/weatherType/daxue.png");
    weatherTypeIconMap.insert("小雪",":/weatherType/weatherType/xiaoxue.png");
    weatherTypeIconMap.insert("暴雪",":/weatherType/weatherType/baoxue.png");
    weatherTypeIconMap.insert("雨夹雪",":/weatherType/weatherType/yujiaxue.png");
    weatherTypeIconMap.insert("",":/weatherType/weatherType/.png");
    weatherTypeIconMap.insert("",":/weatherType/weatherType/.png");
    weatherTypeIconMap.insert("",":/weatherType/weatherType/.png");
    weatherTypeIconMap.insert("",":/weatherType/weatherType/.png");
}

Widget::~Widget()
{
    delete ui;
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    //点击打开菜单，关闭窗口
    if(event->button()==Qt::RightButton){
        menuQuit->exec(QCursor::pos());//鼠标当前位置打开菜单
    }
    //鼠标拖动窗口
    if(event->button()==Qt::LeftButton){
        //鼠标位置-当前窗口位置
        offset = event->globalPos()-this->pos();//点击后获取鼠标偏移
    }
}

//左键按下移动
void Widget::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos()-offset);
}

// void Widget::paintEvent(QPaintEvent *event)
// {

// }

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched==ui->widget0404&&event->type()==QEvent::Paint){
        drawTempLineHigh();
        return true;
    }
    if(watched==ui->widget0405&&event->type()==QEvent::Paint){
        drawTempLineLow();
        return true;
    }
    return QWidget::eventFilter(watched, event); //传递给上层的widget继续处理事件
}

//读取QNetworkReply中的数据
void Widget::readhttpReply(QNetworkReply *reply)
{
    int rescode=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();//将来自应答的进一步信息传递回应用程序
    //判断是否请求成功
    if(reply->error()==QNetworkReply::NoError&&rescode==200){
        QByteArray weatherdata=reply->readAll();//获取的数据
        QString weatherdata_string = QString::fromUtf8(weatherdata);//转成string打印出来
        qDebug()<<weatherdata_string;
        parseJsonDataFor7days(weatherdata);//解析Json数据
    }
    else{
        QMessageBox msg;
        msg.setText("请求失败："+reply->errorString());
        msg.setStyleSheet("QPushButton{color:red}");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
}

//解析一天的数据：将传入的Json数据解析填入ui中
void Widget::parseWeatherJsonData(QByteArray weatherdata)
{
    QJsonDocument jsondoc=QJsonDocument::fromJson(weatherdata);
    if(!jsondoc.isNull()&&jsondoc.isObject()){
        QJsonObject jsonobj=jsondoc.object();
        ui->label_date->setText(jsonobj["date"].toString()+" "+jsonobj["week"].toString());
        ui->label_city->setText(jsonobj["city"].toString());
        //设置天气类型
        ui->label_daytype->setText(jsonobj["wea"].toString());
        ui->label_weatherIcon->setPixmap(weatherTypeIconMap[jsonobj["wea"].toString()]);

        ui->label_temp->setText(jsonobj["tem"].toString()+"℃");
        ui->label_temprange->setText(jsonobj["tem2"].toString()+"℃"+'~'+jsonobj["tem1"].toString()+"℃");
        ui->label_tip->setText("贴心提示："+jsonobj["air_tips"].toString());
        ui->label_winddirection->setText(jsonobj["win"].toString());
        ui->label_windlevel->setText(jsonobj["win_speed"].toString());
        ui->label_pmlevel->setText(jsonobj["air_pm25"].toString());
        ui->label_humiditylevel->setText(jsonobj["humidity"].toString());
        ui->label_airqualitylevel->setText(jsonobj["air_level"].toString());
    }
}

//解析7天的天气数据放到Day数组中
void Widget::parseJsonDataFor7days(QByteArray weatherdata)
{
    QJsonDocument jsondoc=QJsonDocument::fromJson(weatherdata);
    if(!jsondoc.isEmpty()&&jsondoc.isObject()){
        QJsonObject jsonobjRoot=jsondoc.object();
        //day[0]表示当天的信息
        day[0].mcity=jsonobjRoot["city"].toString();
        day[0].mtips=jsonobjRoot["aqi"].toObject()["air_tips"].toString();
        day[0].mpm25=jsonobjRoot["aqi"].toObject()["pm25"].toString();
        //解析包含7个Jsonobj的数组
        if(jsonobjRoot.contains("data")&&jsonobjRoot["data"].isArray()){
            QJsonArray jsonarrDays=jsonobjRoot["data"].toArray();
            for(int i=0;i<jsonarrDays.size();i++){
                QJsonObject jsonobjDay=jsonarrDays[i].toObject();
                day[i].mdate=jsonobjDay["date"].toString();
                day[i].mweek=jsonobjDay["week"].toString();
                day[i].mweatherType=jsonobjDay["wea_day"].toString();
                day[i].mtemp=jsonobjDay["tem"].toString();
                day[i].mtempLow=jsonobjDay["tem2"].toString();
                day[i].mtempHigh=jsonobjDay["tem1"].toString();
                day[i].mhumidity=jsonobjDay["humidity"].toString();
                day[i].mfx=jsonobjDay["win"].toArray()[0].toString();
                day[i].mfl=jsonobjDay["win_speed"].toString();
                day[i].mairq=jsonobjDay["air_level"].toString();
            }
        }
    }
    //更新ui中的信息
    UpdateUI();
    //用于异步请求重绘控件，触发 paintEvent，即eventFilter中的QEvent::Paint，会重绘折线图
    update();
}

void Widget::UpdateUI()
{
    //更新day[0]的信息
    ui->label_date->setText(day[0].mdate+"   "+day[0].mweek);
    ui->label_city->setText(day[0].mcity);
    //设置天气类型
    ui->label_daytype->setText(day[0].mweatherType);
    ui->label_weatherIcon->setPixmap(weatherTypeIconMap[day[0].mweatherType]);//天气图标
    ui->label_temp->setText(day[0].mtemp+"℃");
    ui->label_temprange->setText(day[0].mtempLow+"℃"+'~'+day[0].mtempHigh+"℃");
    ui->label_tip->setText("贴心提示："+day[0].mtips);
    ui->label_winddirection->setText(day[0].mfx);
    ui->label_windlevel->setText(day[0].mfl);
    ui->label_pmlevel->setText(day[0].mpm25);
    ui->label_humiditylevel->setText(day[0].mhumidity);
    ui->label_airqualitylevel->setText(day[0].mairq);
    //更新下面未来6天的信息，list存的是未来6天的label，day存的是7天的天气信息
    for(int i=1;i<7;i++){
        weeklist[i-1]->setText(day[i].mweek);
        QString date=day[i].mdate.split("-")[1]+"-"+day[i].mdate.split("-")[2];//日期取月、日就行
        datelist[i-1]->setText(date);
        weatherIconlist[i-1]->setPixmap(QPixmap(weatherTypeIconMap[day[i].mweatherType]));
        weatherTypelist[i-1]->setText(day[i].mweatherType);
        QString air=day[i].mairq;
        airQualitylist[i-1]->setText(air);
        //根据空气质量设置图标的颜色
        if(air=="优"){
            airQualitylist[i-1]->setStyleSheet("background-color: rgb(48, 145, 43);border-radius: 5px;");
        }else if(air=="良"){
            airQualitylist[i-1]->setStyleSheet("background-color: rgb(255, 170, 0);border-radius: 5px;");
        }else if(air=="轻度"){
            airQualitylist[i-1]->setStyleSheet("background-color: rgb(237, 146, 3);border-radius: 5px;");
        }else if(air=="中度"){
            airQualitylist[i-1]->setStyleSheet("background-color: rgb(171, 105, 2);border-radius: 5px;");
        }else if(air=="重度"){
            airQualitylist[i-1]->setStyleSheet("background-color: rgb(235, 39, 6 );border-radius: 5px;");
        }
        windDirectinlist[i-1]->setText(day[i].mfx);

        QFont font;
        if(day[i].mfl.length()>5){
            font.setPointSize(7);
            windLevellist[i-1]->setFont(font);
        }
        else {
            font.setPointSize(10);
            windLevellist[i-1]->setFont(font);
        }
        windLevellist[i-1]->setText(day[i].mfl);
    }
    weeklist[0]->setText("明天");
    weeklist[1]->setText("后天");
}

//高温折线
void Widget::drawTempLineHigh()
{
    QPainter paint(ui->widget0404); //在指定的控件上进行绘制操作
    paint.setRenderHint(QPainter::Antialiasing,true);//抗锯齿
    QPoint points[6];

    int highavg=0; //温度均值
    int highsum=0;
    for(int i=1;i<7;i++){
        highsum+=day[i].mtempHigh.toInt();
    }
    highavg=highsum/6;

    paint.setPen(Qt::NoPen); //画笔
    paint.setBrush(Qt::NoBrush); //画刷（填充色）
    //y的高度就是widget0404中间的高度+当天温度和平均温度的偏移值
    for(int i=0;i<6;i++){
        points[i].setX(airQualitylist[i]->x()+airQualitylist[i]->width()/2); //x值就是空气质量框框的中间
        points[i].setY(ui->widget0404->height()/2-(day[i+1].mtempHigh.toInt()-highavg)*2); //y值是widget0404中间加上和平均值的差值
        paint.setBrush(QColor(255,107,9)); //设置填充色
        paint.drawEllipse(points[i],3,3); //画温度圈
        paint.setPen(QColor(255,107,9));
        paint.drawText(points[i]+QPoint(-8,-6),day[i+1].mtempHigh+"℃");//标温度值
        //画两个温度点的连接线
        if(i!=0){
            paint.drawLine(points[i-1]+QPoint(3,0),points[i]-QPoint(3,0));
        }
    }
}

//低温折线
void Widget::drawTempLineLow()
{
    QPainter paint(ui->widget0405);
    paint.setRenderHint(QPainter::Antialiasing,true);//抗锯齿
    paint.setPen(Qt::yellow);
    QPoint points[6];

    int lowavg=0;
    int lowsum=0;
    for(int i=1;i<7;i++){
        lowsum+=day[i].mtempLow.toInt();
    }
    lowavg=lowsum/6;

    paint.setPen(Qt::NoPen);
    paint.setBrush(Qt::NoBrush);
    //y的高度就是widget0404中间的高度+当天温度和平均温度的偏移值
    for(int i=0;i<6;i++){
        points[i].setX(airQualitylist[i]->x()+airQualitylist[i]->width()/2);
        points[i].setY(ui->widget0404->height()/2-(day[i+1].mtempLow.toInt()-lowavg)*2);
        paint.setBrush(QColor(0,35,245));
        paint.drawEllipse(points[i],3,3);//画温度点
        paint.setPen(QColor(0,35,245));
        paint.drawText(points[i]+QPoint(-6,15),day[i+1].mtempLow+"℃");//标温度值
        if(i!=0){//在圈圈的边缘画线
            paint.drawLine(points[i-1]+QPoint(3,0),points[i]-QPoint(3,0));
        }
    }
}

//获取输入框中的城市名称
void Widget::on_btn_search_clicked()
{
    QString cityname=ui->lineEdit_search->text();
    QString citycode=citycodeutils.getCitycodeFromName(cityname);//根据名称从Json资源文件中找citycode
    if(citycode.isNull()){
        QMessageBox msg;
        msg.setWindowTitle("输入错误");
        msg.setText("请输入正确的城市名称！");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        return;
    }
    QUrl urlTianqi("http://v1.yiketianqi.com/api?unescape=1&version=v9&appid=34342915&appsecret=FXIZP2jv&cityid="+citycode);
    QNetworkRequest request(urlTianqi);//请求类
    manager->get(request);//发送get请求
}

//LineEdit确认键按下
void Widget::on_lineEdit_search_returnPressed()
{
    on_btn_search_clicked();
}

