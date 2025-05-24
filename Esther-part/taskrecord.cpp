#include "taskrecord.h"
#include <QStandardPaths>

taskRecord::taskRecord(QObject *parent) : QObject(parent) {
    logFilePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/task_log.txt";
    qDebug() << "Log file path set to:" << logFilePath;
}



void taskRecord::logTask(const QString &planName)
{

    QFile file(logFilePath);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << "Cannot open log file";
        return;
    }


    // 读取现有内容
    QStringList lines;
    QTextStream in(&file);
    while (!in.atEnd()) {
        lines << in.readLine();
    }
    // file.resize(0); // 清空文件

    // 准备新条目
    QDateTime now = QDateTime::currentDateTime();
    QString dateStr = now.toString("yyyy-MM-dd");
    QString timeStr = now.toString("hh:mm:ss");
    QString newEntry = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|1").arg(planName, dateStr, now.toString("yyyy"), now.toString("MM"), now.toString("dd"),
                                                       timeStr, now.toString("hh"), now.toString("mm"), now.toString("ss"));

    // 查找是否已有今日记录
    bool found = false;
    for (int i = 0; i < lines.size(); ++i) {
        if (i == 0) continue;  // 跳过表头
        QStringList parts = lines[i].split('|');
        if (parts.size() == 10 && parts[0] == planName && parts[1] == dateStr) {
            int times = parts[9].toInt() + 1;
            lines[i] = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10")
                           .arg(planName, dateStr, parts[2], parts[3], parts[4],
                                parts[5], parts[6], parts[7], parts[8])
                           .arg(times);
            found = true;
            break;
        }
    }


    if (!found) {
        lines.append(newEntry);
    }

    // 写回文件
    QTextStream out(&file);
    foreach (const QString &line, lines) {  // Qt 专用宏，自动优化
        out << line << "\n";
    }
    file.close();
}



// void taskRecord::loadData() {

// }
