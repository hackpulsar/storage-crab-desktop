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
namespace Ui { class MainWindow; }
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

    Ui::MainWindow *ui;
    QWidget *centralWidget;
    QVBoxLayout *layout;

    // Title
    QLabel *titleLabel;
    QLabel *subtitleLabel;

    // Login block
    QLabel *errorLabel;
    QLineEdit *emailLineEdit;
    QLineEdit *passwordLineEdit;

    // Loading animation (GIF) for a login button
    QMovie *loadingAnimation;
    QPushButton *loginButton;

    QLabel *registrationLink;

signals:
    void loginResponseReceived(const std::string& response);

    void loginError(const std::string& title, const std::string& message);

};


#endif //MAIN_WINDOW_H
