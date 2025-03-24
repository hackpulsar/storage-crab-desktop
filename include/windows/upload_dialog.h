#ifndef UPLOAD_DIALOG_H
#define UPLOAD_DIALOG_H

#include <QHBoxLayout>
#include <QtWidgets/QDialog.h>
#include <QtWidgets/QPushButton>

#include "token_pair.h"

class QVBoxLayout;
QT_BEGIN_NAMESPACE
namespace Ui { class UploadDialog; }
QT_END_NAMESPACE

class UploadDialog final : public QDialog {
Q_OBJECT

public:
    UploadDialog(
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

    // User access token
    std::string accessToken;

    // Main layout
    QVBoxLayout *layout;

    // Input text fields layouts
    std::vector<QHBoxLayout*> inputLayouts;

    // Bottom buttons layout
    QHBoxLayout *buttonsLayout;
    QPushButton *uploadButton;
    QPushButton *cancelButton;

    QMovie *loadingAnimation;

};


#endif //UPLOAD_DIALOG_H
