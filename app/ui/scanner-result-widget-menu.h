//
// Created by dingjing on 22-12-1.
//

#ifndef DSIP_SIT_SCANNERRESULTWIDGETMENU_H
#define DSIP_SIT_SCANNERRESULTWIDGETMENU_H

#include <QMenu>
#include <QModelIndex>

class ScannerResultWidgetMenu : public QMenu
{
    Q_OBJECT
public:
    explicit ScannerResultWidgetMenu(QWidget* parent = nullptr);

    void setItem (QModelIndex& idx);

Q_SIGNALS:
    void deleteItem(QModelIndex idx);
    void misReportItem(QModelIndex idx);

private:
    QModelIndex                 mIndex;
};


#endif //DSIP_SIT_SCANNERRESULTWIDGETMENU_H
