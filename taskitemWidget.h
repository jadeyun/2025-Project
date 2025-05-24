#ifndef TASKITEMWIDGET_H
#define TASKITEMWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QStringList>

class TaskItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TaskItemWidget(const QString &text = "", QWidget *parent = nullptr);

    QCheckBox *checkbox;
    QLabel *label;

    bool isChecked() const;
    QString getText() const;

    void setChecked(bool checked);

};

#endif // TASKITEMWIDGET_H

