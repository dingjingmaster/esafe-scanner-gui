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
    const int           mFixSize = 90;
    const int           mIconSize = 60;
    const int           mLabelWidth = 90;
    const int           mLabelHeight = mFixSize - mIconSize;

    QVBoxLayout*        mMainLayout;

    QLabel*             mIcon;
    QLabel*             mText;
};

#endif // TOOLBARACTION_H
