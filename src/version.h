#ifndef VERSION_H
#define VERSION_H

#include <QString>
#include <QSettings>
#include <QCryptographicHash>

const QString SOFT_NAME = "TaskGuard";
const QString SOFT_VERSION = "1.3.5";

// Used to encrypt various data in the application, we recommend that you change this value "QString(value)" for private use.
const QString MAGIC = QCryptographicHash::hash(QString("KhOlBTlkLVWhwMpdBK5iwGt327vvreGc92MaYAq9OXO8aSQw3M0GSMhPvhkUGlENlTdYJrpQBCxzyDQSCie0lqi2qfLLuPF7bquDFA0P2pf71VvVSn9JEaevrIE3w3Ca").toUtf8() + QSysInfo::machineUniqueId(), QCryptographicHash::Blake2b_512).toHex();

// Settings
void save_settings(QString name, QString data);
QString restore_settings(QString name, QString defaultValue = "");

#endif // VERSION_H
