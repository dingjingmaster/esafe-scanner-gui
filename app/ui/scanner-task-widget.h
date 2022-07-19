#ifndef SCANNERTASKWIDGET_H
#define SCANNERTASKWIDGET_H

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;

class ScannerTaskWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScannerTaskWidget(QWidget *parent = nullptr);

private:
    QVBoxLayout*            mMainLayout;
};

#endif // SCANNERTASKWIDGET_H
