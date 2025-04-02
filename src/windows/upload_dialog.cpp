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

    // Key size
    keySizeLabel= new QLabel;
    keySizeLabel->setText("Key size (bits)");
    keySizeLabel->setStyleSheet("font-size: 20pt");

    keySizeComboBox = new QComboBox;
    keySizeComboBox->setStyleSheet("font-size: 16pt; min-height: 1.25em;");
    keySizeComboBox->addItem("1024");
    keySizeComboBox->addItem("2048");
    keySizeComboBox->addItem("4096");

    // Encryption type
    algorithmLabel = new QLabel;
    algorithmLabel->setText("Encryption type");
    algorithmLabel->setStyleSheet("font-size: 20pt");

    algorithmComboBox = new QComboBox;
    algorithmComboBox->setStyleSheet("font-size: 16pt; min-height: 1.25em;");
    algorithmComboBox->addItem("AES");
    algorithmComboBox->addItem("RSA");
    algorithmComboBox->addItem("ECC");

    connect(
        algorithmComboBox, &QComboBox::currentTextChanged,
        this, &UploadDialog::switchEncryptionAlgorithm
    );

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
    labelsLayout->addWidget(keySizeLabel);
    labelsLayout->addWidget(algorithmLabel);
    labelsLayout->addWidget(encryptNameLabel);

    inputLayouts.back()->addLayout(labelsLayout);

    // Same here, Qt will handle the pointer, don't yell CLion, it is not a memory leak
    auto* fieldsLayout = new QVBoxLayout;

    auto* filePathLayout = new QHBoxLayout;
    filePathLayout->addWidget(filePathLineEdit);
    filePathLayout->addWidget(chooseFilePathButton);
    fieldsLayout->addLayout(filePathLayout);

    auto* keyPathLayout = new QHBoxLayout;
    keyPathLayout->addWidget(keyPathLineEdit);
    keyPathLayout->addWidget(chooseKeyPathButton);
    fieldsLayout->addLayout(keyPathLayout);

    fieldsLayout->addWidget(keySizeComboBox);
    fieldsLayout->addWidget(algorithmComboBox);
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

void UploadDialog::switchEncryptionAlgorithm(const QString &newAlgorithm) {
    const auto algorithmType = Cryptography::algorithmTypeFromString(newAlgorithm.toStdString());
    this->encryptionAlgorithm = algorithmType;
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

        // Generate config
        auto config = Cryptography::AES::generateKey(
            std::atoi(this->keySizeComboBox->currentText().toStdString().c_str()),
            IV_SIZE
        );

        // Encrypt with freshly generated config
        auto encrypted = Cryptography::AES::encrypt(config, content.str());

        // Extract file extension
        const std::string fileExtension = filename.substr(filename.find_first_of('.'));

        // Encrypt name if checked
        if (this->encryptNameCheckBox->isChecked()) {
            auto encryptedName = Cryptography::AES::encrypt(
                config,
                filename.substr(0, filename.find_first_of('.'))
            );

            // Bytes array as HEX string
            std::ostringstream oss;
            for (auto byte : encryptedName)
                oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
            filename = oss.str() + fileExtension;
        }

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
