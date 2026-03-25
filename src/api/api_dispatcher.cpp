#include "api/api_dispatcher.hpp"

#include "api/api.h"
#include "api/requests.hpp"

#include "watch_future.hpp"

using namespace API;

ApiDispatcher::ApiDispatcher() : QObject(nullptr), tokenRefreshTimer(new QTimer(this)) {
    connect(this->tokenRefreshTimer, &QTimer::timeout, this, &ApiDispatcher::onRefreshTimer);
}

ApiDispatcher::~ApiDispatcher() {
    this->refreshFuture.waitForFinished();
}

void ApiDispatcher::storeTokens(const std::string& access, const std::string& refresh) {
    this->tokenPair.store(access, refresh);  

    // Access token lifetime is 10 minites, so we refresh 1 minute early
    this->tokenRefreshTimer->start(9 * 60 * 1000); // 9 minutes
}

RequestResultFuture ApiDispatcher::login(const std::string& email, const std::string& password_hash) const {
    return dispatch([email, password_hash] {
        return Requests::POST(
            std::getenv("TOKEN_OBTAIN_URL"),
            {
                {"email", email},
                {"password_hash", password_hash},
            }
        );
    });
}

RequestResultFuture ApiDispatcher::me() {
    return dispatch([this] {
        return Requests::GET(std::getenv("ME_URL"), this->tokenPair.getAccess());
    });
}

RequestResultFuture ApiDispatcher::shareFile(const size_t fileID) {
    return dispatch([this, fileID] {
        return Requests::POST(
            SHARE_URL_FOR(fileID),
            nlohmann::json(),
            this->tokenPair.getAccess()
        );
    });
}

RequestResultFuture ApiDispatcher::downloadFile(const size_t fileID, const std::string& destination) {
    return dispatch([this, fileID, destination] {
        return Requests::GET_DOWNLOAD(
            API::DOWNLOAD_URL_FOR(fileID),
            destination,
            this->tokenPair.getAccess()
        );
    });
}

RequestResultFuture ApiDispatcher::downloadSharedFile(const std::string& code, const std::string& destination) {
    return dispatch([this, code, destination] {
        return Requests::GET_DOWNLOAD(
            API::DOWNLOAD_SHARED_URL_FOR(code),
            destination,
            this->tokenPair.getAccess()
        );
    });
}

RequestResultFuture ApiDispatcher::deleteFile(const size_t fileID) {
    return dispatch([this, fileID] {
        return Requests::POST(
            API::DELETE_URL_FOR(fileID),
            nlohmann::json(),
            this->tokenPair.getAccess()
        );
    });
}

RequestResultFuture ApiDispatcher::uploadFile(const std::string& fileName, const std::string& path) {
    return dispatch([this, fileName, path] {
        return Requests::POST_UPLOAD(
            std::getenv("UPLOAD_URL"),
            {{"filename", fileName + ".enc"}},
            path,
            this->tokenPair.getAccess()
        );
    });
}

RequestResultFuture ApiDispatcher::getFiles() {
    return dispatch([this] {
        return Requests::GET(std::getenv("GET_FILES_URL"), this->tokenPair.getAccess());
    });
}

void ApiDispatcher::onRefreshTimer() {
    this->refreshFuture = dispatch([this] {
        RequestResult response = Requests::POST(
            std::getenv("TOKEN_REFRESH_URL"),
            {{"refresh_token", this->tokenPair.getRefresh()}}
        );

        if (response.ok) {
            std::string access = response.body.at("access_token").get<std::string>();
            std::string refresh = response.body.at("refresh_token").get<std::string>();

            this->tokenPair.store(access, refresh);
        } else
            emit sessionExpired();
    });
}