#ifndef MAINHEADER_H
#define MAINHEADER_H

#include "header-button.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QWidget>

class MainHeader : public QWidget
{
    Q_OBJECT
public:
    explicit MainHeader(QWidget *parent = nullptr);

Q_SIGNALS:
    void windowMin();
    void windowMax();
    void windowClose();

protected:
    void mouseMoveEvent(QMouseEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    QLayout*                mLeftLayout;
    QLayout*                mRightLayout;
    QHBoxLayout*            mMainLayout;

    QLabel*                 mHeaderName;

    HeaderButton*           mMinBtn;
    HeaderButton*           mMaxBtn;
    HeaderButton*           mCloseBtn;

    // data
    bool                    mDrag;
    QPoint                  mMouseStartPoint;
    QPoint                  mWindowTopLeftPoint;
};

#endif // MAINHEADER_H
