#include "login_window.h"

#include "ui_login_window.h"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <nlohmann/json.hpp>

#include <QMessageBox>
#include <QThread>

#include "api.h"
#include "user_dashboard.h"
#include "utils/styles_loader.hpp"

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("🦀 Login");

    // Creating central widget
    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    // Title
    titleLabel = new QLabel(centralWidget);
    titleLabel->setText("Storage Crab");
    titleLabel->setStyleSheet("font-size: 56pt");
    titleLabel->setAlignment(Qt::AlignCenter);

    // Subtitle
    subtitleLabel = new QLabel(centralWidget);
    subtitleLabel->setText("🦀");
    subtitleLabel->setStyleSheet("font-size: 80pt");
    subtitleLabel->setAlignment(Qt::AlignCenter);

    // Error label
    errorLabel = new QLabel(centralWidget);
    errorLabel->setStyleSheet("font-size: 14pt; color: red;");
    errorLabel->setAlignment(Qt::AlignCenter);

    // Login text field
    emailLineEdit = new QLineEdit(centralWidget);
    emailLineEdit->setMinimumWidth(200);
    emailLineEdit->setPlaceholderText("Email");
    emailLineEdit->setAlignment(Qt::AlignCenter);
    emailLineEdit->setStyleSheet("font-size: 16pt;");

    // Password text field
    passwordLineEdit = new QLineEdit(centralWidget);
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    passwordLineEdit->setMinimumWidth(200);
    passwordLineEdit->setPlaceholderText("Password");
    passwordLineEdit->setAlignment(Qt::AlignCenter);
    passwordLineEdit->setStyleSheet("font-size: 16pt;");

    // Login button
    loadingAnimation = new QMovie(this);
    loadingAnimation->setFileName("../assets/loading.gif");
    loginButton = new QPushButton(centralWidget);
    loginButton->setText("Login");
    loginButton->setIconSize(QSize(30, 30));
    loginButton->setStyleSheet(Utils::StylesLoader::loadStyleFromFile("basic_button.css"));

    // Register link
    // TODO: redirect to registration form
    registrationLink = new QLabel(centralWidget);
    registrationLink->setText("Dont have an account yet?");
    registrationLink->setTextFormat(Qt::RichText);
    registrationLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    registrationLink->setOpenExternalLinks(true);
    registrationLink->setAlignment(Qt::AlignCenter);

    layout = new QVBoxLayout(centralWidget);

    // Adding all the widgets to the layout
    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addStretch();
    layout->addWidget(errorLabel, 0, Qt::AlignHCenter);
    layout->addWidget(emailLineEdit, 0, Qt::AlignHCenter);
    layout->addWidget(passwordLineEdit, 0, Qt::AlignHCenter);
    layout->addStretch();
    layout->addWidget(loginButton, 0, Qt::AlignHCenter);
    layout->addStretch();
    layout->addWidget(registrationLink);

    // Connecting login button release signal to a custom login handler
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginButtonClicked);

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
    delete centralWidget; // Also deletes all children
}

void LoginWindow::onLoginButtonClicked() {
    // Validate input
    if (emailLineEdit->text().isEmpty() || passwordLineEdit->text().isEmpty()) {
        errorLabel->setText("Please fill the fields below");
        return;
    }

    // Removing the text from login button and putting a loading animation
    loginButton->setText("");
    connect(loadingAnimation, &QMovie::frameChanged, loginButton, [this] {
        loginButton->setIcon(loadingAnimation->currentPixmap());
    });
    loadingAnimation->start();

    QThread* httpRequestThread = QThread::create([this] {
        try {
            // Form a token obtain request
            cURLpp::Easy login_request;
            login_request.setOpt(cURLpp::options::Url(API::TOKEN_OBTAIN_URL));

            // Token obtain POST request body
            const nlohmann::json body = {
                {"email", emailLineEdit->text().toStdString()},
                {"password_hash", passwordLineEdit->text().toStdString()},
            };

            std::ostringstream responseStream;

            // Informing that we are using JSON
            login_request.setOpt(cURLpp::options::HttpHeader({"Content-Type: application/json"}));
            // Adding the body and its size to request
            login_request.setOpt(curlpp::options::PostFields(body.dump()));
            login_request.setOpt(curlpp::options::PostFieldSize(static_cast<long>(body.dump().length())));
            login_request.setOpt(cURLpp::options::WriteStream(&responseStream));

            // Add timeout
            login_request.setOpt(cURLpp::options::Timeout(10));
            login_request.setOpt(cURLpp::options::ConnectTimeout(5));

            // Performing the request
            login_request.perform();

            // Emitting a signal indicating that the response has been received
            emit loginResponseReceived(responseStream.str());
        } catch (cURLpp::RuntimeError &e) {
            emit loginError("Runtime error", e.what());
        } catch (cURLpp::LogicError &e) {
            emit loginError("Logic error", e.what());
        }
    });

    // When the thread is finished, it is going to get deleted safely
    connect(httpRequestThread, &QThread::finished, httpRequestThread, &QThread::deleteLater);
    httpRequestThread->start();
}

void LoginWindow::handleLoginResponse(const std::string &response) {
    // Parse response string
    const auto response_json = nlohmann::json::parse(response);

    // Validate the response
    if (response_json.contains("details")) {
        // Fail
        errorLabel->setText(QString::fromStdString(response_json.at("details").get<std::string>()));
        this->resetLoginButton();

    } else {
        // Login successful

        cURLpp::terminate(); // Cleanup cURLpp
        this->close(); // Close current window

        // TODO: retrieve username

        // Proceed to player's personal shelter
        auto *shelter = new UserDashboard(
            API::TokenPair(response_json.at("access_token"), response_json.at("refresh_token")),
            "unknown"
        );
        shelter->setAttribute(Qt::WA_DeleteOnClose); // Automatically frees memory allocated for this window
        shelter->show();
    }
}

void LoginWindow::resetLoginButton() {
    loadingAnimation->stop();
    loginButton->setText("Login");
    loginButton->setIcon(QIcon());
}
