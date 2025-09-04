#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "customwidget.h"
#include <QMainWindow>
#include <QButtonGroup>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public CustomMoveWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void SltMainPageChanged(int index);

private:
    Ui::MainWindow *ui;

    QButtonGroup *m_btnGroup;
};
#endif // MAINWINDOW_H
