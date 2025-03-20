#include "widgets/uploaded_file_panel.h"
#include "ui_uploaded_file_panel.h"

#include "utils/styles_loader.hpp"

UploadedFilePanel::UploadedFilePanel(
    const std::string& fileName,
    const std::string& path,
    const std::string& size,
    QWidget *parent
)
    : QWidget(parent)
    , ui(new Ui::UploadedFilePanel)
    , fullPath(path)
{
    ui->setupUi(this);

    // Set widget background color
    QPalette pal = palette();
    pal.setColor(
        QPalette::Window,
        palette().color(QPalette::Window).lightness() < 128 ?
            QColor::fromRgb(40, 40, 40) : QColor::fromRgb(211, 211, 211)
    );
    setAutoFillBackground(true);
    setPalette(pal);

    // Main layout
    layout = new QHBoxLayout;
    this->setLayout(layout);

    // Left side
    leftLayout = new QVBoxLayout;

    filenameLabel = new QLabel(this);
    filenameLabel->setText(QString::fromStdString(fileName));
    filenameLabel->setStyleSheet("font-size: 20pt");

    pathLabel = new QLabel(this);
    pathLabel->setStyleSheet("font-size: 14pt");
    pathLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    pathLabel->setWordWrap(true);

    leftLayout->addWidget(filenameLabel);
    leftLayout->addWidget(pathLabel);

    // Right size
    sizeLabel = new QLabel(this);
    sizeLabel->setText(QString::fromStdString(size));
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
}

UploadedFilePanel::~UploadedFilePanel() {
    delete ui;
}

void UploadedFilePanel::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    pathLabel->setText(
        QFontMetrics(pathLabel->font()).elidedText(
            QString::fromStdString(fullPath),
            Qt::ElideMiddle,
            pathLabel->width()
        )
    );
}
