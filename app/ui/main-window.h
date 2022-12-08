#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QWidget>
#include <QLayout>
#include <QMainWindow>

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
    void mouseMoveEvent(QMouseEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

private Q_SLOTS:
    void onShowStatusString (const QString&);
    void onLoadTaskResult (const ScannerTaskItem* const item);

Q_SIGNALS:

private:
    const int               mMinWidth = 1100;
    const int               mMinHeight = 700;

    QLayout*                mMainLayout;

    MainSubToolBar*         mMainSubtoolbar;
    QLabel*                 mStatusLabel;

    ScannerTaskWidget*      mScannerTaskWidget;
    ScannerResultWidget*    mScannerResultWidget;

    PushButton*             mScanBtn;

    QLabel*                 mCurStatus;
    QTimer*                 mStatusTimer;

    bool                    mDrag;
    QPoint                  mOffset;
    QPoint                  mMouseStartPoint;
    QPoint                  mWindowTopLeftPoint;
};

#endif // MAINWINDOW_H
