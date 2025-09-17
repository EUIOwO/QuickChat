#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>

/// socket通信类
class ClientSocket : public QObject
{
    Q_OBJECT
public:
    explicit ClientSocket(QObject *parent = 0);
    ~ClientSocket();

    // 获取当前用户的ID
    int GetUserId() const;
    void SetUserId(const int &id);

    void CheckConnected();
    void ColseConnected();

    // 连接服务器
    void ConnectToHost(const QString &host, const int &port);
    void ConnectToHost(const QHostAddress &host, const int &port);




signals:
    void signalMessage(const quint8 &type, const QJsonValue &dataVal);
    void signalStatus(const quint8 &state);
public slots:
    // socket消息发送封装
    void SltSendMessage(const quint8 &type, const QJsonValue &dataVal);
private:
    // tcpsocket
    QTcpSocket *m_tcpSocket;
    int m_nId;//用户id
private slots:
    // 与服务器断开链接
    void SltDisconnected();
    // 链接上服务器
    void SltConnected();
    // tcp消息处理
    void SltReadyRead();


private:
    void ParseLogin(const QJsonValue &dataVal);
private:


};


#endif //TCPCLIENT_H
