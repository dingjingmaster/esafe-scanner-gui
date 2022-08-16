#ifndef MAINHEADER_H
#define MAINHEADER_H

#include "header-button.h"

#include <QLabel>
#include <QWidget>
#include <QDateTime>
#include <QMouseEvent>
#include <QHBoxLayout>

class MainHeader : public QWidget
{
    Q_OBJECT
public:
    explicit MainHeader(QWidget *parent = nullptr);

Q_SIGNALS:
    void windowMin();
    void windowMax();
    void windowClose();

public Q_SLOTS:
    void onApplyData ();
    void onApplyDataFinished ();

protected:
    void mouseMoveEvent(QMouseEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseDoubleClickEvent(QMouseEvent*) override;

private:
    const int               mMaxHeight = 140;

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
