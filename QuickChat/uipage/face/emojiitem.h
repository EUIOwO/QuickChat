#ifndef EMOJIITEM_H
#define EMOJIITEM_H

#include <QLabel>

class EmojiItem : public QLabel
{
    Q_OBJECT

public:
    explicit EmojiItem(const QString &fileName, QWidget *parent = 0);
    ~EmojiItem();

signals:
    void sigSelectEmoji(int index);

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *ev);

private:
    int m_emojiIndex;
};

#endif // EMOJIITEM_H
