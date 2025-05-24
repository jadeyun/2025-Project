#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QShowEvent>
#include <QResizeEvent>
#include <QRect>
#include <QPoint>
#include <QLayout>
#include <QInputDialog>
#include <QStandardPaths>
#include <QtCharts>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), TaskRecord(new taskRecord(this))
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // // 暂时清空
    // QFile file(TaskRecord->logFilePath);
    // if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    //     QTextStream out(&file);
    //     out << "name|date|year|month|day|time|hour|minute|second|times\n";
    //     file.close();
    //     qDebug() << "日志文件已在启动时清空";
    // }


    // Button
    // Switch Page Button, bottom bar
    connect(ui->pushButton_1, &QPushButton::clicked, this, [=]() { //
        ui->stackedWidget->setCurrentIndex(0);  // Go to Page 1
    });

    connect(ui->pushButton_2, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(1);  // Go to Page 2
    });

    connect(ui->pushButton_3, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(2);  // Go to Page 3
    });

    connect(ui->pushButton_4, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(3);  // Go to Page 4
    });

    // Connect "Exit" button
    connect(ui->quitButton, &QPushButton::clicked, this, [](){
        QApplication::quit();
    });

    // Image of ToolButton
    ui->choose_img->setPixmap(QPixmap(":/icons/icons/icons8-choose-96.png")); //选择计划
    ui->choose_img->setScaledContents(true);

    ui->plan_img->setPixmap(QPixmap(":/icons/icons/icons8-write-96.png")); //制定计划
    ui->plan_img->setScaledContents(true);

    ui->notdone_img->setPixmap(QPixmap(":/icons/icons/icons8-reload-100.png")); //未完成
    ui->notdone_img->setScaledContents(true);

    ui->record_img->setPixmap(QPixmap(":/icons/icons/icons8-history-100.png")); //历史记录
    ui->record_img->setScaledContents(true);


    // 初始化 Overlays
    englishOverlay = new englishoverlay(ui->centralwidget);
    englishOverlay->hide();


    // 首页

    // 选择计划
    QMenu* menu = new QMenu(this);
    menu->setStyleSheet(R"(
    QMenu {
        background-color: white;
    }
    QMenu::item {
        color: navy;
        padding: 5px 20px;
        font-family: Georgia;
    }
)");

    QAction* action1 = menu->addAction("学习英语");
    QAction* action2 = menu->addAction("学习编程");
    QAction* action3 = menu->addAction("学习剪辑");

    ui->toolButton->setMenu(menu);
    ui->toolButton->setPopupMode(QToolButton::InstantPopup); // or MenuButtonPopup

    connect(action1, &QAction::triggered, this, [this]() { // 学习英语
        qDebug() << "Choice 1 selected";

        TaskRecord->logTask("学习英语");

        QStringList tasks = {"背20个单词", "观看两个英语演讲视频", "阅读两篇英语文章"};
        englishOverlay->setName("学习英语");
        englishOverlay->setTasks(tasks);
        englishOverlay->setGeometry(ui->centralwidget->rect());
        englishOverlay->show();
        showOverlay(englishOverlay);
    });

    connect(action2, &QAction::triggered, this, [this]() { // 学习编程
        qDebug() << "Choice 2 selected";

        TaskRecord->logTask("学习编程");

        QStringList tasks = {"观看编程网课", "完成网课习题", "刷10题LeetCode"};
        englishOverlay->setName("学习编程");
        englishOverlay->setTasks(tasks);
        englishOverlay->setGeometry(ui->centralwidget->rect());
        englishOverlay->show();
        showOverlay(englishOverlay);

    });

    connect(action3, &QAction::triggered, this, [this]() { //学习剪辑
        qDebug() << "Choice 3 selected";

        TaskRecord->logTask("学习剪辑");

        QStringList tasks = {"观看剪辑教学影片", "edit", "edit"};
        englishOverlay->setName("学习剪辑");
        englishOverlay->setTasks(tasks);
        englishOverlay->setGeometry(ui->centralwidget->rect());
        englishOverlay->show();
        showOverlay(englishOverlay);

    });


    //制定计划 (KIV)
    connect(ui->toolButton_3, &QPushButton::clicked, this, [this]() {
        bool ok;
        QString text = QInputDialog::getText(this, "Add Task", "Enter a task:", QLineEdit::Normal, "", &ok);

        // TaskRecord->logTask("name");   // name=计划名字，这里可以让他记录在txt

        if (ok && !text.isEmpty()) {
            QStringList tasks;
            tasks << text;
            englishOverlay->setName("name"); // 页面右上角会显示用户自己填的计划名字
            englishOverlay->setTasks(tasks); // 页面上显示任务
            englishOverlay->show();
        }
    });


    // 未完成
    // 记录在unfinished_plan //code 在 englishoverlay.cpp 的back button部分
    connect(ui->toolButton_2, &QPushButton::clicked, this, &MainWindow::loadUnfinishedPlans);


    // 历史记录


    // 时间表


    // 数据统计
    // Donut Chart
    QPieSeries *series = new QPieSeries();
    series->append("Task 1", 20);
    series->append("Task 2", 10);
    series->append("Task 3", 14);
    series->append("Task 4", 20);

    // Set colors manually
    series->slices().at(0)->setBrush(QColor("#0B1A36")); // dark blue
    series->slices().at(1)->setBrush(QColor("#E28A52")); // orange
    series->slices().at(2)->setBrush(QColor("#FFFFFF")); // white
    series->slices().at(3)->setBrush(QColor("#A3A9B2")); // grey

    // Make it a donut
    series->setHoleSize(0.4);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setBackgroundVisible(false);        // Hide background
    chart->setBackgroundBrush(Qt::NoBrush);    // No background fill
    chart->setContentsMargins(0, 0, 0, 0);      // Remove chart margins


    QChartView *chartView = new QChartView(chart);
    chartView->setStyleSheet("background: transparent;");
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setParent(ui->donut_chart); // or your main widget
    chartView->setGeometry(0, 0, 250, 250); // x, y, width, height


}



