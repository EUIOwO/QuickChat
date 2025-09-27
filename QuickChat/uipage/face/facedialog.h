#ifndef FACEDIALOG_H
#define FACEDIALOG_H

#include <QDialog>
#include <QList>
#include "emojiitem.h"

namespace Ui {
class FaceDialog;
}

class FaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FaceDialog(QWidget *parent = 0);
    ~FaceDialog();

    void setSelectFaceIndex(int index);
    int selectFaceIndex();
    void moveFaceLocation(QPoint p);

private slots:
    void on_btnClose_clicked();

private:
    int m_preRow;
    int m_preColumn;

    int m_selectFaceIndex;
    Ui::FaceDialog *ui;
    QList<EmojiItem*> m_emojiList;

    void addEmojiItem(QString fileName);
};

#endif // FACEDIALOG_H
