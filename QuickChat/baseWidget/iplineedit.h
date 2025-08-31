#ifndef IPLINEEDIT_H
#define IPLINEEDIT_H
#include <QLineEdit>
#include <QWidget>

#define IP_INPUT_SIZE   4

class IPLineEdit : public QLineEdit
{
public:
    IPLineEdit(QWidget *parent = 0);

    ~IPLineEdit();

    QString text();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QLineEdit *m_lineEdit[IP_INPUT_SIZE];
    int getIndex(QLineEdit *pLineEdit);
};

#endif // IPLINEEDIT_H
