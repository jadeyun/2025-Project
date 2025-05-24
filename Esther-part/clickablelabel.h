#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>

class clickableLabel : public QLabel {
    Q_OBJECT
public:
    explicit clickableLabel(QWidget *parent = nullptr);
    ~clickableLabel();

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;

};

#endif // CLICKABLELABEL_H

