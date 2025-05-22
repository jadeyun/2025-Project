#include "profile.h"
#include "ui_profile.h"
#include <QDate>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QDialog>
#include <QPixmap>
#include <QBitmap>
#include <QPainter>
#include <QPropertyAnimation>

struct UserInfo {
    QString nickname, name, country, emailAddress, personalBio, profilePicPath;
    UserInfo& operator() (QString v1, QString v2, QString v3, QString v4, QString v5, QString v6) { nickname = v1; name = v2; country = v3; emailAddress = v4; personalBio = v5; profilePicPath = v6; return *this; }
};
UserInfo user;

struct StreakDetails {
    int streakCount = 0, bestStreak = 0;
    QString month;
    QString weeklyStreak; // Save as 1 or 0 for SMTWTFS. Eg.: 0101000.
    QString calendarStreak; // Similar as above.
};
StreakDetails userStreak;

Profile::Profile(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Profile)
{
    ui->setupUi(this);
    ui->bigLayout->setContentsMargins(30, 30, 30, 30);
    if (QFile::exists("saved_user_info.txt")) {
        savedUserInfoFile = "saved_user_info.txt";
        loadData(savedUserInfoFile);
    }
    connect(ui->editProfileButton, &QPushButton::clicked, this, &Profile::openEditDialog);
    displayLeftPanel();
    displayRightPanel();
    applyStyle();
}

Profile::~Profile()
{
    delete ui;
}

