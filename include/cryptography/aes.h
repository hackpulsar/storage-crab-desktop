#ifndef AES_H
#define AES_H

#include <vector>

// Default IV size for AES (128-bit)
#define IV_SIZE 16

namespace Cryptography {

typedef unsigned char byte;
typedef std::vector<byte> ByteArray;

namespace AES {

typedef std::vector<byte> AESKey;

struct KeyData {
    AESKey key;
    ByteArray iv;
};

// Generates AES key with size
KeyData generateKey(size_t key_size, size_t iv_size);

// Encrypts a string and outputs a byte array
ByteArray encrypt(const KeyData& config, const std::string& input);

}

} // Cryptography

#endif //AES_H
