#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chatstackedwidget.h"
#include "qqcell.h"
#include "databasemagr.h"
#include "myapp.h"
#include "clientsocket.h"
#include "chatwindow.h"
#include "unit.h"
#include <QMenu>
#include <QtCore>

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

   ui->GCStackedWidget->setCurrentIndex(2);

   InitQQListMenu();
   InitSysTrayIcon();


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

/**
* 添加好友列表菜单
*/
void MainWindow::InitQQListMenu()
{
   //设置菜单
   //设置组菜单
   QMenu *addFriend = new QMenu(this);
   addFriend->addAction(tr("添加好友"));
   addFriend->addAction(tr("刷新"));
   addFriend->addSeparator();
   addFriend->addAction(tr("删除该组"));
   connect(addFriend, SIGNAL(triggered(QAction*)), this, SLOT(onAddFriendMenuDidSelected(QAction*)));
   ui->friendListwidget->setGroupPopMenu(addFriend);

   // 设置子菜单
   QMenu *childMenu = new QMenu(this);
   childMenu->addAction(tr("发送即时消息"));
   childMenu->addSeparator();
   childMenu->addAction("移动至黑名单");
   childMenu->addAction("删除联系人");

   QMenu *groupListMenu = new QMenu(tr("移动联系人至"));
   childMenu->addMenu(groupListMenu);

   // childMenu->addAction(tr("创建讨论组"));
   connect(childMenu, SIGNAL(triggered(QAction*)), this, SLOT(onChildPopMenuDidSelected(QAction*)));
   ui->friendListwidget->setChildPopMenu(childMenu);

   //添加默认项
   //好友列表
   QQCell *myFriend = new QQCell;
   myFriend->groupName = QString(tr("我的好友"));
   myFriend->isOpen = false;
   myFriend->type = QQCellType_Group;
   myFriend->name = QString(tr("我的好友"));
   myFriend->subTitle = QString("(0/0)");
   ui->friendListwidget->insertQQCell(myFriend);

   QQCell *blacklist = new QQCell;
   blacklist->groupName = QString(tr("黑名单"));
   blacklist->isOpen = false;
   blacklist->type = QQCellType_Group;
   blacklist->name = QString(tr("黑名单"));
   blacklist->subTitle = QString("(0/0)");
   ui->friendListwidget->insertQQCell(blacklist);

   connect(ui->friendListwidget, SIGNAL(onChildDidDoubleClicked(QQCell*)), this, SLOT(SltFriendsClicked(QQCell*)));

   //组列表
   QMenu *myGroupMenu = new QMenu(this);
   myGroupMenu->addAction(tr("创建讨论组"));
   myGroupMenu->addAction(tr("加入讨论组"));
   myGroupMenu->addAction(tr("删除该组"));
   myGroupMenu->addSeparator();
   connect(myGroupMenu, SIGNAL(triggered(QAction*)), this, SLOT(onGroupPopMenuDidSelected(QAction*)));
   ui->groupListWidget->setGroupPopMenu(myGroupMenu);
   //设置子菜单
   QMenu *groupChildMen = new QMenu(this);
   groupChildMen->addAction(tr("发送即时消息"));
   ui->groupListWidget->setChildPopMenu(groupChildMen);

   //添加默认项
   QQCell *groupCell = new QQCell;
   groupCell->groupName = QString(tr("我的群组"));
   groupCell->isOpen = false;
   groupCell->type = QQCellType_GroupEx;
   groupCell->name = QString(tr("讨论组"));
   groupCell->subTitle = QString("(0/0)");
   ui->groupListWidget->insertQQCell(groupCell);

   connect(ui->groupListWidget, SIGNAL(onChildDidDoubleClicked(QQCell*)), this, SLOT(SltGroupsClicked(QQCell*)));
}

void MainWindow::SetSocket(ClientSocket *tcpSocket, const QString &name)
{
   if(tcpSocket != NULL){
       m_tcpSocket = tcpSocket;

       connect(m_tcpSocket,&ClientSocket::signalMessage, this, &MainWindow::SltTcpReply);
       connect(m_tcpSocket,&ClientSocket::signalStatus, this, &MainWindow::SltTcpStatus);

       ui->labelUserName->setText(name);

       //添加到我的群组
       AddMyGroups(DataBaseMagr::Instance()->GetMyGroup(MyApp::m_nId));
   }
}

