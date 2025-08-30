#include "animationstackedwidget.h"
#include <QPropertyAnimation>
#include <QPainter>
#include <QTransform>

AnimationStackedWidget::AnimationStackedWidget(QWidget *parent):QStackedWidget(parent)
{
    m_nextPageIndex = 0;
    m_isAnimation = false;
}

AnimationStackedWidget::~AnimationStackedWidget(){

}

void AnimationStackedWidget::paintEvent(QPaintEvent *event)
{
    if(m_isAnimation){
        QPainter p(this);

        //旋转角度为0-90
        if(iRotateVal > 0 && iRotateVal <= 90){

            QPixmap pix(currentWidget()->size());
            currentWidget()->render(&pix);
            //旋转
            QTransform transform;
            transform.translate(width()/2, 0);
            transform.rotate(iRotateVal, Qt::YAxis);//设置沿y轴旋转180°
            p.setTransform(transform);
            p.drawPixmap(-1*width()/2, 0, pix);

        //旋转角度为90-180
        }else if(iRotateVal > 90 && iRotateVal <= 180){

            QPixmap pix(widget(m_nextPageIndex)->size());
            widget(m_nextPageIndex)->render(&pix);
            QTransform transform;
            transform.translate(width()/2, 0);
            transform.rotate(iRotateVal+180, Qt::YAxis);//设置沿y轴旋转180°
            p.setTransform(transform);
            p.drawPixmap(-1*width()/2, 0, pix);

        //旋转角度为-180--90
        }else if(iRotateVal > -180 && iRotateVal <= -90){

            QPixmap pix(widget(m_nextPageIndex)->size());
            widget(m_nextPageIndex)->render(&pix);
            //旋转
            QTransform transform;
            transform.translate(width()/2, 0);
            transform.rotate(iRotateVal+180, Qt::YAxis);//设置沿y轴旋转180°
            p.setTransform(transform);
            p.drawPixmap(-1*width()/2, 0, pix);

        //旋转角度为-90-0
        }else if(iRotateVal > -90 && iRotateVal <= 0){

            QPixmap pix(currentWidget()->size());
            currentWidget()->render(&pix);
            //旋转
            QTransform transform;
            transform.translate(width()/2, 0);
            transform.rotate(iRotateVal, Qt::YAxis);//设置沿y轴旋转180°
            p.setTransform(transform);
            p.drawPixmap(-1*width()/2, 0, pix);
        }
    }else{
        QWidget::paintEvent(event);
    }
}

void AnimationStackedWidget::animation(int pageIndex)
{

    if(m_isAnimation){
        return;
    }

    m_nextPageIndex = pageIndex;

    int offsetX = frameRect().width();
    int offsetY = frameRect().height();
    widget(pageIndex)->setGeometry(0,0,offsetX,offsetY);

    //PropertyName
    QPropertyAnimation *animation = new QPropertyAnimation(this,"rotateVal");

    animation->setDuration(700);//设置动画持续事件
    animation->setEasingCurve(QEasingCurve::Linear);//设置动画曲线
    animation->setStartValue(m_startVal);//设置动画起始值
    animation->setEndValue(m_endVal);//设置动画结束值

    QObject::connect(animation,&QPropertyAnimation::valueChanged, this, &AnimationStackedWidget::onValueChanged);
    QObject::connect(animation,&QPropertyAnimation::finished, this, &AnimationStackedWidget::onFinished);

    currentWidget()->hide();

    m_isAnimation = true;

    //动画开始
    animation->start();

}

void AnimationStackedWidget::onValueChanged(const QVariant &value)
{
    //值改变了以后
    repaint();//更新画面
}

void AnimationStackedWidget::onFinished()
{
    m_isAnimation = false;
    //动画结束
    widget(m_nextPageIndex)->show();
    widget(m_nextPageIndex)->raise();

    setCurrentWidget(widget(m_nextPageIndex));


    repaint();
}

