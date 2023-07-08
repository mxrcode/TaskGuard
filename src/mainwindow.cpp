#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = QSqlDatabase::addDatabase("SQLITECIPHER");
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::setup_MainWindow(bool password_state, QString password, bool hwid) {

    use_encryption = password_state;
    use_hwid = hwid;
    g_db_password = password;

    bool db_status = db_connect();

    if (db_status == false) {
        return db_status;
    }

    create_tray_icon();

    encryption_window = new encryption(this);
    settings_window = new SettingsWindow(this);

    update_checker = new UpdateChecker(this);
    update_checker->check();

    setWindowIcon(QIcon(":/img/logo-dark.svg"));

    ui->task_groups_frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->task_list_frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->task_view_frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    fill_group_list_widget(ui->group_list, "task_groups", 18);

    // Set selected item by default
    QListWidgetItem *selected_item = ui->group_list->item(0);
    ui->group_list->setCurrentItem(selected_item);

    fill_task_list_widget(ui->task_list, "task_list", 18);

    /* QListWidget : Context Menu */
    // Group : Action : Rename Selected
    context_action_group_list.push_back(new QAction(tr("Rename Selected"), this));
    connect(context_action_group_list.at(0), &QAction::triggered, this, [&]() {

        QList<QListWidgetItem*> selected = ui->group_list->selectedItems();
        if (selected.isEmpty()) {
            return;
        }

        for (QListWidgetItem *item : selected) {

            if (item->data(Qt::UserRole).toUInt() == 1) {
                continue;
            }

            QDialog confirmation_dialog(this);
            confirmation_dialog.setFixedWidth(300);
            confirmation_dialog.setWindowTitle(tr("Enter a new group name"));

            QLineEdit *line_edit = new QLineEdit(&confirmation_dialog);
            line_edit->setText(item->text());
            QVBoxLayout *layout = new QVBoxLayout;
            layout->addWidget(line_edit);
            confirmation_dialog.setLayout(layout);

            QDialogButtonBox *button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &confirmation_dialog);

            layout->addWidget(button_box);

            connect(button_box, &QDialogButtonBox::accepted, &confirmation_dialog, &QDialog::accept);
            connect(button_box, &QDialogButtonBox::rejected, &confirmation_dialog, &QDialog::reject);

            int result = confirmation_dialog.exec();

            if (result == QDialog::Accepted) {
                QString name = line_edit->text();
                if (name != "") {
                    db_rename_group(item->data(Qt::UserRole).toUInt(), name);
                }

                window_update();
            }

            disconnect(button_box, &QDialogButtonBox::accepted, &confirmation_dialog, &QDialog::accept);
            disconnect(button_box, &QDialogButtonBox::rejected, &confirmation_dialog, &QDialog::reject);
        }
    });
    context_menu_group_list->addAction(context_action_group_list.at(0));

    // Group : Action : Change Position
    context_action_group_list.push_back(new QAction(tr("Change Position"), this));
    connect(context_action_group_list.at(1), &QAction::triggered, this, [&]() {

        QList<QListWidgetItem*> selected = ui->group_list->selectedItems();
        if (selected.isEmpty()) {
            return;
        }

        for (QListWidgetItem *item : selected) {

            if (item->data(Qt::UserRole).toUInt() == 1) {
                continue;
            }

            QDialog confirmation_dialog(this);
            confirmation_dialog.setFixedWidth(300);
            confirmation_dialog.setWindowTitle(tr("Specify new position"));

            QLineEdit *line_edit = new QLineEdit(&confirmation_dialog);
            line_edit->setText(QString::number( db_group_position(item->data(Qt::UserRole).toUInt()) ));
            QVBoxLayout *layout = new QVBoxLayout;
            layout->addWidget(line_edit);
            confirmation_dialog.setLayout(layout);

            QDialogButtonBox *button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &confirmation_dialog);

            layout->addWidget(button_box);

            connect(button_box, &QDialogButtonBox::accepted, &confirmation_dialog, &QDialog::accept);
            connect(button_box, &QDialogButtonBox::rejected, &confirmation_dialog, &QDialog::reject);

            int result = confirmation_dialog.exec();

            if (result == QDialog::Accepted) {
                QString str_pos = line_edit->text();
                int position = 100;

                if (str_pos != "") {
                    try {
                        QVariant convert_test(str_pos);

                        if (convert_test.canConvert<int>() && str_pos.toInt() != 0) {
                            position = str_pos.toInt();
                            db_set_group_position(item->data(Qt::UserRole).toUInt(), position);
                        } else {
                            QMessageBox::warning(nullptr, tr("Failure"), tr("Conversion failed: ") + str_pos + tr(" is not a valid integer.\nAlso, don't use 0."), QMessageBox::Ok);
                        }
                    } catch (const std::exception& e) {
                        QMessageBox::warning(nullptr, "Exception", e.what(), QMessageBox::Ok);
                        qInfo() << "Exception occurred: " << e.what();
                    }
                }

                window_update();
            }

            disconnect(button_box, &QDialogButtonBox::accepted, &confirmation_dialog, &QDialog::accept);
            disconnect(button_box, &QDialogButtonBox::rejected, &confirmation_dialog, &QDialog::reject);
        }
    });
    context_menu_group_list->addAction(context_action_group_list.at(1));

    // Group : Action : Delete Selected
    context_action_group_list.push_back(new QAction(tr("Delete selected"), this));
    connect(context_action_group_list.at(2), &QAction::triggered, this, [&]() {

        QList<QListWidgetItem*> selected = ui->group_list->selectedItems();
        if (selected.isEmpty()) {
            return;
        }
        QMessageBox::StandardButton confirm_delete = QMessageBox::question(this, tr("Delete Group"), tr("Are you sure you want to DELETE the selected group?"));
        if (confirm_delete == QMessageBox::No) {
            return;
        }

        for (QListWidgetItem *item : selected) {

            if (item->data(Qt::UserRole).toUInt() == 1) {
                continue;
            }

            if (item->isSelected()) {
                m_group_current_id = 1;
            }

            db_rm_group(item->data(Qt::UserRole).toUInt());

            ui->group_list->removeItemWidget(item);
        }

        window_update();
    });
    context_menu_group_list->addAction(context_action_group_list.at(2));

    ui->group_list->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->group_list, &QListWidget::customContextMenuRequested, this, &MainWindow::show_context_menu_group);

    // Task : Action : Copy Task
    context_action_task_list.push_back(new QAction(tr("Copy selected"), this));
    connect(context_action_task_list.at(0), &QAction::triggered, this, [&]() {

        QList<QListWidgetItem*> selected = ui->task_list->selectedItems();
        if (selected.isEmpty()) {
            return;
        }
        QMessageBox::StandardButton confirm_copy = QMessageBox::question(this, tr("Copy task"), tr("Are you sure you want to COPY this task?"));
        if (confirm_copy == QMessageBox::No) {
            return;
        }

        for (QListWidgetItem *item : selected) {

            unsigned int task_id = item->data(Qt::UserRole).toUInt();

            QVector<QMap<QString, QVariant>> item_map = get_task_data_by_id(task_id);
            QDateTime current_data = QDateTime::currentDateTime();

            db_add_task(item_map.at(0).value("group_id").toInt(),
                        "Copy - " + item_map.at(0).value("title").toString(),
                        item_map.at(0).value("text").toString(),
                        item_map.at(0).value("task_status").toString(),
                        item_map.at(0).value("alarm").toInt(),
                        item_map.at(0).value("alarm_time").toInt(),
                        current_data.toSecsSinceEpoch());

            window_update();
        }
    });
    context_menu_task_list->addAction(context_action_task_list.at(0));

    // Task : Action : Move Task
    context_action_task_list.push_back(new QAction(tr("Move selected"), this));
    connect(context_action_task_list.at(1), &QAction::triggered, this, [&]() {

        QList<QListWidgetItem*> selected = ui->task_list->selectedItems();
        if (selected.isEmpty()) {
            return;
        }

        for (QListWidgetItem *item : selected) {

            QMap<QString, int> groups;

            QSqlQuery db_query;

            db_query.prepare("SELECT * FROM task_groups WHERE group_status = :request_status ORDER BY position ASC");
            db_query.bindValue(":request_status", "active");

            if (db_query.exec()) {
                while (db_query.next()) {
                    groups.insert(db_query.value("name").toString(), db_query.value("id").toUInt());
                }
            } else {
                qInfo() << "Error when trying to get a list of active groups from the database!";
            }

            QDialog* dialog = create_selection_dialog(tr("Choose a new group"), groups);

            if (dialog->exec() == QDialog::Accepted)
            {
                if (m_create_selection_dialog_cache_value != -1) {
                    task_move(item->data(Qt::UserRole).toUInt(), m_create_selection_dialog_cache_value);
                }
            }

            delete dialog;

            window_update();

        }

    });
    context_menu_task_list->addAction(context_action_task_list.at(1));

    // Task : Action : Delete Selected
    context_action_task_list.push_back(new QAction(tr("Delete selected"), this));
    connect(context_action_task_list.at(2), &QAction::triggered, this, [&]() {

        QList<QListWidgetItem*> selected = ui->task_list->selectedItems();
        if (selected.isEmpty()) {
            return;
        }
        QMessageBox::StandardButton confirm_delete = QMessageBox::question(this, tr("Delete Task"), tr("Are you sure you want to DELETE the selected task?"));
        if (confirm_delete == QMessageBox::No) {
            return;
        }

        for (QListWidgetItem *item : selected) {
            if (item->isSelected()) {
                m_task_current_id = 0;

                ui->task_title->setText("");
                ui->task_edit->setText("");

                ui->task_title->setToolTip("");
            }

            db_rm_task(item->data(Qt::UserRole).toUInt());
            delete item;

            if (ui->task_list->count() == 0) ui->task_view_frame->hide();
        }
    });
    context_menu_task_list->addAction(context_action_task_list.at(2));

    ui->task_list->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->task_list, &QListWidget::customContextMenuRequested, this, &MainWindow::show_context_menu_task);


    connect(ui->group_list, &QListWidget::currentItemChanged, this, &MainWindow::on_group_item_clicked);
    connect(ui->task_list, &QListWidget::currentItemChanged, this, &MainWindow::on_task_item_clicked);

    ui->task_view_frame->hide(); // Hide task_view frame : Show when a task is selected

    // Cron
    QTimer* cron_timer = new QTimer(this);
    cron_timer->setInterval(60000);

    connect(cron_timer, &QTimer::timeout, this, [&](){
        if (g_notifications == true && tray_icon != nullptr) {

            QDateTime current_data = QDateTime::currentDateTime();
            unsigned int current_timestamp = current_data.toSecsSinceEpoch();

            QVector<QMap<QString, QVariant>> task_map;

            QSqlQuery db_query;

            db_query.prepare("SELECT * FROM task_list WHERE alarm_time != :uint_null AND alarm_time <= :current_timestamp AND task_status = :request_status ORDER BY alarm_time DESC");
            db_query.bindValue(":uint_null", QVariant(0));
            db_query.bindValue(":current_timestamp", current_timestamp);
            db_query.bindValue(":request_status", "active");

            if (!db_query.exec())
            {
                qInfo() << "An error occurred when selecting by CRON actual tasks from the database!";
                return;
            }

            while (db_query.next()) {
                QMap<QString, QVariant> t_map;
                t_map.insert("id", db_query.value("id"));
                t_map.insert("group_id", db_query.value("group_id"));
                t_map.insert("title", db_query.value("title"));
                t_map.insert("text", db_query.value("text"));
                t_map.insert("task_status", db_query.value("task_status"));
                t_map.insert("alarm", db_query.value("alarm"));
                t_map.insert("alarm_time", db_query.value("alarm_time"));
                t_map.insert("update_time", db_query.value("update_time"));
                t_map.insert("create_time", db_query.value("create_time"));
                task_map.push_back(t_map);

                unsigned int t_task_id = db_query.value("id").toUInt();
                unsigned int t_alarm_time = db_query.value("alarm_time").toUInt();

                if(!g_alert_cache.contains(t_task_id)){
                    g_alert_cache.insert(t_task_id, t_alarm_time);
                }
            }

            for (int i = 0; i < task_map.size(); ++i) {
                if (current_timestamp >= g_alert_cache.value(task_map.at(i).value("id").toUInt())) { // Repeat alert, every 30 minutes
                    g_alert_cache[task_map.at(i).value("id").toUInt()] += 1800; // 1800 sec = 30 min
                    tray_icon->showMessage(task_map.at(i).value("title").toString(), "", QIcon(":/img/logo-light.svg"), 15000);
                }
            }
        }
        save_splitter_state(); // QSplitter
    });

    cron_timer->start();

    connect(ui->task_list, &QListWidget::itemChanged, this, &MainWindow::on_task_item_state_сhanged);

    // Part of the code responsible for resizing frames.
    g_splitter->addWidget(ui->task_groups_frame);
    g_splitter->addWidget(ui->task_list_frame);
    g_splitter->addWidget(ui->task_view_frame);

    g_splitter->setStretchFactor(0, 2);
    g_splitter->setStretchFactor(1, 5);
    g_splitter->setStretchFactor(2, 5);

    setCentralWidget(g_splitter);

    splitter_state_checker();

    ui->task_search_edit->hide();

    ui->task_title->setPlaceholderText(tr("Title..."));
    ui->task_edit->setPlaceholderText(tr("Description..."));

    show();
    isActiveWindow();
    raise();

    return db_status;
}

