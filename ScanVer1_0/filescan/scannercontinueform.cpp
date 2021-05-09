#include "scannercontinueform.h"
#include "ui_scannercontinueform.h"

ScannerContinueForm::ScannerContinueForm(QWidget* parent, int itime) :
    QDialog(parent),
    ui(new Ui::ScannerContinueForm)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint & ~Qt::WindowContextHelpButtonHint);
    setWindowState(Qt::WindowActive);
    m_inttime = itime;
    ui->label_time->setText(QString::number(m_inttime) + "秒");
    connect(&m_qtimer, SIGNAL(timeout()), this, SLOT(dotime()));
    m_qtimer.start(1000);
}

void ScannerContinueForm::dotime()
{
    m_inttime--;
    ui->label_time->setText(QString::number(m_inttime) + "秒");
    if (m_inttime < 0)
    {
        m_qtimer.stop();
        on_pushButton_ok_clicked();
    }
}

ScannerContinueForm::~ScannerContinueForm()
{
    delete ui;
}

void ScannerContinueForm::on_pushButton_ok_clicked()
{
    this->accept();
}

void ScannerContinueForm::on_pushButton_cancel_clicked()
{
    this->reject();
}

bool ScannerContinueForm::DoShow(QWidget* parent, int itime)
{
    ScannerContinueForm* win = new ScannerContinueForm(parent, itime);
    win->setGeometry(qApp->desktop()->width() - win->width() - 20, qApp->desktop()->height() / 2 - win->width(), win->width(), win->height());
    int dialogCode = win->exec();
    delete win;
    return dialogCode == QDialog::DialogCode::Accepted;
}

void ScannerContinueForm::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return || event->key() == Qt::Key_F7)
    {
        this->accept();
    }
}

