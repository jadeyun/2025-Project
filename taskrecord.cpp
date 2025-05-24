#include "taskrecord.h"
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

taskRecord::taskRecord(QObject *parent) : QObject(parent) {
    logFilePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/task_log.txt";
    qDebug() << "Log file path set to:" << logFilePath;
}

void taskRecord::updateSummaryStats() {
    QFile file(logFilePath);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << "Cannot open log file for stats update";
        return;
    }

    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
        lines << in.readLine();
    }

    QMap<QString, int> dailyDurations; // Map: date -> total duration
    QSet<QString> recordedDates;

    for (int i = 0; i < lines.size(); ++i) {
        if (lines[i].startsWith("#")) continue;
        QStringList parts = lines[i].split('|');
        if (parts.size() >= 11) {
            QString date = parts[1];
            int duration = parts[10].toInt();
            dailyDurations[date] += duration;
            recordedDates.insert(date);
        }
    }

    // 计算今天总耗时
    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    int todayTotal = dailyDurations.value(today, 0);

    // 计算连续天数
    int streak = 0;
    QDate current = QDate::currentDate();
    while (recordedDates.contains(current.toString("yyyy-MM-dd"))) {
        ++streak;
        current = current.addDays(-1);
    }

    // 构造 summary 行
    QString summaryLine = QString("#TOTALS|%1|%2|%3").arg(today).arg(todayTotal).arg(streak);

    // 更新或插入 summary 行
    if (!lines.isEmpty() && lines[0].startsWith("#TOTALS|")) {
        lines[0] = summaryLine;
    } else {
        lines.insert(0, summaryLine);
    }

    // 重写文件
    file.resize(0);
    QTextStream out(&file);
    for (const QString &line : lines) {
        out << line << "\n";
    }
    file.close();
}


void taskRecord::logTask(const QString &planName) {
    QFile file(logFilePath);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << "Cannot open log file";
        return;
    }

    // Read existing content
    QStringList lines;
    QTextStream in(&file);
    while (!in.atEnd()) {
        lines << in.readLine();
    }
    file.resize(0); // Clear file

    // Prepare new entry
    QDateTime now = QDateTime::currentDateTime();
    QString dateStr = now.toString("yyyy-MM-dd");
    QString timeStr = now.toString("hh:mm:ss");
    QString newEntry = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|1|0")
                           .arg(planName, dateStr, now.toString("yyyy"), now.toString("MM"), now.toString("dd"),
                                timeStr, now.toString("hh"), now.toString("mm"), now.toString("ss"));
    // Last field '0' is initial duration in seconds

    // Check for existing record for today
    bool found = false;
    for (int i = 0; i < lines.size(); ++i) {
        if (i == 0) continue; // Skip header
        QStringList parts = lines[i].split('|');
        if (parts.size() >= 10 && parts[0] == planName && parts[1] == dateStr) {
            int times = parts[9].toInt() + 1;
            int duration = parts.size() > 10 ? parts[10].toInt() : 0; // Preserve existing duration
            lines[i] = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11")
                           .arg(planName, dateStr, now.toString("yyyy"), now.toString("MM"), now.toString("dd"),
                                timeStr, now.toString("hh"), now.toString("mm"), now.toString("ss"))
                           .arg(times).arg(duration);
            found = true;
            break;
        }
    }

    if (!found) {
        lines.append(newEntry);
    }

    // Write back to file
    QTextStream out(&file);
    foreach (const QString &line, lines) {
        out << line << "\n";
    }
    file.close();
    updateSummaryStats();

}

void taskRecord::logPlanWithTasks(const QString &planName, const QStringList &tasks) {
    QFile file(logFilePath);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << "Cannot open log file";
        return;
    }

    // Read existing content
    QStringList lines;
    QTextStream in(&file);
    while (!in.atEnd()) {
        lines << in.readLine();
    }
    file.resize(0); // Clear file

    // Prepare new entry
    QDateTime now = QDateTime::currentDateTime();
    QString dateStr = now.toString("yyyy-MM-dd");
    QString timeStr = now.toString("hh:mm:ss");
    QString newEntry = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|1|0")
                           .arg(planName, dateStr, now.toString("yyyy"), now.toString("MM"), now.toString("dd"),
                                timeStr, now.toString("hh"), now.toString("mm"), now.toString("ss"));
    // Last field '0' is initial duration in seconds

    // Check for existing record for today
    bool found = false;
    int insertIndex = lines.size();
    for (int i = 0; i < lines.size(); ++i) {
        if (i == 0) continue; // Skip header
        QStringList parts = lines[i].split('|');
        if (parts.size() >= 10 && parts[0] == planName && parts[1] == dateStr) {
            int times = parts[9].toInt() + 1;
            int duration = parts.size() > 10 ? parts[10].toInt() : 0; // Preserve existing duration
            lines[i] = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11")
                           .arg(planName, dateStr, now.toString("yyyy"), now.toString("MM"), now.toString("dd"),
                                timeStr, now.toString("hh"), now.toString("mm"), now.toString("ss"))
                           .arg(times).arg(duration);
            found = true;
            insertIndex = i + 1;
            break;
        }
    }

    if (!found) {
        lines.append(newEntry);
        insertIndex = lines.size() - 1;
    }

    // Add task details
    for (const QString &task : tasks) {
        if (!task.isEmpty()) {
            lines.insert(insertIndex++, QString("    - %1 (Pending)|%2").arg(task, dateStr));
        }
    }

    // Write back to file
    QTextStream out(&file);
    foreach (const QString &line, lines) {
        out << line << "\n";
    }
    file.close();
    updateSummaryStats();

}

void taskRecord::logTimeSpent(const QString &planName, int duration) {
    QFile file(logFilePath);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << "Cannot open log file";
        return;
    }

    // Read existing content
    QStringList lines;
    QTextStream in(&file);
    while (!in.atEnd()) {
        lines << in.readLine();
    }
    file.resize(0); // Clear file

    // Prepare to update duration
    QDateTime now = QDateTime::currentDateTime();
    QString dateStr = now.toString("yyyy-MM-dd");
    bool found = false;

    for (int i = 0; i < lines.size(); ++i) {
        if (i == 0) continue; // Skip header
        QStringList parts = lines[i].split('|');
        if (parts.size() >= 10 && parts[0] == planName && parts[1] == dateStr) {
            int currentDuration = parts.size() > 10 ? parts[10].toInt() : 0;
            currentDuration += duration; // Add new duration
            lines[i] = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11")
                           .arg(parts[0], parts[1], parts[2], parts[3], parts[4],
                                parts[5], parts[6], parts[7], parts[8], parts[9])
                           .arg(currentDuration);
            found = true;
            break;
        }
    }

    // If no record exists, create a new one with the duration
    if (!found) {
        QString timeStr = now.toString("hh:mm:ss");
        QString newEntry = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|1|%10")
                               .arg(planName, dateStr, now.toString("yyyy"), now.toString("MM"), now.toString("dd"),
                                    timeStr, now.toString("hh"), now.toString("mm"), now.toString("ss"))
                               .arg(duration);
        lines.append(newEntry);
    }

    // Write back to file
    QTextStream out(&file);
    foreach (const QString &line, lines) {
        out << line << "\n";
    }
    file.close();
    updateSummaryStats();
}


