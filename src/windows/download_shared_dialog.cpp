#include "windows/download_shared_dialog.h"
#include "ui_download_shared_dialog.h"

#include <memory>

#include <QFileDialog>
#include <QMessageBox>

#include "api.h"
#include "requests.hpp"

DownloadSharedDialog::DownloadSharedDialog(const API::TokenPair& tokenPair, QWidget* parent) 
    : QDialog(parent), ui(std::make_unique<Ui::DownloadSharedDialog>()), tokenPair(tokenPair)
{
    ui->setupUi(this);

    connect(
        ui->downloadButton, &QPushButton::clicked,
        this, &DownloadSharedDialog::onDownloadButtonClicked
    );
}

DownloadSharedDialog::~DownloadSharedDialog() = default;

void DownloadSharedDialog::onDownloadButtonClicked() {
    ui->downloadButton->setEnabled(false);

    const std::string destinationDir = QFileDialog::getExistingDirectory(
        this,
        "Select destination folder",
        QDir::homePath()
    ).toStdString();

    if (destinationDir.empty()) return;

    const API::RequestResult result = API::Requests::GET_DOWNLOAD(
        API::DOWNLOAD_SHARED_URL_FOR(ui->codeLineEdit->text().toStdString()),
        destinationDir,
        this->tokenPair.getAccess()
    );

    if (result.ok) {
        QMessageBox::information(this, "Sucess!", "File has been downloaded successfully!");
        this->close();
    } else {
        QMessageBox::critical(this, "Error", QString::fromStdString(result.extractErrorDetails()));
        ui->downloadButton->setEnabled(true);
    }
}
