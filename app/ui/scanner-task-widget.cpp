#include "scanner-task-widget.h"

#include <QVBoxLayout>

ScannerTaskWidget::ScannerTaskWidget(QWidget *parent)
    : QWidget{parent}
{
    mMainLayout = new QVBoxLayout;

    // tabview


    setLayout(mMainLayout);
}
