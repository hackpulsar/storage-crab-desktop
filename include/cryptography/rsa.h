#ifndef RSA_H
#define RSA_H

#include <nlohmann/json_fwd.hpp>

#include "key.hpp"
#include "utils/byte_array.hpp"

#include <openssl/rsa.h>

namespace Cryptography::RSA {

struct KeyData final : Key {
    explicit KeyData(EVP_PKEY* keyPair);
    ~KeyData() override;

    EVP_PKEY* keyPair;
};

// Generates an RSA public/private key pair
KeyData generateKey(size_t key_size);

// Converts EVP_PKEY to a string
std::string keyToString(const EVP_PKEY* keyPair, bool isPrivate);

// Encrypts a string and outputs a byte array
Utils::ByteArray encrypt(const KeyData& config, const std::string& input);

}

#endif //RSA_H
