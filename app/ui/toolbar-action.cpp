#include "toolbar-action.h"

#include <QFile>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>

ToolbarAction::ToolbarAction(QWidget *parent)
    : QWidget (parent)
{
    setContentsMargins(10, 0, 0, 0);
    setFixedSize(mFixSize + 30, mFixSize);
    setObjectName("main-toolbar-action");

    mMainLayout = new QVBoxLayout;
    mMainLayout->setSpacing(1);
    mMainLayout->setContentsMargins(0, 0, 0, 0);
    mMainLayout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    mIcon = new QLabel;
    mIcon->setFixedSize(mFixSize + 30, mIconSize);
    mIcon->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    mIcon->setObjectName("main-toolbar-action-icon");

    mText = new QLabel;
    mText->setFixedSize(mFixSize + 30, mLabelHeight);
    mText->setObjectName("main-toolbar-action-label");
    mText->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    mMainLayout->addWidget(mIcon);
    mMainLayout->addWidget(mText);

    setLayout(mMainLayout);
}

void ToolbarAction::setIcon(QString pic)
{
    if (!pic.isEmpty() && QFile::exists(pic)) {
        mIcon->setPixmap(QPixmap(pic).scaled(mIconSize + 30, mIconSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
}

void ToolbarAction::setText(QString text)
{
    if (!text.isEmpty()) {
        mText->setText(text);
    }
}

void ToolbarAction::enterEvent(QEvent *event)
{
    setCursor(Qt::PointingHandCursor);
}

void ToolbarAction::leaveEvent(QEvent *event)
{
    setCursor(Qt::ArrowCursor);
}
