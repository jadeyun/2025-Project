#include "FormDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

FormDialog::FormDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("制定计划");
    setFixedSize(500, 500);
    setStyleSheet("background-color: black; color: white;");

    QVBoxLayout *layout = new QVBoxLayout(this);

    auto createInput = [](const QString &labelText, const QString &placeholder, int height = 40) {
        QWidget *wrapper = new QWidget;
        QVBoxLayout *vbox = new QVBoxLayout(wrapper);
        QLabel *label = new QLabel(labelText);
        QLineEdit *edit = new QLineEdit;
        edit->setPlaceholderText(placeholder);
        edit->setMinimumHeight(height);
        vbox->addWidget(label);
        vbox->addWidget(edit);
        vbox->setContentsMargins(0, 0, 0, 0);
        return qMakePair(wrapper, edit);
    };

    // 创建第一个输入框：学习目标（需要保存指针）
    auto goalPair = createInput("学习目标", "例如：学习剪辑，学习设计");
    goalEdit = goalPair.second;
    layout->addWidget(goalPair.first);

    // 成员变量
    QLineEdit *taskEdit1;
    QLineEdit *taskEdit2;
    QLineEdit *taskEdit3;

    auto taskPair1 = createInput("学习任务1*", "制定一些小目标并逐步完成吧！");
    taskEdit1 = taskPair1.second;
    layout->addWidget(taskPair1.first);

    auto taskPair2 = createInput("学习任务2*", "制定一些小目标并逐步完成吧！");
    taskEdit2 = taskPair2.second;
    layout->addWidget(taskPair2.first);

    auto taskPair3 = createInput("学习任务3*", "制定一些小目标并逐步完成吧！");
    taskEdit3 = taskPair3.second;
    layout->addWidget(taskPair3.first);

    QPushButton *confirmBtn = new QPushButton("确认");
    confirmBtn->setFixedHeight(40);
    layout->addWidget(confirmBtn);

    layout->setSpacing(10);

    // 点击确认时发出信号
    connect(confirmBtn, &QPushButton::clicked, this, [=]() {
        QString planName = goalEdit->text().trimmed();
        if (planName.isEmpty()) {
            QMessageBox::warning(this, "输入错误", "请输入学习目标名称！");
            return;
        }
        QStringList tasks;
        tasks << taskEdit1->text().trimmed()
              << taskEdit2->text().trimmed()
              << taskEdit3->text().trimmed();

        emit planConfirmed(planName, tasks);
        this->accept();
    });

}

