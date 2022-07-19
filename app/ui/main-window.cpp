#include "main-header.h"

#include "main-toolbar.h"
#include "main-window.h"
#include "push-button.h"
#include "toolbar-action.h"

#include <QDebug>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QWidget{parent}
{
    setMinimumSize(800, 600);
    setContentsMargins(0, 0, 0, 0);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);


    mMainLayout = new QVBoxLayout;
    mMainLayout->setContentsMargins(0, 0, 0, 0);
    mMainLayout->setSpacing(0);
    mMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    MainHeader* header = new MainHeader(this);
    header->setAutoFillBackground(true);
    header->setStyleSheet("background-color:red;");

    connect(header, &MainHeader::windowClose,   this, [&] () {qApp->quit();});
    connect(header, &MainHeader::windowMin,     this, [&] () {setWindowState(Qt::WindowMinimized);});
    connect(header, &MainHeader::windowMax,     this, [&] () {
        auto state = windowState();
        if (state & Qt::WindowMaximized) {
            setWindowState(Qt::WindowNoState);
        } else {
            setWindowState(Qt::WindowMaximized);
        }
    });


    mMainLayout->addWidget(header);

    // tool bar
    MainToolBar* toolbar = new MainToolBar(this);
    toolbar->setFixedHeight(80);
    toolbar->setStyleSheet("background-color:red;");
    ToolbarAction* action1 = new ToolbarAction;
    toolbar->addWidget(action1);
    mMainLayout->addWidget(toolbar);

    // button
    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    PushButton* btn1 = new PushButton;
//    btn1->set
    btnLayout->addWidget(btn1);
    mMainLayout->addItem(btnLayout);

    // content View


    setLayout(mMainLayout);
}
