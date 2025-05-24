#ifndef ENGLISHOVERLAY_H
#define ENGLISHOVERLAY_H

#include <QWidget>

namespace Ui {
class englishoverlay;
}

class englishoverlay : public QWidget
{
    Q_OBJECT

public:
    explicit englishoverlay(QWidget *parent = nullptr);
    ~englishoverlay();

    void setTasks(const QStringList &tasks);
    void setName(const QString &name);
    void setTasksWithState(const QList<QPair<QString, bool>> &taskList);


private:
    Ui::englishoverlay *ui;
};

#endif // ENGLISHOVERLAY_H
