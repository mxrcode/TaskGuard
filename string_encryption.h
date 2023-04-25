#ifndef STRING_ENCRYPTION_HPP
#define STRING_ENCRYPTION_HPP

// AES-256-CBC

#include <QString>
#include <QByteArray>
#include <QCryptographicHash>
#include <openssl/evp.h>
#include <openssl/rand.h>

QString magic_encrypt(const QString& data, const QString& password);
QString magic_decrypt(const QString& encrypted_data, const QString& password);

#endif // STRING_ENCRYPTION_HPP
