#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "clickablelabel.h"
#include "timetable.h"
#include "profile.h"
#include "welcomepage.h"

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
#include <QRandomGenerator>
#include <QPieSeries>
#include <QChartView>

bool doTask = true;

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

    generateQuote();

    // Button
    // Switch Page Button, bottom bar
    connect(ui->pushButton_1, &QPushButton::clicked, this, [=]() { //
        ui->stackedWidget->setCurrentIndex(0);  // Go to Page 1
    });

    connect(ui->pushButton_2, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(1);  // Go to Page 2
        this->hide();
        Timetable *timetable = new Timetable(this);
        timetable->show();

    });

    connect(ui->pushButton_3, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(2);  // Go to Page 3
        generateQuote();
        loadTodayTasksToChart();
    });

    connect(ui->pushButton_4, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(3);  // Go to Page 4
        this->hide();
        bool doTaskToday = true;

        Profile *profile = new Profile(this);
        profile->doTaskToday=doTask;
        profile->displayRightPanel();

        profile->applyStyle();
        profile->show();

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

    // ui->notdone_img->setPixmap(QPixmap(":/icons/icons/icons8-reload-100.png")); //未完成
    // ui->notdone_img->setScaledContents(true);

    ui->record_img->setPixmap(QPixmap(":/icons/icons/icons8-history-100.png")); //历史记录
    ui->record_img->setScaledContents(true);


    // 初始化 Overlays
    englishOverlay = new englishoverlay(ui->centralwidget);
    englishOverlay->hide();


    // 首页
    ui->toolButton->setStyleSheet(R"(
        QToolButton{
            background-color: #E28A52;
            color: beige;
            border-radius: 10px;
            font-size: 12px;
        }

        QToolButton:hover {
            background-color: #EDA270;
        }
)");
//     ui->toolButton_2->setStyleSheet(R"(
//         QToolButton{
//             background-color: #E28A52;
//             color: beige;
//             border-radius: 10px;
//             font-size: 12px;
//         }

//         QToolButton:hover {
//             background-color: #EDA270;
//         }
// )");
    ui->toolButton_3->setStyleSheet(R"(
        QToolButton{
            background-color: #E28A52;
            color: beige;
            border-radius: 10px;
            font-size: 12px;
        }

        QToolButton:hover {
            background-color: #EDA270;
        }
)");
    ui->toolButton_4->setStyleSheet(R"(
        QToolButton{
            background-color: #E28A52;
            color: beige;
            border-radius: 10px;
            font-size: 12px;
        }

        QToolButton:hover {
            background-color: #EDA270;
        }
)");

    // 选择计划
    QMenu* menu = new QMenu(this);
    menu->setStyleSheet(R"(
    QMenu {
        background-color: white;
    }
    QMenu::item {
        color: navy;
        padding: 5px 20px;
        font-family: Arial;
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

        QStringList tasks = {"观看剪辑教学影片", "尝试剪辑长达1分钟的影片", "学习新的剪辑技巧"};
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
    // connect(ui->toolButton_2, &QPushButton::clicked, this, &MainWindow::loadUnfinishedPlans);


    // 历史记录


    // 数据统计
    // successLabel - turning page
    connect(ui->successLabel, &clickableLabel::clicked, this, [=]() {
        achievementBoard = new achievementboard(this);  // Set MainWindow as parent

        connect(achievementBoard, &achievementboard::backToMain, this, [this]() {
            achievementBoard->hide();
            this->centralWidget()->show();  // Show main page again
        });

        achievementBoard->show();
        this->centralWidget()->hide();  // Hide main page
    });

    // graphLabel - turning page
    connect(ui->graphLabel, &clickableLabel::clicked, this, [=]() {
        Graph = new graph(this);  // Set MainWindow as parent

        connect(Graph, &graph::backToMain, this, [this]() {
            Graph->hide();
            this->centralWidget()->show();  // Show main page again
        });

        Graph->show();
        this->centralWidget()->hide();  // Hide main page
    });

    // update hoursLabel

    // update totalDaysLabel


}



// // go back to stacked page
void MainWindow::goBackToStackedPage(int index) {
    ui->stackedWidget->setCurrentIndex(index);
    this->show();
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
    int labelWidth = 300;
    int labelHeight = 250;

    // Center the label inside central widget
    int x = (ui->centralwidget->width() - labelWidth) / 2 + 40;
    int y = (ui->centralwidget->height() - labelHeight) / 2;

    // Set geometry: position + size
    ui->label->setGeometry(x, y, labelWidth, labelHeight);

    // Load and scale image
    QPixmap pixmap(":/icons/icons/main-pic.png");
    ui->label->setPixmap(pixmap.scaled(labelWidth, labelHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::showOverlay(QWidget *overlay) {
    englishOverlay->hide();

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


// generate quote
void MainWindow::generateQuote() {
    QStringList testQuotes = {
        "打卡成功！知识+1，拖延症-1","别问学了多少, 问就是稳稳进步了一点点", "你离‘卷王’又近了一步！","一不小心，又变优秀了一点", "脑细胞阵亡数个，但知识获得感爆棚", "今天不是“咸鱼”", "你刚刚悄悄超过了90%的咸鱼", "系统提示：你正在悄悄变厉害！", "不是我吹，我今天真的认真学了！", "别人只在朋友圈学习，我是真的动了脑子"
    };

    // Show ONE random quote
    if (!testQuotes.isEmpty()) {
        int index = QRandomGenerator::global()->bounded(testQuotes.size());
        QString selectedQuote = testQuotes.at(index);
        ui->quoteWord->setText(selectedQuote);
        qDebug() << "Selected quote:" << selectedQuote;  // optional debug
    } else {
        ui->quoteWord->setText("No quotes available.");
    }
}



// chart
void MainWindow::loadTodayTasksToChart() {
    // total
    QMap<QString, int> taskMap;  // Key: task name, Value: total duration

    QFile file(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/saved_data.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Failed to open saved_data.txt");
        return;
    }

    QTextStream in(&file);

    // QDate::currentDate().toString("yyyy-MM-dd");
    QString today = "2025-05-20";  // Hardcoded for testing (replace with QDate::currentDate() later)
    qDebug() << "Loading tasks for date:" << today;

    // Read and accumulate task durations
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split("|");

        // Skip malformed lines (expected format: task|date|duration)
        if (parts.size() != 3) {
            qDebug() << "Skipping malformed line:" << line;
            continue;
        }

        QString task = parts[0].trimmed();
        QString date = parts[1].trimmed();
        bool match;
        int duration = parts[2].trimmed().toInt(&match);

        if (date == today && match) {
            taskMap[task] += duration;  // Sum durations for duplicate tasks
            qDebug() << "Processed task:" << task << "Duration:" << duration << "Total:" << taskMap[task];
        }
    }
    file.close();

    // Create the pie series from merged data
    QPieSeries *series = new QPieSeries();
    for (auto it = taskMap.begin(); it != taskMap.end(); ++it) {
        series->append(it.key(), it.value());
    }

    // Abort if no data found
    if (series->slices().isEmpty()) {
        qWarning("No tasks found for the specified date.");
        delete series;  // Cleanup
        return;
    }

    // Apply styling (unchanged from your original code)
    QStringList colors = {"#0B1A36", "#E28A52", "#A3A9B2"}; // "#FFFFFF", "#7D5BA6", "#56A3A6"
    for (int i = 0; i < series->slices().size(); ++i) {
        if (i < colors.size()) {
            series->slices().at(i)->setBrush(QColor(colors[i]));
        }

        QFont labelFont = series->slices().at(i)->labelFont();
        labelFont.setPointSize(11); // Default is small, increase for better visibility
        series->slices().at(i)->setLabelFont(labelFont);
        series->slices().at(i)->setLabelVisible(true); // ensure labels are shown
    }

    series->setPieSize(0.8);  // 0.8 of the view area — default is 1.0

    series->setHoleSize(0.45);
    for (auto slice : series->slices()) {
        slice->setLabelVisible(true);
        slice->setLabelPosition(QPieSlice::LabelOutside);
    }

    if (ui->donut_chart->layout()) {
        QLayout* oldLayout = ui->donut_chart->layout();
        delete oldLayout;  // This removes and deletes the layout
    }
    ui->donut_chart->setLayout(nullptr);  // Ensures no layout interferes


    // Set up the chart
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setBackgroundVisible(false);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->setMargins(QMargins(30, 40, 130, 90));  // more margin to avoid clipping // (30, 40, 130, 60)
    chart->legend()->hide();

    // chart->setFixedSize(500, 350);

    // Set up the chart view
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet("background: transparent;");
    chartView->setFixedSize(480, 320);


    // Clear previous chart (if any)
    QLayout *layout = ui->donut_chart->layout();
    if (!layout) {
        layout = new QVBoxLayout(ui->donut_chart);
        ui->donut_chart->setLayout(layout);
    } else {
        QLayoutItem *item;
        while ((item = layout->takeAt(0))) {
            if (item->widget()) {
                delete item->widget();  // Delete the old chart view
            }
            delete item;
        }
    }

    // Add the new chart
    layout->addWidget(chartView);
    layout->setContentsMargins(0, 0, 0, 0);  // Bottom padding only
    layout->setSpacing(0);
    layout->setAlignment(chartView, Qt::AlignLeft | Qt::AlignHCenter);  // Then set alignment


    qDebug() << "Chart updated with" << series->slices().size() << "tasks.";


    chartView->setParent(ui->donut_chart);  // Ensure correct parent
    chartView->setGeometry(300, 50, 480, 320);  // x, y, width, height
    chartView->show();  // Make sure it's visible
}










MainWindow::~MainWindow()
{
    delete ui;
}







