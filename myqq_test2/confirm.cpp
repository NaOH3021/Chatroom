#include "confirm.h"
#include "ui_confirm.h"
#include "confirm.h"
#include "ui_confirm.h"
#include <QMessageBox>


confirm::confirm(QString password, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::confirm)
{
    ui->setupUi(this);
    this->password = password;

     connect(ui->confirmButton, &QPushButton::clicked, this, &confirm::on_confirmButton_clicked);
}

confirm::~confirm()
{
    delete ui;
}



void confirm::on_confirmButton_clicked()
{
    bool ok = false;
    QString m_password = ui->passwordConfirm->text();
    if (m_password == password) {
        close();
        ok = true;
    } else {
        QMessageBox::warning(this, "Register Failed", "Passwords do not match. Please try again.");
    }

    // 验证结果信号
    emit passwordConfirmed(ok);
}
