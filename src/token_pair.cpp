#include "token_pair.h"

#include <sstream>
#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>
#include <nlohmann/json.hpp>

#include "api.h"
#include "requests.hpp"

namespace API {

TokenPair::TokenPair(const std::string &access, const std::string &refresh)
    : accessToken(access), refreshToken(refresh) {
}

RequestResult TokenPair::refresh() {
    RequestResult result = Requests::POST(
        TOKEN_REFRESH_URL,
        {{"refresh_token", this->getRefresh()}}
    );

    if (result.ok) {
        this->setAccess(result.response.at("access_token").get<std::string>());
        this->setRefresh(result.response.at("refresh_token").get<std::string>());
        std::cout << "yoo: " << this->getRefresh() << std::endl;
    }

    return result;
}

std::string TokenPair::getAccess() const {
    std::lock_guard lock(this->mutex);
    return this->accessToken;
}

void TokenPair::setAccess(const std::string &access) {
    std::lock_guard lock(this->mutex);
    this->accessToken = access;
}

std::string TokenPair::getRefresh() const {
    std::lock_guard lock(this->mutex);
    return this->refreshToken;
}

void TokenPair::setRefresh(const std::string &refresh) {
    std::lock_guard lock(this->mutex);
    this->refreshToken = refresh;
}

} // API
