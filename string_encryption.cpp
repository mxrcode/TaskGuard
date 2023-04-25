#include "string_encryption.h"

QString magic_encrypt(const QString& data, const QString& password) {

    QByteArray encrypted_data;
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        // Handle error
        return QString();
    }

    // Generate a random IV
    QByteArray iv(EVP_MAX_IV_LENGTH, 0);
    if (RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), EVP_MAX_IV_LENGTH) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        // Handle error
        return QString();
    }

    // Set up the cipher and initialize the encryption operation
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char*>(password.toUtf8().constData()), reinterpret_cast<const unsigned char*>(iv.constData())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        // Handle error
        return QString();
    }

    // Provide the plaintext data to be encrypted
    QByteArray plaintext = data.toUtf8();
    QByteArray ciphertext(plaintext.size() + EVP_CIPHER_CTX_block_size(ctx), 0);
    int ciphertextLength = 0;
    if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(ciphertext.data()), &ciphertextLength, reinterpret_cast<const unsigned char*>(plaintext.constData()), plaintext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        // Handle error
        return QString();
    }

    // Finalize the encryption
    int finalCiphertextLength = 0;
    if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(ciphertext.data()) + ciphertextLength, &finalCiphertextLength) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        // Handle error
        return QString();
    }
    ciphertextLength += finalCiphertextLength;

    // Concatenate the IV and the ciphertext
    encrypted_data.append(iv);
    encrypted_data.append(ciphertext.left(ciphertextLength));

    EVP_CIPHER_CTX_free(ctx);

    // Convert the encrypted data to base64 encoding for safe storage or transmission
    return QString(encrypted_data.toBase64());

}

QString magic_decrypt(const QString& encrypted_data, const QString& password) {

    QByteArray decrypted_data;
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        // Handle error
        return QString();
    }

    // Convert the base64-encoded encrypted data back to binary
    QByteArray encrypted_data_binary = QByteArray::fromBase64(encrypted_data.toUtf8());
    if (encrypted_data_binary.isEmpty()) {
        EVP_CIPHER_CTX_free(ctx);
        // Handle error
        return QString();
    }

    // Extract the IV from the beginning of the encrypted data
    QByteArray iv = encrypted_data_binary.left(EVP_MAX_IV_LENGTH);
    encrypted_data_binary = encrypted_data_binary.mid(EVP_MAX_IV_LENGTH);

    // Set up the cipher and initialize the decryption operation
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char*>(password.toUtf8().constData()), reinterpret_cast<const unsigned char*>(iv.constData())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        // Handle error
        return QString();
    }

    // Provide the ciphertext data to be decrypted
    QByteArray ciphertext = encrypted_data_binary;
    QByteArray plaintext(ciphertext.size() + EVP_CIPHER_CTX_block_size(ctx), 0);
    int plaintext_length = 0;
    if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(plaintext.data()), &plaintext_length, reinterpret_cast<const unsigned char*>(ciphertext.constData()), ciphertext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        // Handle error
        return QString();
    }

    // Finalize the decryption
    int final_plaintext_length = 0;
    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(plaintext.data()) + plaintext_length, &final_plaintext_length) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        // Handle error
        return QString();
    }
    plaintext_length += final_plaintext_length;

    EVP_CIPHER_CTX_free(ctx);

    // Convert the decrypted data back to QString
    decrypted_data = QByteArray(plaintext.left(plaintext_length));
    return QString::fromUtf8(decrypted_data);
}

