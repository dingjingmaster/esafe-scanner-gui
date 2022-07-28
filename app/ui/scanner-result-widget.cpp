#include "push-button.h"
#include "view/header-view.h"
#include "scanner-result-widget.h"
#include "utils/notify-to-filter.h"
#include "scanner-result-delegate.h"
#include "utils/export-scan-result.h"
#include "utils/message-with-fp.pb.h"
#include "model/scanner-result-item.h"
#include "model/scanner-result-model.h"

#include <QDebug>
#include <QToolTip>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDBusMessage>
#include <QStandardPaths>
#include <QDBusConnection>
#include <QDBusPendingCall>
#include <QDateTime>
#include <QFontMetrics>

#define FREEDESKTOP_FM_DBUS             "org.freedesktop.FileManager1"
#define FREEDESKTOP_FM_DBUS_PATH        "/org/freedesktop/FileManager1"


ScannerResultWidget::ScannerResultWidget(QWidget *parent)
    : QWidget{parent}
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    mMainLayout = new QVBoxLayout;

    mBtnLayout = new QHBoxLayout;
    mLeftLayout = new QHBoxLayout;
    PushButton* retBtn = new PushButton(this, PushButton::Type2);
    retBtn->setText(tr("返回"));
    retBtn->setStyleSheet("background-color:red;");
    mLeftLayout->addWidget(retBtn);

    connect (retBtn, &PushButton::clicked, this, [=] () { Q_EMIT returnTaskList(); Q_EMIT mModel->clearData(); });

    mRightLayout = new QHBoxLayout;
    mRightLayout->setSpacing(6);

    PushButton* delBtn = new PushButton(this, PushButton::Type2);
    delBtn->setStyleSheet("background-color:red;");
    delBtn->setText(tr("删除"));
    mRightLayout->addWidget(delBtn);
    delBtn->connect(delBtn, &PushButton::clicked, this, [=] () {
        QList<const ScannerResultItem*> ls = mModel->getSelectedItem();
        if (ls.count() <= 0) {
            QMessageBox::warning(this, "警告", "请选中需要删除的数据后，再执行删除操作！", QMessageBox::Ok);
            return;
        }

        for (auto l : ls) {
            com::esafenet::scanner::client::ScannerClientMessage msg;
            msg.Clear();
            msg.set_filename(const_cast<ScannerResultItem*>(l)->getFileName().toStdString());

            // FIXME://
            msg.set_id(0);
            msg.set_operation(OP_DELETE);
            qDebug() << "delete: " << msg.DebugString().c_str();
            NotifyToFilter::getInstance()->sendData(msg.SerializeAsString());
        }

    });

    PushButton* misBtn = new PushButton(this, PushButton::Type2);
    misBtn->setStyleSheet("background-color:red;");
    misBtn->setText(tr("误报"));
    mRightLayout->addWidget(misBtn);
    misBtn->connect(misBtn, &PushButton::clicked, this, [=] () {
        QList<const ScannerResultItem*> ls = mModel->getSelectedItem();
        if (ls.count() <= 0) {
            QMessageBox::warning(this, "警告", "请选中误报的数据后，再执行操作！", QMessageBox::Ok);
            return;
        }

        for (auto l : ls) {
            com::esafenet::scanner::client::ScannerClientMessage msg;
            msg.Clear();
            msg.set_filename(const_cast<ScannerResultItem*>(l)->getFileName().toStdString());

            // FIXME://
            msg.set_id(0);
            msg.set_operation(OP_MISINFO);
            qDebug() << "misinformation: " << msg.DebugString().c_str();
            NotifyToFilter::getInstance()->sendData(msg.SerializeAsString());
        }
    });

    PushButton* expBtn = new PushButton(this, PushButton::Type2);
    expBtn->setStyleSheet("background-color:red;");
    expBtn->setText(tr("导出"));
    mRightLayout->addWidget(expBtn);
    expBtn->connect(expBtn, &PushButton::clicked, this, [=] () {
        QList<const ScannerResultItem*> ls = mModel->getSelectedItem();
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
            }
        }
    });

    mBtnLayout->addItem(mLeftLayout);
    mBtnLayout->addStretch();
    mBtnLayout->addItem(mRightLayout);
    mMainLayout->addItem(mBtnLayout);

    // tabview
    mModel = new ScannerResultModel;
    mView = new ScannerView;

    mView->setItemDelegate(new ScannerResultDelegate(this));
    HeaderView* headerView = new HeaderView(Qt::Horizontal, mView);
    mView->setHorizontalHeader(headerView);

    mView->setModel(mModel);
    mMainLayout->addWidget(mView);

    connect (headerView, &HeaderView::checkBoxClicked, this, [=] (bool s) {
        mModel->selectAll(s);
    });

    connect (mView, &QAbstractItemView::clicked, this, [=] (const QModelIndex &index) {
        qDebug() << QString("r:%1, c:%2").arg(index.row()).arg(index.column()) << "clicked";
        if (!index.isValid())   return;
        if (index.column() == 0) {
            auto item = static_cast<ScannerResultItem*>(index.internalPointer());
            if (item)   item->setChecked(!item->getChecked());
            Q_EMIT mView->update(index);
            headerView->setChecked(mModel->isCheckAllItems());
        } else if (ScannerResultModel::FileName == index.column() && index.row() >= 0) {
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

    connect (mView, &QAbstractItemView::entered, this, [=] (const QModelIndex &index) {
        if (!index.isValid())   return;
        if (0 == index.column() || ScannerResultModel::FileName == index.column()) {
            setCursor(Qt::PointingHandCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }

        if (ScannerResultModel::FileName == index.column() && index.row() >= 0) {
            QPoint p = mView->visualRect(index).bottomRight();
            QString text = static_cast<ScannerResultItem*>(index.internalPointer())->getFileName();
            QFontMetrics fm(font());
            int w = fm.horizontalAdvance(text);
            int h = fm.height();
            // void showText(const QPoint &pos, const QString &text, QWidget *w, const QRect &rect, int msecDisplayTime)
            QToolTip::showText(mapToGlobal(p), text, this, QRect(p.x(), p.y(), w, h), 3000000);
        }
    });

//    test()u;

    setLayout(mMainLayout);
}

ScannerResultWidget::~ScannerResultWidget()
{
    google::protobuf::ShutdownProtobufLibrary();
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

void ScannerResultWidget::clearData()
{
    if (!mModel)        return;

    mModel->clearData();
}

void ScannerResultWidget::loadTaskResult(QString taskName, QString taskFilter)
{
    if (!mModel)        return;

    Q_EMIT mModel->showData(taskName, taskFilter);
}
