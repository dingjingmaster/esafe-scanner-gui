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
    void loadTaskResult (QString taskName, QString taskFilter, QStringList scanDir, QString filterOutDir);

public Q_SLOTS:
    void onItemDeleted (QString);

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
    void addNewFile (QList<ScannerResultItem*> it);
    void updateFile (QList<ScannerResultItem*> it);
    void delOldFile (QList<ScannerResultItem*> it);

    void addNewFile (ScannerResultItem* it);
    void updateFile (ScannerResultItem* it);
    void delOldFile (ScannerResultItem* it);

private:
    bool                        mIsRunning;         //

    ScanResultHelperPrivate*    d_ptr;

    Q_DISABLE_COPY(ScanResultHelper)
    Q_DECLARE_PRIVATE(ScanResultHelper)
};

#endif // SCANRESULTHELPER_H
