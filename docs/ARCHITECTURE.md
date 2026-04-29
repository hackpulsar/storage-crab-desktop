# Architecture Overview

## High-Level Architecture

Storage Crab Desktop follows a layered architecture with clear separation between core logic and UI components.

```
┌─────────────────────────────────────────────────────────┐
│                      main.cpp                           │
│                  (QApplication entry)                   │
└──────────────────────┬──────────────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────────────┐
│                    LoginWindow                          │
│         (Email/Password → ApiDispatcher::login)         │
└──────────────────────┬──────────────────────────────────┘
                       │ onLoginSuccessfull
                       ▼
┌─────────────────────────────────────────────────────────┐
│                   UserDashboard                         │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌─────────┐     │
│  │  Upload  │ │ Download │ │  Share   │ │ Decrypt │     │
│  │  Dialog  │ │ Shared   │ │  Code    │ │ Dialog  │     │
│  └──────────┘ └──────────┘ └──────────┘ └─────────┘     │
│                                                         │
│  ┌─────────────────────────────────────────────────┐    │
│  │           UploadedFilePanel (list)              │    │
│  │  [filename] [size] [share] [download] [delete]  │    │
│  └─────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────┘
                            │
    ┌───────────────────────┼──────────────────────┐
    ▼                       ▼                      ▼
┌──────────────┐    ┌──────────────┐   ┌─────────────┐
│   core       │    │ ui_elements  │   │  Backend    │
│ (static lib) │◄───│ (static lib) │   │  API Server │
└────┬─────────┘    └──────────────┘   └─────────────┘
     │
     ├─ ApiDispatcher (singleton)
     ├─ TokenPair (thread-safe)
     ├─ CryptoService / FileCrypto
     ├─ AES / RSA
     └─ RequestResult / FileData
```

## Core Library (`core`)

Static library containing all non-UI logic. Depends on Qt6::Core and Qt6::Concurrent for async operations.

### ApiDispatcher (Singleton)

**File:** `include/api/api_dispatcher.hpp`

Central class managing all API communication. Implemented as a thread-safe singleton.

```cpp
class ApiDispatcher final : public QObject {
    static ApiDispatcher& instance();  // Singleton access
    
    void storeTokens(access, refresh);
    RequestResultFuture login(email, password_hash);
    RequestResultFuture me();
    RequestResultFuture getFiles();
    RequestResultFuture uploadFile(fileName, path);
    RequestResultFuture downloadFile(fileID, destination);
    RequestResultFuture downloadSharedFile(code, destination);
    RequestResultFuture deleteFile(fileID);
    RequestResultFuture shareFile(fileID);
    
signals:
    void sessionExpired();  // Emitted when refresh fails
    
private slots:
    void onRefreshTimer();  // Auto-refresh every 9 minutes
};
```

All methods return `QFuture<API::RequestResult>` for asynchronous execution via `QtConcurrent::run()`.

### TokenPair (Thread-Safe)

**File:** `include/api/token_pair.h`

Stores access and refresh tokens with mutex protection for thread-safe access.

```cpp
namespace API {
    class TokenPair {
        void store(access, refresh);
        std::string getAccess() const;
        std::string getRefresh() const;
    private:
        std::string accessToken;
        std::string refreshToken;
        mutable std::mutex mutex;
    };
}
```

### RequestResult

**File:** `include/api/request_result.hpp`

Wraps API response with success status and JSON body.

```cpp
struct RequestResult : public ResultBase {
    nlohmann::json body;
    
    static RequestResult success(const nlohmann::json &body);
    static RequestResult error(const nlohmann::json &body);
    static RequestResult error_msg(const std::string& msg);
    
    std::string extractErrorDetails() const;
};
```

### FileData

**File:** `include/api/file_data.hpp`

Represents a file record from the server.

```cpp
struct FileData {
    std::string name;   // File name
    std::string path;   // Remote storage path
    size_t size;        // Size in bytes
    size_t id;          // File ID
};
```

### watchFuture Helper

**File:** `include/utils/watch_future.hpp`

Utility for handling async API calls with success/failure callbacks on the UI thread.

```cpp
template <typename FutureT, typename SuccessFn, typename FailureFn>
void watchFuture(QObject* parent, QFuture<FutureT> future, 
                 SuccessFn onSuccess, FailureFn onFailure);
```

Usage pattern:
```cpp
auto future = dispatch([&] { return ApiDispatcher::instance().getFiles(); });
watchFuture(this, future,
    [](const API::RequestResult& result) { /* success */ },
    [](const API::RequestResult& result) { /* failure */ }
);
```

## UI Library (`ui_elements`)

Static library containing all Qt widgets and windows. Links against `core`.

### Window Hierarchy

| Window/Dialog | Purpose | Key Signals/Slots |
|---------------|---------|-------------------|
| `LoginWindow` | User authentication | `onLoginButtonClicked()` → `ApiDispatcher::login()` |
| `UserDashboard` | Main file management view | `onUploadButtonClicked()`, `onLogoutButtonClicked()` |
| `UploadDialog` | Configure encryption and upload | Returns `EncryptOptions` + file path |
| `DecryptDialog` | Select encrypted file + key to decrypt | Returns file path + key path |
| `ShareCodeDialog` | Display share code with copy button | Shows code, emits refresh signal |
| `DownloadSharedDialog` | Enter share code to download | Returns share code |
| `UploadedFilePanel` | Single file row widget | `shareButtonPressed`, `downloadButtonPressed`, `deleteButtonPressed` |

