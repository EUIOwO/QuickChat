#include "clientsocket.h"
#include "databasemagr.h"

#include <QFile>
#include <QDebug>
#include <QHostAddress>
#include <QDataStream>
#include <QApplication>
#include <QtCore>
#include "unit.h"
#include "myapp.h"

//#define SERVER_IP "192.168.2.104"
//#define SERVER_PORT 60100

ClientSocket::ClientSocket(QObject *parent) :
    QObject(parent)
{
    m_nId = -1;

    //创建tcpSocket
    m_tcpSocket = new QTcpSocket(this);

    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(SltReadyRead()));
    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(SltConnected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(SltDisconnected()));
}

ClientSocket::~ClientSocket()
{
}

/**
 * @brief ClientSocket::GetUserId
 * 获取当前用户id
 * @return
 */
int ClientSocket::GetUserId() const
{
    return  m_nId;
}

/**
 * @brief ClientSocket::SetUserId
 * 设置当前socket的用户id
 * @param id
 */
void ClientSocket::SetUserId(const int &id)
{
    m_nId = id;
}

void ClientSocket::CheckConnected()
{
    if (m_tcpSocket->state() != QTcpSocket::ConnectedState)
    {
        m_tcpSocket->connectToHost(MyApp::m_strHostAddr, MyApp::m_nMsgPort);
    }
}

/**
 * @brief ClientSocket::ColseConnected
 * 主动断开连接
 */
void ClientSocket::ColseConnected()
{
    if (m_tcpSocket->isOpen()) {
        m_tcpSocket->abort();
    }
}

/**
 * @brief ClientSocket::ConnectToHost
 * 链接服务器接口
 * @param host
 * @param port
 */
void ClientSocket::ConnectToHost(const QString &host, const int &port)
{
    if (m_tcpSocket->isOpen()) m_tcpSocket->abort();
    m_tcpSocket->connectToHost(host, port);
}

/**
 * @brief ClientSocket::ConnectToHost
 * 链接服务器接口
 * @param host
 * @param port
 */
void ClientSocket::ConnectToHost(const QHostAddress &host, const int &port)
{
    if (m_tcpSocket->isOpen()) {
        m_tcpSocket->abort();
    }
    m_tcpSocket->connectToHost(host, port);
}



/**
 * @brief ClientSocket::SltSendMessage
 * tcp socket消息管理
 * @param type
 * @param dataVal
 */
void ClientSocket::SltSendMessage(const quint8 &type, const QJsonValue &dataVal)
{
    // 连接服务器
    if (!m_tcpSocket->isOpen()) {
        m_tcpSocket->connectToHost(MyApp::m_strHostAddr, MyApp::m_nMsgPort);
        m_tcpSocket->waitForConnected(1000);
    }
    // 超时1s后还是连接不上，直接返回
    if (!m_tcpSocket->isOpen()) {
        return;
    }

    // 构建 Json 对象
    QJsonObject json;
    json.insert("type", type);
    json.insert("from", m_nId);
    json.insert("data", dataVal);

    // 构建 Json 文档
    QJsonDocument document;
    document.setObject(json);
    m_tcpSocket->write(document.toJson(QJsonDocument::Compact));

    qDebug() << "m_tcpSocket->write:" << document.toJson(QJsonDocument::Compact);


}

/**
 * @brief ClientSocket::SltDisconnected
 */
void ClientSocket::SltDisconnected()
{
    qDebug() << "has disconnecetd";
    m_tcpSocket->abort();
}

/**
 * @brief ClientSocket::SltConnected
 */
void ClientSocket::SltConnected()
{
    qDebug() << "has connecetd";
    emit signalStatus(ConnectedHost);
}


/**
 * @brief ClientSocket::SltReadyRead
 * 服务器消息处理
 */
void ClientSocket:: SltReadyRead()
{
    //读取socket数据
   QByteArray byRead = m_tcpSocket->readAll();
   QJsonParseError jsonError;
   //转换为JSON文档
   QJsonDocument document = QJsonDocument::fromJson(byRead, &jsonError);

   //解析未发生错误
   if(!document.isNull() && (jsonError.error == QJsonParseError::NoError)){
        //json文档对象
       if(document.isObject()){
          //转化为对象
           QJsonObject jsonObj = document.object();
           QJsonValue dataVal = jsonObj.value("data");
           int nFrom = jsonObj.value("from").toInt();
           int nType = jsonObj.value("type").toInt();

           switch (nType) {
               case Register:
                   //注册
                   break;
               case Login:   //17
                   //登录
                   ParseLogin(dataVal);
               break;
               case AddFriend:
                   emit signalMessage(AddFriend, dataVal);
               break;
               case AddFriendRequist:
                   emit signalMessage(AddFriendRequist, dataVal);
               break;
               case AddGroup:
                   emit signalMessage(AddGroup, dataVal);
               break;
               case AddGroupRequist:
               {
                   emit signalMessage(AddGroupRequist, dataVal);
               }
                   break;
               case CreateGroup:
               {
                   emit signalMessage(CreateGroup, dataVal);
               }
                   break;
           case GetMyGroups:
           {
               emit signalMessage(GetMyGroups, dataVal);
           }
               break;
           case RefreshGroups:
           {
               emit signalMessage(RefreshGroups, dataVal);
           }
               break;
           case SendGroupMsg:
           {
               emit signalMessage(SendGroupMsg, dataVal);
           }
               break;
               case SendMsg:
               {
                   emit signalMessage(SendMsg, dataVal);
               }
               break;
               case SendFace:
               {
                   emit signalMessage(SendFace, dataVal);
               }
               break;
               case SendFile:
               {
                   emit signalMessage(SendFace, dataVal);
               }
               break;
           }
       }
   }
}

