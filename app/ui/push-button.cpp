#include "push-button.h"

#include <QLabel>
#include <QVBoxLayout>

PushButton::PushButton(QWidget *parent, Type type)
    : QWidget{parent}, mType(type)
{
    setMouseTracking(true);
    setFixedWidth(mMaxWidth);
    setContentsMargins(0, 0, 0, 0);

    mLayout = new QVBoxLayout;
    mLayout->setSpacing(0);
    mLayout->setContentsMargins(0, 0, 0, 0);

    mLabel = new QLabel;
    mLabel->setMargin(6);
    mLabel->setFixedWidth(mMaxWidth);
    mLabel->setStyleSheet("font: blod;");
    mLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    mLayout->addWidget(mLabel);

    switch (type) {
    case Type1: {
        QWidget* widget = new QWidget;
        widget->setFixedHeight(3);
        mLabel->setStyleSheet("color: blue;");
        widget->setStyleSheet("background-color:blue;");
        mLayout->addWidget(widget);
        break;
    }
    case Type2: {
        mLabel->setStyleSheet("font: blod; color: rgb(255, 255, 255)");
        break;
    }
    }

    if (Type1 == type) {

    }

    setLayout(mLayout);
}

void PushButton::setText(QString text)
{
    if (nullptr == text)        return;

    mLabel->setText(text);
}

void PushButton::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)

    Q_EMIT clicked ();
}

void PushButton::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)

    Q_EMIT doubleClicked();
}
