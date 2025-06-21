// #include "clickablelabel.h"
// <<<<<<< Updated upstream
// #include <QMouseEvent>

// ClickableLabel::ClickableLabel(const QString &text, QWidget *parent) : QLabel(parent) {
// =======

// clickableLabel::clickableLabel(QWidget* parent) : QLabel(parent) {}

// clickableLabel::clickableLabel(const QString &text, QWidget *parent) : QLabel(parent) {
// >>>>>>> Stashed changes
//     setText(text);
//     setStyleSheet("font-family: monospace; font-weight: bold; text-decoration: underline; color: #00B7FF;");
//     setCursor(Qt::PointingHandCursor);
// }

// <<<<<<< Updated upstream
// ClickableLabel::~ClickableLabel() {}

// void ClickableLabel::mousePressEvent(QMouseEvent *event) {
//     if (event->button() == Qt::LeftButton) {
//         emit clicked();
//     }
//     QLabel::mousePressEvent(event);
// }
// =======
// clickableLabel::~clickableLabel() {}

// void clickableLabel::mousePressEvent(QMouseEvent* event) {
//     emit clicked();
//     QLabel::mousePressEvent(event);
// }

// >>>>>>> Stashed changes


#include "clickablelabel.h"
#include <QMouseEvent>

ClickableLabel::ClickableLabel(QWidget* parent) : QLabel(parent) {}

ClickableLabel::ClickableLabel(const QString &text, QWidget *parent) : QLabel(parent) {
    setText(text);
    setStyleSheet("font-family: monospace; font-weight: bold; text-decoration: underline; color: #00B7FF;");
    setCursor(Qt::PointingHandCursor);
}

ClickableLabel::~ClickableLabel() {}

// void ClickableLabel::mousePressEvent(QMouseEvent *event) {
//     if (event->button() == Qt::LeftButton) {
//         emit clicked();
//     }
//     QLabel::mousePressEvent(event); // keep the base class behavior
// }


void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    emit clicked();
    QLabel::mousePressEvent(event);
}


