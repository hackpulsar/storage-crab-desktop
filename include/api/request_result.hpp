#pragma once

#include <nlohmann/json.hpp>

#include "result_base.hpp"

namespace API {

// Provides a way to summarise a request result.
struct RequestResult : public ResultBase {
    nlohmann::json body;

    RequestResult(bool ok, nlohmann::json body = {}) {
        this->ok = ok;
        this->body = std::move(body);
    }

    static RequestResult success(const nlohmann::json &body) { return RequestResult{ true, body }; }
    static RequestResult success() { return RequestResult{ true }; }
    static RequestResult error(const nlohmann::json &body) { return RequestResult{ false, body }; }
    static RequestResult error_msg(const std::string& msg) { return RequestResult{ false, {{"details", msg}} }; }

    // @returns Error details string if the request was not successful,
    // otherwise returns an empty string
    std::string extractErrorDetails() const { 
        if (!ok && !body.empty())
            return body.at("details").get<std::string>();
        return "";
    }

};

}