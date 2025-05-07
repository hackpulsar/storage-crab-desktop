#ifndef AES_H
#define AES_H

#include <nlohmann/json.hpp>

#include "key.hpp"
#include "utils/byte_array.hpp"

// Default IV size for AES (128-bit)
#define IV_SIZE 16

namespace Cryptography::AES {

using namespace Utils;

// Redefinition of the same type for code clarity purposes
typedef ByteArray AESKey;

struct KeyData final : Key {
    KeyData(AESKey key, ByteArray iv);
    ~KeyData() override = default;

    AESKey key;
    ByteArray iv;

    nlohmann::json toJSON() override;
};

// Generates AES key with size
KeyData generateKey(size_t key_size, size_t iv_size);

// Encrypts a string and outputs a byte array
ByteArray encrypt(const KeyData& config, const std::string& input);

}

#endif //AES_H
