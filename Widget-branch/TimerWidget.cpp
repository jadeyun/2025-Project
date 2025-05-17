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

TimerWidget::TimerWidget(QWidget *parent)
    : QWidget(parent), isRunning(false), remainingSeconds(0), initialSeconds(0) {

    countdownTimer = new QTimer(this);
    countdownTimer->setInterval(1000);
    connect(countdownTimer, &QTimer::timeout, this, &TimerWidget::updateCountdown);

    // 媒体播放初始化
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
        if (modeSelector->currentIndex() == 3) { // 仅当为自定义模式
            bool ok;
            int customMinutes = text.toInt(&ok);
            if (ok && customMinutes > 0) {
                initialSeconds = customMinutes * 60;
                remainingSeconds = initialSeconds;
                updateDisplay();
            }
        }
    });
    customLayout->addWidget(customLabel);
    customLayout->addWidget(customInput);
    mainLayout->addLayout(customLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    startButton = new QPushButton("开始", this);
    pauseButton = new QPushButton("暂停", this);
    resetButton = new QPushButton("重置", this);
    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(pauseButton);
    buttonLayout->addWidget(resetButton);
    mainLayout->addLayout(buttonLayout);

    connect(startButton, &QPushButton::clicked, this, &TimerWidget::startTimer);
    connect(pauseButton, &QPushButton::clicked, this, &TimerWidget::pauseTimer);
    connect(resetButton, &QPushButton::clicked, this, &TimerWidget::resetTimer);

    modeChanged(0); // 初始化模式
}

void TimerWidget::modeChanged(int index) {
    if (index == 3) {
        // 自定义时长启用输入框
        customInput->setEnabled(true);
        bool ok;
        int customMinutes = customInput->text().toInt(&ok);
        if (ok && customMinutes > 0) {
            initialSeconds = customMinutes * 60;
        } else {
            initialSeconds = 25 * 60; // 默认自定义为 25 分钟
        }
    } else {
        // 预设模式禁用输入框
        customInput->setEnabled(false);
        switch (index) {
        case 0: initialSeconds = 25 * 60; break;
        case 1: initialSeconds = 5 * 60; break;
        case 2: initialSeconds = 15 * 60; break;
        }
    }

    remainingSeconds = initialSeconds;
    updateDisplay();
}

void TimerWidget::startTimer() {
    int index = modeSelector->currentIndex();
    if (index == 3) { // 自定义模式
        bool ok;
        int customMinutes = customInput->text().toInt(&ok);
        if (ok && customMinutes > 0) {
            remainingSeconds = customMinutes * 60;
            initialSeconds = remainingSeconds;
        } else {
            remainingSeconds = 25 * 60; // fallback 默认 25 分钟
            initialSeconds = remainingSeconds;
        }
    } else {
        remainingSeconds = initialSeconds; // 用预设时间
    }

    countdownTimer->start();
    isRunning = true;
    updateDisplay();
}

void TimerWidget::pauseTimer() {
    if (isRunning) {
        countdownTimer->stop();
        isRunning = false;
    } else {
        countdownTimer->start();
        isRunning = true;
    }
}

void TimerWidget::resetTimer() {
    countdownTimer->stop();
    remainingSeconds = initialSeconds;
    isRunning = false;
    updateDisplay();
}

void TimerWidget::updateCountdown() {
    if (remainingSeconds > 0) {
        remainingSeconds--;
        updateDisplay();
    } else {
        countdownTimer->stop();
        playAlarm();
        isRunning = false;
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

    player->stop(); // 保证从头播放
    player->play();

    AlarmDialog *dialog = new AlarmDialog(this);

    // 铃声最多播放 60 秒（防止无限响）
    QTimer::singleShot(60000, this, [=]() {
        if (player->isPlaying()) {
            player->stop();
            if (dialog->isVisible()) dialog->accept();
        }
    });

    // 如果用户手动关闭弹窗
    connect(dialog, &AlarmDialog::stopAlarm, this, [=]() {
        if (player->isPlaying()) player->stop();
    });

    dialog->show();
}
