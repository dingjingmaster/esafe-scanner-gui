#include <QApplication>
#include <QListView>

#include "../app/view/scanner-view.h"
#include "../app/model/scanner-task-model.h"

int main (int argc, char* argv[])
{
    QApplication app (argc, argv);

//    QTableView tv;
    ScannerView tv;
    ScannerTaskModel m;

    tv.setModel(&m);
    tv.show();

    return app.exec();
}
