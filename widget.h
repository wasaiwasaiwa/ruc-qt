#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "t1.h"
#include "t2.h"
#include "t3.h"
#include "t4.h"
#include "t5.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void setsingleitem();
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();
    void on_cb1_currentTextChanged(const QString &arg1);


    void on_pushButton_5_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_cb1_2_currentTextChanged(const QString &arg1);

    void on_pushButton_6_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