long MainWindow::version_convert(QString version) {
    QStringList parts = version.split(".");
    int major = parts.at(0).toInt();
    int minor = parts.at(1).toInt();
    int patch = parts.at(2).toInt();
    return major * 1000000 + minor * 10000 + patch * 100;
}

void MainWindow::handle_notification_clicked() {
    show();
    raise();
}

QDialog* MainWindow::create_selection_dialog(QString title, QMap<QString, int> map, QWidget* parent) {

    m_create_selection_dialog_cache_value = -1;

    QDialog* dialog = new QDialog(parent);
    dialog->setWindowTitle(title);

    dialog->setMinimumWidth(260);

    QComboBox* combo_box = new QComboBox(dialog);
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        combo_box->addItem(it.key());
    }

    QPushButton* ok_button = new QPushButton(tr("OK"), dialog);
    QPushButton* cancel_button = new QPushButton(tr("Cancel"), dialog);

    connect(ok_button, &QPushButton::clicked, [=]() {
        int data = map.value(combo_box->currentText());

        m_create_selection_dialog_cache_value = data;

        dialog->accept();
    });
    connect(cancel_button, &QPushButton::clicked, dialog, &QDialog::reject);

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->addWidget(combo_box);
    layout->addWidget(ok_button);
    layout->addWidget(cancel_button);

    dialog->setLayout(layout);

    return dialog;
}

