#ifndef SCANNERTASKWIDGET_H
#define SCANNERTASKWIDGET_H

#include "view/scanner-view.h"
#include "model/scanner-task-model.h"

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class ModelItemSort;

class ScannerTaskWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScannerTaskWidget(QWidget *parent = nullptr);

    // 测试函数
    void test ();

public Q_SLOTS:
    void clearData();

Q_SIGNALS:
    void clicked ();
    void taskDetail (const ScannerTaskItem* const item);

protected:
    void resizeEvent (QResizeEvent* event) override;

private:
    void setBigSize ();
    void setDefaultSize ();

private:
    int                     mCurIndex = 0;
    QVBoxLayout*            mMainLayout;

    ScannerView*            mView;
    ScannerTaskModel*       mModel;
    ModelItemSort*          mProxyModel;
};

#endif // SCANNERTASKWIDGET_H
