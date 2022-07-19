#include "toolbar-action.h"

#include <QFile>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>

ToolbarAction::ToolbarAction(QWidget *parent)
    : QWidget (parent)
{
    setFixedSize(mMaxWidth, mMaxHeight);

    mMainLayout = new QVBoxLayout;
    mMainLayout->setSpacing(10);
    mMainLayout->setContentsMargins(0, 0, 0, 0);
    mMainLayout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    mIcon = new QLabel;
    mIcon->setFixedSize(mIconLabelSize, mIconLabelSize);
    mIcon->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    mText = new QLabel;
    mText->setFixedSize(mMaxWidth, mTextsize);
    mText->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    mMainLayout->addWidget(mIcon);
    mMainLayout->addWidget(mText);

    setLayout(mMainLayout);
}

void ToolbarAction::setIcon(QString pic)
{
    if (!pic.isEmpty() && QFile::exists(pic)) {
        mIcon->setPixmap(QPixmap(pic).scaled(mIconWidth, mIconHeight));
    }
}

void ToolbarAction::setText(QString text)
{
    if (!text.isEmpty()) {
        mText->setText(text);
    }
}
