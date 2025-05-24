#ifndef ACHIEVEMENTBOARD_H
#define ACHIEVEMENTBOARD_H

#include <QWidget>

namespace Ui {
class achievementboard;
}

class achievementboard : public QWidget
{
    Q_OBJECT

public:
    explicit achievementboard(QWidget *parent = nullptr);
    ~achievementboard();

private:
    Ui::achievementboard *ui;
    void loadTasks();

signals:
    void backToMain();


};

#endif // ACHIEVEMENTBOARD_H
