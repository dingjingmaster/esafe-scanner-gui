#include "scanner-result-delegate.h"

#include "scanner-result-widget.h"

#include <QDebug>
#include <QPainter>
#include <QComboBox>
#include <QApplication>

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
    case 2: 
#if 1
    {
        if (auto sr = static_cast <ScannerResultWidget*>(mObj)) {
            if (sr->hasChecked()) {
                QStyledItemDelegate::paint (p, option, index);   
                sr->update ();
                break;
            }
        }
            
        QStyleOptionComboBox cbOp;
        cbOp.initFrom(static_cast<QWidget*>(parent()));
        cbOp.state |= QStyle::State_Enabled;
        cbOp.rect = rect;
        
        cbOp.currentText = "hallo";
        cbOp.editable = true;
        cbOp.frame = false;

        QBrush bth;
        bth.setColor(Qt::darkRed);
        if (index.model()->data(index).toBool()) {
            cbOp.state |= QStyle::State_On;
        } else {
            cbOp.state |= QStyle::State_Off;
        }

        p->setBrush(bth);
        QApplication::style()->drawComplexControl (QStyle::CC_ComboBox, &cbOp, p);
        break;
        
    } 
#endif
    case 1:
    case 3:
    case 4:
    case 5: {
        QString text = index.model()->data(index).toString();
        align |= Qt::AlignHCenter;
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
    case 2: 
    {
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
    
    switch (index.column()) {
    case 2: 
    {
        if (auto sr = static_cast <ScannerResultWidget*>(mObj)) {
            if (!sr->hasChecked()) {
                QComboBox* cb = new QComboBox(parent);
                cb->addItems(QStringList() << tr("误报") << tr("删除") << tr("未处理"));
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
    case 2: 
    {
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

    return QStyledItemDelegate::setModelData(editor, model, index); 
}
