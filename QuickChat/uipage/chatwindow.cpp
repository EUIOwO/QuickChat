#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "iteminfo.h"

#include "global.h"
#include "unit.h"
#include "myapp.h"
#include "databasemagr.h"

#include <QDateTime>
#include <QTimer>
#include <QTime>

#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QKeyEvent>
#include <QToolTip>

#include <QHostAddress>

#include <QFileDialog>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#define DATE_TIME       QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss")

ChatWindow::ChatWindow(QWidget *parent) :
    CustomMoveWidget(parent),
    ui(new Ui::ChatWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    ui->widgetFileBoard->setVisible(false);

    m_nFileType = 0;

    m_model = new QStandardItemModel(this);
    m_model->setRowCount(3);
    ui->tableViewGroups->setModel(m_model);

    QMenu *sendMenu = new QMenu(this);
    QAction *actionEnter     = sendMenu->addAction(QIcon(""), tr("按Enter键发送消息"));
    QAction *actionCtrlEnter = sendMenu->addAction(QIcon(""), tr("按Ctrl+Enter键发送消息"));

    // 设置互斥
    QActionGroup *actionGroup = new QActionGroup(this);
    actionGroup->addAction(actionEnter);
    actionGroup->addAction(actionCtrlEnter);

    // 设置可选
    actionEnter->setCheckable(true);
    actionCtrlEnter->setCheckable(true);

    // 默认配置
    actionCtrlEnter->setChecked(true);

    // 设置菜单
    ui->btnAction->setMenu(sendMenu);

    // 设置快捷键
    ui->btnSendMsg->setShortcut(QKeySequence("alt+s"));
    ui->btnClose->setShortcut(QKeySequence("alt+c"));

    // 关联信号槽
    connect(ui->btnWinClose, SIGNAL(clicked(bool)), this, SLOT(SltCloseWindow()));
    connect(ui->btnClose, SIGNAL(clicked(bool)), this, SLOT(SltCloseWindow()));
    connect(ui->widgetBubble, SIGNAL(signalDownloadFile(QString)), this, SLOT(SltDownloadFiles(QString)));

    ui->textEditMsg->setFocus();

    m_faceDialog = new FaceDialog(this);
    m_faceDialog->setModal(true);
    m_faceDialog->hide();
}

ChatWindow::~ChatWindow()
{
    delete ui;
}

/**
 * @brief ChatWindow::SetCell
 * 设置聊天属性
 * @param cell
 * @param type
 */
void ChatWindow::SetCell(QQCell *cell, const quint8 &type)
{
    m_cell = cell;
    ui->labelWinTitle->setText(QString("与 %1 聊天中").arg(cell->name));
    this->setWindowTitle(QString("与 %1 聊天中").arg(cell->name));

    m_nChatType = type;

    if (0 == type) {
        // 加载历史
    }
    else {
        ui->tableViewGroups->setVisible(true);
        ui->widgetFileInfo->setVisible(false);
        ui->widgetFileBoard->setVisible(true);
        // 添加群组人员
        m_model->setColumnCount(2);
        m_model->setRowCount(3);
        m_model->setHorizontalHeaderLabels(QStringList() << "好友" << "状态");
    }
}

QString ChatWindow::GetIpAddr() const
{
    return m_cell->ipaddr;
}

int ChatWindow::GetUserId() const
{
    return m_cell->id;
}

/**
 * @brief ChatWindow::AddMessage
 * 接受服务器转发过来的消息
 * @param json
 */
void ChatWindow::AddMessage(const QJsonValue &json)
{
    if (json.isObject()) {
        QJsonObject dataObj = json.toObject();
        int type = dataObj.value("type").toInt();

        QString strHead = dataObj.value("head").toString();
        // 如果有头像，则用自己的头像(群组消息的时候会附带头像图片)
        strHead = GetHeadPixmap(strHead);

        ItemInfo *itemInfo = new ItemInfo();
        itemInfo->SetName(0 == m_nChatType ? m_cell->name : dataObj.value("name").toString());
        itemInfo->SetDatetime(DATE_TIME);
        itemInfo->SetHeadPixmap(strHead.isEmpty() ? m_cell->iconPath : strHead);
        itemInfo->SetMsgType(type);

        if(type == Text){
            QString strText = dataObj.value("msg").toString();
            itemInfo->SetText(strText);
        } else if(type == Face){
            int faceIndex = dataObj.value("face").toInt();
            itemInfo->SetFace(faceIndex);
        }


        // 加入聊天窗口
        ui->widgetBubble->addItem(itemInfo);
        // 群组的聊天消息不保存
        if (0 != m_nChatType) return;


    }
}

