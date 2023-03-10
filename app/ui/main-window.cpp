#include "db/db-manager.h"
#include "main-header.h"

#include "main-window.h"
#include "push-button.h"
#include "main-toolbar.h"
#include "xatom-helper.h"
#include "toolbar-action.h"
#include "scanner-task-widget.h"
#include "scanner-result-widget.h"

#include "../widget/progress.h"

#include <QDebug>
#include <QTimer>
#include <QScreen>
#include <QX11Info>
#include <X11/Xlib.h>
#include <QApplication>
#include <utils/scan-status-helper.h>


MainWindow::MainWindow(QWidget *parent)
    : QWidget{parent}
{
    setObjectName("main");
    setContentsMargins(0, 0, 0, 0);
    setMinimumSize(mMinWidth, mMinHeight);

    setWindowTitle ("终端数据防泄漏系统");

    //installEventFilter(this);
    setWindowFlags(Qt::FramelessWindowHint);
    if (QX11Info::isPlatformX11()) {
        XatomHelper::getInstance()->setUKUIDecoraiontHint(this->winId(), false);
        MotifWmHints hints;
        hints.flags = MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
        hints.functions = MWM_FUNC_ALL;
        hints.decorations = MWM_DECOR_BORDER;
        XatomHelper::getInstance()->setWindowMotifHint(this->winId(), hints);
    }

    mCurStatus = new QLabel;
    mCurStatus->setWordWrap(true);
    QString str = ScanStatusHelper::getStatusString();
    if (nullptr != str && !str.isNull() && !str.isEmpty() && "" != str) {
        mCurStatus->setText("正在扫描：" + str);
    } else {
        mCurStatus->setText("");
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

    static bool maxWin = false;
    connect(header, &MainHeader::windowMin,     this, [&] () {
        maxWin = false;
        setWindowState(Qt::WindowMinimized);
    });
    connect(header, &MainHeader::windowMax,     this, [&] () {
        //auto state = windowState();
        if (maxWin) {
            maxWin = false;
            setWindowState(windowState () & ~Qt::WindowMaximized);
            setWindowState(Qt::WindowNoState);
            //setFixedSize(mMinWidth, mMinHeight);
        } else {
            maxWin = true;
            setWindowState(Qt::WindowMaximized);
            //setFixedSize(qApp->screenAt (this->pos())->size());
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
    action1->setText(tr("敏感数据处理"));
    action1->setFocus();
    toolbar->addWidget(action1);
    mMainLayout->addWidget(toolbar);

    mStatusTimer = new QTimer;
    mStatusTimer->setInterval(2 * 1000);

    // button
    auto widget = new QWidget;
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mScanBtn = new PushButton;
    mScanBtn->setText(tr("扫描任务"));
    btnLayout->addWidget(mScanBtn);
    btnLayout->addStretch();
    widget->setLayout (btnLayout);
    widget->setStyleSheet ("border-style: none none solid none; border-width:1px; border-color:#E3E3E3;");
    mMainLayout->addWidget(widget);

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
            mCurStatus->setText("");
        }
    });

//    connect (mScannerResultWidget, &ScannerResultWidget::statusString, this, &MainWindow::onShowStatusString);

    // change content
//    connect(mScannerTaskWidget, &ScannerTaskWidget::taskDetail, this, &MainWindow::onLoadTaskResult);

    connect(mScannerTaskWidget, &ScannerTaskWidget::taskDetail, this, [=] (const ScannerTaskItem* const item) {
        // FIXME:// 此处可能会低概率崩溃，当进入扫描结果页的瞬间删除此条任务(任务删除在另一个线程里)
        ScannerTaskItem* it = const_cast<ScannerTaskItem*> (item);

        if (!item || !it)      return;

        mCurStatus->hide();

        // 此处释放 model 内数据
        mScannerResultWidget->clearData();

        Q_EMIT mScannerResultWidget->statusString (QString("任务名称: %1, 未处理数 %2 条, 例外文件数 %3 条, 共 %4 条结果")
                                                       .arg (it->getName()).arg (0).arg (0).arg (0));

        Q_EMIT DBManager::instance ()->refreshScanResult (it->getName (), it->getFilterName (), it->getScanDir (), it->getFilterOutDir());

        mScanBtn->setText(QString("扫描结果"));

        mScannerResultWidget->setTaskName (it->getName ());

//    mStatusLabel->show();
        mScannerTaskWidget->hide();
        mScannerResultWidget->show();

        DBManager::instance()->setCurPage(DBManager::CUR_RESULT);

    });

    connect (mScannerResultWidget, &ScannerResultWidget::returnTaskList, this, [=] () {
        mScanBtn->setText(tr("扫描任务"));
//        mStatusLabel->hide();
        mCurStatus->show();
        mScannerResultWidget->hide();
        mScannerTaskWidget->show();
        mScannerResultWidget->clearData();
    });

    connect (this, &MainWindow::activePrimaryWindow, this, [=] () {
        show();
        if (windowFlags() & Qt::WindowMinimized) {
            setWindowState (windowState() | Qt::WindowNoState);
        }
        setWindowState (windowState() | Qt::WindowActive);
        activateWindow();
        raise();
        setFocus(Qt::NoFocusReason);
        qInfo() << "active window";
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

void MainWindow::onShowStatusString(const QString& status)
{
//    mStatusLabel->resize(QApplication::fontMetrics().size(Qt::TextSingleLine, status));
//    mStatusLabel->setText(status);
}

void MainWindow::onLoadTaskResult(const ScannerTaskItem * const item)
{
    // FIXME:// 此处可能会低概率崩溃，当进入扫描结果页的瞬间删除此条任务(任务删除在另一个线程里)
    ScannerTaskItem* it = const_cast<ScannerTaskItem*> (item);

    if (!item || !it)      return;

//    mScannerResultWidget->onLoadResultStart();
    //
    //qDebug() << "===> task name: " << it->getName() << "set filter name: " << it->getFilterName();

    // 此处释放 model 内数据
    mScannerResultWidget->clearData();

    Q_EMIT mScannerResultWidget->statusString (QString("任务名称: %1, 未处理数 %2 条, 例外文件数 %3 条, 共 %4 条结果")
                                                .arg (it->getName()).arg (0).arg (0).arg (0));

    Q_EMIT DBManager::instance ()->refreshScanResult (it->getName (), it->getFilterName (), it->getScanDir (), it->getFilterOutDir());
    //mScannerResultWidget->loadTaskResult (it->getName (), it->getFilterName (), it->getScanDir ());

    mScanBtn->setText(QString("扫描结果"));

    mScannerResultWidget->setTaskName (it->getName ());

//    mStatusLabel->show();
    mScannerResultWidget->show();
    mScannerTaskWidget->hide();

    DBManager::instance()->setCurPage(DBManager::CUR_RESULT);
//    mScannerResultWidget->onLoadResultEnd();
}

void MainWindow::mouseMoveEvent(QMouseEvent* e)
{
    QWidget::mouseMoveEvent(e);
    //QMainWindow::mouseMoveEvent(e);
    if (!mDrag)
        return;

    qreal  dpiRatio = qApp->devicePixelRatio();
    if (QX11Info::isPlatformX11()) {
        Display *display = QX11Info::display();
        Atom netMoveResize = XInternAtom(display, "_NET_WM_MOVERESIZE", False);
        XEvent xEvent;
        const auto pos = QCursor::pos();

        memset(&xEvent, 0, sizeof(XEvent));
        xEvent.xclient.type = ClientMessage;
        xEvent.xclient.message_type = netMoveResize;
        xEvent.xclient.display = display;
        xEvent.xclient.window = this->winId();
        xEvent.xclient.format = 32;
        xEvent.xclient.data.l[0] = pos.x() * dpiRatio;
        xEvent.xclient.data.l[1] = pos.y() * dpiRatio;
        xEvent.xclient.data.l[2] = 8;
        xEvent.xclient.data.l[3] = Button1;
        xEvent.xclient.data.l[4] = 0;

        XUngrabPointer(display, CurrentTime);
        XSendEvent(display, QX11Info::appRootWindow(QX11Info::appScreen()),
                   False, SubstructureNotifyMask | SubstructureRedirectMask,
                   &xEvent);
        //XFlush(display);

        XEvent xevent;
        memset(&xevent, 0, sizeof(XEvent));

        xevent.type = ButtonRelease;
        xevent.xbutton.button = Button1;
        xevent.xbutton.window = this->winId();
        xevent.xbutton.x = e->pos().x() * dpiRatio;
        xevent.xbutton.y = e->pos().y() * dpiRatio;
        xevent.xbutton.x_root = pos.x() * dpiRatio;
        xevent.xbutton.y_root = pos.y() * dpiRatio;
        xevent.xbutton.display = display;

        XSendEvent(display, this->effectiveWinId(), False, ButtonReleaseMask, &xevent);
        XFlush(display);

        if (e->source() == Qt::MouseEventSynthesizedByQt) {
            if (!this->mouseGrabber()) {
                this->grabMouse();
                this->releaseMouse();
            }
        }

        mDrag = false;
    } else {
        this->move((QCursor::pos() - mOffset) * dpiRatio);
    }
}

void MainWindow::mousePressEvent(QMouseEvent* e)
{
    QWidget::mousePressEvent(e);
    //QMainWindow::mousePressEvent(e);
    if (e->button() == Qt::LeftButton && !e->isAccepted()) {
        mDrag = true;
        mOffset = mapFromGlobal(QCursor::pos());
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* e)
{
    QWidget::mouseReleaseEvent(e);
    //QMainWindow::mouseReleaseEvent(e);
    mDrag = false;
}

