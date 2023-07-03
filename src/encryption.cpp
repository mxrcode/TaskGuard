#include "encryption.h"
#include "ui_encryption.h"

encryption::encryption(QMainWindow *parent) :
    QMainWindow(parent),
    ui(new Ui::encryption),
    m_parent(parent)
{
    ui->setupUi(this);

    const QString software_label = SOFT_NAME + " " + SOFT_VERSION;

    setWindowTitle(tr("Encryption") + " | " + software_label);

    notification_style.insert("default", "QLabel {border: 2px solid #636872;background-color: #4A4F59;padding: 10px;border-radius:4px;}");
    notification_style.insert("error", "QLabel {border: 2px solid #9B5959;background-color: #804747;padding: 10px;border-radius:4px;}");

    /* SET TAB ORDER */
    // Change Password
    setTabOrder(ui->current_change_password, ui->new_change_password);
    setTabOrder(ui->new_change_password, ui->repeat_change_password);
    setTabOrder(ui->repeat_change_password, ui->use_hwid_change_password);
    setTabOrder(ui->use_hwid_change_password, ui->remember_change_password);
    setTabOrder(ui->remember_change_password, ui->submit_change_password);

    // Encrypt Database
    setTabOrder(ui->new_password_encrypt, ui->repeat_password_encrypt);
    setTabOrder(ui->repeat_password_encrypt, ui->use_hwid_encrypt);
    setTabOrder(ui->use_hwid_encrypt, ui->remember_encrypt);
    setTabOrder(ui->remember_encrypt, ui->encrypt_button);

    // Decrypt Database
    setTabOrder(ui->current_password_decrypt, ui->use_hwid_decrypt);
    setTabOrder(ui->use_hwid_decrypt, ui->decrypt_button);
}

encryption::~encryption()
{
    delete ui;
}

void encryption::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();

    ui->current_password_decrypt->setText("");
    ui->new_password_encrypt->setText("");
    ui->repeat_password_encrypt->setText("");
}

void encryption::open_widget() {

    QString use_hwid = restore_settings("use_hwid");

    if (use_encryption == false && db_password == "") {
        ui->stacked_widget->setCurrentWidget(ui->encrypt_database);
    } else {
        ui->stacked_widget->setCurrentWidget(ui->other_action);
        ui->use_hwid_decrypt->setChecked( (use_hwid == "1") ? true : false );;
        ui->use_hwid_change_password->setChecked( (use_hwid == "1") ? true : false );;
    }

    if (this->isHidden()) {
        this->show();
        this->activateWindow();
        this->raise();
    } else {
        this->hide();
    }
}

void encryption::on_new_password_encrypt_button_pressed()
{
    ui->new_password_encrypt_button->setIcon(QIcon(":/img/eye-white.svg"));
    ui->new_password_encrypt->setEchoMode(QLineEdit::Normal);
}


void encryption::on_new_password_encrypt_button_released()
{
    ui->new_password_encrypt_button->setIcon(QIcon(":/img/eye.svg"));
    ui->new_password_encrypt->setEchoMode(QLineEdit::Password);
}

void encryption::on_repeat_password_encrypt_button_pressed()
{
    ui->repeat_password_encrypt_button->setIcon(QIcon(":/img/eye-white.svg"));
    ui->repeat_password_encrypt->setEchoMode(QLineEdit::Normal);
}

void encryption::on_repeat_password_encrypt_button_released()
{
    ui->repeat_password_encrypt_button->setIcon(QIcon(":/img/eye.svg"));
    ui->repeat_password_encrypt->setEchoMode(QLineEdit::Password);
}

void encryption::set_data(bool encryption, QString password, QSqlDatabase *db, QString db_path) {
    use_encryption = encryption;
    db_password = password;
    m_db = db;
    m_db_path = db_path;
}

