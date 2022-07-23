#include "main-header.h"

#include "main-toolbar.h"
#include "main-window.h"
#include "push-button.h"
#include "scanner-result-widget.h"
#include "scanner-task-widget.h"
#include "toolbar-action.h"

#include <QDebug>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QWidget{parent}
{
    setObjectName("main");
    setContentsMargins(0, 0, 0, 0);
    setMinimumSize(mMinWidth, mMinHeight);
//    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);


    mMainLayout = new QVBoxLayout;
    mMainLayout->setContentsMargins(0, 0, 0, 0);
    mMainLayout->setSpacing(0);
    mMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    MainHeader* header = new MainHeader(this);
    header->setAutoFillBackground(true);

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
    // FIXME:// 图标不能使用 jpg 的，要用 png 的
    MainToolBar* toolbar = new MainToolBar(this);
    ToolbarAction* action1 = new ToolbarAction;
    action1->setIcon("://data/scanner.png");
    action1->setText(tr("网络扫描管理"));
    toolbar->addWidget(action1);
    mMainLayout->addWidget(toolbar);

    // button
    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    PushButton* btn1 = new PushButton;
    btn1->setText(tr("扫描任务"));
    btnLayout->addWidget(btn1);
    mMainLayout->addItem(btnLayout);

    // content View
    mScannerTaskWidget = new ScannerTaskWidget;
    mScannerResultWidget = new ScannerResultWidget;
    mMainLayout->addWidget(mScannerTaskWidget);
    mMainLayout->addWidget(mScannerResultWidget);
    mScannerResultWidget->hide();

    // change content
    connect(mScannerTaskWidget, &ScannerTaskWidget::taskDetail, this, [=] (const ScannerTaskItem* const item) {
        if (!item)      return;

        btn1->setText(tr("扫描结果"));
        //
        ScannerTaskItem* it = const_cast<ScannerTaskItem*> (item);
        qDebug() << "===> task name: " << it->getName() << "set filter name: " << it->getFilterName();

        mScannerResultWidget->loadTaskResult(it->getName(), it->getFilterName());

        mScannerResultWidget->show();
        mScannerTaskWidget->hide();
    });

    connect (mScannerResultWidget, &ScannerResultWidget::returnTaskList, this, [=] () {
        btn1->setText(tr("扫描任务"));
        mScannerResultWidget->hide();
        mScannerTaskWidget->show();
        mScannerResultWidget->clearData();
    });

    setLayout(mMainLayout);
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    mScannerResultWidget->setFixedWidth(width());
    mScannerTaskWidget->setFixedWidth(width());

    qDebug() << "resize";
}
