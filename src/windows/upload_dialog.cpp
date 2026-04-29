#include "windows/upload_dialog.h"
#include "ui_upload_dialog.h"

#include <QFileDialog>
#include <QMovie>
#include <QMessageBox>
#include <QFileInfo>

#include "api/api_dispatcher.hpp"
#include "windows/user_dashboard.h"
#include "cryptography/file_crypto.h"
#include "watch_future.hpp"

UploadDialog::UploadDialog(std::string filePath, QWidget *parent)
    : QDialog(parent), ui(new Ui::UploadDialog), filePath(std::move(filePath))
{
    ui->setupUi(this);

    ui->filePathLineEdit->setText(QString::fromStdString(this->filePath));

    loadingAnimation = new QMovie(this);
    loadingAnimation->setFileName(QString(ASSETS_PATH) + "/loading.gif");

    connect(
       ui->algorithmComboBox, &QComboBox::currentTextChanged,
       this, &UploadDialog::switchEncryptionAlgorithm
    );

    connect(
       ui->aesTypeComboBox, &QComboBox::currentTextChanged,
       this, &UploadDialog::switchAESType
    );

    connect(
        ui->uploadButton, &QPushButton::clicked,
        this, &UploadDialog::onUploadButtonClicked
    );
    connect(
        ui->cancelButton, &QPushButton::clicked,
        this, &UploadDialog::reject
    );

    connect(
        ui->chooseFilePathButton, &QPushButton::clicked,
        this, &UploadDialog::onChooseFilePathButtonClicked
    );
    connect(
        ui->chooseKeyPathButton, &QPushButton::clicked,
        this, &UploadDialog::onChooseKeyPathButtonClicked
    );
}

UploadDialog::~UploadDialog() {
    delete ui;
}

void UploadDialog::onChooseFilePathButtonClicked() {
    // Reading file path
    const std::string newFilePath = QFileDialog::getOpenFileName(
        this, "Select file to upload", QDir::homePath()
    ).toStdString();

    // Setting the new path if not empty
    if (filePath != "") {
        ui->filePathLineEdit->setText(QString::fromStdString(newFilePath));
        this->filePath = newFilePath;
    }
}

void UploadDialog::onChooseKeyPathButtonClicked() {
    auto dialog = QFileDialog(this);
    dialog.setWindowTitle("Select a file to save the key");
    dialog.setDirectory(QDir::homePath());
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("Crab Key (*.crbkey);;All Files (*)"));
    dialog.setDefaultSuffix("crbkey");

    if (dialog.exec())
    {
        // Reading file path
        const std::string newKeyPath = dialog.selectedFiles().first().toStdString();

        // Setting the new path if not empty
        if (newKeyPath != "") {
            ui->keyPathLineEdit->setText(QString::fromStdString(newKeyPath));
            this->keyPath = newKeyPath;
        }
    }
}

void UploadDialog::switchEncryptionAlgorithm(const QString &newAlgorithm) {
    const auto algorithmType = Cryptography::algorithmTypeFromString(newAlgorithm.toStdString());
    this->encryptionAlgorithm = algorithmType;

    // Disable/enable RSA key size UI elements
    if (this->encryptionAlgorithm == Cryptography::AlgorithmType::AES) {
        ui->keySizeLabel->setEnabled(false);
        ui->keySizeComboBox->setEnabled(false);
    } else {
        ui->keySizeLabel->setEnabled(true);
        ui->keySizeComboBox->setEnabled(true);
    }
}

void UploadDialog::switchAESType(const QString &newAESType) {
    const auto aesType = Cryptography::AESTypeFromString(newAESType.toStdString());
    this->AESType = aesType;
}

void UploadDialog::onUploadButtonClicked() {
    if (ui->filePathLineEdit->text().isEmpty() || ui->keyPathLineEdit->text().isEmpty()) {
        QMessageBox::critical(this, "Error", "Please fill all the fields");
        return;
    }

    ui->uploadButton->setText("");
    connect(
        loadingAnimation, &QMovie::frameChanged,
        ui->uploadButton, [this] {
        ui->uploadButton->setIcon(loadingAnimation->currentPixmap());
    });
    loadingAnimation->start();

    using namespace Cryptography;

    FileCrypto::EncryptOptions opts {
        .filePath           = this->filePath,
        .keyPath            = this->keyPath,
        .aesType            = this->AESType,
        .algorithm          = this->encryptionAlgorithm,
        .rsaKeySize         = std::atoi(ui->keySizeComboBox->currentText().toStdString().c_str()),
        .encryptFileName    = ui->encryptNameCheckBox->isChecked()
    };

    watchFuture(
        this, dispatch([opts] { return FileCrypto::encryptFile(opts); }),
        [this](const FileCrypto::Result& result) {
            watchFuture(
                this, ApiDispatcher::instance().uploadFile(result.fileName, result.path),
                [this](const API::RequestResult&) { 
                    QMessageBox::information(this, "Success", "File has been uploaded successfully!"); 
                    emit accepted();
                },
                [this](const API::RequestResult& response) {
                    this->resetUploadButton();
                    QMessageBox::critical(this, "Error", QString::fromStdString(response.extractErrorDetails()));
                }
            );
        },
        [this](const FileCrypto::Result&) { QMessageBox::critical(this, "Error", "Something went wrong during file encryption."); }
    );
}

void UploadDialog::resetUploadButton() {
    loadingAnimation->stop();
    ui->uploadButton->setText("Encrypt && Upload");
    ui->uploadButton->setIcon(QIcon());
}
