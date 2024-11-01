#ifndef VERSION_H
#define VERSION_H

#include <QString>
#include <QSettings>
#include <QCryptographicHash>

const QString SOFT_NAME = "TaskGuard";
const QString SOFT_VERSION = "1.3.0";

// Used to encrypt various data in the application, we recommend that you change this value "QString(value)" for private use.
const QString MAGIC = QCryptographicHash::hash(QString("NfxjiNNSCy2ZIgQUT5V8ZF5qKchadALUpeE5NvFHSlJAbW2t9nqzhF%tToP11bWQODaakdGRRQckcmmPzy1cbWVd7N6ZMVH1Tyq4G#TcdzOmS249oS9eo8YNa6dTWLz3").toUtf8() + QSysInfo::machineUniqueId(), QCryptographicHash::Blake2b_512).toHex();

// Settings
void save_settings(QString name, QString data);
QString restore_settings(QString name);

#endif // VERSION_H
