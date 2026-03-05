#include "windows/upload_dialog.h"

#include <QMessageBox>

#include "ui_upload_dialog.h"

#include <QtWidgets/QFileDialog>
#include <QFormLayout>
#include <QCheckBox>
#include <QThread>
#include <QMovie>
#include <utils/dark_mode.hpp>

#include "api.h"
#include "requests.hpp"
#include "utils/styles_loader.hpp"
#include "windows/user_dashboard.h"
#include "cryptography/algorithm_types.h"
#include "cryptography/aes.h"
#include "cryptography/rsa.h"

UploadDialog::UploadDialog(
    std::string filePath,
    std::string accessToken,
    QWidget *parent
)
    : QDialog(parent)
    , ui(new Ui::UploadDialog)
    , filePath(std::move(filePath))
    , accessToken(std::move(accessToken))
{
    ui->setupUi(this);

    // Setting the file path
    ui->filePathLineEdit->setText(QString::fromStdString(this->filePath));

    // Loading animation
    loadingAnimation = new QMovie(this);
    loadingAnimation->setFileName("../assets/loading.gif");

    connect(
       ui->algorithmComboBox, &QComboBox::currentTextChanged,
       this, &UploadDialog::switchEncryptionAlgorithm
    );

    connect(
       ui->aesTypeComboBox, &QComboBox::currentTextChanged,
       this, &UploadDialog::switchAESType
    );

    connect(
        this, &UploadDialog::error,
        this, &UploadDialog::onError
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
        this, &UploadDialog::encryptionResult,
        this, &UploadDialog::onEncryptionResult
    );
    connect(
        this, &UploadDialog::uploadResult,
        this, &UploadDialog::onUploadResult
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

void UploadDialog::onEncryptionResult(bool ok) {
    // TODO: update status
}

void UploadDialog::onUploadResult(const API::RequestResult &result, const std::string &success_msg) {
    if (result.ok) {
        QMessageBox::information(
            this,
            "Success",
            QString::fromStdString(success_msg)
        );

        // We are done
        emit accepted();
    } else {
        this->resetUploadButton();
        QMessageBox::critical(this, "Error", QString::fromStdString(result.extractErrorDetails()));
    }
}

void UploadDialog::onChooseFilePathButtonClicked() {
    // Reading file path
    const std::string newFilePath = QFileDialog::getOpenFileName(
        this,
        "Select file to upload",
        QDir::homePath()
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

void UploadDialog::onError(const std::string &message) {
    QMessageBox::critical(
        this,
        "Error",
        QString::fromStdString(message),
        QMessageBox::Ok
    );

    this->resetUploadButton();
}

void UploadDialog::onUploadButtonClicked() {
    if (ui->filePathLineEdit->text().isEmpty() || ui->keyPathLineEdit->text().isEmpty()) {
        QMessageBox::critical(
            this,
            "Error",
            "Please fill all the fields"
        );
        return;
    }

    // Reading file name
    std::string filename = filePath.substr(
        filePath.find_last_of('/') + 1
    );

    // Adding loading icon
    ui->uploadButton->setText("");
    connect(
        loadingAnimation, &QMovie::frameChanged,
        ui->uploadButton, [this] {
        ui->uploadButton->setIcon(loadingAnimation->currentPixmap());
    });
    loadingAnimation->start();

    // Sending an upload request in a separate thread
    QThread* uploadThread = QThread::create([this, filename]() mutable {
        std::fstream sourceFile(this->filePath, std::fstream::in | std::fstream::binary);
        if (!sourceFile.is_open()) {
            emit error("Failed to open the source file");
            return;
        }

        // Read file contents into a string
        std::ostringstream content;
        content << sourceFile.rdbuf();
        sourceFile.close();

        // Encrypted bytes will get saved here
        Utils::ByteArray encrypted;

        // Extract file extension
        const std::string fileExtension = filename.substr(filename.find_first_of('.'));

        // Generate config
        auto AESconfig = Cryptography::AES::generateKey(this->AESType);

        // Encrypt with freshly generated config
        encrypted = Cryptography::AES::encrypt(AESconfig, content.str());

        // Encrypt name if checked
        if (ui->encryptNameCheckBox->isChecked()) {
            auto encryptedName = Cryptography::AES::encrypt(
                AESconfig,
                filename.substr(0, filename.find_first_of('.'))
            );

            // Bytes array as HEX string
            filename = Utils::toHEX(encryptedName) + fileExtension;
        }

        auto configJson = AESconfig.toJSON();

        switch (this->encryptionAlgorithm) {
            // Uploading in a hybrid form. AES is used for file and filename encryption
            // and RSA is for encrypting an AES key,
            case Cryptography::AlgorithmType::Hybrid: {
                // Generate RSA config
                auto RSAconfig = Cryptography::RSA::generateKey(
                    std::atoi(ui->keySizeComboBox->currentText().toStdString().c_str())
                );

                // Encrypt the AES key
                encrypted = Cryptography::RSA::encrypt(RSAconfig, Utils::toHEX(AESconfig.key));

                // Save the config file
                configJson["type"] = "hybrid";
                configJson["AES"]["key"] = Utils::toHEX(encrypted);
                configJson["RSA"]["private_key"] = Cryptography::RSA::keyToString(RSAconfig.keyPair, true);
                configJson["RSA"]["public_key"] = Cryptography::RSA::keyToString(RSAconfig.keyPair, false);

                break;
            }
            default: break;
        }

        // Save the config file
        std::ofstream file(this->keyPath);
        file << configJson.dump();
        file.close();

        // Save encrypted file
#ifdef _WIN32
        std::string encryptedFilePath = this->filePath.substr(0, filePath.find_last_of("\\") + 1) + filename + ".enc";
#else
        std::string encryptedFilePath = this->filePath.substr(0, filePath.find_last_of("/") + 1) + filename + ".enc";
#endif

        std::ofstream encryptedFile(encryptedFilePath, std::ios::binary);

        if (!encryptedFile.is_open()) {
            emit error("Failed to write to encrypted file");
            return;
        }
        encryptedFile.write(reinterpret_cast<const std::ostream::char_type*>(encrypted.data()), encrypted.size());
        encryptedFile.close();

        API::RequestResult result = API::Requests::POST_UPLOAD(
            std::getenv("UPLOAD_URL"),
            {{"filename", filename + ".enc"}},
            encryptedFilePath,
            this->accessToken
        );

        // Emitting signal of upload response receive
        emit uploadResult(result, "Upload successful!");
    });

    connect(uploadThread, &QThread::finished, uploadThread, &QThread::deleteLater);
    uploadThread->start();
}

void UploadDialog::resetUploadButton() {
    loadingAnimation->stop();
    ui->uploadButton->setText("Encrypt && Upload");
    ui->uploadButton->setIcon(QIcon());
}
