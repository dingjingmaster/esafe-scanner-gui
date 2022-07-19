#ifndef MAINTOOLBAR_H
#define MAINTOOLBAR_H

#include <QToolBar>

class MainToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit MainToolBar(QWidget *parent = nullptr);


private:
    const int           mMaxHeight = 120;
};

#endif // MAINTOOLBAR_H
