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

#if IS_UOS

#endif
    palette.setColor (QPalette::All, QPalette::WindowText,          QColor::fromRgbF(0, 0, 0, 1));                          // ok
//    palette.setColor (QPalette::All, QPalette::Button,              QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
//    palette.setColor (QPalette::All, QPalette::Light,               QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
//    palette.setColor (QPalette::All, QPalette::Midlight,            QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
    palette.setColor (QPalette::All, QPalette::Dark,                QColor::fromRgbF(0, 0, 0, 1));                          // ok
//    palette.setColor (QPalette::All, QPalette::Mid,                 QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
    palette.setColor (QPalette::All, QPalette::Text,                QColor::fromRgbF(0, 0, 0, 1));                          // ok
//    palette.setColor (QPalette::All, QPalette::BrightText,          QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
//    palette.setColor (QPalette::All, QPalette::ButtonText,          QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
    palette.setColor (QPalette::All, QPalette::Base,                QColor::fromRgbF(1, 1, 1, 1));                          // ok
//    palette.setColor (QPalette::All, QPalette::Window,              QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
//    palette.setColor (QPalette::All, QPalette::Shadow,              QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
    palette.setColor (QPalette::All, QPalette::Highlight,           QColor::fromRgbF(1, 1, 1, 1));                          // ok
//    palette.setColor (QPalette::All, QPalette::HighlightedText,     QColor::fromRgbF(0, 0, 0, 1));                          // ok
//    palette.setColor (QPalette::All, QPalette::Link,                QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
//    palette.setColor (QPalette::All, QPalette::LinkVisited,         QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
//    palette.setColor (QPalette::All, QPalette::AlternateBase,       QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
//    palette.setColor (QPalette::All, QPalette::NoRole,              QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
    palette.setColor (QPalette::All, QPalette::ToolTipBase,         QColor::fromRgbF(1, 1, 1, 1));
    palette.setColor (QPalette::All, QPalette::ToolTipText,         QColor::fromRgbF(0, 0, 0, 1));
    palette.setColor (QPalette::All, QPalette::Foreground,          QColor::fromRgbF(0, 0, 0, 1));                          // ok
    palette.setColor (QPalette::All, QPalette::Background,          QColor::fromRgbF(1, 1, 1, 1));                          // ok

    palette.setColor (QPalette::Active, QPalette::WindowText,       QColor::fromRgbF(1, 1, 1, 1));                          // ok
//    palette.setColor (QPalette::Active, QPalette::Button,           QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
//    palette.setColor (QPalette::Active, QPalette::Light,            QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
//    palette.setColor (QPalette::Active, QPalette::Midlight,         QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
//    palette.setColor (QPalette::Active, QPalette::Dark,             QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
//    palette.setColor (QPalette::Active, QPalette::Mid,              QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
    palette.setColor (QPalette::Active, QPalette::Text,             QColor::fromRgbF(1, 1, 1, 1));
    palette.setColor (QPalette::Active, QPalette::BrightText,       QColor::fromRgbF(1, 1, 1, 1));                          // ok
    palette.setColor (QPalette::Active, QPalette::ButtonText,       QColor::fromRgbF(1, 1, 1, 1));                          // ok
    palette.setColor (QPalette::Active, QPalette::Base,             QColor::fromRgbF(1, 1, 1, 1));                          // ok
//    palette.setColor (QPalette::Active, QPalette::Window,           QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
//    palette.setColor (QPalette::Active, QPalette::Shadow,           QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
    palette.setColor (QPalette::Active, QPalette::Highlight,        QColor::fromRgbF(1, 1, 1, 1));
    palette.setColor (QPalette::Active, QPalette::HighlightedText,  QColor::fromRgbF(1, 1, 1, 1));                          // ok
//    palette.setColor (QPalette::Active, QPalette::Link,             QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
//    palette.setColor (QPalette::Active, QPalette::LinkVisited,      QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
//    palette.setColor (QPalette::Active, QPalette::AlternateBase,    QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
//    palette.setColor (QPalette::Active, QPalette::NoRole,           QColor::fromRgbF(0.254902, 0.301961, .407843, 1));
    palette.setColor (QPalette::Active, QPalette::ToolTipBase,      QColor::fromRgbF(1, 1, 1, 1));                          // ok
    palette.setColor (QPalette::Active, QPalette::ToolTipText,      QColor::fromRgbF(0, 0, 0, 1));                          // ok
    palette.setColor (QPalette::Active, QPalette::NColorRoles,      QColor::fromRgbF(0, 0, 0, 1));                          // ok ??
    palette.setColor (QPalette::Active, QPalette::Foreground,       QColor::fromRgbF(1, 1, 1, 1));                          // ok
    palette.setColor (QPalette::Active, QPalette::Background,       QColor::fromRgbF(1, 1, 1, 1));                          // ok

    palette.setColor (QPalette::Disabled, QPalette::WindowText,         QColor::fromRgbF(0, 0, 0, 1));
