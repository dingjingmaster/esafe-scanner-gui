#include "scanner-result-delegate.h"

#include <QDebug>
#include <QPainter>
#include <QApplication>

ScannerResultDelegate::ScannerResultDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

QSize ScannerResultDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);

    qDebug() << "delegate size hint";

    return size;
}

void ScannerResultDelegate::paint(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);

    qDebug() << "delegate paint";

    QRect rect = option.rect;
    Qt::Alignment align = option.displayAlignment;

    switch (index.column()) {
    case 0: {
        QRect rectCB(rect.left() + 10, rect.top() + (rect.height() - 20) / 2, 20, 20);
        QStyleOptionButton cbOp;
        cbOp.state |= QStyle::State_Enabled;
        cbOp.state |= index.model()->data(index).toBool() ? QStyle::State_On : QStyle::State_Off;
        cbOp.rect = rectCB;
        QApplication::style()->drawControl(QStyle::CE_CheckBox, &cbOp, p);

        break;
    }
    case 1:
    case 2:
    case 3:
    case 4:
    case 5: {
        QString text = index.model()->data(index).toString();
        align |= Qt::AlignHCenter;
        p->drawText(rect, align, text);
        break;
    }
    default:
        break;
    }

}
