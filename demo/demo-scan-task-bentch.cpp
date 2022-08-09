#include "../app/db/db-manager.h"
#include "../app/utils/scan-task-helper.h"

int main (int argc, char* argv[])
{
    ScanTaskHelper* s = DBManager::instance ()->getTaskHelper ();

    s->testInsertItem ();

    return 0;
}