void MainWindow::save_splitter_state() {
    QSettings settings("mxrcode", SOFT_NAME);

    QByteArray state = g_splitter->saveState();
    settings.setValue("splitter_state", state);
}

void MainWindow::restore_splitter_state() {
    QSettings settings("mxrcode", SOFT_NAME);

    QByteArray state = settings.value("splitter_state").toByteArray();
    g_splitter->restoreState(state);
}

void MainWindow::splitter_state_checker() {
    QSettings settings("mxrcode", SOFT_NAME);

    if (settings.contains("splitter_state")) {
        restore_splitter_state();
    } else {
        save_splitter_state();
    }
}

void MainWindow::on_task_item_state_сhanged(QListWidgetItem* item) {

    QDateTime current_data = QDateTime::currentDateTime();

    QString task_status;

    if (item->checkState() == Qt::Checked) {
        task_status = "done";
    } else {
        task_status = "active";
    }

    QSqlQuery db_query;

    db_query.prepare("UPDATE task_list SET task_status = :set_status, update_time = :current_time WHERE id = :task_id");
    db_query.bindValue(":task_id", item->data(Qt::UserRole).toUInt());
    db_query.bindValue(":set_status", task_status);
    db_query.bindValue(":current_time", current_data.toSecsSinceEpoch());
    db_query.exec();

    window_update();
}

void MainWindow::set_tray_icon(QSystemTrayIcon* tray_icon) {
    connect(tray_icon, &QSystemTrayIcon::messageClicked, this, &MainWindow::handle_notification_clicked);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}

void MainWindow::fill_group_list_widget(QListWidget* list_widget, QString table_name, int text_indent) {

    QSqlQuery db_query;

    db_query.prepare("SELECT * FROM " + table_name + " WHERE group_status = :status_active ORDER BY position, create_time ASC");
    db_query.bindValue(":status_active", "active");

    db_query.exec();

    list_widget->setItemDelegate(new TextIndentDelegate(text_indent, list_widget)); // Text indent

    QVector<QVector<QVariant>> db_items;

    while (db_query.next()) {
        db_items.push_back({db_query.value(0), db_query.value(2)});
    }

    for (int i = 0; i < db_items.size(); i++) {
        QListWidgetItem *item = new QListWidgetItem(db_items[i].at(1).toString());
        item->setData(Qt::UserRole, db_items[i].at(0).toUInt());
        list_widget->addItem(item);
    }
}

bool compare_item_maps_uint(const QMap<QString, QVariant>& a, const QMap<QString, QVariant>& b) {
    return a["update_time"].toUInt() < b["update_time"].toUInt();
}

