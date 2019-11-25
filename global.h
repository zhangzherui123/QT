#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <login.h>
#include <QToolButton>
#include <QUdpSocket>
#include <QDataStream>
#include <QMap>
#include <qdebug.h>
#include <QSet>
#include "dialoglist.h"
#include "ui_dialoglist.h"
#include <QToolButton>
#include "widget.h"
#include <QMessageBox>
#include <ctime>
#include <QNetworkInterface>

quint32 leastCommonMultiple(quint32 a, quint32 b);
quint32 maxCommonDivisor(quint32 a, quint32 b);

quint32 multiMod(quint32 input, quint32 rate, quint32 y);

quint32 CheckSum(QString str, quint32);
QString CodeMsg(QString str, quint8 key);
QString DeCodeMsg(QString str, quint8 key);
bool isPrime( quint32 num );
QString read_ip_address(QString& net_ip);
QString Get_OriMsg(QByteArray ch);
#endif // GLOBAL_H