//用户登录
void ClientSocket::ParseLogin(const QJsonValue &dataVal)
{
    QJsonObject dataObj = dataVal.toObject();
    int id = dataObj.value("id").toInt();
    int code = dataObj.value("code").toInt();
    QString msg = dataObj.value("msg").toString();
    if(code == -2){
        qDebug() << "用户已在线" << endl;
        emit signalStatus(LoginRepeat);
        m_nId = id;
    }else if(code == -1){
        qDebug() << "用户未注册" << endl;

        emit signalStatus(LoginPasswdError);
    }else if(code == 0 && msg == "ok"){
        m_nId = id;
        MyApp::m_nId = id;

        qDebug() << "登录成功" << endl;

        emit signalStatus(LoginSuccess);
    }
}

//文件传输类
ClientFileSocket::ClientFileSocket(QObject *parent) :
    QObject(parent)
{
    m_nType = Login;

    m_strFilePath = MyApp::m_strRecvPath;

    InitSocket();
}

ClientFileSocket::~ClientFileSocket()
{
}

bool ClientFileSocket::isConneciton()
{
    return m_tcpSocket->isOpen();
}


void ClientFileSocket::displayError(QAbstractSocket::SocketError)
{
    m_tcpSocket->close();
}


/**
 * @brief ClientFileSocket::StartTransferFile
 * 开始传输文件
 * @param id 当前用户id
 * @param fileName 文件路径
 */
void ClientFileSocket::StartTransferFile(QString fileName)
{
    if (m_bBusy) return;

    // 如果没有连接服务器，重新连接下
    if (!m_tcpSocket->isOpen()) {
        ConnectToServer(MyApp::m_strHostAddr, MyApp::m_nFilePort, m_nWinId);
    }
    m_nType = SendFile;

    // 要发送的文件
    fileToSend = new QFile(fileName);

    if (!fileToSend->open(QFile::ReadOnly))
    {
        qDebug() << "open file error!";
        return;
    }

    // 文件总大小
    ullSendTotalBytes = fileToSend->size();

    // 文件数据流
    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_8);

    // 当前文件名，不包含路径
    QString currentFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);

    // 依次写入总大小信息空间，文件名大小信息空间，文件名
    sendOut << qint64(0) << qint64(0) << currentFileName;

    // 这里的总大小是文件名大小等信息和实际文件大小的总和
    ullSendTotalBytes += outBlock.size();

    // 返回outBolock的开始，用实际的大小信息代替两个qint64(0)空间
    sendOut.device()->seek(0);
    sendOut << ullSendTotalBytes << qint64((outBlock.size() - sizeof(qint64)*2));

    // 发送完头数据后剩余数据的大小
    bytesToWrite = ullSendTotalBytes - m_tcpSocket->write(outBlock);

    outBlock.resize(0);
    m_bBusy = true;
}

/**
 * @brief ClientFileSocket::ConnectToServer
 * 连接服务器
 * @param ip
 */
void ClientFileSocket::ConnectToServer(const QString &ip, const int &port, const int &winId)
{
    if (m_tcpSocket->isOpen()) return;
    m_nWinId = winId;
    m_tcpSocket->connectToHost(QHostAddress(ip), port);
}

/**
 * @brief ClientFileSocket::CloseConnection
 * 断开连接
 */
void ClientFileSocket::CloseConnection()
{
    // 还原变量
    FileTransFinished();
    // 主动断开
    m_tcpSocket->abort();
}

/**
 * @brief ClientFileSocket::FileTransFinished
 * 文件接收或发送完成的数据还原
 */
void ClientFileSocket::FileTransFinished()
{
    m_bBusy = false;

    ullSendTotalBytes   = 0;
    ullRecvTotalBytes   = 0;
    bytesWritten        = 0;
    bytesToWrite        = 0;
    bytesReceived       = 0;

    fileNameSize        = 0;
}

/**
 * @brief ClientFileSocket::SetUserId
 * @param id
 */
void ClientFileSocket::SetUserId(const int &id)
{
    m_nWinId = id;
}

/**
 * @brief ClientFileSocket::InitSocket
 */
