#ifndef VERSION_H
#define VERSION_H

#include <QString>
#include <QCryptographicHash>

const QString SOFT_NAME = "TaskGuard";
const QString SOFT_VERSION = "1.2.0";

// Used to encrypt various data in the application, we recommend that you change this value "QString(value)" for private use.
const QString MAGIC = QCryptographicHash::hash(QString("XNN02Ot7VbVfNBix8psmIXow1zcVu5fCknu&1uPVvynfNwdkCPH4OAOUPDJQbOzbW0Ykroi8GMMiS4qhzm6N57AUJv3yeleTRuedBAert7QEqTe3RQvU2sAKX4XVXPAe").toUtf8() + QSysInfo::machineUniqueId(), QCryptographicHash::Blake2b_512).toHex();

#endif // VERSION_H
