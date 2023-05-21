#include "push-button.h"
#include "threads/scanner-result-save-thread.h"
#include "view/header-view.h"
#include "scanner-result-widget.h"
#include "utils/notify-to-filter.h"
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
#include <QLocalSocket>
#include <QProcess>

#include "../db/db-manager.h"
#include "../widget/progress.h"
#include "../threads/event-loop.h"
#include "scanner-result-widget-menu.h"

#define FREEDESKTOP_FM_DBUS             "org.freedesktop.FileManager1"
#define FREEDESKTOP_FM_DBUS_PATH        "/org/freedesktop/FileManager1"

ScannerResultWidget::ScannerResultWidget(QWidget *parent)
    : QWidget{parent}
{
    setMouseTracking (true);

    mMainLayout = new QVBoxLayout;

    mBtnLayout = new QHBoxLayout;
    mLeftLayout = new QHBoxLayout;
    mRetBtn = new PushButton(this, PushButton::Type2);
    mRightLayout = new QHBoxLayout;
    
    mDelBtn = new PushButton(this, PushButton::Type2);
    mDSMBtn = new PushButton(this, PushButton::Type2);
    mMisBtn = new PushButton(this, PushButton::Type2);
    mExpBtn = new PushButton(this, PushButton::Type2);

    mStatusLabel = new QLabel;

    mModel = new ScannerResultModel;
    mView = new ScannerView;

    mView->setItemDelegate(new ScannerResultDelegate(this));
    mHeaderView = new HeaderView(Qt::Horizontal, mView);

    mProgress = new Progress(mView);
    mProgress->hide();

    setContextMenuPolicy (Qt::CustomContextMenu);
    mMenu = new ScannerResultWidgetMenu(this);

    connect(mModel, &ScannerResultModel::progress, mProgress, &Progress::updateProcess);

    mRetBtn->setText(tr("返回"));
    Q_EMIT mRetBtn->enable (true);
    mRetBtn->setStyleSheet("background-color:red;");
    mLeftLayout->addWidget(mRetBtn);

    mRightLayout->setSpacing(6);

    mDelBtn->setStyleSheet("background-color:red;");
    mDelBtn->setText(tr("删除"));
    mRightLayout->addWidget(mDelBtn);

    mMisBtn->setStyleSheet("background-color:red;");
    mMisBtn->setText(tr("例外文件"));
    mRightLayout->addWidget(mMisBtn);

    mDSMBtn->setStyleSheet("background-color:red;");
    mDSMBtn->setText(tr("授权加密"));
    mRightLayout->addWidget(mDSMBtn);

    mExpBtn->setStyleSheet("background-color:red;");
    mExpBtn->setText(tr("导出"));
    mRightLayout->addWidget(mExpBtn);

    mRightLayout->addWidget (mStatusLabel);

    mBtnLayout->addItem(mLeftLayout);
    mBtnLayout->addStretch();
    mBtnLayout->addItem(mRightLayout);
    mMainLayout->addItem(mBtnLayout);

    setNoSelectedStatus();

    // tabview
    mView->setHorizontalHeader(mHeaderView);
    mView->setModel(mModel);
    mMainLayout->addWidget(mView);

//    QItemSelectionModel* selection = mView->selectionModel();
//    connect (selection, &QItemSelectionModel::selectionChanged, this, [=] (const QItemSelection& selected, const QItemSelection& deselected) {
//        QModelIndexList  ls;
//        ls << selected.indexes() << deselected.indexes();
//        for (auto l : ls) {
//            ScannerResultItem* item = static_cast<ScannerResultItem*> (l.internalPointer());
//            if (item) {
//                qDebug() << item->getFileName();
//                if (item->getChecked()) {
//                    selection->select (l, QItemSelectionModel::Select | QItemSelectionModel::Rows);
//                } else {
//                    selection->select (l, QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
//                }
//            }
//        }
//        mView->update();
//        //selection->setCurrentIndex (idx, QItemSelectionModel::Select);
//    });

    connect (this, &ScannerResultWidget::startApplyData, this, [=] () {
        mRetBtn->enable(false);
        mExpBtn->enable(false);
        mDSMBtn->enable(false);
        mMisBtn->enable(false);
        mDelBtn->enable(false);
    });

    connect (this, &ScannerResultWidget::stopApplyData, this, [=] () {
        mRetBtn->enable(true);
        mMisBtn->enable(true);
        mDSMBtn->enable(true);
    });

    connect (mModel, &ScannerResultModel::lazyUpdateView, this, &ScannerResultWidget::lazyUpdateView);
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
        auto box = new QMessageBox(this);
        box->setText ("确认提交您对数据的更改吗？");
        box->setWindowTitle("提示");

        auto apply = new QPushButton(box);
        auto cancel = new QPushButton(box);

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

    connect (mRetBtn, &PushButton::clicked, this, &ScannerResultWidget::onBackToTaskView, Qt::UniqueConnection);

    connect (mDelBtn, &PushButton::clicked, this, [=] () {
        QList<ScannerResultItem*> ls = mModel->getSelectedItem();
        if (ls.count() <= 0) {
            QMessageBox msg(QMessageBox::NoIcon, "警告", "请选中需要删除的数据后，再执行删除操作！");
            msg.addButton (QMessageBox::Ok)->setText ("确定");
            msg.exec();
            return;
        }

        auto box = new QMessageBox(this);
        box->setText ("是否确定删除？");
        box->setWindowTitle("");

        auto apply = new QPushButton(box);
        auto cancel = new QPushButton(box);

        apply->setText ("确定");
        cancel->setText ("取消");

        box->addButton (apply, QMessageBox::AcceptRole);
        box->addButton (cancel, QMessageBox::RejectRole);
        box->connect (apply, &QPushButton::clicked, this, [=] () {
            for (auto l : ls) {
                mModel->setData (*l, "删除");
            }

            mModel->setSelectedItemStatus(ScannerResultItem::Deleted);
            Q_EMIT mModel->lazyUpdateView();

            Q_EMIT startApplyData();
            mProgress->show();
            mModel->applyData();
            mProgress->hide();
            Q_EMIT stopApplyData();

            mHeaderView->setChecked(false);
            Q_EMIT mHeaderView->checkBoxClicked (false);
            Q_EMIT mModel->lazyUpdateView();
            updateStatus();
            notify_policy_filter (ScannerResultItem::Deleted);
        });
        box->exec();
        box->deleteLater();
    });

    connect (mMisBtn, &PushButton::clicked, this, [=] () {
        QList<ScannerResultItem*> ls = mModel->getSelectedItem();
        if (ls.count() <= 0) {
            QMessageBox msg(QMessageBox::NoIcon, "警告", "请选中例外的数据后，再执行操作！");
            msg.addButton (QMessageBox::Ok)->setText ("确定");
            msg.exec();
            return;
        }

        auto box = new QMessageBox(this);
        box->setText ("是否确定为例外文件？");
        box->setWindowTitle("");

        auto apply = new QPushButton(box);
        auto cancel = new QPushButton(box);

        apply->setText ("确定");
        cancel->setText ("取消");

        box->addButton (apply, QMessageBox::AcceptRole);
        box->addButton (cancel, QMessageBox::RejectRole);

        box->connect (apply, &QPushButton::clicked, this, [=] () {
            for (const auto& l : ls) {
                mModel->setData (*l, "例外文件");
            }
            Q_EMIT mModel->lazyUpdateView();
            mModel->setSelectedItemStatus(ScannerResultItem::MisReport);

            Q_EMIT startApplyData();
            mProgress->show();
            mModel->applyData();
            mProgress->hide();
            Q_EMIT stopApplyData();

            mHeaderView->setChecked(false);
            Q_EMIT mHeaderView->checkBoxClicked (false);
            Q_EMIT mModel->lazyUpdateView();
            updateStatus();
            notify_policy_filter (ScannerResultItem::MisReport);
        });
        box->exec();
        box->deleteLater();
    });

    connect (mDSMBtn, &PushButton::clicked, this, [=] () {
        QList<ScannerResultItem*> ls = mModel->getSelectedItem();
        if (ls.count() <= 0) {
            QMessageBox msg(QMessageBox::NoIcon, "警告", "请选中要操作的文件，再执行操作！");
            msg.addButton (QMessageBox::Ok)->setText ("确定");
            msg.exec();
            return;
        }

        auto box = new QMessageBox(this);
        box->setText ("是否确定制作授权加密文件？");
        box->setWindowTitle("");

        auto apply = new QPushButton(box);
        auto cancel = new QPushButton(box);

        apply->setText ("确定");
        cancel->setText ("取消");

        box->addButton (apply, QMessageBox::AcceptRole);
        box->addButton (cancel, QMessageBox::RejectRole);

        box->connect (apply, &QPushButton::clicked, this, [=] () {
            Q_EMIT mModel->lazyUpdateView();

            QStringList params;
            auto ss = mModel->getSelectedItem();
            for (auto i : ss) {
                params << i->getFileName();
            }
            mModel->unSelectedItem();

            mHeaderView->setChecked(false);
            Q_EMIT mHeaderView->checkBoxClicked (false);
            Q_EMIT mModel->lazyUpdateView();
            updateStatus();

            QProcess::startDetached ("/usr/local/ultrasec/dsm/bin/dsm-gui", QStringList() << "-e" << params);
        });
        box->exec();
        box->deleteLater();
    });

    connect (mExpBtn, &PushButton::clicked, this, [=] () {
        QList<ScannerResultItem*> ls = mModel->getSelectedItem();
        if (ls.count() <= 0) {
            QMessageBox msg(QMessageBox::NoIcon, "警告", "请选中需要导出的数据后，再执行导出操作！");
            msg.addButton (QMessageBox::Ok)->setText ("确定");
            msg.exec();
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
                                .arg((ls.first())->getTaskName())
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
            for (auto& l : ls) {
                exp.write(*l);
                l->setChecked(false);
            }

            QMessageBox information (QMessageBox::NoIcon, nullptr, "文件导出成功！", QMessageBox::Ok, this);
            information.button (QMessageBox::Ok)->setText ("确定");
            information.button (QMessageBox::Ok)->setIcon(QIcon());
            information.setContentsMargins (0, 0, 0, 0);
            information.setStyleSheet ("* {padding:0; margin:0;}"
                                       " QPushButton{padding:3px;margin:3px;}"
                                       " QDialogButtonBox {align:center; qproperty-centerButtons:true;}");
            if (auto textField = information.findChild<QWidget*>("qt_msgbox_label")) {
                if (auto label = dynamic_cast<QLabel*> (textField)) {
                    label->setFixedSize (180, 60);
                    label->setContentsMargins (0, 0, 0, 0);
                    label->setAlignment (Qt::AlignCenter);
                }
            }
            information.exec();
        }

        // 允许选中
        mHeaderView->setChecked(false);
        Q_EMIT mHeaderView->checkBoxClicked (false);

        // 更新界面
        Q_EMIT mModel->lazyUpdateView();
    });

    connect (this, qOverload<QString>(&ScannerResultWidget::statusString), this, qOverload<QString>(&ScannerResultWidget::onShowStatusString));
    connect (this, qOverload<QString, int, int, int>(&ScannerResultWidget::statusString), this, qOverload<QString, int, int, int>(&ScannerResultWidget::onShowStatusString));

    // 更新状态
    connect (mModel, &ScannerResultModel::dataChanged, this, [=] (const QModelIndex &topLeft,
             const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>()) {
        Q_EMIT statusString (mTaskName, mModel->getNoFixCount(), mModel->getMisReportCount(), mModel->getAllCount()); //.arg (mModel->getDeleteCount ()));
    });

    connect (mModel, &ScannerResultModel::dataStatueChanged, this, [=] () {
        Q_EMIT statusString (mTaskName, mModel->getNoFixCount(), mModel->getMisReportCount(), mModel->getAllCount()); //.arg (mModel->getDeleteCount ()));
    });

//    connect (mModel, &ScannerResultModel::lazyUpdateView, this, [=] () {
//        //
//        auto s = mView->verticalScrollBar();
//        auto scrollBarRatio = float(s->value()) / (s->maximum() - s->minimum());
//        auto curItemIndex = scrollBarRatio * mModel->rowCount();
//        auto startIndex = ((curItemIndex - 30) >= 0) ? (curItemIndex - 30) : 0;
//        auto stopIndex = ((curItemIndex + 30) < mModel->rowCount()) ? (curItemIndex + 30) : mModel->rowCount();
//
//        // 开始更新
//        for (auto i = startIndex; i <= stopIndex; ++i) {
//            mView->update(mModel->index(i, 0));
//            mView->update(mModel->index(i, 2));
//        }
//
//#if DEBUG
//        qInfo() << visibleRegion();
//        qInfo() << QString("min: %1, max: %2, cur: %3, %4 %%").arg(s->minimum()).arg(s->maximum()).arg(s->value()).arg(float(s->value()) / (s->maximum() - s->minimum()));
//#endif
//    });

    connect (mHeaderView, &HeaderView::checkBoxClicked, this, [=] (bool s) {
        mModel->selectAll(s);           // model select All
        Q_EMIT checkedItem(s);

        if (mModel->rowCount () <= 0) {
            Q_EMIT mDelBtn->enable (false);
            Q_EMIT mMisBtn->enable (false);
            Q_EMIT mDSMBtn->enable (false);
            Q_EMIT mExpBtn->enable (false);
        }

        lazyUpdateView();
    });

    connect (mView, &QAbstractItemView::doubleClicked, this, [=] (const QModelIndex& index) {
        if (!index.isValid())   return;
        if (ScannerResultModel::FileName == index.column() && index.row() >= 0) {
            QDBusConnection dbus = QDBusConnection::connectToBus (QDBusConnection::SessionBus, FREEDESKTOP_FM_DBUS);
            if (dbus.isConnected()) {
                QDBusMessage msg = QDBusMessage::createMethodCall(FREEDESKTOP_FM_DBUS, FREEDESKTOP_FM_DBUS_PATH, FREEDESKTOP_FM_DBUS, "ShowItems");
                QString file = static_cast<ScannerResultItem*>(index.internalPointer())->getFileName();
                if (!QFile::exists(file)) {
                    QMessageBox msg(QMessageBox::NoIcon, "文件打开失败", QString("文件 '%1' 不存在, 或者当前用户没有查看权限!").arg(file));
                    msg.addButton (QMessageBox::Ok)->setText ("确定");
                    msg.exec();
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
            if (item) {
                item->setChecked(!item->getChecked());
            }
            Q_EMIT mView->update(index);
            bool checkAll = mModel->isCheckAllItems();
            mHeaderView->setChecked(checkAll);
            Q_EMIT checkedItem(checkAll || mModel->hasChecked ()); 
            //mView->updateView();
            lazyUpdateView();
        } else if ((ScannerResultModel::FileName == index.column()) && (index.row() >= 0)) {
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
        } else if ((ScannerResultModel::Status == index.column()) && (index.row() >= 0)) {

            Q_EMIT mView->activated(index);
        }
    });
    
    connect (this, &ScannerResultWidget::checkedItem, this, [=] (bool b) {
//        Q_EMIT mDelBtn->enable (b);
//        Q_EMIT mMisBtn->enable (b);
//        Q_EMIT mExpBtn->enable (b);
        if (b) {
            setSelectedStatus();
        }
        else {
            setNoSelectedStatus();
        }
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
            QPoint p = QCursor::pos (); //mView->visualRect(index).bottomRight();
            QString text = static_cast<ScannerResultItem*>(index.internalPointer())->getFileName();
            if (nullptr != text && !text.isNull() && !text.isEmpty() && "" != text) {
                QFontMetrics fm(font());
                int w = fm.horizontalAdvance(text);
                int h = fm.height();
                QToolTip::showText(p, text, this, QRect(+100, -100, w, h), 3000000);
            }
        }
    });

    connect (mView->verticalScrollBar(), &QScrollBar::valueChanged, this, [=] (int val) {
        auto s = mView->verticalScrollBar();
        mModel->onScrollbarMoved(float(s->value()) / float (s->maximum() - s->minimum()));
    });

    connect (this, &ScannerResultWidget::updateView, mView, &ScannerView::updateView);

    setLayout(mMainLayout);

    connect (mMenu, &ScannerResultWidgetMenu::deleteItem, mModel, &ScannerResultModel::applyDelData);
    connect (mMenu, &ScannerResultWidgetMenu::makeDSMItem, mModel, &ScannerResultModel::applyMakeDSM);
    connect (mMenu, &ScannerResultWidgetMenu::misReportItem, mModel, &ScannerResultModel::applyMisReportData);

    connect (this, &ScannerResultWidget::customContextMenuRequested, [=] (const QPoint& pos) {
        QModelIndex idx = mView->currentIndex(); //mView->indexAt (mView->mapFrom (this, pos));// ->indexAt((pos));
        if (!idx.isValid() || idx.row() < 0 || idx.column() < 0) return;
        mMenu->setItem (idx);
        mMenu->exec(QCursor::pos());
    });
}

ScannerResultWidget::~ScannerResultWidget()
{
    //google::protobuf::ShutdownProtobufLibrary();
}

void ScannerResultWidget::test()
{
    auto sm1 = (new ScannerResultItem("任务1", "/tmp/", ScannerResultItem::Untreated, 0, 0));
    auto sm2 = (new ScannerResultItem("任务2", "/tmp/", ScannerResultItem::Untreated, 0, 0));
    auto sm3 = (new ScannerResultItem("任务3", "/tmp/", ScannerResultItem::Untreated, 0, 0));
    auto sm4 = (new ScannerResultItem("任务4", "/tmp/", ScannerResultItem::Untreated, 0, 0));
    auto sm5 = (new ScannerResultItem("任务5", "/tmp/", ScannerResultItem::Untreated, 0, 0));
    auto sm6 = (new ScannerResultItem("任务6", "/tmp/", ScannerResultItem::Untreated, 0, 0));
    auto sm7 = (new ScannerResultItem("任务7", "/tmp/", ScannerResultItem::Untreated, 0, 0));
    auto sm8 = (new ScannerResultItem("任务8", "/tmp/", ScannerResultItem::Untreated, 0, 0));
    auto sm9 = (new ScannerResultItem("任务9", "/tmp/", ScannerResultItem::Untreated, 0, 0));
    auto sm10 = (new ScannerResultItem("任务10", "/tmp/", ScannerResultItem::Untreated, 0, 0));
    auto sm11 = (new ScannerResultItem("任务11", "/tmp/", ScannerResultItem::Untreated, 0, 0));
    auto sm12 = (new ScannerResultItem("任务12", "/tmp/", ScannerResultItem::Untreated, 0, 0));
    auto sm13 = (new ScannerResultItem("任务13", "/tmp/", ScannerResultItem::Untreated, 0, 0));
    auto sm14 = (new ScannerResultItem("任务14", "/tmp/", ScannerResultItem::Untreated, 0, 0));
    auto sm15 = (new ScannerResultItem("任务15", "/tmp/", ScannerResultItem::Untreated, 0, 0));

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
    Q_EMIT mModel->dataStatueChanged();

    updateView();
}

void ScannerResultWidget::loadTaskResult (QString taskName, QString taskFilter, QStringList scanDir, QString scanOutDir, QString scanType, QString scanTypeOut)
{
    if (!mModel)        return;

    // 加载前清除数据
    clearData();

    mTaskName = taskName;

    Q_EMIT mModel->showData(taskName, taskFilter, scanDir, scanOutDir, scanType, scanTypeOut);
}

void ScannerResultWidget::updateStatus()
{
    Q_EMIT statusString (QString("任务名称: %1, 未处理数 %2 条, 例外文件数 %3 条, 共 %4 条结果")
                         .arg(mTaskName).arg(mModel->getNoFixCount()).arg (mModel->getMisReportCount()).arg (mModel->getAllCount())); //.arg (mModel->getDeleteCount ()));
}

void ScannerResultWidget::onBackToTaskView()
{
    //Q_EMIT applyData ();
    Q_EMIT returnTaskList();
    Q_EMIT checkedItem(false);
    mHeaderView->setChecked(false);
    Q_EMIT mHeaderView->checkBoxClicked (false);
    DBManager::instance()->setCurPage(DBManager::CUR_TASK);
    Q_EMIT DBManager::instance()->refreshScanTask();

    // FIXME:// 释放数据 暂时放在跳转到 result 界面时候(虽然这样浪费了大量内存，但是临时解决了崩溃)，后续 item 都用智能指针管理其生命周期
     Q_EMIT mModel->clearData();
}

void ScannerResultWidget::setScanDir(const QString& name)
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
    mView->horizontalHeader()->resizeSection (2, 130);
    mView->horizontalHeader()->resizeSection (3, 250);
    mView->horizontalHeader()->resizeSection (4, 250);
}

