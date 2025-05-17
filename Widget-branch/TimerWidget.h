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
