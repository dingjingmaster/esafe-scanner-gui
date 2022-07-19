#include "scanner-result-widget.h"

#include <QPushButton>
#include <QVBoxLayout>

ScannerResultWidget::ScannerResultWidget(QWidget *parent)
    : QWidget{parent}
{
    mMainLayout = new QVBoxLayout;

    mLeftLayout = new QHBoxLayout;
    QPushButton* retBtn = new QPushButton(this);
    mLeftLayout->addWidget(retBtn);

    mRightLayout = new QHBoxLayout;
    QPushButton* delBtn = new QPushButton(this);
    mRightLayout->addWidget(delBtn);

    QPushButton* misBtn = new QPushButton(this);
    mRightLayout->addWidget(misBtn);

    QPushButton* expBtn = new QPushButton(this);
    mRightLayout->addWidget(expBtn);

    mMainLayout->addItem(mLeftLayout);
    mMainLayout->addItem(mRightLayout);

    // tabview

    setLayout(mMainLayout);
}
