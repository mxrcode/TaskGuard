#include "loginwindow.h"
#include "mainwindow.h"

void message_handler(QtMsgType type, const QMessageLogContext &context, const QString &msg) // Part for outputting debug info to qInfo.log
{
    QFile log_file("qInfo.log");
    if (log_file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream stream(&log_file);
        QString t_msg = QDateTime::currentDateTime().toString() + " : " + msg;
        stream << t_msg << Qt::endl;
    }
}

int main(int argc, char *argv[])
{
    QThread::msleep(100);

    QApplication app(argc, argv);

    // qInstallMessageHandler(message_handler); // Output debug info to qInfo.log | Disable for Debug

    app.setWindowIcon(QIcon(":/img/logo-dark.svg"));

    QTranslator translator;
    const QStringList ui_languages = QLocale::system().uiLanguages();
    for (const QString &locale : ui_languages) {
        const QString base_name = SOFT_NAME + "_" + QLocale(locale).name();
        if (translator.load("translations/" + base_name)) {
            app.installTranslator(&translator);
            break;
        }
    }

    LoginWindow loginWindow;

    return app.exec();
}
