#include "widget.h"
#include "ui_widget.h"
#include <QDataStream>
#include <QMessageBox>
#include <QDateTime>
#include <QColorDialog>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
//#include <QDebug>


Widget::Widget(QWidget *parent, QString name, quint16 port, quint16 NameListSize, quint32 E, quint32 N, QString net_ip) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    qsrand(time(NULL));
    this->net_ip = net_ip;

    isopenCAP = false; //摄像头默认关闭

    // 设置和服务器交互所用的公钥
    this->E = E;
    this->N = N;
    udpSocket = new QUdpSocket(this);

    // 用户名的获取
    uName = name;

    // 端口号
    this->port = port;
    this->NameListSize = NameListSize;
    //绑定端口号
    // 第一个flag表示共享地址和端口, 第二个参数表示断线重连, 这两个一般配套使用
//    udpSocket->bind(port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
//    udpSocket->bind(QHostAddress::LocalHost,port);
//    read_ip_address
    udpSocket->bind(QHostAddress(read_ip_address(net_ip)),port);
//    udpSocket->bind(port);
    // 发送一个新用户进入
    sndMsg(UsrEnter);

    // 点击发送按钮发送信息
    connect(ui->sendBtn, &QPushButton::clicked, [=](){
       sndMsg(Msg);
    });

    // 监听别人发送的消息

    connect(udpSocket, &QUdpSocket::readyRead,this, &Widget::ReceiveMessage);

    // 点击退出的按钮
    connect(ui->exitBtn, &QPushButton::clicked, [=](){

       this->close();
    });


    // 其余的辅助功能
    // 字体改变的时候会触发这个函数
    connect(ui->fontCbx, &QFontComboBox::currentFontChanged, [=](const QFont& font){
        ui->msgTxtEdit->setCurrentFont(font);
        ui->msgTxtEdit->setFocus();

    });

    // 字号改变
    // 因为currentIndexChanged 是被重载的,所以需要特殊处理
    void(QComboBox:: *cbxsignal)(const QString &text) = &QComboBox::currentIndexChanged;
    connect(ui->sizeCbx, cbxsignal, [=](const QString &text){
        ui->msgTxtEdit->setFontPointSize(text.toDouble());
        ui->msgTxtEdit->setFocus();
    });


    // 字体加粗
    connect(ui->boldTBtn, &QToolButton::clicked, [=](bool ischeck){
        if(ischeck)
            ui->msgTxtEdit->setFontWeight(QFont::Bold);
        else
            ui->msgTxtEdit->setFontWeight(QFont::Normal);
    });

    // 字体倾斜
    connect(ui->italicTBtn, &QToolButton::clicked, [=](bool ischeck){
        ui->msgTxtEdit->setFontItalic(ischeck);
    });

    // 下划线
    connect(ui->underlineTBtn, &QToolButton::clicked, [=](bool check){
       ui->msgTxtEdit->setFontUnderline(check);
    });

    // 字体颜色
    connect(ui->colorTBtn, &QToolButton::clicked, [=](){
       QColor color = QColorDialog::getColor(Qt::red);
       ui->msgTxtEdit->setTextColor(color);
    });

    // 清空聊天记录
    connect(ui->clearTBtn, &QToolButton::clicked, [=](){
       ui->msgBrowser->clear();

    });

    // 保存聊天记录
    connect(ui->saveTBtn, &QToolButton::clicked, [=](){


        if(mymap.size() == 0)
        {
            QMessageBox::warning(this, "警告", "聊天框内 不能为空");
            return;
        }
        QString str1 = QStringLiteral("密钥记录");
        QString path = QFileDialog::getSaveFileName(this, str1, "(*.txt)");
        if(path.isEmpty())
        {
            QMessageBox::warning(this, "警告", "路径 不能为空");

            return;
        }
        QFile file(path);
        // 打开模式加换行操作
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream stream(&file);
        for(auto it = mymap.begin(); it != mymap.end(); it++)
        {
            stream << it.key() << "  " << qrand()%3698 << endl;
        }

        file.close();


//        if(ui->msgBrowser->document()->isEmpty())
//        {
//            QMessageBox::warning(this, "警告", "聊天框内 不能为空");
//            return;
//        }
//        QString str1 = QStringLiteral("保存记录");
//        QString str2 = QStringLiteral("聊天记录");
//        QString path = QFileDialog::getSaveFileName(this, str1, str2, "(*.txt)");
        // 如果聊天框是空的, 这里的documnet是另外一种获取文本的方式
//        if(path.isEmpty())
//        {
//            QMessageBox::warning(this, "警告", "路径 不能为空");

//            return;
//        }

//        QFile file(path);
//        // 打开模式加换行操作
//        file.open(QIODevice::WriteOnly | QIODevice::Text);
//        QTextStream stream(&file);
//        stream << ui->msgBrowser->toPlainText();
//        file.close();


    });
}

