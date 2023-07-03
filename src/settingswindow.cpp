#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QMainWindow *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingsWindow),
    m_parent(parent)
{
    ui->setupUi(this);

    const QString software_label = SOFT_NAME + " " + SOFT_VERSION;

    setWindowTitle(tr("Settings") + " | " + software_label);

    // LANGUAGE
    QMap<QString, QString> language_pack;
    language_pack.insert("system_system", tr("System"));
    language_pack.insert("en_US", "English");
    language_pack.insert("uk_UA", "Ukrainian");
    language_pack.insert("es_ES", "Spanish");
    language_pack.insert("ru_RU", "Russian");

    foreach (const QString& key, language_pack.keys()) {
        QString name = language_pack.value(key);
        ui->language_comboBox->addItem(name);
        ui->language_comboBox->setItemData(ui->language_comboBox->count() - 1, key);
    }

    QString current_lang = restore_settings("lang");

    if (current_lang == "" || current_lang.isEmpty()) {
        current_lang = "system_system";
        save_settings("lang", "system_system");
    }

    ui->language_comboBox->setCurrentText(language_pack[current_lang]);
    //\LANGUAGE
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}

void SettingsWindow::open_widget() {
    if (this->isHidden()) {
        this->show();
        this->activateWindow();
        this->raise();
    } else {
        this->hide();
    }
}

void SettingsWindow::on_save_settings_clicked()
{
    int selected_index_lang = ui->language_comboBox->currentIndex();
    QString selected_lang = ui->language_comboBox->itemData(selected_index_lang).toString();
    save_settings("lang", selected_lang);

    // Restart App
    QProcess::startDetached(qApp->applicationFilePath());
    std::exit(0);
}

