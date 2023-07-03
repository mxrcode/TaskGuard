#include "loginwindow.h"
#include "ui_loginwindow.h"

LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);

    QString key_data = restore_settings("en_key");
    QString use_encryption = restore_settings("use_encryption");
    QString use_hwid = restore_settings("use_hwid");

    if (key_data == "") save_settings("en_key", "");
    if (use_encryption == "") save_settings("use_encryption", "1");
    if (use_hwid == "") save_settings("use_hwid", "0");

    const QString software_label = SOFT_NAME + " " + SOFT_VERSION;

    setWindowTitle(tr("Login") + " | " + software_label);

    mainWindow.setWindowTitle(software_label);

    show();

    notification_style.insert("default", "QLabel {border: 2px solid #636872;background-color: #4A4F59;padding: 10px;border-radius:4px;}");
    notification_style.insert("error", "QLabel {border: 2px solid #9B5959;background-color: #804747;padding: 10px;border-radius:4px;}");

    ui->notification_sign_up->setStyleSheet(notification_style.value("default"));
    ui->notification_sign_up->setText(tr("Enter and confirm a password to encrypt"));

    if (mainWindow.db_exist()) {
        ui->stacked_widget->setCurrentWidget(ui->sign_in);

        ui->use_hwid->setChecked( (use_hwid == "1") ? true : false );

        if (use_encryption == "0" || use_encryption == "") { // IF DOESN'T USE ENCRYPTION

            bool window_status = mainWindow.setup_MainWindow(false, "");

            if (window_status == true) {
                mainWindow.show();
                window_data_wipe();
                hide();
            } else {
                ui->notification_sign_in->setStyleSheet(notification_style.value("error"));
                ui->notification_sign_in->setText(tr("Wrong password or other errors!"));
            }

        } else if (use_encryption == "1" && key_data != "") { // IF USE ENCRYPTION

            bool window_status = mainWindow.setup_MainWindow(true, key_data);

            if (window_status == true) {
                mainWindow.show();
                window_data_wipe();
                hide();
            } else {
                ui->notification_sign_in->setStyleSheet(notification_style.value("error"));
                ui->notification_sign_in->setText(tr("Wrong password or other errors!"));
            }

        } else if (use_encryption == "1" && key_data != "" && use_hwid == "1") { // IF USE ENCRYPTION & IF USE HWID

            bool window_status = mainWindow.setup_MainWindow(true, key_data, true);

            if (window_status == true) {
                mainWindow.show();
                window_data_wipe();
                hide();
            } else {
                ui->notification_sign_in->setStyleSheet(notification_style.value("error"));
                ui->notification_sign_in->setText(tr("Wrong password or other errors!"));
            }

        }

    } else {
        save_settings("en_key", "");
        save_settings("use_encryption", "1");
        save_settings("use_hwid", "0");

        ui->stacked_widget->setCurrentWidget(ui->sign_up);
    }

    /* SET TAB ORDER */
    // Sing in
    setTabOrder(ui->password_sign_in, ui->use_hwid);
    setTabOrder(ui->use_hwid, ui->remember_on_device_sign_in);
    setTabOrder(ui->remember_on_device_sign_in, ui->login_button);

    // Sing up
    setTabOrder(ui->new_password_sign_up, ui->repeat_password_sign_up);
    setTabOrder(ui->repeat_password_sign_up, ui->use_hwid_sign_up);
    setTabOrder(ui->use_hwid_sign_up, ui->remember_on_device_sign_up);
    setTabOrder(ui->remember_on_device_sign_up, ui->create_button);

    connect(ui->password_sign_in, &QLineEdit::returnPressed, [=]() {
        ui->login_button->click();
    });
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::window_data_wipe() {
    ui->new_password_sign_up->setText("");
    ui->repeat_password_sign_up->setText("");
    ui->password_sign_in->setText("");
}

void LoginWindow::on_login_button_clicked()
{
    QString t_password = ui->password_sign_in->text().trimmed();
    bool use_hwid = ui->use_hwid->checkState();
    bool remember_on_device = ui->remember_on_device_sign_in->checkState();

    if (use_hwid == true) t_password += QSysInfo::machineUniqueId();

    bool window_status = false;

    if (t_password != "") {
        window_status = mainWindow.setup_MainWindow(true, magic_encrypt(t_password, MAGIC));
    } else {
        window_status = mainWindow.setup_MainWindow(false);
    }

    if (window_status == true) {
        mainWindow.show();
        window_data_wipe();
        this->hide();

        login_status = true;

        if (remember_on_device == true && t_password != "") save_settings("en_key", magic_encrypt(t_password, MAGIC));

    } else {
        ui->notification_sign_in->setStyleSheet(notification_style.value("error"));
        ui->notification_sign_in->setText(tr("Wrong password or other errors!"));
    }
}

