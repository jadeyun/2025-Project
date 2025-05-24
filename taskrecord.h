#ifndef TASKRECORD_H
#define TASKRECORD_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

class taskRecord : public QObject
{
    Q_OBJECT
public:
    explicit taskRecord(QObject *parent = nullptr);
    void logTask(const QString &planName);
    void logPlanWithTasks(const QString &planName, const QStringList &tasks);
    void logTimeSpent(const QString &planName, int duration);
    void updateSummaryStats();

// private:
    QString logFilePath = "task_record.txt"; // 日志文件路径

};

#endif // TASKRECORD_H
