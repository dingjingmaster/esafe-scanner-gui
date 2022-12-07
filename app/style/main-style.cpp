#include "main-style.h"

#include <QRect>
#include <QPainter>
#include <qdrawutil.h>
#include <QPainterPath>
#include <QStyleOptionButton>

static QWindow *qt_getWindow(const QWidget *widget)
{
    return widget ? widget->window()->windowHandle() : nullptr;
}

MainStyle::MainStyle(const QString& key)
    : QProxyStyle{key}
{

}

int MainStyle::pixelMetric(PixelMetric m, const QStyleOption *opt, const QWidget *widget) const
{
    int ret;

    switch (m) {
    default:
        ret = QProxyStyle::pixelMetric(m, opt, widget);
        break;
    }

    return ret;
}

QRect MainStyle::subElementRect(SubElement sr, const QStyleOption *opt, const QWidget *widget) const
{
    QRect r;

    switch (sr) {
    case SE_CheckBoxIndicator: {
        int h = proxy()->pixelMetric(PM_IndicatorHeight, opt, widget);
        r.setRect(opt->rect.x(), opt->rect.y() + ((opt->rect.height() - h) / 2),
                  proxy()->pixelMetric(PM_IndicatorWidth, opt, widget), h);
        r = visualRect(opt->direction, opt->rect, r);
        break;
    }
    case SE_CheckBoxContents: {
        int radioWidth = proxy()->pixelMetric(PM_IndicatorWidth, opt, widget);
        int spacing = proxy()->pixelMetric(PM_RadioButtonLabelSpacing, opt, widget);
        return visualRect(opt->direction, opt->rect, opt->rect.adjusted(radioWidth + spacing, 0, 0, 0));
    }

    case SE_CheckBoxFocusRect: {
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            if (btn->icon.isNull() && btn->text.isEmpty()) {
                r = subElementRect(SE_CheckBoxIndicator, opt, widget);
                r.adjust(1, 1, -1, -1);
                break;
            }
            // As above, deal with the logical first, then convert it back to screen coords.
            QRect cr = visualRect(btn->direction, btn->rect,
                                  subElementRect(SE_CheckBoxContents, btn, widget));

            QRect iconRect, textRect;
            if (!btn->text.isEmpty()) {
                textRect = itemTextRect(opt->fontMetrics, cr, Qt::AlignAbsolute | Qt::AlignLeft
                                        | Qt::AlignVCenter | Qt::TextShowMnemonic,
                                        btn->state & State_Enabled, btn->text);
            }
            if (!btn->icon.isNull()) {
                iconRect = itemPixmapRect(cr, Qt::AlignAbsolute | Qt::AlignLeft | Qt::AlignVCenter
                                        | Qt::TextShowMnemonic,
                                   btn->icon.pixmap(qt_getWindow(widget), btn->iconSize, QIcon::Normal));
                if (!textRect.isEmpty())
                    textRect.translate(iconRect.right() + 4, 0);
            }
            r = iconRect | textRect;
            r.adjust(-3, -2, 3, 2);
            r = r.intersected(btn->rect);
            r = visualRect(btn->direction, btn->rect, r);
        }
        break;
    }
    case SE_CheckBoxClickRect: {
        r = subElementRect(SE_CheckBoxFocusRect, opt, widget);
        r |= subElementRect(SE_CheckBoxIndicator, opt, widget);
        break;
    }
    default:
        QProxyStyle::subElementRect(sr, opt, widget);
    }
}

