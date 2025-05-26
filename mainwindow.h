#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>  // 新增：用于页面切换
#include <QWidget>         // 新增：基础部件
#include <QVBoxLayout>     // 新增：垂直布局
#include <QLabel>          // 新增：标签
#include <QPushButton>     // 新增：按钮
#include <QMenu>           // 新增：菜单
#include <QAction>         // 新增：菜单项
#include <QToolButton>     // 新增：工具按钮
#include "englishoverlay.h"
// #include "programmingoverlay.h"
#include "taskrecord.h"
#include "achievementboard.h"
#include "graph.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void goBackToStackedPage(int index);

private:
    Ui::MainWindow *ui;

protected:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showOverlay(QWidget* overlay);
    void loadUnfinishedPlans();
    void loadSelectedPlan(const QString &planName);
    void generateQuote();
    void loadTodayTasksToChart();

private:
    englishoverlay *englishOverlay;
    QWidget* overlayWidget;
    taskRecord* TaskRecord;
    taskRecord taskLogger;
    achievementboard *achievementBoard;
    graph *Graph;

};
#endif // MAINWINDOW_H
