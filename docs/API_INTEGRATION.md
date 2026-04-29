# API Integration Guide

This document describes how the Storage Crab Desktop frontend communicates with the backend API.

## Configuration

API endpoints are configured via environment variables loaded from a `.env` file using `dotenv-cpp`.

### Environment Variables

Create a `.env` file in the project root:

```env
# Authentication
TOKEN_OBTAIN_URL=http://localhost:8000/api/token/
TOKEN_REFRESH_URL=http://localhost:8000/api/token/refresh/

# User info
ME_URL=http://localhost:8000/api/me/

# File operations
UPLOAD_URL=http://localhost:8000/api/upload/
GET_FILES_URL=http://localhost:8000/api/files/
DOWNLOAD_URL=http://localhost:8000/api/download/
DELETE_URL=http://localhost:8000/api/delete/
SHARE_URL=http://localhost:8000/api/share/

# Shared downloads (no auth required)
DOWNLOAD_SHARED_URL=http://localhost:8000/api/shared/
```

### URL Builder Functions

**File:** `include/api/api.h`

```cpp
namespace API {
    inline auto DOWNLOAD_URL_FOR = [](size_t fileID) {
        return std::getenv("DOWNLOAD_URL") + std::to_string(fileID) + "/";
    };
    
    inline auto DELETE_URL_FOR = [](size_t fileID) {
        return std::getenv("DELETE_URL") + std::to_string(fileID) + "/";
    };
    
    inline auto SHARE_URL_FOR = [](size_t fileID) {
        return std::getenv("SHARE_URL") + std::to_string(fileID) + "/";
    };
    
    inline auto DOWNLOAD_SHARED_URL_FOR = [](const std::string& code) {
        return std::getenv("DOWNLOAD_SHARED_URL") + code + "/";
    };
}
```

## Authentication

### Login

**Endpoint:** `POST /api/token/` (configured via `TOKEN_OBTAIN_URL`)

**Request:**
```json
{
    "email": "user@example.com",
    "password": "<hashed_password>"
}
```

**Response (Success - 200):**
```json
{
    "access": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
    "refresh": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
}
```

**Response (Failure - 401):**
```json
{
    "details": "Invalid credentials"
}
```

**Frontend Implementation:**
```cpp
// LoginWindow triggers this
auto future = dispatch([&] { 
    return ApiDispatcher::instance().login(email, password_hash); 
});

watchFuture(this, future,
    [this](const API::RequestResult& result) {
        // result.body contains {access, refresh}
        onLoginSuccessfull(result);
    },
    [this](const API::RequestResult& result) {
        // result.extractErrorDetails() for error message
    }
);
```

### Token Refresh

**Endpoint:** `POST /api/token/refresh/` (configured via `TOKEN_REFRESH_URL`)

**Request:**
```json
{
    "refresh": "<refresh_token>"
}
```

**Response (Success - 200):**
```json
{
    "access": "<new_access_token>"
}
```

**Response (Failure - 401):**
```json
{
    "details": "Token is invalid or expired"
}
```

The `ApiDispatcher` automatically refreshes the access token every 9 minutes using a `QTimer`. If refresh fails, the `sessionExpired()` signal is emitted.

### Get Current User

**Endpoint:** `GET /api/me/` (configured via `ME_URL`)

**Headers:**
```
Authorization: Bearer <access_token>
```

**Response (Success - 200):**
```json
{
    "id": 1,
    "email": "user@example.com",
    "username": "user"
}
```

## File Operations

### Upload File

**Endpoint:** `POST /api/upload/` (configured via `UPLOAD_URL`)

**Headers:**
```
Authorization: Bearer <access_token>
Content-Type: multipart/form-data
```

**Body (multipart/form-data):**
- `json`: JSON metadata string
- `file`: The encrypted file

**JSON Metadata:**
```json
{
    "filename": "document.txt",
    "filesize": 1024,
    "encryption_config": {
        "algorithm": "AES",
        "aes_type": 32,
        "encrypt_name": false
    }
}
```

**Response (Success - 201):**
```json
{
    "id": 123,
    "name": "document.txt",
    "size": 1024,
    "path": "/storage/123/document.txt"
}
```

**Frontend Implementation:**
```cpp
// File is encrypted locally before upload
auto future = dispatch([&] { 
    return ApiDispatcher::instance().uploadFile(fileName, encryptedFilePath); 
});
```

### List Files

**Endpoint:** `GET /api/files/` (configured via `GET_FILES_URL`)

**Headers:**
```
Authorization: Bearer <access_token>
```

**Response (Success - 200):**
```json
[
    {
        "id": 123,
        "name": "document.txt",
        "size": 1024,
        "path": "/storage/123/document.txt"
    },
    {
        "id": 124,
        "name": "image.png",
        "size": 2048,
        "path": "/storage/124/image.png"
    }
]
```

