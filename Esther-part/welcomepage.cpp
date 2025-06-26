#include "welcomepage.h"
#include "ui_welcomepage.h"

#include <QSlider>
#include <QVBoxLayout>
#include <QPushButton>


WelcomePage::WelcomePage(QWidget *parent)
    : QDialog(parent), ui(new Ui::WelcomePage)
{
    ui->setupUi(this);
    setWindowTitle("Welcome");

    ui->startButton->setStyleSheet(R"(
        QPushButton {
            background-color: #0B1A36;
            color: beige;
            border-radius: 15px;
        }
        QPushButton:hover {
            background-color: #2F4470;
        }
)");


}

void WelcomePage::on_startButton_clicked() {
    accept();
}



WelcomePage::~WelcomePage()
{
    delete ui;
}