### Signal/Slot Connections

```
LoginWindow
    └─ login success → UserDashboard (created, shown)
    └─ login success → LoginWindow (hidden)

UserDashboard
    └─ upload → UploadDialog → ApiDispatcher::uploadFile()
    └─ download → ApiDispatcher::downloadFile()
    └─ delete → ApiDispatcher::deleteFile()
    └─ share → ApiDispatcher::shareFile() → ShareCodeDialog
    └─ decrypt → DecryptDialog → FileCrypto::decryptFile()
    └─ download shared → DownloadSharedDialog → ApiDispatcher::downloadSharedFile()
    └─ logout → LoginWindow (shown), UserDashboard (closed)
    
ApiDispatcher
    └─ sessionExpired() → LoginWindow (shown), Dashboard (closed)
```

## Cryptography Module

### Algorithm Types

**File:** `include/cryptography/algorithm_types.h`

```cpp
enum class AlgorithmType { AES = 0, Hybrid };
enum class AESType { AES_128 = 16, AES_192 = 24, AES_256 = 32 };
```

### CryptoService (High-Level)

**File:** `include/cryptography/crypto_service.h`

Orchestrates encryption/decryption operations.

```cpp
class CryptoService {
    struct EncryptResult { ByteArray encryptedContent; std::string encryptedFileName; nlohmann::json config; };
    struct DecryptResult { ByteArray content; std::string decryptedFileName; };
    
    static EncryptResult Encrypt(content, fileName, aesType, algorithmType, rsaKeySize, encryptName);
    static DecryptResult Decrypt(content, fileName, config, decryptName);
};
```

### FileCrypto (File-Level Operations)

**File:** `include/cryptography/file_crypto.h`

Handles reading/writing files and generating key files.

```cpp
class FileCrypto {
    struct EncryptOptions { filePath, keyPath, aesType, algorithm, rsaKeySize, encryptFileName; };
    struct DecryptOptions { filePath, keyPath, decryptFileName; };
    struct Result : ResultBase { path, fileName; };
    
    static Result encryptFile(options);
    static Result decryptFile(options);
};
```

### AES Module

**File:** `include/cryptography/aes.h`

Implements AES encryption with configurable key sizes.

```cpp
namespace Cryptography::AES {
    struct KeyData : Key { AESKey key; ByteArray iv; AESType type; };
    
    KeyData generateKey(AESType type);
    KeyData parseKey(const nlohmann::json& config);
    ByteArray encrypt(const KeyData& config, const ByteArray& input);
    ByteArray decrypt(const KeyData& config, const ByteArray& ciphertext);
}
```

### RSA Module

**File:** `include/cryptography/rsa.h`

Uses OpenSSL for RSA key generation and encryption.

```cpp
namespace Cryptography::RSA {
    struct KeyData : Key { EVP_PKEY* keyPair; };
    
    KeyData generateKey(size_t key_size);
    KeyData parseKey(const nlohmann::json& config);
    ByteArray encrypt(const KeyData& config, const ByteArray& input);
    ByteArray decrypt(const KeyData& config, const ByteArray& ciphertext);
}
```

## Backend Communication Flow

### Authentication Flow

```
User enters credentials
        │
        ▼
LoginWindow::onLoginButtonClicked()
        │
        ▼
ApiDispatcher::login(email, password_hash)
        │  POST /api/token/  {email, password}
        ▼
Backend returns { access, refresh }
        │
        ▼
ApiDispatcher::storeTokens(access, refresh)
        │
        ▼
TokenRefreshTimer starts (9 minute interval)
        │
        ▼
Login successful → UserDashboard shown
```

### Token Refresh Flow

```
QTimer fires (every 9 minutes)
        │
        ▼
ApiDispatcher::onRefreshTimer()
        │  POST /api/token/refresh/  {refresh: refreshToken}
        ▼
If success: store new access token
If fail: emit sessionExpired() → return to LoginWindow
```

### File Upload Flow

```
UserDashboard::onUploadButtonClicked()
        │
        ▼
UploadDialog (select file, encryption options)
        │
        ▼
FileCrypto::encryptFile(options) → writes encrypted file + .crbkey file
        │
        ▼
ApiDispatcher::uploadFile(fileName, encryptedFilePath)
        │  POST /api/upload/  multipart/form-data {json: metadata, file: encryptedFile}
        ▼
Server stores file, returns file record
        │
        ▼
UserDashboard::addFile(fileData) → adds UploadedFilePanel
```

## Build Targets

| Target | Type | Depends On |
|--------|------|------------|
| `core` | Static library | Qt6::Core, Qt6::Concurrent, curlpp, nlohmann_json, OpenSSL |
| `ui_elements` | Static library | `core`, Qt6::Widgets |
| `storage-crab-desktop` | Executable | `core`, `ui_elements` |
