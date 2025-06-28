#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include <QWidget>
#include <QVBoxLayout>

class HistoryWindow : public QWidget {
    Q_OBJECT

public:
    explicit HistoryWindow(QWidget *parent = nullptr);
    ~HistoryWindow();

    void setOverlay(QWidget *overlay) { this->overlay = overlay; } // Keep for compatibility

signals:
    void planSelected(const QString &planName, const QList<QPair<QString, bool>> &tasks);


private:
    QVBoxLayout *mainLayout;
    QWidget *overlay;

    void loadLog();
};

#endif // HISTORYWINDOW_H
