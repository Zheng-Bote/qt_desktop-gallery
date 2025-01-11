#pragma once

#include <QCloseEvent>
#include <QMenu>
#include <QResizeEvent>
#include <QTableWidgetItem>
#include <QWidget>
#include "includes/rz_photo.hpp"

namespace Ui {
class PictureWidget;
}

class PictureWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PictureWidget(QWidget *parent = nullptr);
    ~PictureWidget();

    void setImage(QString pathToFile);

private slots:
    void on_closeBtn_clicked();

    void on_resizeSmallerImage_Btn_clicked();
    void on_resizeBiggerImage_Btn_clicked();

    void on_exifTableWidget_itemDoubleClicked(QTableWidgetItem *item);
    void on_iptcTableWidget_itemDoubleClicked(QTableWidgetItem *item);

    void rotateSrcImg(int val);
    void exportSrcImgToWebP();
    void exportSrcImgToWebP(int size);

private:
    Ui::PictureWidget *ui;
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);

    void resizePicture();

    bool dataModified_bool{false};
    QString pathToImage;
    QPixmap *picture;

    const QList<QString> validMetaImageTypes = {"jpg", "jpeg", "png", "webp", "tiff"};
    bool checkValidMetaImg();
    void readSrcExif();
    void markExifCopyrightCell();
    void readSrcIptc();
    void markIptcCopyrightCell();

    void createRotateMenu();
    QMenu *rotateMnu;
    QAction *rotate_90;
    QAction *rotate_m90;
    QAction *rotate_120;
    QAction *rotate_m120;

    void createExportMenu();
    QMenu *exportMnu;
    QAction *webp_size_all;
    QAction *webp_size_480;
    QAction *webp_size_640;
    QAction *webp_size_800;
};
