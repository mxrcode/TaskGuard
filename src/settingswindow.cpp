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
