#ifndef API_SETTINGS_HPP
#define API_SETTINGS_HPP

#include <dotenv.h>

namespace API {

// URL builder for downloading files
inline auto DOWNLOAD_URL_FOR = [](const size_t fileID) {
    return std::getenv("DOWNLOAD_URL") + std::to_string(fileID) + "/";
};

// URL builder for deleting files
inline auto DELETE_URL_FOR = [](const size_t fileID) {
    return std::getenv("DELETE_URL") + std::to_string(fileID) + "/";
};

}

#endif //API_SETTINGS_HPP
