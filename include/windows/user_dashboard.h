#ifndef USER_DASHBOARD_H
#define USER_DASHBOARD_H

#include <QMainWindow>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QScrollArea>

#include <thread>
#include <nlohmann/json_fwd.hpp>

#include "api/token_pair.h"
#include "widgets/uploaded_file_panel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class UserDashboard; }
QT_END_NAMESPACE

class UserDashboard final : public QMainWindow {
Q_OBJECT

public:
    UserDashboard(const std::string& username, bool autoRetrieve = true, QWidget *parent = nullptr);
    ~UserDashboard() override;

    void addFile(FileData data);

private slots:
    void onLogoutButtonClicked();

    void onUploadButtonClicked();

    void onDecryptButtonClicked();

    void onDownloadSharedButtonClicked();

    void onShareFile(size_t fileID);

    void onFileDownload(const FileData &fileData);

    void onFileDelete(size_t fileID);

private:
    void closeEvent(QCloseEvent *event) override;

    void tryRetrieveFiles();

    std::unique_ptr<Ui::UserDashboard> ui;

    std::vector<UploadedFilePanel*> uploadedFilePanels;

};

#endif //USER_DASHBOARD_H
