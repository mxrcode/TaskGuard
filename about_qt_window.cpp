#include "about_qt_window.h"
#include "ui_about_qt_window.h"

about_qt_window::about_qt_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::about_qt_window)
{
    ui->setupUi(this);
}

about_qt_window::~about_qt_window()
{
    delete ui;
}

void about_qt_window::on_pushButton_clicked()
{
    this->hide();
}

void about_qt_window::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}
