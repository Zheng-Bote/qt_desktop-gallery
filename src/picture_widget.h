#pragma once

#include <QCloseEvent>
#include <QResizeEvent>
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
    void readSrcIptc();
};
