#include "clickablelabel.h"
#include <QMouseEvent>

ClickableLabel::ClickableLabel(QWidget* parent) : QLabel(parent) {}

ClickableLabel::ClickableLabel(const QString &text, QWidget *parent) : QLabel(parent) {
    setText(text);
    setStyleSheet("font-family: monospace; font-weight: bold; text-decoration: underline; color: #00B7FF;");
    setCursor(Qt::PointingHandCursor);
}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    emit clicked();
    QLabel::mousePressEvent(event);
}


