#include "achievementboard.h"
#include "ui_achievementboard.h"

#include <QMovie>
#include <QStackedWidget>
#include <QFile>
#include <QTextStream>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QStringList>
#include <QStandardPaths>


achievementboard::achievementboard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::achievementboard)
{
    ui->setupUi(this);

    // back button
    connect(ui->backButton, &QPushButton::clicked, this, [this]() {
        emit backToMain();
        this->hide();
    });

    // star GIF
    QMovie *movie = new QMovie(":/icons/star2.gif"); // if in .qrc

    QSize gifSize = ui->starLabel->size();  // Assuming the QLabel is called starLabel
    movie->setScaledSize(gifSize);
    ui->starLabel->setMovie(movie);
    ui->starLabel_2->setMovie(movie);
    ui->starLabel_3->setMovie(movie);
    movie->start(); // Start playing the GIF

    loadTasks();
}


void achievementboard::loadTasks() {
    QString filePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/task_log.txt";

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QStandardItemModel* model = new QStandardItemModel(this);
    QTextStream in(&file);

    in.readLine();

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        // need to change
        QStringList parts = line.split('|');
        if (parts.size() < 10)
            continue;

        QString task = parts[0].trimmed();
        QString freq = parts[9].trimmed();

        QString paddedTask = task.leftJustified(50, ' ');  // Adjust width as needed
        QString displayText = QString("%1%2").arg(paddedTask, QString("打卡次数: %1").arg(freq));
        QStandardItem* item = new QStandardItem(displayText);

        // Style
        item->setBackground(QBrush(QColor("#F0F0F0"))); // Light background
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        item->setSizeHint(QSize(200, 40)); // Height for padding
        item->setFont(QFont("Arial", 12));

        model->appendRow(item);
    }

    file.close();

    // Set the model to your QListView
    ui->listView->setModel(model);

    // Optional: add spacing and selection style
    ui->listView->setSpacing(8);
    ui->listView->setStyleSheet(R"(
        QListView::item {
            border: 1px solid #E28A52;
            border-radius: 10px;
            margin: 5px;
            padding: 5px;
            color: #0B1A36;
        }
        QListView::item:selected {
            hover-color: rgba(163, 169, 178, 0.3);
            color: #0B1A36;
        }
    )");
}


achievementboard::~achievementboard()
{
    delete ui;
}
