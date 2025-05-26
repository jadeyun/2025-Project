#include "mainwindow.h"
#include "welcomepage.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;

    WelcomePage welcome;
    if (welcome.exec() == QDialog::Accepted) {
        w.show();
        return a.exec();


    }
}
