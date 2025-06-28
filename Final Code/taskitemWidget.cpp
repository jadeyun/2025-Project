#include "taskitemWidget.h"

TaskItemWidget::TaskItemWidget(const QString &text, QWidget *parent)
    : QWidget(parent)
{
    checkbox = new QCheckBox(this);
    label = new QLabel(text, this);
    label->setStyleSheet("font-size: 14px; color: navy");


    // 主水平布局
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(40, 10, 40, 10); // 设置边距
    layout->setSpacing(10);                    // 设置控件间距

    // 添加一个弹簧，使内容整体居中
    layout->addWidget(label);                  // 文本标签（左对齐）
    layout->addStretch();                      // 右侧弹簧
    layout->addWidget(checkbox);               // 复选框

    // 设置标签对齐方式（左对齐 + 垂直居中）
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

}

bool TaskItemWidget::isChecked() const {
    return checkbox->isChecked();
}

QString TaskItemWidget::getText() const {
    return label->text();
}

void TaskItemWidget::setChecked(bool checked) {
    checkbox->setChecked(checked);
}


