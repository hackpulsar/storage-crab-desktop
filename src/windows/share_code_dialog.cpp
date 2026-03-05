#include "windows/share_code_dialog.h"
#include "ui_share_code_dialog.h"

ShareCodeDialog::ShareCodeDialog(const std::string code, QWidget* parent)
    : QDialog(parent), ui(new Ui::ShareCodeDialog)
{
    ui->setupUi(this);

    ui->codeLabel->setText(QString::fromStdString(code));

    connect(
        ui->closeButton, &QPushButton::clicked,
        this, [this] { this->close(); }
    );

}