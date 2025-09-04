#include "loginwidget.h"
#include "ui_loginwidget.h"
#include "mainwindow.h"
#include "clientsocket.h"
#include <QFile>
#include <QtCore>
#include <QDebug>
#include "unit.h"

LoginWidget::LoginWidget(QWidget *parent) :
    CustomMoveWidget(parent),
    ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint); //去掉qt自带的标题栏
    this->setAttribute(Qt::WA_TranslucentBackground); //使透明生效

    QFile file(":/qss/resource/qss/default.css");
    file.open(QIODevice::ReadOnly);


    //设置样式表
    qApp->setStyleSheet(file.readAll());
    file.close();

    ui->stackedWidget->setCurrentIndex(0);


    //给lineEditUser添加图片
    ui->lineEditUser->SetIcon(QPixmap(":/resource/common/ic_user.png"));
    ui->lineEditPasswd->SetIcon(QPixmap(":/resource/common/ic_lock.png"));

    ui->lineEditUser->setPlaceholderText("用户名");
    ui->lineEditPasswd->setPlaceholderText("密码");
    ui->lineEditPasswd->setEchoMode(QLineEdit::Password);

    m_tcpSocket = new ClientSocket;
    connect(m_tcpSocket,&ClientSocket::signalMessage, this, &LoginWidget::onSignalMessage);
    connect(m_tcpSocket,&ClientSocket::signalStatus, this, &LoginWidget::onSignalStatus);

    //检查是否连接到服务器
    m_tcpSocket->CheckConnected();

}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::on_btnWinMenu_clicked()
{
    ui->stackedWidget->setStartVal(0);//反转的起始值
    ui->stackedWidget->setEndVal(180);//反转的结束值
    //m_asw实现动画效果
    ui->stackedWidget->animation(1);
}

void LoginWidget::on_btnCancel_clicked()
{
    ui->stackedWidget->setStartVal(0);//反转的起始值
    ui->stackedWidget->setEndVal(-180);//反转的结束值
    //m_asw实现动画效果
    ui->stackedWidget->animation(0);
}

void LoginWidget::on_btnLogin_clicked()
{
    //检查是否连接到服务器
    m_tcpSocket->CheckConnected();

    //获取输入框的数据
    QString username = ui->lineEditUser->text();
    QString passwd = ui->lineEditPasswd->text();

    // 构建 Json 对象
    QJsonObject json;
    json.insert("name", username);
    json.insert("passwd", passwd);

    m_tcpSocket->SltSendMessage(0x11, json);
}

void LoginWidget::onSignalMessage(const quint8 &type, const QJsonValue &dataVal)
{

}

void LoginWidget::onSignalStatus(const quint8 &state)
{
    switch(state){
        case ConnectedHost://已连接服务器
            ui->labelWinTitle->setText("已连接服务器");
        break;
        case LoginSuccess://用户登录成功
        {
            qDebug() << "用户登录成功" << endl;

            MainWindow *mainWindow = new MainWindow;
            mainWindow->show();

            this->hide();
        }
        break;
        case LoginPasswdError://用户未注册
            qDebug() << "用户未注册" << endl;
        break;
        case LoginRepeat://用户已在线
            qDebug() << "用户已在线" << endl;
        break;
    }
}
