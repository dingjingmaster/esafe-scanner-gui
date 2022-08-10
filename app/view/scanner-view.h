#ifndef SCANNERVIEW_H
#define SCANNERVIEW_H

#include <QTableView>
#include <QWheelEvent>

class ScannerView : public QTableView
{
    Q_OBJECT
public:
    explicit ScannerView(QWidget *parent = nullptr);

    void wheelEvent (QWheelEvent* event) override;
    
Q_SIGNALS:
    void updateView ();
    void rowchanged (int row);
    void verticalbarValue (double);
};

#endif // SCANNERVIEW_H
