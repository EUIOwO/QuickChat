#include "facedialog.h"
#include "ui_facedialog.h"
#include <QDebug>

FaceDialog::FaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FaceDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint| Qt::WindowStaysOnTopHint);


    QString fileName;
    //加载表情图片
    for (int i=0;i<132;i++) {
         fileName = QString(":/resource/faces/%1.gif").arg(i+1);
        //添加表情
        addEmojiItem(fileName);
    }

    m_selectFaceIndex = 0;
}

FaceDialog::~FaceDialog()
{
    delete ui;
}


void FaceDialog::addEmojiItem(QString fileName)
{
    //需要计算这个表情添加到哪个位置
    int row = m_emojiList.size()/ui->tableWidget->columnCount();
    int column = m_emojiList.size()%ui->tableWidget->columnCount();

    QTableWidgetItem *twItem = new QTableWidgetItem;
    ui->tableWidget->setItem(row, column, twItem);

    EmojiItem *emojiItem = new EmojiItem(fileName);
    connect(emojiItem, &EmojiItem::sigSelectEmoji, this, [=](int faceIndex){
        m_selectFaceIndex = faceIndex;
        this->hide();
    });


    ui->tableWidget->setCellWidget(row, column, emojiItem);

    m_emojiList.append(emojiItem);
}

void FaceDialog::on_btnClose_clicked()
{
    this->hide();
}

void FaceDialog::setSelectFaceIndex(int index)
{
    m_selectFaceIndex = index;
}

int FaceDialog::selectFaceIndex()
{
    return m_selectFaceIndex;
}

void FaceDialog::moveFaceLocation(QPoint p)
{
    this->move(p);
}
