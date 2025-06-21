#include "historywindow.h"
#include <QFile>
#include <QLabel>
#include <QTextStream>
#include <QStandardPaths>
#include <QDebug>
#include <QHBoxLayout>
#include <QDate>
#include "clickablelabel.h"

HistoryWindow::HistoryWindow(QWidget *parent) : QWidget(parent), overlay(nullptr) {
    setWindowFlags(Qt::Window);
    setWindowTitle("历史记录");
    resize(600, 500);
    this->setStyleSheet("background-color: black; color: white; font-size: 14px;");

    mainLayout = new QVBoxLayout(this);
    loadLog();
}

HistoryWindow::~HistoryWindow() {}

void HistoryWindow::loadLog() {
    // Load the task log
    QString taskLogPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/task_log.txt";
    QFile taskFile(taskLogPath);

    if (!taskFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QLabel *label = new QLabel("无法读取历史记录文件", this);
        QHBoxLayout *hLayout = new QHBoxLayout;
        hLayout->addStretch();
        hLayout->addWidget(label);
        hLayout->addStretch();
        mainLayout->addLayout(hLayout);
        return;
    }

    this->setStyleSheet("background-color: black; color: white; font-size: 14px;");

    // Structure: date -> planName -> {titleDisplay, taskDisplays}
    QMap<QString, QMap<QString, QPair<QString, QList<QString>>>> dateToPlans;
    QTextStream taskIn(&taskFile);
    QString currentPlanName;
    QString currentDate;
    QString currentTime;
    QString currentCount;

    while (!taskIn.atEnd()) {
        QString line = taskIn.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split('|');
        if (parts.size() == 11) { // Updated to handle new 11-field format
            // Main plan entry
            currentPlanName = parts[0];
            currentDate = parts[1];
            currentTime = parts[5];
            currentCount = parts[9];
            QString duration = parts[10]; // New duration field (in seconds)

            // Convert duration to MM:SS format for display
            int totalSeconds = duration.toInt();
            int minutes = totalSeconds / 60;
            int seconds = totalSeconds % 60;
            QString durationDisplay = QString("%1:%2")
                                          .arg(minutes, 2, 10, QChar('0'))
                                          .arg(seconds, 2, 10, QChar('0'));

            QString titleDisplay = QString("🕒 %1    📘 %2    ✏️ 次数: %3    ⏱️ 时长: %4")
                                       .arg(currentTime, -10)
                                       .arg(currentPlanName, -10)
                                       .arg(currentCount, -5)
                                       .arg(durationDisplay, -8);
            if (!dateToPlans[currentDate].contains(currentPlanName)) {
                dateToPlans[currentDate][currentPlanName] = qMakePair(titleDisplay, QList<QString>());
            }
        } else if (line.startsWith("    - ")) {
            // Task entry
            QString task = line.mid(6); // Remove "    - "
            int statusIndex = task.lastIndexOf("(Pending)");
            QString status = (statusIndex != -1) ? "❌" : "✔️";
            if (statusIndex != -1) {
                task = task.left(statusIndex).trimmed(); // Remove status text
            }
            QString taskDisplay = QString("    %1 %2").arg(status, -5).arg(task);
            if (!currentPlanName.isEmpty() && dateToPlans.contains(currentDate)) {
                dateToPlans[currentDate][currentPlanName].second.append(taskDisplay);
            }
        }
    }
    taskFile.close();

    // Load unfinished plans
    QString unfinishedPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/unfinished_plans.txt";
    QFile unfinishedFile(unfinishedPath);

    if (unfinishedFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream unfinishedIn(&unfinishedFile);
        QString currentPlanName;
        QString currentDate = QDate::currentDate().toString("yyyy-MM-dd"); // Use today's date for unfinished tasks

        while (!unfinishedIn.atEnd()) {
            QString line = unfinishedIn.readLine().trimmed();
            if (line.isEmpty()) continue;

            if (line.startsWith("[[") && line.endsWith("]]")) {
                currentPlanName = line.mid(2, line.length() - 4); // Extract plan name
                QString titleDisplay = QString("🕒 %1    📘 %2    ✏️ 次数: %3    ⏱️ 时长: %4")
                                           .arg("N/A", -10)
                                           .arg(currentPlanName, -10)
                                           .arg("0", -5)
                                           .arg("00:00", -8);
                if (!dateToPlans[currentDate].contains(currentPlanName)) {
                    dateToPlans[currentDate][currentPlanName] = qMakePair(titleDisplay, QList<QString>());
                }
                continue;
            }

            QStringList parts = line.split('|');
            if (parts.size() != 2) continue;

            QString taskText = parts[0];
            QString status = parts[1] == "1" ? "✔️" : "❌";
            QString taskDisplay = QString("    %1 %2").arg(status, -5).arg(taskText);
            if (!currentPlanName.isEmpty() && dateToPlans.contains(currentDate)) {
                dateToPlans[currentDate][currentPlanName].second.append(taskDisplay);
            }
        }
        unfinishedFile.close();
    }

    // Display all tasks with sorting by time
    for (auto dateIt = dateToPlans.constBegin(); dateIt != dateToPlans.constEnd(); ++dateIt) {
        QString date = dateIt.key();
        QLabel *dateLabel = new QLabel(QString("📅 %1").arg(date), this);
        dateLabel->setStyleSheet("font-weight: bold; font-size: 18px; margin: 10px;");
        QHBoxLayout *dateLayout = new QHBoxLayout;
        dateLayout->addStretch();
        dateLayout->addWidget(dateLabel);
        dateLayout->addStretch();
        mainLayout->addLayout(dateLayout);

        // Convert inner map to a list for sorting
        QList<QPair<QString, QPair<QString, QList<QString>>>> plansList;
        for (auto planIt = dateIt.value().constBegin(); planIt != dateIt.value().constEnd(); ++planIt) {
            plansList.append(qMakePair(planIt.key(), planIt.value()));
        }
        // Sort by time (extracted from titleDisplay)
        std::sort(plansList.begin(), plansList.end(), [](const QPair<QString, QPair<QString, QList<QString>>> &a,
                                                         const QPair<QString, QPair<QString, QList<QString>>> &b) {
            QString timeA = a.second.first.mid(3, 8); // Extract "HH:MM:SS" from "🕒 HH:MM:SS"
            QString timeB = b.second.first.mid(3, 8);
            // Handle "N/A" case for unfinished plans (place them last)
            if (timeA == "N/A") return false; // timeA is "N/A", should be last
            if (timeB == "N/A") return true;  // timeB is "N/A", should be last
            return timeA > timeB; // Sort in descending order (latest first)
        });

        // Iterate over sorted plans
        for (const auto &plan : plansList) {
            // Display the plan title as clickable
            QString titleDisplay = plan.second.first;
            QString planName = titleDisplay.split("    📘 ")[1].split("    ✏️")[0].trimmed(); // Extract plan name

            ClickableLabel *titleLabel = new ClickableLabel(titleDisplay, this);
            titleLabel->setCursor(Qt::PointingHandCursor);
            titleLabel->setStyleSheet(R"(
            QLabel {
                font-family: monospace;
                font-weight: bold;
                color: white;
            }
            QLabel:hover {
                color: yellow;
                text-decoration: underline;
            }
        )");

            connect(titleLabel, &ClickableLabel::clicked, this, [=]() {
                QList<QPair<QString, bool>> taskPairs;
                for (const QString &taskDisplay : plan.second.second) {
                    QString cleanTask = taskDisplay.trimmed();
                    bool isChecked = false;

                    if (cleanTask.startsWith("✔️")) {
                        isChecked = true;
                        cleanTask = cleanTask.mid(2).trimmed();
                    } else if (cleanTask.startsWith("❌")) {
                        isChecked = false;
                        cleanTask = cleanTask.mid(2).trimmed();
                    }

                    taskPairs.append({cleanTask, isChecked});
                }

                emit planSelected(planName, taskPairs);
                this->close();
            });



            QHBoxLayout *titleLayout = new QHBoxLayout;
            titleLayout->addStretch();
            titleLayout->addWidget(titleLabel);
            titleLayout->addStretch();
            mainLayout->addLayout(titleLayout);

            // Display the tasks under this plan
            for (const QString &taskDisplay : plan.second.second) {
                QLabel *taskLabel = new QLabel(taskDisplay, this);
                taskLabel->setStyleSheet("font-family: monospace;");
                QHBoxLayout *taskLayout = new QHBoxLayout;
                taskLayout->addStretch();
                taskLayout->addWidget(taskLabel);
                taskLayout->addStretch();
                mainLayout->addLayout(taskLayout);
            }
        }
    }

    mainLayout->addStretch();
}