Widget::~Widget()
{
    delete ui;
}

// 关闭窗口
void Widget::closeEvent(QCloseEvent* e)
{
    emit this->closeWidget();
    sndMsg(UsrLeft);

    // 断开套接字
    udpSocket->close();
    udpSocket->destroyed();

    timer.stop();
    camera.release();
    ui->CAPSHOW0->clear();
    QWidget::closeEvent(e);
}

QString Widget::getMsg() //获取聊天信息
{
    // tohtml 可以实现字体的各种变换
    QString str = ui->msgTxtEdit->toHtml();

    // 清空输入框
    ui->msgTxtEdit->clear();

    // 控制光标回到原始位置
    ui->msgTxtEdit->setFocus();
    return str;
}

void Widget::sndMsg(MsgType type, QByteArray video_msg)//广播UDP消息
{
    // 发送的消息分为三类
    // 发送的数据中需要分段, 第一段是消息的类型, 第二段是消息的数据

    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);

    stream << type << getUsr(); // 第一段内容添加到流中, 第二段是自己的名字


    switch (type) {
    case Msg: //普通的消息发送
    {

        // 如果用户没有输入内容就不能发送
        if(ui->msgTxtEdit->toPlainText() == "")
        {
            // 如果是空的话
            QString str = QStringLiteral("发送内容为空不能发送  ");
            QMessageBox::warning(this, "警告", str);
            return;
        }
        // 再在这里拼接数据
        // CodeMsg 用于对消息进行加密,之后发送
        QString msg = getMsg();
        quint32 checksum = CheckSum(msg,0);
//        udpSocket->writeDatagram(msg, QHostAddress("192.168.81.1"), 6000);
        stream << E << CodeMsg(msg, E) << checksum;
//        stream << getMsg();
        udpSocket->writeDatagram(array, QHostAddress("192.168.81.1"), 6000);
        break;
    }

    case UsrEnter:  //用户进入的消息

        break;

    case UsrLeft:   // 用户离开的消息

        break;

    case CAPMSG:
    {
        stream << video_msg;
        break;
    }
    default:

        break;

    }

    // 准备发送, 广播发送
    for(int i = 0; i < 9; i++)
    {
        udpSocket->writeDatagram(array, QHostAddress::Broadcast, 9000 + i);

//        udpSocket->writeDatagram(array, QHostAddress("10.164.111.168"), 9000 + i);
    }

}

