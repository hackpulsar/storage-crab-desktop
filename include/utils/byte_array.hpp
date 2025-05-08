#ifndef BYTE_ARRAY_HPP
#define BYTE_ARRAY_HPP

#include <vector>
#include <iomanip>
#include <sstream>

namespace Utils {

typedef unsigned char byte;
typedef std::vector<byte> ByteArray;

// Converts byte array to a string of HEX
inline std::string toHEX(const ByteArray &data) {
    std::ostringstream oss;
    for (const auto byte : data)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    return oss.str();
}

}

#endif //BYTE_ARRAY_HPP
