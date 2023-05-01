#ifndef VERSION_H
#define VERSION_H

#include <QString>
#include <QCryptographicHash>

const QString SOFT_NAME = "TaskGuard";
const QString SOFT_VERSION = "1.1.1";

// Used to encrypt various data in the application, we recommend that you change this value "QString(value)" for private use.
const QString MAGIC = QCryptographicHash::hash(QString("s%3iSyOVLYGGjhXOY9O3DG!dx4MHHr#uPGVUsEt0e8pyQtlYS0EnTCTVcW76I6jR").toUtf8() + QSysInfo::machineUniqueId(), QCryptographicHash::Blake2b_512).toHex();

#endif // VERSION_H
