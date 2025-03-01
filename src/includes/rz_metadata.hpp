/**
 * @file rz_metadata.hpp
 * @author ZHENG Robert (robert.hase-zheng.net)
 * @brief Header file for the default metadata
 * @version 0.1
 * @date 2025-03-01
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 */
#pragma once

#include <QHash>
#include <QString>

namespace rz_metaDefaultData
{

    struct defaultMetaStruct
    {
        QHash<QString, QString> xmpDefault{};
        QHash<QString, QString> exifDefault{};
    };

    const QHash<QString, QString> xmpDefaultTags{
        {"Xmp.dc.rights",
         "lang=\"x-default\" local language Copyright Notice, lang=\"en\" Copyright Notice"},
        {"Xmp.dc.CopyrightOwner", "Copyright Owner"},
        {"Xmp.dc.Category",
         "comma separated\nSupplemental categories further refine the subject of an object data."
         "\n"
         "Asia, China, Yunnan, Kunming"},
        {"Xmp.dc.subject", "comma separated\nUsed to indicate specific information retrieval words."},
        {"Xmp.dc.SecurityClassification",
         "comma separated\nSecurity classification assigned to the image.\n\"public \", \"private\", "
         "\"comment_on\", \"rating_on\""}};

    const QHash<QString, QString> exifDefaultTags{
        {"Exif.Image.GPSTag", "don't modify manually untill you're knowing what you're doing."},
        {"Exif.GPSInfo.GPSLatitudeRef",
         "don't modify manually untill you're knowing what you're doing."},
        {"Exif.GPSInfo.GPSLatitude", "don't modify manually untill you're knowing what you're doing."},
        {"Exif.GPSInfo.GPSLongitudeRef",
         "don't modify manually untill you're knowing what you're doing."},
        {"Exif.GPSInfo.GPSLongitude", "don't modify manually untill you're knowing what you're doing."},
        {"Exif.GPSInfo.GPSAltitudeRef",
         "don't modify manually untill you're knowing what you're doing."},
        {"Exif.GPSInfo.GPSAltitude", "don't modify manually untill you're knowing what you're doing."},
        {"Exif.GPSInfo.GPSTimeStamp", "don't modify manually untill you're knowing what you're doing."},
        {"Exif.GPSInfo.GPSMapDatum", "don't modify manually untill you're knowing what you're doing."},
        {"Exif.GPSInfo.GPSDateStamp", "don't modify manually untill you're knowing what you're doing."},
        {"Exif.Photo.DateTimeOriginal",
         "don't modify manually untill you're knowing what you're doing."}};

} // namespace rz_metaDefaultData
