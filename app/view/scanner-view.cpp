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
}
