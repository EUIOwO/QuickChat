#include "loginwidget.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginWidget w;
    w.show();
    return a.exec();

//    QApplication a(argc, argv);
//    MainWindow m;
//    m.show();
//    return a.exec();
}

