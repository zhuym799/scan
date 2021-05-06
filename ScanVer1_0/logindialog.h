#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QtSql>
#include "qscanglobal.h"
namespace Ui {
class loginDialog;
}

class loginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit loginDialog(QWidget *parent = 0);
    ~loginDialog();
protected:

private slots:
    void on_login_btn_4_clicked();
    void on_cancel_btn_4_clicked();
    void on_checkBox_5_clicked();
private:
    Ui::loginDialog *ui;
    QSqlDatabase  DB; //数据库
};

#endif // LOGINDIALOG_H