void MainStyle::drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    QPainter *painter = p;
    QStyleOption option = *opt;

    switch (pe) {
    case PE_IndicatorItemViewItemCheck: {
        //option.state |= QStyle::State_Selected;
        proxy()->drawPrimitive(PE_IndicatorCheckBox, &option, p, widget);
        break;
    }
    case PE_IndicatorCheckBox: {
        if (const QStyleOptionButton *checkbox = qstyleoption_cast<const QStyleOptionButton*>(opt)) {
            const bool useDarkPalette = false;
            bool enable = checkbox->state & State_Enabled;
            bool mouseOver = false; //checkbox->state & State_MouseOver;
            bool sunKen = checkbox->state & State_Sunken;
            bool on = checkbox->state & State_On;
            bool noChange = checkbox->state & State_NoChange;

            QRectF rect = checkbox->rect;
            int width = rect.width();
            int heigth = rect.height();
            int x_Radius = 4;
            int y_Radius = 4;

            QPainterPath path;
            if (on) {
                path.moveTo(width/4 + checkbox->rect.left(), heigth/2 + checkbox->rect.top());
                path.lineTo(width*0.45 + checkbox->rect.left(), heigth*3/4 + checkbox->rect.top());
                path.lineTo(width*3/4 + checkbox->rect.left(), heigth/4 + checkbox->rect.top());
            } else if (noChange){
                path.moveTo(rect.left() + width/4, rect.center().y());
                path.lineTo(rect.right() - width/4 , rect.center().y());
            }

            p->save();
            painter->setClipRect(rect);
            painter->setRenderHint(QPainter::Antialiasing, true);
            if (enable) {
                if (on | noChange) {
                    painter->setPen(QColor(255, 0, 0));
                    painter->setBrush(QColor(255, 138, 140));
                    painter->drawRoundedRect(rect, x_Radius, y_Radius);

                    painter->setPen(QPen(checkbox->palette.brush(QPalette::Active, QPalette::HighlightedText), 2,
                                         Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    //painter->setBrush(Qt::NoBrush);
                    painter->drawPath(path);
                } else {
                    QColor red = qRgb(255, 138, 140);
                    painter->setPen(red);
                    painter->setBrush(checkbox->palette.color(QPalette::Active, QPalette::Window));
                    painter->drawRoundedRect(rect, x_Radius, y_Radius);
                }
            } else {
                painter->setPen(QColor(224, 224, 224));
                painter->setBrush(QColor(233, 233, 233));
                painter->drawRoundedRect(rect, x_Radius, y_Radius);
                if (on | noChange) {
                    painter->setPen(QPen(checkbox->palette.brush(QPalette::Disabled, QPalette::ButtonText), 2,
                                         Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter->setBrush(Qt::NoBrush);
                    painter->drawPath(path);
                }
            }
            painter->restore();
            return;
        }
        break;
    }
    default:
        QProxyStyle::drawPrimitive(pe, opt, p, widget);
        break;
    }
}

void MainStyle::drawControl(ControlElement element, const QStyleOption *opt, QPainter *p, const QWidget* widget) const
{
    QPainter* painter = p;
    const QStyleOption* option = opt;
    switch (element) {
    case QStyle::CE_CheckBox: {
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            QStyleOptionButton subopt = *button;
            subopt.rect = proxy()->subElementRect(SE_CheckBoxIndicator, option, widget);
            proxy()->drawPrimitive(PE_IndicatorCheckBox, &subopt, painter, widget);
            subopt.rect = proxy()->subElementRect(SE_CheckBoxContents, option, widget);
            proxy()->drawControl(CE_CheckBoxLabel, &subopt, painter, widget);
            return;
        }
        break;
    }
    case QStyle::CE_CheckBoxLabel: {
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            uint alignment = visualAlignment(btn->direction, Qt::AlignLeft | Qt::AlignVCenter);

            if (!proxy()->styleHint(SH_UnderlineShortcut, btn, widget))
                alignment |= Qt::TextHideMnemonic;
            QPixmap pix;
            QRect textRect = btn->rect;
            if (!btn->icon.isNull()) {
                pix = btn->icon.pixmap(qt_getWindow(widget), btn->iconSize, btn->state & State_Enabled ? QIcon::Normal : QIcon::Disabled);
                proxy()->drawItemPixmap(p, btn->rect, alignment, pix);
                if (btn->direction == Qt::RightToLeft)
                    textRect.setRight(textRect.right() - btn->iconSize.width() - 4);
                else
                    textRect.setLeft(textRect.left() + btn->iconSize.width() + 4);
            }
            if (!btn->text.isEmpty()){
                proxy()->drawItemText(p, textRect, alignment | Qt::TextShowMnemonic,
                    btn->palette, btn->state & State_Enabled, btn->text, QPalette::WindowText);
            }
        }
        break;
    }
    case QStyle::CE_ItemViewItem: {
    }
    default:
        QProxyStyle::drawControl(element, opt, p, widget);
        break;
    }
}

QSize MainStyle::sizeFromContents(ContentsType ct, const QStyleOption *opt, const QSize &csz, const QWidget *widget) const
{
    QSize sz(csz);
    switch (ct) {
    case CT_RadioButton:
    case CT_CheckBox: {
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            bool isRadio = (ct == CT_RadioButton);

            int w = proxy()->pixelMetric(isRadio ? PM_ExclusiveIndicatorWidth
                                        : PM_IndicatorWidth, btn, widget);
            int h = proxy()->pixelMetric(isRadio ? PM_ExclusiveIndicatorHeight
                                        : PM_IndicatorHeight, btn, widget);

            int margins = 0;
            // we add 4 pixels for label margins
            if (!btn->icon.isNull() || !btn->text.isEmpty())
                margins = 4 + proxy()->pixelMetric(isRadio ? PM_RadioButtonLabelSpacing
                                                  : PM_CheckBoxLabelSpacing, opt, widget);
            sz += QSize(w + margins, 4);
            sz.setHeight(qMax(sz.height(), h));
        }
        break;
    }
    default:
        sz = QProxyStyle::sizeFromContents(ct, opt, csz, widget);
        break;
    }

    return sz;
}

void MainStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    switch (control) {
    case CC_ComboBox: {
        break;
        if (const QStyleOptionComboBox* cbOption = qstyleoption_cast <const QStyleOptionComboBox*> (option)) {
            painter->save();
            QRect rect = subControlRect (CC_ComboBox, option, SC_ComboBoxFrame, widget).adjusted (+1, +1, -1, -1);
            painter->setBrush(QColor("#128bf1"));
            painter->drawRect(rect);            
                    
            rect = subControlRect (CC_ComboBox, option, SC_ComboBoxEditField, widget).adjusted (+2, +2, -2, -2);
            QLinearGradient gradient (rect.topLeft(), rect.bottomRight());
            gradient.setColorAt(0.0, QColor("#fa709a"));
            gradient.setColorAt(1.0, QColor("#fee140"));
            painter->setPen(Qt::NoPen);
            painter->setBrush(gradient);
            painter->drawRect(rect);
                    
            rect = subControlRect (CC_ComboBox, option, SC_ComboBoxArrow, widget).adjusted(+1, +1, -1, -1);
            painter->setPen(Qt::transparent);
            painter->translate(option->rect.x(), option->rect.y());
                    
            QLinearGradient grandient2 (rect.topLeft(), rect.bottomRight());
            grandient2.setColorAt (0.0, QColor("#84fab0"));
            grandient2.setColorAt (1.0, QColor("#8fd3f4"));
                    
            painter->setBrush (grandient2);
            painter->drawRect(rect);
            painter->restore();
                    
            QStyleOption arrowOpt (*cbOption);
            arrowOpt.rect = rect.adjusted (+rect.width() * 0.3, +rect.height() * 0.3, -rect.width() * 0.3, -rect.height() * 0.3);
            //drawPrimitive (PE_IndicatorArrowDown, &arrowOpt, painter);
            
            if (option->activeSubControls == SC_ComboBoxEditField || option->activeSubControls == SC_ComboBoxArrow) {
                painter->save();
                QColor s(0, 0, 0, 63);
                painter->setBrush (s);
                painter->drawRect(widget->rect());
                painter->restore();
            }
        }
        return;
    }
    default:
        break;
    }
    
    QProxyStyle::drawComplexControl (control, option, painter, widget);
}
