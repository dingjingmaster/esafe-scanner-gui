#ifndef SCANNERRESULTWIDGET_H
#define SCANNERRESULTWIDGET_H

#include "view/scanner-view.h"
#include <QWidget>

class Progress;
class HeaderView;
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

    void setScanDir(const QString& name);
    void setScanDir(QStringList name);
    void setTaskName (QString name);
    void setScanFilter (QString name);

    void clearData ();
    bool hasChecked ();                 // 是否有选中条目
    void loadTaskResult (QString taskName, QString taskFilter, QStringList mScanDir);

protected:
    void resizeEvent (QResizeEvent* event) override;

private:
    void setBigSize ();
    void setDefaultSize ();
    
Q_SIGNALS:
    void stopApplyData();
    void startApplyData();

    void saveData();
    void applyData();
    void updateView();
    void checkedItem(bool);
    void returnTaskList ();
    void statusString (QString);        //"任务名称: (%1), 总条数: (%2), 未处理: (%3), 误报: (%5), 删除: (%5)");

public Q_SLOTS:
    void updateStatus();
    void onBackToTaskView ();


private:
    int                     mCurIndex = 0;
    QStringList             mScanDir;
    QString                 mTaskName;
    QString                 mScanFilter;

    HeaderView*             mHeaderView;

    QVBoxLayout*            mMainLayout;

    QHBoxLayout*            mBtnLayout;
    QHBoxLayout*            mLeftLayout;
    QHBoxLayout*            mRightLayout;
    
    PushButton*             mMisBtn;
    PushButton*             mDelBtn;
    PushButton*             mExpBtn;

    ScannerView*            mView;
    ScannerResultModel*     mModel;

    bool                    mSavingData = false;
    QThread*                mThreadSaveData;

    // 进度条
    Progress*               mProgress;
};

#endif // SCANNERRESULTWIDGET_H
