#include "api/token_pair.h"

#include <sstream>
#include <curlpp/Options.hpp>
#include <nlohmann/json.hpp>

#include "api/api.h"
#include "api/requests.hpp"

namespace API {

TokenPair::TokenPair(std::string access, std::string refresh)
    : accessToken(std::move(access))
    , refreshToken(std::move(refresh))
{ }

void TokenPair::store(const std::string& access, const std::string& refresh) {
    std::lock_guard lock(this->mutex);
    this->accessToken = access;
    this->refreshToken  = refresh;
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
