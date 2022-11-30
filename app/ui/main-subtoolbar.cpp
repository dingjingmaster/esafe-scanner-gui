#include "main-subtoolbar.h"
#include "push-button.h"

#include <QSpacerItem>
#include <QHBoxLayout>

MainSubToolBar::MainSubToolBar(QWidget *parent)
    : QWidget{parent}
{
    setObjectName("main-subtoolbar");
    //setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    mMainLayout = new QHBoxLayout;
    mMainLayout->setContentsMargins(0, 0, 0, 0);
    mMainLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

void MainSubToolBar::addButton(QString name)
{
    PushButton* btn1 = new PushButton;
    btn1->setText(name);
    mMainLayout->addWidget(btn1);
}
