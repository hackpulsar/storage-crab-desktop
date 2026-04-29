#include "widgets/uploaded_file_panel.h"
#include "ui_uploaded_file_panel.h"

#include "api/file_data.hpp"

UploadedFilePanel::UploadedFilePanel(
    const FileData &fileData,
    QWidget *parent
)
    : QWidget(parent)
    , ui(new Ui::UploadedFilePanel)
    , fileData(fileData)
{
    ui->setupUi(this);

    ui->filenameLabel->setText(fileData.name.c_str());
    ui->pathLabel->setText(fileData.path.c_str());

    // Set widget background color
    QPalette pal = palette();
    pal.setColor(
        QPalette::Window,
        this->palette().color(QPalette::Window).lightness() < 128 ?
            QColor::fromRgb(40, 40, 40) : QColor::fromRgb(239, 239, 239)
    );
    setAutoFillBackground(true);
    setPalette(pal);

    ui->sizeLabel->setText(QString::fromStdString(
        std::to_string(fileData.size / (1000.f * 1000.f)) + "mb"
    ));

    ui->shareButton->setIcon(QIcon(QString(ASSETS_PATH) + "/share.png"));
    ui->downloadButton->setIcon(QIcon(QString(ASSETS_PATH) + "/download.png"));
    ui->deleteButton->setIcon(QIcon(QString(ASSETS_PATH) + "/delete.png"));

    connect(
        ui->shareButton, &QPushButton::clicked,
        this, [this] { emit shareButtonPressed(this->fileData.id); }
    );

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

QString UploadedFilePanel::elideText(const QString& text, const QFont& font, int width) const {
    return QFontMetrics(font).elidedText(text, Qt::ElideMiddle, width);
}

void UploadedFilePanel::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    ui->pathLabel->setText(elideText(
        fileData.path.c_str(),
        ui->pathLabel->font(),
        ui->pathLabel->width()
    ));

    ui->pathLabel->setText(elideText(
        fileData.name.c_str(),
        ui->filenameLabel->font(),
        ui->filenameLabel->width()
    ));
}
