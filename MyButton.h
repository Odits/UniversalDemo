#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QPushButton>
#include <QMouseEvent>

QT_BEGIN_NAMESPACE
class MyButton;
QT_END_NAMESPACE

class MyButton : public QPushButton
{
    Q_OBJECT
public:
    MyButton(QWidget *parent = nullptr) : QPushButton(parent) {}
    MyButton(const QString &text, QWidget *parent = nullptr) : QPushButton(text, parent) {}

    int getTextWidth()
    {
        // 使用 QFontMetrics 计算文本的宽度
        QFontMetrics fontMetrics(this->font());
        return fontMetrics.width(this->text());
    }

    void setMaximum_Width_Height(int width, int height)
    {
        this->setMaximumWidth(width);
        this->setMaximumHeight(height);
    }


signals:
    void leftClicked(QString);
    void rightClicked(QString);

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if(event->button() == Qt::RightButton)
        {
            emit rightClicked(this->text());
        }
        if(event->button() == Qt::LeftButton)
        {
            emit leftClicked(this->text());
        }
        else
        {
            QPushButton::mousePressEvent(event);
        }
    }
};





#endif // MYBUTTON_H
