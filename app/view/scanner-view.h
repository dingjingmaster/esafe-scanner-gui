#ifndef SCANNERVIEW_H
#define SCANNERVIEW_H

#include <QTableView>

class ScannerView : public QTableView
{
    Q_OBJECT
public:
    explicit ScannerView(QWidget *parent = nullptr);

protected:
    void paintEvent (QPaintEvent* ev) override;
    
Q_SIGNALS:
    void updateView ();

private:
    float            mCurPos;
};

#endif // SCANNERVIEW_H