void encryption::on_encrypt_button_clicked()
{

    ui->notification_encrypt->setStyleSheet(notification_style.value("default"));
    ui->notification_encrypt->setText(tr("Enter a password to encrypt the database"));

    QString new_password = ui->new_password_encrypt->text().trimmed();
    QString repeat_password = ui->repeat_password_encrypt->text().trimmed();

    if (new_password == "" && repeat_password == "") { // without password

        ui->notification_encrypt->setStyleSheet(notification_style.value("error"));
        ui->notification_encrypt->setText(tr("You must enter a password to encrypt!"));

        return;

    } else if (new_password != repeat_password) { // passwords don't match

        ui->notification_encrypt->setStyleSheet(notification_style.value("error"));
        ui->notification_encrypt->setText(tr("Attention, Passwords don't match!"));

        return;
    }

    QString t_password = new_password;

    if (ui->use_hwid_encrypt->isChecked()) { // alert about "Encrypt for this device only" function

        QMessageBox::StandardButton empty_password = QMessageBox::question(this, tr("Attention"), tr("The \"Encrypt for this device only\" function means that you cannot decrypt database file with your data using another computer.\nSince encryption is done using the HWID. Are you sure?"));
        if (empty_password == QMessageBox::No) {
            ui->use_hwid_encrypt->setChecked(false);
            return;
        }

        save_settings("use_hwid", "1");
        t_password += QSysInfo::machineUniqueId();
    }

    save_settings("use_encryption", "1");

    if (ui->remember_encrypt->isChecked()) {
        save_settings("en_key", magic_encrypt(t_password, MAGIC));
    } else {
        save_settings("en_key", "");
    }

    // Disconnect
    QString db_name = m_db->connectionName();
    m_db->close();
    QSqlDatabase::removeDatabase(db_name);

    // Encryption
    bool status = db_encrypt(t_password);

    if (status == false) {
        qInfo() << tr("Unknown bug when trying to encrypt database.");
    }

    // Restart app
    QProcess::startDetached(qApp->applicationFilePath());
    std::exit(0);
}

void encryption::on_change_password_button_clicked()
{
    ui->other_action_stacked_widget->setCurrentWidget(ui->change_password);
}

void encryption::on_decrypt_database_button_clicked()
{
    ui->other_action_stacked_widget->setCurrentWidget(ui->decrypt_database);
}

void encryption::on_current_password_decrypt_eye_button_pressed()
{
    ui->current_password_decrypt_eye_button->setIcon(QIcon(":/img/eye-white.svg"));
    ui->current_password_decrypt->setEchoMode(QLineEdit::Normal);
}

void encryption::on_current_password_decrypt_eye_button_released()
{
    ui->current_password_decrypt_eye_button->setIcon(QIcon(":/img/eye.svg"));
    ui->current_password_decrypt->setEchoMode(QLineEdit::Password);
}

void encryption::on_current_change_password_eye_button_pressed()
{
    ui->current_change_password_eye_button->setIcon(QIcon(":/img/eye-white.svg"));
    ui->current_change_password->setEchoMode(QLineEdit::Normal);
}

void encryption::on_current_change_password_eye_button_released()
{
    ui->current_change_password_eye_button->setIcon(QIcon(":/img/eye.svg"));
    ui->current_change_password->setEchoMode(QLineEdit::Password);
}

void encryption::on_new_change_password_eye_button_pressed()
{
    ui->new_change_password_eye_button->setIcon(QIcon(":/img/eye-white.svg"));
    ui->new_change_password->setEchoMode(QLineEdit::Normal);
}

void encryption::on_new_change_password_eye_button_released()
{
    ui->new_change_password_eye_button->setIcon(QIcon(":/img/eye.svg"));
    ui->new_change_password->setEchoMode(QLineEdit::Password);
}

void encryption::on_repeat_change_password_eye_button_pressed()
{
    ui->repeat_change_password_eye_button->setIcon(QIcon(":/img/eye-white.svg"));
    ui->repeat_change_password->setEchoMode(QLineEdit::Normal);
}

void encryption::on_repeat_change_password_eye_button_released()
{
    ui->repeat_change_password_eye_button->setIcon(QIcon(":/img/eye.svg"));
    ui->repeat_change_password->setEchoMode(QLineEdit::Password);
}

void encryption::on_decrypt_button_clicked()
{
    ui->notification_decrypt->setStyleSheet(notification_style.value("default"));
    ui->notification_decrypt->setText(tr("Enter a password to decrypt the database"));

    QString password = ui->current_password_decrypt->text().trimmed();

    if (ui->use_hwid_decrypt->isChecked()) {
        password += QSysInfo::machineUniqueId();
    }

    if (password == "") {
        ui->notification_decrypt->setStyleSheet(notification_style.value("error"));
        ui->notification_decrypt->setText(tr("The password field cannot be blank."));
        return;
    }

    if (password != magic_decrypt(db_password, MAGIC)) {
        ui->notification_decrypt->setStyleSheet(notification_style.value("error"));
        ui->notification_decrypt->setText(tr("You entered the wrong password."));
        return;
    }

    QMessageBox::StandardButton decrypt_alert = QMessageBox::question(this, tr("Attention"), tr("The database will be decrypted, are you sure you want to do that?\nThe data will no longer be protected!"));
    if (decrypt_alert == QMessageBox::No) {
        ui->current_password_decrypt->setText("");
        return;
    }

    // Disconnect
    QString db_name = m_db->connectionName();
    m_db->close();
    QSqlDatabase::removeDatabase(db_name);

    // Decryption
    bool status = db_decrypt(password);

    if (status == false) {
        qInfo() << tr("Unknown bug when trying to decrypt database.");
    }

    save_settings("use_encryption", "0");
    save_settings("en_key", "");
    save_settings("use_hwid", "0");

    // Restart app
    QProcess::startDetached(qApp->applicationFilePath());
    std::exit(0);
}

