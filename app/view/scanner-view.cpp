#include "scanner-view.h"
#include <QHeaderView>

#include <QScrollBar>

ScannerView::ScannerView(QWidget *parent)
    : QTableView(parent)
{
    setMouseTracking(true);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);

    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // 更新
    //connect(verticalScrollBar(), &QScrollBar::valueChanged, this, [=] (int val) {
    //    auto v = verticalScrollBar();
    //    mCurPos = float(val) / (v->maximum() - v->minimum());
    //});

#if 0
    connect (this, &ScannerView::updateView, this, [=] () {
        return;
        auto its = model ()->rowCount ();
        for (auto it = 0; it < its; ++it) {


            update (model ()->index (it, 2));
        }
    });
#endif
}

void ScannerView::paintEvent(QPaintEvent *ev)
{
#if 0
    const QAbstractItemModel* md = model();
    if (md) {
        auto count = md->rowCount();
        auto cur = count * mCurPos + 1;
        auto start = cur - 30;
        auto end = cur + 30;
    }
#endif

    QTableView::paintEvent(ev);
}

