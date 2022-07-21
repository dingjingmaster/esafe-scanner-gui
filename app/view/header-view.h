#ifndef HEADERVIEW_H
#define HEADERVIEW_H

#include <QHeaderView>
#include <QObject>

class HeaderView : public QHeaderView
{
    Q_OBJECT
public:
    HeaderView(Qt::Orientation orientation, QWidget* parent=nullptr);

    bool isChecked () const;
    void setChecked (bool);

Q_SIGNALS:
    void checkBoxClicked (bool state);
//    void stateChanged(bool);

public Q_SLOTS:
//    void onStateChanged(int);

public:
    void paintSection (QPainter* p, const QRect& rect, int logicalIndex) const override;

protected:
//    bool event (QEvent* e) override;
    void mousePressEvent (QMouseEvent* e) override;
//    void mouseReleaseEvent (QMouseEvent* e) override;

private:
    void redrawCheckBox();

private:
//    bool                mPressed = false;
    bool                mChecked = false;
//    bool                mTristate = false;
//    bool                mNoChange = false;
//    bool                mMoving = false;
};

#endif // HEADERVIEW_H