/**
 * @brief ChatWindow::UpdateUserStatus
 * 更新列表状态
 * @param json
 */
void ChatWindow::UpdateUserStatus(const QJsonValue &dataVal)
{
    if (ui->tableViewGroups->isVisible()) {
        // data 的 value 是数组
        if (dataVal.isArray()) {
            QJsonArray array = dataVal.toArray();
            int nSize = array.size();
            m_model->clear();
            m_model->setColumnCount(2);
            m_model->setRowCount(nSize - 1);
            m_model->setHorizontalHeaderLabels(QStringList() << "好友" << "状态");

            for (int i = 1; i < nSize; ++i) {
                QJsonObject jsonObj = array.at(i).toObject();

                int nStatus = jsonObj.value("status").toInt();

                m_model->setData(m_model->index(i - 1, 0), jsonObj.value("name").toString());
                m_model->setData(m_model->index(i - 1, 1), nStatus ==  OnLine ? QString("在线") : QString("离线"));
            }

            ui->tableViewGroups->setColumnWidth(0, 90);
            ui->tableViewGroups->setColumnWidth(1, 50);
        }

    }
}

/**
 * @brief ChatWindow::changeEvent
 * 翻译切换
 * @param e
 */
void ChatWindow::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ChatWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Return:
    {
        if (Qt::ControlModifier == event->modifiers()) {
            on_btnSendMsg_clicked();
        }
    }
        break;
    default:
        break;
    }

    QWidget::keyPressEvent(event);
}

/**
 * @brief ChatWindow::SltChatMessage
 * @param name
 * @param text
 */
void ChatWindow::SltChatMessage(const QString &text)
{
    if (text.isEmpty()) return;

    // 组织消息
    ItemInfo *itemInfo = new ItemInfo();
    itemInfo->SetName(m_cell->name);
    itemInfo->SetDatetime(DATE_TIME);
    itemInfo->SetHeadPixmap(m_cell->iconPath);
    itemInfo->SetText(text);

    // 加入聊天界面
    ui->widgetBubble->addItem(itemInfo);
}

/**
 * @brief ChatWindow::on_btnSendMsg_clicked
 */
void ChatWindow::on_btnSendMsg_clicked()
{
    QString text = ui->textEditMsg->toPlainText();
    // 把最后一个回车换行符删掉
    while (text.endsWith("\n")) {
        text.remove(text.length() - 2, 2);
    }

    // 判断消息是否为空
    if (text.isEmpty()) {
        QPoint point = ui->widgetMsg->mapToGlobal(ui->btnSendMsg->pos());

        QToolTip::showText(point, tr("发送消息内容不能为空，请重新输入！"));
        return;
    }

    // 构建json数据
    QJsonObject json;
    json.insert("id", MyApp::m_nId);
    json.insert("to", m_cell->id);
    json.insert("msg", text);
    json.insert("type", Text);

    // 发送消息
    Q_EMIT signalSendMessage(0 == m_nChatType ? SendMsg : SendGroupMsg, json);

    // 构建气泡消息
    ItemInfo *itemInfo = new ItemInfo();
    itemInfo->SetName(MyApp::m_strUserName);
    itemInfo->SetDatetime(DATE_TIME);
    itemInfo->SetHeadPixmap(MyApp::m_strHeadFile);
    itemInfo->SetText(text);
    itemInfo->SetOrientation(Right);

    // 加入聊天界面
    ui->widgetBubble->addItem(itemInfo);
    // 清除输入框
    ui->textEditMsg->clear();

    // 群组消息不记录
    if (0 != m_nChatType) return;
}

// 延迟关闭
void ChatWindow::SltCloseWindow()
{
    Q_EMIT signalClose();
    QTimer::singleShot(100, this, SLOT(close()));
}

/**
 * @brief ChatWindow::on_toolButton_7_clicked
 * 添加图片
 */