void Profile::loadData(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file.";
        return;
    }
    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        if (line.startsWith("UserInfo:")) {
            QStringList values = line.section(":", 1).split('|');
            if (values.size() == 6) { user(values[0], values[1], values[2], values[3], values[4], values[5]); }
        }
        else if (line.startsWith("StreakCount:")) {
            userStreak.streakCount = line.section(":", 1).toInt();
        }
        else if (line.startsWith("BestStreak:")) {
            userStreak.bestStreak = line.section(":", 1).toInt();
        }
        else if (line.startsWith("Month:")) {
            userStreak.month = line.section(":", 1).trimmed();
        }
        else if (line.startsWith("Week:")) {
            userStreak.weeklyStreak = line.section(":", 1).trimmed();
        }
        else if (line.startsWith("Calendar:")) {
            userStreak.calendarStreak = line.section(":", 1).trimmed();
        }
    }
    file.close();

    // Reset.
    if (QDate::currentDate().dayOfWeek() == 7 || userStreak.weeklyStreak.isEmpty()) {
        userStreak.weeklyStreak = QString(7, '0'); // Reset if today is Sunday.
    }
    QString currentMonth = QDate::currentDate().toString("yyyy-MM");
    if (userStreak.month != currentMonth) {
        userStreak.month = currentMonth;
        userStreak.calendarStreak = QString(QDate::currentDate().daysInMonth(), '0'); // Clear.
    }
}
void Profile::saveData(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for writing.";
        return;
    }
    QTextStream out(&file);
    out << "UserInfo:" << user.nickname << "|" << user.name << "|" << user.country << "|" << user.emailAddress << "|" << user.personalBio << "|" << user.profilePicPath << "\n";
    out << "StreakCount:" << userStreak.streakCount << "\n";
    out << "BestStreak:" << userStreak.bestStreak << "\n";
    out << "Month:" << userStreak.month << "\n";
    out << "Week:" << userStreak.weeklyStreak << "\n";
    out << "Calendar:" << userStreak.calendarStreak << "\n";
    file.close();
}
void Profile::displayLeftPanel()
{
    ui->bigLayout->setStretch(0, 1);
    ui->bigLayout->setStretch(1, 1);
    setLabelText();
    ui->leftLayout->setSpacing(2);
    this->setStyleSheet("background-color: #fff7ef");
    updateProfilePic(user.profilePicPath);
    ui->nicknameLabel->setFixedSize(120, 40);
    ui->emailAddressLabel->setFixedSize(255, 20);
    ui->personalBioLabel->setFixedSize(255, 120);
    updateFlagLabel();
}
void Profile::displayRightPanel()
{
    ui->bestStreakLabel->setText(userStreak.bestStreak + "天");
    ui->bestStreakLabel->setStyleSheet("font-size: 16px; font-weight: bold; font-family: \"Noto Sans SC\", \"Helvetica\"; color: #633a00;");
    QGraphicsDropShadowEffect* glowEffect = new QGraphicsDropShadowEffect();
    glowEffect->setBlurRadius(20);
    glowEffect->setOffset(0);
    glowEffect->setColor(QColor(255, 165, 0));
    ui->bestStreakLabel->setGraphicsEffect(glowEffect);
    QPropertyAnimation* animation = new QPropertyAnimation(glowEffect, "blurRadius");
    animation->setStartValue(10);
    animation->setEndValue(25);
    animation->setDuration(1000);
    animation->setLoopCount(-1);
    animation->setEasingCurve(QEasingCurve::InOutSine);
    animation->start();
    populateCalendar();

}
void Profile::populateCalendar()
{
    QGridLayout* calendarLayout = new QGridLayout(ui->calendarWidget);
    calendarLayout->setSpacing(2);
    QStringList daysInWeek = {"S", "M", "T", "W", "T", "F", "S"};
    for (int i = 0; i < 7; i++) {
        QLabel* dayLabel = new QLabel(daysInWeek[i]);
        dayLabel->setAlignment(Qt::AlignCenter);
        dayLabel->setStyleSheet("font-weight: bold;");
        calendarLayout->addWidget(dayLabel, 0, i);
    }
    QDate today = QDate::currentDate();
    QDate first = QDate(today.year(), today.month(), 1);
    int startCol = first.dayOfWeek() % 7;
    int totalDays = first.daysInMonth();
    QVector<int> streakDates;
    for (int i = 0; i < userStreak.calendarStreak.length(); i++) {
        if (userStreak.calendarStreak[i] == "1") {
            streakDates.push_back(i + 1);
        }
    }
    int row = 1, col = startCol;
    for (int i = 1; i <= totalDays; i++) {
        QLabel* dataLabel = new QLabel(QString::number(i));
        dataLabel->setAlignment(Qt::AlignCenter);
        dataLabel->setFixedSize(32, 32);
        if (streakDates.contains(i)) {
            dataLabel->setStyleSheet("background-color: orange; color: #633a00; border-radius: 6px;");
        }
        else {
            dataLabel->setStyleSheet("background-color: white; color: #633a00; border-radius: 6px;");
        }
        calendarLayout->addWidget(dataLabel, row, col);
        col++;
        if (col > 6) { col = 0; row++; }
    }
}
void Profile::updateProfilePic(const QString& filename)
{
    QPixmap pic(filename);
    pic = pic.scaled(200, 200, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QBitmap mask(200, 200); // Make round.
    mask.fill(Qt::color0);
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::color1);
    painter.drawEllipse(0, 0, 200, 200);
    pic.setMask(mask);
    ui->profilePic->setPixmap(pic);
    ui->profilePic->setFixedSize(200, 200);
    ui->profilePic->setStyleSheet("border: 2px solid #633a00; border-radius: 100px; padding: 0px;");
}
void Profile::updateFlagLabel()
{
    QString flagFile = QString("resources/cropped/%1.png").arg(user.country.toLower());
    QPixmap flagPixmap(flagFile);
    ui->flagLabel->setPixmap(flagPixmap.scaled(75, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->flagLabel->setFixedSize(75, 50);
    ui->flagLabel->setStyleSheet("border-radius: 6px; margin-left: 20px;"); // Flag is not done yet.
}
void Profile::openEditDialog()
{
    EditDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        user.nickname = dialog.getNickname();
        user.name = dialog.getName();
        user.country = dialog.getCountry();
        user.emailAddress = dialog.getEmailAddress();
        user.personalBio = dialog.getPersonalBio();
        setLabelText();
    }
}
void Profile::on_returnButton_clicked()
{
    saveData(savedUserInfoFile);
    this->hide();
}
void Profile::on_editPicButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Choose Profile Picture", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if (!filename.isEmpty()) {
        QPixmap testPic(filename);
        if (!testPic.isNull()) {
            user.profilePicPath = filename;
            updateProfilePic(user.profilePicPath);
        }
    }
}
void Profile::setLabelText()
{
    ui->nicknameLabel->setText(user.nickname);
    ui->nicknameLabel->setStyleSheet("font-size: 40px; font-weight: bold;");
    ui->emailAddressLabel->setText(user.emailAddress);
    ui->emailAddressLabel->setStyleSheet("font-size: 14px; color: gray;");
    ui->personalBioLabel->setText(user.personalBio);
    ui->personalBioLabel->setWordWrap(true);
    ui->innerLayout->setSpacing(5);
    ui->leftLayout->setContentsMargins(0, 0, 0, 0);
}