**Frontend Implementation:**
```cpp
auto future = dispatch([&] { return ApiDispatcher::instance().getFiles(); });
watchFuture(this, future,
    [this](const API::RequestResult& result) {
        // result.body is a JSON array of file objects
        for (auto& fileJson : result.body) {
            FileData data{fileJson["name"], fileJson["path"], fileJson["size"], fileJson["id"]};
            addFile(data);
        }
    },
    [](const API::RequestResult& result) { /* handle error */ }
);
```

### Download File

**Endpoint:** `GET /api/download/{fileID}/` (configured via `DOWNLOAD_URL_FOR(fileID)`)

**Headers:**
```
Authorization: Bearer <access_token>
```

**Response (Success - 200):**
- Binary file content with `content-disposition` header containing filename

**Frontend Implementation:**
```cpp
auto future = dispatch([&] { 
    return ApiDispatcher::instance().downloadFile(fileID, destinationPath); 
});
```

### Delete File

**Endpoint:** `DELETE /api/delete/{fileID}/` (configured via `DELETE_URL_FOR(fileID)`)

**Headers:**
```
Authorization: Bearer <access_token>
```

**Response (Success - 204):**
- No content

**Frontend Implementation:**
```cpp
auto future = dispatch([&] { 
    return ApiDispatcher::instance().deleteFile(fileID); 
});
```

### Share File

**Endpoint:** `POST /api/share/{fileID}/` (configured via `SHARE_URL_FOR(fileID)`)

**Headers:**
```
Authorization: Bearer <access_token>
Content-Type: application/json
```

**Response (Success - 200):**
```json
{
    "code": "ABC123XYZ"
}
```

**Frontend Implementation:**
```cpp
auto future = dispatch([&] { 
    return ApiDispatcher::instance().shareFile(fileID); 
});
watchFuture(this, future,
    [this](const API::RequestResult& result) {
        // result.body["code"] contains the share code
        showShareCodeDialog(result.body["code"]);
    },
    [](const API::RequestResult& result) { /* handle error */ }
);
```

### Download Shared File

**Endpoint:** `GET /api/shared/{code}/` (configured via `DOWNLOAD_SHARED_URL_FOR(code)`)

**No authentication required.**

**Response (Success - 200):**
- Binary file content with `content-disposition` header

**Response (Failure - 404):**
```json
{
    "details": "Invalid or expired share code"
}
```

**Frontend Implementation:**
```cpp
auto future = dispatch([&] { 
    return ApiDispatcher::instance().downloadSharedFile(code, destinationPath); 
});
```

## Request Helpers

**File:** `include/api/requests.hpp`

Low-level HTTP request functions using cURLpp:

### POST (JSON)

```cpp
API::Requests::POST(url, json_body, access_token);
```

- Sets `Content-Type: application/json`
- Adds `Authorization: Bearer <token>` if token provided
- 20 second timeout
- SSL verification disabled (dev only)

### POST_UPLOAD (Multipart)

```cpp
API::Requests::POST_UPLOAD(url, metadata_json, filepath, access_token);
```

- Sets `Content-Type: multipart/form-data`
- Sends `json` metadata field and `file` field

### GET (JSON Response)

```cpp
API::Requests::GET(url, access_token);
```

- Returns parsed JSON response
- 20 second timeout

### GET_DOWNLOAD (Binary File)

```cpp
API::Requests::GET_DOWNLOAD(url, destination_path, access_token);
```

- Extracts filename from `content-disposition` header
- Writes binary content to `destination/filename`

## Error Handling

All API requests return `API::RequestResult`:

```cpp
struct RequestResult {
    bool ok;
    nlohmann::json body;
    
    // Check if error
    if (!result.ok) {
        std::string errorMsg = result.extractErrorDetails();
        // "Invalid credentials", "Token expired", etc.
    }
    
    // Use success response
    if (result.ok) {
        // result.body contains response JSON
    }
};
```

Common error responses:
- `401 Unauthorized` - Invalid/expired token
- `403 Forbidden` - Insufficient permissions
- `404 Not Found` - File or resource doesn't exist
- `500 Internal Server Error` - Server error

## Important Notes

### SSL Verification

SSL verification is currently **disabled** in all requests for development:

```cpp
request.setOpt(cURLpp::options::SslVerifyPeer(false));
request.setOpt(cURLpp::options::SslVerifyHost(false));
```

**Do not use in production** without enabling proper SSL verification.

### Async Pattern

All API calls are executed asynchronously using `QtConcurrent::run()`:

```cpp
// 1. Dispatch async call
auto future = dispatch([&] { return apiCall(); });

// 2. Watch for completion
watchFuture(this, future, onSuccess, onFailure);
```

This prevents blocking the UI thread during network operations.

### Token Lifecycle

1. User logs in → access token (10 min) + refresh token stored
2. `QTimer` triggers refresh every 9 minutes
3. If refresh fails → `sessionExpired()` signal → user must login again
4. All API calls automatically use the current access token
