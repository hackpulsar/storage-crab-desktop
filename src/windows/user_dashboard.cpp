#include "windows/user_dashboard.h"
#include "ui_user_dashboard.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QThread>

#include "windows/upload_dialog.h"
#include "windows/login_window.h"
#include "windows/share_code_dialog.h"
#include "windows/download_shared_dialog.h"
#include "widgets/uploaded_file_panel.h"

#include "api/api_dispatcher.hpp"
#include "watch_future.hpp"

UserDashboard::UserDashboard(const std::string& username, QWidget* parent)
    : QMainWindow(parent), ui(new Ui::UserDashboard)
{
    ui->setupUi(this);

    // Update username labels
    this->setWindowTitle(QString::fromStdString(username + "'s dashboard"));
    ui->usernameLabel->setText(username.c_str());

    this->tryRetrieveFiles();

    connect(
        ui->logoutButton, &QPushButton::clicked,
        this, &UserDashboard::onLogoutButtonClicked
    );
    connect(
        ui->uploadButton, &QPushButton::clicked,
        this, &UserDashboard::onUploadButtonClicked
    );
    connect(
        ui->downloadSharedButton, &QPushButton::clicked,
        this, &UserDashboard::onDownloadSharedButtonClicked
    );

    connect(
        &ApiDispatcher::instance(), &ApiDispatcher::sessionExpired,
        this, [this] {
            QMessageBox::critical(this, "Error", "Your session has expired");
            this->close();
        }
    );
}

UserDashboard::~UserDashboard() {
    delete ui;
}

void UserDashboard::onLogoutButtonClicked() {
    // Ask user if sure
    const auto reply = QMessageBox::question(
        this, "Logout",
        "Are you sure you want to logout?",
        QMessageBox::Yes | QMessageBox::No
    );

    switch (reply) {
        case QMessageBox::Yes: this->close();
        default: break;
    }
}

void UserDashboard::onUploadButtonClicked() {
    // Reading file path
    const std::string filePath = QFileDialog::getOpenFileName(
        this,
        "Select file to upload",
        QDir::homePath()
    ).toStdString();

    if (filePath.empty()) return;

    auto *dialog = new UploadDialog(filePath, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowModality(Qt::WindowModal);
    dialog->show();

    // Reloading the files on dialog close
    connect(
        dialog, &UploadDialog::rejected,
        this, [dialog] { dialog->close(); }
    );

    connect(
        dialog, &UploadDialog::accepted,
        this, [this, dialog] {
            dialog->close();
            this->tryRetrieveFiles();
        }
    );
}

void UserDashboard::onDownloadSharedButtonClicked() {
    auto *dialog = new DownloadSharedDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowModality(Qt::WindowModal);
    dialog->show();
}

void UserDashboard::onShareFile(const size_t fileID) {
    watchFuture(
        this, ApiDispatcher::instance().shareFile(fileID),
        [this](const API::RequestResult& response) {
            auto *dialog = new ShareCodeDialog(response.body.at("code").get<std::string>(), this);
            dialog->setAttribute(Qt::WA_DeleteOnClose);
            dialog->setWindowModality(Qt::WindowModal);
            dialog->show();
        },
        [this](const API::RequestResult& response) {
            QMessageBox::critical(this, "Error", QString::fromStdString(response.extractErrorDetails()));
        }
    );
}

void UserDashboard::onFileDownload(const FileData &fileData) {
    const std::string destinationDir = QFileDialog::getExistingDirectory(
        this,
        "Select destination folder",
        QDir::homePath()
    ).toStdString();

    if (destinationDir.empty()) return;

    watchFuture(
        this, ApiDispatcher::instance().downloadFile(fileData.id, destinationDir),
        [this](const API::RequestResult& response) {
            QMessageBox::information(this, "Success", QString::fromStdString("File downloaded sucessfully!"));
            this->tryRetrieveFiles();
        },
        [this](const API::RequestResult& response) { 
            QMessageBox::critical(this, "Error", QString::fromStdString(response.extractErrorDetails()));
        }
    );
}

void UserDashboard::onFileDelete(const size_t fileID) {
    watchFuture(
        this, ApiDispatcher::instance().deleteFile(fileID),
        [this](const API::RequestResult& response) {
            QMessageBox::information(this, "Success", QString::fromStdString("File deleted sucessfully!"));
            this->tryRetrieveFiles();
        },
        [this](const API::RequestResult& response) { 
            QMessageBox::critical(this, "Error", QString::fromStdString(response.extractErrorDetails()));
        }
    );
}

void UserDashboard::closeEvent(QCloseEvent *event) {
    // Back to main window
    auto *loginWindow = new LoginWindow;
    loginWindow->setAttribute(Qt::WA_DeleteOnClose);
    loginWindow->show();

    QWidget::closeEvent(event);
}

void UserDashboard::tryRetrieveFiles() {
    watchFuture(
        this, ApiDispatcher::instance().getFiles(),
        [this](const API::RequestResult& response) {
            // Removing all the previously loaded files
            for (auto* panel : uploadedFilePanels) {
                ui->middlePanelLayout->removeWidget(panel);
                delete panel;
            }
            uploadedFilePanels.clear();

            for (const auto& file_data : response.body) {
                auto panel = new UploadedFilePanel(
                    FileData {
                        file_data.at("filename"),
                        file_data.at("path"),
                        file_data.at("size").get<size_t>(),
                        file_data.at("id").get<size_t>(),
                    },
                    ui->scrollArea
                );

                connect(
                    panel, &UploadedFilePanel::shareButtonPressed,
                    this, &UserDashboard::onShareFile
                );
                connect(
                    panel, &UploadedFilePanel::downloadButtonPressed,
                    this, &UserDashboard::onFileDownload
                );
                connect(
                    panel, &UploadedFilePanel::deleteButtonPressed,
                    this, &UserDashboard::onFileDelete
                );

                uploadedFilePanels.push_back(panel);
                ui->middlePanelLayout->insertWidget(ui->middlePanelLayout->count() - 1, uploadedFilePanels.back());
            }
        },
        [this](const API::RequestResult& response) {
            QMessageBox::critical(this, "Error", "Couldn't retrieve files.", QMessageBox::Ok);
        }
    );
}
