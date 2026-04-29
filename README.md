# 🦀 Storage Crab Desktop

A Qt/C++ desktop client for Storage Crab cloud storage with client-side encryption. Upload, download, and share files securely using AES and RSA encryption.

## Features

- **Secure Authentication** - Email/password login with JWT access/refresh token pair
- **Client-Side Encryption** - Files encrypted locally before upload
  - AES-128, AES-192, AES-256 encryption
  - Hybrid mode (AES + RSA)
  - Optional file name encryption
- **File Management** - Upload, download, delete, and share files
- **File Sharing** - Generate share codes for others to download files
- **Shared Downloads** - Download files using share codes without an account
- **Decrypt Tool** - Decrypt downloaded encrypted files using key files

## Prerequisites

- CMake 3.29+
- C++23 compatible compiler
- Qt6 (Core, Widgets, Concurrent)
- OpenSSL
- libcurl
- Git (for submodules)

### macOS

```bash
brew install cmake qt@6 openssl curl
```

### Ubuntu/Debian

```bash
sudo apt-get install -y \
  cmake \
  build-essential \
  libgl1-mesa-dev \
  libcurl4-openssl-dev \
  libssl-dev \
  qt6-base-dev \
  libqt6concurrent6 \
  qt6-base-dev-tools
```

## Build Instructions

```bash
git clone --recurse-submodules https://github.com/yourusername/storage-crab-desktop.git
cd storage-crab-desktop
cmake -B build -DCMAKE_BUILD_TYPE=Release
cd build && make
```

## Configuration

Create a `.env` file in the project root with backend API URLs:

```env
TOKEN_OBTAIN_URL=http://localhost:8000/api/token/
TOKEN_REFRESH_URL=http://localhost:8000/api/token/refresh/
ME_URL=http://localhost:8000/api/me/
UPLOAD_URL=http://localhost:8000/api/upload/
GET_FILES_URL=http://localhost:8000/api/files/
DOWNLOAD_URL=http://localhost:8000/api/download/
DELETE_URL=http://localhost:8000/api/delete/
SHARE_URL=http://localhost:8000/api/share/
DOWNLOAD_SHARED_URL=http://localhost:8000/api/shared/
```

## Running Tests

```bash
chmod +x run_tests.sh
./run_tests.sh
```

## Project Structure

```
storage-crab-desktop/
├── assets/              # Image resources (icons, animations)
├── deps/                # Dependencies (submodules)
├── docs/                # Documentation
├── forms/               # Qt UI form files (.ui)
├── include/             # Header files
│   ├── api/             # API communication
│   ├── cryptography/    # Encryption classes
│   ├── utils/           # Utility headers
│   ├── widgets/         # UI widget headers
│   └── windows/         # Window headers
├── src/                 # Source files
│   ├── api/             # API implementation
│   ├── cryptography/    # Encryption implementation
│   ├── widgets/         # Widget implementation
│   ├── windows/         # Window implementation
│   └── main.cpp         # Entry point
├── tests/               # Unit tests
│   ├── logic/           # Core logic tests
│   ├── ui/              # UI component tests
│   └── fixtures/        # Files used in tests
├── CMakeLists.txt       # Build configuration
└── run_tests.sh         # Test runner script
```

## Architecture Overview

The application is split into two static libraries:

- **core** - API communication and cryptography logic (no Qt UI dependency)
- **ui_elements** - Qt widgets and windows

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for detailed architecture.

## Documentation
See the [docs](docs/) directory for detailed documentation:
- [API Integration](docs/API_INTEGRATION.md)
- [Architecture](docs/ARCHITECTURE.md)
- [Encryption](docs/ENCRYPTION.md)

## License

Apache 2.0 License - see [LICENSE](LICENSE)
