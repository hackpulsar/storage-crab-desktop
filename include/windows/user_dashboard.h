#ifndef USER_DASHBOARD_H
#define USER_DASHBOARD_H

#include <QMainWindow>
#include <ui_user_dashboard.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QScrollArea>

#include <thread>
#include <nlohmann/json_fwd.hpp>

#include "token_pair.h"
#include "widgets/uploaded_file_panel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class UserDashboard final : public QMainWindow {
Q_OBJECT

public:
    UserDashboard(
        const API::TokenPair& tokenPair,
        const std::string& username,
        QWidget *parent = nullptr
    );
    ~UserDashboard() override;

private slots:
    // Handles Logout button click signal.
    // Logs user out.
    void onLogoutButtonClicked();

    // Handles file upload.
    // Opens file dialog, then upload settings window.
    void onUploadButtonClicked();

    // Handles failure.
    // Shows message box and logs user out.
    void onFailure(const std::string& message);

    // Handles server response.
    void onResponse(
        const API::RequestResult &result,
        const std::string &success_msg
    );

    // Downloads selected file
    void onFileDownload(const FileData &fileData);

    // Deletes selected file
    void onFileDelete(size_t fileID);

signals:
    void failure(const std::string& message);

    void response(const API::RequestResult &result, const std::string &success_msg);

private:
    // Logs user out
    void logout();

    // Background task for refreshing a token
    void tokenRefreshTask();

    // Overrides base window close event
    void closeEvent(QCloseEvent *event) override;

    // Retrieves files related to current user from remote storage
    void tryRetrieveFiles();

    // Sets active flag, with mutex
    void setActive(bool value);

    // Returns active flag, with mutex
    bool getActive();

    Ui::UserDashboard *ui;

    // Token pair
    API::TokenPair tokenPair;

    // A flag indicating whether the user is active or not.
    // Used in multiple threads.
    bool active;

    // Thread for token refresh background job
    std::thread tokenRefreshThread;

    // Mutex for accessing active flag (even though it is atomic)
    // Avoids "late wakeups"
    std::mutex activeMutex;
    
    // Condition variable for token refresh loop
    std::condition_variable tokenRefreshCV;

    // Hols all uploaded files widgets
    std::vector<UploadedFilePanel*> uploadedFilePanels;

};

#endif //USER_DASHBOARD_H
