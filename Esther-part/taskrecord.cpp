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

    QMap<QString, int> dailyDurations; // Map: date -> total duration in seconds
    QSet<QString> recordedDates;

    for (int i = 0; i < lines.size(); ++i) {
        if (lines[i].startsWith("#")) continue;
        QStringList parts = lines[i].split('|');
        if (parts.size() >= 11) {
            QString date = parts[1];
            int duration = parts[10].toInt(); // seconds
            dailyDurations[date] += duration;
            recordedDates.insert(date);
        }
    }

    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    int todayTotalSeconds = dailyDurations.value(today, 0);
    double todayTotalHours = todayTotalSeconds / 3600.0;

    int streak = 0;
    QDate current = QDate::currentDate();
    while (recordedDates.contains(current.toString("yyyy-MM-dd"))) {
        ++streak;
        current = current.addDays(-1);
    }

    QString summaryLine = QString("#TOTALS|%1|%2|%3")
                              .arg(today)
                              .arg(QString::number(todayTotalHours, 'f', 2)) // e.g. 1.50 hours
                              .arg(streak);

    if (!lines.isEmpty() && lines[0].startsWith("#TOTALS|")) {
        lines[0] = summaryLine;
    } else {
        lines.insert(0, summaryLine);
    }

    file.resize(0);
    QTextStream out(&file);
    for (const QString &line : lines) {
        out << line << "\n";
    }
    file.close();
    // 发射统计更新信号
    QString hoursText = QString::number(todayTotalHours, 'f', 1) + "h";
    emit statsUpdated(hoursText, streak);

    // 更新内部变量
    m_todayHoursText = hoursText;
    m_currentStreak = streak;
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

void taskRecord::refreshStats() {
    QFile file(logFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open log file for reading stats";
        return;
    }

    QTextStream in(&file);
    QString firstLine = in.readLine();
    file.close();

    // 解析第一行的统计信息
    if (firstLine.startsWith("#TOTALS|")) {
        QStringList parts = firstLine.split('|');
        if (parts.size() >= 4) {
            double todayHours = parts[2].toDouble();
            int streak = parts[3].toInt();

            // 格式化时长显示
            m_todayHoursText = QString::number(todayHours, 'f', 1) + "";
            m_currentStreak = streak;

            emit statsUpdated(m_todayHoursText, m_currentStreak);
        }
    } else {
        m_todayHoursText = "0.0";
        m_currentStreak = 0;
        emit statsUpdated(m_todayHoursText, m_currentStreak);
    }
}

QPair<QString, int> taskRecord::getCurrentStats() {
    return QPair<QString, int>(m_todayHoursText, m_currentStreak);
}


