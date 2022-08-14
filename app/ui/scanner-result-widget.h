#ifndef SCANNERRESULTWIDGET_H
#define SCANNERRESULTWIDGET_H

#include "view/scanner-view.h"
#include <QWidget>

class PushButton;
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

    void setTaskName (QString name);

    void clearData ();
    bool hasChecked ();                 // 是否有选中条目
    void loadTaskResult (QString taskName, QString taskFilter, QStringList mScanDir);
    
Q_SIGNALS:
    void applyData();
    void updateView();
    void checkedItem(bool);
    void returnTaskList ();
    void statusString (QString);        //"任务名称: (%1), 总条数: (%2), 未处理: (%3), 误报: (%5), 删除: (%5)");

public Q_SLOTS:
    void updateStatus();

private:
    QStringList             mScanDir;
    QString                 mTaskName;

    QVBoxLayout*            mMainLayout;

    QHBoxLayout*            mBtnLayout;
    QHBoxLayout*            mLeftLayout;
    QHBoxLayout*            mRightLayout;
    
    PushButton*             mMisBtn;
    PushButton*             mDelBtn;
    PushButton*             mExpBtn;

    ScannerView*            mView;
    ScannerResultModel*     mModel;
};

#endif // SCANNERRESULTWIDGET_H
