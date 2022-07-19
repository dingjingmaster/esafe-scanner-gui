#include "header-button.h"

#include <QDebug>

HeaderButton::HeaderButton(QWidget *parent, Type type)
    : QPushButton{parent}
{
    setFixedSize(16, 16);

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

void HeaderButton::onTypeChanged(Type type)
{
    qDebug() << "button type changed!!!";

    switch (type) {
    case MIN:
        setIcon(QIcon::fromTheme("window-minimize-symbolic"));
        break;
    case MAX:
        setIcon(QIcon::fromTheme("window-maximize-symbolic"));
        break;
    case CLOSE:
        setIcon(QIcon::fromTheme("window-close-symbolic"));
        break;
    case RESTORE:
        setIcon(QIcon::fromTheme("window-restore-symbolic"));
        break;
    default:
        break;
    }
}
