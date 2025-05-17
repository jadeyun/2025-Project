#ifndef ALARMDIALOG_H
#define ALARMDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>

class AlarmDialog : public QDialog {
    Q_OBJECT

public:
    explicit AlarmDialog(QWidget *parent = nullptr);

signals:
    void stopAlarm(); // 通知主窗口关闭铃声

private:
    QLabel *messageLabel;
    QPushButton *stopButton;
    QTimer *autoCloseTimer;
};

#endif // ALARMDIALOG_H
