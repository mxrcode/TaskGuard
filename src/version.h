#ifndef VERSION_H
#define VERSION_H

#include <QString>
#include <QSettings>
#include <QCryptographicHash>

const QString SOFT_NAME = "TaskGuard";
const QString SOFT_VERSION = "1.3.2";

// Used to encrypt various data in the application, we recommend that you change this value "QString(value)" for private use.
const QString MAGIC = QCryptographicHash::hash(QString("JZCtYU84lMJHY4O2WKTzCoS9ucDm7Sv51YSjcfyfqIKMH9JtG1JYCBCZumHyoaxMhGGKqp3MBwSDexG47C0LgegB6yakhdg5kFjX5X1YlDeRNSVbMtz5FkQzIrpjf6Ba").toUtf8() + QSysInfo::machineUniqueId(), QCryptographicHash::Blake2b_512).toHex();

// Settings
void save_settings(QString name, QString data);
QString restore_settings(QString name);

#endif // VERSION_H
