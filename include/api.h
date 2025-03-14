#ifndef API_SETTINGS_HPP
#define API_SETTINGS_HPP

#include <string>

namespace API {

// Base URL for API access
const std::string API_BASE_URL = "http://13.61.0.183:8080/api/";

// URL for token obtain (login)
const std::string TOKEN_OBTAIN_URL = API_BASE_URL + "token/get/";

// URL for token refresh
const std::string TOKEN_REFRESH_URL = API_BASE_URL + "token/refresh/";

// URL for retrieving files related to the user
const std::string GET_FILES_URL = API_BASE_URL + "files/";

}

#endif //API_SETTINGS_HPP