void MainWindow::fill_task_list_widget(QListWidget* list_widget, QString table_name, int text_indent) {

    QSqlQuery db_query;

    if (m_group_current_id == 1) {
        db_query.prepare("SELECT * FROM " + table_name + " WHERE task_status = :status_active OR task_status = :status_done ORDER BY alarm_time, create_time DESC");
    } else {
        db_query.prepare("SELECT * FROM " + table_name + " WHERE group_id = :group_current_id AND (task_status = :status_active OR task_status = :status_done) ORDER BY alarm_time, create_time DESC");
    }

    db_query.bindValue(":status_active", "active");
    db_query.bindValue(":status_done", "done");
    db_query.bindValue(":group_current_id", m_group_current_id);

    db_query.exec();

    // list_widget->setItemDelegate(new TextIndentDelegate(text_indent, list_widget)); // Text indent

    QVector<QMap<QString, QVariant>> item_map;

    while (db_query.next()) {
        QMap<QString, QVariant> t_map;
        t_map.insert("id", db_query.value("id"));
        t_map.insert("group_id", db_query.value("group_id"));
        t_map.insert("title", db_query.value("title"));
        t_map.insert("text", db_query.value("text"));
        t_map.insert("task_status", db_query.value("task_status"));
        t_map.insert("alarm", db_query.value("alarm"));
        t_map.insert("alarm_time", db_query.value("alarm_time"));
        t_map.insert("update_time", db_query.value("update_time"));
        t_map.insert("create_time", db_query.value("create_time"));
        item_map.push_back(t_map);
    }

    for (int i = 0; i < item_map.size(); i++) { // Active

        QDateTime alarm_time;
        QString format_create_time;
        unsigned int int_alarm_time = item_map[i].value("alarm_time").toUInt();

        QString item_title = item_map[i].value("title").toString();

        if (int_alarm_time > 0) {
            alarm_time.setSecsSinceEpoch(int_alarm_time);
            format_create_time = alarm_time.toString("[dd/MM] ");

            item_title.prepend(format_create_time);
        }

        QListWidgetItem *item = new QListWidgetItem(item_title);
        item->setData(Qt::UserRole, item_map[i].value("id").toUInt());

        if (item_map[i].value("task_status").toString() == "active") {

            item->setCheckState(Qt::Unchecked);
        } else {
            continue;
        }
        list_widget->addItem(item);
    }

    std::sort(item_map.begin(), item_map.end(), compare_item_maps_uint);

    for (int i = item_map.size()-1; i >= 0; i--) { // Done

        if (item_map[i].value("task_status").toString() != "done") {
            continue;
        }

        QDateTime alarm_time;
        QString format_create_time;
        unsigned int int_alarm_time = item_map[i].value("alarm_time").toUInt();

        QString item_title = item_map[i].value("title").toString();

        if (int_alarm_time > 0) {
            alarm_time.setSecsSinceEpoch(int_alarm_time);
            format_create_time = alarm_time.toString("[dd/MM] ");

            item_title.prepend(format_create_time);
        }

        QListWidgetItem *item = new QListWidgetItem(item_title);
        item->setData(Qt::UserRole, item_map[i].value("id").toUInt());

        item->setForeground(Qt::gray);
        item->setCheckState(Qt::Checked);
        QFont t_font = item->font();
        t_font.setStrikeOut(true);
        item->setFont(t_font);

        list_widget->addItem(item);
    }
}

bool MainWindow::db_connect() {

    /*
     * SQLITECIPHER
     * Github:     https://github.com/devbean/QtCipherSqlitePlugin/
     * LGPL-2.1:   https://github.com/devbean/QtCipherSqlitePlugin/blob/develop/License
     * Release:    https://github.com/devbean/QtCipherSqlitePlugin/releases/tag/v1.3
     * How-to-use: https://github.com/devbean/QtCipherSqlitePlugin/wiki/How-to-use
    */

    db.setDatabaseName(g_db_path);

    QString t_password = magic_decrypt(g_db_password, MAGIC);

    if (use_hwid == true) t_password += QSysInfo::machineUniqueId();

    if (use_encryption == true) {
        if (g_db_password != "") {
            db.setPassword(t_password);
        } else {
            // qInfo() << "No password is specified in the variable, the database will be created without using encryption.";
            return false;
        }
    }

    bool creation_required = false;

    if (!QFile::exists(g_db_path)) {
        creation_required = true;
    }

    if (!db.open()) {
        qInfo() << "Error when opening the database: " << db.lastError().driverText();
        return false;
    }

    if (creation_required == true) db_create();

    QSqlQuery check_query;
    check_query.exec("SELECT * FROM internal_data WHERE id = 1");
    bool db_connected = false;
    while (check_query.next()) {
        db_connected = true;
    }
    if (db_connected == false) return db_connected;

    return true;
}

bool MainWindow::db_exist() {
    if (QFile::exists(g_db_path)) {
        return true;
    } else {
        return false;
    }
}

bool MainWindow::db_create() {
    bool status = true;

    db_query = new QSqlQuery();

    db_query->exec("CREATE TABLE task_groups (id INTEGER PRIMARY KEY, group_status TEXT DEFAULT 'active', name TEXT DEFAULT 'no_name', allowed_delete INTEGER DEFAULT 1, create_time INTEGER DEFAULT 1, position INTEGER DEFAULT 1);");

    db_query->exec("CREATE TABLE task_list (id INTEGER PRIMARY KEY, group_id INTEGER DEFAULT 1, title TEXT DEFAULT 'Undefined', text TEXT DEFAULT 'New task', task_status TEXT DEFAULT 'active', alarm INTEGER DEFAULT 1, alarm_time INTEGER DEFAULT 1, update_time INTEGER DEFAULT 1, create_time INTEGER DEFAULT 1);");

    db_query->exec("CREATE TABLE internal_data (id INTEGER PRIMARY KEY, version INTEGER DEFAULT 1);");
    db_query->exec("INSERT INTO internal_data (version) VALUES (" + QString::number(version_convert(SOFT_VERSION)) + ")");

    status = db_add_group("All tasks", 0, 1);

    return status;
}

bool MainWindow::db_add_group(QString name, int allowed_delete, int position) {

    QDateTime data_time = QDateTime::currentDateTime();
    int timestamp = data_time.toSecsSinceEpoch();

    QSqlQuery db_query;
    db_query.prepare("INSERT INTO task_groups (name, group_status, allowed_delete, create_time, position) VALUES (:name, :group_status, :allowed_delete, :create_time, :group_position)");
    db_query.bindValue(":name", name);
    db_query.bindValue(":group_status", "active");
    db_query.bindValue(":allowed_delete", allowed_delete);
    db_query.bindValue(":create_time", timestamp);
    db_query.bindValue(":group_position", position);

    if (!db_query.exec()) {
        qInfo() << "Failed to add the row \"" + name + "\" groups to the database!";
        return false;
    }

    return true;
}

bool MainWindow::db_rename_group(unsigned int group_id, QString name) {

    QSqlQuery db_query;
    db_query.prepare("UPDATE task_groups SET name = :name WHERE id = :group_id");
    db_query.bindValue(":group_id", group_id);
    db_query.bindValue(":name", name);

    if (!db_query.exec()) {
        qInfo() << "Failed to change the string to \"" + name + "\" for groups in the database!";
        return false;
    }

    return true;
}

bool MainWindow::db_set_group_position(unsigned int group_id, int position) {

    QSqlQuery db_query;
    db_query.prepare("UPDATE task_groups SET position = :set_position WHERE id = :group_id");
    db_query.bindValue(":group_id", group_id);
    db_query.bindValue(":set_position", position);

    if (!db_query.exec()) {
        qInfo() << "Failed to change the position to \"" + QString::number(position) + "\" for groups in the database!";
        return false;
    }

    return true;
}

