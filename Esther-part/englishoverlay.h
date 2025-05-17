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

private:
    Ui::englishoverlay *ui;
};

#endif // ENGLISHOVERLAY_H
