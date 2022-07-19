#ifndef TOOLBARACTION_H
#define TOOLBARACTION_H

#include <QWidget>

class QLabel;
class QVBoxLayout;

class ToolbarAction : public QWidget
{
public:
    explicit ToolbarAction(QWidget *parent = nullptr);

    void setIcon (QString pic);
    void setText (QString text);

private:
    const int           mMaxWidth = 100;
    const int           mMaxHeight = 110;
    const int           mIconWidth = 100;
    const int           mIconHeight = mIconWidth - 10;
    const int           mTextsize = 20;

    QVBoxLayout*        mMainLayout;

    QLabel*             mIcon;
    QLabel*             mText;
};

#endif // TOOLBARACTION_H
