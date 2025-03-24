#include "windows/upload_dialog.h"

#include <QMessageBox>

#include "ui_upload_dialog.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFileDialog>
#include <QThread>
#include <QMovie>

#include "api.h"
#include "requests.hpp"
#include "utils/styles_loader.hpp"
#include "windows/user_dashboard.h"

UploadDialog::UploadDialog(
    std::string accessToken,
    QWidget *parent
)
    : QDialog(parent)
    , ui(new Ui::UploadDialog)
    , accessToken(std::move(accessToken))
{
    ui->setupUi(this);

    // Inputs
    inputLayouts.reserve(4);

    // File
    QLabel *fileLabel = new QLabel;
    fileLabel->setText("File");
    fileLabel->setStyleSheet("font-size: 20pt");

    QLineEdit *fileLineEdit = new QLineEdit;
    fileLineEdit->setStyleSheet("font-size: 20pt; min-height: 1.25em;");

    inputLayouts.push_back(new QHBoxLayout);
    inputLayouts.back()->addWidget(fileLabel);
    inputLayouts.back()->addWidget(fileLineEdit);

    // Encryption
    QLabel *encryptionLabel = new QLabel;
    encryptionLabel->setText("Encryption type");
    encryptionLabel->setStyleSheet("font-size: 20pt");

    QComboBox *encryptionBox = new QComboBox;
    encryptionBox->setStyleSheet("font-size: 20pt; min-height: 1.25em;");
    encryptionBox->addItem("AES");
    encryptionBox->addItem("RSA");
    encryptionBox->addItem("ECC");

    inputLayouts.push_back(new QHBoxLayout);
    inputLayouts.back()->addWidget(encryptionLabel);
    inputLayouts.back()->addWidget(encryptionBox);

    // Keys
    QLabel *publicLabel = new QLabel;
    publicLabel->setText("Public key");
    publicLabel->setStyleSheet("font-size: 20pt");
    QLabel *privateLabel = new QLabel;
    privateLabel->setText("Private key");
    privateLabel->setStyleSheet("font-size: 20pt");

    QLineEdit *publicLineEdit = new QLineEdit;
    publicLineEdit->setStyleSheet("font-size: 20pt; min-height: 1.25em;");
    QLineEdit *privateLineEdit = new QLineEdit;
    privateLineEdit->setStyleSheet("font-size: 20pt; min-height: 1.25em;");

    inputLayouts.push_back(new QHBoxLayout);
    inputLayouts.back()->addWidget(publicLabel);
    inputLayouts.back()->addWidget(publicLineEdit);
    inputLayouts.push_back(new QHBoxLayout);
    inputLayouts.back()->addWidget(privateLabel);
    inputLayouts.back()->addWidget(privateLineEdit);

    using namespace Utils;

    // Adding buttons
    uploadButton = new QPushButton(this);
    uploadButton->setText("Encrypt && Upload");
    uploadButton->setStyleSheet(StylesLoader::loadStyleFromFile("upload_button.css"));

    cancelButton = new QPushButton(this);
    cancelButton->setText("Cancel");
    cancelButton->setStyleSheet(StylesLoader::loadStyleFromFile("cancel_button.css"));

    buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(uploadButton);
    buttonsLayout->addWidget(cancelButton);

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

void UploadDialog::onUploadButtonClicked() {
    // Reading file path
    const std::string filepath = QFileDialog::getOpenFileName(
    this,
    "Select file to upload"
    ).toStdString();

    if (filepath.empty()) return;

    // Reading file name
    const std::string filename = filepath.substr(
    filepath.find_last_of('/') + 1
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
    QThread* uploadThread = QThread::create([this, filepath, filename] {
        // TODO: do the actual encryption
        emit encryptionResult(true);

        API::RequestResult result = API::Requests::POST_UPLOAD(
            API::UPLOAD_URL,
            {{"filename", filename}},
            filepath,
            this->accessToken
        );

        // Emitting signal of upload response receive
        emit uploadResult(result, "Upload successful!");
    });

    connect(uploadThread, &QThread::finished, uploadThread, &QThread::deleteLater);
    uploadThread->start();
}
