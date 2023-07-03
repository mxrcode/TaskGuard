#include "version.h"

// Settings
void save_settings(QString name, QString data) {

    QSettings settings("mxrcode", SOFT_NAME);
    settings.setValue(name, data);
}

QString restore_settings(QString name) {

    QSettings settings("mxrcode", SOFT_NAME);
    return settings.value(name).toString();
}
