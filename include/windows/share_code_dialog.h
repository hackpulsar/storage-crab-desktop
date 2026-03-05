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
    ~ShareCodeDialog() override = default;

private:
    Ui::ShareCodeDialog* ui;

};

#endif // SHARE_CODE_DIALOG_H