#ifndef USER_DASHBOARD_H
#define USER_DASHBOARD_H

#include <QMainWindow>
#include <ui_user_dashboard.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <thread>

#include "token_pair.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class UserDashboard final : public QMainWindow {
Q_OBJECT

public:
    explicit UserDashboard(
        const API::TokenPair& tokenPair,
        const std::string& username,
        QWidget *parent = nullptr
    );
    ~UserDashboard() override;

private slots:
    // Handles Logout button click signal.
    // Logs user out.
    void onLogoutButtonClicked();

    // Handles failure.
    // Shows message box and logs user out.
    void onFailure(const std::string& message);

signals:
    void failure(const std::string& message);

private:
    // Logs user out
    void logout();

    // Background task for refreshing a token
    void tokenRefreshTask();

    // Overrides base window close event
    void closeEvent(QCloseEvent *event) override;

    Ui::UserDashboard *ui;
    QWidget *centralWidget;

    // Main window layout
    QVBoxLayout *layout;

    // Top panel
    QHBoxLayout *topPanelLayout;
    QPushButton *logoutButton;
    QLabel *titleLabel;
    QLabel *usernameLabel;

    // Bottom panel
    QHBoxLayout *bottomPanelLayout;
    QPushButton *uploadButton;

    // Token pair
    API::TokenPair tokenPair;

    // A flag indicating whether the user is active or not.
    // Used in multiple threads.
    std::atomic<bool> active;

    // Thread for token refresh background job
    std::thread tokenRefreshThread;

    // Condition variable for token refresh loop
    std::condition_variable tokenRefreshCV;

};

#endif //USER_DASHBOARD_H
