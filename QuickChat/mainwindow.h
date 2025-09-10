#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "customwidget.h"
#include "clientsocket.h"
#include "qqcell.h"
#include <QMainWindow>
#include <QButtonGroup>
#include <QSystemTrayIcon>
#include <QtCore>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public CustomMoveWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void SetSocket(ClientSocket *tcpSocket, const QString &name);

private slots:
    void SltMainPageChanged(int index);

    //托盘菜单处理
    void SltTrayIcoClicked(QSystemTrayIcon::ActivationReason reason);
    void SltTrayIconMenuClicked(QAction *action);

    //程序退出处理
    void SltQuitAPP();

    void on_btnWinClose_clicked();

    void on_btnWinMin_clicked();

    //好友点击
    void SltFriendsClicked(QQCell* );
    //群组点击
    void SltGroupsClicked(QQCell* );

    // 右键菜单
    void onAddFriendMenuDidSelected(QAction *action);
    //void onGroupPopMenuDidSelected(QAction *action);
    //void onChildPopMenuDidSelected(QAction *action);

    //QTcpSocket信号要关联的槽函数
    void SltTcpReply(const quint8 &type, const QJsonValue &dataVal);
    void SltTcpStatus(const quint8 &state);

    void SltFriendChatWindowClose();

private:
    Ui::MainWindow *ui;

    QButtonGroup *m_btnGroup;

    ClientSocket *m_tcpSocket;

    QSystemTrayIcon *systemTrayIcon;

    void InitSysTrayIcon();
    void InitQQListMenu();

    void PraseAddFriendReply(const QJsonValue dataVal);
    void PraseAddFriendRequistReply(const QJsonValue dataVal);
};
#endif // MAINWINDOW_H
