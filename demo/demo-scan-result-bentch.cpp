#include "../app/db/db-manager.h"
#include "../app/utils/scan-result-helper.h"

int main (int argc, char* argv[])
{
    auto s = DBManager::instance ()->getResultHelper ();

    s->testInsertItem ();

    return 0;
}
