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
        {"Exif.Image.Artist",
         "This tag records the name of the camera owner, photographer or image creator."
         "\n"
         "The detailed format is not specified,"
         "\n"
         "but it is recommended that the information be written as in the example below for ease "
         "of Interoperability."
         "\n"
         "When the field is left blank, it is treated as unknown."
         "\n"
         "Example:"
         "\n"
         "'Camera owner, John Smith; "
         "Photographer, Michael "
         "Brown; Image creator, Ken James'"
         "\n"
         "(Ascii)"},
        {"Exif.Image.Rating",
         "Rating tag used by Windows."
         "\n"
         "(Short)"},
        {"Exif.Image.RatingPercent",
         "Rating tag used by Windows, value in percent."
         "\n"
         "(short)"},
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
         "(Ascii)"}};

    const QMap<QString, QString> iptcMetaTags{
        {"Iptc.Application2.ObjectName",
         "Used as a shorthand reference for the object."
         "\n"
         "(64B)"},
        {"Iptc.Application2.Subject",
         "The Subject Reference is a structured definition of the subject matter."
         "\n"
         "(236B)"},
        {"Iptc.Application2.Category",
         "Identifies the subject of the object data in the opinion of the provider."
         "\n"
         "A list of categories will be maintained by a regional registry, where available,"
         "\n"
         "otherwise by the provider."
         "\n"
         "(3B)"},
        {"Iptc.Application2.SuppCategory",
         "Supplemental categories further refine the subject of an object data."
         "A supplemental category may include any of the recognised categories as used in tag "
         "<Category>."
         "\n"
         "Otherwise, selection of supplemental categories are left to the provider."
         "\n"
         "(32B)"},
        {"Iptc.Application2.Keywords",
         "Used to indicate specific information retrieval words."
         "\n"
         "It is expected that a provider of various types of data that are related in subject "
         "matter uses the same keyword,"
         "\n"
         "enabling the receiving system or subsystems to search across all types of data for "
         "related material."
         "\n"
         "(64B)"},
        {"Iptc.Application2.LocationCode",
         "Indicates the code of a country/geographical location referenced by the content of the "
         "object."
         "\n"
         "Where ISO has established an appropriate country code under ISO 3166, that code will be "
         "used."
         "\n"
         "When ISO 3166 does not adequately provide for identification of a location or a country,"
         "\n"
         "e.g. ships at sea, space,"
         "\n"
         "IPTC will assign an appropriate three-character code under the provisions of ISO 3166 to "
         "avoid conflicts."
         "\n"
         "(3B)"},
        {"Iptc.Application2.LocationName",
         "Provides a full, publishable name of a country/geographical location referenced by the "
         "content of the object,"
         "\n"
         "according to guidelines of the provider."
         "\n"
         "(64B)"},
        {"Iptc.Application2.City",
         "Identifies city of object data origin according to guidelines established by the "
         "provider."
         "\n"
         "(32B)"},
        {"Iptc.Application2.SubLocation",
         "Identifies the location within a city from which the object data originates,"
         "\n"
         "according to guidelines established by the provider."
         "\n"
         "(32B)"},
        {"Iptc.Application2.ProvinceState",
         "Identifies Province/State of origin according to guidelines established by the provider."
         "\n"
         "(32B)"},
        {"Iptc.Application2.CountryCode",
         "Indicates the code of the country/primary location where the intellectual property of "
         "the object data was created,"
         "\n"
         "e.g. a photo was taken, an event occurred."
         "\n"
         "Where ISO has established an appropriate country code under ISO 3166,"
         "\n"
         "that code will be used. When ISO 3166 does not adequately provide for identification of "
         "a location or a new country,"
         "\n"
         "e.g. ships at sea, space,"
         "\n"
         "IPTC will assign an appropriate three-character code under the "
         "provisions of ISO 3166 to avoid conflicts."
         "\n"
         "(3B)"},
        {"Iptc.Application2.CountryName",
         "Provides full, publishable, name of the country/primary location"
         "\n"
         "where the intellectual property of the object data was created,"
         "\n"
         "according to guidelines of the provider."
         "\n"
         "(64B)"},
        {"Iptc.Application2.Headline",
         "A publishable entry providing a synopsis of the contents of the object data."
         "\n"
         "(256B)"},
        {"Iptc.Application2.Copyright",
         "Contains any necessary copyright notice."
         "\n"
         "(128B)"},
        {"Iptc.Application2.Caption",
         "A textual description of the object data."
         "\n"
         "(2000B)"},
        {"Iptc.Application2.Language",
         "Describes the major national language of the object, according to the 2-letter codes of "
         "ISO 639:1988."
         "\n"
         "Does not define or imply any coded character set, but is used for internal routing,"
         "\n"
         "e.g. to various editorial desks."
         "\n"
         "(3B)"}};
};
