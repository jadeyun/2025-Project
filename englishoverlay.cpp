#include "englishoverlay.h"
#include "ui_englishoverlay.h"
#include "taskitemWidget.h"

#include <QMessageBox>
#include <QFile>
#include <QStandardPaths>

englishoverlay::englishoverlay(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::englishoverlay)
{
    ui->setupUi(this);

    // back button
    connect(ui->backButton, &QPushButton::clicked, this, [this]() {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Confirm");
        msgBox.setText("Do you want to quit this page?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        msgBox.setStyleSheet(
            "QLabel{ color: red; font-weight: bold; }"
            "QPushButton{color: navy;}"
        );


        // 这里记录未完成的计划
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes) {
            QString planName = ui->plan_name->text();

            QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/unfinished_plans.txt";

            // if要暂时清空文件
            // QFile file(path);
            // if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            //     file.close(); // 直接关闭即可，内容已被清空
            //     QMessageBox::information(this, "Success", "All saved plans have been cleared.");
            // } else {
            //     QMessageBox::warning(this, "Error", "Failed to clear saved plans.");
            // }

            // 收集任务及状态
            QList<QString> taskLines;
            bool allChecked = true;

            for (int i = 0; i < ui->listWidget->count(); ++i) {
                QListWidgetItem *item = ui->listWidget->item(i);
                TaskItemWidget *taskWidget = qobject_cast<TaskItemWidget *>(ui->listWidget->itemWidget(item));
                if (taskWidget) {
                    QString text = taskWidget->getText();
                    int status = taskWidget->isChecked() ? 1 : 0;
                    taskLines << text + "|" + QString::number(status);
                    if (status == 0) allChecked = false; // 有一个没勾就不是全勾
                }
            }

            // 读取原有文件内容
            QStringList lines;
            QFile file(path);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                while (!in.atEnd()) {
                    lines << in.readLine();
                }
                file.close();
            }

            // 移除已存在的同名计划
            QStringList updatedLines;
            bool insidePlan = false;
            for (const QString &line : lines) {
                if (line == "[[" + planName + "]]") {
                    insidePlan = true;
                    continue;
                }
                if (insidePlan) {
                    if (line.startsWith("[[") && line.endsWith("]]")) {
                        insidePlan = false;
                        updatedLines << line;  // 新计划开始，加进去
                    }
                    continue;  // 跳过旧的该计划
                }
                updatedLines << line;
            }

            // 添加新版本的计划，无论是否完成都记录，并加状态标记
            updatedLines << "[[" + planName + "]]";
            updatedLines << (allChecked ? "[FINISHED]" : "[UNFINISHED]");
            updatedLines << taskLines;
            updatedLines << ""; // 空行分隔计划


            // 写回文件
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                for (const QString &line : updatedLines)
                    out << line << "\n";
                file.close();
            }

            this->hide();
        }


    });


}


void englishoverlay::setTasks(const QStringList &tasks) {

    ui->listWidget->clear();

    for (const QString &taskText : tasks) {
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        item->setSizeHint(QSize(0, 40)); // Adjust height as needed
        TaskItemWidget *taskWidget = new TaskItemWidget(taskText);
        ui->listWidget->setItemWidget(item, taskWidget);
    }
}

void englishoverlay::setName(const QString &name) {
    ui->plan_name->setText(name);
}

void englishoverlay::setTasksWithState(const QList<QPair<QString, bool>> &taskList) {
    ui->listWidget->clear();

    for (const auto &pair : taskList) {
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        item->setSizeHint(QSize(0, 40));
        TaskItemWidget *taskWidget = new TaskItemWidget(pair.first); // task text
        taskWidget->setChecked(pair.second); // task checkbox state
        ui->listWidget->setItemWidget(item, taskWidget);
    }
}




englishoverlay::~englishoverlay()
{
    delete ui;
}
