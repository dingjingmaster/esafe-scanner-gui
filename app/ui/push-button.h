#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QWidget>

class QLabel;
class QVBoxLayout;
class PushButton : public QWidget
{
    Q_OBJECT
public:
    explicit PushButton(QWidget *parent = nullptr);


private:
    const int           mMaxWidth = 120;

    QLabel*             mLabel;
    QVBoxLayout*        mLayout;

    QString             mText;
};

#endif // PUSHBUTTON_H
