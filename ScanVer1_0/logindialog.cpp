#include "logindialog.h"
#include "ui_logindialog.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <qdebug.h>
#include <QTextCodec>
#include <QSettings>
loginDialog::loginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setWindowTitle("登录");
    QSettings set(QApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
    set.setIniCodec(QTextCodec::codecForName("GB2312"));
    set.beginGroup("userinfo");
    ui->userNoText_5->setText(set.value("userno", "").toString().trimmed());
    QString learn = set.value("learn", "").toString().trimmed();
    if( learn == "true"){
        ui->checkBox_5->setCheckState(Qt::Checked);
        ui->passwordText_5->setText(set.value("password", "").toString().trimmed());
    } else {
        ui->checkBox_5->setCheckState(Qt::Unchecked);
        ui->passwordText_5->setText("");
    }
    QPalette pal;
    pal.setColor(QPalette::WindowText, Qt::white);
    ui->label_15->setPalette(pal);

    pal.setColor(QPalette::ButtonText, Qt::white);
    ui->login_btn_4->setPalette(pal);
    ui->cancel_btn_4->setPalette(pal);
    set.endGroup();
    ui->passwordText_5->setEchoMode(QLineEdit::Password);
    connect(ui->userNoText_5, SIGNAL(returnPressed()),ui->login_btn_4 , SIGNAL(clicked()), Qt::UniqueConnection);
    connect(ui->passwordText_5, SIGNAL(returnPressed()),ui->login_btn_4, SIGNAL(clicked()), Qt::UniqueConnection);
    DBUtil::opendatabase();
}


loginDialog::~loginDialog()
{
    delete ui;
}

void loginDialog::on_login_btn_4_clicked()
{
    QString userNo=(ui->userNoText_5->text());
    if( QString(userNo).isEmpty()){
        return;
    }
    QString password=(ui->passwordText_5->text());
    if( QString(password).isEmpty()){
        return;
    }
    QString where=" where No='"+userNo+"' and password='"+password+"'";
    QList<QStringList> list=QScanGlobal::instance.selectAllUser(where);
    if(list.count()>0)
    {
        QSettings set(QApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
        set.beginGroup("userinfo");
        set.setValue("userno", userNo);
        if (ui->checkBox_5->checkState() == Qt::Checked)
        {
            set.setValue("learn", "true");
            set.setValue("password", password);
        } else {
            set.setValue("learn", "false");
            set.setValue("password", "");
        }
        set.endGroup();
        MainWindow* mainwindow=new MainWindow();
        this->close();
        mainwindow->showFullScreen();
        mainwindow->show();
    } else{
        QMessageBox::information(this,"消息提示","用户不存在或密码错误!");
        return;
    }
}

void loginDialog::on_cancel_btn_4_clicked()
{
    QApplication* app;
    app->quit();
}

void loginDialog::on_checkBox_5_clicked()
{
    QSettings set(QApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
    set.beginGroup("userinfo");
    if (ui->checkBox_5->checkState() == Qt::Checked)
    {
        set.setValue("learn", "true");
    } else {
        set.setValue("learn", "false");
    }
    set.endGroup();
    return;
}


