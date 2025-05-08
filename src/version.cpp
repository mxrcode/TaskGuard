#include "version.h"

// Settings
void save_settings(QString name, QString data) {

    QSettings settings("mxrcode", SOFT_NAME);
    settings.setValue(name, data);
}

QString restore_settings(QString name, QString defaultValue)
{
    QSettings settings("mxrcode", SOFT_NAME);
    if (!settings.contains(name)) {
        save_settings(name, defaultValue);
        return defaultValue;
    }
    return settings.value(name).toString();
}
