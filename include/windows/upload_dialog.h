#ifndef UPLOAD_DIALOG_H
#define UPLOAD_DIALOG_H

#include <QHBoxLayout>
#include <QMessageBox>
#include <cryptography/algorithm_types.h>
#include <QtWidgets/QDialog.h>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>

#include "token_pair.h"
#include "cryptography/aes.h"

QT_BEGIN_NAMESPACE
namespace Ui { class UploadDialog; }
QT_END_NAMESPACE

class UploadDialog final : public QDialog {
Q_OBJECT

public:
    UploadDialog(
        std::string filePath,
        std::string accessToken,
        QWidget *parent = nullptr
    );
    ~UploadDialog() override;

public slots:
    // Handles encryption result
    void onEncryptionResult(bool ok);

    // Handles upload result
    void onUploadResult(
        const API::RequestResult &result,
        const std::string &success_msg
    );

    // Switches encryption algorithm
    void switchEncryptionAlgorithm(const QString &newAlgorithm);

    // Handles errors. Shows a message box with error message
    void onError(const std::string &message);

signals:
    void encryptionResult(bool ok);

    void uploadResult(
        const API::RequestResult &result,
        const std::string &success_msg
    );

    void error(const std::string &message);

private:
    // Handles file upload
    void onUploadButtonClicked();

    // Resets upload button
    void resetUploadButton();

    Ui::UploadDialog *ui;

    // Path to a file dialog opens with
    std::string filePath;

    // User access token
    std::string accessToken;

    // Main layout
    QVBoxLayout *layout;

    // File path
    QLabel *filePathLabel;
    QLineEdit *filePathLineEdit;
    QPushButton *chooseFilePathButton;

    // Key path
    QLabel *keyPathLabel;
    QLineEdit *keyPathLineEdit;
    QPushButton *chooseKeyPathButton;

    // Key size
    QLabel *keySizeLabel;
    QComboBox *keySizeComboBox;

    // Encryption algorithm
    QLabel *algorithmLabel;
    QComboBox *algorithmComboBox;

    // Current selected encryption algorithm
    Cryptography::AlgorithmType encryptionAlgorithm;

    QLabel *encryptNameLabel;
    QCheckBox *encryptNameCheckBox;

    // Input text fields layouts
    std::vector<QHBoxLayout*> inputLayouts;

    // Bottom buttons layout
    QHBoxLayout *buttonsLayout;
    QPushButton *uploadButton;
    QPushButton *cancelButton;

    QMovie *loadingAnimation;

};

#endif //UPLOAD_DIALOG_H
