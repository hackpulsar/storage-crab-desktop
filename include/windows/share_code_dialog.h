#ifndef SHARE_CODE_DIALOG_H
#define SHARE_CODE_DIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class ShareCodeDialog; }
QT_END_NAMESPACE

class ShareCodeDialog final : public QDialog {
Q_OBJECT

public:
    ShareCodeDialog(std::string code, size_t fileID, QWidget* parent = nullptr);
    ~ShareCodeDialog() override;

private:
    std::unique_ptr<Ui::ShareCodeDialog> ui;

    std::string code;
    size_t fileID;

    QTimer* copyTimer;
    QIcon copyIcon, clickedIcon, refreshIcon;

    void resetCopyButton();

private slots:
    void onCopyClicked();
    void onRefreshClicked();

};

#endif // SHARE_CODE_DIALOG_H