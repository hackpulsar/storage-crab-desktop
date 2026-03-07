#include "windows/share_code_dialog.h"
#include "ui_share_code_dialog.h"

#include <memory>

#include <QTimer>
#include <QClipboard>

ShareCodeDialog::ShareCodeDialog(const std::string code, QWidget* parent)
    : QDialog(parent), ui(std::make_unique<Ui::ShareCodeDialog>()), code(code)
{
    ui->setupUi(this);

    this->copyIcon = QIcon("../assets/copy.png");
    this->copiedIcon = QIcon("../assets/check.png");

    copyTimer = new QTimer(this);

    ui->codeLabel->setText(QString::fromStdString(code));
    ui->copyButton->setIcon(this->copyIcon);

    connect(
        ui->closeButton, &QPushButton::clicked,
        this, [this] { this->close(); }
    );

    connect(
        copyTimer, &QTimer::timeout, 
        this, [this] { ui->copyButton->setIcon(this->copyIcon); }
    );
    connect(ui->copyButton, &QPushButton::clicked, this, &ShareCodeDialog::onCopyClicked);

}

ShareCodeDialog::~ShareCodeDialog() = default;

void ShareCodeDialog::onCopyClicked() {
    ui->copyButton->setIcon(this->copiedIcon);
    copyTimer->start(3000);

    static QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(QString::fromStdString(this->code));
}
