#ifndef USER_DASHBOARD_H
#define USER_DASHBOARD_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class UserDashboard; }
QT_END_NAMESPACE

class UserDashboard : public QWidget {
Q_OBJECT

public:
    explicit UserDashboard(QWidget *parent = nullptr);
    ~UserDashboard() override;

private:
    Ui::UserDashboard *ui;
};


#endif //USER_DASHBOARD_H
