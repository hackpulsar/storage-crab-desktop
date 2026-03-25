#ifndef SHARE_CODE_DIALOG_H
#define SHARE_CODE_DIALOG_H

#include <QDialog>
#include <QTime>

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

    QTimer* displayTimer;
    QTime timeRemaining;

    void resetCopyButton();

    void restartRefreshTimer();

private slots:
    void onCopyClicked();
    void onRefreshClicked();

};

#endif // SHARE_CODE_DIALOG_H