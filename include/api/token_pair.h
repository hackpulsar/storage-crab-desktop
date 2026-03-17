#ifndef TOKEN_PAIR_H
#define TOKEN_PAIR_H

#include <string>
#include <mutex>

#include "api/request_result.hpp"

namespace API {

// Represents an Access/Refresh token pair.
// Used for communication with the API.
class TokenPair final {
public:
    TokenPair() = default;

    TokenPair(std::string access, std::string refresh);

    ~TokenPair() = default;

    void store(const std::string& access, const std::string& refresh);

    std::string getAccess() const;

    void setAccess(const std::string &access);

    std::string getRefresh() const;

    void setRefresh(const std::string &refresh);

private:
    // The tokens
    std::string accessToken;
    std::string refreshToken;

    // Mutex to protect the token
    mutable std::mutex mutex;
};

}

#endif //TOKEN_PAIR_H
