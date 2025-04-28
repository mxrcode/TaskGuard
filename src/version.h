#ifndef VERSION_H
#define VERSION_H

#include <QString>
#include <QSettings>
#include <QCryptographicHash>

const QString SOFT_NAME = "TaskGuard";
const QString SOFT_VERSION = "1.3.4";

// Used to encrypt various data in the application, we recommend that you change this value "QString(value)" for private use.
const QString MAGIC = QCryptographicHash::hash(QString("2as3N6vtY5AjB674vig1YGuynTL2hQwU0eKUy4IiHi0eAG1HvcYnuzy8WAT2xQdrRhFPlnF0i0Ysgm3w47glH7gYbmqrxzz3TDMCECEAvOBqjN07xRASGY6fLAEHsSn5").toUtf8() + QSysInfo::machineUniqueId(), QCryptographicHash::Blake2b_512).toHex();

// Settings
void save_settings(QString name, QString data);
QString restore_settings(QString name);

#endif // VERSION_H
