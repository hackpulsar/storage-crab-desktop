#include "windows/share_code_dialog.h"
#include "ui_share_code_dialog.h"

#include <memory>

#include <QTimer>
#include <QClipboard>
#include <QMessageBox>

#include "api/api_dispatcher.hpp"
#include "watch_future.hpp"

ShareCodeDialog::ShareCodeDialog(const std::string code, const size_t fileID, QWidget* parent)
    : QDialog(parent), ui(std::make_unique<Ui::ShareCodeDialog>()), code(code), fileID(fileID)
{
    ui->setupUi(this);

    this->copyIcon = QIcon(QString(ASSETS_PATH) + "/copy.png");
    this->clickedIcon = QIcon(QString(ASSETS_PATH) + "/check.png");
    this->refreshIcon = QIcon(QString(ASSETS_PATH) + "/refresh.png");

    copyTimer = new QTimer(this);
    displayTimer = new QTimer(this);

    ui->codeLabel->setText(QString::fromStdString(code));
    ui->copyButton->setIcon(this->copyIcon);
    ui->refreshButton->setIcon(this->refreshIcon);

    connect(
        ui->closeButton, &QPushButton::clicked,
        this, [this] { this->close(); }
    );

    connect(
        copyTimer, &QTimer::timeout, 
        this, &ShareCodeDialog::resetCopyButton
    );

    connect(
        displayTimer, &QTimer::timeout,
        this, [this] {
            timeRemaining = timeRemaining.addSecs(-1);
            ui->timeLabel->setText(timeRemaining.toString("mm:ss"));

            if (timeRemaining == QTime(0, 0, 0)) {
                displayTimer->stop();
                this->onRefreshClicked();
            }
        }
    );

    this->restartRefreshTimer();

    connect(ui->copyButton, &QPushButton::clicked, this, &ShareCodeDialog::onCopyClicked);
    connect(ui->refreshButton, &QPushButton::clicked, this, &ShareCodeDialog::onRefreshClicked);
}

ShareCodeDialog::~ShareCodeDialog() = default;

void ShareCodeDialog::resetCopyButton() {
    ui->copyButton->setEnabled(true);
    ui->copyButton->setIcon(this->copyIcon);
}

void ShareCodeDialog::restartRefreshTimer() {
    timeRemaining = QTime(0, 5, 0);
    displayTimer->start(1000);
    ui->timeLabel->setText(timeRemaining.toString("mm:ss"));
}

void ShareCodeDialog::onCopyClicked() {
    ui->copyButton->setEnabled(false);
    ui->copyButton->setIcon(this->clickedIcon);

    copyTimer->start(3000);

    static QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(QString::fromStdString(this->code));
}

void ShareCodeDialog::onRefreshClicked() {
    ui->refreshButton->setEnabled(false);
    ui->refreshButton->setIcon(this->clickedIcon);
    ui->codeLabel->setText("...");

    this->resetCopyButton();

    // Sharing the same file again, which is basically regenerating code
    watchFuture(
        this, ApiDispatcher::instance().shareFile(fileID),
        [this](const API::RequestResult& response) {
            ui->codeLabel->setText(QString::fromStdString(response.body.at("code").get<std::string>()));
            ui->refreshButton->setEnabled(true);
            ui->refreshButton->setIcon(this->refreshIcon);

            this->restartRefreshTimer();
        },
        [this](const API::RequestResult& response) {
            QMessageBox::critical(this, "Error", QString::fromStdString(response.extractErrorDetails()));
            this->close();
        }
    );
}
