#ifndef REQUESTS_HPP
#define REQUESTS_HPP

#include <fstream>
#include <functional>

#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>

#include <QDir>

#include "token_pair.h"

namespace API::Requests {

inline RequestResult POST(
    const std::string& url,
    const nlohmann::json &body,
    const std::string& access_token = ""
) {
    try {
        cURLpp::Easy request;
        request.setOpt(cURLpp::options::Url(url));

        request.setOpt(cURLpp::options::SslVerifyPeer(false));
        request.setOpt(cURLpp::options::SslVerifyHost(false));

        // String stream for retrieving
        std::ostringstream responseStream;

        std::list<std::string> headers;
        // Informing that we are using JSON
        headers.push_back("Content-Type: application/json");

        // Add authorization field if access token is provided
        if (!access_token.empty())
            headers.push_back("Authorization: Bearer " + access_token);

        request.setOpt(cURLpp::options::HttpHeader(headers));
        
        // Adding the body and its size to request
        const std::string bodyStr = body.dump();
        request.setOpt(curlpp::options::PostFields(bodyStr));
        request.setOpt(curlpp::options::PostFieldSize(static_cast<long>(bodyStr.length())));
        request.setOpt(curlpp::options::WriteStream(&responseStream));

        // Timeout
        request.setOpt(curlpp::options::Timeout(20L)); // 20 seconds

        // Performing the request
        request.perform();

        // Parse response if present. No content code
        if (cURLpp::infos::ResponseCode::get(request) == 204)
            return RequestResult::success();

        const nlohmann::json response = nlohmann::json::parse(responseStream.str());
        
        // Handle failure
        if (response.contains("details"))
            return RequestResult::error(response);

        return RequestResult::success(response);
    } catch (cURLpp::RuntimeError& e) {
        return RequestResult::error_msg("Request timed out");
    } catch (cURLpp::LogicError& e) {
        return RequestResult::error_msg("Logic error: " + std::string(e.what()));
    } catch (std::exception& e) {
        return RequestResult::error_msg("Other error: " + std::string(e.what()));
    }
}

// POST overload for sending files
inline RequestResult POST_UPLOAD(
    const std::string& url,
    const nlohmann::json &metadata,         // Metadata for a file
    const std::string& filepath,            // Path to a file
    const std::string& access_token = ""
) {
    try {
        cURLpp::Easy request;
        request.setOpt(cURLpp::options::Url(url));

        request.setOpt(cURLpp::options::SslVerifyPeer(false));
        request.setOpt(cURLpp::options::SslVerifyHost(false));

        // String stream for retrieving
        std::ostringstream responseStream;
        request.setOpt(cURLpp::options::WriteStream(&responseStream));

        std::list<std::string> headers;
        // Informing that we are using JSON
        headers.push_back("Content-Type: multipart/form-data");

        // Add authorization field if access token is provided
        if (!access_token.empty())
            headers.push_back("Authorization: Bearer " + access_token);

        request.setOpt(cURLpp::options::HttpHeader(headers));

        // Forming request
        cURLpp::Forms formParts;
        formParts.push_back(new cURLpp::FormParts::Content(
            "json", metadata.dump(),
            "application/json"
        ));
        formParts.push_back(new cURLpp::FormParts::File("file", filepath));

        request.setOpt(cURLpp::options::HttpPost(formParts));

        // Performing a request
        request.perform();

        // Parsing the response
        const nlohmann::json response = nlohmann::json::parse(responseStream.str());

        // Fail
        if (response.contains("details"))
            return RequestResult::error(response);

        return RequestResult::success(response);
    } catch (cURLpp::RuntimeError& e) {
        return RequestResult::error_msg("Request timed out");
    } catch (cURLpp::LogicError& e) {
        return RequestResult::error_msg("Logic error: " + std::string(e.what()));
    }
}

// Downloads the file to destination on successfull request
inline RequestResult GET_DOWNLOAD(
    const std::string &url,
    const std::string &destination,
    const std::string &access_token = ""
) {
    try {
        cURLpp::Easy request;
        request.setOpt(cURLpp::options::Url(url));

        request.setOpt(cURLpp::options::SslVerifyPeer(false));
        request.setOpt(cURLpp::options::SslVerifyHost(false));

        std::string responseBody;
        std::string filename;

        // Get filename from headers
        request.setOpt(curlpp::options::HeaderFunction([&](char *data, size_t size, size_t nmemb) {
            std::string header(data, size * nmemb);
            
            if (header.find("content-disposition") != std::string::npos) {
                auto pos = header.find("filename=\"");
                if (pos != std::string::npos) {
                    pos += 10; // skipping 'filename="'
                    auto end = header.find("\"", pos);
                    filename = header.substr(pos, end - pos);
                }
            }
            return size * nmemb;
        }));

        // Capture response body
        request.setOpt(cURLpp::options::WriteFunction([&](char *data, size_t size, size_t nmemb) {
            responseBody.append(data, size * nmemb);
            return size * nmemb;
        }));

        if (!access_token.empty())
            request.setOpt(cURLpp::options::HttpHeader({"Authorization: Bearer " + access_token}));

        request.perform();

        int responseCode = cURLpp::infos::ResponseCode::get(request);

        if (responseCode != 200) {
            // Parse error response
            const nlohmann::json response = nlohmann::json::parse(responseBody);
            return RequestResult::error(response);
        }

        // Write file
        std::string path = QDir::toNativeSeparators(
            QDir::cleanPath(QString::fromStdString(destination + "/" + filename))
        ).toStdString();
        std::ofstream file(path, std::ios::binary);
        file.write(responseBody.c_str(), responseBody.size());
        file.close();

        return RequestResult::success();

    } catch (cURLpp::RuntimeError& e) {
        return RequestResult::error_msg("Request failed: " + std::string(e.what()));
    } catch (const std::exception& e) {
        return RequestResult::error_msg(std::string(e.what()));
    }
}

inline RequestResult GET(
    const std::string& url,
    const std::string& access_token = ""
) {
    try {
        cURLpp::Easy request;
        request.setOpt(cURLpp::options::Url(url));

        request.setOpt(cURLpp::options::SslVerifyPeer(false));
        request.setOpt(cURLpp::options::SslVerifyHost(false));

        // String stream for retrieving
        std::ostringstream responseStream;

        std::list<std::string> headers;
        // Informing that we are using JSON
        headers.push_back("Content-Type: multipart/form-data");

        // Add authorization field if access token is provided
        if (!access_token.empty())
            headers.push_back("Authorization: Bearer " + access_token);

        request.setOpt(cURLpp::options::HttpHeader(headers));

        request.setOpt(cURLpp::options::WriteStream(&responseStream));

        // Timeout
        request.setOpt(curlpp::options::Timeout(20L)); // 20 seconds

        // Performing the request
        request.perform();

        // Parsing the response
        const nlohmann::json response = nlohmann::json::parse(responseStream.str());

        // Fail
        if (response.contains("details"))
            return RequestResult::error(response);
        return RequestResult::success(response);
    } catch (cURLpp::RuntimeError& e) {
        return RequestResult::error_msg("Request timed out");
    } catch (cURLpp::LogicError& e) {
        return RequestResult::error_msg("Logic error: " + std::string(e.what()));
    }
}

}

#endif //REQUESTS_HPP
