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
    setMouseTracking (true);
    setContentsMargins(0, 0, 0, 0);
    setWindowTitle ("终端数据防泄漏系统");
    setMinimumSize(mMinWidth, mMinHeight);
    setWindowFlags(Qt::FramelessWindowHint);

    mCurStatus = new QLabel;
    mCurStatus->setWordWrap(true);
    QString str = ScanStatusHelper::getStatusString();
    if (nullptr != str && !str.isNull() && !str.isEmpty() && "" != str) {
        mCurStatus->setText(str);
    } else {
        mCurStatus->setText("");
    }
    mMainLayout = new QVBoxLayout;
    QVBoxLayout* contentLayout = new QVBoxLayout;
    QHBoxLayout* btnLayout = new QHBoxLayout;

    MainHeader* header = new MainHeader(this);

    MainToolBar* toolbar = new MainToolBar(this);
    ToolbarAction* action1 = new ToolbarAction(this);

    mScannerTaskWidget = new ScannerTaskWidget(this);
    mScannerResultWidget = new ScannerResultWidget(this);

    mCurStatus->setContentsMargins(8, 0, 6, 0);

    mMainLayout->setSpacing(0);
    mMainLayout->setContentsMargins(0, 0, 0, 0);
    mMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(3, 0, 3, 6);
    contentLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

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
    contentLayout->addWidget(widget);

    // content View
    contentLayout->addWidget(mScannerTaskWidget);
    contentLayout->addWidget(mScannerResultWidget);
    onShowTaskWidget();

    // 当前状态
    contentLayout->addWidget(mCurStatus);

    connect(mStatusTimer, &QTimer::timeout, this, [=] () {
        QString str = ScanStatusHelper::getStatusString();
        if (nullptr != str && !str.isNull() && !str.isEmpty() && "" != str) {
            mCurStatus->setText(str);
        } else {
            mCurStatus->setText("");
        }
    });

    connect(mScannerTaskWidget, &ScannerTaskWidget::taskDetail, this, [=] (const ScannerTaskItem* const item) {
        // FIXME:// 此处可能会低概率崩溃，当进入扫描结果页的瞬间删除此条任务(任务删除在另一个线程里)
        auto* it = const_cast<ScannerTaskItem*> (item);

        if (!item || !it)      return;

        mCurStatus->hide();

        // 此处释放 model 内数据
        mScannerResultWidget->clearData();

        Q_EMIT mScannerResultWidget->statusString (QString("任务名称: %1, 未处理数 %2 条, 例外文件数 %3 条, 共 %4 条结果")
                                                       .arg (it->getName()).arg (0).arg (0).arg (0));

        Q_EMIT DBManager::instance ()->refreshScanResult (it->getName (), it->getFilterName (), it->getScanDir (), it->getFilterOutDir(), it->getScanFileTypeStr(), it->getScanFileOutTypeStr());

        mScanBtn->setText(QString("扫描结果"));

        mScannerResultWidget->setTaskName (it->getName ());

        onShowTaskResultWidget();
    });

    connect (mScannerResultWidget, &ScannerResultWidget::returnTaskList, this, [=] () {
        mScanBtn->setText(tr("扫描任务"));
        mCurStatus->show();
        onShowTaskWidget();
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

    mMainLayout->addItem(contentLayout);

    setLayout(mMainLayout);

    // cursor
    connect (DBManager::instance(), &DBManager::loadTaskStart,      this, [=] {setCursor (Qt::BusyCursor);});
    connect (DBManager::instance(), &DBManager::loadTaskResultStart,this, [=] {setCursor (Qt::BusyCursor);});
    connect (DBManager::instance(), &DBManager::loadTaskStop,       this, [=] {if (Qt::BusyCursor == cursor()) setCursor (Qt::ArrowCursor);});
    connect (DBManager::instance(), &DBManager::loadTaskResultStop, this, [=] {if (Qt::BusyCursor == cursor()) setCursor (Qt::ArrowCursor);});

    mStatusTimer->start();
}

void MainWindow::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent (e);

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

    // 此处释放 model 内数据
    mScannerResultWidget->clearData();

    Q_EMIT mScannerResultWidget->statusString (QString("任务名称: %1, 未处理数 %2 条, 例外文件数 %3 条, 共 %4 条结果")
                                                .arg (it->getName()).arg (0).arg (0).arg (0));

    Q_EMIT DBManager::instance ()->refreshScanResult (it->getName (), it->getFilterName (), it->getScanDir (), it->getFilterOutDir());

    mScanBtn->setText(QString("扫描结果"));

    mScannerResultWidget->setTaskName (it->getName ());

    onShowTaskResultWidget();
}

