#ifndef GRAPH_H
#define GRAPH_H

#include <QWidget>

namespace Ui {
class graph;
}

#include <QWidget>
#include <QtCharts/QChartView>


class graph : public QWidget
{
    Q_OBJECT

public:
    explicit graph(QWidget *parent = nullptr);
    ~graph();

signals:
    void backToMain();  // Signal to notify going back

private slots:
    void on_comboRange_currentIndexChanged(int index);


private:
    Ui::graph *ui;
    void loadGraph(const QVector<QPair<QString, double>>& dataToBeUsed);
    void loadData(const QString& filename);
    void filterData();
    void applyStyle();
    QString savedDataFile = "saved_data.txt";
    QChartView* currentChartView = nullptr;
};

#endif // GRAPH_H


