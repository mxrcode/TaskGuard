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

    qInstallMessageHandler(message_handler); // Output debug info to qInfo.log | Disable for Debug

    app.setWindowIcon(QIcon(":/img/logo-dark.svg"));

    QTranslator translator;
    QString current_lang = restore_settings("lang");
    if (current_lang == "system_system" || current_lang.isEmpty() || current_lang == "") {
        const QStringList ui_languages = QLocale::system().uiLanguages();
        for (const QString &locale : ui_languages) {
            const QString base_name = QLocale(locale).name();
            if (translator.load("translations/" + base_name)) {
                app.installTranslator(&translator);
                break;
            }
        }
    } else {
        if (translator.load("translations/" + current_lang)) {
            app.installTranslator(&translator);
        }
    }

    // CHECK IF THE APPLICATION IS RUNNING
    // IF SO, SEND THEM A REQUEST TO ACTIVATE THE WINDOW AND CLOSE THE CURRENT COPY
    const QString APP_ID = QCryptographicHash::hash(QString(QCoreApplication::applicationFilePath() + "-" + SOFT_NAME + "-" + SOFT_VERSION).toUtf8(), QCryptographicHash::Md5).toHex();

    QLocalSocket socket;
    socket.connectToServer(APP_ID);
    if (socket.waitForConnected(500))
    {
        QString command = "show";
        QByteArray byte_command = command.toUtf8();
        socket.write(byte_command);
        socket.waitForBytesWritten();
        socket.disconnectFromServer();
        qInfo() << "socket : Unable to start the application. Another instance may already be running.";
        return 0;
    }

    QLocalServer server;
    if (!server.listen(APP_ID))
    {
        qInfo() << "server : Unable to start the application. Another instance may already be running.";
        return 1;
    }

    LoginWindow loginWindow;
    QMutex mutex_loginWindow;

    QObject::connect(&server, &QLocalServer::newConnection, [&]() {

        QLocalSocket *client_connection = server.nextPendingConnection();
        if (client_connection) {

            QObject::connect(client_connection, &QLocalSocket::readyRead, [=, &mutex_loginWindow, &loginWindow]() {
                QByteArray byte_command = client_connection->readAll();
                QString command = QString::fromUtf8(byte_command);
                if (command == "show") {
                    mutex_loginWindow.lock();
                    loginWindow.show_me();
                    mutex_loginWindow.unlock();
                }
                client_connection->disconnectFromServer();
                client_connection->deleteLater();
            });
        }
    });
    //\CHECK IF THE APPLICATION IS RUNNING

    return app.exec();
}
