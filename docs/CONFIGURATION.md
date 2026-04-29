# Configuration

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