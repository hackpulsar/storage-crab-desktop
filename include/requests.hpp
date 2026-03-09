#ifndef REQUESTS_HPP
#define REQUESTS_HPP

#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>
#include <qdebug.h>
#include <fstream>

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

        // Parse response if present. No content code
        if (cURLpp::infos::ResponseCode::get(request) == 204)
            return RequestResult::success();

        const nlohmann::json response = nlohmann::json::parse(responseStream.str());
        // Handle failure
        if (response.contains("details"))
            return RequestResult::error(response);

        return RequestResult::success(response);
    } catch (cURLpp::RuntimeError&) {
        return RequestResult::error("Runtime error");
    } catch (cURLpp::LogicError&) {
        return RequestResult::error("Logic error");
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

        // String stream for retrieving
        std::ostringstream responseStream;
        request.setOpt(cURLpp::options::WriteStream(&responseStream));

        // Forming header
        request.setOpt(cURLpp::options::HttpHeader({"Content-Type: multipart/form-data"}));
        // Add authorization field if access token is provided
        if (!access_token.empty())
            request.setOpt(cURLpp::options::HttpHeader({"Authorization: Bearer " + access_token}));

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
    } catch (cURLpp::RuntimeError&) {
        return RequestResult::error("Runtime error");
    } catch (cURLpp::LogicError&) {
        return RequestResult::error("Logic error");
    }
}

inline RequestResult GET_DOWNLOAD(
    const std::string &url,
    const std::string &destination,
    const std::string &access_token = ""
) {
    try {
        cURLpp::Easy request;
        request.setOpt(cURLpp::options::Url(url));

        std::ostringstream responseStream;

        request.setOpt(cURLpp::options::WriteStream(&responseStream));

        // Reading file details
        std::string filename;
        request.setOpt(curlpp::options::HeaderFunction([&](char *data, size_t size, size_t nmemb) {
            std::string header(data, size * nmemb);
            
            if (header.find("content-disposition") != std::string::npos) {
                auto pos = header.find("filename=\"");
                if (pos != std::string::npos) {
                    pos += 10; // skip 'filename="'
                    auto end = header.find("\"", pos);
                    filename = header.substr(pos, end - pos);
                }
            }
            return size * nmemb;
        }));

        // File download function
        std::ofstream file;
        request.setOpt(cURLpp::options::WriteFunction([&](char *data, size_t size, size_t nmemb) {
            size_t written = size * nmemb;
            
            // Open file lazily once we have the filename from headers
            if (!file.is_open()) {
                std::string path = QDir::toNativeSeparators(
                    QDir::cleanPath(QString::fromStdString(destination + "/" + filename))
                ).toStdString();
                file.open(path, std::ios::binary);
                if (!file) throw cURLpp::RuntimeError("Error writing to a file");
            }
            
            file.write(data, written);
            return written;
        }));

        // Add authorization field if access token is provided
        if (!access_token.empty())
            request.setOpt(cURLpp::options::HttpHeader({"Authorization: Bearer " + access_token}));

        // Performing the request
        request.perform();

        int responseCode = cURLpp::infos::ResponseCode::get(request);

        if (responseCode == 200)
            return RequestResult::success();

        const nlohmann::json response = nlohmann::json::parse(responseStream.str());
        return RequestResult::error(response);
    } catch (cURLpp::RuntimeError& e) {
        return RequestResult::error(nlohmann::json{{"details", e.what()}});
    } catch (cURLpp::LogicError& e) {
        return RequestResult::error(nlohmann::json{{"details", e.what()}});
    }
}

inline RequestResult GET(
    const std::string& url,
    const std::string& access_token = ""
) {
    try {
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
