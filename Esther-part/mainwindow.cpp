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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Button
    // Switch Page Button

    connect(ui->pushButton_1, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(0);  // Go to Page 1
    });

    connect(ui->pushButton_2, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(1);  // Go to Page 2
    });

    connect(ui->pushButton_4, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(2);  // Go to Page 3
    });

    connect(ui->pushButton_3, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(3);  // Go to Page 4
    });

    // Connect "Exit" button
    connect(ui->quitButton, &QPushButton::clicked, this, [](){
        QApplication::quit();
    });

    // Image of ToolButton
    ui->choose_img->setPixmap(QPixmap(":/icons/icons/icons8-choose-96.png"));
    ui->choose_img->setScaledContents(true);

    ui->plan_img->setPixmap(QPixmap(":/icons/icons/icons8-write-96.png"));
    ui->plan_img->setScaledContents(true);

    ui->notdone_img->setPixmap(QPixmap(":/icons/icons/icons8-reload-100.png"));
    ui->notdone_img->setScaledContents(true);

    ui->record_img->setPixmap(QPixmap(":/icons/icons/icons8-history-100.png"));
    ui->record_img->setScaledContents(true);


    // Overlay Widget
    overlayWidget = new QWidget(ui->centralwidget);  // parent it to central widget
    overlayWidget->setGeometry(ui->centralwidget->rect());  // cover whole central widget
    overlayWidget->setStyleSheet("background-color: beige"); // semi-transparent black
    overlayWidget->hide();  // start hidden

    QLabel* label = new QLabel("This is an overlay page", overlayWidget);
    label->setStyleSheet("color: white; font-size: 20px;");
    label->move(50, 50);


    // 初始化 Overlays
    englishOverlay = new englishoverlay(ui->centralwidget);
    englishOverlay->hide();

    programmingOverlay = new programmingoverlay(ui->centralwidget);
    programmingOverlay->hide();


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
    }
)");

    QAction* action1 = menu->addAction("学习英语");
    QAction* action2 = menu->addAction("学习编程");
    QAction* action3 = menu->addAction("健身计划");

    ui->toolButton->setMenu(menu);
    ui->toolButton->setPopupMode(QToolButton::InstantPopup); // or MenuButtonPopup

    connect(action1, &QAction::triggered, this, [this]() {
        qDebug() << "Choice 1 selected";

        englishOverlay->setGeometry(ui->centralwidget->rect());
        englishOverlay->show();
        showOverlay(englishOverlay);
    });

    connect(action2, &QAction::triggered, this, [this]() {
        qDebug() << "Choice 2 selected";

        programmingOverlay->setGeometry(ui->centralwidget->rect());
        programmingOverlay->show();
        showOverlay(programmingOverlay);

    });

    connect(action3, &QAction::triggered, this, [this]() {
        qDebug() << "Choice 3 selected";
        overlayWidget->show();

    });


}



// 调整overlay大小
void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    englishOverlay->setGeometry(ui->centralwidget->rect());
    programmingOverlay->setGeometry(ui->centralwidget->rect());
    overlayWidget->setGeometry(ui->centralwidget->rect());
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    englishOverlay->setGeometry(ui->centralwidget->rect());
    programmingOverlay->setGeometry(ui->centralwidget->rect());
    overlayWidget->setGeometry(ui->centralwidget->rect());
}

void MainWindow::showOverlay(QWidget *overlay) {
    englishOverlay->hide();
    programmingOverlay->hide();

    overlay->setGeometry(ui->centralwidget->rect());
    overlay->raise();
    overlay->show();
}


MainWindow::~MainWindow()
{
    delete ui;
}







