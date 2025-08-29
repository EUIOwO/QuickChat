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
}

LoginWidget::~LoginWidget()
{
    delete ui;
}
