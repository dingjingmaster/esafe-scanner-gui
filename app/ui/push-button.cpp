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
    mLabel->setText(tr("扫描结果"));
    mLabel->setFixedWidth(mMaxWidth);
    mLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    mLayout->addWidget(mLabel);

    if (Type1 == type) {
        QWidget* widget = new QWidget;
        widget->setFixedHeight(3);
        widget->setStyleSheet("background-color:blue;");
        mLayout->addWidget(widget);
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
