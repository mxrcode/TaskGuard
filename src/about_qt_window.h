#ifndef ABOUT_QT_WINDOW_H
#define ABOUT_QT_WINDOW_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class about_qt_window;
}

class about_qt_window : public QDialog
{
    Q_OBJECT

public:
    explicit about_qt_window(QWidget *parent = nullptr);
    ~about_qt_window();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_pushButton_clicked();

private:
    Ui::about_qt_window *ui;
};

#endif // ABOUT_QT_WINDOW_H
