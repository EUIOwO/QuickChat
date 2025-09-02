#ifndef DATABASEMAGR_H
#define DATABASEMAGR_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMutex>


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


signals:

public slots:

private:
    static DataBaseMagr *self;

    // 数据库管理
    QSqlDatabase userdb;
};

#endif // DATABASEMAGR_H
