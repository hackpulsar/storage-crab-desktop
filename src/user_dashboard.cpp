#include "user_dashboard.h"
#include "ui_user_dashboard.h"

UserDashboard::UserDashboard(QWidget *parent) :
    QWidget(parent), ui(new Ui::UserDashboard) {
    ui->setupUi(this);
}

UserDashboard::~UserDashboard() {
    delete ui;
}