int MainWindow::db_group_position(unsigned int group_id) {

    int position = -1;

    QSqlQuery db_query;
    db_query.prepare("SELECT position FROM task_groups WHERE id = :group_id");
    db_query.bindValue(":group_id", group_id);

    if (!db_query.exec()) {
        qInfo() << "Failed to get the group position by id \"" + QString::number(group_id) + "\" from the database!";
        return position;
    } else {
        while (db_query.next()) {
            position = db_query.value("position").toInt();
        }
    }

    return position;
}

bool MainWindow::task_move(unsigned int task_id, unsigned int to_group_id) {

    QSqlQuery db_query;

    db_query.prepare("UPDATE task_list SET group_id = :new_id WHERE id = :requested_task_id");
    db_query.bindValue(":requested_task_id", task_id);
    db_query.bindValue(":new_id", to_group_id);

    if (!db_query.exec()) {
        qInfo() << "Error when moving task with id \"" + QString::number(task_id) + "\" in database.";
        return false;
    }

    return true;
}

bool MainWindow::task_move(unsigned int task_id, unsigned int from_group_id, unsigned int to_group_id) {

    QSqlQuery db_query;

    db_query.prepare("UPDATE task_list SET group_id = :new_id WHERE id = :requested_task_id AND group_id = :requested_group_id");
    db_query.bindValue(":requested_task_id", task_id);
    db_query.bindValue(":requested_group_id", from_group_id);
    db_query.bindValue(":new_id", to_group_id);

    if (!db_query.exec()) {
        qInfo() << "Error when moving task with id \"" + QString::number(task_id) + "\" in database.";
        return false;
    }

    return true;
}

QVector<int> MainWindow::db_get_task_list_by_group(unsigned int group_id) {

    QVector<int> response;

    QSqlQuery db_query;

    db_query.prepare("SELECT id FROM task_list WHERE group_id = :requested_id");
    db_query.bindValue(":requested_id", group_id);
    db_query.exec();

    while (db_query.next()) {
        response.push_back(db_query.value("id").toUInt());
    }

    return response;
}

bool MainWindow::db_rm_group(unsigned int group_id) {

    QVector<int> task_list_id = db_get_task_list_by_group(group_id);

    for (int i = 0; i < task_list_id.size(); ++i) {
        task_move(task_list_id[i], group_id, 1);
    }

    QSqlQuery db_query;

    db_query.prepare("UPDATE task_groups SET group_status = :set_status WHERE id = :requested_id");
    db_query.bindValue(":requested_id", group_id);
    db_query.bindValue(":set_status", "deleted");

    if (!db_query.exec()) {
        qInfo() << "Error when deleting an item with id \"" + QString::number(group_id) + "\" from the database.";
        return false;
    }

    return true;
}

bool MainWindow::db_add_task(int group_id,
                             QString title,
                             QString text,
                             QString task_status,
                             int alarm,
                             int alarm_time,
                             int update_time)
{

    QDateTime data_time = QDateTime::currentDateTime();
    int timestamp = data_time.toSecsSinceEpoch();

    QSqlQuery db_query;
    db_query.prepare("INSERT INTO task_list (group_id, title, text, task_status, alarm, alarm_time, update_time, create_time) VALUES (:group_id, :title, :text, :task_status, :alarm, :alarm_time, :update_time, :create_time)");
    db_query.bindValue(":group_id", group_id);
    db_query.bindValue(":title", title);
    db_query.bindValue(":text", text);
    db_query.bindValue(":task_status", task_status);
    db_query.bindValue(":alarm", alarm);
    db_query.bindValue(":alarm_time", alarm_time);
    db_query.bindValue(":update_time", update_time);
    db_query.bindValue(":create_time", timestamp);

    if (!db_query.exec()) {
        qInfo() << "Failed to add the task \"" + title + "\" to the database.!";
        return false;
    }

    return true;
}

bool MainWindow::db_rm_task(unsigned int task_id) {

    QSqlQuery db_query;

    // db_query.prepare("DELETE FROM task_list WHERE id = :task_id");
    // db_query.bindValue(":task_id", task_id);

    db_query.prepare("UPDATE task_list SET task_status = :set_status WHERE id = :task_id");
    db_query.bindValue(":task_id", task_id);
    db_query.bindValue(":set_status", "deleted");

    if (!db_query.exec()) {
        qInfo() << "Error when deleting an item with id \"" + QString::number(task_id) + "\" from the database.";
        return false;
    }

    return true;
}

