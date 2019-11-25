#include "global.h"


// 最大公约数
quint32 maxCommonDivisor(quint32 a, quint32 b) {
    quint32 temp = a;
    if (a < b)
    {
        a = b;
        b = temp;
    }

    while(a % b) {
        temp = b;
        b = a % b;
        a = temp;
    }
    return b;
}

// 最小公倍数
quint32 leastCommonMultiple(quint32 a, quint32 b) {
    quint32 macDivisor = maxCommonDivisor(a, b);
    return a / macDivisor  *  b;
}

// 计算 input ^ rate mod y
quint32 multiMod(quint32 a, quint32 b, quint32 n)
{
    quint32 y=1;
    while(b != 0)
    {
        /*对于b中的每个1，累加y*/
        if(b & 1)
            y = (y*a) % n;
        a = (a*a) % n;
        b = b>>1;
    }
    return y;
}

QString CodeMsg(QString str, quint8 key)
{
    //char* ch = (char *)str.toStdString().c_str();
    QByteArray ch = str.toUtf8();
    for(int i = 0; i < str.size(); i++)
    {
        ch[i] = ch[i] ^ key;

    }
//    str.prepend(ch);
    return ch;

}

QString DeCodeMsg(QString str, quint8 key)
{
    QByteArray ch = str.toUtf8();
    for(int i = 0; i < str.size(); i++)
    {
        ch[i] = ch[i] ^ key;

    }
//    str.prepend(ch);
    return ch;

}
QString Get_OriMsg(QByteArray ch)
{
    return ch.toBase64();
}

quint32 CheckSum(QString str, quint32)
{
    quint32 ret = 0;
    QByteArray ch = str.toUtf8();
    for(int i = 0; i < str.size(); i++)
    {
        ret = (ch[i] + ret)%str.size();

    }
//    str.prepend(ch);
    return ret;

}

bool isPrime( quint32 num )
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


QString read_ip_address(QString& net_ip)
{
    QString ip_address;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (int i = 0; i < ipAddressesList.size(); ++i)
    {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&  ipAddressesList.at(i).toIPv4Address())
        {

            ip_address = ipAddressesList.at(i).toString();
            if(ip_address.indexOf(net_ip) != -1)
            {
                return ip_address;
            }
//            qDebug()<<ip_address;  //debug
            //break;
        }
    }
    if (ip_address.isEmpty())
        ip_address = QHostAddress(QHostAddress::LocalHost).toString();
    return ip_address;
}
