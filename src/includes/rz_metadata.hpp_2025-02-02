#pragma once

#include <QDebug>

#include <exiv2/exiv2.hpp>

#include <QMap>
#include <string>

namespace rz_metaData {

// Meta mappings
const QMap<QString, QString> exif_to_xmp = {{"Exif.Image.Copyright", "Xmp.dc.CopyrightOwner"},
                                            {"Exif.Image.DocumentName", "Xmp.dc.DocumentName"},
                                            {"Exif.Image.ImageDescription", "Xmp.dc.Description"},
                                            {"Exif.Image.ImageID", "Xmp.dc.ImageID"},
                                            {"Exif.Image.SecurityClassification",
                                             "Xmp.dc.SecurityClassification"},
                                            {"Exif.Photo.UserComment", "Xmp.dc.Subject"}};

const QMap<QString, QString> exif_to_iptc = {
    {"Exif.Image.Copyright", "Iptc.Application2.Copyright"},
    {"Exif.Image.DocumentName", "Iptc.Application2.ObjectName"},
    {"Exif.Image.ImageDescription", "Iptc.Application2.Caption"},
};

const QMap<QString, QString> xmp_to_exif = {{"Xmp.dc.CopyrightOwner", "Exif.Image.Copyright"},
                                            {"Xmp.dc.DocumentName", "Exif.Image.DocumentName"},
                                            {"Xmp.dc.Description", "Exif.Image.ImageDescription"},
                                            {"Xmp.dc.ImageID", "Exif.Image.ImageID"},
                                            {"Xmp.dc.SecurityClassification",
                                             "Exif.Image.SecurityClassification"},
                                            {"Xmp.dc.Subject", "Exif.Photo.UserComment"}};

const QMap<QString, QString> xmp_to_iptc = {{"Xmp.dc.Description", "Iptc.Application2.Caption"},
                                            {"Xmp.dc.CopyrightOwner", "Iptc.Application2.Copyright"},
                                            {"Xmp.dc.DocumentName", "Iptc.Application2.ObjectName"}};

const QMap<QString, QString> iptc_to_xmp = {{"Iptc.Application2.Caption", "Xmp.dc.Description"},
                                            {"Iptc.Application2.Copyright", "Xmp.dc.CopyrightOwner"},
                                            {"Iptc.Application2.ObjectName", "Xmp.dc.DocumentName"}};

const QMap<QString, QString> iptc_to_exif
    = {{"Iptc.Application2.Caption", "Exif.Image.ImageDescription"},
       {"Iptc.Application2.Copyright", "Exif.Image.Copyright"},
       {"Iptc.Application2.ObjectName", "Exif.Image.DocumentName"}};

// Meta attributes-values
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

bool writeXmp(QString pathToFile, QString key, QString value)
{
    //qDebug() << "writeXmp: " << pathToFile << " " << key << " " << value;
    Exiv2::XmpParser::initialize();
    ::atexit(Exiv2::XmpParser::terminate);
#ifdef EXV_ENABLE_BMFF
    Exiv2::enableBMFF();
#endif
    try {
        auto xmp_image = Exiv2::ImageFactory::open(pathToFile.toStdString());
        xmp_image->readMetadata();
        Exiv2::XmpData &xmpData = xmp_image->xmpData();
        xmpData[key.toStdString()] = value.toStdString();
        xmp_image->setXmpData(xmpData);
        xmp_image->writeMetadata();
        //qDebug() << "writeXmp: write OK";
        return true;
    } catch (...) {
        qCritical() << "writeXmp: write NOK";
        return false;
    }
}

bool writeExif(QString pathToFile, QString key, QString value)
{
    //qDebug() << "writeExif: " << pathToFile << " " << key << " " << value;
    Exiv2::XmpParser::initialize();
    ::atexit(Exiv2::XmpParser::terminate);
#ifdef EXV_ENABLE_BMFF
    Exiv2::enableBMFF();
#endif
    try {
        Exiv2::XmpParser::initialize();
        ::atexit(Exiv2::XmpParser::terminate);

        auto exif_image = Exiv2::ImageFactory::open(pathToFile.toStdString());
        exif_image->readMetadata();
        Exiv2::ExifData &exifData = exif_image->exifData();
        exifData[key.toStdString()] = value.toStdString();

        /*
            auto v = Exiv2::Value::create(Exiv2::asciiString);
            qDebug() << "Set the value to a string";
            v->read(text.toStdString());
            qDebug() << "Add the value together with its key to the Exif data container";
            Exiv2::ExifKey key("Exif.Photo.ImageTitle");
            qDebug() << "add";
            exifData.add(key, v.get());
            */

        exif_image->setExifData(exifData);
        exif_image->writeMetadata();
        return true;
    } catch (Exiv2::Error &e) {
        qCritical() << "Caught Exiv2 exception " << e.what() << "\n";
        return false;
    }
}

bool writeIptc(QString pathToFile, QString key, QString value)
{
    //qDebug() << "writeIptc: " << pathToFile << " " << key << " " << value;

    Exiv2::XmpParser::initialize();
    ::atexit(Exiv2::XmpParser::terminate);
#ifdef EXV_ENABLE_BMFF
    Exiv2::enableBMFF();
#endif
    try {
        Exiv2::XmpParser::initialize();
        ::atexit(Exiv2::XmpParser::terminate);
        auto iptc_image = Exiv2::ImageFactory::open(pathToFile.toStdString());
        iptc_image->readMetadata();
        Exiv2::IptcData &iptcData = iptc_image->iptcData();
        iptcData[key.toStdString()] = value.toStdString();
        iptc_image->setIptcData(iptcData);
        iptc_image->writeMetadata();
        return true;
    } catch (Exiv2::Error &e) {
        qCritical() << "Caught Exiv2 exception " << e.what() << "\n";
        return false;
    }
}

} // namespace rz_metaData
