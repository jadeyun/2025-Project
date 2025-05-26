#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

#include "mainwindow.h"

#include <QDialog>

namespace Ui {
class WelcomePage;
}

// class MainWindow;


// class WelcomePage : public QWidget
// {
//     Q_OBJECT

// public:
//     explicit WelcomePage(MainWindow *parentWindow, QWidget *parent = nullptr);
//     ~WelcomePage();

// private:
//     Ui::WelcomePage *ui;
//     MainWindow *mainWindow;
//     WelcomePage *welcomepage;

// private slots:
//     void on_startButton_clicked();


// };

#include <QDialog>

class WelcomePage : public QDialog
{
    Q_OBJECT

public:
    explicit WelcomePage(QWidget *parent = nullptr);
    ~WelcomePage();

private slots:
    void on_startButton_clicked();

private:
    Ui::WelcomePage *ui;
};


#endif // WELCOMEPAGE_H
