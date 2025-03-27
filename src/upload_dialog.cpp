#include "windows/upload_dialog.h"

#include <QMessageBox>

#include "ui_upload_dialog.h"

#include <QtWidgets/QFileDialog>
#include <QCheckBox>
#include <QThread>
#include <QMovie>

#include "api.h"
#include "requests.hpp"
#include "utils/styles_loader.hpp"
#include "windows/user_dashboard.h"
#include "encryption/algorithm_types.h"

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
    inputLayouts.reserve(4);

    // File
    filenameLabel = new QLabel;
    filenameLabel->setText("File");
    filenameLabel->setStyleSheet("font-size: 20pt");

    filenameLineEdit = new QLineEdit;
    filenameLineEdit->setStyleSheet("font-size: 20pt; min-height: 1.25em;");
    filenameLineEdit->setText(QString::fromStdString(this->filePath));

    encryptionCheckBox = new QCheckBox("Encrypt name");
    encryptionCheckBox->setStyleSheet("font-size: 20pt;");

    inputLayouts.push_back(new QHBoxLayout);
    inputLayouts.back()->addWidget(filenameLabel);
    inputLayouts.back()->addWidget(filenameLineEdit);
    inputLayouts.back()->addWidget(encryptionCheckBox);

    // Encryption
    algorithmLabel = new QLabel;
    algorithmLabel->setText("Encryption type");
    algorithmLabel->setStyleSheet("font-size: 20pt");

    algorithmComboBox = new QComboBox;
    algorithmComboBox->setStyleSheet("font-size: 20pt; min-height: 1.25em;");
    algorithmComboBox->addItem("AES");
    algorithmComboBox->addItem("RSA");
    algorithmComboBox->addItem("ECC");

    connect(
        algorithmComboBox, &QComboBox::currentTextChanged,
        this, &UploadDialog::switchEncryptionAlgorithm
    );

    inputLayouts.push_back(new QHBoxLayout);
    inputLayouts.back()->addWidget(algorithmLabel);
    inputLayouts.back()->addWidget(algorithmComboBox);

    // Keys
    publicKeyLabel = new QLabel;
    publicKeyLabel->setText("Public key");
    publicKeyLabel->setStyleSheet("font-size: 20pt");

    privateKeyLabel = new QLabel;
    privateKeyLabel->setText("Private key");
    privateKeyLabel->setStyleSheet("font-size: 20pt");

    publicKeyLineEdit = new QLineEdit;
    publicKeyLineEdit->setStyleSheet("font-size: 20pt; min-height: 1.25em;");

    privateKeyLineEdit = new QLineEdit;
    privateKeyLineEdit->setStyleSheet("font-size: 20pt; min-height: 1.25em;");
    privateKeyLineEdit->setEnabled(false); // AES is selected by default, so no private key

    inputLayouts.push_back(new QHBoxLayout);
    inputLayouts.back()->addWidget(publicKeyLabel);
    inputLayouts.back()->addWidget(publicKeyLineEdit);
    inputLayouts.push_back(new QHBoxLayout);
    inputLayouts.back()->addWidget(privateKeyLabel);
    inputLayouts.back()->addWidget(privateKeyLineEdit);

    using namespace Utils;

    // Adding buttons
    regenerateButton = new QPushButton(this);
    regenerateButton->setText("Regenerate");
    regenerateButton->setStyleSheet(StylesLoader::loadStyleFromFile("basic_button.css"));

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
    buttonsLayout->addWidget(regenerateButton);

    // Assembling main layout
    layout = new QVBoxLayout(this);
    for (const auto& l : inputLayouts)
        layout->addLayout(l);
    layout->addLayout(buttonsLayout);

    // Loading animation
    loadingAnimation = new QMovie(this);
    loadingAnimation->setFileName("../assets/loading.gif");

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
        // Reset upload button
        loadingAnimation->stop();
        uploadButton->setText("Encrypt && Upload");
        uploadButton->setIcon(QIcon());

        QMessageBox::critical(
            this,
            "Error",
            QString::fromStdString(result.response.at("details").get<std::string>())
        );
    }
}

void UploadDialog::switchEncryptionAlgorithm(const QString &newAlgorithm) {
    const auto algorithmType = Encryption::algorithmTypeFromString(newAlgorithm.toStdString());
    this->encryptionAlgorithm = algorithmType;

    // AES doesn't use private key
    if (algorithmType == Encryption::AlgorithmType::AES) {
        this->privateKeyLineEdit->setEnabled(false);
    } else {
        this->privateKeyLineEdit->setEnabled(true);
    }
}

void UploadDialog::onUploadButtonClicked() {
    // Reading file name
    const std::string filename = filePath.substr(
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
    QThread* uploadThread = QThread::create([this, filename] {
        // TODO: do the actual encryption
        emit encryptionResult(true);

        API::RequestResult result = API::Requests::POST_UPLOAD(
            API::UPLOAD_URL,
            {{"filename", filename}},
            this->filePath,
            this->accessToken
        );

        // Emitting signal of upload response receive
        emit uploadResult(result, "Upload successful!");
    });

    connect(uploadThread, &QThread::finished, uploadThread, &QThread::deleteLater);
    uploadThread->start();
}
