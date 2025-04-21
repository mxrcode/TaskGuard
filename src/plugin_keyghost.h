#ifndef PLUGIN_KEYGHOST_H
#define PLUGIN_KEYGHOST_H

// KeyGhost Plugin
// A plugin for sending keystrokes to other applications.
// This file is part of KeyGhost.
// Github: https://github.com/mxrcode/KeyGhost/

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

#ifdef Q_OS_WIN
    #include <Windows.h>
#endif

#include "version.h"

namespace Ui {
class PluginKeyGhost;
}

class PluginKeyGhost : public QMainWindow
{
    Q_OBJECT
public:
    explicit PluginKeyGhost(QMainWindow *parent = nullptr);
    ~PluginKeyGhost();

    void sendKeystroke();
    void sendText(const QString &text);

    void open_widget();

protected:
    void closeEvent(QCloseEvent *event) override;

signals:

private slots:
    void on_text_typing_clicked();

private:
    Ui::PluginKeyGhost *ui;
    QMainWindow *m_parent;
};

#endif // PLUGIN_KEYGHOST_H
