#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QUdpSocket>
#include <qdebug.h>
#include "global.h"
#include <QMap>
#include <QBuffer>
#include <QImageReader>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"
#include <QTimer>
#include <QNetworkInterface>


using namespace cv;
using namespace std;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    enum MsgType {Msg,UsrEnter,UsrLeft, LoginSuccess,UnLogin,PasswdError,Secret_Request, Secret_Response, LoginReqeust, LoginReponse,
                        Widget_Exit, loginReapt,CAPMSG};
public:
    explicit Widget(QWidget *parent, QString name, quint16 port, quint16 NameListSize, quint32 E, quint32 N, QString net_ip);
    ~Widget();
    quint16 NameListSize;
    quint32 server_Cilent;
    quint32 E;
    quint32 N;
    QMap<QString, quint32> mymap;
    QTimer timer;//
    VideoCapture camera;
private:
    Ui::Widget *ui;
    QString net_ip;
signals:
    // 关闭窗口主动的发送信号
    void closeWidget();
public:
    // 关闭事件(这个函数是内部的,我们只需要实现即可)
    void closeEvent(QCloseEvent* );


// 以下是多人聊天相关的
public:
   void sndMsg(MsgType type, QByteArray video_msg = 0); //广播UDP消息
   void usrEnter(QString username);//处理新用户加入
   void usrLeft(QString usrname,QString time); //处理用户离开
   QString getUsr(); //获取用户名
   QString getMsg(); //获取聊天信息
private slots:
   void on_openCAP_clicked();
   void video_capture_send();
   void on_send_video_clicked();

   void on_pushButton_clicked();

   void on_pushButton_2_clicked();

private:

    QUdpSocket * udpSocket; //udp套接字
    qint16 port; //端口
    QString uName; //用户名
    bool isopenCAP;
    void ReceiveMessage();   //接受UDP消息

};

#endif // WIDGET_H
