#include "scanner-view.h"
#include <QHeaderView>

ScannerView::ScannerView(QWidget *parent)
    : QTableView(parent)
{
    setMouseTracking(true);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);

    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    
    connect (this, &ScannerView::updateView, this, [=] () {
        auto its = model ()->rowCount ();
        for (auto it = 0; it < its; ++it) {
            update (model ()->index (it, 2));
        }
    });
}
