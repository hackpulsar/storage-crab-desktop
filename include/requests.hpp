#ifndef REQUESTS_HPP
#define REQUESTS_HPP

#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

namespace API::Requests {

inline RequestResult POST(
    const std::string& url,
    const nlohmann::json &body,
    const std::string& access_token = ""
) {
    try {
        // Form a token refresh request
        cURLpp::Easy request;
        request.setOpt(cURLpp::options::Url(url));

        // String stream for retrieving
        std::ostringstream responseStream;

        // Informing that we are using JSON
        request.setOpt(cURLpp::options::HttpHeader({"Content-Type: application/json"}));

        // Add authorization field if access token is provided
        if (!access_token.empty())
            request.setOpt(cURLpp::options::HttpHeader({"Authorization: Bearer " + access_token}));

        // Adding the body and its size to request
        request.setOpt(curlpp::options::PostFields(body.dump()));
        request.setOpt(curlpp::options::PostFieldSize(static_cast<long>(body.dump().length())));
        request.setOpt(cURLpp::options::WriteStream(&responseStream));

        // Performing the request
        request.perform();

        // Parsing the response
        const nlohmann::json response = nlohmann::json::parse(responseStream.str());

        // Fail
        if (response.contains("details"))
            return RequestResult::error(response);
        return RequestResult::success(response);
    } catch (cURLpp::RuntimeError&) {
        return RequestResult::error("Runtime error");
    } catch (cURLpp::LogicError&) {
        return RequestResult::error("Logic error");
    }
}

inline RequestResult GET(
    const std::string& url,
    const std::string& access_token = ""
) {
    try {
        // Form a token refresh request
        cURLpp::Easy request;
        request.setOpt(cURLpp::options::Url(url));

        // String stream for retrieving
        std::ostringstream responseStream;

        // Informing that we are using JSON
        request.setOpt(cURLpp::options::HttpHeader({"Content-Type: application/json"}));

        // Add authorization field if access token is provided
        if (!access_token.empty())
            request.setOpt(cURLpp::options::HttpHeader({"Authorization: Bearer " + access_token}));

        request.setOpt(cURLpp::options::WriteStream(&responseStream));

        // Performing the request
        request.perform();

        // Parsing the response
        const nlohmann::json response = nlohmann::json::parse(responseStream.str());

        // Fail
        if (response.contains("details"))
            return RequestResult::error(response);
        return RequestResult::success(response);
    } catch (cURLpp::RuntimeError&) {
        return RequestResult::error("Runtime error");
    } catch (cURLpp::LogicError&) {
        return RequestResult::error("Logic error");
    }
}

}

#endif //REQUESTS_HPP
