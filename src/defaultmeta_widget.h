#pragma once

#include <QCloseEvent>
#include <QTableWidgetItem>
#include <QWidget>

namespace Ui {
class DefaultMeta;
}

class DefaultMeta : public QWidget
{
    Q_OBJECT

public:
    explicit DefaultMeta(QWidget *parent = nullptr);
    ~DefaultMeta();

private slots:
    void on_close_Btn_clicked();
    void on_save_Btn_clicked();
    void on_tableWidget_itemDoubleClicked(QTableWidgetItem *item);

private:
    Ui::DefaultMeta *ui;

    bool dataModified_bool{false};

    void closeEvent(QCloseEvent *event);
    void createTableWidget();
};
