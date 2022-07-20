#include "main-toolbar.h"

MainToolBar::MainToolBar(QWidget *parent)
    : QToolBar{parent}
{
    setObjectName("main-toolbar");

    setFixedHeight(mMaxHeight);
}
