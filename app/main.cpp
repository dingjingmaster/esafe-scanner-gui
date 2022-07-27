#include "3thrd/singleton/singleton-app.h"

#include "ui/main-window.h"
#include "style/main-style.h"

//#include <QCommonStyle>

#include <QFile>


int main(int argc, char *argv[])
{
    SingletonApp app (argc, argv, APP_NAME);

    app.setStyle(new MainStyle(""));
//    app.setStyle(new QCommonStyle());

    MainWindow w;

    QFile file ("://data/stylesheet.qss");
    if (file.open(QFile::ReadOnly)) {
        w.setStyleSheet(file.readAll());
        file.close();
    }

    if (app.isPrimary()) {
        w.show();
    } else {
        qApp->exit();
    }

    return app.exec();
}
