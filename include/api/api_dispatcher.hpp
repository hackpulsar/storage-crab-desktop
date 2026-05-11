#ifndef API_DISPATCHER_HPP
#define API_DISPATCHER_HPP

#include <QFuture>
#include <QTimer>

#include "api/token_pair.h"

typedef QFuture<API::RequestResult> RequestResultFuture;

// Singleton encapsulating API calls
class ApiDispatcher final : public QObject { 
Q_OBJECT

public:
    static ApiDispatcher& instance() {
        static ApiDispatcher instance;
        return instance;
    }

    ApiDispatcher(const ApiDispatcher&) = delete;
    ApiDispatcher& operator=(const ApiDispatcher&) = delete;

    ~ApiDispatcher();

    void storeTokens(const std::string& access, const std::string& refresh);
    
    RequestResultFuture login(const std::string& email, const std::string& password_hash) const;

    RequestResultFuture register_user(const std::string& email, const std::string& username, const std::string& password_hash) const;

    RequestResultFuture me();

    RequestResultFuture shareFile(const size_t fileID);

    RequestResultFuture downloadFile(const size_t fileID, const std::string& destination);

    RequestResultFuture downloadSharedFile(const std::string& code, const std::string& destination);

    RequestResultFuture deleteFile(const size_t fileID);

    RequestResultFuture uploadFile(const std::string& fileName, const std::string& path);

    RequestResultFuture getFiles();

signals:
    void sessionExpired();

private slots:
    void onRefreshTimer();

private:
    ApiDispatcher();

    API::TokenPair tokenPair;
    QTimer* tokenRefreshTimer;

    // For waiting for refresh thread to finish on destruction
    QFuture<void> refreshFuture;

};

#endif // API_DISPATCHER_HPP