#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLayout>
#include <QLabel>

class QResizeEvent;
class ScannerTaskWidget;
class ScannerResultWidget;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void resizeEvent (QResizeEvent*) override;

Q_SIGNALS:

private:
    const int               mMinWidth = 900;
    const int               mMinHeight = 600;

    QLayout*                mMainLayout;

    ScannerTaskWidget*      mScannerTaskWidget;
    ScannerResultWidget*    mScannerResultWidget;
};

#endif // MAINWINDOW_H
