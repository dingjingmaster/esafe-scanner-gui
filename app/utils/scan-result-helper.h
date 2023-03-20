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
    void testInsertItem();

    void misReportByIDs(const QStringList&);
    void deleteItemByIDs(const QStringList&);

private:
    explicit ScanResultHelper(QString dbPath, QObject *parent = nullptr);
    ~ScanResultHelper();


public Q_SLOTS:
    void reset ();
    void cancel ();
    void refreshResult();
    void loadTaskResult (const QString& taskName, const QString& taskFilter, const QStringList& scanDir, const QString& filterOutDir);

public Q_SLOTS:
    void onItemDeleted (const QString&);

private:

Q_SIGNALS:
    /**
     * @brief
     *  仅仅是处理了误报、删除操作的文件
     */
    void detailOne();
    void allItemsUpdated ();
    void delOldFile (QString& it);

    void cancelledFinished ();
    void addNewFile (const QList<QSharedPointer<ScannerResultItem>>& it);
    void updateFile (const QList<QSharedPointer<ScannerResultItem>>& it);
    void delOldFile (const QList<QSharedPointer<ScannerResultItem>>& it);

    void addNewFile (const QSharedPointer<ScannerResultItem>& it);
    void updateFile (const QSharedPointer<ScannerResultItem>& it);
    void delOldFile (const QSharedPointer<ScannerResultItem>& it);

private:
    ScanResultHelperPrivate*    d_ptr;

    Q_DISABLE_COPY(ScanResultHelper)
    Q_DECLARE_PRIVATE(ScanResultHelper)
};

#endif // SCANRESULTHELPER_H
