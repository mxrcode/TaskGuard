#ifndef VERSION_H
#define VERSION_H

#include <QString>
#include <QSettings>
#include <QCryptographicHash>

const QString SOFT_NAME = "TaskGuard";
const QString SOFT_VERSION = "1.3.3";

// Used to encrypt various data in the application, we recommend that you change this value "QString(value)" for private use.
const QString MAGIC = QCryptographicHash::hash(QString("ay4pFGqiEuoCN8Lg6QZoFnkfM7Uqoe9P62b7amZzO6wmcrPfkeUDcW2CT0JAvjnvF3uu7aWY30yRL2ylBQoxKtPkkC9NkvFlUZcipg11yRZwp5qN5m46YxE3pUtry2p2").toUtf8() + QSysInfo::machineUniqueId(), QCryptographicHash::Blake2b_512).toHex();

// Settings
void save_settings(QString name, QString data);
QString restore_settings(QString name);

#endif // VERSION_H
