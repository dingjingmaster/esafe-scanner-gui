#include "push-button.h"
#include "threads/scanner-result-save-thread.h"
#include "view/header-view.h"
#include "scanner-result-widget.h"
//#include "utils/notify-to-filter.h"
#include "scanner-result-delegate.h"
#include "utils/export-scan-result.h"
//#include "utils/message-with-fp.pb.h"
#include "model/scanner-result-item.h"
#include "model/scanner-result-model.h"

#include <QDebug>
#include <QToolTip>
#include <QDateTime>
#include <QScrollBar>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QResizeEvent>
#include <QDBusMessage>
#include <QFontMetrics>
#include <QStandardPaths>
#include <QDBusConnection>
#include <QDBusPendingCall>

#include <QEventLoop>
#include <QApplication>

#include "../db/db-manager.h"
#include "../widget/progress.h"
#include "../threads/event-loop.h"

#define FREEDESKTOP_FM_DBUS             "org.freedesktop.FileManager1"
#define FREEDESKTOP_FM_DBUS_PATH        "/org/freedesktop/FileManager1"


ScannerResultWidget::ScannerResultWidget(QWidget *parent)
    : QWidget{parent}
{
    //GOOGLE_PROTOBUF_VERIFY_VERSION;

    mMainLayout = new QVBoxLayout;

    mBtnLayout = new QHBoxLayout;
    mLeftLayout = new QHBoxLayout;
    PushButton* retBtn = new PushButton(this, PushButton::Type2);
    mRightLayout = new QHBoxLayout;
    
    mDelBtn = new PushButton(this, PushButton::Type2);
    mMisBtn = new PushButton(this, PushButton::Type2);
    mExpBtn = new PushButton(this, PushButton::Type2);

    mModel = new ScannerResultModel;
    mView = new ScannerView;

    mView->setItemDelegate(new ScannerResultDelegate(this));
    mHeaderView = new HeaderView(Qt::Horizontal, mView);

    mProgress = new Progress(mView);
    mProgress->hide();

    connect(mModel, &ScannerResultModel::progress, mProgress, &Progress::updateProcess);

    retBtn->setText(tr("返回"));
    Q_EMIT retBtn->enable (true);
    retBtn->setStyleSheet("background-color:red;");
    mLeftLayout->addWidget(retBtn);

    mRightLayout->setSpacing(6);

    mDelBtn->setStyleSheet("background-color:red;");
    mDelBtn->setText(tr("删除"));
    mRightLayout->addWidget(mDelBtn);

    mMisBtn->setStyleSheet("background-color:red;");
    mMisBtn->setText(tr("误报"));
    mRightLayout->addWidget(mMisBtn);

    mExpBtn->setStyleSheet("background-color:red;");
    mExpBtn->setText(tr("导出"));
    mRightLayout->addWidget(mExpBtn);

    mBtnLayout->addItem(mLeftLayout);
    mBtnLayout->addStretch();
    mBtnLayout->addItem(mRightLayout);
    mMainLayout->addItem(mBtnLayout);

    // tabview
    mView->setHorizontalHeader(mHeaderView);
    mView->setModel(mModel);
    mMainLayout->addWidget(mView);


    connect (this, &ScannerResultWidget::startApplyData, this, [=] () {
        retBtn->enable(false);
        mExpBtn->enable(false);
        mMisBtn->enable(false);
        mDelBtn->enable(false);
    });

    connect (this, &ScannerResultWidget::stopApplyData, this, [=] () {
        retBtn->enable(true);
        mMisBtn->enable(true);
    });

    // save data
    //mThreadSaveData = new QThread;
    //ScannerResultSaveThread* srThread = new ScannerResultSaveThread(mModel, DBManager::instance()->getResultHelper(), this);
    //connect (this, &ScannerResultWidget::startApplyData, srThread, &ScannerResultSaveThread::onSaveScanResultData, Qt::QueuedConnection);
    //connect (srThread, &ScannerResultSaveThread::saveScanResultDataFinished, this, [=] () {
    //    Q_EMIT stopApplyData();
    //    Q_EMIT retBtn->enable(true);
    //    qInfo() << "DJ- finished";
    //});
    //srThread->moveToThread(mThreadSaveData);
    //mThreadSaveData->start();

    mView->horizontalHeader()->setMinimumSectionSize(10);
    mView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    mView->verticalHeader()->setDefaultSectionSize (60);

    mView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    mView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
    mView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Interactive);
    mView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Interactive);

    connect (this, &ScannerResultWidget::applyData, this, [=] () {
        QMessageBox* box = new QMessageBox(this);
        box->setText ("确认提交您对数据的更改吗？");
        box->setWindowTitle("提示");

        QPushButton* apply = new QPushButton(box);
        QPushButton* cancel = new QPushButton(box);

        apply->setText ("提交更改");
        cancel->setText ("取消");

        box->addButton (apply, QMessageBox::AcceptRole);
        box->addButton (cancel, QMessageBox::RejectRole);

        box->connect (apply, &QPushButton::clicked, this, [=] () {
#if 0
            auto ls = mModel->getChangedItem ();

            for (auto l : ls) {
                com::esafenet::scanner::client::ScannerClientMessage msg;
                ScannerResultItem* item = const_cast<ScannerResultItem*>(l);
                QString fileName = item->getFileName ();

                msg.Clear();
                msg.set_filename(fileName.toStdString());

                int op = -1;
                int status = item->getStatus2 ();
                if (ScannerResultItem::MisReport == status) {
                    op = OP_MISINFO;
                } else if (ScannerResultItem::Deleted == status) {
                    op = OP_DELETE;
                } else {
                    qDebug() << "not apply: " << fileName;
                    continue;
                }

                msg.set_id(item->getID ());
                msg.set_operation(op);

                qDebug() << "apply " << item->getStatus () << " data: " << msg.DebugString().c_str();
                NotifyToFilter::getInstance()->sendData(msg.SerializeAsString());
            }
#else

            // 单线程版本
            Q_EMIT startApplyData();
            mProgress->show();
            mModel->applyData();
            mProgress->hide();
            Q_EMIT stopApplyData();

            // 此处使用多线程
            //EventLoop loop;
            // saveScanResultDataFinished
            //box->connect (srThread, &ScannerResultSaveThread::saveScanResultDataFinished, this, [&] () {
            //    box->deleteLater();
            //    loop.exit(0);
            //});

            //Q_EMIT startApplyData();
            //loop.exec();
#endif
        });
        if (mModel->hasChanged ()) {
            box->exec ();
        } else {
            box->deleteLater ();
        }
    });


    connect (retBtn, &PushButton::clicked, this, &ScannerResultWidget::onBackToTaskView, Qt::UniqueConnection);

    mDelBtn->connect(mDelBtn, &PushButton::clicked, this, [=] () {
        QList<ScannerResultItem*> ls = mModel->getSelectedItem();
        if (ls.count() <= 0) {
            QMessageBox::warning(this, "警告", "请选中需要删除的数据后，再执行删除操作！", QMessageBox::Ok);
            return;
        }
        
        for (auto l : ls) {
#if 0
            com::esafenet::scanner::client::ScannerClientMessage msg;
            msg.Clear();
            msg.set_filename(const_cast<ScannerResultItem*>(l)->getFileName().toStdString());
            // FIXME://
            msg.set_id(0);
            msg.set_operation(OP_DELETE);
            qDebug() << "delete: " << msg.DebugString().c_str();
            NotifyToFilter::getInstance()->sendData(msg.SerializeAsString());
#endif
            //mModel->setData (mModel->getIndexByItem (l, 2), "删除");

            mModel->setData (*l, "删除");
        }

        Q_EMIT mModel->lazyUpdateView();
        mModel->setSelectedItemStatus(ScannerResultItem::Deleted);
        updateStatus();

    });

    mMisBtn->connect(mMisBtn, &PushButton::clicked, this, [=] () {
        QList<ScannerResultItem*> ls = mModel->getSelectedItem();
        if (ls.count() <= 0) {
            QMessageBox::warning(this, "警告", "请选中误报的数据后，再执行操作！", QMessageBox::Ok);
            return;
        }
        
        for (auto l : ls) {
#if 0
            com::esafenet::scanner::client::ScannerClientMessage msg;
            msg.Clear();
            msg.set_filename(const_cast<ScannerResultItem*>(l)->getFileName().toStdString());

            // FIXME://
            msg.set_id(0);
            msg.set_operation(OP_MISINFO);
            qDebug() << "misinformation: " << msg.DebugString().c_str();
            NotifyToFilter::getInstance()->sendData(msg.SerializeAsString());
#endif
            //mModel->setData (mModel->getIndexByItem (l, 2), "误报");
            mModel->setData (*l, "误报");
        }
        Q_EMIT mModel->lazyUpdateView();
        mModel->setSelectedItemStatus(ScannerResultItem::MisReport);
        updateStatus();
    });

    mExpBtn->connect(mExpBtn, &PushButton::clicked, this, [=] () {
        QList<ScannerResultItem*> ls = mModel->getSelectedItem();
        if (ls.count() <= 0) {
            QMessageBox::warning(this, "警告", "请选中需要导出的数据后，再执行导出操作！", QMessageBox::Ok);
            return;
        }

#if 0
        QString name = QString("%1_%2_%3")
            .arg(const_cast<ScannerResultItem*>(ls.first())->getTaskName())
            .arg(QDate::currentDate().toString("yyyyMMdd"))
            .arg(QDateTime::currentDateTime().toString("hhmmss"));
        QFileDialog dlg;
        dlg.setDefaultSuffix(".csv");
        dlg.setNameFilter("*.csv");
        dlg.setFileMode(QFileDialog::AnyFile);
        dlg.setAcceptMode(QFileDialog::AcceptSave);
        dlg.setLabelText(QFileDialog::FileName, name);
        dlg.setDirectory(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));

        if (dlg.exec() && !dlg.selectedFiles().isEmpty()) {
            QString path = dlg.selectedFiles().first();
#else
        QString name = QString ("%1/%2")
                            .arg(QStandardPaths::writableLocation(QStandardPaths::HomeLocation))
                            .arg(QString("%1_%2_%3.csv")
                                .arg(const_cast<ScannerResultItem*>(ls.first())->getTaskName())
                                .arg(QDate::currentDate().toString("yyyyMMdd"))
                                .arg(QDateTime::currentDateTime().toString("hhmmss")));
                            
        qDebug() << "0000: " << QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        qDebug() << "name: " << name;
                            
        QString path = QFileDialog::getSaveFileName(this, "保存文件", name, "Document (*.csv)");
        if (!path.isEmpty()) {
#endif
            qDebug() << "name: " << name;
            qDebug() << "path: " << path;
            ExportScanResult exp(path, this);
            for (auto l : ls) {
                exp.write(*const_cast<ScannerResultItem*>(l));
                const_cast<ScannerResultItem*>(l)->setChecked(false);
                mHeaderView->setChecked (false);
            }
        }
    });

    
    // 更新状态
    connect (mModel, &ScannerResultModel::dataChanged, this, [=] (const QModelIndex &topLeft,
             const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>()) {
        
       Q_EMIT statusString (QString("任务名称: (%1), 总条数: (%2), 未处理: (%3), 误报: (%4), 删除: (%5)")
                .arg(mTaskName).arg(mModel->getAllCount ()).arg (mModel->getNoFixCount ()).arg (mModel->getMisReportCount ()).arg (mModel->getDeleteCount ()));
    });

    connect (mModel, &ScannerResultModel::lazyUpdateView, this, [=] () {
        //
        auto s = mView->verticalScrollBar();
        auto scrollBarRatio = float(s->value()) / (s->maximum() - s->minimum());
        auto curItemIndex = scrollBarRatio * mModel->rowCount();
        auto startIndex = ((curItemIndex - 30) >= 0) ? (curItemIndex - 30) : 0;
        auto stopIndex = ((curItemIndex + 30) < mModel->rowCount()) ? (curItemIndex + 30) : mModel->rowCount();

        // 开始更新
        for (auto i = startIndex; i <= stopIndex; ++i) {
            mView->update(mModel->index(i, 0));
            mView->update(mModel->index(i, 2));
        }

#if DEBUG
        qInfo() << visibleRegion();
        qInfo() << QString("min: %1, max: %2, cur: %3, %4 %%").arg(s->minimum()).arg(s->maximum()).arg(s->value()).arg(float(s->value()) / (s->maximum() - s->minimum()));
#endif
    });

    connect (mHeaderView, &HeaderView::checkBoxClicked, this, [=] (bool s) {
        mModel->selectAll(s);           // model select All
        Q_EMIT checkedItem(s);

        if (mModel->rowCount () <= 0) {
            Q_EMIT mDelBtn->enable (false);
            Q_EMIT mMisBtn->enable (false);
            Q_EMIT mExpBtn->enable (false);
        }
    });

    connect (mView, &QAbstractItemView::doubleClicked, this, [=] (const QModelIndex& index) {
        if (!index.isValid())   return;
        if (ScannerResultModel::FileName == index.column() && index.row() >= 0) {
            QDBusConnection dbus = QDBusConnection::connectToBus (QDBusConnection::SessionBus, FREEDESKTOP_FM_DBUS);
            if (dbus.isConnected()) {
                QDBusMessage msg = QDBusMessage::createMethodCall(FREEDESKTOP_FM_DBUS, FREEDESKTOP_FM_DBUS_PATH, FREEDESKTOP_FM_DBUS, "ShowItems");
                QString file = static_cast<ScannerResultItem*>(index.internalPointer())->getFileName();
                if (!QFile::exists(file)) {
                    QMessageBox::warning(this, "文件打开失败", QString("文件 '%1' 不存在!").arg(file), QMessageBox::Ok);
                    return;
                }
                msg.setArguments(QList<QVariant>() << (QStringList() << "file://" + file) << "");
                QDBusMessage reply = dbus.call(msg);
                if (reply.isDelayedReply() && QDBusMessage::ErrorMessage == reply.ReplyMessage) {
                    qDebug() << reply.errorMessage();
                }
            }
        }
    });

    connect (mView, &QAbstractItemView::clicked, this, [=] (const QModelIndex &index) {
        if (!index.isValid())   return;
        if (index.column() == 0) {
            auto item = static_cast<ScannerResultItem*>(index.internalPointer());
            if (item)   item->setChecked(!item->getChecked());
            Q_EMIT mView->update(index);
            bool checkAll = mModel->isCheckAllItems();
            mHeaderView->setChecked(checkAll);
            Q_EMIT checkedItem(checkAll || mModel->hasChecked ()); 
            mView->updateView();
        } else if (ScannerResultModel::FileName == index.column() && index.row() >= 0) {
#if 0
            QDBusConnection dbus = QDBusConnection::connectToBus (QDBusConnection::SessionBus, FREEDESKTOP_FM_DBUS);
            if (dbus.isConnected()) {
                QDBusMessage msg = QDBusMessage::createMethodCall(FREEDESKTOP_FM_DBUS, FREEDESKTOP_FM_DBUS_PATH, FREEDESKTOP_FM_DBUS, "ShowItems");
                QString file = static_cast<ScannerResultItem*>(index.internalPointer())->getFileName();
                if (!QFile::exists(file)) {
                    QMessageBox::warning(this, "文件打开失败", QString("文件 '%1' 不存在!").arg(file), QMessageBox::Ok);
                    return;
                }
                msg.setArguments(QList<QVariant>() << (QStringList() << "file://" + file) << "");
                QDBusMessage reply = dbus.call(msg);
                if (reply.isDelayedReply() && QDBusMessage::ErrorMessage == reply.ReplyMessage) {
                    qDebug() << reply.errorMessage();
                }
            }
#endif
        } else if (ScannerResultModel::Status == index.column() && index.row() >= 0) {

            Q_EMIT mView->activated(index);
        }
    });
    
    connect (this, &ScannerResultWidget::checkedItem, this, [=] (bool b) {
        Q_EMIT mDelBtn->enable (b);
        Q_EMIT mMisBtn->enable (b);
        Q_EMIT mExpBtn->enable (b);
    });

    connect (mView, &QAbstractItemView::entered, this, [=] (const QModelIndex &index) {
        if (!index.isValid())   return;
        if (0 == index.column() || ScannerResultModel::FileName == index.column()) {
            setCursor(Qt::PointingHandCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }

        if (ScannerResultModel::FileName == index.column() && index.row() >= 0) {
            //QPoint p = mView->visualRect(index).bottomRight();
            QPoint p = QCursor().pos (); //mView->visualRect(index).bottomRight();
            QString text = static_cast<ScannerResultItem*>(index.internalPointer())->getFileName();
            if (nullptr != text && !text.isNull() && !text.isEmpty() && "" != text) {
                QFontMetrics fm(font());
                int w = fm.horizontalAdvance(text);
                int h = fm.height();
                // void showText(const QPoint &pos, const QString &text, QWidget *w, const QRect &rect, int msecDisplayTime)
                QToolTip::showText(p, text, this, QRect(+100, -100, w, h), 3000000);
            }
        }
    });

    connect (mView->verticalScrollBar(), &QScrollBar::valueChanged, this, [=] (int val) {
        auto s = mView->verticalScrollBar();
        mModel->onScrollbarMoved(float(s->value()) / (s->maximum() - s->minimum()));
    });

//    test();
    
    connect (this, &ScannerResultWidget::updateView, mView, &ScannerView::updateView);

    setLayout(mMainLayout);
}

