#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QWidget>

class QLabel;
class QVBoxLayout;
class PushButton : public QWidget
{
    Q_OBJECT
public:
    enum Type { Type1, Type2 };
public:
    explicit PushButton(QWidget *parent = nullptr, Type t=Type1);

    void setText (QString text);

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

Q_SIGNALS:
    void clicked ();
    void doubleClicked ();

private:
    const int           mMaxWidth = 120;

    QLabel*             mLabel;
    QVBoxLayout*        mLayout;

    QString             mText;

    Type                mType;
};

#endif // PUSHBUTTON_H
