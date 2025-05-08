#include "cryptography/aes.h"

#include <openssl/rand.h>
#include <openssl/aes.h>
#include <fstream>

namespace Cryptography::AES {

KeyData::KeyData(AESKey key, ByteArray iv, const AESType type)
    : key(std::move(key)), iv(std::move(iv)), type(type)
{}

nlohmann::json KeyData::toJSON() {
    nlohmann::json keyJson;

    // Assemble the JSON
    keyJson["type"] = "AES";
    keyJson["AES"]["key"] = toHEX(key);
    keyJson["AES"]["iv"] = toHEX(iv);

    return keyJson;
}

KeyData generateKey(AESType type) {
    AESKey key((int)type);

    if (!RAND_bytes(key.data(), (int)type))
        throw std::runtime_error("Failed to generate AES key");

    ByteArray iv(IV_SIZE);

    if (!RAND_bytes(iv.data(), IV_SIZE))
        throw std::runtime_error("Failed to generate AES iv");

    return {key, iv, type};
}

ByteArray encrypt(const KeyData& config, const std::string& input) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        throw std::runtime_error("Failed to create EVP_CIPHER_CTX");

    const EVP_CIPHER* cipher = nullptr;
    switch (config.type) {
        case AESType::AES_128: cipher = EVP_aes_128_cbc(); break;
        case AESType::AES_192: cipher = EVP_aes_192_cbc(); break;
        case AESType::AES_256: cipher = EVP_aes_256_cbc(); break;
    }

    // Initialize encryption context
    const int init_result = EVP_EncryptInit_ex(
        ctx,
        cipher,
        nullptr,
        config.key.data(),
        config.iv.data()
    );
    if (init_result != 1) throw std::runtime_error("Failed to initialize AES context");

    // Encrypt the plaintext
    ByteArray ciphertext(input.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int len = 0, ciphertext_len = 0;

    const int encryption_result = EVP_EncryptUpdate(
        ctx,
        ciphertext.data(),
        &len,
        reinterpret_cast<const unsigned char*>(input.data()),
        input.size()
    );

    if (encryption_result != 1) throw std::runtime_error("Failed to encrypt AES context");

    ciphertext_len = len;

    // Finalize encryption
    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1)
        throw std::runtime_error("Failed to finalize AES context");

    ciphertext_len += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);

    ciphertext.resize(ciphertext_len);
    return ciphertext;
}

} // Cryptography