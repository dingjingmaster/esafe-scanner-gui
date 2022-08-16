#include "db/db-manager.h"
#include "main-header.h"

#include "main-window.h"
#include "push-button.h"
#include "main-toolbar.h"
#include "toolbar-action.h"
#include "scanner-task-widget.h"
#include "scanner-result-widget.h"

#include "../widget/progress.h"

#include <QDebug>
#include <QTimer>
#include <QApplication>
#include <utils/scan-status-helper.h>


MainWindow::MainWindow(QWidget *parent)
    : QWidget{parent}
{
    setObjectName("main");
    setContentsMargins(0, 0, 0, 0);
    setMinimumSize(mMinWidth, mMinHeight);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    setWindowTitle ("扫描自查工具");

    //setGraphicsEffect(new QGraphicsBlurEffect);

    mCurStatus = new QLabel;
    mCurStatus->setWordWrap(true);
    QString str = ScanStatusHelper::getStatusString();
    if (nullptr != str && !str.isNull() && !str.isEmpty() && "" != str) {
        mCurStatus->setText("正在扫描：" + str);
    } else {
        mCurStatus->setText("正在扫描：无");
    }
    mMainLayout = new QVBoxLayout;
    QHBoxLayout* btnLayout = new QHBoxLayout;

    MainHeader* header = new MainHeader(this);

    MainToolBar* toolbar = new MainToolBar(this);
    ToolbarAction* action1 = new ToolbarAction(this);

    mScannerTaskWidget = new ScannerTaskWidget(this);
    mScannerResultWidget = new ScannerResultWidget(this);

    mCurStatus->setContentsMargins(8, 0, 6, 3);

    mMainLayout->setSpacing(0);
    mMainLayout->setContentsMargins(0, 0, 0, 0);
    mMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    header->setAutoFillBackground(true);
    header->setContentsMargins(0, 0, 0, 0);

    connect(header, &MainHeader::windowClose,   this, [&] () {
        // 保存数据
        if (mScannerResultWidget) {
            Q_EMIT mScannerResultWidget->applyData ();
        }
        qApp->quit();
    });
    connect(header, &MainHeader::windowMin,     this, [&] () {setWindowState(Qt::WindowMinimized);});
    connect(header, &MainHeader::windowMax,     this, [&] () {
        auto state = windowState();
        if (state & Qt::WindowMaximized) {
            setWindowState(windowState () & ~Qt::WindowMaximized);
            setWindowState(Qt::WindowNoState);
        } else {
            setWindowState(Qt::WindowMaximized);
        }
    });

    connect (mScannerResultWidget, &ScannerResultWidget::startApplyData, header, &MainHeader::onApplyData);
    connect (mScannerResultWidget, &ScannerResultWidget::stopApplyData, header, &MainHeader::onApplyDataFinished);

    mMainLayout->addWidget(header);

    // tool bar
    // FIXME:// 图标不能使用 jpg 的，要用 png 的
    toolbar->setContentsMargins(0, 0, 0, 0);
    toolbar->setAutoFillBackground(true);
    action1->setIcon("://data/scanner-new.png");
    //action1->setText(tr("网络扫描管理"));
    action1->setFocus();
    toolbar->addWidget(action1);
    mMainLayout->addWidget(toolbar);

    mStatusTimer = new QTimer;
    mStatusTimer->setInterval(2 * 1000);

    // button
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    PushButton* btn1 = new PushButton;
    btn1->setText(tr("扫描任务"));
    
    mStatusLabel = new QLabel;
    mStatusLabel->hide();

    btnLayout->addWidget(btn1);
    btnLayout->addStretch();
    btnLayout->addWidget(mStatusLabel);
    btnLayout->addSpacing(10);
    mMainLayout->addItem(btnLayout);

    // content View
    mMainLayout->addWidget(mScannerTaskWidget);
    mMainLayout->addWidget(mScannerResultWidget);
    mScannerResultWidget->hide();

    // 当前状态
    mMainLayout->addWidget(mCurStatus);

    connect(mStatusTimer, &QTimer::timeout, this, [=] () {
        QString str = ScanStatusHelper::getStatusString();
        if (nullptr != str && !str.isNull() && !str.isEmpty() && "" != str) {
            mCurStatus->setText("正在扫描：" + str);
        } else {
            mCurStatus->setText("正在扫描：无");
        }
    });

    connect (mScannerResultWidget, &ScannerResultWidget::statusString, this, [=] (QString status) {
        mStatusLabel->resize(QApplication::fontMetrics().size(Qt::TextSingleLine, status));
        mStatusLabel->setText(status);
    });

    // change content
    connect(mScannerTaskWidget, &ScannerTaskWidget::taskDetail, this, [=] (const ScannerTaskItem* const item) {
        if (!item)      return;

        //
        ScannerTaskItem* it = const_cast<ScannerTaskItem*> (item);
        //qDebug() << "===> task name: " << it->getName() << "set filter name: " << it->getFilterName();

        Q_EMIT mScannerResultWidget->statusString (QString("任务名称: (%1), 总条数: (%2), 未处理: (%3), 误报: (%4), 删除: (%5)")
                .arg(it->getName ()).arg(0).arg (0).arg (0).arg (0));

        Q_EMIT DBManager::instance ()->refreshScanResult (it->getName (), it->getFilterName (), it->getScanDir ());
        //mScannerResultWidget->loadTaskResult (it->getName (), it->getFilterName (), it->getScanDir ());

        btn1->setText(QString("扫描结果"));

        mScannerResultWidget->setTaskName (it->getName ());

        mStatusLabel->show();
        mScannerResultWidget->show();
        mScannerTaskWidget->hide();

        DBManager::instance()->setCurPage(DBManager::CUR_RESULT);
    });
    
    connect (mScannerResultWidget, &ScannerResultWidget::returnTaskList, this, [=] () {
        btn1->setText(tr("扫描任务"));
        mStatusLabel->hide();
        mScannerResultWidget->hide();
        mScannerTaskWidget->show();
        mScannerResultWidget->clearData();
    });

    setLayout(mMainLayout);

    mStatusTimer->start();
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    mScannerResultWidget->setFixedWidth(width());
    mScannerTaskWidget->setFixedWidth(width());

    qDebug() << "resize";
}
