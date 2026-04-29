#ifndef UPLOADED_FILE_PANEL_H
#define UPLOADED_FILE_PANEL_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "api/file_data.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class UploadedFilePanel; }
QT_END_NAMESPACE

class UploadedFilePanel final : public QWidget {
Q_OBJECT

public:
    explicit UploadedFilePanel(
        const FileData &fileData,
        QWidget *parent = nullptr
    );
    ~UploadedFilePanel() override;

    QString elideText(const QString& text, const QFont& font, int width) const;

signals:
    void shareButtonPressed(size_t file_id);
    void downloadButtonPressed(const FileData &fileData);
    void deleteButtonPressed(size_t file_id);

private:
    void resizeEvent(QResizeEvent *event) override;

    Ui::UploadedFilePanel *ui;

    FileData fileData;

};


#endif //UPLOADED_FILE_PANEL_H
