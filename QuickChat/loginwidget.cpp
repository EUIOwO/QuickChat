#include "loginwidget.h"
#include "ui_loginwidget.h"
#include <QFile>
LoginWidget::LoginWidget(QWidget *parent) :
    QWidget(parent),
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
