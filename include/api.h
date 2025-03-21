#ifndef API_SETTINGS_HPP
#define API_SETTINGS_HPP

#include <string>

namespace API {

// Base URL for API access
const std::string API_BASE_URL = "http://54.172.153.171:8080/api/";

// URL for token obtain (login)
const std::string TOKEN_OBTAIN_URL = API_BASE_URL + "token/get/";

// URL for token refresh
const std::string TOKEN_REFRESH_URL = API_BASE_URL + "token/refresh/";

// URL for retrieving files related to the user
const std::string GET_FILES_URL = API_BASE_URL + "files/";

// URL for uploading files
const std::string UPLOAD_URL = API_BASE_URL + "files/upload/";

// URL builder for downloading files
inline auto DOWNLOAD_URL_FOR = [](const size_t fileID) {
    return API_BASE_URL + "files/download/" + std::to_string(fileID) + "/";
};

// URL builder for deleting files
inline auto DELETE_URL_FOR = [](const size_t fileID) {
    return API_BASE_URL + "files/delete/" + std::to_string(fileID) + "/";
};

}

#endif //API_SETTINGS_HPP
