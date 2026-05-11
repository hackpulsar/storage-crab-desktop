#pragma once

#include <QMainWindow>
#include <QMovie>

#include "api/request_result.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class RegisterWindow; }
QT_END_NAMESPACE

class RegisterWindow final : public QMainWindow {
Q_OBJECT

public:
    explicit RegisterWindow(QWidget *parent = nullptr);
    ~RegisterWindow() override;

private slots:
    void onRegisterButtonClicked();

private:
    void onRegisterSuccessfull(const API::RequestResult& response);

    void resetRegisterButton();

    std::unique_ptr<Ui::RegisterWindow> ui;

    QMovie *loadingAnimation;

};