void encryption::on_submit_change_password_clicked()
{
    ui->notification_change_password->setStyleSheet(notification_style.value("default"));
    ui->notification_change_password->setText(tr("Enter current and new passwords"));

    QString current_password = ui->current_change_password->text().trimmed();
    QString new_password = ui->new_change_password->text().trimmed();
    QString repeat_password = ui->repeat_change_password->text().trimmed();

    if (current_password == "" || new_password == "" || repeat_password == "") {
        ui->notification_change_password->setStyleSheet(notification_style.value("error"));
        ui->notification_change_password->setText(tr("The password fields cannot be empty."));
        return;
    }

    if (new_password != repeat_password) { // passwords don't match
        ui->notification_change_password->setStyleSheet(notification_style.value("error"));
        ui->notification_change_password->setText(tr("Attention, Passwords don't match!"));
        return;
    }

    if (ui->use_hwid_change_password->isChecked()) {
        current_password += QSysInfo::machineUniqueId();
        new_password += QSysInfo::machineUniqueId();
    }

    if (current_password != magic_decrypt(db_password, MAGIC)) {
        ui->notification_change_password->setStyleSheet(notification_style.value("error"));
        ui->notification_change_password->setText(tr("You entered the wrong password."));
        return;
    }

    QMessageBox::StandardButton change_password_alert = QMessageBox::question(this, tr("Attention"), tr("The database will be encrypted using a new password, are you sure you want to do that? You will lose access to this data if you forget a new password."));
    if (change_password_alert == QMessageBox::No) {
        ui->current_change_password->setText("");
        ui->new_change_password->setText("");
        ui->repeat_change_password->setText("");
        return;
    }

    // Disconnect
    QString db_name = m_db->connectionName();
    m_db->close();
    QSqlDatabase::removeDatabase(db_name);

    // Change encryption key
    bool status = db_change_key(current_password, new_password);

    if (status == false) {
        qInfo() << tr("Unknown error when trying to change the encryption key for the database.");
    }

    save_settings("use_encryption", "1");

    if (ui->use_hwid_change_password->isChecked()) {
        save_settings("use_hwid", "1");
    } else {
        save_settings("use_hwid", "0");
    }

    if (ui->remember_change_password->isChecked()) {
        save_settings("en_key", magic_encrypt(new_password, MAGIC));
    } else {
        save_settings("en_key", "");
    }

    // Restart app
    QProcess::startDetached(qApp->applicationFilePath());
    std::exit(0);
}

bool encryption::db_encrypt(QString password) { // Encrypt an Existing Database

    QSqlDatabase db_local = QSqlDatabase::addDatabase("SQLITECIPHER");
    db_local.setDatabaseName(m_db_path);

    if (QFile::exists(m_db_path)) {
        db_local.setPassword(password);
        db_local.setConnectOptions("QSQLITE_CREATE_KEY");

        db_password = password;

    } else {
        qInfo() << tr("Database file not found.");
    }

    if (!db_local.open()) {
        qInfo() << tr("Error when trying to open a newly encrypted database!");
        return false;
    }

    return true;
}

bool encryption::db_decrypt(QString password) {

    QSqlDatabase db_local = QSqlDatabase::addDatabase("SQLITECIPHER");
    db_local.setDatabaseName(m_db_path);

    db_local.setPassword(password);
    db_local.setConnectOptions("QSQLITE_REMOVE_KEY");

    db_password = "";

    if (!db_local.open()) {
        qInfo() << tr("Error when attempting to decrypt the database!");
        return false;
    }

    return true;
}

bool encryption::db_change_key(QString old_password, QString new_password) {

    QSqlDatabase db_local = QSqlDatabase::addDatabase("SQLITECIPHER");
    db_local.setDatabaseName(m_db_path);

    if (QFile::exists(m_db_path)) {
        db_local.setPassword(old_password);
        db_local.setConnectOptions("QSQLITE_UPDATE_KEY=" + new_password);

        db_password = new_password;

    } else {
        qInfo() << tr("Database file not found.");
    }

    if (!db_local.open()) {
        qInfo() << tr("Error when trying to open the database after a password change!");
        return false;
    }

    return true;

}
