#ifndef UIWIDGET__H
#define UIWIDGET__H

#include <QWidget>

namespace Ui {
    class Widget;
}

class UIWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UIWidget(QWidget *parent = 0);
    ~UIWidget();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_dial_2_valueChanged(int value);

    void on_pushButton_5_toggled(bool checked);

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