void Widget::ReceiveMessage()   //接受UDP消息
{
    // 解析数据
    // 获取长度
    qDebug() << uName << " ReceiveMessage " ;
    qint64  mysize = udpSocket->pendingDatagramSize();

    QByteArray array = QByteArray(mysize, 0);
    QHostAddress recIP;
    quint16 recPort;
    udpSocket->readDatagram(array.data(), mysize,&recIP, &recPort);
    qDebug() << recIP << " : " <<  recPort;
//    qdebug << recIP.toString() << " " << recPort << endl;
    //qDebug("ip = %s , port = %d \r\n", recIP.toString(), recPort);
    // 解析第一段
    QDataStream stream(&array, QIODevice::ReadOnly);

    int msgType;
    stream >> msgType;

    QString usrname;
    QString msg;


    // 获取当前的时间
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    // 得到类型之后对具体的类型进行处理
    switch (msgType) {

    case Msg:
    {
        quint32 key;
        quint32 CheckSum;
//        stream >> usrname >> msg;
        stream >> usrname >> key >> msg >> CheckSum;
        mymap.insert(usrname, key);
//        QString orinal = msg;
        msg = DeCodeMsg(msg, key);
//        qDebug() << " start " << msg << " end";
        QString orinal = Get_OriMsg(msg.toUtf8());
        quint32 check = 0;
        QByteArray ch = msg.toUtf8();
        for(int i = 0; i < msg.size(); i++)
        {
            check = (ch[i] + check)%msg.size();

        }
        // 对数据进行校验,如果校验成功则打印
        if(CheckSum == check)
        {
            qDebug() << " Data Is checked ";
        }
        // 追加聊天记录
        ui->msgBrowser->setTextColor(Qt::blue);
        ui->msgBrowser->append("[" + usrname + "]" + time);
        ui->msgBrowser->append(msg + "(" + orinal +")");
        break;
    }


    case UsrEnter:
    {
        // 更新右侧的在线列表
        stream >> usrname;
        usrEnter(usrname);
        break;
    }


    case UsrLeft:
        stream >> usrname;
        usrLeft(usrname, time);
        break;

    case CAPMSG:
    {
        qDebug() << uName << " CAPMSG " ;
        stream >> usrname;
        QByteArray video_msg;
        stream >> video_msg;
        if(isopenCAP)
        {


            QBuffer buffer(&video_msg);
        //    qDebug() << buff;
            //提供用Qbytearray读写内存中IO缓存区的接口，若无传参内部默认自动创建一个Qbytearray；
            //对IO缓存区读写操作等效于像IO设备读写数据
            //操作QBuffer像在操作文件（其实原理都差不多，各自都是内存中一块特殊区域嘛）

            QImageReader reader(&buffer,"JPG");
            //可读入磁盘文件、设备文件中的图像、以及其他图像数据如pixmap和image，相比较更加专业。
            //buffer属于设备文件一类，
            //专业地读取设备文件的图像数据。


            QImage image = reader.read();
            //read()方法用来读取设备图像，也可读取视频，读取成功返回QImage*，否则返回NULL
            //格式转换
            //ui->CAPSHOW0->setPixmap(QPixmap::fromImage(image));
            //ui->CAPSHOW0->resize(image.width(),image.height());
            switch (recPort) {
            case 9000:
            {
                ui->CAPSHOW0->setPixmap(QPixmap::fromImage(image));
                ui->CAPSHOW0->resize(image.width(),image.height());
                break;
            }
            case 9001:
            {
                ui->CAPSHOW1->setPixmap(QPixmap::fromImage(image));
                ui->CAPSHOW1->resize(image.width(),image.height());
                break;
            }
            case 9002:
            {
                ui->CAPSHOW2->setPixmap(QPixmap::fromImage(image));
                ui->CAPSHOW2->resize(image.width(),image.height());
                break;
            }
            case 9003:
            {
                ui->CAPSHOW3->setPixmap(QPixmap::fromImage(image));
                ui->CAPSHOW3->resize(image.width(),image.height());
                break;
            }
            case 9004:
            {
                ui->CAPSHOW4->setPixmap(QPixmap::fromImage(image));
                ui->CAPSHOW4->resize(image.width(),image.height());
                break;
            }
            case 9005:
            {
                ui->CAPSHOW5->setPixmap(QPixmap::fromImage(image));
                ui->CAPSHOW5->resize(image.width(),image.height());
                break;
            }
            case 9006:
            {
                ui->CAPSHOW6->setPixmap(QPixmap::fromImage(image));
                ui->CAPSHOW6->resize(image.width(),image.height());
                break;
            }
            case 9007:
            {
                ui->CAPSHOW7->setPixmap(QPixmap::fromImage(image));
                ui->CAPSHOW7->resize(image.width(),image.height());
                break;
            }
            case 9008:
            {
                ui->CAPSHOW8->setPixmap(QPixmap::fromImage(image));
                ui->CAPSHOW8->resize(image.width(),image.height());
                break;
            }

            default:
                break;
            }
            break;
        }
        else
        {
            break;
        }
    }

    default:
        break;
    }
}

