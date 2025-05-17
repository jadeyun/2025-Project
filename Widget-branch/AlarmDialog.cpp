#include "AlarmDialog.h"
#include <QGraphicsBlurEffect>
#include <QApplication>
#include <QScreen>

AlarmDialog::AlarmDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("时间到");
    setModal(false);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(300, 200);

    // 整体主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // ---------- 背景层 ----------
    QWidget *blurBackground = new QWidget(this);
    blurBackground->setStyleSheet("background-color: rgba(255, 255, 255, 180); border-radius: 15px;");
    blurBackground->setGeometry(this->rect());

    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect();
    blur->setBlurRadius(20);
    blurBackground->setGraphicsEffect(blur);

    // ---------- 前景内容 ----------
    QWidget *contentWidget = new QWidget(this);
    contentWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    contentWidget->setStyleSheet("background-color: transparent;");
    contentWidget->setFixedSize(300, 200);

    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setAlignment(Qt::AlignCenter);

    messageLabel = new QLabel("时间到！", this);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setStyleSheet("font-size: 28px; color: #e53935;");

    stopButton = new QPushButton("关闭铃声", this);
    stopButton->setStyleSheet(R"(
        QPushButton {
            font-size: 18px;
            padding: 8px 16px;
            background-color: #2196F3;
            color: white;
            border: none;
            border-radius: 10px;
        }
        QPushButton:hover {
            background-color: #1976D2;
        }
    )");

    contentLayout->addStretch();
    contentLayout->addWidget(messageLabel);
    contentLayout->addWidget(stopButton, 0, Qt::AlignCenter);
    contentLayout->addStretch();

    mainLayout->addWidget(contentWidget);

    // 自动关闭计时器（20 秒）
    autoCloseTimer = new QTimer(this);
    autoCloseTimer->setSingleShot(true);
    connect(autoCloseTimer, &QTimer::timeout, this, [=]() {
        emit stopAlarm();
        this->accept();
    });
    autoCloseTimer->start(20000); // 20秒

    connect(stopButton, &QPushButton::clicked, this, [=]() {
        emit stopAlarm();
        this->accept();
    });

    // 居中弹窗
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    move(screenGeometry.center() - rect().center());
}
