#pragma once

#include <QCloseEvent>
#include <QResizeEvent>
#include <QTableWidgetItem>
#include <QWidget>

#include "includes/rz_metadata.hpp"
#include <memory>

namespace Ui {
class DefaultMeta;
}

class DefaultMeta : public QWidget
{
    Q_OBJECT

public:
    explicit DefaultMeta(QWidget *parent = nullptr);
    ~DefaultMeta();

    void setDefaultMeta(rz_metaDefaultData::defaultMetaStruct &_defaultMetaPtr);

private slots:
    void on_close_Btn_clicked();
    void on_save_Btn_clicked();
    void on_tableWidget_itemDoubleClicked(QTableWidgetItem *item);
    void on_exif_tableWidget_itemDoubleClicked(QTableWidgetItem *item);

private:
    Ui::DefaultMeta *ui;

    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
    void createXmpTableWidget();
    void createExifTableWidget();

    bool dataModified_bool{false};
    rz_metaDefaultData::defaultMetaStruct *defaultMetaPtr;
    void markXmp(QString searchFor);
    void markExif(QString searchFor);
};
