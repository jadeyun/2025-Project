#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class FormDialog : public QDialog {
    Q_OBJECT
public:
   explicit FormDialog(QWidget *parent = nullptr);


signals:
    void planConfirmed(const QString &planName , const QStringList &tasks);  // 发射计划名

private:
    QLineEdit *goalEdit;  // 存储“学习目标”输入框

};
