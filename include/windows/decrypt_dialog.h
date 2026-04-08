#pragma once

#include <QDialog>
#include <QMovie>

QT_BEGIN_NAMESPACE
namespace Ui { class DecryptDialog; }
QT_END_NAMESPACE

class DecryptDialog final : public QDialog {
Q_OBJECT

public:
    DecryptDialog(const std::string& filePath, QWidget* parent = nullptr);
    ~DecryptDialog();

private:
    std::unique_ptr<Ui::DecryptDialog> ui;

    std::string filePath;
    std::string keyPath;

    QMovie *loadingAnimation;

private slots:
    void onDecryptButtonClicked();

    void onChooseFilePathButtonClicked();

    void onChooseKeyPathButtonClicked();

};
