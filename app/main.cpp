#include "3thrd/singleton/singleton-app.h"
#include "ui/main-window.h"


int main(int argc, char *argv[])
{
    SingletonApp app (argc, argv, APP_NAME);

    MainWindow w;
    w.show();
    if (app.isPrimary()) {
    } else {
        qApp->exit();
    }

    return app.exec();
}
