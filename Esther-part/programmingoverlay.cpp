#include "programmingoverlay.h"
#include "ui_programmingoverlay.h"

programmingoverlay::programmingoverlay(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::programmingoverlay)
{
    ui->setupUi(this);
    connect(ui->backButton, &QPushButton::clicked, this, &QWidget::hide);

}

programmingoverlay::~programmingoverlay()
{
    delete ui;
}
