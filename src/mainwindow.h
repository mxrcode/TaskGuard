#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "version.h"

#include <QMainWindow>
#include <QApplication>
#include <QLocale>
#include <QString>
#include <QVector>
#include <QMap>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QGraphicsDropShadowEffect>
#include <QFont>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QFile>
#include <QListWidget>
#include <QMessageBox>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QCalendarWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTimer>
#include <QShortcut>
#include <QComboBox>
#include <QTranslator>

#include <QSystemSemaphore>
#include <QLocalSocket>
#include <QLocalServer>

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlError>

#include <QSplitter>
#include <QByteArray>
#include <QSettings>

#include <Windows.h>
#include <shlobj.h>

#include <algorithm>

#include "textindentdelegate.h"
#include "about_author.h"
#include "about_qt_window.h"

#include "string_encryption.h"

#include "encryption.h"
#include "settingswindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool db_connect();
    bool db_create();

    bool db_encrypt(QString password);
    bool db_decrypt(QString password);
    bool db_change_key(QString old_password, QString new_password);

    bool db_add_group(QString name, int allowed_delete = 1, int db_add_group = 100);
    bool db_rename_group(unsigned int group_id, QString name);
    bool db_set_group_position(unsigned int group_id, int position);
    int db_group_position(unsigned int group_id);
    bool db_rm_group(unsigned int group_id);
    bool db_add_task(int group_id = 1,
                     QString title = tr("Undefined"),
                     QString text = "",
                     QString task_status = "active",
                     int alarm = 0,
                     int alarm_time = 0,
                     int update_time = 0);
    bool db_rm_task(unsigned int task_id);
    void fill_group_list_widget (QListWidget * list_widget, QString table_name, int text_indent = 18);
    void fill_task_list_widget (QListWidget * list_widget, QString table_name, int text_indent = 18);
    void window_update();

    // CONTEXT MENU
    void show_context_menu_group(const QPoint &pos);
    void show_context_menu_task(const QPoint &pos);

    bool db_show_group(unsigned int group_id);
    bool db_show_task(unsigned int task_id);
    void on_group_item_clicked(QListWidgetItem *item);
    void on_task_item_clicked(QListWidgetItem *item);

    void auto_save();

    void groups_update();
    void tasks_update();

    QVector<QMap<QString, QVariant>> get_task_data_by_id(unsigned int task_id);

    void set_tray_icon(QSystemTrayIcon *tray_icon);

    void on_task_item_state_сhanged(QListWidgetItem* item);

    void save_splitter_state();
    void restore_splitter_state();
    void splitter_state_checker();

    long version_convert(QString version);

    QVector<int> db_get_task_list_by_group(unsigned int group_id);

    bool task_move(unsigned int task_id, unsigned int to_group_id);
    bool task_move(unsigned int task_id, unsigned int from_group_id, unsigned int to_group_id);

    QDialog* create_selection_dialog(QString title, QMap<QString, int> map, QWidget* parent = nullptr);

    bool setup_MainWindow(bool password_state = false, QString password = "", bool hwid = false);

    void create_tray_icon();
    bool is_soft_in_autorun();
    void add_soft_to_autorun();
    void remove_soft_from_autorun();

    bool db_exist();

    QIcon tray_svg = QIcon(":/img/logo-light.svg");
    QSystemTrayIcon *tray_icon = new QSystemTrayIcon(tray_svg, this);
    QMenu tray_menu;
    about_qt_window about_qt_window;
    about_author author_window;
    encryption *encryption_window;

    SettingsWindow *settings_window;

public slots:
    void handle_notification_clicked();

protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:

    void on_task_add_clicked();

    void on_add_alarm_clicked();

    void on_group_add_clicked();

    void on_task_search_clicked();

    void on_encryption_button_clicked();

    void on_settings_button_clicked();

private:
    Ui::MainWindow *ui;

    // CONTEXT MENU
    QMenu *context_menu_task_list = new QMenu(this);
    QMenu *context_menu_group_list = new QMenu(this);
    QVector<QAction*> context_action_task_list;
    QVector<QAction*> context_action_group_list;

    unsigned int m_group_current_id = 1;
    unsigned int m_task_current_id = 0;

    bool g_notifications = true;

    QMap<unsigned int, unsigned int> g_alert_cache;

    QSplitter *g_splitter = new QSplitter(Qt::Horizontal);

    int m_create_selection_dialog_cache_value = -1;

    QString g_db_path = "./user_data.db";
    QSqlDatabase db;
    QSqlQuery *db_query;
    QString g_db_password;
    bool use_encryption = false;
    bool use_hwid = false;
};

#endif // MAINWINDOW_H
