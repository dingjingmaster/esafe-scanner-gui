#include "main-header.h"

//#include "xatom-helper.h"

#include <QDebug>
#include <QHBoxLayout>

MainHeader::MainHeader(QWidget *parent)
    : QWidget{parent}
{
    setMouseTracking(true);
    setObjectName("main-header");
    setMaximumHeight(mMaxHeight);
    setContentsMargins(0, 0, 0, 0);
    setAttribute(Qt::WidgetAttribute::WA_StyledBackground);

    mMainLayout = new QHBoxLayout;
    mMainLayout->setContentsMargins(0, 0, 0, 0);

    mHeaderName = new QLabel;
    mHeaderName->setObjectName("main-header-title");

    mLeftLayout = new QHBoxLayout;
    mRightLayout = new QHBoxLayout;
    mRightLayout->setSpacing(3);

    // title
    mHeaderName->setText(tr("终端数据防泄漏系统"));
    mLeftLayout->addWidget(mHeaderName);

    // button
    mMinBtn = new HeaderButton(nullptr, HeaderButton::MIN);
    mMaxBtn = new HeaderButton(nullptr, HeaderButton::MAX);
    mCloseBtn = new HeaderButton(nullptr, HeaderButton::CLOSE);
    mMinBtn->setObjectName("main-header-button");
    mMaxBtn->setObjectName("main-header-button");
    mCloseBtn->setObjectName("main-header-button-close");
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

void MainHeader::onApplyData()
{
    mCloseBtn->setEnabled(false);
    mMaxBtn->setEnabled(false);
    mMinBtn->setEnabled(false);
}

void MainHeader::onApplyDataFinished()
{
    mCloseBtn->setEnabled(true);
    mMaxBtn->setEnabled(true);
    mMinBtn->setEnabled(true);
}

//void MainHeader::mouseMoveEvent(QMouseEvent* ev)
//{
//    if (mDrag) {
//        QPoint dist = ev->globalPos() - mMouseStartPoint;
//        qDebug() << ev->globalPos() << " -- " << dist << (mWindowTopLeftPoint + dist);
//        QWidget* parentt = qobject_cast<QWidget*>(parent());
//        if (parentt) {
//            parentt->move(mWindowTopLeftPoint + dist);
//        }
//    }
//}
//
//void MainHeader::mousePressEvent(QMouseEvent* ev)
//{
//    if (ev->button() == Qt::LeftButton) {
//        mDrag = true;
//        QWidget* parentt = qobject_cast<QWidget*>(parent());
//        if (parentt) {
//            mMouseStartPoint = ev->globalPos();
//            mWindowTopLeftPoint = parentt->frameGeometry().topLeft();
//        }
//    }
//}
//
//void MainHeader::mouseReleaseEvent(QMouseEvent* ev)
//{
//    if (ev->button() == Qt::LeftButton) {
//        mDrag = false;
//    }
//}

void MainHeader::mouseDoubleClickEvent(QMouseEvent *)
{
    qDebug() << "double clicked!";

    Q_EMIT windowMax();
}
