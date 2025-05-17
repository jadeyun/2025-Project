#ifndef PROGRAMMINGOVERLAY_H
#define PROGRAMMINGOVERLAY_H

#include <QWidget>

namespace Ui {
class programmingoverlay;
}

class programmingoverlay : public QWidget
{
    Q_OBJECT

public:
    explicit programmingoverlay(QWidget *parent = nullptr);
    ~programmingoverlay();

private:
    Ui::programmingoverlay *ui;
};

#endif // PROGRAMMINGOVERLAY_H
