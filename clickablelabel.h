#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QWidget>
<<<<<<< Updated upstream

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(const QString &text, QWidget *parent = nullptr);
    ~ClickableLabel();
=======
#include <QMouseEvent>

class clickableLabel : public QLabel {
    Q_OBJECT
public:
    explicit clickableLabel(QWidget *parent = nullptr);
    clickableLabel(const QString &text, QWidget *parent) ;
    ~clickableLabel();
>>>>>>> Stashed changes

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
<<<<<<< Updated upstream
};

#endif // CLICKABLELABEL_H
=======

};

#endif // CLICKABLELABEL_H

>>>>>>> Stashed changes
