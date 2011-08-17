#include "uiwidget.h"
#include "ui_widget.h"

UIWidget::UIWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

UIWidget::~UIWidget()
{
    delete ui;
}

void UIWidget::on_pushButton_clicked()
{

}

void UIWidget::on_pushButton_2_clicked()
{

}

void UIWidget::on_pushButton_3_clicked()
{

}

void UIWidget::on_pushButton_4_clicked()
{

}

void UIWidget::on_dial_2_valueChanged(int value)
{

}

void UIWidget::on_pushButton_5_toggled(bool checked)
{

}
