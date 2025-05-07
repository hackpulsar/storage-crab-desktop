#include "cryptography/rsa.h"

#include <fstream>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <nlohmann/json.hpp>

namespace Cryptography::RSA {
KeyData::KeyData(EVP_PKEY* keyPair)
    : keyPair(std::move(keyPair))
{}

KeyData::~KeyData() {
    EVP_PKEY_free(keyPair);
}

KeyData generateKey(const size_t key_size) {
    EVP_PKEY_CTX* ctx = nullptr;
    EVP_PKEY* pkey = nullptr;

    ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (!ctx || EVP_PKEY_keygen_init(ctx) <= 0) {
        throw std::runtime_error("Could not initialize RSA keygen context");
    }

    std::cout << key_size << std::endl;

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, key_size) <= 0) {
        throw std::runtime_error("Could not set RSA key size");
    }

    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        throw std::runtime_error("Could not generate RSA key");
    }

    EVP_PKEY_CTX_free(ctx);

    return KeyData(pkey);
}

std::string keyToString(const EVP_PKEY* keyPair, const bool isPrivate) {
    BIO* mem = BIO_new(BIO_s_mem());
    if (isPrivate)
        PEM_write_bio_PrivateKey(mem, keyPair, nullptr, nullptr, 0, nullptr, nullptr);
    else
        PEM_write_bio_PUBKEY(mem, keyPair);

    char* data;
    const long len = BIO_get_mem_data(mem, &data);
    std::string result(data, len);
    BIO_free_all(mem);
    return result;
}

Utils::ByteArray encrypt(const KeyData& config, const std::string& input) {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(config.keyPair, nullptr);
    Utils::ByteArray ciphertext;

    // Convert std::string to ByteArray
    const auto plaintext = reinterpret_cast<const unsigned char*>(input.data());
    const size_t plaintext_len = input.size();

    if (!ctx || EVP_PKEY_encrypt_init(ctx) <= 0)
        throw std::runtime_error("Could not initialize RSA key encryption context");

    // Getting out size
    size_t outlen = 0;
    if (EVP_PKEY_encrypt(ctx, nullptr, &outlen, plaintext, plaintext_len) <= 0)
        throw std::runtime_error("Could not encrypt with RSA");

    // Performing the actual encryption
    ciphertext.resize(outlen);
    if (EVP_PKEY_encrypt(ctx, ciphertext.data(), &outlen, plaintext, plaintext_len) <= 0) {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Could not encrypt with RSA");
    }

    ciphertext.resize(outlen); // Trim to actual size

    EVP_PKEY_CTX_free(ctx);
    return ciphertext;
}

}