/**
* @brief MainWindow::onAddFriendMenuDidSelected
* 添加好友
* @param action
*/
void MainWindow::onAddFriendMenuDidSelected(QAction *action)
{
   if (!action->text().compare(tr("添加好友")))
   {
       QString text = CInputDialog::GetInputText(this, "milo");

       if (!text.isEmpty()) {
            //首先判断该用户是否已经是我的好友了
           if (DataBaseMagr::Instance()->isMyFriend(MyApp::m_nId, text)) {
               CMessageBox::Infomation(this, "该用户已经是你的好友了！");
               return;
           }

           // 构建 Json 对象
           QJsonObject json;
           json.insert("id", m_tcpSocket->GetUserId());
           json.insert("name", text);

           m_tcpSocket->SltSendMessage(AddFriend, json);
       }
   }
   else if (!action->text().compare(tr("刷新")))
   {
       // 上线的时候获取当前好友的状态
       QJsonArray friendArr = DataBaseMagr::Instance()->GetMyFriend(MyApp::m_nId);

       // 组织Jsonarror
       m_tcpSocket->SltSendMessage(RefreshFriends, friendArr);
   }
   else if (!action->text().compare(tr("删除该组")))
   {
       qDebug() << "delete group";
   }
}

//创建群组
void MainWindow::onGroupPopMenuDidSelected(QAction *action)
{
   if(!action->text().compare(tr("创建讨论组"))){
       qDebug()<<"开始创建讨论组";
           QString text = CInputDialog::GetInputText(this, "我的朋友们");

           qDebug() << "用户输入的讨论组名称:" << text;

           if(!text.isEmpty()){
               //构建Json对象;
               QJsonObject json;
               json.insert("id", m_tcpSocket->GetUserId());
               json.insert("name", text);

               qDebug() << "发送创建讨论组请求，用户ID:" << m_tcpSocket->GetUserId()
                                   << "，讨论组名称:" << text;

               m_tcpSocket->SltSendMessage(CreateGroup,json);
           }
   }else if(!action->text().compare(tr("加入讨论组"))){
       QString text = CInputDialog::GetInputText(this, "我的朋友们");
       if(!text.isEmpty()){
           //首先判断是否已经加入过该群组
           if(DataBaseMagr::Instance()->isInGroup(text)){
               CMessageBox::Infomation(this,"你已经添加该群组了！");
               return;
           }
           //构建json对象
           QJsonObject json;
           json.insert("id", MyApp::m_nId);
           json.insert("name", text);

           m_tcpSocket->SltSendMessage(AddGroup, json);

           ui->groupListWidget->upload();
       }
   }else if(!action->text().compare(tr("删除该组"))){
       qDebug() << "delete group" << endl;
   }
}

