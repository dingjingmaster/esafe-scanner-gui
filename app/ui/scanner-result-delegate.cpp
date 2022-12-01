#include "scanner-result-delegate.h"

#include "model/scanner-result-item.h"
#include "scanner-result-widget.h"

#include <QDebug>
#include <QPainter>
#include <QComboBox>
#include <QApplication>
#include <QPainterPath>

ScannerResultDelegate::ScannerResultDelegate(QObject *parent)
    : QStyledItemDelegate{parent}, mObj(parent)
{

}

QSize ScannerResultDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);

    return size;
}

void ScannerResultDelegate::paint(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    p->save();

    QRect rect = option.rect;
    Qt::Alignment align = option.displayAlignment;
    QPalette pal;

    switch (index.column()) {
    case 0: {
        QRect rectCB(rect.left() + (rect.width() - 20) / 2, rect.top() + (rect.height() - 20) / 2, 20, 20);
        QStyleOptionButton cbOp;
        cbOp.initFrom(static_cast<QWidget*>(parent()));
        cbOp.state |= QStyle::State_Enabled;
        cbOp.rect = rectCB;

        QBrush bth;
        bth.setColor(Qt::darkRed);
        if (index.model()->data(index).toBool()) {
            cbOp.state |= QStyle::State_On;
        } else {
            cbOp.state |= QStyle::State_Off;
        }

        p->setBrush(bth);
        QApplication::style()->drawControl(QStyle::CE_CheckBox, &cbOp, p);
        break;
    }
    case 2: {
#if 1
        QStyledItemDelegate::paint (p, option, index);
#else
        if (auto sr = static_cast <ScannerResultWidget*>(mObj)) {
            if (sr->hasChecked()) {
                QStyledItemDelegate::paint (p, option, index);
                sr->update ();
                break;
            }
        }

        QStyledItemDelegate::paint (p, option, index);
        p->save();
        QRect rect1 = rect;
        rect1.setX (rect.left () + rect.width () - 8 - 6);
        rect1.setY (rect.top () + (float)(rect.height () - 5) / 2);
        rect1.setWidth (8);
        rect1.setHeight (5);
        QPainterPath path;
        path.moveTo (rect1.topLeft ());
        path.lineTo (rect1.topRight ());
        path.lineTo (rect1.bottomRight () - QPoint(4, 0));
        path.lineTo (rect1.topLeft());
        path.closeSubpath();

        p->setBrush (pal.color (QPalette::ButtonText));
        p->setRenderHint (QPainter::HighQualityAntialiasing, true);

        p->drawPath (path);
        p->restore();

#endif
        break;
    }
    case 3:
    case 4:
    case 5: {
        QString text = index.model()->data(index).toString();
        align |= Qt::AlignHCenter;
        p->setBrush(pal.windowText());
        p->drawText(rect, align, text);
        break;
    }
    case 1: {
        QString text = index.model()->data(index).toString();
        align |= Qt::AlignLeft;
        p->setBrush(pal.windowText());
        p->drawText(rect, align, text);
        break;
    }
    default:
        break;
    }

    p->restore();    
}

void ScannerResultDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (!index.isValid ())      return;
    
    switch (index.column()) {
    case 2: {
        return ;
    }
    case 0:
    case 1:
    case 3:
    case 4:
    case 5:
    default:
        break;
    }

    QStyledItemDelegate::setEditorData(editor, index);
}

QWidget *ScannerResultDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid ())      return nullptr;

    QString text = index.model()->data(index).toString();
    auto iitem = static_cast <ScannerResultItem*>(index.internalPointer ());

    switch (index.column()) {
    case 2: {
        return nullptr;
        if (auto sr = static_cast <ScannerResultWidget*>(mObj)) {
            if (!sr->hasChecked()) {
                QComboBox* cb = new QComboBox(parent);
                QStringList ls;
                ls << "例外文件" << "删除";
                if (!iitem->canUntreated ()) {
                    ls << "未处理";
                }

                cb->addItems(ls);

                if (text == "例外文件") {
                    cb->setCurrentIndex (0);
                } else if (text == "删除") {
                    cb->setCurrentIndex (1);
                } else {
                    cb->setCurrentIndex (2);
                }
                return cb;
            }
        }
        return nullptr;
    }
    case 0:
    case 1:
    case 3:
    case 4:
    case 5:
    default:
        break;
    }

    return QStyledItemDelegate::createEditor(parent, option, index); 
}

void ScannerResultDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (!index.isValid ())      return;
    
    switch (index.column()) {
//    case 2: {
//        if (editor) {
//            model->setData (index, static_cast<QComboBox*>(editor)->currentText ());
//        }
//        return ;
//    }
    case 0:
    case 1:
    case 3:
    case 4:
    case 5:
    default:
        break;
    }

    return QStyledItemDelegate::setModelData(editor, model, index); 
}
