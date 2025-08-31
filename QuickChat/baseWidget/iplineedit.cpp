#include "iplineedit.h"
#include <QSizePolicy>
#include <QHBoxLayout>
#include <QLabel>
#include <QRegExp>
#include <QRegExpValidator>
#include <QEvent>
#include <QKeyEvent>
#include <QDebug>

IPLineEdit::IPLineEdit(QWidget *parent)
    :QLineEdit(parent)
{
    QHBoxLayout *hboxLayout = new QHBoxLayout(this);
    hboxLayout->setContentsMargins(1,1,1,1);

    QRegExp regExp("^(25[0-5]|2[0-4][0-9]|[01]?[0-9]{1,2})$");

    QLabel *labelDot[3];

    for(int i = 0; i < IP_INPUT_SIZE; i++){
        m_lineEdit[i] = new QLineEdit(this);

        //保存线条编辑是否使用边框绘制本身
        m_lineEdit[i]->setFrame(false);

        //设置输入框输入的最大位数
        m_lineEdit[i]->setMaxLength(3);

        //消除布局中的控件并居中
        m_lineEdit[i]->setAlignment(Qt::AlignCenter);

        //保存小部件的默认布局
        m_lineEdit[i]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

        //添加验证规则
        m_lineEdit[i]->setValidator(new QRegExpValidator(regExp, this));

        //安装事件过滤器，过滤子控件事件，截获控件按键和鼠标事件
        m_lineEdit[i]->installEventFilter(this);

        hboxLayout->addWidget(m_lineEdit[i]);

        if(i < 3){
            labelDot[i] = new QLabel(this);
            labelDot[i]->setText(".");
            labelDot[i]->setFixedWidth(2);
            hboxLayout->addWidget(labelDot[i]);
        }

        this->setReadOnly(true);

        m_lineEdit[0]->setFocus();
        m_lineEdit[0]->selectAll();
    }
}

IPLineEdit::~IPLineEdit()
{

}

QString IPLineEdit::text()
{
    //获取ip地址
    QString ip;
    for(int i = 0; i < IP_INPUT_SIZE; i++){
        ip.append(m_lineEdit[i]->text());
        if(3 != i){
            ip.append(".");
        }
    }
    return ip;
}

bool IPLineEdit::eventFilter(QObject *obj, QEvent *event)
{
    if(children().contains(obj) && QEvent::KeyPress == event->type()){
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
        QLineEdit *pCurrentEdit = qobject_cast<QLineEdit*>(obj);

        switch(keyEvent->key()){
            case Qt::Key_0:
            case Qt::Key_1:
            case Qt::Key_2:
            case Qt::Key_3:
            case Qt::Key_4:
            case Qt::Key_5:
            case Qt::Key_6:
            case Qt::Key_7:
            case Qt::Key_8:
            case Qt::Key_9:
            {
                QString strText = pCurrentEdit->text();
                if(strText.length() <= 3 && strText.toInt()*10 > 255){
                    int index = getIndex(pCurrentEdit);
                    if(index != -1 && index != 3){
                       m_lineEdit[index+1]->setFocus();
                       m_lineEdit[index+1]->selectAll();
                    }
                }
                return QLineEdit::eventFilter(obj,event);
            }
            break;
            case Qt::Key_Left:
            {
                if(!pCurrentEdit->cursorPosition()){
                    int index = getIndex(pCurrentEdit);
                    if(index != -1 && index != 0){
                        m_lineEdit[index -1]->setFocus();
                        int length = m_lineEdit[index -1]->text().length();
                        m_lineEdit[index - 1]->setCursorPosition(length?length:0);
                    }
                }
                return QLineEdit::eventFilter(obj,event);
            }
            break;
            case Qt::Key_Right:
            {
                if(!pCurrentEdit->cursorPosition()){
                    int index = getIndex(pCurrentEdit);
                    if(index != -1 && index != 3){
                        m_lineEdit[index + 1]->setFocus();
                        int length = m_lineEdit[index + 1]->text().length();
                        m_lineEdit[index + 1]->setCursorPosition(length?length:0);
                    }
                }
                return QLineEdit::eventFilter(obj,event);
            }
            break;
            case Qt::Key_Backspace://后退键
            {
                QString strText = pCurrentEdit->text();
                if(strText.isEmpty()){
                    int index = getIndex(pCurrentEdit);
                    if(index != -1 && index != 0){
                        m_lineEdit[index - 1]->setFocus();
                        int length = m_lineEdit[index - 1]->text().length();
                        m_lineEdit[index - 1]->setCursorPosition(length?length:0);
                    }
                }
                return QLineEdit::eventFilter(obj,event);
            }
            break;
            case Qt::Key_Period://点号.
            {
                int index = getIndex(pCurrentEdit);
                if(index != -1 && index != 3){
                    m_lineEdit[index + 1]->setFocus();
                    m_lineEdit[index + 1]->selectAll();
                }
                return QLineEdit::eventFilter(obj,event);
            }
            break;
            case Qt::Key_Enter:
            {
                QString strText = pCurrentEdit->text();
                    if(!strText.isEmpty()){
                        int index = getIndex(pCurrentEdit);
                        if(index != 3){
                            m_lineEdit[index + 1]->setFocus();
                            int length = m_lineEdit[index + 1]->text().length();
                            m_lineEdit[index + 1]->setCursorPosition(length?length:0);
                        }
                    }
                return QLineEdit::eventFilter(obj,event);
            }
            break;
        }
    }
}

int IPLineEdit::getIndex(QLineEdit *pLineEdit)
{
    int index = -1;
    for(int i = 0; i < IP_INPUT_SIZE;i++){
        if(pLineEdit == m_lineEdit[i]){
            index = i;
        }
    }
    return index;
}
