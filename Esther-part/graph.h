#ifndef GRAPH_H
#define GRAPH_H

#include <QWidget>
#include <QtCharts/QChartView>

namespace Ui {
class Graph;
}

class Graph : public QWidget
{
    Q_OBJECT

public:
    explicit Graph(QWidget *parent = nullptr);
    ~Graph();

private slots:
    void on_returnButton_clicked();
    void on_comboRange_currentIndexChanged(int index);

private:
    Ui::Graph *ui;
    void loadGraph(const QVector<QPair<QString, double>>& dataToBeUsed);
    void loadData(const QString& filename);
    void filterData();
    void applyStyle();
    QString savedDataFile = "saved_data.txt";
    QChartView* currentChartView = nullptr;
};

#endif // GRAPH_H
