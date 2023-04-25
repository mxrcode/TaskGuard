#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QSettings>
#include <QFile>
#include <QProcess>

#include "version.h"

#include "string_encryption.h"

namespace Ui {
class encryption;
}

class encryption : public QMainWindow
{
    Q_OBJECT

public:
    explicit encryption(QMainWindow *parent = nullptr);
    ~encryption();

    void save_settings(QString name, QString data);
    QString restore_settings(QString name);

    void open_widget();
    void set_data(bool encryption, QString password, QSqlDatabase *db, QString db_path);

    bool db_encrypt(QString password);
    bool db_decrypt(QString password);
    bool db_change_key(QString old_password, QString new_password);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:

    void on_new_password_encrypt_button_pressed();

    void on_new_password_encrypt_button_released();

    void on_repeat_password_encrypt_button_pressed();

    void on_repeat_password_encrypt_button_released();

    void on_encrypt_button_clicked();

    void on_decrypt_database_button_clicked();

    void on_change_password_button_clicked();

    void on_current_password_decrypt_eye_button_pressed();

    void on_current_password_decrypt_eye_button_released();

    void on_decrypt_button_clicked();

    void on_current_change_password_eye_button_pressed();

    void on_current_change_password_eye_button_released();

    void on_new_change_password_eye_button_pressed();

    void on_new_change_password_eye_button_released();

    void on_repeat_change_password_eye_button_pressed();

    void on_repeat_change_password_eye_button_released();

    void on_submit_change_password_clicked();

private:
    Ui::encryption *ui;
    QMainWindow *m_parent;

    QSqlDatabase *m_db;
    QString m_db_path;

    bool use_encryption;
    QString db_password;


    QMap<QString, QString> notification_style;
};

#endif // ENCRYPTION_H
