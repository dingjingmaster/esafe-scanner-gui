#include "header-button.h"

#include <QDebug>

static QPixmap transColor (const QPixmap& icon);

HeaderButton::HeaderButton(QWidget *parent, Type type)
    : QPushButton{parent}
{
    setFlat(true);
    setMouseTracking(true);
    setFocusPolicy(Qt::NoFocus);

    onTypeChanged(type);

    connect (this, &HeaderButton::typeChanged, this, &HeaderButton::onTypeChanged);
}

HeaderButton::Type HeaderButton::getType()
{
    return mType;
}

void HeaderButton::setType(Type type)
{
    if (mType != type) {
        Q_EMIT typeChanged(type);
        mType = type;
    }
}

void HeaderButton::enterEvent(QEvent *event)
{
    setCursor(Qt::PointingHandCursor);
}

void HeaderButton::leaveEvent(QEvent *event)
{
    setCursor(Qt::ArrowCursor);
}

void HeaderButton::onTypeChanged(Type type)
{
    qDebug() << "button type changed!!!";

    switch (type) {
    case MIN:
        setIcon(QIcon(transColor(QIcon::fromTheme("window-minimize-symbolic").pixmap(width(), height()))));
        break;
    case MAX:
        setIcon(QIcon(transColor(QIcon::fromTheme("window-maximize-symbolic").pixmap(width(), height()))));
        break;
    case CLOSE:
        setIcon(QIcon(transColor(QIcon::fromTheme("window-close-symbolic").pixmap(width(), height()))));
        break;
    case RESTORE:
        setIcon(QIcon(transColor(QIcon::fromTheme("window-restore-symbolic").pixmap(width(), height()))));
        break;
    default:
        break;
    }
}


static QPixmap transColor (const QPixmap& icon)
{
    if (icon.isNull())  return QPixmap();

    QImage oldp = icon.toImage();

    QImage newp(icon.width(), icon.height(), QImage::Format_ARGB32);

    for (auto i = 0; i < oldp.width(); ++i) {
        for (auto j = 0; j < oldp.height(); ++j) {
            QColor c = QColor(oldp.pixel(i, j));
            if ((c.red() != 0) || (c.green() != 0) || (c.blue() != 0)) {
                newp.setPixel(i, j, qRgb(255, 255, 255));
            }
        }
    }

    return QPixmap::fromImage(newp);
}
