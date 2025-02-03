#pragma once

#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QList>
#include <QMatrix2x2>
#include <QPainter>
#include <QString>

//#include "rz_metadata.hpp"
#include <exiv2/exiv2.hpp>

struct imageStruct
{
    QString fileName{""};       // 2014-04-18_203353.jpg
    QString fileBasename{""};   // 2014-04-18_203353
    QString fileSuffix{""};     // jpg
    QString fileAbolutePath{""}; // /home/zb_bamboo/DEV/__NEW__/CPP/qt_convert_image/images
    Exiv2::ExifData exifData;
    QString newFolder{"WebP"};
    QString newSuffix{"webp"};
    QList<int> webpSizes = {480, 680, 800, 1024, 1280};
};

class Photo
{
private:
    const QList<QString> validMetaImageTypes = {"jpg", "jpeg", "png", "webp", "tiff"};
    bool isValidMetaImageType();

    imageStruct imgStruct;
    void setImageStruct(const QString &imageInput);

    bool oversizeSmallerPicture_bool{false};
    bool overwriteExitingWebp_bool{false};
    bool watermarkWebp_bool{true};
    bool createWebpPath();
    bool checkImgTargetExists(const QFile &pathToTargetImage);
    bool checkImgWidth(const QImage &imageInput, const int &targetWidth);

    bool backupOrigFile();

    void readExif();

public:
    Photo();
    Photo(const QString &imageInput);
    ~Photo();

    bool convertImage(const int &targetSize, const int &quality = 75);
    const bool convertImages(const int &quality = 75);
    bool rotateImage(const int &turn);
    QList<int> getWebSizes();

    QString getSuffix();
    void setOversizeSmallerPicture(const bool oversizeSmallerPicture = false);
    void setOverwriteExistingWebp(const bool overwriteExitingWebp = false);
    void setWatermarkWebp(const bool watermarkWebp = true);

    QList<QString> srcPics(const QString &srcPath);
    QList<QString> srcPicsRecursive(const QString &srcPath);

    bool writeXmp(QString &key, QString &value);
    bool writeExif(QString &key, QString &value);
    bool writeIptc(QString &key, QString &value);

    QString getXmpCopyrightOwner();
    bool writeToAllCopyrightOwner(const QString &owner);

    struct metaCopyrightStruct
    {
        QString xmp{"Xmp.dc.CopyrightOwner"};
        QString exif = "Exif.Image.Copyright";
        QString iptc = "Iptc.Application2.Copyright";
    };

    struct exifGpsStruct
    {
        QString GPSLatitudeRef{""};
        QString GPSLatitude{""};
        QString GPSLongitudeRef{""};
        QString GPSLongitude{""};
        QString GPSAltitudeRef{""};
        QString GPSAltitude{""};
    };
    exifGpsStruct getGpsData() const;
    void writeDefaultGpsData(const exifGpsStruct &gpsData);

public:
    const QHash<QString, QString> exif_to_xmp = {{"Exif.Image.Copyright", "Xmp.dc.CopyrightOwner"},
                                                 {"Exif.Image.DocumentName", "Xmp.dc.DocumentName"},
                                                 {"Exif.Image.ImageDescription",
                                                  "Xmp.dc.description"},
                                                 {"Exif.Image.ImageID", "Xmp.dc.ImageID"},
                                                 {"Exif.Image.SecurityClassification",
                                                  "Xmp.dc.SecurityClassification"},
                                                 {"Exif.Photo.UserComment", "Xmp.dc.subject"}};

    const QHash<QString, QString> exif_to_iptc = {
        {"Exif.Image.Copyright", "Iptc.Application2.Copyright"},
        {"Exif.Image.DocumentName", "Iptc.Application2.ObjectName"},
        {"Exif.Image.ImageDescription", "Iptc.Application2.Caption"},
    };

    const QHash<QString, QString> xmp_to_exif = {{"Xmp.dc.CopyrightOwner", "Exif.Image.Copyright"},
                                                 {"Xmp.dc.DocumentName", "Exif.Image.DocumentName"},
                                                 {"Xmp.dc.description",
                                                  "Exif.Image.ImageDescription"},
                                                 {"Xmp.dc.ImageID", "Exif.Image.ImageID"},
                                                 {"Xmp.dc.SecurityClassification",
                                                  "Exif.Image.SecurityClassification"},
                                                 {"Xmp.dc.subject", "Exif.Photo.UserComment"}};

    const QHash<QString, QString> xmp_to_iptc
        = {{"Xmp.dc.description", "Iptc.Application2.Caption"},
           {"Xmp.dc.CopyrightOwner", "Iptc.Application2.Copyright"},
           {"Xmp.dc.DocumentName", "Iptc.Application2.ObjectName"}};

    const QHash<QString, QString> iptc_to_xmp
        = {{"Iptc.Application2.Caption", "Xmp.dc.description"},
           {"Iptc.Application2.Copyright", "Xmp.dc.CopyrightOwner"},
           {"Iptc.Application2.ObjectName", "Xmp.dc.DocumentName"}};

    const QHash<QString, QString> iptc_to_exif
        = {{"Iptc.Application2.Caption", "Exif.Image.ImageDescription"},
           {"Iptc.Application2.Copyright", "Exif.Image.Copyright"},
           {"Iptc.Application2.ObjectName", "Exif.Image.DocumentName"}};

    // Meta attributes-values
    const QHash<QString, QString> xmpMetaTags{
        {"Xmp.dc.DocumentName", "the original document name."},
        {"Xmp.dc.ImageID",
         "full pathname of the original, high-resolution image,"
         "\n"
         "or any other uniquely identifying string."},
        {"Xmp.dc.title", "lang=\"en\" "},
        {"Xmp.dc.subject", "lang=\"en\" "},
        {"Xmp.dc.description", "lang=\"en\" "},
        {"Xmp.dc.rights", "lang=\"en\" Copyright Notice"},
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
         "Provides full, publishable, name of the country/primary location where the intellectual "
         "property of the object data was created."},
        {"Xmp.dc.ProvinceState", "Identifies Province/State of origin."},
        {"Xmp.dc.City", "City of Origin"},
        {"Xmp.dc.SubLocation",
         "sublocation of origin"
         "\n"
         "Identifies the location within a city from which the object data originates."},
        {"Xmp.dc.ZipCode", ""},
        {"Xmp.dc.StreetName", "street of origin"},
        {"Xmp.dc.LocalAddress", "address in local language and format."},
        {"Xmp.dc.Language",
         "Describes the major national language of the object, according to the 2-letter codes of "
         "ISO 639:1988."
         "\n"
         "Does not define or imply any coded character set."},
        {"Xmp.dc.Category", "Supplemental categories further refine the subject of an object data."},
        {"Xmp.dc.Keywords", "Used to indicate specific information retrieval words."},
        {"Xmp.dc.SecurityClassification", "Security classification assigned to the image."}};

    const QHash<QString, QString> exifGpsTags{{"Exif.Image.GPSTag", ""},
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
    const QHash<QString, QString> exifMetaTags{
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

    const QHash<QString, QString> iptcMetaTags{{"Iptc.Application2.ObjectName",
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
