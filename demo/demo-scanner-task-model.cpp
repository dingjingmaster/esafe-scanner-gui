#include <QApplication>
#include <QListView>
#include <QTableView>

#include "../app/model/scanner-task-model.h"

int main (int argc, char* argv[])
{
    QApplication app (argc, argv);

    QTableView tv;
//    QListView tv;
    ScannerTaskModel m;

    tv.setModel(&m);
    tv.show();

    return app.exec();
}
