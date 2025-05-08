#include "plugin_passgen.h"
#include "ui_plugin_passgen.h"
#include <QRandomGenerator> // Added for password generation
#include <QMessageBox>    // Added for potential error messages

PluginPassGen::PluginPassGen(QMainWindow *parent) :
    QMainWindow(parent),
    ui(new Ui::PluginPassGen),
    m_parent(parent)
{
    ui->setupUi(this);

    const QString software_label = SOFT_NAME + " " + SOFT_VERSION;

    setWindowTitle(tr("Password Generator") + " | " + software_label);
    setWindowModality(Qt::NonModal);
}

PluginPassGen::~PluginPassGen()
{
    delete ui;
}

void PluginPassGen::closeEvent(QCloseEvent *event)
{
    event->ignore();
    ui->password_text_edit->clear();
    hide();
}

void PluginPassGen::open_widget() {
    if (this->isHidden()) {
        this->show();
        this->activateWindow();
        this->raise();

        this->restore_parameters();
        this->generate_password();
    } else {
        this->hide();
        ui->password_text_edit->clear();
    }
}

void PluginPassGen::restore_parameters()
{
    QString charSet = restore_settings("plugin_passgen_char_set", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()-_=+[]{}<>?/|~");
    QString passLength = restore_settings("plugin_passgen_password_length", "128");
    bool useSpecialChars = restore_settings("plugin_passgen_password_use_special_chars", "1") == "1" ? true : false;

    ui->char_set_line_edit->setText(charSet);
    ui->char_special_check_box->setChecked(useSpecialChars);
    ui->password_length->setValue(passLength.toInt());

    ui->char_set_line_edit->setCursorPosition(0); // Set cursor to the beginning of the line edit
}

void PluginPassGen::save_parameters()
{
    QString charSet = ui->char_set_line_edit->text();
    QString passLength = QString::number(ui->password_length->value());
    bool useSpecialChars = ui->char_special_check_box->isChecked() ? true : false;

    save_settings("plugin_passgen_char_set", charSet);
    save_settings("plugin_passgen_password_length", passLength);
    save_settings("plugin_passgen_password_use_special_chars", useSpecialChars ? "1" : "0");
}

void PluginPassGen::reset_parameters()
{
    QString defaultCharSet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()-_=+[]{}<>?/|~";
    QString defaultPassLength = "128";
    bool defaultUseSpecialChars = true;

    ui->char_set_line_edit->setText(defaultCharSet);
    ui->char_special_check_box->setChecked(defaultUseSpecialChars);
    ui->password_length->setValue(defaultPassLength.toInt());

    ui->char_set_line_edit->setCursorPosition(0); // Set cursor to the beginning of the line edit

    save_settings("plugin_passgen_char_set", defaultCharSet);
    save_settings("plugin_passgen_password_length", defaultPassLength);
    save_settings("plugin_passgen_password_use_special_chars", defaultUseSpecialChars ? "1" : "0");
}

void PluginPassGen::generate_password()
{
    ui->password_text_edit->clear();

    // Get parameters from UI
    QString charSet = ui->char_set_line_edit->text();
    int passLength = ui->password_length->value();
    bool useSpecialChars = ui->char_special_check_box->isChecked();

    // Input validation
    if (charSet.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Character set cannot be empty."));
        return;
    }

    if (passLength <= 0) {
        QMessageBox::warning(this, tr("Warning"), tr("Password length must be greater than 0."));
        return;
    }

    // Process the character set based on special characters checkbox
    QString finalCharSet = charSet;
    if (!useSpecialChars) {
        // Remove special characters if checkbox is unchecked
        finalCharSet.remove(QRegularExpression("[^a-zA-Z0-9]"));
        
        // Check if we have any characters left
        if (finalCharSet.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), 
                tr("No characters left after removing special characters. Using original set."));
            finalCharSet = charSet;
        }
    }

    // Reserve memory for efficiency
    QString generatedPassword;
    generatedPassword.reserve(passLength);

    // Get secure random generator instance
    QRandomGenerator *secureRandom = QRandomGenerator::system();
    
    // Generate the password
    for (int i = 0; i < passLength; ++i) {
        int randomIndex = secureRandom->bounded(finalCharSet.length());
        generatedPassword.append(finalCharSet.at(randomIndex));
    }

    // Display the generated password
    ui->password_text_edit->setPlainText(generatedPassword);
}

void PluginPassGen::on_save_button_clicked()
{
    save_parameters();
    generate_password();
}

void PluginPassGen::on_reset_button_clicked()
{
    reset_parameters();
    generate_password();
}


void PluginPassGen::on_gen_button_clicked()
{
    generate_password();
}


void PluginPassGen::on_copy_button_clicked()
{
    QString password = ui->password_text_edit->toPlainText();
    if (!password.isEmpty()) {
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(password);
        QMessageBox::information(this, tr("Copied"), tr("Password copied to clipboard."));
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("No password to copy."));
    }
}