void MainWindow::onChildPopMenuDidSelected(QAction *action)
{
   QQCell *cell = ui->friendListwidget->GetRightClickedCell();
   if(NULL == cell) return;

   if(!action->text().compare(tr("发送即时消息"))){
       qDebug() << "send message" << cell->name;
       //打开对话框
       SltFriendsClicked(cell);
   }else if(!action->text().compare(tr("移动至黑名单"))){
       qDebug() << "black friend list" << endl;
   }else if(!action->text().compare(tr("删除联系人"))){
       qDebug() << "delete my friend" << endl;
       ui->friendListwidget->removeQQCell(cell);
   }
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

void MainWindow::SltTcpStatus(const quint8 &state)
{

}

//关闭与好友聊天的窗口
void MainWindow::SltFriendChatWindowClose()
{

}

void MainWindow::on_btnWinMin_clicked()
{
   this->hide();
}

//好友点击
void MainWindow::SltFriendsClicked(QQCell* cell)
{
   qDebug() << "双击" << endl;
   ChatWindow *chatWindow = new ChatWindow();
   connect(chatWindow, &ChatWindow::signalSendMessage, m_tcpSocket, &ClientSocket::SltSendMessage);
   connect(chatWindow, &ChatWindow::signalClose, this, &MainWindow::SltFriendChatWindowClose);

   //设置窗口属性
   chatWindow->SetCell(cell);
   chatWindow->show();
}

//群组点击
void MainWindow::SltGroupsClicked(QQCell* cell)
{
   //构建json对象
   QJsonObject json;
   json.insert("id", cell->id);
   json.insert("name", cell->name);

   m_tcpSocket->SltSendMessage(GetMyGroups, json);

   //判断该用户是否有聊天窗口，如果有弹出窗口
   foreach(ChatWindow *window, m_chatGroupWindows){
       if(window->GetUserId() == cell->id){
           window->show();
           return;
       }
   }

   //没有检索到聊天窗口，直接弹出新窗口
   ChatWindow *chatWindow = new ChatWindow();
   connect(chatWindow, SIGNAL(signalSendMessage(quint8, QJsonValue)), m_tcpSocket, SLOT(SltSendMessage(quint8,QJsonValue)));
   connect(chatWindow, SIGNAL(signalClose()), this, SLOT(SltGroupChatWindowClose()));

   chatWindow->SetCell(cell, 1);
   chatWindow->show();

   //添加到当前聊天框
   m_chatGroupWindows.append(chatWindow);

}

void MainWindow::SltTcpReply(const quint8 &type, const QJsonValue &dataVal)
{

    qDebug()<<"=== 收到TCP响应 ===";
    qDebug()<<"消息类型:"<< type <<"CreateGroup枚举值:"<< CreateGroup;
    qDebug()<<"数据内容:"<< dataVal;

   switch (type) {
       case AddFriend:{ //服务器返回主动添加好友的用户消息
           PraseAddFriendReply(dataVal);
       }
       break;
   case AddFriendRequist:{//服务器返回主动添加好友的用户消息
           PraseAddFriendRequistReply(dataVal);
       }
       break;
       case AddGroup:{
           PraseAddGroupReply(dataVal);
       }
       break;
       case AddGroupRequist:{
           PraseAddGroupRequest(dataVal);
       }
       break;
       case CreateGroup:{
            qDebug()<<"开始处理创建群组相应";
           PraseCreatGroupReply(dataVal);
       }
       break;
       case GetMyGroups:{
           PraseGetGroupFriendsReply(dataVal);
       }
       break;
       case RefreshGroups:{
           PraseRefreshGroupFriendsReply(dataVal);
       }
           break;
       case SendGroupMsg:{
           PraseGroupMessageReply(dataVal);
       }
           break;
   }
}

//服务器返回 主动添加好友的用户 的消息
void MainWindow::PraseAddFriendReply(const QJsonValue dataVal)
{
   if(dataVal.isObject()){
       QJsonObject dataJson = dataVal.toObject();
       int id = dataJson.value("id").toInt();
       QString name = dataJson.value("name").toString();
       int status = dataJson.value("status").toInt();
       QString head = dataJson.value("head").toString();

       QQCell *cell = new QQCell;
       cell->groupName = QString("我的好友");
       cell->iconPath = QString(":/resource/head/%1").arg(head);
       cell->type = QQCellType_Child;
       cell->name = name;
       cell->subTitle = QString("当前用户状态: %1").arg(OnLine == status ? QString("在线") : QString("离线"));
       cell->id = id;
       cell->status = status;

       ui->friendListwidget->insertQQCell(cell);
   }
}

//服务器返回 通知被添加的好友 的消息
void MainWindow::PraseAddFriendRequistReply(const QJsonValue dataVal)
{
  if(dataVal.isObject()){
      QJsonObject dataJson = dataVal.toObject();
      int id = dataJson.value("id").toInt();
      QString name = dataJson.value("name").toString();
      QString msg = dataJson.value("msg").toString();
      QString head = dataJson.value("head").toString();

      if(!DataBaseMagr::Instance()->isMyFriend(MyApp::m_nId, name)){
          QQCell *cell = new QQCell;
          cell->groupName = QString("我的好友");
          cell->iconPath = QString(":/resource/head/%1").arg(head);
          cell->type = QQCellType_Child;
          cell->name = name;
          cell->subTitle = QString("当前用户状态: 在线");
          cell->id = id;
          cell->status = OnLine;

          ui->friendListwidget->insertQQCell(cell);

          qDebug() << "MyApp::m_nId" << MyApp::m_nId << "添加好友，好友id:" << id << endl;

          //更新数据库
          DataBaseMagr::Instance()->AddFriend(cell->id, MyApp::m_nId, name);
      }
  }
}

void MainWindow::PraseAddGroupReply(const QJsonValue &dataVal)
{
   if(dataVal.isObject()){
       QJsonObject jsonObj = dataVal.toObject();
       int nId = jsonObj.value("id").toInt();
       if(-1 == nId){
           CMessageBox::Infomation(this, "未找到该群组");
           return;
       }
       QQCell *cell = new QQCell;
       cell->groupName = QString(tr("我的群组"));
       cell->iconPath  = GetHeadPixmap(jsonObj.value("head").toString());
       cell->type      = QQCellType_Child;
       cell->name      = jsonObj.value("name").toString();
       cell->subTitle  = QString("我的群，我做主...");
       cell->id        = nId;
       cell->status    = OnLine;

       ui->groupListWidget->insertQQCell(cell);

       //更新至数据库
       DataBaseMagr::Instance()->AddGroup(nId, MyApp::m_nId, cell->name);

   }
}

//有人加入了你的群组
void MainWindow::PraseAddGroupRequest(const QJsonValue &dataVal)
{
   if(!dataVal.isObject()) return;
}

void MainWindow::PraseCreatGroupReply(const QJsonValue &dataVal)
{
    qDebug() << "解析创建群组响应";
   if(dataVal.isObject()){
       QJsonObject dataObj = dataVal.toObject();
       int nId = dataObj.value("id").toInt();

       QString name = dataObj.value("name").toString();
       qDebug()<<"群组id" << nId << "名称" << name;

       //为查询到该用户
       if(-1 == nId){
           CMessageBox::Infomation(this, "该群组已添加");
           return;
       }
       QQCell *cell = new QQCell;
       cell->groupName = QString(tr("我的群组"));
       cell->iconPath  = GetHeadPixmap(dataObj.value("head").toString());
       cell->type      = QQCellType_Child;
       cell->name      = dataObj.value("name").toString();
       cell->subTitle  = QString("我的群，我做主...");
       cell->id        = nId;
       cell->status    = OnLine;

       ui->groupListWidget->insertQQCell(cell);
       qDebug() << "群组已添加到页面";

       //更新至数据库
       DataBaseMagr::Instance()->AddGroup(cell->id, MyApp::m_nId, cell->name);

   }
}

void MainWindow:: PraseGetGroupFriendsReply(const QJsonValue &dataVal)
{
   //data的value时数组
   if(dataVal.isArray()){
       QJsonArray array = dataVal.toArray();
       int nGroupId = array.at(0).toInt();

       //将数据更新至界面
       //判断与该用户是否有聊天窗口，如果有弹出窗口
       foreach(ChatWindow *window, m_chatGroupWindows){
           if(window->GetUserId() == nGroupId){
               window->UpdateUserStatus(dataVal);
               window->show();
               return;
           }
       }
   }
}

void MainWindow::PraseRefreshGroupFriendsReply(const QJsonValue &dataVal)
{
   //data的value是数组
   if(dataVal.isArray()){
       QJsonArray array = dataVal.toArray();
       int nSize = array.size();
       for(int i = 0; i < nSize; i++){
//            QJsonObject jsonObj = array.at(i).toObject();
//            int nId = jsonObj.value("status").toInt();
//            QList<QQCell *> friends = ui->groupListWidget->getCells();
//            foreach(QQCell *cell, friends.at(0)->childs){
//                if(cell->id == nId){
//                    cell->SetSubtitle(QString("当前用户状态:%1").arg(OnLine == nStatus ? tr("在线"):tr("离线")));
//                }
//            }
//            ui->groupListWidget->upload();
       }
   }
}

void MainWindow::PraseGroupMessageReply(const QJsonValue &dataVal)
{
   //消息格式为object对象
   if(dataVal.isObject()){
       QJsonObject dataObj = dataVal.toObject();
       int nId = dataObj.value("group").toInt();

       //如果收到消息时有聊天窗口存在，直接添加到聊天记录，并弹出窗口
       foreach(ChatWindow *window, m_chatGroupWindows){
           if(window->GetUserId() == nId){
               window->AddMessage(dataVal);
               window->show();
               return;
           }
       }

       //没有检索到聊天窗口，直接弹出新窗口
       QList<QQCell *> groups = ui->groupListWidget->getCells();
       foreach(QQCell *cell, groups.at(0)->childs){
           //有列表的才创建
           if(cell->id == nId){
               ChatWindow *chatWindow = new ChatWindow();
               connect(chatWindow, SIGNAL(signalSendMessage(quint8, QJsonValue)), m_tcpSocket, SLOT(SltMessage(quint8, QJsonValue)));
               connect(chatWindow, SIGNAL(signalClose()), this, SLOT(SltGroupChatWindowClose()));

               //构建Json对象
               QJsonObject json;
               json.insert("id", cell->id);
               json.insert("name", cell->name);

              //获取最新用户状态
               m_tcpSocket->SltSendMessage(GetMyGroups, json);

               chatWindow->SetCell(cell, 1);
               chatWindow->AddMessage(dataVal);
               chatWindow->show();
               //添加到当前聊天框
               m_chatGroupWindows.append(chatWindow);
               return;
           }
       }
   }
}

void MainWindow::AddMyGroups(const QJsonValue &dataVal)
{
   // data 的 value 是数组
   if (dataVal.isArray()) {
       QJsonArray array = dataVal.toArray();
       int nSize = array.size();
       for (int i = 0; i < nSize; ++i) {
           QJsonObject jsonObj = array.at(i).toObject();

           QQCell *cell = new QQCell;
           cell->groupName = QString(tr("我的群组"));
           cell->iconPath  = ":/resource/head/1.bmp";
           cell->type      = QQCellType_Child;
           cell->name      = jsonObj.value("name").toString();
           cell->subTitle  = QString("我的群，我做主...");
           cell->id        = jsonObj.value("id").toInt();;
           cell->SetStatus(OnLine);

           ui->groupListWidget->insertQQCell(cell);
           ui->groupListWidget->update();
       }
   }
}

QString MainWindow::GetHeadPixmap(const QString &name)
{
   if (QFile::exists(MyApp::m_strHeadPath + name)) {
       return MyApp::m_strHeadPath + name;
   }
   return ":/resource/head/1.bmp";
}



