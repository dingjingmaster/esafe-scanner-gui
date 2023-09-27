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
    enum Direction {UP = 0, DOWN, LEFT, RIGHT, LEFT_TOP, LEFT_BOTTOM, RIGHT_BOTTOM, RIGHT_TOP, NONE};
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void leaveEvent (QEvent*) override;
    void resizeEvent (QResizeEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

private Q_SLOTS:
    void onShowStatusString (const QString&);
    void onLoadTaskResult (const ScannerTaskItem* const item);

    void onShowTaskWidget();
    void onShowTaskResultWidget();

Q_SIGNALS:
    void activePrimaryWindow();

private:
    void region (const QPoint& cursorGlobalPoint);

private:
    const int               mMinWidth = 1100;
    const int               mMinHeight = 700;

    QLayout*                mMainLayout;

    MainSubToolBar*         mMainSubtoolbar{};
//    QLabel*                 mStatusLabel;

    ScannerTaskWidget*      mScannerTaskWidget;
    ScannerResultWidget*    mScannerResultWidget;

    PushButton*             mScanBtn;

    QLabel*                 mCurStatus;
    QTimer*                 mStatusTimer;

    bool                    mDrag{};
    QPoint                  mOffset;
    Direction               mDirection;
    bool                    mIsPress = false;
    QPoint                  mDragPos;
    QPoint                  mMouseStartPoint;
    QPoint                  mWindowTopLeftPoint;
};

#endif // MAINWINDOW_H
