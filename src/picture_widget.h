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

    void _on_resizeSmallerImage_Btn_clicked();
    void _on_resizeBiggerImage_Btn_clicked();

    void on_exifTableWidget_itemDoubleClicked(QTableWidgetItem *item);
    void on_iptcTableWidget_itemDoubleClicked(QTableWidgetItem *item);
    void on_xmpTableWidget_itemDoubleClicked(QTableWidgetItem *item);

    void rotateSrcImg(int val);
    void exportSrcImgToWebpThread();
    void exportSrcImgToWebP(int size);

    void on_tabWidget_tabBarClicked(int index);

private:
    Ui::PictureWidget *ui;

    void createRotateMenu();
    QMenu *rotateMnu;
    QAction *rotate_90;
    QAction *rotate_m90;
    QAction *rotate_120;
    QAction *rotate_m120;

    void createExportMenu();
    QMenu *exportMnu;
    QAction *webp_oversizeAct;
    QAction *webp_overwriteWebpAct;
    QAction *webp_watermarkWebpAct;
    QAction *webp_size_all;
    QAction *webp_size_480;
    QAction *webp_size_640;
    QAction *webp_size_800;
    QAction *webp_size_1024;
    QAction *webp_size_1280;

    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);

    void resizePicture();

    bool dataModified_bool{false};
    QString pathToImage;
    QPixmap *picture;

    const QList<QString> validMetaImageTypes = {"jpg", "jpeg", "png", "webp", "tiff"};
    bool checkValidMetaImg();
    void disableMetaTabs(int tab);
    const void readSrcExif();
    void markIptc(QString searchFor);
    void markIptcCopyrightCell();
    void markExif(QString searchFor);
    const void readSrcIptc();

    const void readSrcXmp();
    void markXmp(QString searchFor);

    const QMap<QString, QString> xmpMetaTags{
        {"Xmp.dc.DocumentName", "the original document name."},
        {"Xmp.dc.ImageID",
         "full pathname of the original, high-resolution image,"
         "\n"
         "or any other uniquely identifying string."},
        {"Xmp.dc.Title", "lang=\"en\" "},
        {"Xmp.dc.Subject", "lang=\"en\" "},
        {"Xmp.dc.Description", "lang=\"en\" "},
        {"Xmp.dc.Rights", "Copyright Notice"},
        {"Xmp.dc.CopyrightOwner", "Copyright Owner"},
        {"Xmp.plus.CopyrightOwner", "Copyright Owner"},
        {"Xmp.dc.CountryCode",
         "Indicates the code of the country/primary location where the intellectual property of "
         "the object data was created,"
         "\n"
         "e.g. a photo was taken, an event occurred."
         "\n"
         "Where ISO has established an appropriate country code under ISO 3166."},
        {"Xmp.dc.CountryName",
         "lang=\"en\" \n"
         "Provides full, publishable, name of the country/primary location where the intellectual "
         "property of the object data was created."},
        {"Xmp.dc.ProvinceState",
         "lang=\"en\" \n"
         "Identifies Province/State of origin."},
        {"Xmp.dc.City", "lang=\"en\" "},
        {"Xmp.dc.SubLocation",
         "lang=\"en\" \n"
         "Identifies the location within a city from which the object data originates."},
        {"Xmp.dc.ZipCode", ""},
        {"Xmp.dc.StreetName", "lang=\"en\" "},
        {"Xmp.dc.LocalAddress", "address in local language and format."},
        {"Xmp.dc.Language",
         "Describes the major national language of the object, according to the 2-letter codes of "
         "ISO 639:1988."
         "\n"
         "Does not define or imply any coded character set."},
        {"Xmp.dc.Category", "Supplemental categories further refine the subject of an object data."},
        {"Xmp.dc.Keywords", "Used to indicate specific information retrieval words."},
        {"Xmp.dc.SecurityClassification", "Security classification assigned to the image."}};

    const QMap<QString, QString> exifMetaTags{
        {"Exif.Image.DocumentName",
         "The name of the document from which this image was scanned."
         "\n"
         "(Ascii)"},
        {"Exif.Image.ImageDescription",
         "A character string giving the title of the image."
         "\n"
         "It may be a comment such as '1988 company picnic' or the like."
         "\n"
         "Two-bytes character codes cannot be used."
         "\n"
         "When a 2-bytes code is necessary, the Exif Private tag <UserComment> is to be used."
         "\n"
         "(Ascii)"},
        {"Exif.Image.ImageID",
         "ImageID is the full pathname of the original, high-resolution image,"
         "\n"
         "or any other identifying string that uniquely identifies the original image (Adobe OPI)."
         "\n"
         "(Ascii)"},
        {"Exif.Image.Copyright",
         "Copyright information."
         "\n"
         "In this standard the tag is used to indicate both the photographer and editor copyrights."
         "\n"
         "It is the copyright notice of the person or organization claiming rights to the image."
         "\n"
         "The Interoperability copyright statement including date and rights should be written in "
         "this field; e.g.:"
         "\n"
         "'Copyright, John Smith, 19xx. All rights reserved.'."
         "\n"
         "(Ascii)"},
        {"Exif.Image.SecurityClassification",
         "Security classification assigned to the image."
         "\n"
         "(Ascii)"},
        {"Exif.Photo.UserComment",
         "A tag for Exif users to write keywords or comments on the image"
         "\n"
         "besides those in <ImageDescription>, and without the character code limitations of the "
         "<ImageDescription> tag."
         "\n"
         "(Comment)"},
        {"Exif.Photo.ImageUniqueID",
         "This tag indicates an identifier assigned uniquely to each image."
         "\n"
         "It is recorded as an ASCII string equivalent to hexadecimal notation and 128-bit fixed "
         "length."
         "\n"
         "(Ascii)"},
        {"Exif.Image.GPSTag", ""},
        {"Exif.GPSInfo.GPSLatitudeRef", ""},
        {"Exif.GPSInfo.GPSLatitude", ""},
        {"Exif.GPSInfo.GPSLongitudeRef", ""},
        {"Exif.GPSInfo.GPSLongitude", ""},
        {"Exif.GPSInfo.GPSAltitudeRef", ""},
        {"Exif.GPSInfo.GPSAltitude", ""},
        {"Exif.GPSInfo.GPSTimeStamp", ""},
        {"Exif.GPSInfo.GPSMapDatum", ""},
        {"Exif.GPSInfo.GPSDateStamp", ""},
        {"Exif.Photo.DateTimeOriginal", ""}};

    const QMap<QString, QString> iptcMetaTags{{"Iptc.Application2.ObjectName",
                                               "Used as a shorthand reference for the object."
                                               "\n"
                                               "(64B)"},
                                              {"Iptc.Application2.Copyright",
                                               "Contains any necessary copyright notice."
                                               "\n"
                                               "(128B)"},
                                              {"Iptc.Application2.Caption",
                                               "A textual description of the object data."
                                               "\n"
                                               "(2000B)"}};
};
