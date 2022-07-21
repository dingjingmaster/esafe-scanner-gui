#include "model/scanner-result-item.h"
#include "model/scanner-result-model.h"
#include "push-button.h"
#include "scanner-result-delegate.h"
#include "scanner-result-widget.h"
#include "view/header-view.h"

#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>

ScannerResultWidget::ScannerResultWidget(QWidget *parent)
    : QWidget{parent}
{
    mMainLayout = new QVBoxLayout;

    mBtnLayout = new QHBoxLayout;
    mLeftLayout = new QHBoxLayout;
    PushButton* retBtn = new PushButton(this, PushButton::Type2);
    retBtn->setText(tr("返回"));
    retBtn->setStyleSheet("background-color:red;");
    mLeftLayout->addWidget(retBtn);

    connect (retBtn, &PushButton::clicked, this, [=] () { Q_EMIT returnTaskList(); });

    mRightLayout = new QHBoxLayout;
    mRightLayout->setSpacing(6);

    PushButton* delBtn = new PushButton(this, PushButton::Type2);
    delBtn->setStyleSheet("background-color:red;");
    delBtn->setText(tr("删除"));
    mRightLayout->addWidget(delBtn);

    PushButton* misBtn = new PushButton(this, PushButton::Type2);
    misBtn->setStyleSheet("background-color:red;");
    misBtn->setText(tr("误报"));
    mRightLayout->addWidget(misBtn);

    PushButton* expBtn = new PushButton(this, PushButton::Type2);
    expBtn->setStyleSheet("background-color:red;");
    expBtn->setText(tr("导出"));
    mRightLayout->addWidget(expBtn);

    mBtnLayout->addItem(mLeftLayout);
    mBtnLayout->addStretch();
    mBtnLayout->addItem(mRightLayout);

    mMainLayout->addItem(mBtnLayout);

    // tabview
    mModel = new ScannerResultModel;
    mView = new ScannerView;

    mView->setItemDelegate(new ScannerResultDelegate);
    mView->setHorizontalHeader(new HeaderView(Qt::Horizontal, mView));

    mView->setModel(mModel);
    mMainLayout->addWidget(mView);

    connect (mView, &QAbstractItemView::clicked, this, [=] (const QModelIndex &index) {
        qDebug() << QString("r:%1, c:%2").arg(index.row()).arg(index.column()) << "clicked";
        if (!index.isValid())   return;
        if (index.column() == 0) {
            auto item = static_cast<ScannerResultItem*>(index.internalPointer());
            if (item)   item->setChecked(!item->getChecked());
            Q_EMIT mView->update(index);
        }
    });

    connect (mView, &QAbstractItemView::entered, this, [=] (const QModelIndex &index) {
        if (0 == index.column() || ScannerResultModel::FileName == index.column()) {
            setCursor(Qt::PointingHandCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
    });

    test();

    setLayout(mMainLayout);
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