void LoginWindow::on_create_button_clicked()
{
    ui->notification_sign_up->setStyleSheet(notification_style.value("default"));
    ui->notification_sign_up->setText(tr("Enter and confirm a password to encrypt"));

    QString new_password = ui->new_password_sign_up->text().trimmed();
    QString repeat_password = ui->repeat_password_sign_up->text().trimmed();

    if (new_password == "" && repeat_password == "") { // without password

        QMessageBox::StandardButton empty_password = QMessageBox::question(this, tr("Password is empty"), tr("Are you sure you don't want to use encryption?"));
        if (empty_password == QMessageBox::No) {
            return;
        }

        save_settings("use_encryption", "0");
        save_settings("use_hwid", "0");
        save_settings("en_key", "");

        mainWindow.setup_MainWindow(false, "");
        mainWindow.show();
        window_data_wipe();
        this->hide();

        login_status = true;

        return;
    }

    if (new_password != repeat_password) { // passwords don't match

        ui->notification_sign_up->setStyleSheet(notification_style.value("error"));
        ui->notification_sign_up->setText(tr("Attention, Passwords don't match!"));

        return;
    }

    QString t_password = new_password;

    if (ui->use_hwid_sign_up->isChecked()) { // alert about "Encrypt for this device only" function

        QMessageBox::StandardButton empty_password = QMessageBox::question(this, tr("Attention"), tr("The \"Encrypt for this device only\" function means that you cannot decrypt database file with your data using another computer.\nSince encryption is done using the HWID. Are you sure?"));
        if (empty_password == QMessageBox::No) {
            ui->use_hwid_sign_up->setChecked(false);
            return;
        }

        save_settings("use_hwid", "1");
        t_password += QSysInfo::machineUniqueId();
    }

    save_settings("use_encryption", "1");

    if (ui->remember_on_device_sign_up->isChecked()) {
        save_settings("en_key", magic_encrypt(t_password, MAGIC));
    } else {
        save_settings("en_key", "");
    }

    mainWindow.setup_MainWindow(true, magic_encrypt(t_password, MAGIC));
    mainWindow.show();
    window_data_wipe();
    this->hide();

    login_status = true;
}

void LoginWindow::on_new_password_sign_up_eye_button_pressed()
{
    ui->new_password_sign_up_eye_button->setIcon(QIcon(":/img/eye-white.svg"));
    ui->new_password_sign_up->setEchoMode(QLineEdit::Normal);
}


void LoginWindow::on_new_password_sign_up_eye_button_released()
{
    ui->new_password_sign_up_eye_button->setIcon(QIcon(":/img/eye.svg"));
    ui->new_password_sign_up->setEchoMode(QLineEdit::Password);
}


void LoginWindow::on_repeat_password_sign_up_eye_button_pressed()
{
    ui->repeat_password_sign_up_eye_button->setIcon(QIcon(":/img/eye-white.svg"));
    ui->repeat_password_sign_up->setEchoMode(QLineEdit::Normal);
}


void LoginWindow::on_repeat_password_sign_up_eye_button_released()
{
    ui->repeat_password_sign_up_eye_button->setIcon(QIcon(":/img/eye.svg"));
    ui->repeat_password_sign_up->setEchoMode(QLineEdit::Password);
}


void LoginWindow::on_password_sign_in_eye_button_pressed()
{
    ui->password_sign_in_eye_button->setIcon(QIcon(":/img/eye-white.svg"));
    ui->password_sign_in->setEchoMode(QLineEdit::Normal);
}


void LoginWindow::on_password_sign_in_eye_button_released()
{
    ui->password_sign_in_eye_button->setIcon(QIcon(":/img/eye.svg"));
    ui->password_sign_in->setEchoMode(QLineEdit::Password);
}

void LoginWindow::show_me() {
    if (login_status == true) {
        if (mainWindow.isHidden()) mainWindow.show();
    } else {
        this->show();
        this->activateWindow();
    }
}
