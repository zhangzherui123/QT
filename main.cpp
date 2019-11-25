#include "widget.h"
#include <QApplication>
#include "dialoglist.h"
#include "widget.h"
#include "login.h"




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DialogList d;
    d.show();

    return a.exec();
}