// 调整overlay大小
void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    englishOverlay->setGeometry(ui->centralwidget->rect());

}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    englishOverlay->setGeometry(ui->centralwidget->rect());


    // Define label size
    int labelWidth = 400;
    int labelHeight = 350;

    // Center the label inside central widget
    int x = (ui->centralwidget->width() - labelWidth) / 2 + 40;
    int y = (ui->centralwidget->height() - labelHeight) / 2 - 50;

    // Set geometry: position + size
    ui->label->setGeometry(x, y, labelWidth, labelHeight);

    // Load and scale image
    QPixmap pixmap(":/icons/icons/main-pic.png");
    ui->label->setPixmap(pixmap.scaled(labelWidth, labelHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::showOverlay(QWidget *overlay) {
    englishOverlay->hide();
    // programmingOverlay->hide();

    overlay->setGeometry(ui->centralwidget->rect());
    overlay->raise();
    overlay->show();
}


// 选择后，加载到新的页面
void MainWindow::loadSelectedPlan(const QString &planName) {
    QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/unfinished_plans.txt";
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    QString line;
    bool found = false;
    QList<QPair<QString, bool>> taskPairs;

    while (!in.atEnd()) {
        line = in.readLine().trimmed();
        if (line == "[[" + planName + "]]") {
            found = true;
            continue;
        }
        if (found) {
            if (line.startsWith("[[") && line.endsWith("]]")) break; // next plan
            if (!line.isEmpty()) {
                QStringList parts = line.split("|");
                if (parts.size() == 2) {
                    QString text = parts[0];
                    bool checked = (parts[1] == "1");
                    taskPairs.append(qMakePair(text, checked));
                }
            }
        }
    }
    file.close();

    // Pass tasks with check states to overlay
    englishOverlay->setName(planName);
    englishOverlay->setTasksWithState(taskPairs);  // New function!
    englishOverlay->show();
}



// 连接到 - 未完成button
void MainWindow::loadUnfinishedPlans() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/unfinished_plans.txt";
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    QString line;
    QStringList planNames;

    while (!in.atEnd()) {
        line = in.readLine().trimmed();
        if (line.startsWith("[[") && line.endsWith("]]")) {
            planNames << line.mid(2, line.length() - 4); // Extract name
        }
    }
    file.close();

    if (planNames.isEmpty()) {
        QMessageBox::information(this, "No Plans", "No unfinished plans found.");
        return;
    }

    // Let user pick one
    bool ok;
    QString selected = QInputDialog::getItem(this, "Select Plan", "Unfinished Plans:", planNames, 0, false, &ok);

    if (ok && !selected.isEmpty()) {
        loadSelectedPlan(selected); // Load into englishoverlay
    }
}



MainWindow::~MainWindow()
{
    delete ui;
}







