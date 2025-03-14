#include "user_dashboard.h"
#include "ui_user_dashboard.h"

#include <QMessageBox>

#include "utils/styles_loader.hpp"
#include "login_window.h"

UserDashboard::UserDashboard(
    const API::TokenPair& tokenPair,
    const std::string& username,
    QWidget *parent
)
    : QMainWindow(parent)
    , ui(new Ui::UserDashboard)
    , tokenPair(tokenPair.getAccess(), tokenPair.getRefresh())
    , active(true)
    , tokenRefreshThread(&UserDashboard::tokenRefreshTask, this)
{
    ui->setupUi(this);
    this->setWindowTitle(QString::fromStdString(username + "'s dashboard"));

    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    using namespace Utils;

    // Top panel
    topPanelLayout = new QHBoxLayout;

    logoutButton = new QPushButton(centralWidget);
    logoutButton->setText("Log me out");
    logoutButton->setStyleSheet(StylesLoader::loadStyleFromFile("basic_button.css"));

    titleLabel = new QLabel(centralWidget);
    titleLabel->setText("Dashboard");
    titleLabel->setStyleSheet("font-size: 36pt");
    titleLabel->setAlignment(Qt::AlignCenter);

    usernameLabel = new QLabel(centralWidget);
    usernameLabel->setText(QString::fromStdString(username));
    usernameLabel->setStyleSheet("font-size: 24pt");
    usernameLabel->setAlignment(Qt::AlignCenter);

    topPanelLayout->addWidget(logoutButton);
    topPanelLayout->addStretch();
    topPanelLayout->addWidget(titleLabel);
    topPanelLayout->addStretch();
    topPanelLayout->addWidget(usernameLabel);

    // Bottom panel
    bottomPanelLayout = new QHBoxLayout;

    uploadButton = new QPushButton(centralWidget);
    uploadButton->setText("Upload");
    uploadButton->setStyleSheet("font-size: 24pt");
    uploadButton->setStyleSheet(StylesLoader::loadStyleFromFile("upload_button.css"));

    bottomPanelLayout->addWidget(uploadButton);

    layout = new QVBoxLayout(centralWidget);
    layout->addLayout(topPanelLayout);
    layout->addStretch();
    layout->addLayout(bottomPanelLayout);

    connect(logoutButton, &QPushButton::clicked, this, &UserDashboard::onLogoutButtonClicked);
    connect(this, &UserDashboard::failure, this, &UserDashboard::onFailure);
}

UserDashboard::~UserDashboard() {
    delete ui;
}

void UserDashboard::onLogoutButtonClicked() {
    // Ask user if sure
    const auto reply = QMessageBox::question(
        this,
        "Logout",
        "Are you sure you want to logout?",
        QMessageBox::Yes | QMessageBox::No
    );

    switch (reply) {
        case QMessageBox::Yes: {
            // Logout and close current window
            this->close();
            break;
        }
        default: break;
    }
}

void UserDashboard::onFailure(const std::string& message) {
    QMessageBox::critical(
        this,
        "Error",
        (std::string("Something went wrong.\nDetails: ") + std::string(message)).c_str()
    );
    this->close();
}

void UserDashboard::logout() {
    this->active = false;
    // Notifying all the waiting threads
    this->tokenRefreshCV.notify_all();
    // Joining token refresh task thread
    this->tokenRefreshThread.join();
}

void UserDashboard::tokenRefreshTask() {
    std::mutex tokenRefreshCVMutex;

    while (this->active) {
        // Wait with a condition variable
        std::unique_lock lock(tokenRefreshCVMutex);
        if (tokenRefreshCV.wait_for(lock, std::chrono::minutes(10), [this] {
            return !this->active;
        })) {
            break; // End the task. User is no longer active
        }

        // Handle failure
        auto result = this->tokenPair.refresh();
        if (!result.ok) {
            emit failure("Your session has expired, logging out.");
            break;
        }

    }
}

void UserDashboard::closeEvent(QCloseEvent *event) {
    // Logging the user out
    this->logout();

    // Back to main window
    auto *loginWindow = new LoginWindow;
    loginWindow->setAttribute(Qt::WA_DeleteOnClose);
    loginWindow->show();

    QWidget::closeEvent(event);
}
