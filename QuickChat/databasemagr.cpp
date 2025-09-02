#include "databasemagr.h"

#include <QDebug>

DataBaseMagr *DataBaseMagr::self = NULL;

DataBaseMagr::DataBaseMagr(QObject *parent) :
    QObject(parent)
{

}

DataBaseMagr::~DataBaseMagr()
{
    if (userdb.isOpen()) {
        userdb.close();
    }
}

/**
 * @brief DataBaseMagr::OpenDb
 * 打开数据库
 */
bool DataBaseMagr::OpenUserDb(const QString &dataName)
{
    userdb = QSqlDatabase::addDatabase("QSQLITE", "connectionUser");
    userdb.setDatabaseName(dataName);
    if (!userdb.open()) {
        qDebug() << "Open sql failed";
        return false;
    }

    // 添加数据表
    QSqlQuery query(userdb);

    // 创建我的好友表 id为好友id，userid为当前用户id
    query.exec("CREATE TABLE FRIEND (id INT, userId INT, name varchar(50))");

    // 创建群组表 id为群组id，userid为当前用户id
    query.exec("CREATE TABLE MYGROUP (id INT, userId INT, name varchar(50))");

    // 用户数据保存
    query.exec("CREATE TABLE USERINFO (id INT, name varchar(50), passwd varchar(50))");

    return true;
}
