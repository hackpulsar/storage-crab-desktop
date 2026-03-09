#ifndef SHARE_CODE_DIALOG_H
#define SHARE_CODE_DIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class ShareCodeDialog; }
QT_END_NAMESPACE

class ShareCodeDialog final : public QDialog {
Q_OBJECT

public:
    ShareCodeDialog(std::string code, QWidget* parent = nullptr);
    ~ShareCodeDialog() override;

private:
    std::unique_ptr<Ui::ShareCodeDialog> ui;

    std::string code;

    QTimer* copyTimer;
    QIcon copyIcon, copiedIcon;

private slots:
    void onCopyClicked();

};

#endif // SHARE_CODE_DIALOG_H