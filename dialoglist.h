#ifndef DIALOGLIST_H
#define DIALOGLIST_H

#include <QWidget>
#include <login.h>
#include <QToolButton>
#include <QUdpSocket>
#include <QDataStream>
#include <QMap>
#include <qdebug.h>
#include <QSet>
#include "global.h"

namespace Ui {
class DialogList;
}

class DialogList : public QWidget
{
    Q_OBJECT

public:
    // 服务器的质数,设置为固定值
    quint32 server_Key = 127 ;
    quint32 D;
    quint32 N;
    // 服务器的密钥池
    QSet<quint32> myset;
    // key:是客户端可以用的公钥, value是该公钥对应的私钥,需要服务器去解密使用
    QMap<quint32, quint32> Match_Key;
    bool isPrime( int num ); // 密钥协商所用的函数
    quint32 Creat_Key(quint32 server_Key, quint32 client_key);

    explicit DialogList(QWidget *parent = 0);
    QVector< bool > isShow;
    QVector<QToolButton *> vToolBtn;
    QUdpSocket * udpSocket_Server;
    QHostAddress Server_IP;
    // 账号 - 密码
    QMap<QString, quint32> mymap;
//    mymap["one"] = "111";
    QString net_ip;
    QString Curname;
    ~DialogList();
    // 封装创建窗口的函数
    void CreatWidget(int i);
    void ReceiveMessage();   //接受UDP消息
private:
    Ui::DialogList *ui;

signals:
    // 关闭窗口主动的发送信号
    void LoginSuccess();
    void UnLogin();
    void PasswdError();
private slots:
    void on_serverIP_clicked();
    void on_serverIP_2_clicked();
};

#endif // DIALOGLIST_H
