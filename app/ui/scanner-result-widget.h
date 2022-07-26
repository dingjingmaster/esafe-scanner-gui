#ifndef SCANNERRESULTWIDGET_H
#define SCANNERRESULTWIDGET_H

#include "view/scanner-view.h"
#include <QWidget>

class QHBoxLayout;
class QVBoxLayout;
class ScannerResultModel;

class ScannerResultWidget : public QWidget
{
    Q_OBJECT
    enum Operation { OP_DELETE = 1, OP_MISINFO };
public:
    explicit ScannerResultWidget(QWidget *parent = nullptr);
    ~ScannerResultWidget();

    // 测试函数
    void test ();

    void clearData ();
    void loadTaskResult (QString taskName, QString taskFilter);

Q_SIGNALS:
    void returnTaskList ();

private:
    QVBoxLayout*            mMainLayout;

    QHBoxLayout*            mBtnLayout;
    QHBoxLayout*            mLeftLayout;
    QHBoxLayout*            mRightLayout;

    ScannerView*            mView;
    ScannerResultModel*     mModel;
};

#endif // SCANNERRESULTWIDGET_H
