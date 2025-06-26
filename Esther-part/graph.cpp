#include "graph.h"
#include "ui_graph.h"
#include <QVector>
#include <QDate>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QtCharts/QChartView>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QStandardPaths>
using namespace std;


struct GraphData {
    QString itemName;
    QDate date;
    double timeSpent; // In minutes.
    GraphData(QString v1, QDate v2, double v3) : itemName(v1), date(v2), timeSpent(v3) {}
};
QVector<GraphData> graphData ; // Test data: = { {"Sleep", QDate::fromString("2025-05-20", "yyyy-MM-dd"), 20}, {"Cook", QDate::fromString("2025-05-20", "yyyy-MM-dd"), 50}}
QMap<QString, double> today_data;
QMap<QString, double> this_month_data;
QMap<QString, double> this_year_data;
QVector<QPair<QString, double>> _today_data;
QVector<QPair<QString, double>> _this_month_data;
QVector<QPair<QString, double>> _this_year_data;

graph::graph(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::graph)
{
    ui->setupUi(this);

    // back button
    connect(ui->backButton, &QPushButton::clicked, this, [this]() {
        emit backToMain();
        this->hide();
    });



    ui->bigLayout->setContentsMargins(30, 30, 30, 30);

    QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/task_log.txt";
    qDebug() << "Looking for file at:" << path;  // Verify this path is correct

    if (QFile::exists(path)) {
        loadData(path);
    } else {
        qDebug() << "File not found at:" << path;
    }
    filterData();
    loadGraph(_today_data); // Default: daily.
    applyStyle();

}


void graph::on_comboRange_currentIndexChanged(int index)
{
    switch (index) {
    case 0: loadGraph(_today_data); break;
    case 1: loadGraph(_this_month_data); break;
    case 2: loadGraph(_this_year_data); break;
    }
}



void graph::loadGraph(const QVector<QPair<QString, double>>& dataToBeUsed)
{
    qDebug() << "Loading graph with" << dataToBeUsed.size() << "items";
    for (const auto& item : dataToBeUsed) {
        qDebug() << " - " << item.first << ":" << item.second;
    }

    if (currentChartView) {
        ui->graphLayout->removeWidget(currentChartView);
        delete currentChartView;
        currentChartView = nullptr;
    }

    if (dataToBeUsed.isEmpty()) {
        qDebug() << "No data to display!";
        QChart* chart = new QChart();
        chart->setTitle("暂无可显示的数据"); // "No data to display"
        chart->legend()->hide();

        currentChartView = new QChartView(chart);
        currentChartView->setRenderHint(QPainter::Antialiasing);
        currentChartView->setMinimumSize(400, 300); // Ensures space is kept

        ui->graphLayout->addWidget(currentChartView);

        return;
    }

    // Rest of the function...
    QBarSet* set = new QBarSet("累计时间（分钟）");
    QStringList categories;
    for (int i = 0; i < dataToBeUsed.size() && i < 5; i++) {
        *set << dataToBeUsed[i].second;
        categories << dataToBeUsed[i].first;
    }

    QBarSeries* series = new QBarSeries();
    series->append(set);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("耗时最多的前五项活动"); // UI: Esther. Please edit in Chinese.
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis* x_axis = new QBarCategoryAxis();
    x_axis->append(categories);
    chart->addAxis(x_axis, Qt::AlignBottom);
    series->attachAxis(x_axis);

    QValueAxis* y_axis = new QValueAxis();
    // y_axis->append("分钟");
    chart->addAxis(y_axis, Qt::AlignLeft);
    series->attachAxis(y_axis);

    currentChartView = new QChartView(chart);
    currentChartView->setRenderHint(QPainter::Antialiasing);
    //debug
    if (!ui->graphLayout) {
        qDebug() << "graphLayout is null!";
    }

    ui->graphLayout->addWidget(currentChartView);

    set->setColor(QColor("#E28A52")); //#ffa500
    //chart->setStyle("backgroundcolor: #fff7ef;");

    currentChartView->update();
    this->update();
}


void graph::loadData(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file:" << filename;
        return;
    }
    QTextStream stream(&file);
    graphData.clear();

    stream.readLine();  // Skip header line

    while (!stream.atEnd()) {
        QString line = stream.readLine();
        qDebug() << "Raw line:" << line;  // Debug output
        QStringList values = line.split('|');
        if (values.size() != 11) {
            qDebug() << "Invalid line format:" << line;
            continue;
        }
        GraphData temp(values[0], QDate::fromString(values[1], "yyyy-MM-dd"), values[10].toDouble());
        graphData.push_back(temp);
    }
    file.close();
    qDebug() << "Graph data loaded:" << graphData.size();
    for (const auto& data : graphData) {
        qDebug() << data.itemName << data.date.toString() << data.timeSpent;
    }
}



void graph::filterData() {
    today_data.clear(); this_month_data.clear(); this_year_data.clear();
    _today_data.clear(); _this_month_data.clear(); _this_year_data.clear();
    if (graphData.isEmpty()) {
        qDebug() << "No graph data available to filter!";
        return;
    }

    // QDate today = QDate::currentDate();
    QDate today = QDate::fromString("2025-05-27", "yyyy-MM-dd");
    // qDebug() << "Filtering data for date:" << today.toString();

    for (const auto& data : graphData) {
        if (data.date == today) { today_data[data.itemName] += data.timeSpent; }
        if (data.date.month() == today.month()) { this_month_data[data.itemName] += data.timeSpent; }
        if (data.date.year() == today.year()) { this_year_data[data.itemName] += data.timeSpent; }
    }

    // Debug output
    qDebug() << "Today's data:" << today_data;
    qDebug() << "This month's data:" << this_month_data;
    qDebug() << "This year's data:" << this_year_data;

    // Rest of the function...
    if (!today_data.isEmpty()) {
            for (auto it = today_data.begin(); it != today_data.end(); it++) { _today_data.push_back(qMakePair(it.key(), it.value())); }
            std::sort(_today_data.begin(), _today_data.end(), [](const auto& a, const auto& b) { return a.second > b.second; } );
        }
        if (!this_month_data.isEmpty()) {
            for (auto it = this_month_data.begin(); it != this_month_data.end(); it++) { _this_month_data.push_back(qMakePair(it.key(), it.value())); }
            std::sort(_this_month_data.begin(), _this_month_data.end(), [](const auto& a, const auto& b) { return a.second > b.second; } );
        }
        if (!this_year_data.isEmpty()) {
            for (auto it = this_year_data.begin(); it != this_year_data.end(); it++) { _this_year_data.push_back(qMakePair(it.key(), it.value())); }
            std::sort(_this_year_data.begin(), _this_year_data.end(), [](const auto& a, const auto& b) { return a.second > b.second; } );
        }
}


void graph::applyStyle() // Esther: ui. Below are the original styling code from timetable section.
{
    //#fff4e6
    QString style = R"(
        QComboBox { background-color: beige; border: 1px solid #ffbf68; padding: 5px; font-size: 13px; border-radius: 5px; font-family: "Arial", "san-serif"; color: #633a00; }
        QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: top right; width: 20px; border-left: 1px solid #ffbf68; color: #633a00; }
        QComboBox QAbstractItemView { background-color: #ffffff; selection-background-color: #ff6f00; } )";
    ui->comboRange->setStyleSheet(style);

    this->setStyleSheet("background-color: beige;");//#fff7ef
}




graph::~graph()
{
    delete ui;
}