void Widget::usrEnter(QString usrname)
{
    bool isempty = false;
    isempty = ui->usrTb1Widget->findItems(usrname, Qt::MatchExactly).isEmpty();
    if(isempty)
    {
        // 如果列表中没有这一项的话
        QTableWidgetItem* usr = new QTableWidgetItem(usrname);
        // 插入上面的item
        ui->usrTb1Widget->insertRow(0);
        ui->usrTb1Widget->setItem(0,0,usr);

        // 插入完成

        // 追加聊天记录
        ui->msgBrowser->setTextColor(Qt::gray);
//        QString::fromLocal8Bit("%1 上线了  ").arg(usrname);

        ui->msgBrowser->append( QStringLiteral("%1 上线了  ").arg(usrname));
//        ui->msgBrowser->append( QString::fromLocal8Bit("%1 上线了  ").arg(usrname));
        // 更新在线人数
        ui->usrNumLb1->setText(QStringLiteral("在线用户: %1人 ").arg(ui->usrTb1Widget->rowCount()));

        // 把自身信息广播出去
        sndMsg(UsrEnter);



    }
}

void Widget::usrLeft(QString usrname,QString time) //处理用户离开
{
   // 更新右侧的用户列表
    bool isempty = ui->usrTb1Widget->findItems(usrname, Qt::MatchExactly).isEmpty();
    if(!isempty)
    {
        int row = ui->usrTb1Widget->findItems(usrname, Qt::MatchExactly).first()->row();
        ui->usrTb1Widget->removeRow(row);

        // 追加聊天记录
        ui->msgBrowser->setTextColor(Qt::gray);
        ui->msgBrowser->append(QStringLiteral("%1 于 %2 离开本聊天室").arg(usrname).arg(time));

        // 更新在线人数
        ui->usrNumLb1->setText(QStringLiteral("在线用户: %1人 ").arg(ui->usrTb1Widget->rowCount()));
    }

}

QString Widget::getUsr()
{
    return this->uName;
}

void Widget::on_openCAP_clicked()
{
    isopenCAP = true;
}

void Widget::on_send_video_clicked()
{
    camera.open(0);
    connect(&timer,SIGNAL(timeout()),this,SLOT(video_capture_send()));
    timer.start(33);//结合上面，每隔33ms截屏一次并发送

}


void Widget::video_capture_send()
{
    Mat frame;
    //mat也是动态管理内存的数据类型
    //也会尽量避免重复数据拷贝，（通过mat头），节省内存
    //mat是三维数组，但也可通过二维数组初始化
    camera.read(frame);

    cvtColor(frame,frame,CV_BGR2RGB); //BGRtoRGB
    //转换为常规的格式RGB

    QImage image((unsigned char *)(frame.data),
                 frame.cols,frame.rows,
                 QImage::Format_RGB888);

    //图像首地址是unsigned char *格式
    //专用于读取IO图片数据
    //Qpixmap专用于屏幕显示
    //Qpicture是专用于执行QPianter指令作图的。

//    ui->label->setPixmap(QPixmap::fromImage(image));
//    ui->label->resize(image.width(),image.height());
    //   进一步转换为Qpixmap用于屏幕显示
    //调整为适应图像大小

    QByteArray byte;
    //字节数组 要进行传输必须先转换成这个格式
    QBuffer buff(&byte);
    // 建立一个用于IO读写（网卡）的缓冲区
    //buff作为byte和IO设备的接口（显式）
    // byte在内部作为一个与image的接口（隐式）
    //操作都是对buff进行的，byte被隐藏
    image.save(&buff,"JPEG");
    // image先向下转为byte的类型，再存入buff

//    QByteArray ss = qCompress(byte,5);
    //数据压缩算法，压缩比为5
//    QByteArray vv = ss.toBase64();
    //数据加密算法
//    sender.writeDatagram(byte,dstip,dstport);
    sndMsg(CAPMSG, byte);
}

void Widget::on_pushButton_clicked()
{
    timer.stop();
    camera.release();
    ui->CAPSHOW0->clear();
    ui->CAPSHOW1->clear();
    ui->CAPSHOW2->clear();
    ui->CAPSHOW3->clear();
    ui->CAPSHOW4->clear();
    ui->CAPSHOW5->clear();
    ui->CAPSHOW6->clear();
    ui->CAPSHOW7->clear();
    ui->CAPSHOW8->clear();
}

void Widget::on_pushButton_2_clicked()
{
    isopenCAP =false;
    ui->CAPSHOW0->clear();
    ui->CAPSHOW1->clear();
    ui->CAPSHOW2->clear();
    ui->CAPSHOW3->clear();
    ui->CAPSHOW4->clear();
    ui->CAPSHOW5->clear();
    ui->CAPSHOW6->clear();
    ui->CAPSHOW7->clear();
    ui->CAPSHOW8->clear();
}
