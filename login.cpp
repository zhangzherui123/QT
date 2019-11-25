#include "login.h"
#include "ui_login.h"
#include "dialoglist.h"

Login::Login(QHostAddress Server_IP, quint16 Server_Port, quint16 My_Port,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);


    udpSocket_Client = new QUdpSocket(this);
    udpSocket_Client->bind(5000, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    this->Server_IP = Server_IP;
    this->Server_Port = Server_Port;

    connect(udpSocket_Client, &QUdpSocket::readyRead, this, &Login::ReceiveMessage);

//    ui->password->set
    connect(ui->quit, &QPushButton::clicked, [=](){
       this->close();
    });

    connect(ui->login, &QPushButton::clicked, [=](){
        QByteArray array;
        QDataStream stream(&array, QIODevice::WriteOnly);
//        qDebug() <<  ui->usrname->toPlainText();
//        qDebug() <<  ui->password->toPlainText();
        QString str1 = ui->usrname->toPlainText();
        QString str2 = ui->password->toPlainText();
//        str1 = str1.remove('?');
//        str1.replace(QString('?'), QString(""));
        str1 = str1.right(str1.size() - 1);
//        str2 = str2.remove('?');
//        str2.replace(QString('?'), QString(""));
        str2 = str2.right(str2.size() - 1);
        qDebug() << str1;
        qDebug() << str2;
        qDebug() << "111";
        quint32 passwd = multiMod(str2.toInt(), E, N);
        stream << LoginReqeust << str1 << passwd << E; // 第一段内容添加到流中, 第二段是自己的名字

        udpSocket_Client->writeDatagram(array, Server_IP, Server_Port);
    });


    // 登录成功.关闭登录窗口
//    connect(server, &DialogList::LoginSuccess, [=](){
//       this->close();
//    });


}


void Login::closeEvent(QCloseEvent* e)
{
//    // 断开套接字
    udpSocket_Client->close();
    udpSocket_Client->destroyed();

    QWidget::closeEvent(e);
}

void Login::ReceiveMessage()
{
    qint64  mysize = udpSocket_Client->pendingDatagramSize();

    QByteArray array = QByteArray(mysize, 0);
    QHostAddress recIP;
    quint16 recPort;
    udpSocket_Client->readDatagram(array.data(), mysize,&recIP, &recPort);

    // 第一段是用户名, 第二段是密码
    QDataStream stream(&array, QIODevice::ReadOnly);
    int msgType;
    QString usrname;
    stream >> msgType;
    switch (msgType) {
    case LoginSuccess:
    {
        stream >> usrname;
        QString str = usrname +  QStringLiteral(" Congratulations, login success");
        QMessageBox::warning(this, "恭喜", str);
        emit this->LoginSuccess_Signal();
        this->close();
        break;
    }

    case UnLogin:
    {
        QString str = QStringLiteral("No one, please register");
        QMessageBox::warning(this, "警告", str);
        break;
    }


    case PasswdError:
    {
        QString str = QStringLiteral("The user name is correct. Please check the password");
        QMessageBox::warning(this, "警告", str);
        break;
    }

    case Secret_Response:
    {
        QString str = QStringLiteral("Key generation successful");
        QMessageBox::warning(this, "警告", str);
        stream >> E >> N;
        break;
    }

    case loginReapt:
    {
        QString str = QStringLiteral("The user has logged in, please do not log in again");
        QMessageBox::warning(this, "警告", str);
        this->close();
    }


//    case Register_Reply:
//    {
//        QString str = QStringLiteral("User registration successful");
//        QMessageBox::warning(this, "警告", str);
//    }

    default:
        break;
    }
}

//connect(ui->l, &QToolButton::clicked, [=](bool ischeck){
//    if(ischeck)
//        ui->msgTxtEdit->setFontWeight(QFont::Bold);
//    else
//        ui->msgTxtEdit->setFontWeight(QFont::Normal);
//});

Login::~Login()
{
    delete ui;
}

void Login::on_trans_clicked()
{
    quint32 client_key = 0;

    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream << Secret_Request << client_key;
    udpSocket_Client->writeDatagram(array, Server_IP, Server_Port);


}

bool Login::isPrime( int num )
{
     //两个较小数另外处理
     if(num ==2|| num==3 )
                     return true ;
     //不在6的倍数两侧的一定不是质数
     if(num %6!= 1&&num %6!= 5)
                     return false ;
     int tmp =sqrt( num);
     //在6的倍数两侧的也可能不是质数
     for(int i= 5;i <=tmp; i+=6 )
                     if(num %i== 0||num %(i+ 2)==0 )
                                     return false ;
     //排除所有，剩余的是质数
     return true ;
}

void Login::on_Regist_clicked()
{
//    QByteArray array;
//    QDataStream stream(&array, QIODevice::WriteOnly);
//    QString str1 = ui->usrname->toPlainText();
//    QString str2 = ui->password->toPlainText();
//    str1 = str1.right(str1.size() - 1);
//    str2 = str2.right(str2.size() - 1);
////    qDebug() << str1;
////    qDebug() << str2;
////    quint32 passwd = multiMod(str2.toInt(), E, N);
//    stream << Register << str1 << str2; // 第一段内容添加到流中, 第二段是自己的名字

//    udpSocket_Client->writeDatagram(array, Server_IP, Server_Port);
}
