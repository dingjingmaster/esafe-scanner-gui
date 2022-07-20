#include "scanner-task-widget.h"
#include "model/scanner-task-item.h"
#include "scanner-task-delegate.h"

#include <QDebug>
#include <QVBoxLayout>

ScannerTaskWidget::ScannerTaskWidget(QWidget *parent)
    : QWidget{parent}
{
    mMainLayout = new QVBoxLayout;

    mView = new ScannerView;
    mModel = new ScannerTaskModel;

    mView->setModel(mModel);
    mMainLayout->addWidget(mView);

    mView->setItemDelegate(new ScannerTaskDelegate);
    setLayout(mMainLayout);

    connect (mView, &QAbstractItemView::doubleClicked, this, [=] (const QModelIndex &index) {
        qDebug() << QString("r:%1, c:%2").arg(index.row()).arg(index.column()) << "clicked";
    });

    connect (mView, &QAbstractItemView::entered, this, [=] (const QModelIndex &index) {
        if (ScannerTaskModel::TaskOperation == index.column()) {
            setCursor(Qt::PointingHandCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
    });

    test();
}

void ScannerTaskWidget::test()
{
    auto sm1 = new ScannerTaskItem("任务1", ScannerTaskItem::Scanning, 0, 0, "已扫描文件数:50\n文件总数:100\n终端文件数:300");
    auto sm2 = new ScannerTaskItem("任务2", ScannerTaskItem::Finish, 0, 0, "已扫描文件数:50\n文件总数:100\n终端文件数:300");
    auto sm3 = new ScannerTaskItem("任务3", ScannerTaskItem::Scanning, 0, 0, "已扫描文件数:50\n文件总数:100\n终端文件数:300");
    auto sm4 = new ScannerTaskItem("任务4", ScannerTaskItem::Scanning, 0, 0, "已扫描文件数:50\n文件总数:100\n终端文件数:300");
    auto sm5 = new ScannerTaskItem("任务5", ScannerTaskItem::Scanning, 0, 0, "已扫描文件数:50\n文件总数:100\n终端文件数:300");
    auto sm6 = new ScannerTaskItem("任务6", ScannerTaskItem::Scanning, 0, 0, "已扫描文件数:50\n文件总数:100\n终端文件数:300");
    auto sm7 = new ScannerTaskItem("任务7", ScannerTaskItem::Finish, 0, 0, "已扫描文件数:50\n文件总数:100\n终端文件数:300");
    auto sm8 = new ScannerTaskItem("任务8", ScannerTaskItem::Scanning, 0, 0, "已扫描文件数:50\n文件总数:100\n终端文件数:300");
    auto sm9 = new ScannerTaskItem("任务9", ScannerTaskItem::Scanning, 0, 0, "已扫描文件数:50\n文件总数:100\n终端文件数:300");
    auto sm10 = new ScannerTaskItem("任务10", ScannerTaskItem::Scanning, 0, 0, "已扫描文件数:50\n文件总数:100\n终端文件数:300");
    auto sm11 = new ScannerTaskItem("任务11", ScannerTaskItem::Finish, 0, 0, "已扫描文件数:50\n文件总数:100\n终端文件数:300");
    auto sm12 = new ScannerTaskItem("任务12", ScannerTaskItem::Finish, 0, 0, "已扫描文件数:50\n文件总数:100\n终端文件数:300");
    auto sm13 = new ScannerTaskItem("任务13", ScannerTaskItem::Scanning, 0, 0, "已扫描文件数:50\n文件总数:100\n终端文件数:300");
    auto sm14 = new ScannerTaskItem("任务14", ScannerTaskItem::Finish, 0, 0, "已扫描文件数:50\n文件总数:100\n终端文件数:300");
    auto sm15 = new ScannerTaskItem("任务15", ScannerTaskItem::Scanning, 0, 0, "已扫描文件数:50\n文件总数:100\n终端文件数:300");

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
