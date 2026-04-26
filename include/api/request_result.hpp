#pragma once

#include <nlohmann/json.hpp>

#include "result_base.hpp"

namespace API {

// Provides a way to summarise a request result.
struct RequestResult : public ResultBase {
    nlohmann::json body;

    static RequestResult success(const nlohmann::json &body) { return RequestResult{ { .ok = true }, .body = body }; }
    static RequestResult success() { return RequestResult{{ .ok = true }, .body = nlohmann::json() }; }
    static RequestResult error(const nlohmann::json &body) { return RequestResult{ { .ok = false }, .body = body }; }
    static RequestResult error_msg(const std::string& msg) { return RequestResult{ { .ok = false }, .body = {{"details", msg}} }; }

    // @returns Error details string if the request was not successful,
    // otherwise returns an empty string
    std::string extractErrorDetails() const { 
        if (!ok && !body.empty())
            return body.at("details").get<std::string>();
        return "";
    }

};

}