//    palette.setColor (QPalette::Disabled, QPalette::Button,             QColor::fromRgbF(.941176, .941176, .941176, 1));
//    palette.setColor (QPalette::Disabled, QPalette::Light,              QColor::fromRgbF(.941176, .941176, .941176, 1));
//    palette.setColor (QPalette::Disabled, QPalette::Midlight,           QColor::fromRgbF(.941176, .941176, .941176, 1));
//    palette.setColor (QPalette::Disabled, QPalette::Dark,               QColor::fromRgbF(.941176, .941176, .941176, 1));
//    palette.setColor (QPalette::Disabled, QPalette::Mid,                QColor::fromRgbF(.941176, .941176, .941176, 1));
    palette.setColor (QPalette::Disabled, QPalette::Text,               QColor::fromRgbF(0, 0, 0, 1));
//    palette.setColor (QPalette::Disabled, QPalette::BrightText,         QColor::fromRgbF(.941176, .941176, .941176, 1));
//    palette.setColor (QPalette::Disabled, QPalette::ButtonText,         QColor::fromRgbF(.941176, .941176, .941176, 1));
//    palette.setColor (QPalette::Disabled, QPalette::Base,               QColor::fromRgbF(.941176, .941176, .941176, 1));
//    palette.setColor (QPalette::Disabled, QPalette::Window,             QColor::fromRgbF(.941176, .941176, .941176, 1));
//    palette.setColor (QPalette::Disabled, QPalette::Shadow,             QColor::fromRgbF(.941176, .941176, .941176, 1));
    palette.setColor (QPalette::Disabled, QPalette::Highlight,          QColor::fromRgbF(0, 0, 0, 1));
    palette.setColor (QPalette::Disabled, QPalette::HighlightedText,    QColor::fromRgbF(0, 0, 0, 1));
//    palette.setColor (QPalette::Disabled, QPalette::Link,               QColor::fromRgbF(.941176, .941176, .941176, 1));
//    palette.setColor (QPalette::Disabled, QPalette::LinkVisited,        QColor::fromRgbF(.941176, .941176, .941176, 1));
//    palette.setColor (QPalette::Disabled, QPalette::AlternateBase,      QColor::fromRgbF(.941176, .941176, .941176, 1));
//    palette.setColor (QPalette::Disabled, QPalette::NoRole,             QColor::fromRgbF(.941176, .941176, .941176, 1));
    palette.setColor (QPalette::Disabled, QPalette::ToolTipBase,        QColor::fromRgbF(1, 1, 1, 1));                      // ok
    palette.setColor (QPalette::Disabled, QPalette::ToolTipText,        QColor::fromRgbF(0, 0, 0, 1));                      // ok
//    palette.setColor (QPalette::Disabled, QPalette::NColorRoles,        QColor::fromRgbF(.941176, .941176, .941176, 1));
    palette.setColor (QPalette::Disabled, QPalette::Foreground,         QColor::fromRgbF(0, 0, 0, 1));                      // ok
    palette.setColor (QPalette::Disabled, QPalette::Background,         QColor::fromRgbF(1, 1, 1, 1));                      // ok

    palette.setColor (QPalette::Inactive, QPalette::WindowText,         QColor::fromRgbF(0, 0, 0, 1));
//    palette.setColor (QPalette::Inactive, QPalette::Button,             QColor::fromRgbF(.929412, .929412, .929412, 1));
//    palette.setColor (QPalette::Inactive, QPalette::Light,              QColor::fromRgbF(.929412, .929412, .929412, 1));
//    palette.setColor (QPalette::Inactive, QPalette::Midlight,           QColor::fromRgbF(.929412, .929412, .929412, 1));
//    palette.setColor (QPalette::Inactive, QPalette::Dark,               QColor::fromRgbF(.929412, .929412, .929412, 1));
//    palette.setColor (QPalette::Inactive, QPalette::Mid,                QColor::fromRgbF(.929412, .929412, .929412, 1));
//    palette.setColor (QPalette::Inactive, QPalette::Text,               QColor::fromRgbF(.929412, .929412, .929412, 1));
    palette.setColor (QPalette::Inactive, QPalette::BrightText,         QColor::fromRgbF(0, 0, 0, 1));
    palette.setColor (QPalette::Inactive, QPalette::ButtonText,         QColor::fromRgbF(0, 0, 0, 1));
    palette.setColor (QPalette::Inactive, QPalette::Base,               QColor::fromRgbF(1, 1, 1, 1));                      // ok
