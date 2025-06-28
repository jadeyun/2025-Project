#ifndef TIMERWIDGET_H
#define TIMERWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>

class QLabel;
class QPushButton;
class QComboBox;
class QLineEdit;

class TimerWidget : public QWidget {
    Q_OBJECT

public:
    TimerWidget(QWidget *parent = nullptr);

signals:
    void timerFinished(int duration); // Emitted when timer reaches zero, duration is time spent
    void timerPaused(int duration);   // Emitted when timer is paused, duration is time spent

private slots:
    void updateCountdown();
    void startTimer();
    void pauseTimer();
    void resetTimer();
    void modeChanged(int index);

private:
    void setupUI();
    void updateDisplay();
    void playAlarm();

    QTimer *countdownTimer;
    int remainingSeconds;
    int initialSeconds;
    int timeSpent;                    // Total time spent in seconds
    qint64 startTimestamp;            // Timestamp when timer was last started

    QLabel *timeDisplay;
    QComboBox *modeSelector;
    QLineEdit *customInput;
    QPushButton *startButton;
    QPushButton *pauseButton;
    QPushButton *resetButton;

    QMediaPlayer *player;
    QAudioOutput *audioOutput;

    bool isRunning;
};

#endif // TIMERWIDGET_H
