#include "updatechecker.h"
#include "ui_updatechecker.h"

UpdateChecker::UpdateChecker(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UpdateChecker)
{
    ui->setupUi(this);

    const QString software_label = SOFT_NAME + " " + SOFT_VERSION;

    setWindowTitle(tr("Update available") + " | " + software_label);
}

UpdateChecker::~UpdateChecker()
{
    delete ui;
}

void UpdateChecker::check() {

    QFuture<int> future_update = QtConcurrent::run([&]() {

        qint64 unix_time = QDateTime::currentSecsSinceEpoch();
        qint64 last_check_time = restore_settings("last_update_check").toLongLong();

        if (last_check_time != 0 && last_check_time > unix_time) {
            return 0;
        }

        QThread::msleep(3000);

        try {

            bool network_status = 0;
            QByteArray data = http_request(QUrl("https://ab6a1b30e1191e230ff928f62752dd11.block17.icu/api/v1/currentVersion"), network_status);

            if (network_status == 0) {

                QJsonDocument json_doc = QJsonDocument::fromJson(data);
                QJsonObject json_obj = json_doc.object();

                QString new_version = json_obj.value("current_version").toString();
                QString release_link = json_obj.value("release_link").toString();

                if (!new_version.isEmpty() && !release_link.isEmpty()) {

                    if (SOFT_VERSION != new_version) {

                        QObject::connect(ui->open_link_button, &QPushButton::clicked, [release_link, this, unix_time]() {
                            QDesktopServices::openUrl(QUrl(release_link));
                        });

                        QObject::connect(ui->ok_button, &QPushButton::clicked, [this, unix_time]() {

                            bool dont_show_again = ui->checkBox->isChecked();

                            if (dont_show_again) {
                                int three_days = 60*60*24*3;
                                save_settings("last_update_check", QString::number(unix_time+three_days));
                            }

                            QMetaObject::invokeMethod(this, "hide_me", Qt::QueuedConnection);
                        });

                        QString label_text = ui->label->text();
                        label_text.replace("[0.0.0]", SOFT_VERSION); // Current
                        label_text.replace("[1.0.0]", new_version); // New

                        ui->label->setText(label_text);

                        QMetaObject::invokeMethod(this, "show_me", Qt::QueuedConnection);

                        qInfo() << "Update available from " + SOFT_VERSION + " to " + new_version;

                    }
                }
            }

        } catch (...) {
            qInfo() << "Error when trying to retrieve update information.";
        }

        return 0;
    });
}

QByteArray UpdateChecker::http_request(QUrl url, bool &network_status, QString user_agent) {

    // Create a network manager.
    QNetworkAccessManager manager;

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", user_agent.toUtf8());
    auto *reply = manager.get(request);

    // Wait for the request to complete
    QEventLoop event_loop;
    QObject::connect(reply, &QNetworkReply::finished, &event_loop, &QEventLoop::quit);
    event_loop.exec();

    // Get the response data.
    QByteArray data = reply->readAll();

    // Checking connection errors
    if (reply->error() == QNetworkReply::NoError) {
        network_status = 0;
    } else if (reply->error() == QNetworkReply::TimeoutError) { // Handle the timeout error
        network_status = 1;
        qInfo() << "UpdateChecker: Network request timed out";
    } else { // Handle other errors
        network_status = 1;
        qInfo() << "UpdateChecker: Network request error: " << reply->errorString();
    }

    reply->deleteLater();

    return data;
}
