#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QMovie>


QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow final : public QMainWindow {
Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow() override;

private slots:
    // Handles user login.
    // Closes login window and proceeds to player's shelter on success.
    void onLoginButtonClicked();

private:
    // Login response handler
    void handleLoginResponse(const std::string& response);

    // Resets login button
    void resetLoginButton();

    Ui::LoginWindow *ui;

    // Loading animation (GIF) for a login button
    QMovie *loadingAnimation;

signals:
    void loginResponseReceived(const std::string& response);

    void loginError(const std::string& title, const std::string& message);

};


#endif //MAIN_WINDOW_H
