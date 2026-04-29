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
- [Configuration](docs/CONFIGURATION.md)
- [API Integration](docs/API_INTEGRATION.md)
- [Architecture](docs/ARCHITECTURE.md)
- [Encryption](docs/ENCRYPTION.md)

## License

Apache 2.0 License - see [LICENSE](LICENSE)
