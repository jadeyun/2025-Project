#include "timetable.h"
#include "ui_timetable.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QTableWidgetItem>
#include <QDebug>
#include <QTimer>
#include <QDate>

struct ClassEvent {
    QString name;
    QTime time;
    int dayOfWeek;
};
QList<ClassEvent> classEvents;

Timetable::Timetable(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Timetable)
{
    this->setFixedSize(1200, 600);
    ui->setupUi(this);
    ui->gridLayout->setContentsMargins(20, 20, 20, 20);
    QString savedFile = "saved_timetable.csv";
    if (QFile::exists(savedFile)) {
        loadCSVFile(savedFile);
    }
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/icons/book.png"));
    trayIcon->show();
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Timetable::checkForClassNotification);
    timer->start(60000);
}

Timetable::~Timetable()
{
    delete ui;
}

void Timetable::on_loadFileButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Timetable CSV", "", "CSV Files (*.csv)");
    if (!filename.isEmpty()) {
        QFile::remove("saved_timetable.csv");
        QFile::copy(filename, "saved_timetable.csv");
        loadCSVFile("saved_timetable.csv");
    }
}

void Timetable::loadCSVFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file.";
        return;
    }
    QTextStream stream(&file);
    ui->tableTimetable->verticalHeader()->setVisible(false);
    ui->tableTimetable->horizontalHeader()->setVisible(false);
    ui->tableTimetable->clearContents();
    ui->tableTimetable->setRowCount(0);
    ui->tableTimetable->setColumnCount(7);
    QStringList days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    ui->tableTimetable->setHorizontalHeaderLabels(days);
    ui->tableTimetable->horizontalHeader()->setVisible(true);
    ui->tableTimetable->setRowCount(12);
    QStringList periods {"1st (08:00)", "2nd (09:00)", "3rd (10:10)", "4th (11:10)", "5th (13:00)", "6th (14:00)", "7th (15:10)", "8th (16:10)", "9th (17:10)", "10th (18:40)", "11th (19:40)", "12th (20:40)"};
    ui->tableTimetable->setVerticalHeaderLabels(periods);
    ui->tableTimetable->verticalHeader()->setVisible(true);
    ui->tableTimetable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableTimetable->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableTimetable->setFocusPolicy(Qt::NoFocus);
    ui->tableTimetable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    classEvents.clear();
    int row = 0;
    while (row < 12) {
        QStringList values;
        if (!stream.atEnd()) {
            QString line = stream.readLine();
            values = line.split(',');
        }
        for (int col = 0; col < 7; col++) {
            QString text = "";
            if (!values.empty()) {
                text = values[col];
            }
            QTableWidgetItem* item = new QTableWidgetItem(text);
            item->setTextAlignment(Qt::AlignCenter);
            if (text != "") {
                item->setBackground(QBrush(QColor("#e1f5fe")));
                ClassEvent e;
                e.name = text;
                e.dayOfWeek = col + 1;
                e.time = QTime::fromString(periods[row].mid(periods[row].indexOf('(') + 1, 5), "hh:mm");
                classEvents.append(e);
            }
            else {
                item->setBackground(QBrush(QColor(Qt::white)));
            }
            ui->tableTimetable->setItem(row, col, item);
        }
        row++;
    }
    ui->tableTimetable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableTimetable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableTimetable->setShowGrid(true);
    applyStyle(0);
    file.close();
}

void Timetable::on_comboStyle_currentIndexChanged(int index)
{
    applyStyle(index);
}

void Timetable::applyStyle(int index)
{
    QString style;
    switch (index){
    case 0:
        style = R"(
        QTableWidget {
            background-color: #f5faff;
            gridline-color: white;
            font-size: 14px;
            font-family: "Helvetica";
        }
        QHeaderView::section {
            background-color: #90caf9;
            font-weight: bold;
            font-size: 14px;
            font-family: "Helvetica";
            border: 1px solid #ffffff;
            border-radius: 6px;
            padding: 6px;
            qproperty-alignment: 'AlignCenter';
        }
        QTableCornerButton::section {
            background-color: #90caf9;
            border: 1px solid #ffffff;
            border-radius: 6px;
            padding: 6px;
        }
        QTableWidget::item:selected {
            background-color: #c5e1a5;
        }
    )";
        break;
    case 1:
        style = R"(
        QTableWidget {
            background-color: #fce4ec;
            gridline-color: white;
            font-size: 14px;
            font-family: "Helvetica";
            color: #880e4f;
        }
        QHeaderView::section {
            background-color: #f38fb1;
            font-weight: bold;
            font-size: 14px;
            font-family: "Helvetica";
            border: 1px solid #ffffff;
            border-radius: 6px;
            padding: 6px;
            qproperty-alignment: 'AlignCenter';
            color: #880e4f;
        }
        QTableCornerButton::section {
            background-color: #f38fb1;
            border: 1px solid #ffffff;
            border-radius: 6px;
            padding: 6px;
        }
        QTableWidget::item:selected {
            background-color: #f8bbd0;
        }
    )";
        break;
    case 2:
        style = "QTableWidget::item {background-color: #d3f8e2; }";
        break;
    }
    ui->tableTimetable->setStyleSheet(style);

    ui->comboStyle->setStyleSheet(R"(
        QComboBox {
            background-color: #e3f2fd;
            border: 1px solid #90caf9;
            padding: 5px;
            font-size: 13px;
            border-radius: 5px;
            font-family: "Helvetica";
        }
        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 20px;
            border-left: 1px solid #90caf9;
        }
        QComboBox QAbstractItemView {
            background-color: #ffffff;
            selection-background-color: #bbdefb;
        }
    )");

    ui->loadFileButton->setStyleSheet(R"(
        QPushButton {
            background-color: #e3f2fd;
            border: 1px solid #90caf9;
            padding: 8px 16px;
            font-size: 14px;
            border-radius: 6px;
            font-family: "Helvetica";
        }
        QPushButton::hover {
            background-color: #90caf9;
        }
        QPushButton::pressed {
            background-color: #4db6ac;
        }
    )");
}

void Timetable::checkForClassNotification() {
    QDateTime current = QDateTime::currentDateTime();
    int today = current.date().dayOfWeek();
    QTime now = current.time();
    if (current.date() != lastCheckedDate) {
        notifiedEventsToday.clear();
        lastCheckedDate = current.date();
    }
    for (const ClassEvent& e : classEvents) {
        QString eventKey = QString("%1-%2").arg(e.name).arg(e.time.toString("hh:mm"));
        if (notifiedEventsToday.contains(eventKey)) {
            continue;
        }
        if (e.dayOfWeek == today && (e.time == now || (now.secsTo(e.time) <= 600 && now.secsTo(e.time) >= -1800))) {
            trayIcon->showMessage("Schedule Reminder", QString("%1 starts now").arg(e.name), QSystemTrayIcon::Information, 5000);
            notifiedEventsToday.insert(eventKey);
        }
    }
}








