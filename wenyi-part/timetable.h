#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QDate>
namespace Ui {
class Timetable;
}

class Timetable : public QWidget
{
    Q_OBJECT

public:
    explicit Timetable(QWidget *parent = nullptr);
    ~Timetable();

private slots:
    void on_loadFileButton_clicked();
    void on_comboStyle_currentIndexChanged(int index);

private:
    Ui::Timetable *ui;
    void loadCSVFile(const QString& filePath);
    void applyStyle(int index);
    void checkForClassNotification();
    QSystemTrayIcon *trayIcon;
    QSet<QString> notifiedEventsToday;
    QDate lastCheckedDate;

};

#endif // TIMETABLE_H
