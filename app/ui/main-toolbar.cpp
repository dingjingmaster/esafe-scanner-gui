#include "main-toolbar.h"

MainToolBar::MainToolBar(QWidget *parent)
    : QToolBar{parent}
{
    setFixedHeight(mMaxHeight);
}
