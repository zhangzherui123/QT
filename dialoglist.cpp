#include "dialoglist.h"
#include "ui_dialoglist.h"
#include <QToolButton>
#include "widget.h"
#include <QMessageBox>




DialogList::DialogList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialogList)
{
    ui->setupUi(this);

    // 先把自己的密钥加入到内存池
    myset.insert(server_Key);
    ui->severip_text->setText("192.168.81.1");
    ui->net_ip->setText("192.168.81.");
    this->Server_IP = QHostAddress("192.168.81.1");
    this->net_ip = ui->net_ip->text();
    // 创建服务器
    //udpSocket_Server = new QUdpSocket(this);
    //udpSocket_Server->bind(6000, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    //connect(udpSocket_Server, &QUdpSocket::readyRead,this, &DialogList::ReceiveMessage);

//  设置标题
    setWindowTitle("Zhang Zhe Rui's QQ");
//  设置图标
    setWindowIcon(QPixmap(":/images/qq.png"));

    // 设置所有的图标
    QList<QString> nameList;
    nameList.append("Aggie");
    nameList.append("mike");
    nameList.append("Blossom");
    nameList.append("Cherry");
    nameList.append("Cady");
    nameList.append("girl");
    nameList.append("Dabria");
    nameList.append("Ebony");
    nameList.append("Faith");

    QStringList iconNameList; //图标资源列表
    iconNameList << "ftbz"<< "ymrl" <<"qq" <<"Cherry"<< "dr"
                 <<"jj"<<"lswh"<<"qmnn"<<"wy";




    for(int i = 0; i < nameList.size(); i++)
    {
        //  设置头像
        QToolButton* btn = new QToolButton;
        // 设置文字
        btn->setText(nameList[i]);
        // 设置头像
        QString str = QString(":/images/%1.png").arg(iconNameList.at(i));
        btn->setIcon(QPixmap(str));
        // 设置头像的大小
        btn->setIconSize(QPixmap(":/images/ftbz.png").size());
        // 设置按钮的风格
        btn->setAutoRaise(true);
        // 设置文字和图片一起显示
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        // 加到垂直布局中
        ui->vLayout->addWidget(btn);

        vToolBtn.push_back(btn);
        // 加标识(是否弹出)
        isShow.push_back(false);
        mymap.insert(nameList[i], (100*(i+1) + 10*(i+1) + i + 1));
    }

    // 对9个按钮添加信号槽
    for(int i = 0; i < vToolBtn.size(); i++)
    {
        connect(vToolBtn[i], &QToolButton::clicked, [=]() mutable{

            Login *mylogin = new Login(Server_IP, 6000, 9000 + i);
            // 绑定事件
            connect(mylogin, &Login::LoginSuccess_Signal, [=](){
                if(isShow[i])
                {
                        QString str = QStringLiteral(" %1 的窗口已经被打开, 请勿重复打开").arg(vToolBtn[i]->text());
                        qDebug() << " error";
                        QMessageBox::warning(this, "警告", str);
                    return;
                }


                isShow[i] = true;
                // 弹出一个对话框
                // 参数一: 顶层的方式弹出
                // 参数二: 窗口的名字
                //
                Widget* widget = new Widget(0, vToolBtn[i]->text(), 9000 + i, vToolBtn.size(), mylogin->E, mylogin->N, this->net_ip);
                // 设置窗口的名称
                widget->setWindowTitle(vToolBtn[i]->text());
                // 设置窗口的头像
                widget->setWindowIcon(vToolBtn[i]->icon());
                // 主动的让他显示
                widget->show();

                connect(widget, &Widget::closeWidget, [=](){
                    isShow[i] = false;

                    QByteArray array;
                    QDataStream stream(&array, QIODevice::WriteOnly);
                    stream << mylogin->Widget_Exit << vToolBtn[i]->text();
                    mylogin->udpSocket_Client->writeDatagram(array, mylogin->Server_IP, mylogin->Server_Port);

                });
            });
            // 设置窗口的名称
            mylogin->setWindowTitle("登录窗口");
//            QIcon tmp(":/images/timg.png");
            mylogin->setWindowIcon(vToolBtn[2]->icon());
            mylogin->setFixedSize(394, 230);
            mylogin->show();
//            mylogin->set_KEY(E, N);

        });



    }

}


DialogList::~DialogList()
{
    delete ui;
}




//void DialogList::on_pushButton_clicked()
//{
//    qDebug() << " ??? " << endl;
//    this->Server_IP = QHostAddress(ui->severip_text->text());
//    QString str = QStringLiteral("绑定成功");
//    QMessageBox::warning(this, "警告", str);

//}

void DialogList::on_serverIP_clicked()
{
//        qDebug() << " ??? " << endl;
        this->Server_IP = QHostAddress(ui->severip_text->text());
        QString str = QStringLiteral("Set server address successfully");
        QMessageBox::warning(this, "警告", str);
}

void DialogList::on_serverIP_2_clicked()
{
    this->net_ip = ui->severip_text->text();
    QString str = QStringLiteral("Set Net successfully");
    QMessageBox::warning(this, "警告", str);
}
