#ifndef TOKEN_PAIR_H
#define TOKEN_PAIR_H

#include <string>
#include <mutex>

namespace API {
    // Provides a way to summarise a request result.
    struct RequestResult {
        bool ok;
        std::string message;

        static RequestResult success() { return RequestResult{true}; }
        static RequestResult error(const std::string& message) { return RequestResult{false, message}; }
    };

    // Represents an Access/Refresh token pair.
    // Used for communication with the API.
    class TokenPair final {
    public:
        TokenPair() = default;
        TokenPair(const std::string& access, const std::string& refresh);

        ~TokenPair() = default;

        // Returns access token
        [[nodiscard]] std::string getAccess() const;

        // Sets access token
        void setAccess(const std::string& access);

        // Returns refresh token
        [[nodiscard]] std::string getRefresh() const;

        // Sets refresh token
        void setRefresh(const std::string& refresh);

        // Sends a refresh POST request.
        RequestResult refresh();

    private:
        // The tokens
        std::string accessToken;
        std::string refreshToken;

        // Mutex to protect the token
        mutable std::mutex mutex;
    };

} // API

#endif //TOKEN_PAIR_H
