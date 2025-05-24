#ifndef PROFILE_H
#define PROFILE_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDialog>
#include <QComboBox>
#include "mainwindow.h"


namespace Ui {
class Profile;
}

class MainWindow;

class Profile : public QWidget
{
    Q_OBJECT

public:
    explicit Profile(MainWindow *parentWindow ,QWidget *parent = nullptr);
    // explicit Profile(bool doTaskToday = false,QWidget *parent = nullptr);
    bool doTaskToday = false;
    void displayRightPanel();
    void applyStyle();


    ~Profile();

    void markTodayDone();

private slots:
    void on_returnButton_clicked();
    void on_editPicButton_clicked();
    void openEditDialog();

private:
    Ui::Profile *ui;
    MainWindow *mainWindow;
    void loadData(const QString& filename);
    void saveData(const QString& filename);
    void displayLeftPanel();
    // void displayRightPanel(bool doTaskToday);
    void populateCalendar();
    void clearCalendar();
    void updateProfilePic(const QString& filename);
    void setLabelText();
    // void applyStyle();
    void updateFireLabel();
    QString savedUserInfoFile = "saved_user_info.txt";
    QGridLayout* calendarLayout;
    QLabel *fireTextLabel;
    QLabel *fireGifLabel;
    QVBoxLayout *fireLayout;
};

class EditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditDialog(QWidget *parent = nullptr);
    QString getNickname() const;
    QString getName() const;
    QString getCountry() const;
    QString getEmailAddress() const;
    QString getPersonalBio() const;

private:
    QLineEdit* editNicknameLine;
    QLineEdit* editNameLine;
    QComboBox* countryComboBox;
    QLineEdit* editEmailAddressLine;
    QTextEdit* editPersonalBioLine;
    QPushButton* saveButton;
    QPushButton* cancelButton;
    QVBoxLayout* layout;
    QHBoxLayout* buttonLayout;
    void applyDialogStyle();
    void loadData(const QString& filename);
};

#endif // PROFILE_H
