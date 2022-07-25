#ifndef EXPORTSCANRESULT_H
#define EXPORTSCANRESULT_H

#include <QObject>
#include <fstream>

class ScannerResultItem;

using namespace std;

class ExportScanResult : public QObject
{
    Q_OBJECT
public:
    explicit ExportScanResult(QString path, QObject *parent = nullptr);
    ~ExportScanResult();

    ofstream& write (ScannerResultItem& it);

private:
    QString                 mExportPath;
    ofstream                mOutStream;
};

#endif // EXPORTSCANRESULT_H
