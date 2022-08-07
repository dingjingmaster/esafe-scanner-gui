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

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    const int           mFixSize = 90;
    const int           mIconSize = 90;
    const int           mLabelWidth = 90;
    const int           mLabelHeight = mFixSize - mIconSize;

    QVBoxLayout*        mMainLayout;

    QLabel*             mIcon;
    QLabel*             mText;
};

#endif // TOOLBARACTION_H