ScannerResultWidget::~ScannerResultWidget()
{
    //google::protobuf::ShutdownProtobufLibrary();
}

void ScannerResultWidget::test()
{
    auto sm1 = new ScannerResultItem("任务1", "/tmp/", ScannerResultItem::Untreated, 0, 0);
    auto sm2 = new ScannerResultItem("任务2", "/tmp/", ScannerResultItem::Untreated, 0, 0);
    auto sm3 = new ScannerResultItem("任务3", "/tmp/", ScannerResultItem::Untreated, 0, 0);
    auto sm4 = new ScannerResultItem("任务4", "/tmp/", ScannerResultItem::Untreated, 0, 0);
    auto sm5 = new ScannerResultItem("任务5", "/tmp/", ScannerResultItem::Untreated, 0, 0);
    auto sm6 = new ScannerResultItem("任务6", "/tmp/", ScannerResultItem::Untreated, 0, 0);
    auto sm7 = new ScannerResultItem("任务7", "/tmp/", ScannerResultItem::Untreated, 0, 0);
    auto sm8 = new ScannerResultItem("任务8", "/tmp/", ScannerResultItem::Untreated, 0, 0);
    auto sm9 = new ScannerResultItem("任务9", "/tmp/", ScannerResultItem::Untreated, 0, 0);
    auto sm10 = new ScannerResultItem("任务10", "/tmp/", ScannerResultItem::Untreated, 0, 0);
    auto sm11 = new ScannerResultItem("任务11", "/tmp/", ScannerResultItem::Untreated, 0, 0);
    auto sm12 = new ScannerResultItem("任务12", "/tmp/", ScannerResultItem::Untreated, 0, 0);
    auto sm13 = new ScannerResultItem("任务13", "/tmp/", ScannerResultItem::Untreated, 0, 0);
    auto sm14 = new ScannerResultItem("任务14", "/tmp/", ScannerResultItem::Untreated, 0, 0);
    auto sm15 = new ScannerResultItem("任务15", "/tmp/", ScannerResultItem::Untreated, 0, 0);

    mModel->addItem(sm1);
    mModel->addItem(sm2);
    mModel->addItem(sm3);
    mModel->addItem(sm4);
    mModel->addItem(sm5);
    mModel->addItem(sm6);
    mModel->addItem(sm7);
    mModel->addItem(sm8);
    mModel->addItem(sm9);
    mModel->addItem(sm10);
    mModel->addItem(sm11);
    mModel->addItem(sm12);
    mModel->addItem(sm13);
    mModel->addItem(sm14);
    mModel->addItem(sm15);
}

