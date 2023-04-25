#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QWidget>
#include <QSysInfo>
#include <QToolButton>
#include <QThread>

#include "string_encryption.h"

#include "mainwindow.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

    void window_data_wipe();

    void save_settings(QString name, QString data);
    QString restore_settings(QString name);

private slots:
    void on_login_button_clicked();

    void on_create_button_clicked();

    void on_new_password_sign_up_eye_button_pressed();

    void on_new_password_sign_up_eye_button_released();

    void on_repeat_password_sign_up_eye_button_pressed();

    void on_repeat_password_sign_up_eye_button_released();

    void on_password_sign_in_eye_button_pressed();

    void on_password_sign_in_eye_button_released();

private:
    Ui::LoginWindow *ui;

    MainWindow mainWindow;

    QMap<QString, QString> notification_style;
};

#endif // LOGINWINDOW_H
