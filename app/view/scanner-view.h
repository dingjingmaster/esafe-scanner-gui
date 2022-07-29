#ifndef SCANNERVIEW_H
#define SCANNERVIEW_H

#include <QTableView>

class ScannerView : public QTableView
{
    Q_OBJECT
public:
    explicit ScannerView(QWidget *parent = nullptr);
    
Q_SIGNALS:
    void updateView ();
};

#endif // SCANNERVIEW_H