void ScannerResultWidget::setTaskName(QString name)
{
    mTaskName = name;
}

bool ScannerResultWidget::hasChecked()
{
   if (mMisBtn)     return mMisBtn->isEnable ();
   
   return true;
}

void ScannerResultWidget::clearData()
{
    if (!mModel)        return;

    Q_EMIT mModel->clearData();
}

void ScannerResultWidget::loadTaskResult(QString taskName, QString taskFilter, QStringList scanDir)
{
    if (!mModel)        return;

    // 加载前清除数据
    clearData();

    mTaskName = taskName;

    Q_EMIT mModel->showData(taskName, taskFilter, scanDir);
}

void ScannerResultWidget::updateStatus()
{
    Q_EMIT statusString (QString("任务名称: (%1), 总条数: (%2), 未处理: (%3), 误报: (%4), 删除: (%5)")
                         .arg(mTaskName).arg(mModel->getAllCount ()).arg (mModel->getNoFixCount ()).arg (mModel->getMisReportCount ()).arg (mModel->getDeleteCount ()));
}

void ScannerResultWidget::onBackToTaskView()
{
    Q_EMIT applyData ();
    Q_EMIT returnTaskList();
    Q_EMIT checkedItem(false);
    mHeaderView->setChecked(false);
    Q_EMIT mHeaderView->checkBoxClicked (false);
    DBManager::instance()->setCurPage(DBManager::CUR_TASK);

    // FIXME:// 释放数据 暂时放在跳转到 result 界面时候(虽然这样浪费了大量内存，但是临时解决了崩溃)，后续 item 都用智能指针管理其生命周期
    // Q_EMIT mModel->clearData();
}

