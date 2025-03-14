#ifndef UPLOADED_FILE_PANEL_H
#define UPLOADED_FILE_PANEL_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class UploadedFilePanel; }
QT_END_NAMESPACE

class UploadedFilePanel final : public QWidget {
Q_OBJECT

public:
    explicit UploadedFilePanel(
        const std::string& fileName,
        const std::string& path,
        const std::string& size,
        QWidget *parent = nullptr
    );
    ~UploadedFilePanel() override;

private:
    Ui::UploadedFilePanel *ui;

    QHBoxLayout *layout;

    // Left part
    QVBoxLayout *leftLayout;
    QLabel *filenameLabel;
    QLabel *pathLabel;

    // Right part
    QLabel *sizeLabel;
    QPushButton *downloadButton;
    QPushButton *deleteButton;

};


#endif //UPLOADED_FILE_PANEL_H
