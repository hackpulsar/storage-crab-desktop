#include "windows/download_shared_dialog.h"
#include "ui_download_shared_dialog.h"

#include <memory>

#include <QFileDialog>
#include <QMessageBox>
#include <QThreadPool>

#include "api/api_dispatcher.hpp"
#include "utils/watch_future.hpp"

DownloadSharedDialog::DownloadSharedDialog(QWidget* parent) 
    : QDialog(parent), ui(std::make_unique<Ui::DownloadSharedDialog>())
{
    ui->setupUi(this);

    connect(
        ui->downloadButton, &QPushButton::clicked,
        this, &DownloadSharedDialog::onDownloadButtonClicked
    );
}

DownloadSharedDialog::~DownloadSharedDialog() = default;

void DownloadSharedDialog::onDownloadButtonClicked() {
    if (ui->codeLineEdit->text().trimmed().isEmpty()) {
        QMessageBox::critical(this, "Error", "Please fill all the fields");
        return;
    }

    ui->downloadButton->setEnabled(false);

    const std::string destinationDir = QFileDialog::getExistingDirectory(
        this,
        "Select destination folder",
        QDir::homePath()
    ).toStdString();

    if (destinationDir.empty()) return;

    watchFuture(
        this, ApiDispatcher::instance().downloadSharedFile(ui->codeLineEdit->text().toStdString(), destinationDir),
        [this](const API::RequestResult&) {
            QMessageBox::information(this, "Sucess!", "File has been downloaded successfully!");
            this->close();
        },
        [this](const API::RequestResult& response) {
            QMessageBox::critical(this, "Error", QString::fromStdString(response.extractErrorDetails()));
            ui->downloadButton->setEnabled(true);
        }
    );
}
