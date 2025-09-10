#include "loginwidget.h"
#include "databasemagr.h"
#include <QApplication>
#include "global.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DataBaseMagr::Instance()->OpenUserDb("user.db");
    DataBaseMagr::Instance()->OpenMessageDb("msg.db");

    LoginWidget w;
    myHelper::FormInCenter(&w);
    w.show();
    return a.exec();
}

