#ifndef ABOUT_AUTHOR_H
#define ABOUT_AUTHOR_H

#include "version.h"

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class about_author;
}

class about_author : public QDialog
{
    Q_OBJECT

public:
    explicit about_author(QWidget *parent = nullptr);
    ~about_author();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_pushButton_clicked();

private:
    Ui::about_author *ui;
};

#endif // ABOUT_AUTHOR_H
