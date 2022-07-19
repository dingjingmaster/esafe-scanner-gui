#ifndef SCANNERRESULTWIDGET_H
#define SCANNERRESULTWIDGET_H

#include "view/scanner-view.h"
#include <QWidget>

class QHBoxLayout;
class QVBoxLayout;
class ScannerResultWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScannerResultWidget(QWidget *parent = nullptr);

private:
    QVBoxLayout*            mMainLayout;

    QHBoxLayout*            mLeftLayout;
    QHBoxLayout*            mRightLayout;

    ScannerView*            mView;

};

#endif // SCANNERRESULTWIDGET_H
