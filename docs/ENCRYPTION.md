# Encryption Guide

Storage Crab Desktop encrypts files client-side before uploading to the server. The server never has access to unencrypted file contents or decryption keys.

## Encryption Algorithms

### Supported Algorithms

| Algorithm | Description | Key Size | Use Case |
|-----------|-------------|----------|----------|
| AES-128 | AES with 128-bit key | 16 bytes | Basic encryption, faster |
| AES-192 | AES with 192-bit key | 24 bytes | Balanced security/speed |
| AES-256 | AES with 256-bit key | 32 bytes | Strongest AES encryption |
| Hybrid (AES + RSA) | AES encrypts file, RSA encrypts AES key | Varies | Maximum security with key exchange |

### Algorithm Types

**File:** `include/cryptography/algorithm_types.h`

```cpp
namespace Cryptography {
    enum class AlgorithmType { AES = 0, Hybrid };
    enum class AESType { AES_128 = 16, AES_192 = 24, AES_256 = 32 };
}
```

## Encryption Flow

### Overview

```
Original File
     │
     ▼
┌─────────────────────────────────────┐
│       FileCrypto::encryptFile()     │
│                                     │
│  1. Read file contents              │
│  2. Generate encryption keys        │
│  3. CryptoService::Encrypt()        │
│     ├─ AES: Generate key + IV       │
│     └─ Hybrid: Generate RSA keypair │
│        + AES key, then encrypt AES  │
│        key with RSA                 │
│  4. Write encrypted file            │
│  5. Write .crbkey file (JSON config)│
└─────────────────────────────────────┘
     │
     ├─► Encrypted File (.enc)
     │
     └─► Key File (.crbkey) - Needed for decryption
```

### Upload Dialog Configuration

**File:** `forms/upload_dialog.ui` / `include/widgets/upload_dialog.h`

User selects:
- **Algorithm**: AES or Hybrid (AES + RSA)
- **AES Type**: AES-128, AES-192, or AES-256
- **RSA Key Size** (Hybrid only): 2048, 4096, etc.
- **Encrypt File Name**: Option to encrypt the filename

### EncryptOptions

```cpp
struct FileCrypto::EncryptOptions {
    std::string filePath;       // Path to original file
    std::string keyPath;        // Where to save the .crbkey file
    AESType aesType;            // AES_128, AES_192, AES_256
    AlgorithmType algorithm;    // AES or Hybrid
    int rsaKeySize;             // e.g., 2048, 4096 (Hybrid only)
    bool encryptFileName;       // Whether to encrypt the filename
};
```

### Key File Format

The .crbkey file is a JSON file containing all information needed to decrypt the file.

**AES-only key file:**
```json
{
    "algorithm": "AES",
    "aes_type": 32,
    "key": "3f2a1b4c5d6e7f8091a2b3c4d5e6f708192a3b4c5d6e7f8091a2b3c4d5e6f70",
    "iv": "1a2b3c4d5e6f708192a3b4c5d6e7f80",
    "encrypted_name": false
}
```

**Hybrid (AES + RSA) key file:**
```json
{
    "algorithm": "Hybrid (AES + RSA)",
    "aes_type": 32,
    "key": "3f2a1b4c5d6e7f8091a2b3c4d5e6f708192a3b4c5d6e7f8091a2b3c4d5e6f70",
    "iv": "1a2b3c4d5e6f708192a3b4c5d6e7f80",
    "encrypted_name": false,
    "rsa_private_key": "-----BEGIN PRIVATE KEY-----\nMIIEvQIBADAN...\n-----END PRIVATE KEY-----"
}
```

## Decryption Flow

### Overview

```
Encrypted File + Key File
     │
     ▼
┌─────────────────────────────────────┐
│       FileCrypto::decryptFile()     │
│                                     │
│  1. Read encrypted file             │
│  2. Parse .crbkey file (JSON)          │
│  3. CryptoService::Decrypt()        │
│     ├─ AES: Use key + IV directly   │
│     └─ Hybrid: Extract RSA private  │
│        key, decrypt AES key, then   │
│        decrypt file with AES        │
│  4. Write decrypted file            │
└─────────────────────────────────────┘
     │
     ▼
Decrypted File (original)
```

### DecryptOptions

```cpp
struct FileCrypto::DecryptOptions {
    std::string filePath;          // Path to encrypted file
    std::string keyPath;           // Path to .crbkey file
    bool decryptFileName;          // Whether to decrypt the filename
};
```

## AES Encryption

**File:** `include/cryptography/aes.h`

Uses OpenSSL's EVP API for AES encryption in CBC mode.

### Key Generation

```cpp
namespace Cryptography::AES {
    struct KeyData {
        AESKey key;        // Encryption key (16, 24, or 32 bytes)
        ByteArray iv;      // Initialization vector (16 bytes, IV_SIZE)
        AESType type;      // AES_128, AES_192, or AES_256
    };
    
    KeyData generateKey(AESType type);
}
```

- Key is generated using `RAND_bytes()` from OpenSSL
- IV (Initialization Vector) is also randomly generated
- Both stored in the .crbkey file

### Encryption

```cpp
ByteArray encrypt(const KeyData& config, const ByteArray& input);
```

- Pads input using PKCS7 padding
- Encrypts using AES in CBC mode
- Returns ciphertext

### Decryption

```cpp
ByteArray decrypt(const KeyData& config, const ByteArray& ciphertext);
```

- Decrypts using AES in CBC mode
- Removes PKCS7 padding
- Returns plaintext

## RSA Encryption (Hybrid Mode)

**File:** `include/cryptography/rsa.h`

Uses OpenSSL's EVP API for RSA key generation and encryption.

### Key Generation