void ClientFileSocket::InitSocket()
{
    // 将整个大的文件分成很多小的部分进行发送，每部分为50字节
    loadSize            = 50 * 1024;
    ullSendTotalBytes   = 0;
    ullRecvTotalBytes   = 0;
    bytesWritten        = 0;
    bytesToWrite        = 0;
    bytesReceived       = 0;

    m_nWinId            = -1;

    fileNameSize        = 0;
    m_bBusy = false;

    fileToSend = new QFile(this);
    fileToRecv = new QFile(this);

    m_tcpSocket = new QTcpSocket(this);

    // 当有数据发送成功时，我们更新进度条
    connect(m_tcpSocket, SIGNAL(bytesWritten(qint64)),
            this, SLOT(SltUpdateClientProgress(qint64)));
    // 当有数据接收成功时，我们更新进度条
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(SltReadyRead()));
    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(SltConnected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(SltDisConnected()));
}


/**
 * @brief ClientFileSocket::SltUpdateClientProgress
 * 更新文件的上传、下载进度
 * @param numBytes
 */
void ClientFileSocket::SltUpdateClientProgress(qint64 numBytes)
{
    // 开始的时候，不记录流程
    if (Login == m_nType) {
        m_nType = Unknow;
        return;
    }

    // 已经发送数据的大小
    bytesWritten += (int)numBytes;
    if (bytesToWrite > 0) //如果已经发送了数据
    {
        // 每次发送loadSize大小的数据，这里设置为4KB，如果剩余的数据不足4KB，就发送剩余数据的大小
        outBlock = fileToSend->read(qMin(bytesToWrite, loadSize));

        // 发送完一次数据后还剩余数据的大小
        bytesToWrite -= (int)m_tcpSocket->write(outBlock);

        // 清空发送缓冲区
        outBlock.resize(0);
    }
    else
    {
        // 如果没有发送任何数据，则关闭文件
        if (fileToSend->isOpen())
            fileToSend->close();
    }

    // 发送进度信息
    Q_EMIT signalUpdateProgress(bytesWritten, ullSendTotalBytes);

    // 发送完毕
    if (bytesWritten >= ullSendTotalBytes)
    {
        if (fileToSend->isOpen())
            fileToSend->close();

        bytesWritten = 0;
        ullSendTotalBytes = 0;
        bytesToWrite = 0;
        // 文件发送完成，复位相关信息
        Q_EMIT signalSendFinished();

        FileTransFinished();
    }
}

// 更新进度条，实现文件的接收
void ClientFileSocket::SltReadyRead()
{
    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_4_8);

    // 如果接收到的数据小于等于20个字节，那么是刚开始接收数据，我们保存为头文件信息
    if (bytesReceived <= (sizeof(qint64)*2))
    {
        int nlen = sizeof(qint64) * 2;
        // 接收数据总大小信息和文件名大小信息
        if ((m_tcpSocket->bytesAvailable() >= nlen) && (fileNameSize == 0))
        {
            in >> ullRecvTotalBytes >> fileNameSize;
            if (0 != ullRecvTotalBytes) bytesReceived += nlen;
        }

        // 接收文件名，并建立文件
        if((m_tcpSocket->bytesAvailable() >= (qint64)fileNameSize) &&
                ((qint64)fileNameSize != 0) &&
                (0 != ullRecvTotalBytes))
        {
            in >> fileReadName;
            fileReadName = (-2 == m_nWinId ? MyApp::m_strHeadPath : MyApp::m_strRecvPath) + fileReadName;
            bytesReceived += fileNameSize;

            fileToRecv->setFileName(fileReadName);

            if (!fileToRecv->open(QFile::WriteOnly | QIODevice::Truncate))
            {
                qDebug() << "recv file open failed" << fileReadName;
                return;
            }

            qDebug() << "Begin to recv file" << m_nWinId << fileReadName;
        }
    }

    // 如果接收的数据小于总数据，那么写入文件
    if (bytesReceived < ullRecvTotalBytes)
    {
        bytesReceived += m_tcpSocket->bytesAvailable();
        inBlock = m_tcpSocket->readAll();

        if (fileToRecv->isOpen())
            fileToRecv->write(inBlock);

        inBlock.resize(0);
    }

    // 更新进度条
    Q_EMIT signalUpdateProgress(bytesReceived, ullRecvTotalBytes);

    // 接收数据完成时
    if ((bytesReceived >= ullRecvTotalBytes) && (0 != ullRecvTotalBytes))
    {
        fileToRecv->close();
        bytesReceived = 0; // clear for next receive
        ullRecvTotalBytes = 0;
        fileNameSize = 0;

        Q_EMIT signamFileRecvOk(Text, fileToRecv->fileName());
        qDebug() << "File recv ok" << fileToRecv->fileName();
        // 数据接受完成
        FileTransFinished();
    }
}

/**
 * @brief ClientFileSocket::SltConnected
 * 连接后，发送本机ID至服务器
 */
void ClientFileSocket::SltConnected()
{
    m_nType = Login;

    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_8);

    // 给服务器socket上报自己的id，方便下次查询
    sendOut << qint32(MyApp::m_nId) << qint32(m_nWinId);

    // 发送完头数据后剩余数据的大小
    m_tcpSocket->write(outBlock);

    // 发送连接上的信号
    Q_EMIT signalConnectd();
}

void ClientFileSocket::SltDisConnected()
{
    if (m_tcpSocket->isOpen()) m_tcpSocket->close();
}

