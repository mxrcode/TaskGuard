#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QMainWindow>
#include <QtConcurrent>
#include <QThread>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QDesktopServices>
#include <QDateTime>

#include "version.h"

namespace Ui {
class UpdateChecker;
}

class UpdateChecker : public QMainWindow
{
    Q_OBJECT

public:
    explicit UpdateChecker(QWidget *parent = nullptr);
    ~UpdateChecker();

    void check();
    QByteArray http_request(QUrl url, bool &network_status, QString user_agent = SOFT_NAME + " " + SOFT_VERSION);

public slots:
    void show_me() {show();}
    void hide_me() {hide();}

private:
    Ui::UpdateChecker *ui;
};

#endif // UPDATECHECKER_H
