#include "main-header.h"

#include "main-window.h"

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
    mMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    MainHeader* header = new MainHeader(this);

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

    // content View


    setLayout(mMainLayout);



}