void ChatWindow::on_toolButton_7_clicked()
{
    // 群组消息不记录
    if (0 != m_nChatType) {
        QMessageBox::information(this, tr("提示"), tr("不支持群组文件传输"));
        return;
    }

    static QString s_strPath = MyApp::m_strAppPath;
    QString strFileName = QFileDialog::getOpenFileName(this,
                                                       tr("选择图片"),
                                                       s_strPath,
                                                       tr("图片(*.jpg;*.png;*.bmp)"));
    // 文件选择检测
    if (strFileName.isEmpty()) return;
    s_strPath = strFileName;

    // 构建json数据
    QJsonObject json;
    json.insert("id", MyApp::m_nId);
    json.insert("to", m_cell->id);
    json.insert("msg", strFileName);
    json.insert("type", Picture);

    m_nFileType = SendPicture;

    // 发送消息
    Q_EMIT signalSendMessage(SendPicture, json);

    // 构建气泡消息
    ItemInfo *itemInfo = new ItemInfo();
    itemInfo->SetName(MyApp::m_strUserName);
    itemInfo->SetDatetime(DATE_TIME);
    itemInfo->SetHeadPixmap(MyApp::m_strHeadFile);
    itemInfo->SetText(strFileName);
    itemInfo->SetOrientation(Right);
    itemInfo->SetMsgType(Picture);

    // 加入聊天界面
    ui->widgetBubble->addItem(itemInfo);
    // 清除聊天框
    ui->textEditMsg->clear();

    // 群组消息不记录
    if (0 != m_nChatType) return;

    //保存发送的消息
   DataBaseMagr::Instance()->AddHistoryMsg(MyApp::m_nId, itemInfo);
}

/**
 * @brief ChatWindow::SltUpdateProgress
 * @param currSize
 * @param total
 */
void ChatWindow::SltUpdateProgress(quint64 bytes, quint64 total)
{

}

/**
 * @brief ChatWindow::SltFileRecvFinished
 * @param type
 * @param filePath
 */
void ChatWindow::SltFileRecvFinished(const quint8 &type, const QString &filePath)
{

}

/**
 * @brief ChatWindow::on_toolButton_6_clicked
 */
void ChatWindow::on_toolButton_6_clicked()
{
}

// 查看历史记录
void ChatWindow::on_toolButton_clicked()
{

}

// 发送文件
void ChatWindow::on_btnSendFile_clicked()
{

}

// 服务器下载文件
void ChatWindow::SltDownloadFiles(const QString &fileName)
{

}

void ChatWindow::on_toolButton_4_clicked()
{

}

/**
 * @brief ChatWindow::GetHeadPixmap
 * 获取用户头像
 * @param name
 * @return
 */
QString ChatWindow::GetHeadPixmap(const QString &name) const
{
    if (name.isEmpty()) return name;

    if (QFile::exists(MyApp::m_strHeadPath + name)) {
        return MyApp::m_strHeadPath + name;
    }

    return ":/resource/head/1.bmp";
}

void ChatWindow::sendFaceMsg(int faceIndex)
{
    // 构建json数据
    QJsonObject json;
    json.insert("id", MyApp::m_nId);
    json.insert("to", m_cell->id);
    json.insert("face", faceIndex);
    json.insert("type", Face);//表情消息

    // 发送消息
    Q_EMIT signalSendMessage(0 == m_nChatType ? SendMsg : SendGroupMsg, json);

    // 构建气泡消息
    ItemInfo *itemInfo = new ItemInfo();
    itemInfo->SetName(MyApp::m_strUserName);
    itemInfo->SetDatetime(DATE_TIME);
    itemInfo->SetHeadPixmap(MyApp::m_strHeadFile);
    itemInfo->SetFace(faceIndex);
    itemInfo->SetOrientation(Right);
    itemInfo->SetMsgType(Face); //设置表情消息

    // 加入聊天界面
    ui->widgetBubble->addItem(itemInfo);

    //保存发送的消息
    DataBaseMagr::Instance()->AddHistoryMsg(MyApp::m_nId, itemInfo);
}

// 插入表情
void ChatWindow::on_toolButton_3_clicked()
{
    QPoint p = ui->toolButton_3->pos();
    m_faceDialog->moveFaceLocation(p);
    m_faceDialog->setSelectFaceIndex(0);
    m_faceDialog->exec();

    int selectFaceIndex = m_faceDialog->selectFaceIndex();
    if(selectFaceIndex == 0){
        //点击了关闭按钮
        qDebug() << "点击了关闭按钮" << endl;

    }else{
        //点击了表情
        qDebug() << "selectFaceIndex" << selectFaceIndex;
        sendFaceMsg(selectFaceIndex);
    }
}