void ScannerResultWidget::setScanDir(QString name)
{
    mScanDir = name.split("|");
}

void ScannerResultWidget::setScanFilter(QString name)
{
    mScanFilter = name;
}

void ScannerResultWidget::setScanDir(QStringList name)
{
    mScanDir = name;
}

void ScannerResultWidget::resizeEvent(QResizeEvent *event)
{
    // 放大
    if ((event->oldSize().width() > 0) && (event->size().width() > event->oldSize().width())) {
        setBigSize();
    } else {
        setDefaultSize();
    }

    QWidget::resizeEvent(event);
}

void ScannerResultWidget::setBigSize()
{
    if (!mView || !mView->horizontalHeader()) return;

    mView->horizontalHeader()->resizeSection (0, 80);
    mView->horizontalHeader()->resizeSection (2, 120);
    mView->horizontalHeader()->resizeSection (3, 240);
    mView->horizontalHeader()->resizeSection (4, 240);
}

void ScannerResultWidget::setDefaultSize()
{
    if (!mView || !mView->horizontalHeader()) return;

    mView->horizontalHeader()->resizeSection (0, 40);
    mView->horizontalHeader()->resizeSection (2, 90);
    mView->horizontalHeader()->resizeSection (3, 200);
    mView->horizontalHeader()->resizeSection (4, 200);
}
