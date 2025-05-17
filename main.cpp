#include "mainwindow.h"

#include <QApplication>
#include "TimerWidget.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    TimerWidget timer;
    timer.resize(300, 150);
    timer.setWindowTitle("多学习模式");
    timer.show();

    return a.exec();
}
