#ifndef API_SETTINGS_HPP
#define API_SETTINGS_HPP

#include <dotenv.h>

namespace API {

// URL builder helper functions

inline auto DOWNLOAD_URL_FOR = [](const size_t fileID) {
    return std::getenv("DOWNLOAD_URL") + std::to_string(fileID) + "/";
};

inline auto DELETE_URL_FOR = [](const size_t fileID) {
    return std::getenv("DELETE_URL") + std::to_string(fileID) + "/";
};

inline auto SHARE_URL_FOR = [](const size_t fileID) {
    return std::getenv("SHARE_URL") + std::to_string(fileID) + "/";
};

inline auto DOWNLOAD_SHARED_URL_FOR = [](const std::string& code) {
    return std::getenv("DOWNLOAD_SHARED_URL") + code + "/";
};

}

#endif //API_SETTINGS_HPP
