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

Graph::Graph(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Graph)
{
    ui->setupUi(this);
    ui->bigLayout->setContentsMargins(30, 30, 30, 30);
    if (QFile::exists("saved_data.txt")) {
        savedDataFile = "saved_data.txt";
        loadData(savedDataFile);
    }
    filterData();
    loadGraph(_today_data); // Default: daily.
    applyStyle();
}

Graph::~Graph()
{
    delete ui;
}

void Graph::on_returnButton_clicked()
{
    this->hide();
}

void Graph::on_comboRange_currentIndexChanged(int index)
{
    switch (index) {
    case 0: loadGraph(_today_data); break;
    case 1: loadGraph(_this_month_data); break;
    case 2: loadGraph(_this_year_data); break;
    }
}

void Graph::loadGraph(const QVector<QPair<QString, double>>& dataToBeUsed)
{
    if (currentChartView) {
        ui->graphLayout->removeWidget(currentChartView);
        delete currentChartView;
        currentChartView = nullptr;
    }
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
    chart->setTitle("Top 5 Activities by Time Spent"); // UI: Esther. Please edit in Chinese.
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
    ui->graphLayout->addWidget(currentChartView);
    set->setColor(QColor("#ffa500"));
    //chart->setStyle("backgroundcolor: #fff7ef;");
}

void Graph::loadData(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file.";
        return;
    }
    QTextStream stream(&file);
    graphData.clear();
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        QStringList values = line.split('|');
        GraphData temp(values[0], QDate::fromString(values[1], "yyyy-MM-dd"), values[2].toDouble());
        graphData.push_back(temp);
    }
}

void Graph::filterData()
{
    today_data.clear(); this_month_data.clear(); this_year_data.clear();
    _today_data.clear(); _this_month_data.clear(); _this_year_data.clear();
    if (graphData.isEmpty()) { return; }
    QDate today = QDate::currentDate();
    for (const auto& data : graphData) {
        if (data.date == today) { today_data[data.itemName] += data.timeSpent; }
        if (data.date.month() == today.month()) { this_month_data[data.itemName] += data.timeSpent; }
        if (data.date.year() == today.year()) { this_year_data[data.itemName] += data.timeSpent; }
    }
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

void Graph::applyStyle() // Esther: ui. Below are the original styling code from timetable section.
{
    QString style = R"(
        QComboBox { background-color: #fff4e6; border: 1px solid #ffbf68; padding: 5px; font-size: 13px; border-radius: 5px; font-family: "Noto Sans SC", "Helvetica"; color: #633a00; }
        QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: top right; width: 20px; border-left: 1px solid #ffbf68; color: #633a00; }
        QComboBox QAbstractItemView { background-color: #ffffff; selection-background-color: #ff6f00; } )";
    ui->comboRange->setStyleSheet(style);

    style = R"(
        QPushButton { background-color: #fff4e6; border: 1px solid #ffbf68; padding: 8px 16px; font-size: 12px; border-radius: 6px; font-family: "Noto Sans SC", "Helvetica"; color: #633a00; }
        QPushButton::hover { background-color: #ffbf68; }
        QPushButton::pressed { background-color: #ffdfb3; } )";
    ui->returnButton->setStyleSheet(style);

    this->setStyleSheet("background-color: #fff7ef;");
}
