#include "header-button.h"

#include <QFile>
#include <QDebug>
#include <QPixmap>
#include <QPainter>

static QPixmap transColor (const QPixmap& icon);

HeaderButton::HeaderButton(QWidget *parent, Type type)
    : QPushButton{parent}
{
    setFlat(true);
    setMouseTracking(true);
    setFocusPolicy(Qt::NoFocus);
    
    setFixedSize(42, 42);
    setContentsMargins(3, 3, 3, 3);

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
    
    QString path = nullptr;

    switch (type) {
    case MIN: {
        //path = ":/data/min.png";
        path = "window-minimize-symbolic";
        break;
    }
    case MAX: {
        //path = ":/data/max.png";
        path = "window-maximize-symbolic";
        break;
    }
    case CLOSE: {
        //path = ":/data/close.png";
        path = "window-close-symbolic";
        break;
    }
    case RESTORE: {
        //path = ":/data/restore.png";
        path = "window-restore-symbolic";
        break;
    }
    default:
        break;
    }
    
    //if (!QFile::exists(path))       return;
    
    //QPixmap pm(width(), height());
    //pm.load (path);
    
    //setIcon(QIcon(transColor(pm)));
    setIcon(QIcon(transColor(QIcon::fromTheme(path).pixmap (width(), height()))));
    update();
}


static QPixmap transColor (const QPixmap& icon)
{
    //return icon;
    if (icon.isNull())  return QPixmap();

    QImage oldp = icon.toImage();

    QImage newp(icon.width(), icon.height(), QImage::Format_ARGB32);

    for (auto i = 0; i < oldp.width(); ++i) {
        for (auto j = 0; j < oldp.height(); ++j) {
            auto c = QColor(oldp.pixel(i, j));
            if (c.red () != 0 || c.green () != 0 || c.blue () != 0)
            qDebug() << "r: " << c.red() << " g: " << c.green () << " b: " << c.blue ();
                        
            if ((c.alpha() > 0) && (c.red() > 0) && (c.green() > 0) && (c.blue() > 0)) {
                newp.setPixel(i, j, qRgb(255, 255, 255));
            } else {
                newp.setPixel(i, j, qRgba(0, 0, 0, 0));
            }
        }
    }

    return QPixmap::fromImage(newp);
}
