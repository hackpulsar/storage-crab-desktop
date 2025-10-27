#include "widgets/uploaded_file_panel.h"
#include "ui_uploaded_file_panel.h"

#include "utils/styles_loader.hpp"
#include "utils/dark_mode.hpp"
#include "file_data.hpp"

UploadedFilePanel::UploadedFilePanel(
    const FileData &fileData,
    QWidget *parent
)
    : QWidget(parent)
    , ui(new Ui::UploadedFilePanel)
    , fileData(fileData)
{
    ui->setupUi(this);

    // Set widget background color
    QPalette pal = palette();
    pal.setColor(
        QPalette::Window,
        Utils::isDarkMode(this) ?
            QColor::fromRgb(40, 40, 40) : QColor::fromRgb(211, 211, 211)
    );
    setAutoFillBackground(true);
    setPalette(pal);

    ui->sizeLabel->setText(QString::fromStdString(
        std::to_string(fileData.size / (1000.f * 1000.f)) + "mb"
    ));

    connect(
        ui->downloadButton, &QPushButton::clicked,
        this, [this] { emit downloadButtonPressed(this->fileData); }
    );

    connect(
        ui->deleteButton, &QPushButton::clicked,
        this, [this] { emit deleteButtonPressed(this->fileData.id); }
    );
}

UploadedFilePanel::~UploadedFilePanel() {
    delete ui;
}

void UploadedFilePanel::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    ui->pathLabel->setText(
        QFontMetrics(ui->pathLabel->font()).elidedText(
            QString::fromStdString(this->fileData.path),
            Qt::ElideMiddle,
            ui->pathLabel->width()
        )
    );

    ui->filenameLabel->setText(
        QFontMetrics(ui->filenameLabel->font()).elidedText(
            QString::fromStdString(this->fileData.name),
            Qt::ElideMiddle,
            ui->filenameLabel->width()
        )
    );
}