void ScannerResultWidget::setDefaultSize()
{
    if (!mView || !mView->horizontalHeader()) return;

    mView->horizontalHeader()->resizeSection (0, 40);
    mView->horizontalHeader()->resizeSection (2, 100);
    mView->horizontalHeader()->resizeSection (3, 240);
    mView->horizontalHeader()->resizeSection (4, 240);
}

void ScannerResultWidget::lazyUpdateView()
{
    auto s = mView->verticalScrollBar();
    auto scrollBarRatio = float(s->value()) / (s->maximum() - s->minimum());
    auto curItemIndex = scrollBarRatio * mModel->rowCount();
    auto startIndex = ((curItemIndex - 30) >= 0) ? (curItemIndex - 30) : 0;
    auto stopIndex = ((curItemIndex + 30) < mModel->rowCount()) ? (curItemIndex + 30) : mModel->rowCount();

    // 开始更新
    for (auto i = startIndex; i <= stopIndex; ++i) {
        mView->update(mModel->index(i, 0));
        mView->update(mModel->index(i, 1));
        mView->update(mModel->index(i, 2));
        mView->update(mModel->index(i, 3));
        mView->update(mModel->index(i, 4));
    }
}

void ScannerResultWidget::onShowStatusString(QString str)
{
    mStatusLabel->setText(str);
//    if (!mStatusLabel->isHidden()) {
//        mStatusLabel->resize(QApplication::fontMetrics().size(Qt::TextSingleLine, str));
//    }
}

void ScannerResultWidget::setNoSelectedStatus()
{
    mDelBtn->hide();
    mMisBtn->hide();
    mDSMBtn->hide();
    mExpBtn->hide();

    mStatusLabel->show();
}

void ScannerResultWidget::setSelectedStatus()
{
    mDelBtn->enable (true);
    mMisBtn->enable (true);
    mDSMBtn->enable (true);
    mExpBtn->enable (true);

    mDelBtn->show();
    mMisBtn->show();
    mDSMBtn->show();
    mExpBtn->show();


    mStatusLabel->hide();
}

void ScannerResultWidget::onLoadResultStart()
{
    mRetBtn->enable (false);

}

void ScannerResultWidget::onLoadResultEnd()
{
    mRetBtn -> enable(true);
}

void ScannerResultWidget::onShowStatusString(QString t, int a, int b, int c)
{
    Q_EMIT statusString (QString("任务名称: %1 未处理数 <span style='color:#00CCFF;'>%2</span> 条 例外文件数 <span style='color:blue;'>%3</span> 条 共 %4 条结果").arg(t).arg(a).arg (b).arg (c)); //.arg (mModel->getDeleteCount ()));
}