//    palette.setColor (QPalette::Inactive, QPalette::Window,             QColor::fromRgbF(.929412, .929412, .929412, 1));
//    palette.setColor (QPalette::Inactive, QPalette::Shadow,             QColor::fromRgbF(.929412, .929412, .929412, 1));
//    palette.setColor (QPalette::Inactive, QPalette::Highlight,          QColor::fromRgbF(.929412, .929412, .929412, 1));
    palette.setColor (QPalette::Inactive, QPalette::HighlightedText,    QColor::fromRgbF(0, 0, 0, 1));
//    palette.setColor (QPalette::Inactive, QPalette::Link,               QColor::fromRgbF(.929412, .929412, .929412, 1));
//    palette.setColor (QPalette::Inactive, QPalette::LinkVisited,        QColor::fromRgbF(.929412, .929412, .929412, 1));
//    palette.setColor (QPalette::Inactive, QPalette::AlternateBase,      QColor::fromRgbF(.929412, .929412, .929412, 1));
//    palette.setColor (QPalette::Inactive, QPalette::NoRole,             QColor::fromRgbF(.929412, .929412, .929412, 1));
    palette.setColor (QPalette::Inactive, QPalette::ToolTipBase,        QColor::fromRgbF(1, 1, 1, 1));
    palette.setColor (QPalette::Inactive, QPalette::ToolTipText,        QColor::fromRgbF(0, 0, 0, 1));
//    palette.setColor (QPalette::Inactive, QPalette::NColorRoles,        QColor::fromRgbF(.929412, .929412, .929412, 1));
    palette.setColor (QPalette::Inactive, QPalette::Foreground,         QColor::fromRgbF(0, 0, 0, 1));
    palette.setColor (QPalette::Inactive, QPalette::Background,         QColor::fromRgbF(1, 1, 1, 1));                      // ok

    palette.setColor (QPalette::Current, QPalette::WindowText,          QColor::fromRgbF(0, 0, 0, 1));                      // ok
//    palette.setColor (QPalette::Current, QPalette::Button,              QColor::fromRgbF(.890196, .890196, .980196, 1));
//    palette.setColor (QPalette::Current, QPalette::Light,               QColor::fromRgbF(.890196, .890196, .980196, 1));
//    palette.setColor (QPalette::Current, QPalette::Midlight,            QColor::fromRgbF(.890196, .890196, .980196, 1));
//    palette.setColor (QPalette::Current, QPalette::Dark,                QColor::fromRgbF(.890196, .890196, .980196, 1));
//    palette.setColor (QPalette::Current, QPalette::Mid,                 QColor::fromRgbF(.890196, .890196, .980196, 1));
    palette.setColor (QPalette::Current, QPalette::Text,                QColor::fromRgbF(1, 1, 1, 1));                      // ok
    palette.setColor (QPalette::Current, QPalette::BrightText,          QColor::fromRgbF(1, 1, 1, 1));                      // ok
    palette.setColor (QPalette::Current, QPalette::ButtonText,          QColor::fromRgbF(0, 0, 0, 1));                      // ok
    palette.setColor (QPalette::Current, QPalette::Base,                QColor::fromRgbF(1, 1, 1, 1));                      // ok
//    palette.setColor (QPalette::Current, QPalette::Window,              QColor::fromRgbF(.890196, .890196, .980196, 1));
//    palette.setColor (QPalette::Current, QPalette::Shadow,              QColor::fromRgbF(.890196, .890196, .980196, 1));
    palette.setColor (QPalette::Current, QPalette::Highlight,           QColor::fromRgbF(1, 1, 1, 1));                      // ok
    palette.setColor (QPalette::Current, QPalette::HighlightedText,     QColor::fromRgbF(1, 1, 1, 1));                      // ok
