#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLayout>

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);


Q_SIGNALS:

private:
    QLayout*            mMainLayout;
};

#endif // MAINWINDOW_H
