#include "../app/db/db-manager.h"
#include "../app/utils/scan-result-helper.h"

#include <QApplication>

int main (int argc, char* argv[])
{
    auto s = DBManager::instance ()->getResultHelper ();

    s->testInsertItem ();

    return 0;
}
