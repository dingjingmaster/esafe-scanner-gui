//
// Created by dingjing on 22-12-1.
//

#include "scanner-result-widget-menu.h"

#include <QDebug>

ScannerResultWidgetMenu::ScannerResultWidgetMenu(QWidget *parent)
    : QMenu (nullptr, parent), mIndex(QModelIndex())
{
    connect (addAction ("删除"), &QAction::triggered, this, [=] (bool) {
        Q_EMIT deleteItem (mIndex);
    });

    connect (addAction ("例外文件"), &QAction::triggered, this, [=] (bool) {
        Q_EMIT misReportItem(mIndex);
    });
}

void ScannerResultWidgetMenu::setItem(QModelIndex &idx)
{
    mIndex = idx;
}
