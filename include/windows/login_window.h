#pragma once

#include <QMainWindow>
#include <QMovie>

#include "api/request_result.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow final : public QMainWindow {
Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow() override;

private slots:
    void onLoginButtonClicked();

    void onRegisterButtonClicked();

private:
    void onLoginSuccessfull(const API::RequestResult& response);

    void resetLoginButton();

    std::unique_ptr<Ui::LoginWindow> ui;

    QMovie *loadingAnimation;

};
