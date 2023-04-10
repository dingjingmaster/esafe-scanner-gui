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

    QRect rect = option.rect;
    Qt::Alignment align = option.displayAlignment;
    QPalette pal;

    // 绘制背景
    auto item = static_cast<ScannerResultItem*>(index.internalPointer());
    if (!item) { return;}

    p->save();
    if (item->getChecked()) {
        p->save();
        p->setPen (Qt::NoPen);
        p->setBrush (QColor(255, 138, 140));
        p->drawRect (rect);
        p->restore();
    }

    switch (index.column()) {
    case 0: {
        QRect rectCB(rect.left() + (rect.width() - 20) / 2, rect.top() + (rect.height() - 20) / 2, 20, 20);
        QStyleOptionButton cbOp;
        cbOp.initFrom(dynamic_cast<QWidget*>(parent()));
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
        auto m = index.model();
        if (m) {
            auto d = m->data (index);
            if (d.isValid()) {
                QString text = d.toString();
                if (nullptr == text || text.isNull() || text.isEmpty()) text = "";
                p->setBrush(pal.windowText());
                p->drawText(rect, Qt::AlignCenter, text);
            }
        }
        break;
    }
    case 3:
    case 4:
    case 5: {
        auto m = index.model();
        if (m) {
            auto d = m->data (index);
            if (d.isValid()) {
                QString text = d.toString();
                if (nullptr == text || text.isNull() || text.isEmpty()) text = "";
                align |= Qt::AlignHCenter;
                p->setBrush(pal.windowText());
                p->drawText(rect, (int) align, text);
            }
        }
        break;
    }
    case 1: {
        auto m = index.model();
        if (m) {
            auto d = m->data (index);
            if (d.isValid()) {
                QString text = d.toString();
                if (nullptr == text || text.isNull() || text.isEmpty()) text = "";
                align |= Qt::AlignLeft;
                p->setBrush(pal.windowText());
                QFont f = option.font;
                QFontMetrics fm(f);
                QString text1 = fm.elidedText (text, Qt::ElideRight, rect.width() * 2 - 10);
                p->setFont (f);
                p->drawText(rect, (int) (align | Qt::TextWordWrap | Qt::ElideRight), text1);
            }
        }
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
    auto item = (static_cast<ScannerResultItem*>(index.internalPointer()));

    switch (index.column()) {
    case 2: {
        return nullptr;
        if (auto sr = static_cast <ScannerResultWidget*>(mObj)) {
            if (!sr->hasChecked()) {
                QComboBox* cb = new QComboBox(parent);
                QStringList ls;
                ls << "例外文件" << "删除";
                if (!item->canUntreated ()) {
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
