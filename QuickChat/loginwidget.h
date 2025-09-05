#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H
#include "customwidget.h"
#include "clientsocket.h"

#include <QWidget>

namespace Ui {
class LoginWidget;
}

class LoginWidget : public CustomMoveWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

private slots:
    void on_btnWinMenu_clicked();

    void on_btnCancel_clicked();

    void on_btnLogin_clicked();

    void onSignalMessage(const quint8 &type, const QJsonValue &dataVal);

    void onSignalStatus(const quint8 &state);

    void on_btnWinClose_clicked();

    void on_btnWinMin_clicked();

private:
    Ui::LoginWidget *ui;

    ClientSocket *m_tcpSocket;
};

#endif // LOGINWIDGET_H
