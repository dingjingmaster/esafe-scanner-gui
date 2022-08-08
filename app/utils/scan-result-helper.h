#ifndef SCANRESULTHELPER_H
#define SCANRESULTHELPER_H

#include <QObject>

class ScannerResultItem;
class ScanResultHelperPrivate;

class ScanResultHelper : public QObject
{
    Q_OBJECT
public:
    explicit ScanResultHelper(QString dbPath, QObject *parent = nullptr);
    ~ScanResultHelper();

    void misReportByName(QString);
    void deleteItemByName (QString);


public Q_SLOTS:
    void clearData();
    void loadTaskResult (QString taskName = "", QString taskFilter = "");

private:

Q_SIGNALS:
    void addNewFile (ScannerResultItem* it);
    void delOldFile (ScannerResultItem* it);

private:
    ScanResultHelperPrivate*    d_ptr;

    Q_DISABLE_COPY(ScanResultHelper)
    Q_DECLARE_PRIVATE(ScanResultHelper)
};

#endif // SCANRESULTHELPER_H