bool MainWindow::db_show_task(unsigned int task_id) {

    QSqlQuery db_query;

    db_query.prepare("SELECT text, title, update_time, create_time, alarm_time FROM task_list WHERE id = :task_id");
    db_query.bindValue(":task_id", task_id);

    if (db_query.exec())
    {
        if (db_query.next()) {
            QString title = db_query.value("title").toString();
            QString text = db_query.value("text").toString();

            ui->task_title->setText(title);
            ui->task_edit->setText(text);

            ui->task_title->setToolTip(title);

            QDateTime create_time;
            QDateTime update_time;
            QDateTime alert_time;

            create_time.setSecsSinceEpoch(db_query.value("create_time").toUInt());
            update_time.setSecsSinceEpoch(db_query.value("update_time").toUInt());
            alert_time.setSecsSinceEpoch(db_query.value("alarm_time").toUInt());

            QString format_create_time = create_time.toString("dd/MM/yyyy hh:mm");
            QString format_update_time = update_time.toString("dd/MM/yyyy hh:mm");
            QString format_alert_time = alert_time.toString("dd/MM/yyyy hh:mm");

            ui->created_data->setText(format_create_time);
            ui->updated_data->setText(format_update_time);
            ui->alert_data->setText(format_alert_time);

            if (db_query.value("update_time").toUInt() == 0) {
                ui->updated_text->hide();
                ui->updated_data->hide();
            } else {
                ui->updated_text->show();
                ui->updated_data->show();
            }

            if (db_query.value("alarm_time").toUInt() == 0) {
                ui->alert_text->hide();
                ui->alert_data->hide();
            } else {
                ui->alert_text->show();
                ui->alert_data->show();
            }
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }

    return true;
}

bool MainWindow::db_show_group(unsigned int group_id) {

    QSqlQuery db_query;

    db_query.prepare("SELECT * FROM group_list WHERE id = :group_id");
    db_query.bindValue(":group_id", group_id);

    if (db_query.exec())
    {
        if (db_query.next()) {
            QString title = db_query.value("title").toString();
            QString text = db_query.value("text").toString();
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }

    return true;
}

void MainWindow::window_update() {
    ui->group_list->blockSignals(true);
    ui->task_list->blockSignals(true);

    groups_update();
    tasks_update();

    ui->group_list->blockSignals(false);
    ui->task_list->blockSignals(false);
}

void MainWindow::groups_update() {
    int current_row = ui->group_list->currentRow();

    ui->group_list->clear();
    fill_group_list_widget(ui->group_list, "task_groups", 18);

    if (current_row >= 0) {
        ui->group_list->setCurrentRow(current_row);
    }
}

void MainWindow::tasks_update() {
    int current_row = ui->task_list->currentRow();

    ui->task_list->clear();
    fill_task_list_widget(ui->task_list, "task_list", 18);

    if (current_row >= 0) {
        ui->task_list->setCurrentRow(current_row);
    }
}

void MainWindow::on_task_add_clicked()
{
    QModelIndex current_index = ui->task_list->currentIndex();
    int row = current_index.row();

    db_add_task(m_group_current_id);
    window_update();

    if (current_index.isValid()) {
        ui->task_list->setCurrentRow(row);
    }
}

/* QListWidget : Context Menu */
void MainWindow::show_context_menu_group(const QPoint &pos)
{
    QListWidgetItem *item = ui->group_list->itemAt(pos);
    if (item && item->data(Qt::UserRole).toUInt() == 1) {
        return;
    }

    QPoint global_pos = ui->group_list->mapToGlobal(pos);

    QAction *selected_item = context_menu_group_list->exec(global_pos);

    if (selected_item == context_action_group_list.at(0)) {
        // qInfo() << "Action 1 is complete";
    }
}

void MainWindow::show_context_menu_task(const QPoint &pos)
{
    QPoint global_pos = ui->task_list->mapToGlobal(pos);

    QAction *selected_item = context_menu_task_list->exec(global_pos);

    if (selected_item == context_action_task_list.at(0)) {
        // qInfo() << "Action 1 is complete";
    }
}

void MainWindow::on_group_item_clicked(QListWidgetItem *item) {

    unsigned int group_id;

    if (item != 0x0) {
        group_id = item->data(Qt::UserRole).toUInt();
    } else {
        group_id = 0;
    }

    if (group_id > 0) {
        m_group_current_id = group_id;
        window_update();
    }

}

void MainWindow::on_task_item_clicked(QListWidgetItem *item) {

    unsigned int task_id;

    if (item != 0x0) {
        task_id = item->data(Qt::UserRole).toUInt();
    } else {
        task_id = 0;
    }

    if (task_id > 0) {
        m_task_current_id = 0;

        if (db_show_task(task_id)) {
            m_task_current_id = task_id;
            auto_save();

            ui->task_title->setCursorPosition(0);
            ui->task_view_frame->show();
        }
    }

}

void MainWindow::auto_save() {

    connect(ui->task_title, &QLineEdit::textChanged, [this]() {

        if (m_task_current_id == 0) {
            return;
        }

        QDateTime data_time = QDateTime::currentDateTime();
        int timestamp = data_time.toSecsSinceEpoch();

        // Auto save QTextEdit
        QSqlQuery db_query;

        db_query.prepare("UPDATE task_list SET title = :title, update_time = :update_time WHERE id = :id");
        db_query.bindValue(":id", m_task_current_id);
        db_query.bindValue(":title", ui->task_title->text());
        db_query.bindValue(":update_time", timestamp);

        if (!db_query.exec())
        {
            qInfo() << "An error occurred when executing the query related to autosave task title!";
        } else {
            window_update();

            QDateTime t_update_time;
            t_update_time.setSecsSinceEpoch(timestamp);
            QString format_update_time = t_update_time.toString("dd/MM/yyyy hh:mm");
            ui->updated_data->setText(format_update_time);

            ui->updated_text->show();
            ui->updated_data->show();
        }

    });

    connect(ui->task_edit, &QTextEdit::textChanged, [this]() {

        if (m_task_current_id == 0) {
            return;
        }

        QDateTime data_time = QDateTime::currentDateTime();
        int timestamp = data_time.toSecsSinceEpoch();

        // Auto save QTextEdit
        QSqlQuery db_query;

        db_query.prepare("UPDATE task_list SET text = :text, update_time = :update_time WHERE id = :id");
        db_query.bindValue(":id", m_task_current_id);
        db_query.bindValue(":text", ui->task_edit->toPlainText());
        db_query.bindValue(":update_time", timestamp);

        if (!db_query.exec())
        {
            qInfo() << "An error occurred when executing the query related to autosave task text!";
        } else {
            QDateTime t_update_time;
            t_update_time.setSecsSinceEpoch(timestamp);
            QString format_update_time = t_update_time.toString("dd/MM/yyyy hh:mm");
            ui->updated_data->setText(format_update_time);

            ui->updated_text->show();
            ui->updated_data->show();
        }

    });

}

QVector<QMap<QString, QVariant>> MainWindow::get_task_data_by_id(unsigned int task_id) {

    QVector<QMap<QString, QVariant>> item_map;

    QSqlQuery db_query;

    db_query.prepare("SELECT * FROM task_list WHERE id = :id");
    db_query.bindValue(":id", task_id);

    if (db_query.exec()) {
        while (db_query.next()) {
            QMap<QString, QVariant> t_map;
            t_map.insert("id", db_query.value("id"));
            t_map.insert("group_id", db_query.value("group_id"));
            t_map.insert("title", db_query.value("title"));
            t_map.insert("text", db_query.value("text"));
            t_map.insert("task_status", db_query.value("task_status"));
            t_map.insert("alarm", db_query.value("alarm"));
            t_map.insert("alarm_time", db_query.value("alarm_time"));
            t_map.insert("update_time", db_query.value("update_time"));
            t_map.insert("create_time", db_query.value("create_time"));
            item_map.push_back(t_map);
        }
    } else {
        qInfo() << "Error when querying rows by id \"" + QString::number(task_id) + "\" from the database";
    }

    return item_map;
}

void MainWindow::on_add_alarm_clicked()
{
    QDateTime current_data = QDateTime::currentDateTime();
    unsigned int current_timestamp = current_data.toSecsSinceEpoch();

    QVector<QMap<QString, QVariant>> item_map = get_task_data_by_id(m_task_current_id);
    unsigned int current_alarm_time_timestamp;

    if (item_map.size() > 0 && item_map.at(0).value("alarm_time").toUInt() > 0) {
        current_alarm_time_timestamp = item_map.at(0).value("alarm_time").toUInt();
    } else {
        current_alarm_time_timestamp = current_timestamp;
    }

    QDateTime current_alarm_time_datetime = QDateTime::fromSecsSinceEpoch(current_alarm_time_timestamp);
    QDate current_alarm_time_date = current_alarm_time_datetime.date();
    QTime current_alarm_time_qtime = current_alarm_time_datetime.time();

    QDialog dialog(this);
    dialog.setWindowTitle(tr("Select date and time for alarm"));
    dialog.setWindowFlag(Qt::WindowStaysOnTopHint);
    dialog.setStyleSheet("QDialog {background-color:#FFF;}");

    QVBoxLayout layout(&dialog);

    QCalendarWidget *calendar = new QCalendarWidget();
    calendar->setSelectedDate(current_alarm_time_date);
    calendar->setGridVisible(true);
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    layout.addWidget(calendar);

    QDateTimeEdit *time_edit = new QDateTimeEdit(QTime::currentTime());
    time_edit->setDisplayFormat("hh:mm");
    time_edit->setTime(current_alarm_time_qtime);
    layout.addWidget(time_edit);

    QDialogButtonBox *button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout.addWidget(button_box);

    connect(button_box, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(button_box, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QDateTime data_time = calendar->selectedDate().startOfDay().addMSecs(time_edit->time().msecsSinceStartOfDay());
        int alarm_time = data_time.toSecsSinceEpoch();

        if (m_task_current_id == 0) {
            return;
        }

        QSqlQuery db_query;

        db_query.prepare("UPDATE task_list SET alarm_time = :alarm_time, update_time = :update_time WHERE id = :id");
        db_query.bindValue(":id", m_task_current_id);
        db_query.bindValue(":alarm_time", alarm_time);
        db_query.bindValue(":update_time", current_timestamp);

        if (!db_query.exec())
        {
            qInfo() << "Error when writing a new alarm time to the database!";
        } else {
            window_update();

            QDateTime t_update_time;
            QDateTime t_alert_time;

            t_update_time.setSecsSinceEpoch(current_timestamp);
            t_alert_time.setSecsSinceEpoch(alarm_time);

            QString format_update_time = t_update_time.toString("dd/MM/yyyy hh:mm");
            QString format_alert_time = t_alert_time.toString("dd/MM/yyyy hh:mm");

            ui->updated_data->setText(format_update_time);
            ui->alert_data->setText(format_alert_time);

            if (current_timestamp == 0) {
                ui->updated_text->hide();
                ui->updated_data->hide();
            } else {
                ui->updated_text->show();
                ui->updated_data->show();
            }

            if (alarm_time == 0) {
                ui->alert_text->hide();
                ui->alert_data->hide();
            } else {
                ui->alert_text->show();
                ui->alert_data->show();
            }

            if (g_alert_cache.contains(m_task_current_id)) {
                g_alert_cache[m_task_current_id] = current_timestamp;
            }
        }
    }
}

void MainWindow::on_group_add_clicked()
{
    QDialog confirmation_dialog(this);
    confirmation_dialog.setFixedWidth(300);
    confirmation_dialog.setWindowTitle(tr("Enter a new group name"));

    QLineEdit *line_edit = new QLineEdit(&confirmation_dialog);
    line_edit->setText(tr("Notes"));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(line_edit);
    confirmation_dialog.setLayout(layout);

    QDialogButtonBox *button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &confirmation_dialog);

    layout->addWidget(button_box);

    connect(button_box, &QDialogButtonBox::accepted, &confirmation_dialog, &QDialog::accept);
    connect(button_box, &QDialogButtonBox::rejected, &confirmation_dialog, &QDialog::reject);

    int result = confirmation_dialog.exec();

    if (result == QDialog::Accepted) {
        QString name = line_edit->text();
        if (name == "") {
            name = tr("Undefined");
        }

        db_add_group(name, 1);

        window_update();
    }

    disconnect(button_box, &QDialogButtonBox::accepted, &confirmation_dialog, &QDialog::accept);
    disconnect(button_box, &QDialogButtonBox::rejected, &confirmation_dialog, &QDialog::reject);

    delete line_edit;
    delete layout;
    delete button_box;
}

void MainWindow::on_task_search_clicked()
{
    if (ui->task_search_edit->isHidden()) {
        ui->task_search_edit->show();
        ui->task_search_edit->setPlaceholderText(tr("Search..."));

        connect(ui->task_search_edit, &QLineEdit::textChanged, [this]() {

            QString search_query = ui->task_search_edit->text();
            QList<QListWidgetItem*> items = ui->task_list->findItems(search_query, Qt::MatchContains);

            for (int i = 0; i < ui->task_list->count(); i++) {
                QListWidgetItem* item = ui->task_list->item(i);
                if (items.contains(item)) {
                    item->setHidden(false);
                } else {
                    item->setHidden(true);
                }
            }
        });
    } else {
        disconnect(ui->task_search_edit);
        ui->task_search_edit->hide();
        window_update();
    }
}


void MainWindow::create_tray_icon() {

    /* Tray Block */
    // Create a tray icon and a menu
    set_tray_icon(tray_icon);

    // Remove the empty filed for Icons
    tray_menu.setStyleSheet("QMenu::item::icon{width: 0px;} QMenu::item::icon:disabled {width: 0px;} QMenu::item::icon:off {width: 0px;} QMenu::item::icon:on {width: 0px;} QMenu::item::icon:selected {width: 0px;} QMenu::item::icon:checked {width: 0px;} QMenu::indicator { width: 0px;}");

    // Show app
    QAction *app_show = tray_menu.addAction(SOFT_NAME);
    QObject::connect(app_show, &QAction::triggered, qApp, [&]() {
        show();
        raise();
    });

    // Add a "Autorun" action with CheckBox to the menu
    QAction *autorun_action = tray_menu.addAction("Autorun");
    autorun_action->setCheckable(true);
    autorun_action->setChecked(is_soft_in_autorun());

    // Add the action to toggle "Autorun" to the tray menu
    QObject::connect(autorun_action, &QAction::toggled, [&](bool checked) {
        if (checked) {
            add_soft_to_autorun();
        } else {
            remove_soft_from_autorun();
        }
    });

    // Separator
    QAction* separator_1 = new QAction();
    separator_1->setSeparator(true);
    tray_menu.addAction(separator_1);

    // About Author
    QAction *about_me = tray_menu.addAction("About Me");
    author_window.setWindowTitle("About Author - " + SOFT_NAME + " " + SOFT_VERSION);
    QObject::connect(about_me, &QAction::triggered, qApp, [&]() {
        author_window.show();
    });

    // About Qt
    QAction *about_qt = tray_menu.addAction("About Qt");
    QObject::connect(about_qt, &QAction::triggered, qApp, [&]() {
        qt_window.show();
    });

    // Separator
    QAction* separator_2 = new QAction();
    separator_2->setSeparator(true);
    tray_menu.addAction(separator_2);

    QObject::connect(tray_icon, &QSystemTrayIcon::activated, qApp, [&](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            if (isHidden()) {
                show();
                activateWindow();
                raise();
            } else {
                hide();
            }
        }
    });

    // Add a "Close" action to the menu
    QAction *close_action = tray_menu.addAction("Close");
    QObject::connect(close_action, &QAction::triggered, qApp, &QApplication::quit);

    // Set the tray icon and menu
    tray_icon->setContextMenu(&tray_menu);

    tray_icon->show();

}

#ifdef Q_OS_WIN
    bool MainWindow::is_soft_in_autorun()
    {
        QString startup_dir = QDir::toNativeSeparators(QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation).first()) + QDir::separator() + "Startup" + QDir::separator();
        QString lnk_path = startup_dir + SOFT_NAME + ".lnk";

        QFile link(lnk_path);
        if (link.exists()) {
            return true;
        } else {
            return false;
        }
    }

    void MainWindow::add_soft_to_autorun() {

        QString startup_dir = QDir::toNativeSeparators(QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation).first()) + QDir::separator() + "Startup" + QDir::separator();
        QString lnk_path = startup_dir + SOFT_NAME + ".lnk";

        QFile link(lnk_path);
        if (link.exists()) {
            link.remove();
        }

        IShellLink *shell_link;
        CoInitialize(NULL);
        HRESULT result = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, reinterpret_cast<void**>(&shell_link));
        if (result == S_OK) {
            IPersistFile *persist_file;
            shell_link->SetPath(reinterpret_cast<const wchar_t *>(QDir::toNativeSeparators(qApp->applicationFilePath()).utf16()));
            shell_link->SetWorkingDirectory(reinterpret_cast<const wchar_t *>(QDir::toNativeSeparators(qApp->applicationDirPath()).utf16()));
            result = shell_link->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&persist_file));
            if (result == S_OK) {
                persist_file->Save(reinterpret_cast<const wchar_t *>(lnk_path.utf16()), TRUE);
                persist_file->Release();
            }
            shell_link->Release();
        }
        CoUninitialize();
    }

    void MainWindow::remove_soft_from_autorun()
    {
        QString startup_dir = QDir::toNativeSeparators(QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation).first()) + QDir::separator() + "Startup" + QDir::separator();
        QString lnk_path = startup_dir + SOFT_NAME + ".lnk";

        QFile link(lnk_path);
        if (link.exists()) {
            link.remove();
        }
    }
