#include "timetable.h"
#include "ui_timetable.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QTableWidgetItem>
#include <QDebug>
#include <QTimer>
#include <QDate>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QGraphicsDropShadowEffect>
#include <QStandardPaths>

int currentStyle = 0;
QVector<QVector<QString>> timetableData(12, QVector<QString>(7));

struct ClassEvent {
    QString name;
    QTime time;
    int dayOfWeek;
};
QList<ClassEvent> classEvents;

struct CellNote {
    QString name;
    QString location;
    QString remark;
};

QVector<QVector<CellNote>> notesData(12, QVector<CellNote>(7));

Timetable::Timetable(MainWindow *parentWindow, QWidget *parent)
    : QWidget(parent), mainWindow(parentWindow)
    , ui(new Ui::Timetable)
{
    ui->setupUi(this);
    ui->gridLayout->setContentsMargins(30, 30, 30, 30);
    applyStyle(currentStyle);
    if (QFile::exists("saved_timetable.csv")) {
        savedFile = "saved_timetable.csv";
        loadCSVFile(savedFile);
        qDebug() << "Current working directory:" << QDir::currentPath();
        qDebug() << "Looking for file at:" << QDir::currentPath() + "/saved_timetable.csv";

    }


    if (QFile::exists("saved_CellData.txt")) {
        savedCellDataFile = "saved_CellData.txt";
        loadCellData(savedCellDataFile);
    }


    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/icons/icon/book.png"));
    trayIcon->show();
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Timetable::checkForClassNotification);
    timer->start(60000);
    connect(ui->tableTimetable, &QTableWidget::cellClicked, this, &Timetable::handleCellClicked);
}

Timetable::~Timetable()
{
    delete ui;
}

void Timetable::loadCellData(const QString& filename)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(',');
            if (parts.size() < 4) {
                continue;
            }
            int row = parts[0].toInt();
            int col = parts[1].toInt();
            QString location = parts[2].replace("&#44;", ",");
            QString remark = parts[3].replace("&#44;", ",").replace("\\n", "\n");
            if (row >= 0 && row < 12 && col >= 0 && col < 7) {
                notesData[row][col].location = location;
                notesData[row][col].remark = remark;
            }
        }
        file.close();
        //QMessageBox::information(this, "Loaded", "Notes loaded successfully.");
    }
    else {
        QMessageBox::warning(this, "Failed to load data.", "Unable to open file for reading.");
        return;
    }
}


void Timetable::saveCellData(const QString& filename)
{
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < 7; j++) {
                CellNote& note = notesData[i][j];
                out << i << "," << j << "," << note.location.replace(",", "&#44;") << "," << note.remark.replace("\n", "\\n").replace(",", "&#44;") << "\n";
            }
        }
        file.close();
        //QMessageBox::information(this, "Saved", "Notes saved successfully.");
    }
    else {
        QMessageBox::warning(this, "Failed to save data.", "Unable to open file for writing.");
        return;
    }
}

void Timetable::on_returnButton_clicked()
{
    saveCellData(savedCellDataFile);
    saveCSVFile(savedFile);
    currentStyle = 0;
    mainWindow->goBackToStackedPage(0);
    this->hide();

}


void Timetable::on_loadFileButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Timetable CSV", "", "CSV Files (*.csv)");
    if (!filename.isEmpty()) {
        QFile::remove("saved_timetable.csv");
        savedFile = "saved_timetable.csv";
        QFile::copy(filename, savedFile);
        loadCSVFile(savedFile);
        QFile::remove("saved_CellData.txt");
        savedCellDataFile = "saved_CellData.txt";
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < 7; j++) {
                notesData[i][j] = CellNote();
            }
        }
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
    //QStringList days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    QStringList days = {"星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日"};
    ui->tableTimetable->setHorizontalHeaderLabels(days);
    ui->tableTimetable->horizontalHeader()->setVisible(true);
    ui->tableTimetable->setRowCount(12);
    //QStringList periods {"1st (08:00)", "2nd (09:00)", "3rd (10:10)", "4th (11:10)", "5th (13:00)", "6th (14:00)", "7th (15:10)", "8th (16:10)", "9th (17:10)", "10th (18:40)", "11th (19:40)", "12th (20:40)"};
    QStringList periods {"第一节 (08:00)", "第二节 (09:00)", "第三节 (10:10)", "第四节 (11:10)", "第五节 (13:00)", "第六节 (14:00)", "第七节 (15:10)", "第八节 (16:10)", "第九节 (17:10)", "第十节 (18:40)", "第十一节 (19:40)", "第十二节 (20:40)"};
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
                ClassEvent e;
                e.name = text;
                e.dayOfWeek = col + 1;
                e.time = QTime::fromString(periods[row].mid(periods[row].indexOf('(') + 1, 5), "hh:mm");
                classEvents.append(e);
            }
            ui->tableTimetable->setItem(row, col, item);
        }
        row++;
    }
    ui->tableTimetable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableTimetable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableTimetable->setShowGrid(true);
    applyStyle(currentStyle);
    file.close();
}


