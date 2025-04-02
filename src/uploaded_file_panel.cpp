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

    // Main layout
    layout = new QHBoxLayout;
    this->setLayout(layout);

    //this->setMaximumWidth(850);

    // Left side
    leftLayout = new QVBoxLayout;

    filenameLabel = new QLabel(this);
    filenameLabel->setText(QString::fromStdString(fileData.name));
    filenameLabel->setStyleSheet("font-size: 20pt");
    filenameLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    filenameLabel->setWordWrap(true);

    pathLabel = new QLabel(this);
    pathLabel->setStyleSheet("font-size: 14pt");
    pathLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    pathLabel->setWordWrap(true);

    leftLayout->addWidget(filenameLabel);
    leftLayout->addWidget(pathLabel);

    // Right size
    sizeLabel = new QLabel(this);
    sizeLabel->setText(QString::fromStdString(
        std::to_string(fileData.size / (1000.f * 1000.f)) + "mb"
    ));
    sizeLabel->setStyleSheet("font-size: 16pt");

    downloadButton = new QPushButton(this);
    downloadButton->setText("Download");
    downloadButton->setStyleSheet(Utils::StylesLoader::loadStyleFromFile("basic_button.css"));

    deleteButton = new QPushButton(this);
    deleteButton->setText("Delete");
    deleteButton->setStyleSheet(Utils::StylesLoader::loadStyleFromFile("delete_button.css"));

    layout->addLayout(leftLayout, 1);
    layout->addWidget(sizeLabel);
    layout->addWidget(downloadButton);
    layout->addWidget(deleteButton);

    connect(
        this->downloadButton, &QPushButton::clicked,
        this, [this] { emit downloadButtonPressed(this->fileData); }
    );

    connect(
        this->deleteButton, &QPushButton::clicked,
        this, [this] { emit deleteButtonPressed(this->fileData.id); }
    );
}

UploadedFilePanel::~UploadedFilePanel() {
    delete ui;
}

void UploadedFilePanel::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    pathLabel->setText(
        QFontMetrics(pathLabel->font()).elidedText(
            QString::fromStdString(this->fileData.path),
            Qt::ElideMiddle,
            pathLabel->width()
        )
    );

    filenameLabel->setText(
        QFontMetrics(filenameLabel->font()).elidedText(
            QString::fromStdString(this->fileData.name),
            Qt::ElideMiddle,
            filenameLabel->width()
        )
    );
}
