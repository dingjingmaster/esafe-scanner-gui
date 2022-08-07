#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <qglobal.h>

class DBManager : public QObject
{
    Q_OBJECT
public:
    enum CurPage { CUR_TASK, CUR_RESULT }; Q_ENUM(CurPage)
public:
    DBManager* instance();


private:
    explicit DBManager(QObject *parent = nullptr);
    //~DBManager();


private:
    CurPage                 mPage;
    static DBManager*       gInstance;
};

#endif // DBMANAGER_H
