#include <QApplication>

#include "../app/3thrd/singleton/singleton-app.h"

int main (int argc, char* argv[])
{
    SingletonApp app (argc, argv, "demo", false);

    if (app.isSecondary()) {
        qDebug() << "secondary demo";
    }

    return app.exec();
}
