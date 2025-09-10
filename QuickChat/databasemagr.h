#ifndef DATABASEMAGR_H
#define DATABASEMAGR_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMutex>
#include "iteminfo.h"

/// 客户端数据库管理类
class DataBaseMagr : public QObject
{
    Q_OBJECT
private:
    explicit DataBaseMagr(QObject *parent = 0);
    ~DataBaseMagr();

public:
    bool OpenUserDb(const QString &dataName);

    // 单实例运行
    static DataBaseMagr *Instance()
    {
        static QMutex mutex;
        if (NULL == self) {
            QMutexLocker locker(&mutex);

            if (!self) {
                self = new DataBaseMagr();
            }
        }

        return self;
    }

    //是否是我的好友
    bool isMyFriend(const int &userId, const QString &name);
    //获取好友列表
    QJsonArray GetMyFriend(const int &userId);
    //添加好友
    void AddFriend(const int &friendId, const int userId, const QString &name);
    // 获取历史聊天记录
    QVector<ItemInfo *> QueryHistory(const int &id, const int &count = 0);
    // 添加历史聊天记录
    void AddHistoryMsg(const int &userId, ItemInfo *itemInfo);
    //打开消息数据库
    bool OpenMessageDb(const QString &dataName);

signals:

public slots:

private:
    static DataBaseMagr *self;

    // 数据库管理
    QSqlDatabase userdb;
    QSqlDatabase msgdb;
};

#endif // DATABASEMAGR_H
