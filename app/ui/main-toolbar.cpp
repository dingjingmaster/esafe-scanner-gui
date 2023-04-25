#include "main-toolbar.h"

MainToolBar::MainToolBar(QWidget *parent)
    : QToolBar{parent}
{
    setMouseTracking (true);
    setAutoFillBackground(true);
    setObjectName("main-toolbar");
    setContentsMargins(0, 0, 0, 0);
    setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
    //setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    setFixedHeight(mMaxHeight);
}
