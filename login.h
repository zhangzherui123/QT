#ifndef LOGIN_H
#define LOGIN_H

#include "widget.h"
#include "dialoglist.h"

#include <QWidget>
#include <QUdpSocket>
#include <QMessageBox>
#include "global.h"


class DialogList;

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QHostAddress Server_IP, quint16 Server_Port, quint16 My_Port, QWidget *parent = 0);
//    DialogList* tmp;
//    DialogList d;
    QUdpSocket* udpSocket_Client;
    ~Login();
    void set_KEY(quint32 E, quint32 N)
    {
        this->E = E;
        this->N = N;
    }
    QHostAddress Server_IP;
    quint16 Server_Port;
    quint16 My_Port;
    enum Server_MsgType {Msg,UsrEnter,UsrLeft, LoginSuccess,UnLogin,PasswdError,Secret_Request, Secret_Response, LoginReqeust, LoginReponse,
                        Widget_Exit, loginReapt,CAPMSG};

private:
    Ui::Login *ui;



signals:
    // 关闭窗口主动的发送信号
    void closeWidget();
    void clickdLogin();
    void LoginSuccess_Signal();

public:
    quint32 E;
    quint32 N;
    // 关闭事件(这个函数是内部的,我们只需要实现即可)
    void closeEvent(QCloseEvent* );

    void ReceiveMessage();   //接受UDP消息

    bool isPrime( int num );

private slots:
    void on_trans_clicked();
    void on_Regist_clicked();
};

#endif // LOGIN_H
