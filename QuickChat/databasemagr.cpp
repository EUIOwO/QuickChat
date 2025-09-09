#include "databasemagr.h"
#include <QtCore>

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

bool DataBaseMagr::isMyFriend(const int &userId, const QString &name)
{
    QString strQuery = "SELETE [id] FROM FRIEND";
    strQuery.append(QString(" WHERE name = %1").arg(name));
    strQuery.append(QString(" AND userId = %1").arg(QString::number(userId)));

    QSqlQuery query(strQuery, userdb);

    return query.next();
}

QJsonArray DataBaseMagr::GetMyFriend(const int &userId)
{
    QJsonArray myFriends;
    QString strQuery = "SELETE [id] FROM FRIEND";
    strQuery.append(QString(" AND userId = %1").arg(QString::number(userId)));

    QSqlQuery query(strQuery, userdb);
    while(query.next()){
        myFriends.append(query.value("id").toInt());
    }

    return myFriends;
}

void DataBaseMagr::AddFriend(const int &friendId, const int userId, const QString &name)
{
    QString strQuery = "SELETE [id] FROM FRIEND";
    strQuery.append(QString(" WHERE id = %1").arg(QString::number(friendId)));
    strQuery.append(QString(" AND userId = ").arg(QString::number(userId)));
    strQuery.append(QString());

    QSqlQuery query(strQuery, userdb);
    if(query.next()){
        //该用户有此好友了，不需要参加
        qDebug() << "好友已存在" << endl;
        return;

    }
    query.prepare("INSERT INTO FRIEND(id, userId, name) valus(?, ?, ?)");
    query.bindValue(0, friendId);
    query.bindValue(1,userId);
    query.bindValue(2, name);

    query.exec();
}
