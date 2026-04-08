#include "windows/decrypt_dialog.h"
#include "ui_decrypt_dialog.h"

#include <QFileDialog>
#include <QMessageBox>

#include "cryptography/file_crypto.h"
#include "watch_future.hpp"

DecryptDialog::DecryptDialog(const std::string& filePath, QWidget* parent) 
    : QDialog(parent), ui(std::make_unique<Ui::DecryptDialog>()), filePath(filePath)
{
    ui->setupUi(this);

    ui->filePathLineEdit->setText(filePath.c_str());

    loadingAnimation = new QMovie(this);
    loadingAnimation->setFileName("../assets/loading.gif");

    connect(
        ui->cancelButton, &QPushButton::clicked,
        this, [this] { this->close(); }
    );

    connect(
        ui->decryptButton, &QPushButton::clicked,
        this, &DecryptDialog::onDecryptButtonClicked
    );

    connect(
        ui->chooseFilePathButton, &QPushButton::clicked,
        this, &DecryptDialog::onChooseFilePathButtonClicked
    );
    connect(
        ui->chooseKeyPathButton, &QPushButton::clicked,
        this, &DecryptDialog::onChooseKeyPathButtonClicked
    );
}

DecryptDialog::~DecryptDialog() = default;

void DecryptDialog::onDecryptButtonClicked() {
    if (ui->filePathLineEdit->text().isEmpty() || ui->keyPathLineEdit->text().isEmpty()) {
        QMessageBox::critical(this, "Error", "Please fill all the fields");
        return;
    }

    ui->decryptButton->setText("");
    connect(
        loadingAnimation, &QMovie::frameChanged,
        ui->decryptButton, [this] {
        ui->decryptButton->setIcon(loadingAnimation->currentPixmap());
    });
    loadingAnimation->start();

    using namespace Cryptography;

    FileCrypto::DecryptOptions opts {
        ui->filePathLineEdit->text().toStdString(),
        ui->keyPathLineEdit->text().toStdString(),
        ui->encryptNameCheckBox->isChecked()
    };

    watchFuture(
        this, dispatch([opts] { return FileCrypto::decryptFile(opts); }),
        [this](const FileCrypto::Result& result) {
            QMessageBox::information(this, "Success", "File decrypted successfully, verify file integrity.");
            this->close();
        },
        [this](const FileCrypto::Result&) { 
            QMessageBox::critical(this, "Error", "Something went wrong during file decryption."); 
        }
    );
}

void DecryptDialog::onChooseFilePathButtonClicked() {
    auto dialog = QFileDialog(this);
    dialog.setWindowTitle("Select file to decrypt");
    dialog.setDirectory(QDir::homePath());
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Encrypted Files (*.enc);;All Files (*)"));

    if (dialog.exec()) {
        const std::string newFilePath = dialog.selectedFiles().first().toStdString();

        if (newFilePath != "") {
            ui->filePathLineEdit->setText(QString::fromStdString(newFilePath));
            this->filePath = newFilePath;
        }
    }
}

void DecryptDialog::onChooseKeyPathButtonClicked() {
    auto dialog = QFileDialog(this);
    dialog.setWindowTitle("Select a key");
    dialog.setDirectory(QDir::homePath());
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Crab Key (*.crbkey);;All Files (*)"));

    if (dialog.exec()) {
        const std::string newKeyPath = dialog.selectedFiles().first().toStdString();

        if (newKeyPath != "") {
            ui->keyPathLineEdit->setText(QString::fromStdString(newKeyPath));
            this->keyPath = newKeyPath;
        }
    }
}