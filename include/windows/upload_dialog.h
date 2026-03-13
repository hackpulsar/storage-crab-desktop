#ifndef UPLOAD_DIALOG_H
#define UPLOAD_DIALOG_H

#include <QDialog>

#include "cryptography/algorithm_types.h"

QT_BEGIN_NAMESPACE
namespace Ui { class UploadDialog; }
QT_END_NAMESPACE

class UploadDialog final : public QDialog {
Q_OBJECT

public:
    UploadDialog(std::string filePath, QWidget *parent = nullptr);
    ~UploadDialog() override;

public slots:
    void onChooseFilePathButtonClicked();

    void onChooseKeyPathButtonClicked();

    void switchEncryptionAlgorithm(const QString &newAlgorithm);

    void switchAESType(const QString &newAESType);

private:
    void onUploadButtonClicked();

    void resetUploadButton();

    Ui::UploadDialog *ui;

    std::string filePath;

    std::string keyPath;

    QMovie *loadingAnimation;

    Cryptography::AlgorithmType encryptionAlgorithm = Cryptography::AlgorithmType::AES;
    Cryptography::AESType AESType = Cryptography::AESType::AES_128;

};

#endif //UPLOAD_DIALOG_H
