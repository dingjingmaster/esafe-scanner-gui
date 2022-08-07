#include "db-manager.h"

#include <QMutex>

static QMutex locker;

DBManager* DBManager::gInstance = nullptr;

DBManager* DBManager::instance()
{
    if (!gInstance) {
        locker.lock ();
        if (!gInstance) {
            gInstance = new DBManager;
        }
        locker.unlock ();
    }
}

DBManager::DBManager(QObject *parent)
    : QObject{parent}
{

}