void MainWindow::mouseMoveEvent(QMouseEvent* e)
{
    QPoint globalPos = e->globalPos();
    QRect rect = this->rect();
    QPoint tl = mapToGlobal (rect.topLeft());
    QPoint rb = mapToGlobal (rect.bottomRight());

    qreal dpiRatio = qApp->devicePixelRatio ();
    qDebug() << "pressed: " << (mIsPress ? "true" : "false");
    if (!mIsPress) {
        region (globalPos);
    }
    else {
        if (NONE != mDirection) {
            QRect rMove (tl, rb);
            switch (mDirection) {
                case LEFT: {
                    if (rb.x () - globalPos.x () <= this->minimumWidth ()) {
                        rMove.setX (tl.x ());
                    }
                    else {
                        rMove.setX (globalPos.x ());
                    }
                    qDebug() << "pressed: " << (mIsPress ? "true" : "false") << " left";
                    break;
                }
                case RIGHT: {
                    rMove.setWidth (globalPos.x () - tl.x ());
//                    qDebug() << "pressed: " << (mIsPress ? "true" : "false") << " right";
                    break;
                }
                case UP: {
                    if (rb.y () - globalPos.y () <= this->minimumHeight ()) {
                        rMove.setY (tl.y ());
                    }
                    else {
                        rMove.setY (globalPos.y ());
                    }
//                    qDebug() << "pressed: " << (mIsPress ? "true" : "false") << " up";
                    break;
                }
                case DOWN: {
                    rMove.setHeight (globalPos.y () - tl.y ());
//                    qDebug() << "pressed: " << (mIsPress ? "true" : "false") << " down";
                    break;
                }
                case LEFT_TOP: {
                    if (rb.x () - globalPos.x () <= this->minimumWidth ()) {
                        rMove.setX (tl.x ());
                    }
                    else {
                        rMove.setX (globalPos.x ());
                    }

                    if (rb.y () - globalPos.y () <= this->minimumHeight ()) {
                        rMove.setY (tl.y ());
                    }
                    else {
                        rMove.setY (globalPos.y ());
                    }
//                    qDebug() << "pressed: " << (mIsPress ? "true" : "false") << " left_top";
                    break;
                }
                case RIGHT_TOP: {
                    rMove.setWidth (globalPos.x () - tl.x ());
                    rMove.setY (globalPos.y ());
//                    qDebug() << "pressed: " << (mIsPress ? "true" : "false") << " right_top";
                    break;
                }
                case LEFT_BOTTOM: {
                    rMove.setX (globalPos.x ());
                    rMove.setHeight (globalPos.y () - tl.y ());
//                    qDebug() << "pressed: " << (mIsPress ? "true" : "false") << " left_bottom";
                    break;
                }
                case RIGHT_BOTTOM: {
                    rMove.setWidth (globalPos.x () - tl.x ());
                    rMove.setHeight (globalPos.y () - tl.y ());
//                    qDebug() << "pressed: " << (mIsPress ? "true" : "false") << " right_bottom";
                    break;
                }
                default: {
                    break;
                }
            }
            this->setGeometry(rMove);
            return;
        }
    }

    if (mDrag) {
        move ((QCursor::pos () - mOffset) * dpiRatio);
    }
}

void MainWindow::mousePressEvent(QMouseEvent* e)
{
    QWidget::mousePressEvent (e);

    if (e->isAccepted()) {
        return;
    }

    switch (e->button()) {
        case Qt::LeftButton: {
            mDrag = true;
            mIsPress = true;
            mOffset = mapFromGlobal (QCursor::pos());
            if (NONE != mDirection) {
                mouseGrabber();
            }
            else {
                mDragPos = e->globalPos() - frameGeometry().topLeft();
            }
            break;
        }
        case Qt::RightButton:
        default:{
            QWidget::mousePressEvent (e);
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton || e->button() == Qt::RightButton) {
        mDrag = false;
        mIsPress = false;
        if (NONE != mDirection) {
            releaseMouse();
            setCursor (QCursor(Qt::ArrowCursor));
        }
    }
    else {
        QWidget::mouseReleaseEvent (e);
    }

}

void MainWindow::onShowTaskWidget()
{
    mCurStatus->hide();
    mScannerResultWidget->hide();

    DBManager::instance()->setCurPage(DBManager::CUR_STOP);
    mScannerTaskWidget->clearData();
    DBManager::instance()->setCurPage(DBManager::CUR_TASK);
    Q_EMIT DBManager::instance()->loadTaskStart();
    DBManager::instance()->refreshScanTask();

    mScannerTaskWidget->show();
    mCurStatus->show();
}

void MainWindow::onShowTaskResultWidget()
{
    mScannerTaskWidget->hide();

    DBManager::instance()->setCurPage(DBManager::CUR_STOP);
    mScannerResultWidget->clearData();
    DBManager::instance()->setCurPage(DBManager::CUR_RESULT);
    Q_EMIT DBManager::instance()->loadTaskResultStart();
    DBManager::instance()->refreshScanResult2();

    mScannerResultWidget->show();
}

void MainWindow::region(const QPoint &cursorGlobalPoint)
{
    QRect rect = this->rect();
    QPoint tl = mapToGlobal(rect.topLeft());
    QPoint rb = mapToGlobal(rect.bottomRight());
    int x = cursorGlobalPoint.x();
    int y = cursorGlobalPoint.y();

    if (tl.x() + 3 >= x && x >= tl.x() - 3 && tl.y() + 3 >= y && y >= tl.y() - 3) {
        mDirection = LEFT_TOP;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));  // 设置鼠标形状
    }
    else if (x >= rb.x() - 3 && x <= rb.x() + 3 && y >= rb.y() - 3 && y <= rb.y() + 3) {
        mDirection = RIGHT_BOTTOM;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));
    }
    else if (x <= tl.x() + 3 && x >= tl.x() - 3 && y >= rb.y() - 3 && y <= rb.y() + 3) {
        mDirection = LEFT_BOTTOM;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if (x <= rb.x() + 3 && x >= rb.x() - 3 && y >= tl.y() + 3 && y <= tl.y() - 3) {
        mDirection = RIGHT_TOP;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if (x <= tl.x() + 3 && x >= tl.x() - 3) {
        mDirection = LEFT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if (x <= rb.x() + 3 && x >= rb.x() - 3) {
        mDirection = RIGHT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if (y >= tl.y() - 3 && y <= tl.y() + 3) {
        mDirection = UP;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    }
    else if (y <= rb.y() + 3 && y >= rb.y() - 3) {
        mDirection = DOWN;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    }
    else {
        mDirection = NONE;
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
}


