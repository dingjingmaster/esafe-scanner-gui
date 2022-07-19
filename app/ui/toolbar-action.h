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
    const int           mMaxHeight = 80;
    const int           mIconLabelSize = 80;
    const int           mIconWidth = 60;
    const int           mIconHeight = mIconWidth;
    const int           mTextsize = 20;

    QVBoxLayout*        mMainLayout;

    QLabel*             mIcon;
    QLabel*             mText;
};

#endif // TOOLBARACTION_H
