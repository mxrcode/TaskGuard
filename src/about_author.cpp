#include "about_author.h"
#include "ui_about_author.h"

about_author::about_author(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::about_author)
{
    ui->setupUi(this);
}

about_author::~about_author()
{
    delete ui;
}

void about_author::on_pushButton_clicked()
{
    this->hide();
}

void about_author::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}