void Profile::applyStyle()
{
    QString style = R"(
        QPushButton { background-color: #fff4e6; border: 1px solid #633a00; padding: 8px 16px; font-size: 12px; border-radius: 6px; font-family: "Noto Sans SC", "Helvetica"; color: #633a00; }
        QPushButton::hover { background-color: #ffbf68; }
        QPushButton::pressed { background-color: #ffdfb3; } )";
    ui->returnButton->setStyleSheet(style);
    style = R"(
        QPushButton { background-color: #fff4e6; border: 1px solid #633a00; padding: 1px 2px; font-size: 12px; border-radius: 6px; font-family: "Noto Sans SC", "Helvetica"; color: #633a00; }
        QPushButton::hover { background-color: #ffbf68; }
        QPushButton::pressed { background-color: #ffdfb3; } )";
    ui->editButtonLayout->setSpacing(20);
    ui->editProfileButton->setStyleSheet(style);
    ui->editPicButton->setStyleSheet(style);
}


// For Edit Dialog:
EditDialog::EditDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Edit Profile");
    QLabel* titleLabel = new QLabel("修改个人信息");
    titleLabel->setObjectName("ProfileTitleLabel");
    editNicknameLine = new QLineEdit;
    editNameLine = new QLineEdit;
    countryComboBox = new QComboBox(this);
    countryComboBox->addItems({"Malaysia", "United States", "Canada", "South Korea", "China", "Kazakhstan"});
    int index = countryComboBox->findText(user.country);
    if (index != -1) {
        countryComboBox->setCurrentIndex(index);
    }
    editEmailAddressLine = new QLineEdit;
    editPersonalBioLine = new QTextEdit;
    editNicknameLine->setText(user.nickname);
    editNameLine->setText(user.name);
    editEmailAddressLine->setText(user.emailAddress);
    editPersonalBioLine->setText(user.personalBio);
    saveButton = new QPushButton("保存");
    cancelButton = new QPushButton("取消");
    layout = new QVBoxLayout;
    buttonLayout = new QHBoxLayout;
    layout->addWidget(titleLabel);
    layout->addWidget(new QLabel("昵称:"));
    layout->addWidget(editNicknameLine);
    layout->addWidget(new QLabel("姓名:"));
    layout->addWidget(editNameLine);
    layout->addWidget(new QLabel("国籍:"));
    layout->addWidget(countryComboBox);
    layout->addWidget(new QLabel("邮箱:"));
    layout->addWidget(editEmailAddressLine);
    layout->addWidget(new QLabel("个性签名:"));
    layout->addWidget(editPersonalBioLine);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);
    setLayout(layout);
    applyDialogStyle();
    connect(saveButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}
QString EditDialog::getNickname() const { return editNicknameLine->text(); }
QString EditDialog::getName() const { return editNameLine->text(); }
QString EditDialog::getCountry() const { return countryComboBox->currentText(); }
QString EditDialog::getEmailAddress() const { return editEmailAddressLine->text(); }
QString EditDialog::getPersonalBio() const { return editPersonalBioLine->toPlainText(); }
void EditDialog::applyDialogStyle() // UI: Esther. Below are extracted from timtable's dialog.
{
    QString style = R"(
        QLabel { background-color: #fff7ef; font-size: 14px; font-family: "Noto Sans SC", "Helvetica"; font-weight: bold; color: #444444; }
        QLabel#ProfileTitleLabel { background-color: #fff7ef; font-size: 16px; font-family: "Noto Sans SC", "Helvetica"; font-weight: bold; color: #444444; qproperty-alignment: AlignCenter; padding: 12px; border-bottom: 2px solid #c4a484; letter-spacing: 1px; }
        QDialog{ background-color: #fff7ef; border: 1px solid #ffbf68; padding: 8px 16px; font-size: 14px; border-radius: 6px; font-family: "Noto Sans SC", "Helvetica"; color: #633a00; }
        QLineEdit { background-color: #ffffff; border: 2px solid #d0d0d0; padding: 8px; font-size: 14px; border-radius: 10px; font-family: "Noto Sans SC", "Helvetica"; color: #633a00; }
        QLineEdit:focus { border: 2px solid #a67b5b; background-color: #fdf6ec; }
        QTextEdit{ background-color: #ffffff; border: 2px solid #d0d0d0; padding: 8px; font-size: 14px; border-radius: 10px; font-family: "Noto Sans SC", "Helvetica"; color: #633a00; }
        QTextEdit:focus { border: 2px solid #a67b5b; background-color: #fdf6ec; }
        QPushButton { background-color: #fff4e6; border: 1px solid #c4a484; padding: 8px 16px; font-size: 14px; border-radius: 12px; font-family: "Noto Sans SC", "Helvetica"; color: #633a00; }
        QPushButton::hover { background-color: #ffbf68; }
        QPushButton::pressed { background-color: #ffdfb3; }
        QComboBox { background-color: #ffffff; border: 2px solid #d0d0d0; padding: 8px; font-size: 14px; border-radius: 10px; font-family: "Noto Sans SC", "Helvetica"; color: #633a00; }
        QComboBox::focus { border: 2px solid #a67b5b; background-color: #fdf6ec; }
        QComboBox::drop-down { border: none; } )";
    buttonLayout->setSpacing(6);
    this->setStyleSheet(style);
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 80));
    this->setGraphicsEffect(shadow);
}
