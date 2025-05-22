#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QDate>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

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
    void on_returnButton_clicked();
    void on_loadFileButton_clicked();
    void on_comboStyle_currentIndexChanged(int index);
    void handleCellClicked(int row, int col);

private:
    Ui::Timetable *ui;
    void loadCellData(const QString& filename);
    void saveCellData(const QString& filename);
    void loadCSVFile(const QString& filePath);
    void saveCSVFile(const QString& filePath);
    void applyStyle(int index);
    void checkForClassNotification();
    QString savedFile = "saved_timetable.csv";
    QString savedCellDataFile = "saved_cellData.txt";
    QSystemTrayIcon *trayIcon;
    QSet<QString> notifiedEventsToday;
    QDate lastCheckedDate;
};

class NoteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NoteDialog(QWidget *parent = nullptr);
    void setCourseName(const QString& itemName);
    void setLocation(const QString& location);
    void setRemark(const QString& remark);
    QString getLocation() const;
    QString getRemark() const;

private:
    QLabel* itemLabel;
    QLineEdit* editLocationLine;
    QTextEdit* editRemarkText;
    QPushButton* saveButton;
    QVBoxLayout* layout;
    void applyDialogStyle(int index);
};











#endif // TIMETABLE_H