//    palette.setColor (QPalette::Current, QPalette::Link,                QColor::fromRgbF(.890196, .890196, .980196, 1));
//    palette.setColor (QPalette::Current, QPalette::LinkVisited,         QColor::fromRgbF(.890196, .890196, .980196, 1));
//    palette.setColor (QPalette::Current, QPalette::AlternateBase,       QColor::fromRgbF(.890196, .890196, .980196, 1));
//    palette.setColor (QPalette::Current, QPalette::NoRole,              QColor::fromRgbF(.890196, .890196, .980196, 1));
    palette.setColor (QPalette::Current, QPalette::ToolTipBase,         QColor::fromRgbF(1, 1, 1, 1));                      // ok
    palette.setColor (QPalette::Current, QPalette::ToolTipText,         QColor::fromRgbF(0, 0, 0, 1));                      // ok
//    palette.setColor (QPalette::Current, QPalette::NColorRoles,         QColor::fromRgbF(.890196, .890196, .980196, 1));
    palette.setColor (QPalette::Current, QPalette::Foreground,          QColor::fromRgbF(0, 0, 0, 1));                      // ok
    palette.setColor (QPalette::Current, QPalette::Background,          QColor::fromRgbF(1, 1, 1, 1));                      // ok

    palette.setColor (QPalette::NColorGroups, QPalette::WindowText,         QColor::fromRgbF(0, 0, 0, 1));                  // ok
//    palette.setColor (QPalette::NColorGroups, QPalette::Button,             QColor::fromRgbF(.254902, .301961, .407843, 1));
//    palette.setColor (QPalette::NColorGroups, QPalette::Light,              QColor::fromRgbF(.254902, .301961, .407843, 1));
//    palette.setColor (QPalette::NColorGroups, QPalette::Midlight,           QColor::fromRgbF(.254902, .301961, .407843, 1));
//    palette.setColor (QPalette::NColorGroups, QPalette::Dark,               QColor::fromRgbF(.254902, .301961, .407843, 1));
//    palette.setColor (QPalette::NColorGroups, QPalette::Mid,                QColor::fromRgbF(.254902, .301961, .407843, 1));
    palette.setColor (QPalette::NColorGroups, QPalette::Text,               QColor::fromRgbF(0, 0, 0, 1));                  // ok
    palette.setColor (QPalette::NColorGroups, QPalette::BrightText,         QColor::fromRgbF(0, 0, 0, 1));                  // ok
    palette.setColor (QPalette::NColorGroups, QPalette::ButtonText,         QColor::fromRgbF(0, 0, 0, 1));                  // ok
    palette.setColor (QPalette::NColorGroups, QPalette::Base,               QColor::fromRgbF(1, 1, 1, 1));                  // ok
//    palette.setColor (QPalette::NColorGroups, QPalette::Window,             QColor::fromRgbF(.254902, .301961, .407843, 1));
//    palette.setColor (QPalette::NColorGroups, QPalette::Shadow,             QColor::fromRgbF(.254902, .301961, .407843, 1));
    palette.setColor (QPalette::NColorGroups, QPalette::Highlight,          QColor::fromRgbF(1, 1, 1, 1));                  // ok
    palette.setColor (QPalette::NColorGroups, QPalette::HighlightedText,    QColor::fromRgbF(0, 0, 0, 1));                  // ok
//    palette.setColor (QPalette::NColorGroups, QPalette::Link,               QColor::fromRgbF(.254902, .301961, .407843, 1));
//    palette.setColor (QPalette::NColorGroups, QPalette::LinkVisited,        QColor::fromRgbF(.254902, .301961, .407843, 1));
//    palette.setColor (QPalette::NColorGroups, QPalette::AlternateBase,      QColor::fromRgbF(.254902, .301961, .407843, 1));
//    palette.setColor (QPalette::NColorGroups, QPalette::NoRole,             QColor::fromRgbF(.254902, .301961, .407843, 1));
    palette.setColor (QPalette::NColorGroups, QPalette::ToolTipBase,        QColor::fromRgbF(1, 1, 1, 1));                  // ok
    palette.setColor (QPalette::NColorGroups, QPalette::ToolTipText,        QColor::fromRgbF(0, 0, 0, 1));                  // ok
    palette.setColor (QPalette::NColorGroups, QPalette::Foreground,         QColor::fromRgbF(0, 0, 0, 1));                  // ok
    palette.setColor (QPalette::NColorGroups, QPalette::Background,         QColor::fromRgbF(1, 1, 1, 1));                  // ok

    palette.setColor (QPalette::All, QPalette::Highlight, red);
    palette.setColor (QPalette::All, QPalette::HighlightedText, Qt::white);
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
