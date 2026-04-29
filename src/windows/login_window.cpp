#include "windows/login_window.h"
#include "ui_login_window.h"

#include <QMessageBox>

#include <memory>

#include "api/api.h"
#include "api/api_dispatcher.hpp"
#include "utils/watch_future.hpp"
#include "windows/user_dashboard.h"

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent), ui(std::make_unique<Ui::LoginWindow>())
{
    ui->setupUi(this);

    dotenv::init();

    // DEBUG
    ui->emailLineEdit->setText("admin@admin.com");
    ui->passwordLineEdit->setText("admin");

    loadingAnimation = new QMovie(this);
    loadingAnimation->setFileName(QString(ASSETS_PATH) + "/loading.gif");

    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginButtonClicked);
}

LoginWindow::~LoginWindow() = default;

void LoginWindow::onLoginButtonClicked() {
    // Validate input
    if (ui->emailLineEdit->text().isEmpty() || ui->passwordLineEdit->text().isEmpty()) {
        ui->errorLabel->setText("Please fill the fields below");
        return;
    }

    // Removing the text from login button and putting a loading animation
    ui->loginButton->setText("");
    connect(loadingAnimation, &QMovie::frameChanged, ui->loginButton, [this] {
        ui->loginButton->setIcon(loadingAnimation->currentPixmap());
    });
    loadingAnimation->start();

    const std::string email = ui->emailLineEdit->text().toStdString();
    const std::string pass_hash = ui->passwordLineEdit->text().toStdString();
    watchFuture(
        this, ApiDispatcher::instance().login(email, pass_hash),
        [this](const API::RequestResult& response) { this->onLoginSuccessfull(response); },
        [this](const API::RequestResult& response) {
            ui->errorLabel->setText(QString::fromStdString(response.extractErrorDetails()));
            this->resetLoginButton();
        }
    );
}

void LoginWindow::onLoginSuccessfull(const API::RequestResult& response) {
    ApiDispatcher::instance().storeTokens(response.body.at("access_token"), response.body.at("refresh_token"));

    // Try retrieve username
    watchFuture(
        this, ApiDispatcher::instance().me(),
        [this](const API::RequestResult& response) {
            this->close(); // Close current window

            // Proceed to player's personal shelter
            auto *shelter = new UserDashboard(response.body.at("username").get<std::string>());
            shelter->setAttribute(Qt::WA_DeleteOnClose); // Automatically frees memory allocated for this window
            shelter->show();
        },
        [this](const API::RequestResult& response) {
            QMessageBox::critical(this, "Error", QString::fromStdString(response.extractErrorDetails()));
        }
    );
}

void LoginWindow::resetLoginButton() {
    loadingAnimation->stop();
    ui->loginButton->setText("Login");
    ui->loginButton->setIcon(QIcon());
}
