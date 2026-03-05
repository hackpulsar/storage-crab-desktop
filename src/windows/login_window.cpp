#include "windows/login_window.h"
#include "ui_login_window.h"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <nlohmann/json.hpp>

#include <QMessageBox>
#include <QThread>

#include "api.h"
#include "requests.hpp"
#include "windows/user_dashboard.h"
#include "utils/styles_loader.hpp"

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::LoginWindow)
{
    ui->setupUi(this);

    dotenv::init();

    // DEBUG
    ui->emailLineEdit->setText("admin@admin.com");
    ui->passwordLineEdit->setText("admin");

    loadingAnimation = new QMovie(this);
    loadingAnimation->setFileName("../assets/loading.gif");

    // Connecting login button release signal to a custom login handler
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginButtonClicked);

    // Connecting login response receive signal to a handler
    connect(
        this, &LoginWindow::loginResponseReceived,
        this, &LoginWindow::handleLoginResponse
    );

    // Connect login error to handler
    connect(
        this, &LoginWindow::loginError,
        this, [this](const std::string& title, const std::string& message) {
            qDebug() << "here";
            this->resetLoginButton();
            QMessageBox::critical(this, QString::fromStdString(title), QString::fromStdString(message));
    });

    // Initialise cURL
    cURLpp::initialize();
}

LoginWindow::~LoginWindow() {
    delete ui;
}

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

    QThread* loginRequestThread = QThread::create([this] {
        API::RequestResult result = API::Requests::POST(
            std::getenv("TOKEN_OBTAIN_URL"),
            {
                {"email", ui->emailLineEdit->text().toStdString()},
                {"password_hash", ui->passwordLineEdit->text().toStdString()},
            }
        );

        // Emitting a signal indicating that the response has been received
        emit loginResponseReceived(result.response.dump());
    });

    // When the thread is finished, it is going to get deleted safely
    connect(loginRequestThread, &QThread::finished, loginRequestThread, &QThread::deleteLater);
    loginRequestThread->start();
}

void LoginWindow::handleLoginResponse(const std::string &response) {
    // Parse response string
    const auto response_json = nlohmann::json::parse(response);

    // Validate the response
    if (response_json.contains("details")) {
        // Fail
        ui->errorLabel->setText(QString::fromStdString(response_json.at("details").get<std::string>()));
        this->resetLoginButton();

    } else {
        // Login successful

        // Try etrieve username
        API::RequestResult result = API::Requests::GET(
            std::getenv("ME_URL"),
            response_json.at("access_token")
        );

        if (!result.ok) {
            QMessageBox::critical(this, "Error", QString::fromStdString(result.extractErrorDetails()));
            return;
        }

        cURLpp::terminate(); // Cleanup cURLpp
        this->close(); // Close current window

        // Proceed to player's personal shelter
        auto *shelter = new UserDashboard(
            API::TokenPair(response_json.at("access_token"), response_json.at("refresh_token")),
            result.response.at("username").get<std::string>()
        );
        shelter->setAttribute(Qt::WA_DeleteOnClose); // Automatically frees memory allocated for this window
        shelter->show();
    }
}

void LoginWindow::resetLoginButton() {
    loadingAnimation->stop();
    ui->loginButton->setText("Login");
    ui->loginButton->setIcon(QIcon());
}
