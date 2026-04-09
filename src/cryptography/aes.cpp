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

    keyJson["type"] = "AES";
    keyJson["AES"]["key"] = toHEX(key);
    keyJson["AES"]["iv"] = toHEX(iv);
    keyJson["AES"]["type"] = type;

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

KeyData parseKey(const nlohmann::json& config) {
    const auto& keyJson = config.at("AES").at("key");

    Utils::ByteArray keyBytes;
    if (keyJson.is_array())
        keyBytes.assign(keyJson.begin(), keyJson.end());
    else
        keyBytes = toByteArray(keyJson.get<std::string>());

    return KeyData(
        keyBytes,
        toByteArray(config.at("AES").at("iv").get<std::string>()),
        config.at("AES").at("type")
    );
}

ByteArray encrypt(const KeyData& config, const Utils::ByteArray& input) {
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

ByteArray decrypt(const KeyData& config, const ByteArray& ciphertext) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        throw std::runtime_error("Failed to create EVP_CIPHER_CTX");

    const EVP_CIPHER* cipher = nullptr;
    switch (config.type) {
        case AESType::AES_128: cipher = EVP_aes_128_cbc(); break;
        case AESType::AES_192: cipher = EVP_aes_192_cbc(); break;
        case AESType::AES_256: cipher = EVP_aes_256_cbc(); break;
        default:
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Unsupported AES type");
    }

    if (EVP_DecryptInit_ex(
            ctx,
            cipher,
            nullptr,
            config.key.data(),
            config.iv.data()
        ) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize AES decryption context");
    }

    // Allocate enough room: ciphertext size + block size
    ByteArray plaintext(ciphertext.size() + EVP_CIPHER_block_size(cipher));
    int len = 0;
    int plaintext_len = 0;

    if (EVP_DecryptUpdate(
            ctx,
            plaintext.data(),
            &len,
            ciphertext.data(),
            static_cast<int>(ciphertext.size())
        ) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to decrypt AES ciphertext");
    }

    plaintext_len = len;

    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + plaintext_len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize AES decryption (bad key/IV/data?)");
    }

    plaintext_len += len;
    plaintext.resize(plaintext_len);

    EVP_CIPHER_CTX_free(ctx);
    return plaintext;
}

} // Cryptography