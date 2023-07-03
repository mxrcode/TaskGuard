#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QTranslator>

#include "version.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SettingsWindow(QMainWindow *parent = nullptr);
    ~SettingsWindow();

    void open_widget();

protected:
    void closeEvent(QCloseEvent *event) override;

signals:

private:
    Ui::SettingsWindow *ui;
    QMainWindow *m_parent;
};

#endif // SETTINGSWINDOW_H
