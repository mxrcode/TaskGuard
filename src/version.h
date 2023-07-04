#ifndef VERSION_H
#define VERSION_H

#include <QString>
#include <QSettings>
#include <QCryptographicHash>

const QString SOFT_NAME = "TaskGuard";
const QString SOFT_VERSION = "1.2.1";

// Used to encrypt various data in the application, we recommend that you change this value "QString(value)" for private use.
const QString MAGIC = QCryptographicHash::hash(QString("e@txAaCLhYnlmwFSuG3#ADJaGdF3HElOWr1bC8FOlxD1lS25HjgDHS5oQL8BNDUuHKr&WTfEfRh#zzsd6jid9vEFEEVAwurqI2zy1dKed35uswIqhn7WmHCN4BGpo2lg").toUtf8() + QSysInfo::machineUniqueId(), QCryptographicHash::Blake2b_512).toHex();

// Settings
void save_settings(QString name, QString data);
QString restore_settings(QString name);

#endif // VERSION_H
