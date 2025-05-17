#include "englishoverlay.h"
#include "ui_englishoverlay.h"
#include "taskitemWidget.h"


englishoverlay::englishoverlay(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::englishoverlay)
{
    ui->setupUi(this);
    connect(ui->backButton, &QPushButton::clicked, this, &QWidget::hide);

    QStringList tasks = {"Learn 5 new words", "Review grammar", "Watch an English video"};
    for (const QString &taskText : tasks) {
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        item->setSizeHint(QSize(0, 40)); // Adjust height as needed
        TaskItemWidget *taskWidget = new TaskItemWidget(taskText);
        ui->listWidget->setItemWidget(item, taskWidget);
    }


}



englishoverlay::~englishoverlay()
{
    delete ui;
}
