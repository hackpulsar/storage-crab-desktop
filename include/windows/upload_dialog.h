#ifndef UPLOAD_DIALOG_H
#define UPLOAD_DIALOG_H

#include <QHBoxLayout>
#include <QMessageBox>
#include <encryption/algorithm_types.h>
#include <QtWidgets/QDialog.h>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>

#include "token_pair.h"

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

    // Switches encryption algortithm
    void switchEncryptionAlgorithm(const QString &newAlgorithm);

signals:
    void encryptionResult(bool ok);

    void uploadResult(
        const API::RequestResult &result,
        const std::string &success_msg
    );

private:
    // Handles file upload
    void onUploadButtonClicked();

    Ui::UploadDialog *ui;

    // Path to a file dialog opens with
    std::string filePath;

    // User access token
    std::string accessToken;

    // Main layout
    QVBoxLayout *layout;

    // Filename line
    QLabel *filenameLabel;
    QLineEdit *filenameLineEdit;
    QCheckBox *encryptionCheckBox;

    // Encryption algorithm
    QLabel *algorithmLabel;
    QComboBox *algorithmComboBox;

    // Current selected encryption algorithm
    Encryption::AlgorithmType encryptionAlgorithm;

    // Keys
    QLabel *publicKeyLabel;
    QLineEdit *publicKeyLineEdit;
    QLabel *privateKeyLabel;
    QLineEdit *privateKeyLineEdit;

    // Input text fields layouts
    std::vector<QHBoxLayout*> inputLayouts;

    // Bottom buttons layout
    QHBoxLayout *buttonsLayout;
    QPushButton *regenerateButton;
    QPushButton *uploadButton;
    QPushButton *cancelButton;

    QMovie *loadingAnimation;

};

#endif //UPLOAD_DIALOG_H