void Timetable::saveCSVFile(const QString& filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);
        for (int i = 0; i < 12; i++) {
            QStringList rowContents;
            for (int j = 0; j < 7; j++) {
                QTableWidgetItem* item = ui->tableTimetable->item(i, j);
                QString text = item ? item->text() : "";
                rowContents << text.replace(",", "&#44;");
            }
            out << rowContents.join(",") << "\n";
        }
        file.close();
        //QMessageBox::information(this, "Saved", "Notes saved successfully.");
    }
    else {
        qDebug() << "Unable to open file for writing: " << filePath;
        return;
    }
}

void Timetable::on_comboStyle_currentIndexChanged(int index)
{
    currentStyle = index;
    applyStyle(index);
}

void Timetable::applyStyle(int index)
{
    QString style;
    switch (index) {
    case 2: style = "#f5faff"; break;
    case 1: style = "#fff8f9"; break;
    case 3: style = "#f8f4f8"; break;
    case 0: style = "#fff7ef"; break;
    }

    this->setStyleSheet("background-color: " + style + "; ");

    switch (index){
        case 2:
            style = R"(
            QTableWidget { background-color: #f5faff; gridline-color: white; font-size: 12px; font-family: "Noto Sans SC", "Helvetica"; color: #051650; }
            QHeaderView::section { background-color: #90caf9; font-weight: bold; font-size: 12px; font-family: "Noto Sans SC", "Helvetica"; border: 1px solid #ffffff; border-radius: 6px; padding: 6px; qproperty-alignment: 'AlignCenter'; color: #051650; }
            QTableCornerButton::section { background-color: #90caf9; border: 1px solid #ffffff; border-radius: 6px; padding: 6px; }
            QTableWidget::item:selected { background-color: #c5e1a5; } )";
            break;
        case 1:
            style = R"(
            QTableWidget { background-color: #fff8f9; gridline-color: white; font-size: 12px; font-family: "Noto Sans SC", "Helvetica"; color: #451926; }
            QHeaderView::section { background-color: #f38fb1; font-weight: bold; font-size: 12px; font-family: "Noto Sans SC", "Helvetica"; border: 1px solid #ffffff; border-radius: 6px; padding: 6px; qproperty-alignment: 'AlignCenter'; color: #451926; }
            QTableCornerButton::section { background-color: #f38fb1; border: 1px solid #ffffff; border-radius: 6px; padding: 6px; }
            QTableWidget::item:selected { background-color: #f8bbd0; } )";
            break;
        case 3:
            style = R"(
            QTableWidget { background-color: #f8f4f8; gridline-color: white; font-size: 12px; font-family: "Noto Sans SC", "Helvetica"; color: #2a0134; }
            QHeaderView::section { background-color: #9575cd; font-weight: bold; font-size: 12px; font-family: "Noto Sans SC", "Helvetica"; border: 1px solid #ffffff; border-radius: 6px; padding: 6px; qproperty-alignment: 'AlignCenter'; color: #2a0134; }
            QTableCornerButton::section { background-color: #9575cd; border: 1px solid #ffffff; border-radius: 6px; padding: 6px; }
            QTableWidget::item:selected { background-color: #d1c4e9; } )";
            break;
        case 0:
            style = R"(
            QTableWidget { background-color: #fff7ef; gridline-color: white; font-size: 12px; font-family: "Noto Sans SC", "Helvetica"; color: #633a00; }
            QHeaderView::section { background-color: #ffbf68; font-weight: bold; font-size: 12px; font-family: "Noto Sans SC", "Helvetica"; border: 1px solid #ffffff; border-radius: 6px; padding: 6px; qproperty-alignment: 'AlignCenter'; color: #633a00; }
            QTableCornerButton::section { background-color: #ffbf68; border: 1px solid #ffffff; border-radius: 6px; padding: 6px; }
            QTableWidget::item:selected { background-color: #ffdfb3; } )";
            break;
    }
    ui->tableTimetable->setStyleSheet(style);

    switch (index){
    case 2: style = "#e1f5fe"; break;
    case 1: style = "#fce4ec"; break;
    case 3: style = "#ede7f6"; break;
    case 0: style = "#ffeacd"; break;
    }
    for (int row = 0; row < ui->tableTimetable->rowCount(); row++) {
        for (int col = 0; col < ui->tableTimetable->columnCount(); col++) {
            QTableWidgetItem* item = ui->tableTimetable->item(row, col);
            if (item && item->text().isEmpty()) { item->setBackground(QBrush(QColor(Qt::white))); }
            else { item->setBackground(QBrush(QColor(style))); }
        }
    }

    switch (index){
    case 2:
        style = R"(
        QComboBox { background-color: #e3f2fd; border: 1px solid #90caf9; padding: 5px; font-size: 13px; border-radius: 5px; font-family: "Noto Sans SC", "Helvetica"; color: #051650; }
        QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: top right; width: 20px; border-left: 1px solid #90caf9; color: #051650; }
        QComboBox QAbstractItemView { background-color: #ffffff; selection-background-color: #bbdefb; } )";
        break;
    case 1:
        style = R"(
        QComboBox { background-color: #fce4ec; border: 1px solid #f38fb1; padding: 5px; font-size: 13px; border-radius: 5px; font-family: "Noto Sans SC", "Helvetica"; color: #451926; }
        QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: top right; width: 20px; border-left: 1px solid #f38fb1; color: #451926; }
        QComboBox QAbstractItemView { background-color: #ffffff; selection-background-color: #b94366; } )";
        break;
    case 3:
        style = R"(
        QComboBox { background-color: #ede7f6; border: 1px solid #9575cd; padding: 5px; font-size: 13px; border-radius: 5px; font-family: "Noto Sans SC", "Helvetica"; color: #2a0134; }
        QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: top right; width: 20px; border-left: 1px solid #9575cd; color: #2a0134; }
        QComboBox QAbstractItemView { background-color: #ffffff; selection-background-color: #b399d4; } )";
        break;
    case 0:
        style = R"(
        QComboBox { background-color: #fff4e6; border: 1px solid #ffbf68; padding: 5px; font-size: 13px; border-radius: 5px; font-family: "Noto Sans SC", "Helvetica"; color: #633a00; }
        QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: top right; width: 20px; border-left: 1px solid #ffbf68; color: #633a00; }
        QComboBox QAbstractItemView { background-color: #ffffff; selection-background-color: #ff6f00; } )";
        break;
    }
    ui->comboStyle->setStyleSheet(style);

    switch (index){
    case 2:
        style = R"(
        QPushButton { background-color: #e3f2fd; border: 1px solid #90caf9; padding: 8px 16px; font-size: 14px; border-radius: 6px; font-family: "Noto Sans SC", "Helvetica"; color: #051650; }
        QPushButton::hover { background-color: #90caf9; }
        QPushButton::pressed { background-color: #4db6ac; } )";
        break;
    case 1:
        style = R"(
        QPushButton { background-color: #fce4ec; border: 1px solid #f38fb1; padding: 8px 16px; font-size: 14px; border-radius: 6px; font-family: "Noto Sans SC", "Helvetica"; color: #451926; }
        QPushButton::hover { background-color: #f38fb1; }
        QPushButton::pressed { background-color: #f8bbd0; } )";
        break;
    case 3:
        style = R"(
        QPushButton { background-color: #ede7f6; border: 1px solid #9575cd; padding: 8px 16px; font-size: 14px; border-radius: 6px; font-family: "Noto Sans SC", "Helvetica"; color: #2a0134; }
        QPushButton::hover { background-color: #9575cd; }
        QPushButton::pressed { background-color: #d1c4e9; } )";
        break;
    case 0:
        style = R"(
        QPushButton { background-color: #fff4e6; border: 1px solid #ffbf68; padding: 8px 16px; font-size: 14px; border-radius: 6px; font-family: "Noto Sans SC", "Helvetica"; color: #633a00; }
        QPushButton::hover { background-color: #ffbf68; }
        QPushButton::pressed { background-color: #ffdfb3; } )";
        break;
    }
    ui->loadFileButton->setStyleSheet(style);

    switch (index){
    case 2:
        style = R"(
        QPushButton { background-color: #e3f2fd; border: 1px solid #90caf9; padding: 8px 16px; font-size: 12px; border-radius: 6px; font-family: "Noto Sans SC", "Helvetica"; color: #051650; }
        QPushButton::hover { background-color: #90caf9; }
        QPushButton::pressed { background-color: #4db6ac; } )";
        break;
    case 1:
        style = R"(
        QPushButton { background-color: #fce4ec; border: 1px solid #f38fb1; padding: 8px 16px; font-size: 12px; border-radius: 6px; font-family: "Noto Sans SC", "Helvetica"; color: #451926; }
        QPushButton::hover { background-color: #f38fb1; }
        QPushButton::pressed { background-color: #f8bbd0; } )";
        break;
    case 3:
        style = R"(
        QPushButton { background-color: #ede7f6; border: 1px solid #9575cd; padding: 8px 16px; font-size: 12px; border-radius: 6px; font-family: "Noto Sans SC", "Helvetica"; color: #2a0134; }
        QPushButton::hover { background-color: #9575cd; }
        QPushButton::pressed { background-color: #d1c4e9; } )";
        break;
    case 0:
        style = R"(
        QPushButton { background-color: #fff4e6; border: 1px solid #ffbf68; padding: 8px 16px; font-size: 12px; border-radius: 6px; font-family: "Noto Sans SC", "Helvetica"; color: #633a00; }
        QPushButton::hover { background-color: #ffbf68; }
        QPushButton::pressed { background-color: #ffdfb3; } )";
        break;
    }
    ui->returnButton->setStyleSheet(style);
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
        if (e.dayOfWeek == today && (e.time == now || (now.secsTo(e.time) <= 0 && now.secsTo(e.time) >= -600))) { // Reminder at ten minutes before.
            trayIcon->showMessage("日程提醒", QString("事件“%1”将在10分钟后开始。").arg(e.name), QSystemTrayIcon::Information, 5000);
            notifiedEventsToday.insert(eventKey);
        }
    }
}


void Timetable::handleCellClicked(int row, int col)
{
    QString itemName = ui->tableTimetable->item(row, col) ? ui->tableTimetable->item(row, col)->text() : "";
    if (itemName.isEmpty()) {
        return;
    }
    NoteDialog dialog(this);
    dialog.setCourseName(itemName);
    dialog.setLocation(notesData[row][col].location);
    dialog.setRemark(notesData[row][col].remark);
    if (dialog.exec() == QDialog::Accepted) {
        notesData[row][col].name = itemName;
        notesData[row][col].location = dialog.getLocation();
        notesData[row][col].remark = dialog.getRemark();
    }
}


// For Note Dialog:
NoteDialog::NoteDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Edit Note");
    itemLabel = new QLabel("事件");
    editLocationLine = new QLineEdit;
    editRemarkText = new QTextEdit;
    saveButton = new QPushButton("保存");
    layout = new QVBoxLayout;
    layout->addWidget(itemLabel);
    layout->addWidget(new QLabel("地点:"));
    layout->addWidget(editLocationLine);
    layout->addWidget(new QLabel("备注:"));
    layout->addWidget(editRemarkText);
    layout->addWidget(saveButton);
    setLayout(layout);
    applyDialogStyle(currentStyle);
    connect(saveButton, &QPushButton::clicked, this, &QDialog::accept);
}
void NoteDialog::setCourseName(const QString& itemName)
{
    itemLabel->setText("事件: " + itemName);
}
void NoteDialog::setLocation(const QString& location)
{
    editLocationLine->setText(location);
}
void NoteDialog::setRemark(const QString& remark)
{
    editRemarkText->setPlainText(remark);
}

QString NoteDialog::getLocation() const
{
    return editLocationLine->text();
}
QString NoteDialog::getRemark() const
{
    return editRemarkText->toPlainText();
}
void NoteDialog::applyDialogStyle(int index)
{
    QString style;
    switch (index){
    case 2:
        style = R"(
        QLabel { font-size: 16px; font-family: "Noto Sans SC", "Helvetica"; font-weight: bold; color: #444444; }
        QDialog{ background-color: #f5faff; border: 1px solid #90caf9; padding: 8px 16px; font-size: 14px; border-radius: 6px; font-family: "Noto Sans SC", "Helvetica"; color: #051650; }
        QLineEdit { background-color: #ffffff; border: 2px solid #d0d0d0; padding: 8px; font-size: 14px; border-radius: 10px; font-family: "Noto Sans SC", "Helvetica"; color: #051650; }
        QLineEdit:focus { border: 2px solid #91c9f7; background-color: #f0f4ff; }
        QTextEdit{ background-color: #ffffff; border: 2px solid #d0d0d0; padding: 8px; font-size: 14px; border-radius: 10px; font-family: "Noto Sans SC", "Helvetica"; color: #051650; }
        QTextEdit:focus { border: 2px solid #91c9f7; background-color: #f0f4ff; }
        QPushButton { background-color: #e3f2fd; border: 1px solid #90caf9; padding: 8px 16px; font-size: 14px; border-radius: 12px; font-family: "Noto Sans SC", "Helvetica"; color: #051650; }
        QPushButton::hover { background-color: #90caf9; }
        QPushButton::pressed { background-color: #4db6ac; } )";
        break;

    case 1:
        style = R"(
        QLabel { font-size: 14px; font-family: "Noto Sans SC", "Helvetica"; font-weight: bold; color: #444444; }
        QDialog{ background-color: #fff8f9; border: 1px solid #f38fb1; padding: 8px 16px; font-size: 14px; border-radius: 6px; font-family: "Noto Sans SC", "Helvetica"; color: #451926;}
        QLineEdit { background-color: #ffffff; border: 2px solid #d0d0d0; padding: 8px; font-size: 14px; border-radius: 10px; font-family: "Noto Sans SC", "Helvetica"; color: #451926; }
        QLineEdit:focus { border: 2px solid #ff87b2; background-color: #fff8fb; }
        QTextEdit{ background-color: #ffffff; border: 2px solid #d0d0d0; padding: 8px; font-size: 14px; border-radius: 10px; font-family: "Noto Sans SC", "Helvetica"; color: #451926; }
        QTextEdit:focus { border: 2px solid #ff87b2; background-color: #fff8fb; }
        QPushButton { background-color: #fce4ec; border: 1px solid #f38fb1; padding: 8px 16px; font-size: 14px; border-radius: 12px; font-family: "Noto Sans SC", "Helvetica"; color: #451926; }
        QPushButton::hover { background-color: #f38fb1; }
        QPushButton::pressed { background-color: #f8bbd0; } )";
        break;

    case 3:
        style = R"(
        QLabel { font-size: 14px; font-family: "Noto Sans SC", "Helvetica"; font-weight: bold; color: #444444; }
        QDialog{ background-color: #f8f4f8; border: 1px solid #9575cd; padding: 8px 16px; font-size: 14px; border-radius: 6px; font-family: "Noto Sans SC", "Helvetica"; color: #2a0134; }
        QLineEdit { background-color: #ffffff; border: 2px solid #d0d0d0; padding: 8px; font-size: 14px; border-radius: 10px; font-family: "Noto Sans SC", "Helvetica"; color: #2a0134; }
        QLineEdit:focus { border: 2px solid #a685e2; background-color: #f9f5ff; }
        QTextEdit{ background-color: #ffffff; border: 2px solid #d0d0d0; padding: 8px; font-size: 14px; border-radius: 10px; font-family: "Noto Sans SC", "Helvetica"; color: #2a0134; }
        QTextEdit:focus { border: 2px solid #a685e2; background-color: #f9f5ff; }
        QPushButton { background-color: #ede7f6; border: 1px solid #9575cd; padding: 8px 16px; font-size: 14px; border-radius: 12px; font-family: "Noto Sans SC", "Helvetica"; color: #2a0134; }
        QPushButton::hover { background-color: #9575cd; }
        QPushButton::pressed { background-color: #d1c4e9; } )";
        break;
    case 0:
        style = R"(
        QLabel { font-size: 14px; font-family: "Noto Sans SC", "Helvetica"; font-weight: bold; color: #444444; }
        QDialog{ background-color: #fff7ef; border: 1px solid #ffbf68; padding: 8px 16px; font-size: 14px; border-radius: 6px; font-family: "Noto Sans SC", "Helvetica"; color: #633a00; }
        QLineEdit { background-color: #ffffff; border: 2px solid #d0d0d0; padding: 8px; font-size: 14px; border-radius: 10px; font-family: "Noto Sans SC", "Helvetica"; color: #633a00; }
        QLineEdit:focus { border: 2px solid #a67b5b; background-color: #fdf6ec; }
        QTextEdit{ background-color: #ffffff; border: 2px solid #d0d0d0; padding: 8px; font-size: 14px; border-radius: 10px; font-family: "Noto Sans SC", "Helvetica"; color: #633a00; }
        QTextEdit:focus { border: 2px solid #a67b5b; background-color: #fdf6ec; }
        QPushButton { background-color: #fff4e6; border: 1px solid #c4a484; padding: 8px 16px; font-size: 14px; border-radius: 12px; font-family: "Noto Sans SC", "Helvetica"; color: #633a00; }
        QPushButton::hover { background-color: #ffbf68; }
        QPushButton::pressed { background-color: #ffdfb3; } )";
        break;
    }

    this->setStyleSheet(style);
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 80));
    this->setGraphicsEffect(shadow);
}



