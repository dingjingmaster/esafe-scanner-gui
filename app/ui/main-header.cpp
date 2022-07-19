#include "main-header.h"
#include "main-toolbar.h"

#include <QDebug>
#include <QHBoxLayout>

MainHeader::MainHeader(QWidget *parent)
    : QWidget{parent}
{
    setMaximumHeight(200);
    setContentsMargins(0, 0, 0, 0);
    setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    mMainLayout = new QHBoxLayout;
    mHeaderName = new QLabel;

    mLeftLayout = new QHBoxLayout;
    mRightLayout = new QHBoxLayout;
    mRightLayout->setSpacing(3);

    // title
    mHeaderName->setText(tr("Scan self-check tool"));
    mLeftLayout->addWidget(mHeaderName);

    // button
    mMinBtn = new HeaderButton(nullptr, HeaderButton::MIN);
    mMaxBtn = new HeaderButton(nullptr, HeaderButton::MAX);
    mCloseBtn = new HeaderButton(nullptr, HeaderButton::CLOSE);
    connect(mMinBtn,    &QPushButton::clicked, this, &MainHeader::windowMin);
    connect(mMaxBtn,    &QPushButton::clicked, this, &MainHeader::windowMax);
    connect(mCloseBtn,  &QPushButton::clicked, this, &MainHeader::windowClose);

    mRightLayout->addWidget(mMinBtn);
    mRightLayout->addWidget(mMaxBtn);
    mRightLayout->addWidget(mCloseBtn);

    // main layout
    mMainLayout->addItem(mLeftLayout);
    mMainLayout->addStretch();
    mMainLayout->addItem(mRightLayout);
    setLayout(mMainLayout);
}

void MainHeader::mouseMoveEvent(QMouseEvent* ev)
{
    if (mDrag) {
        QPoint dist = ev->globalPos() - mMouseStartPoint;
        QWidget* parentt = qobject_cast<QWidget*>(parent());
        if (parentt) {
            parentt->move(mWindowTopLeftPoint + dist);
        }
    }
}

void MainHeader::mousePressEvent(QMouseEvent* ev)
{
    if (ev->button() == Qt::LeftButton) {
        mDrag = true;
        QWidget* parentt = qobject_cast<QWidget*>(parent());
        if (parentt) {
            mMouseStartPoint = ev->globalPos();
            mWindowTopLeftPoint = parentt->frameGeometry().topLeft();
        }
    }
}

void MainHeader::mouseReleaseEvent(QMouseEvent* ev)
{
    if (ev->button() == Qt::LeftButton) {
        mDrag = false;
    }
}
