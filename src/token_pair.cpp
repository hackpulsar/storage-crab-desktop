#include "token_pair.h"

#include <sstream>

#include "api.h"

#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>
#include <nlohmann/json.hpp>
#include <QtCore/qlogging.h>

namespace API {
    TokenPair::TokenPair(const std::string& access, const std::string& refresh)
        : accessToken(access), refreshToken(refresh)
    {}

    RequestResult TokenPair::refresh() {
        try {
            // Form a token refresh request
            cURLpp::Easy refresh_request;
            refresh_request.setOpt(cURLpp::options::Url(TOKEN_REFRESH_URL));

            // Request body
            const nlohmann::json body = {
                {"refresh_token", this->getRefresh()},
            };

            // String stream for retrieving
            std::ostringstream responseStream;

            // Informing that we are using JSON
            refresh_request.setOpt(cURLpp::options::HttpHeader({"Content-Type: application/json"}));
            // Adding the body and its size to request
            refresh_request.setOpt(curlpp::options::PostFields(body.dump()));
            refresh_request.setOpt(curlpp::options::PostFieldSize(static_cast<long>(body.dump().length())));
            refresh_request.setOpt(cURLpp::options::WriteStream(&responseStream));

            // Performing the request
            refresh_request.perform();

            // Parsing the response
            const nlohmann::json response = nlohmann::json::parse(responseStream.str());

            // Update access token
            if (response.contains("access_token") && response.contains("refresh_token")) {
                this->setAccess(response.at("access_token").get<std::string>());
                this->setRefresh(response.at("refresh_token").get<std::string>());
                std::cout << "yoo: " << this->getRefresh() << std::endl;
                return RequestResult::success();
            }

            // Fail
            if (response.contains("details"))
                return RequestResult::error(response.at("details"));
            return RequestResult::error("Unknown error");
        } catch (cURLpp::RuntimeError&) {
            return RequestResult::error("Runtime error");
        } catch (cURLpp::LogicError&) {
            return RequestResult::error("Logic error");
        }
    }

    std::string TokenPair::getAccess() const {
        std::lock_guard lock(this->mutex);
        return this->accessToken;
    }

    void TokenPair::setAccess(const std::string& access) {
        std::lock_guard lock(this->mutex);
        this->accessToken = access;
    }

    std::string TokenPair::getRefresh() const {
        std::lock_guard lock(this->mutex);
        return this->refreshToken;
    }

    void TokenPair::setRefresh(const std::string& refresh) {
        std::lock_guard lock(this->mutex);
        this->refreshToken = refresh;
    }

} // API