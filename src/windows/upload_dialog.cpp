#include "windows/upload_dialog.h"

#include <QMessageBox>

#include "ui_upload_dialog.h"

#include <QtWidgets/QFileDialog>
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

    // Inputs
    inputLayouts.reserve(2);

    // File path
    filePathLabel = new QLabel;
    filePathLabel->setText("File path");
    filePathLabel->setStyleSheet("font-size: 20pt");

    filePathLineEdit = new QLineEdit;
    filePathLineEdit->setStyleSheet("font-size: 16pt; min-height: 1.25em;");
    filePathLineEdit->setText(QString::fromStdString(this->filePath));

    chooseFilePathButton = new QPushButton;
    chooseFilePathButton->setText("...");

    if (Utils::isDarkMode(this))
        chooseFilePathButton->setStyleSheet(Utils::StylesLoader::loadStyleFromFile("choose_button_dark.css"));
    else
        chooseFilePathButton->setStyleSheet(Utils::StylesLoader::loadStyleFromFile("choose_button_light.css"));

    // Key path
    keyPathLabel = new QLabel;
    keyPathLabel->setText("Key path");
    keyPathLabel->setStyleSheet("font-size: 20pt");

    keyPathLineEdit = new QLineEdit;
    keyPathLineEdit->setStyleSheet("font-size: 16pt; min-height: 1.25em;");

    chooseKeyPathButton = new QPushButton;
    chooseKeyPathButton->setText("...");

    if (Utils::isDarkMode(this))
        chooseKeyPathButton->setStyleSheet(Utils::StylesLoader::loadStyleFromFile("choose_button_dark.css"));
    else
        chooseKeyPathButton->setStyleSheet(Utils::StylesLoader::loadStyleFromFile("choose_button_light.css"));

    // Encryption type
    algorithmLabel = new QLabel;
    algorithmLabel->setText("Encryption type");
    algorithmLabel->setStyleSheet("font-size: 20pt");

    algorithmComboBox = new QComboBox;
    algorithmComboBox->setStyleSheet("font-size: 16pt; min-height: 1.25em;");
    algorithmComboBox->addItem("AES");
    algorithmComboBox->addItem("Hybrid (AES + RSA)");

    connect(
       algorithmComboBox, &QComboBox::currentTextChanged,
       this, &UploadDialog::switchEncryptionAlgorithm
   );

    // AES type
    aesTypeLabel = new QLabel;
    aesTypeLabel->setText("AES Type");
    aesTypeLabel->setStyleSheet("font-size: 20pt");

    aesTypeComboBox = new QComboBox;
    aesTypeComboBox->setStyleSheet("font-size: 16pt; min-height: 1.25em;");
    aesTypeComboBox->addItem("AES-128");
    aesTypeComboBox->addItem("AES-192");
    aesTypeComboBox->addItem("AES-256");

    connect(
       aesTypeComboBox, &QComboBox::currentTextChanged,
       this, &UploadDialog::switchAESType
   );

    // Key size
    keySizeLabel= new QLabel;
    keySizeLabel->setText("RSA key size (bits)");
    keySizeLabel->setStyleSheet("font-size: 20pt");
    keySizeLabel->setEnabled(false);

    keySizeComboBox = new QComboBox;
    keySizeComboBox->setStyleSheet("font-size: 16pt; min-height: 1.25em;");
    keySizeComboBox->addItem("1024");
    keySizeComboBox->addItem("2048");
    keySizeComboBox->addItem("4096");
    keySizeComboBox->setEnabled(false);

    // Encrypt name checkbox
    encryptNameLabel = new QLabel;
    encryptNameLabel->setText("Encrypt name");
    encryptNameLabel->setStyleSheet("font-size: 20pt");

    encryptNameCheckBox = new QCheckBox;
    encryptNameCheckBox->setStyleSheet("font-size: 20pt;");

    using namespace Utils;

    // Adding buttons
    uploadButton = new QPushButton(this);
    uploadButton->setText("Encrypt && Upload");
    uploadButton->setIconSize(QSize(30, 30));
    uploadButton->setStyleSheet(StylesLoader::loadStyleFromFile("upload_button.css"));

    cancelButton = new QPushButton(this);
    cancelButton->setText("Cancel");
    cancelButton->setStyleSheet(StylesLoader::loadStyleFromFile("cancel_button.css"));

    buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addWidget(uploadButton);

    // Assembling the layouts
    inputLayouts.push_back(new QHBoxLayout);

    // Qt ownership system will handle deletion
    auto* labelsLayout = new QVBoxLayout;
    labelsLayout->addWidget(filePathLabel);
    labelsLayout->addWidget(keyPathLabel);
    labelsLayout->addWidget(algorithmLabel);
    labelsLayout->addWidget(aesTypeLabel);
    labelsLayout->addWidget(keySizeLabel);
    labelsLayout->addWidget(encryptNameLabel);

    inputLayouts.back()->addLayout(labelsLayout);

    // Same here, Qt will handle the pointer
    auto* fieldsLayout = new QVBoxLayout;

    // Don't yell CLion, it is not a memory leak
    auto* filePathLayout = new QHBoxLayout;
    filePathLayout->addWidget(filePathLineEdit);
    filePathLayout->addWidget(chooseFilePathButton);
    fieldsLayout->addLayout(filePathLayout);

    // That is also not a memory leak, CLion stop
    auto* keyPathLayout = new QHBoxLayout;
    keyPathLayout->addWidget(keyPathLineEdit);
    keyPathLayout->addWidget(chooseKeyPathButton);
    fieldsLayout->addLayout(keyPathLayout);

    fieldsLayout->addWidget(algorithmComboBox);
    fieldsLayout->addWidget(aesTypeComboBox);
    fieldsLayout->addWidget(keySizeComboBox);
    fieldsLayout->addWidget(encryptNameCheckBox);

    inputLayouts.back()->addLayout(fieldsLayout);

    // Assembling main layout
    layout = new QVBoxLayout(this);
    for (const auto& l : inputLayouts)
        layout->addLayout(l);
    layout->addLayout(buttonsLayout);

    // Loading animation
    loadingAnimation = new QMovie(this);
    loadingAnimation->setFileName("../assets/loading.gif");

    connect(
        this, &UploadDialog::error,
        this, &UploadDialog::onError
    );

    connect(
        uploadButton, &QPushButton::clicked,
        this, &UploadDialog::onUploadButtonClicked
    );
    connect(
        cancelButton, &QPushButton::clicked,
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
        chooseFilePathButton, &QPushButton::clicked,
        this, &UploadDialog::onChooseFilePathButtonClicked
    );
    connect(
        chooseKeyPathButton, &QPushButton::clicked,
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

        QMessageBox::critical(
            this,
            "Error",
            QString::fromStdString(result.response.at("details").get<std::string>())
        );
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
        this->filePathLineEdit->setText(QString::fromStdString(newFilePath));
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
            this->keyPathLineEdit->setText(QString::fromStdString(newKeyPath));
            this->keyPath = newKeyPath;
        }
    }
}

void UploadDialog::switchEncryptionAlgorithm(const QString &newAlgorithm) {
    const auto algorithmType = Cryptography::algorithmTypeFromString(newAlgorithm.toStdString());
    this->encryptionAlgorithm = algorithmType;

    // Disable/enable RSA key size UI elements
    if (this->encryptionAlgorithm == Cryptography::AlgorithmType::AES) {
        this->keySizeLabel->setEnabled(false);
        this->keySizeComboBox->setEnabled(false);
    } else {
        this->keySizeLabel->setEnabled(true);
        this->keySizeComboBox->setEnabled(true);
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
    if (this->filePathLineEdit->text().isEmpty() || this->keyPathLineEdit->text().isEmpty()) {
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
    uploadButton->setText("");
    connect(
        loadingAnimation, &QMovie::frameChanged,
        uploadButton, [this] {
        uploadButton->setIcon(loadingAnimation->currentPixmap());
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
        if (this->encryptNameCheckBox->isChecked()) {
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
                    std::atoi(this->keySizeComboBox->currentText().toStdString().c_str())
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
            API::UPLOAD_URL,
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
    uploadButton->setText("Encrypt && Upload");
    uploadButton->setIcon(QIcon());
}
