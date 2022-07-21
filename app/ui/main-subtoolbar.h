#ifndef MAINSUBTOOLBAR_H
#define MAINSUBTOOLBAR_H

#include <QWidget>

class QHBoxLayout;
class MainSubToolBar : public QWidget
{
    Q_OBJECT
public:
    explicit MainSubToolBar(QWidget *parent = nullptr);
    void addButton (QString name);

private:
    QHBoxLayout*            mMainLayout;
};

#endif // MAINSUBTOOLBAR_H
