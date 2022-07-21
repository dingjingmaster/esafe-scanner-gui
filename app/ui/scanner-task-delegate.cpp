#include "scanner-task-delegate.h"

#include <QStyle>
#include <QDebug>
#include <QPainter>
#include <QItemDelegate>

ScannerTaskDelegate::ScannerTaskDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

QSize ScannerTaskDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);

    qDebug() << "delegate size hint";

    return size;
}

void ScannerTaskDelegate::paint(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);

    qDebug() << "delegate paint";

    p->save();

    QString text = index.model()->data(index).toString();
    QRect rect = option.rect;
    Qt::Alignment align = option.displayAlignment;

    switch (index.column()) {
    case 0:
    case 1:
    case 2:
        align |= Qt::AlignHCenter;
        break;
    case 6:
        p->setPen(Qt::blue);
        align |= Qt::AlignHCenter;
        break;
    default:
        break;
    }

    p->drawText(rect, align, text);

    p->restore();
}
