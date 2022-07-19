#ifndef SCANNERRESULTMODEL_H
#define SCANNERRESULTMODEL_H

#include <QObject>

class ScannerResultModel : public QObject
{
    Q_OBJECT
public:
    explicit ScannerResultModel(QObject *parent = nullptr);

};

#endif // SCANNERRESULTMODEL_H
