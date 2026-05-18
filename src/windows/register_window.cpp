#include "windows/register_window.h"
#include "ui_register_window.h"

#include <QMessageBox>

#include "api/api_dispatcher.hpp"
#include "windows/login_window.h"
#include "utils/watch_future.hpp"

RegisterWindow::RegisterWindow(QWidget* parent)
    : QMainWindow(parent), ui(std::make_unique<Ui::RegisterWindow>())
{
    ui->setupUi(this);

    loadingAnimation = new QMovie(this);
    loadingAnimation->setFileName(QString(ASSETS_PATH) + "/loading.gif");

    connect(ui->registerButton, &QPushButton::clicked, this, &RegisterWindow::onRegisterButtonClicked);
}

RegisterWindow::~RegisterWindow() = default;

void RegisterWindow::onRegisterButtonClicked() {
    if (ui->emailLineEdit->text().isEmpty() || ui->passwordLineEdit->text().isEmpty() 
        || ui->passwordRepeatLineEdit->text().isEmpty() || ui->usernameLineEdit->text().isEmpty()) {
        ui->errorLabel->setText("Please fill the fields below");
        return;
    }

    if (ui->passwordLineEdit->text() != ui->passwordRepeatLineEdit->text()) {
        ui->errorLabel->setText("Passwords don't match");
        return;
    }

    ui->registerButton->setText("");
    connect(loadingAnimation, &QMovie::frameChanged, ui->registerButton, [this] {
        ui->registerButton->setIcon(loadingAnimation->currentPixmap());
    });
    loadingAnimation->start();

    const std::string email = ui->emailLineEdit->text().toStdString();
    const std::string username = ui->usernameLineEdit->text().toStdString();
    const std::string pass = ui->passwordLineEdit->text().toStdString();
    watchFuture(
        this, ApiDispatcher::instance().register_user(email, username, pass),
        [this](const API::RequestResult& response) { this->onRegisterSuccessfull(response); },
        [this](const API::RequestResult& response) {
            ui->errorLabel->setText(QString::fromStdString(response.extractErrorDetails()));
            this->resetRegisterButton();
        }
    );
}

void RegisterWindow::onRegisterSuccessfull(const API::RequestResult& response) {
    std::string welcome_msg = "Welcome aboard, " + response.body.at("username").get<std::string>();
    QMessageBox::information(this, "Success", welcome_msg.c_str());
    
    // Return to login page
    this->close();
    auto *login_window = new LoginWindow;
    login_window->setAttribute(Qt::WA_DeleteOnClose);
    login_window->show();
}

void RegisterWindow::resetRegisterButton() {
    loadingAnimation->stop();
    ui->registerButton->setText("Register");
    ui->registerButton->setIcon(QIcon());
}