#else
    bool MainWindow::is_soft_in_autorun()
    {
        QString autostart_dir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QDir::separator() + "autostart" + QDir::separator();
        QString desktop_file_path = autostart_dir + SOFT_NAME + ".desktop";

        QFile desktop_file(desktop_file_path);
        return desktop_file.exists();
    }

    void MainWindow::add_soft_to_autorun()
    {
        QString autostart_dir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QDir::separator() + "autostart" + QDir::separator();

        // Create an autostart folder if it does not exist
        QDir().mkpath(autostart_dir);

        QString desktop_file_path = autostart_dir + SOFT_NAME + ".desktop";

        QFile desktop_file(desktop_file_path);
        if (desktop_file.exists()) {
            desktop_file.remove();
        }

        QString desktop_file_content = "[Desktop Entry]\n"
                                       "Type=Application\n"
                                       "Exec=" + QCoreApplication::applicationFilePath() + "\n"
                                                                                   "Path=" + QCoreApplication::applicationDirPath() + "\n"
                                                                                  "Name=" + SOFT_NAME + "\n";

        desktop_file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream stream(&desktop_file);
        stream << desktop_file_content;
        desktop_file.close();
    }

    void MainWindow::remove_soft_from_autorun()
    {
        QString autostart_dir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QDir::separator() + "autostart" + QDir::separator();
        QString desktop_file_path = autostart_dir + SOFT_NAME + ".desktop";

        QFile desktop_file(desktop_file_path);
        if (desktop_file.exists()) {
            desktop_file.remove();
        }
    }
