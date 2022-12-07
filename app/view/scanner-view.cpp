#include "scanner-view.h"
#include <QHeaderView>

#include <QScrollBar>

ScannerView::ScannerView(QWidget *parent)
    : QTableView(parent)
{
    setMouseTracking(true);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::NoSelection);

    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void ScannerView::paintEvent(QPaintEvent *ev)
{
    QTableView::paintEvent(ev);
}

