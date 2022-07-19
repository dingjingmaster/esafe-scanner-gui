#ifndef SCANNERMODEL_H
#define SCANNERMODEL_H

#include <QObject>

class ScannerModel : public QObject
{
    Q_OBJECT
public:
    explicit ScannerModel(QObject *parent = nullptr);

};

#endif // SCANNERMODEL_H
