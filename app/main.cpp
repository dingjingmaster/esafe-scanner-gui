#include "3thrd/singleton/singleton-app.h"

#include "ui/main-window.h"
#include "style/main-style.h"

#include <QRgba64>

//#include <QCommonStyle>
#include <QTextCodec>

#include <QFile>

void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char *argv[])
{
    qInstallMessageHandler(messageOutput);

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    qInfo() << "\n\n\n\n\n\nstart...";

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));

    SingletonApp app (argc, argv, APP_NAME);
    SingletonApp::setApplicationName ("DLP");

    SingletonApp::setApplicationDisplayName ("DLP");
    SingletonApp::setStyle(new MainStyle("fusion"));

    QPalette palette = QApplication::palette();
    QColor red = qRgb(255, 138, 140);
    palette.setColor (QPalette::All, QPalette::Highlight, red);
    QApplication::setPalette (palette);

    MainWindow* w = nullptr;
    if (app.isPrimary()) {
        w = new MainWindow;
        w->setWindowIcon (QIcon("://data/dsip.png"));

        QFile file ("://data/stylesheet.qss");
        if (file.open(QFile::ReadOnly)) {
            w->setStyleSheet(file.readAll());
            file.close();
        }

        SingletonApp::connect (&app, &SingletonApp::receivedMessage, [=] (quint32, QByteArray msg) {
            qWarning() << "msg: " << msg;
            if ("active" == msg) {
                qWarning() << "msg: " << msg;
                Q_EMIT w->activePrimaryWindow();
            }
        });

        w->show();
    } else {
        qWarning() << "send msg";
        app.sendMessage ("active");
        SingletonApp::exit (0);
        exit (0);
    }

    return SingletonApp::exec();
}

void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(type)

    QByteArray localMsg = msg.toLocal8Bit();

    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";

    fprintf(stdout, "[%s - %s:%d] %s\n", file, function, context.line, localMsg.constData());
}
