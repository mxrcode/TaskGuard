#ifndef PLUGIN_PASSGEN_H
#define PLUGIN_PASSGEN_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QTranslator>
#include <QDir>
#include <QComboBox>
#include <QFileInfoList>
#include <QMap>
#include <QSettings>
#include <QProcess>
#include <QMessageBox>
#include <QTimer>
#include <QThread>
#include <QClipboard>

#ifdef Q_OS_WIN
    #include <Windows.h>
#endif

#include "version.h"

namespace Ui {
class PluginPassGen;
}

class PluginPassGen : public QMainWindow
{
    Q_OBJECT
public:
    explicit PluginPassGen(QMainWindow *parent = nullptr);
    ~PluginPassGen();

    void open_widget();
    void restore_parameters();
    void save_parameters();
    void reset_parameters();
    void generate_password();

protected:
    void closeEvent(QCloseEvent *event) override;

signals:

private slots:
    void on_save_button_clicked();
    void on_reset_button_clicked();
    void on_gen_button_clicked();
    void on_copy_button_clicked();

private:
    Ui::PluginPassGen *ui;
    QMainWindow *m_parent;
};

#endif // PLUGIN_PASSGEN_H
