#ifndef DOWNLOAD_SHARED_DIALOG_H
#define DOWNLOAD_SHARED_DIALOG_H

#include <QDialog>

#include "api/token_pair.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DownloadSharedDialog; }
QT_END_NAMESPACE

class DownloadSharedDialog final : public QDialog {
Q_OBJECT

public:
    DownloadSharedDialog(QWidget* parent = nullptr);
    ~DownloadSharedDialog() override;

private:
    std::unique_ptr<Ui::DownloadSharedDialog> ui;

private slots:
    void onDownloadButtonClicked();

};

#endif // DOWNLOAD_SHARED_DIALOG_H