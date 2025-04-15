#include "cryptography/aes.h"

#include <openssl/rand.h>
#include <openssl/aes.h>
#include <nlohmann/json.hpp>
#include <fstream>

namespace Cryptography::AES {

KeyData::KeyData(AESKey key, ByteArray iv)
    : key(std::move(key)), iv(std::move(iv))
{}

void KeyData::exportTo(const std::string &path) {
    nlohmann::json keyJson;

    // Assemble the JSON
    keyJson["algorithm"] = "AES";
    keyJson["key"] = toHEX(key);
    keyJson["iv"] = toHEX(iv);

    // Write to a file
    std::ofstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Could not open file " + path);
    file << keyJson.dump();
}

KeyData generateKey(const size_t key_size, const size_t iv_size) {
    AESKey key(key_size);

    if (!RAND_bytes(key.data(), key_size))
        throw std::runtime_error("Failed to generate AES key");

    ByteArray iv(iv_size);

    if (!RAND_bytes(iv.data(), iv_size))
        throw std::runtime_error("Failed to generate AES iv");

    return {key, iv};
}

ByteArray encrypt(const KeyData& config, const std::string& input) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        throw std::runtime_error("Failed to create EVP_CIPHER_CTX");

    // Initialize encryption context
    const int init_result = EVP_EncryptInit_ex(
        ctx,
        EVP_aes_256_cbc(),
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