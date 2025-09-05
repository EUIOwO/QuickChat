#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chatstackedwidget.h"
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : CustomMoveWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);//去掉qt自带标题栏

    m_btnGroup = new QButtonGroup(this);
    m_btnGroup->addButton(ui->btnConversation, 0);
    m_btnGroup->addButton(ui->btnApply, 1);
    m_btnGroup->addButton(ui->btnFriend, 2);
    m_btnGroup->addButton(ui->btnGroup, 3);

    connect(m_btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(SltMainPageChanged(int)));

    InitSysTrayIcon();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetSocket(ClientSocket *tcpSocket, const QString &name)
{
    if(tcpSocket != NULL){
        m_tcpSocket = tcpSocket;

        connect(m_tcpSocket,&ClientSocket::signalMessage, this, &MainWindow::SltTcpReply);
        connect(m_tcpSocket,&ClientSocket::signalStatus, this, &MainWindow::SltTcpStatus);
    }
}

void MainWindow::SltMainPageChanged(int index){
    //切换面板
    static int s_nIndex = 0;//记录当前ui->GCStackedWidget选中的页的下标
    if(s_nIndex == index){
        return;
    }

    ui->GCStackedWidget->setLength(ui->GCStackedWidget->width(),
                                   index > s_nIndex ? ChatStackedWidget::LeftToRight : ChatStackedWidget::RightToLeft);
    ui->GCStackedWidget->start(index);
    s_nIndex = index;
}


void MainWindow::on_btnWinClose_clicked()
{
    this->hide();
}

void MainWindow::InitSysTrayIcon(){
    systemTrayIcon = new QSystemTrayIcon(this);
    systemTrayIcon->setIcon(QIcon(":/resource/background/app.png"));

    QMenu *m_trayMenu = new QMenu(this);
    m_trayMenu->addAction("我在线上");
    m_trayMenu->addAction("离线");
    m_trayMenu->addSeparator();
    m_trayMenu->addAction("显示主面板");
    m_trayMenu->addSeparator();
    m_trayMenu->addAction("退出");

    systemTrayIcon->setContextMenu(m_trayMenu);//设置右键菜单
    systemTrayIcon->show();//显示托盘

    connect(systemTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(SltTrayIconClicked(QSystemTrayIcon::ActivationReason reason)));
    connect(m_trayMenu, SIGNAL(triggered(QAction*)), this, SLOT(SltTrayIconMenuClicked(QAction*)));
}

//托盘菜单
void MainWindow::SltTrayIcoClicked(QSystemTrayIcon::ActivationReason reason){
    switch(reason){
        case QSystemTrayIcon::DoubleClick:{ //双击
            if(!this->isVisible()){ //当面板没有被显示，show被调用
                this->show();
            }
        }
        break;
    default:
        break;
    }
}

//托盘菜单
void MainWindow::SltTrayIconMenuClicked(QAction *action){
    if("退出" == action->text()){
        this->hide();
        QTimer::singleShot(500, this, SLOT(SltQuitAPP()));
    }else if("显示主面板" == action->text()){
        this->show();
    }else if(!QString::compare("我在线上", action->text())){
        m_tcpSocket->CheckConnected();

    }else if(!QString::compare("离线", action->text())){
        m_tcpSocket->ColseConnected();
    }
}

void MainWindow::SltQuitAPP()
{
    delete ui;
    qApp->quit();
}

void MainWindow::SltTcpReply(const quint8 &type, const QJsonValue &dataVal)
{
    connect(m_tcpSocket,&ClientSocket::signalMessage, this, &MainWindow::SltTcpReply);
    connect(m_tcpSocket,&ClientSocket::signalStatus, this, &MainWindow::SltTcpStatus);
}

void MainWindow::SltTcpStatus(const quint8 &state)
{

}
