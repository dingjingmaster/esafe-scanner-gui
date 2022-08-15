#include "scanner-task-widget.h"
#include "model/model-item-sort.h"
#include "model/scanner-task-item.h"
#include "scanner-task-delegate.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QResizeEvent>
#include <QtWidgets/QToolTip>


ScannerTaskWidget::ScannerTaskWidget(QWidget *parent)
    : QWidget{parent}
{
    mMainLayout = new QVBoxLayout;

    mView = new ScannerView;
    mModel = new ScannerTaskModel;
    mProxyModel = new ModelItemSort(this);

    mProxyModel->setSourceModel(mModel);

    mView->setModel(mProxyModel);
    mView->horizontalHeader()->setMinimumSectionSize(40);
    mView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    mView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    mView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
    mView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Interactive);
    mView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Interactive);
    mView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Interactive);
    mView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Interactive);

    //mView->setSortingEnabled(true);
    mMainLayout->addWidget(mView);

    mView->setItemDelegate(new ScannerTaskDelegate);
    setLayout(mMainLayout);

    // FIXME:// 此处需要修改
    //connect (mView->horizontalHeader (), &QHeaderView::sectionResized, [=] (int index, int oldSize, int newSize) {
    //    QRect viewRect = mView->rect ();
    //});

#if 1
    connect (mModel, &ScannerTaskModel::dataChanged, this, [=] (const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>()) {
        mView->update();
    });
#endif

    connect (mView, &QAbstractItemView::clicked, this, [=] (const QModelIndex &indexT) {
        QModelIndex index = mProxyModel->mapToSource(indexT);
        //qDebug() << QString("r:%1, c:%2").arg(index.row()).arg(index.column()) << "clicked";

        // 跳转到扫描结果显示页面
        if (ScannerTaskModel::TaskOperation == index.column()) {
            Q_EMIT taskDetail(static_cast<ScannerTaskItem*>(index.internalPointer()));
        }
    });

    connect (mView, &QAbstractItemView::entered, this, [=] (const QModelIndex &indexT) {
        QModelIndex index = mProxyModel->mapToSource(indexT);
        if (ScannerTaskModel::TaskOperation == index.column()) {
            setCursor(Qt::PointingHandCursor);
        } else if (ScannerTaskModel::TaskName == index.column() && index.row() >= 0) {
            //QPoint p = mView->visualRect(index).bottomRight();
            QPoint p = QCursor().pos (); //mView->visualRect(index).bottomRight();
            QString text = static_cast<ScannerTaskItem*>(index.internalPointer())->getName();
            if (nullptr != text && !text.isNull() && !text.isEmpty() && "" != text) {
                QFontMetrics fm(font());
                int w = fm.horizontalAdvance(text);
                int h = fm.height();
                // void showText(const QPoint &pos, const QString &text, QWidget *w, const QRect &rect, int msecDisplayTime)
                QToolTip::showText(p, text, this, QRect(+100, -100, w, h), 3000000);
            }

        } else {
            setCursor(Qt::ArrowCursor);
        }
    });

    // 界面测试数据
//    test();
}

void ScannerTaskWidget::test()
{
    auto sm1 = new ScannerTaskItem("任务1", ScannerTaskItem::Scanning, 0, 0);
    auto sm2 = new ScannerTaskItem("任务2", ScannerTaskItem::Finish, 0, 0);
    auto sm3 = new ScannerTaskItem("任务3", ScannerTaskItem::Scanning, 0, 0);
    auto sm4 = new ScannerTaskItem("任务4", ScannerTaskItem::Scanning, 0, 0);
    auto sm5 = new ScannerTaskItem("任务5", ScannerTaskItem::Scanning, 0, 0);
    auto sm6 = new ScannerTaskItem("任务6", ScannerTaskItem::Scanning, 0, 0);
    auto sm7 = new ScannerTaskItem("任务7", ScannerTaskItem::Finish, 0, 0);
    auto sm8 = new ScannerTaskItem("任务8", ScannerTaskItem::Scanning, 0, 0);
    auto sm9 = new ScannerTaskItem("任务9", ScannerTaskItem::Scanning, 0, 0);
    auto sm10 = new ScannerTaskItem("任务10", ScannerTaskItem::Scanning, 0, 0);
    auto sm11 = new ScannerTaskItem("任务11", ScannerTaskItem::Finish, 0, 0);
    auto sm12 = new ScannerTaskItem("任务12", ScannerTaskItem::Finish, 0, 0);
    auto sm13 = new ScannerTaskItem("任务13", ScannerTaskItem::Scanning, 0, 0);
    auto sm14 = new ScannerTaskItem("任务14", ScannerTaskItem::Finish, 0, 0);
    auto sm15 = new ScannerTaskItem("任务15", ScannerTaskItem::Scanning, 0, 0);

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

void ScannerTaskWidget::resizeEvent(QResizeEvent *event)
{
    // 放大
    if ((event->oldSize().width() > 0) && (event->size().width() > event->oldSize().width())) {
        setBigSize();
    } else {
        setDefaultSize();
    }

    QWidget::resizeEvent(event);
}

void ScannerTaskWidget::setDefaultSize()
{
    if (!mView || !mView->horizontalHeader()) return;

    mView->horizontalHeader()->resizeSection (0, 40);
    mView->horizontalHeader()->resizeSection (2, 70);
    mView->horizontalHeader()->resizeSection (3, 180);
    mView->horizontalHeader()->resizeSection (4, 180);
    mView->horizontalHeader()->resizeSection (5, 180);
    mView->horizontalHeader()->resizeSection (6, 90);
}

void ScannerTaskWidget::setBigSize()
{
    if (!mView || !mView->horizontalHeader()) return;

    mView->horizontalHeader()->resizeSection (0, 80);
    mView->horizontalHeader()->resizeSection (2, 100);
    mView->horizontalHeader()->resizeSection (3, 300);
    mView->horizontalHeader()->resizeSection (4, 300);
    mView->horizontalHeader()->resizeSection (5, 300);
    mView->horizontalHeader()->resizeSection (6, 100);
}