```cpp
namespace Cryptography::RSA {
    struct KeyData {
        EVP_PKEY* keyPair;    // OpenSSL key pair
    };
    
    KeyData generateKey(size_t key_size);  // e.g., 2048, 4096
}
```

- Generates RSA public/private key pair
- Private key is stored in the .crbkey file
- **Note:** In Hybrid mode, the RSA private key is embedded in the key file

### How Hybrid Mode Works

1. Generate AES key and IV (as in AES-only mode)
2. Generate RSA key pair
3. Encrypt the file with AES (using step 1 key)
4. Encrypt the AES key with RSA public key
5. Store RSA private key in .crbkey file (allows decryption later)
6. Store encrypted AES key in .crbkey file

**Decryption:**
1. Read RSA private key from .crbkey file
2. Decrypt the AES key using RSA
3. Decrypt the file using AES with the decrypted key

### Encrypt/Decrypt

```cpp
ByteArray encrypt(const KeyData& config, const ByteArray& input);
ByteArray decrypt(const KeyData& config, const ByteArray& ciphertext);
```

In Hybrid mode, RSA is used to encrypt/decrypt the AES key, not the file itself.

## CryptoService (High-Level Interface)

**File:** `include/cryptography/crypto_service.h`

Orchestrates the encryption/decryption process, handling both AES-only and Hybrid modes.

### Encrypt

```cpp
static EncryptResult Encrypt(
    const ByteArray& content,         // File contents
    const std::string& fileName,     // Original filename
    AESType aesType,                  // AES-128/192/256
    AlgorithmType algorithmType,      // AES or Hybrid
    size_t rsaKeySize,                // RSA key size (Hybrid)
    bool encryptName                  // Encrypt filename?
);
```

**Returns:**
```cpp
struct EncryptResult {
    Utils::ByteArray encryptedContent;
    std::string encryptedFileName;    // May be encrypted if encryptName=true
    nlohmann::json config;            // Key file JSON
};
```

### Decrypt

```cpp
static DecryptResult Decrypt(
    const ByteArray& content,         // Encrypted file contents
    const std::string& fileName,      // Current filename
    nlohmann::json& config,           // Key file JSON
    bool decryptName                  // Decrypt filename?
);
```

**Returns:**
```cpp
struct DecryptResult {
    Utils::ByteArray content;          // Decrypted file contents
    std::string decryptedFileName;     // Original filename
};
```

## File Name Encryption

When `encryptFileName` is true:

1. The original filename is encrypted using the same AES key
2. The encrypted filename is stored in the key file
3. The uploaded file uses a hash or random name on the server
4. On decryption, the original filename is recovered from the key file

**Key file field:**
```json
{
    "encrypted_name": true,
    "original_name": "4a5b6c7d8e9f...",  // Encrypted filename
    ...
}
```

## Security Considerations

### What's Protected

- **File contents** - Always encrypted before upload
- **Filenames** - Optional encryption
- **Encryption keys** - Stored locally in `.crbkey` files, never sent to server

### What's NOT Protected

- **File sizes** - Visible to server (can infer information)
- **Upload/download patterns** - Server can see when and how often you access files
- **Metadata** - File types may be inferable from encrypted content patterns

### Key File Safety

- **Backup your `.crbkey` files** - Without them, encrypted files cannot be decrypted
- **Store separately** - Don't upload `.crbkey` files to the server
- **Secure storage** - Consider storing keys in a secure location (not next to encrypted files in cloud storage)

### Hybrid Mode Security

In Hybrid mode, the RSA private key is stored in the `.crbkey` file. This means:
- Anyone with the `.crbkey` file can decrypt the data
- The `.crbkey` file must be kept secure
- This mode is useful when you want to use RSA for key encapsulation

## Implementation Details

### ByteArray Utility

**File:** `include/utils/byte_array.hpp`

```cpp
namespace Utils {
    typedef std::vector<unsigned char> ByteArray;
    
    std::string toHex(const ByteArray& bytes);
    ByteArray fromHex(const std::string& hex);
}
```

Used throughout the cryptography module for handling binary data.

### Key Base Class

**File:** `include/cryptography/key.hpp`

```cpp
namespace Cryptography {
    class Key {
    public:
        virtual nlohmann::json toJSON() = 0;
    };
}
```

Base class for `AES::KeyData` and `RSA::KeyData`, providing a common interface for serializing key data to JSON (for the .crbkey file).

## Example: Programmatic Encryption

```cpp
#include "cryptography/file_crypto.h"
#include "cryptography/algorithm_types.h"

using namespace Cryptography;

// Encrypt a file
FileCrypto::EncryptOptions encOpts;
encOpts.filePath = "/path/to/document.pdf";
encOpts.crbkeyPath = "/path/to/document.pdf.crbkey";
encOpts.aesType = AESType::AES_256;
encOpts.algorithm = AlgorithmType::AES;
encOpts.rsaKeySize = 0;  // Not used for AES-only
encOpts.encryptFileName = false;

FileCrypto::Result encResult = FileCrypto::encryptFile(encOpts);
if (encResult.ok) {
    std::cout << "Encrypted to: " << encResult.path << std::endl;
    std::cout << "Key saved to: " << encResult.fileName << ".crbkey" << std::endl;
}

// Decrypt a file
FileCrypto::DecryptOptions decOpts;
decOpts.filePath = "/path/to/document.pdf.enc";
decOpts.crbkeyPath = "/path/to/document.pdf.crbkey";
decOpts.decryptFileName = false;

FileCrypto::Result decResult = FileCrypto::decryptFile(decOpts);
if (decResult.ok) {
    std::cout << "Decrypted to: " << decResult.path << std::endl;
}
```
