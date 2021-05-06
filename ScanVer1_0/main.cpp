#include "logindialog.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    loginDialog* win = new loginDialog();
    win->show();
    return a.exec();
}
