#include "TimerWidget.h"
#include "AlarmDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QCoreApplication>
#include <QUrl>
#include <QDebug>
#include <QDir>
#include <QDateTime>

TimerWidget::TimerWidget(QWidget *parent)
    : QWidget(parent), isRunning(false), remainingSeconds(0), initialSeconds(0), timeSpent(0), startTimestamp(0) {

    countdownTimer = new QTimer(this);
    countdownTimer->setInterval(1000);
    connect(countdownTimer, &QTimer::timeout, this, &TimerWidget::updateCountdown);

    // Initialize media player
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.5);
    QString path = QCoreApplication::applicationDirPath() + "/alarms/defaultalarm.mp3";
    player->setSource(QUrl::fromLocalFile(path));

    setupUI();
}

void TimerWidget::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    timeDisplay = new QLabel("00:00", this);
    timeDisplay->setAlignment(Qt::AlignCenter);
    timeDisplay->setStyleSheet("font-size: 36px;");
    mainLayout->addWidget(timeDisplay);

    modeSelector = new QComboBox(this);
    modeSelector->addItem("番茄钟 (25分钟)");
    modeSelector->addItem("短休息 (5分钟)");
    modeSelector->addItem("长休息 (15分钟)");
    modeSelector->addItem("自定义时长");
    connect(modeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TimerWidget::modeChanged);
    mainLayout->addWidget(modeSelector);

    QHBoxLayout *customLayout = new QHBoxLayout();
    QLabel *customLabel = new QLabel("用户自定义时长（分钟）:");
    customInput = new QLineEdit("25");
    connect(customInput, &QLineEdit::textChanged, this, [=](const QString &text) {
        if (modeSelector->currentIndex() == 3) { // Custom mode
            bool ok;
            int customMinutes = text.toInt(&ok);
            if (ok && customMinutes > 0) {
                initialSeconds = customMinutes * 60;
                if (!isRunning) {
                    remainingSeconds = initialSeconds;
                    updateDisplay();
                }
            }
        }
    });
    customLayout->addWidget(customLabel);
    customLayout->addWidget(customInput);
    mainLayout->addLayout(customLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    startButton = new QPushButton("开始", this);
    pauseButton = new QPushButton("暂停/继续", this);
    resetButton = new QPushButton("重置", this);
    QString buttonStyle = R"(
    QPushButton {
        background-color: black;
        border: 1px solid #aaa;
        border-radius: 5px;
        padding: 6px 12px;
    }
    QPushButton:hover {
        background-color: #d0eaff;
        border: 1px solid #3399ff;
    }
    QPushButton:pressed {
        background-color: #a4d3f5;
    }
)";
    startButton->setStyleSheet(buttonStyle);
    pauseButton->setStyleSheet(buttonStyle);
    resetButton->setStyleSheet(buttonStyle);

    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(pauseButton);
    buttonLayout->addWidget(resetButton);
    mainLayout->addLayout(buttonLayout);

    connect(startButton, &QPushButton::clicked, this, &TimerWidget::startTimer);
    connect(pauseButton, &QPushButton::clicked, this, &TimerWidget::pauseTimer);
    connect(resetButton, &QPushButton::clicked, this, &TimerWidget::resetTimer);

    modeChanged(0); // Initialize mode
}

void TimerWidget::modeChanged(int index) {
    if (index == 3) {
        // Enable custom input
        customInput->setEnabled(true);
        bool ok;
        int customMinutes = customInput->text().toInt(&ok);
        if (ok && customMinutes > 0) {
            initialSeconds = customMinutes * 60;
        } else {
            initialSeconds = 25 * 60; // Default to 25 minutes
        }
    } else {
        // Disable custom input for preset modes
        customInput->setEnabled(false);
        switch (index) {
        case 0: initialSeconds = 25 * 60; break; // Pomodoro
        case 1: initialSeconds = 5 * 60; break;  // Short break
        case 2: initialSeconds = 15 * 60; break; // Long break
        }
    }

    if (!isRunning) {
        remainingSeconds = initialSeconds;
        timeSpent = 0;
        startTimestamp = 0;
        updateDisplay();
    }
}

void TimerWidget::startTimer() {
    int index = modeSelector->currentIndex();
    if (index == 3) { // Custom mode
        bool ok;
        int customMinutes = customInput->text().toInt(&ok);
        if (ok && customMinutes > 0) {
            initialSeconds = customMinutes * 60;
            if (!isRunning) {
                remainingSeconds = initialSeconds;
            }
        } else {
            initialSeconds = 25 * 60; // Fallback to 25 minutes
            if (!isRunning) {
                remainingSeconds = initialSeconds;
            }
        }
    } else if (!isRunning) {
        remainingSeconds = initialSeconds; // Use preset time
    }

    if (!isRunning && remainingSeconds > 0) {
        countdownTimer->start();
        isRunning = true;
        startTimestamp = QDateTime::currentMSecsSinceEpoch();
        updateDisplay();
    }
}

void TimerWidget::pauseTimer() {
    if (isRunning) {
        countdownTimer->stop();
        isRunning = false;
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        int sessionTime = (currentTime - startTimestamp) / 1000; // Time spent in this session (seconds)
        if (sessionTime > 0) {
            timeSpent += sessionTime;
            emit timerPaused(timeSpent);
            timeSpent = 0; // Reset time spent after emitting
            startTimestamp = 0;
        }
    } else if (remainingSeconds > 0) {
        countdownTimer->start();
        isRunning = true;
        startTimestamp = QDateTime::currentMSecsSinceEpoch();
    }
    updateDisplay();
}

void TimerWidget::resetTimer() {
    countdownTimer->stop();
    remainingSeconds = initialSeconds;
    isRunning = false;
    timeSpent = 0;
    startTimestamp = 0;
    updateDisplay();
}

void TimerWidget::updateCountdown() {
    if (remainingSeconds > 0) {
        remainingSeconds--;
        updateDisplay();
    } else {
        countdownTimer->stop();
        isRunning = false;
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        int sessionTime = (currentTime - startTimestamp) / 1000; // Time spent in this session
        timeSpent += sessionTime;
        emit timerFinished(timeSpent);
        timeSpent = 0; // Reset time spent after emitting
        startTimestamp = 0;
        playAlarm();
    }
}

void TimerWidget::updateDisplay() {
    int minutes = remainingSeconds / 60;
    int seconds = remainingSeconds % 60;
    timeDisplay->setText(QString("%1:%2")
                             .arg(minutes, 2, 10, QChar('0'))
                             .arg(seconds, 2, 10, QChar('0')));
}

void TimerWidget::playAlarm() {
    qDebug() << "播放铃声...";

    player->stop(); // Ensure alarm plays from the start
    player->play();

    AlarmDialog *dialog = new AlarmDialog(this);

    // Stop alarm after 60 seconds
    QTimer::singleShot(60000, this, [=]() {
        if (player->isPlaying()) {
            player->stop();
            if (dialog->isVisible()) dialog->accept();
        }
    });

    // Stop alarm if dialog is closed
    connect(dialog, &AlarmDialog::stopAlarm, this, [=]() {
        if (player->isPlaying()) player->stop();
    });

    dialog->show();
}
