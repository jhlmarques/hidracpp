#include "hintwidget.h"

#include <QToolTip>

HintWidget::HintWidget(QWidget* parent, QString label, QString hint)
    : QLabel(parent)
{
    this->setTextFormat(Qt::RichText);
    this->setStyleSheet("QLabel { color: rgb(128, 128, 128); }");
    this->setText(label);
    this->hint = hint;
}

HintWidget::~HintWidget() {}

void HintWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPoint point = this->mapToGlobal(QPoint(this->width()/2, this->height()));
    QRect rect = QRect(this->x(), this->y(), this->width(), this->height());
    QToolTip::showText(point, this->hint, this, rect);
}