#endif

void MainWindow::on_encryption_button_clicked()
{
    encryption_window->set_data(use_encryption, g_db_password, &db, g_db_path);
    encryption_window->open_widget();
}

void MainWindow::on_settings_button_clicked()
{
    settings_window->open_widget();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete) { // task_list : KeyPressEvent

        QListWidgetItem *item = ui->task_list->currentItem();
        if (item && item->isSelected()) {

            QMessageBox::StandardButton confirmDelete = QMessageBox::question(this, tr("Delete Task"), tr("Are you sure you want to DELETE the selected task?"));
            if (confirmDelete == QMessageBox::No) {
                return;
            }

            m_task_current_id = 0;
            ui->task_title->setText("");
            ui->task_edit->setText("");

            ui->task_title->setToolTip("");

            db_rm_task(item->data(Qt::UserRole).toUInt());
            delete item;

            if (ui->task_list->count() == 0) ui->task_view_frame->hide();
        }
    } else if (event->key() == Qt::Key_F5) {

        window_update();
        QWidget::keyPressEvent(event);
    } else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_R) {

        window_update();
        QWidget::keyPressEvent(event);
    } else if (event->key() == Qt::Key_Escape) {

        window_update();
        ui->task_view_frame->hide();
        QWidget::keyPressEvent(event);
    } else {

        QWidget::keyPressEvent(event);
    }
}
