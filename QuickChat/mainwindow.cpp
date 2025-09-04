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

}

MainWindow::~MainWindow()
{
    delete ui;
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
