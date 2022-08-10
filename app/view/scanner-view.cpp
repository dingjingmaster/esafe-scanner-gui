#include "scanner-view.h"

#include <QScrollBar>
#include <QHeaderView>
#include <QWheelEvent>

ScannerView::ScannerView(QWidget *parent)
    : QTableView(parent)
{
    setMouseTracking(true);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);

    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    verticalScrollBar()->setMaximum (100);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, [=] (int v) {
        int total = verticalScrollBar ()->maximum ();
        double ratio = double(v) / total;

        Q_EMIT verticalbarValue(ratio);
    });

    connect (this, &ScannerView::updateView, this, [=] () {
        auto its = model ()->rowCount ();
        for (auto it = 0; it < its; ++it) {
            update (model ()->index (it, 2));
        }
    });
}

void ScannerView::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta ();

    int row;

    if (delta > 0) {
        row = 1;
    } else {
        row = -1;
    }

    Q_EMIT rowchanged (row);

    event->accept ();
}

