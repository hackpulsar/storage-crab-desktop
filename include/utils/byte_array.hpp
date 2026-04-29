#ifndef BYTE_ARRAY_HPP
#define BYTE_ARRAY_HPP

#include <vector>
#include <iomanip>
#include <cstdint>
#include <sstream>

namespace Utils {

typedef unsigned char byte;
typedef std::vector<byte> ByteArray;

// Converts byte array to a string of HEX
inline std::string toHEX(const ByteArray& data) {
    std::ostringstream oss;
    for (const auto byte : data)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    return oss.str();
}

// Converts a string of HEX to a byte array
inline ByteArray toByteArray(const std::string& hex) {
    ByteArray bytes;
    bytes.reserve(hex.size() / 2);

    for (size_t i = 0; i < hex.size(); i += 2) {
        uint8_t byte = static_cast<uint8_t>(std::stoi(hex.substr(i, 2), nullptr, 16));
        bytes.push_back(byte);
    }

    return bytes;
}

}

#endif //BYTE_ARRAY_HPP
