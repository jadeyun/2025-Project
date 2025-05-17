#include "taskitemWidget.h"

TaskItemWidget::TaskItemWidget(const QString &text, QWidget *parent)
    : QWidget(parent)
{
    checkbox = new QCheckBox(this);
    label = new QLabel(text, this);
    label->setStyleSheet("font-size: 14px; color: navy");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5);
    layout->setSpacing(10);
    layout->addWidget(label);
    layout->addWidget(checkbox);

}
