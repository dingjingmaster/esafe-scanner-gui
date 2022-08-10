#ifndef SCANRESULTHELPER_H
#define SCANRESULTHELPER_H

#include <QObject>

class DBManager;
class ScannerResultItem;
class ScanResultHelperPrivate;

class ScanResultHelper : public QObject
{
    Q_OBJECT
    friend class DBManager;
public:
    void misReportByName(QString);
    void deleteItemByName (QString);

    void testInsertItem();

private:
    explicit ScanResultHelper(QString dbPath, QObject *parent = nullptr);
    ~ScanResultHelper();


public Q_SLOTS:
    void clearData();
    void refresResult();
    void loadTaskResult (QString taskName, QString taskFilter, QStringList scanDir);

private:

Q_SIGNALS:
    void addNewFile (ScannerResultItem* it);
    void updateFile (ScannerResultItem* it);
    void delOldFile (ScannerResultItem* it);

private:
    ScanResultHelperPrivate*    d_ptr;

    Q_DISABLE_COPY(ScanResultHelper)
    Q_DECLARE_PRIVATE(ScanResultHelper)
};

#endif // SCANRESULTHELPER_H
