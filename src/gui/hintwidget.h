#ifndef HINTWIDGET_H
#define HINTWIDGET_H

#include <QLabel>
#include <QWidget>
#include <Qt>

class HintWidget : public QLabel
{ 
    Q_OBJECT 

public:
    explicit HintWidget(QWidget* parent = Q_NULLPTR, QString label = "", QString hint = "");
    ~HintWidget();

protected:
    void mouseMoveEvent(QMouseEvent* event);

private:
    QString hint;
};

#endif // HINTWIDGET_H
