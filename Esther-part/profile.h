#ifndef PROFILE_H
#define PROFILE_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>
#include <QComboBox>

namespace Ui {
class Profile;
}

class Profile : public QWidget
{
    Q_OBJECT

public:
    explicit Profile(QWidget *parent = nullptr);
    ~Profile();

private slots:
    void on_returnButton_clicked();
    void on_editPicButton_clicked();
    void openEditDialog();

private:
    Ui::Profile *ui;
    void loadData(const QString& filename);
    void saveData(const QString& filename);
    void displayLeftPanel();
    void displayRightPanel();
    void populateCalendar();
    void updateProfilePic(const QString& filename);
    void updateFlagLabel();
    void setLabelText();
    void applyStyle();
    QString savedUserInfoFile = "saved_user_info.txt";
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
