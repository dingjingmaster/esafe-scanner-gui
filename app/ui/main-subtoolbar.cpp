#include "main-subtoolbar.h"
#include "push-button.h"

#include <QHBoxLayout>

MainSubToolBar::MainSubToolBar(QWidget *parent)
    : QWidget{parent}
{
    setObjectName("main-subtoolbar");
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    mMainLayout = new QHBoxLayout;
    mMainLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

void MainSubToolBar::addButton(QString name)
{
    PushButton* btn1 = new PushButton;
    btn1->setText(name);
    mMainLayout->addWidget(btn1);
}
