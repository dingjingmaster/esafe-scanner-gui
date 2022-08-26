#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLayout>
#include <QLabel>

class QResizeEvent;
class ScannerTaskItem;
class ScannerTaskWidget;
class ScannerResultWidget;

class Progress;
class PushButton;
class MainSubToolBar;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void resizeEvent (QResizeEvent*) override;

private Q_SLOTS:
    void onShowStatusString (const QString&);
    void onLoadTaskResult (const ScannerTaskItem* const item);

Q_SIGNALS:

private:
    const int               mMinWidth = 900;
    const int               mMinHeight = 600;

    QLayout*                mMainLayout;

    MainSubToolBar*         mMainSubtoolbar;
    QLabel*                 mStatusLabel;

    ScannerTaskWidget*      mScannerTaskWidget;
    ScannerResultWidget*    mScannerResultWidget;

    PushButton*             mScanBtn;

    QLabel*                 mCurStatus;
    QTimer*                 mStatusTimer;
};

#endif // MAINWINDOW_H
