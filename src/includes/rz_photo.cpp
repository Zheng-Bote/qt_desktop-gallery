/**
 * @file rz_photo.cpp
 * @author ZHENG Robert (robert.hase-zheng.net)
 * @brief Main class for the photo handling
 * @version 0.1
 * @date 2025-03-01
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 */
#include "rz_photo.hpp"

#include <chrono>
#include <exiv2/exiv2.hpp>
#include <iostream>

#include <QDir>
#include <QDirIterator>
#include <QFileDialog>

#include <QMessageBox>

#include <QFuture>
#include <QFutureWatcher>
#include <QThread>
#include <QtConcurrent>

Photo::Photo() {}

Photo::Photo(const QString &imageInput)
{
    setImageStruct(imageInput);
}

Photo::~Photo() {}

void Photo::setImageStruct(const QString &imageInput)
{
    Exiv2::XmpParser::initialize();
    ::atexit(Exiv2::XmpParser::terminate);
#ifdef EXV_ENABLE_BMFF
    Exiv2::enableBMFF();
#endif

    QFile file = imageInput;
    QFileInfo fileInfo(file.fileName());
    imgStruct = *new class imageStruct;

    imgStruct.fileName = fileInfo.fileName();
    imgStruct.fileBasename = fileInfo.completeBaseName();
    imgStruct.fileSuffix = fileInfo.completeSuffix();
    imgStruct.fileAbolutePath = fileInfo.absolutePath();
}

bool Photo::createWebpPath()
{
    QDir path = imgStruct.fileAbolutePath;
    QString newPath = imgStruct.fileAbolutePath + "/" + imgStruct.newFolder;
    if (path.mkpath(newPath))
    {
        // qDebug() << "createPath OK: " << newPath.toStdString();
        return true;
    }
    else
    {
        qCritical() << "createPath NOK: " << newPath.toStdString();
        return false;
    }
}

bool Photo::checkImgTargetExists(const QFile &pathToTargetImage)
{
    return pathToTargetImage.exists();
}

bool Photo::checkImgWidth(const QImage &imageInput, const int &targetWidth)
{
    // qDebug() << "width - target: " << imageInput.width() << " - " << targetWidth;
    return imageInput.width() >= targetWidth;
}

bool Photo::backupOrigFile()
{
    using std::chrono::floor;
#ifndef __APPLE__
    auto epoch = std::chrono::utc_clock::now();
#else
    auto epoch = std::chrono::system_clock::now();
#endif
    auto humanTime = floor<std::chrono::seconds>(epoch);
    std::string dt = std::format("{0:%Y-%m-%d_%H-%M-%S}", humanTime);

    std::string srcPath = imgStruct.fileAbolutePath.toStdString() + "/";
    std::string src = srcPath + imgStruct.fileBasename.toStdString() + "." + imgStruct.fileSuffix.toStdString();
    std::string destPath = QDir::tempPath().toStdString() + "/";
    std::string dest = destPath + imgStruct.fileBasename.toStdString() + "_orig-from_" + dt + "." + imgStruct.fileSuffix.toStdString();

    const auto copyOptions = std::filesystem::copy_options::update_existing;

    std::filesystem::copy(src, dest, copyOptions);
    try
    {
        std::filesystem::copy(src, dest, copyOptions);
        std::cout << "File copied successfully\n";
        return true;
    }
    catch (std::filesystem::filesystem_error &e)
    {
        std::cerr << e.what() << '\n';
    }

    return false;
}

void Photo::readExif()
{
    std::string pathToFile = imgStruct.fileAbolutePath.toStdString() + "/" + imgStruct.fileName.toStdString();

    auto exif_image = Exiv2::ImageFactory::open(pathToFile);
    exif_image->readMetadata();

    imgStruct.exifData = exif_image->exifData();
    Exiv2::Exifdatum &tag = imgStruct.exifData["Exif.Image.Model"];
    // qDebug() << "exif: " << tag.toString();
}

QString Photo::getPhotoDateTimeHuman()
{
    std::string pathToFile = imgStruct.fileAbolutePath.toStdString() + "/" + imgStruct.fileName.toStdString();

    Exiv2::XmpParser::initialize();
    ::atexit(Exiv2::XmpParser::terminate);
#ifdef EXV_ENABLE_BMFF
    Exiv2::enableBMFF();
#endif
    try
    {
        auto exif_image = Exiv2::ImageFactory::open(pathToFile);

        exif_image->readMetadata();
        Exiv2::ExifData &exifData = exif_image->exifData();

        if (exifData.empty())
        {
            auto ftime = std::filesystem::last_write_time(pathToFile);
            auto datetime = std::format("{0:%Y-%m-%d}_{0:%H%M%S}", ftime);
            QString dt = datetime.c_str();
            dt = dt.remove(QRegularExpression("(\\.\\d+)$"));
            return dt;
        }
        else
        {
            QString date_time = exifData["Exif.Image.DateTime"].value().toString().c_str();
            if (!date_time.isEmpty())
            {
                QStringList list = date_time.split(u' ');
                QString date = list[0];
                date.replace(":", "-");
                QString time = list[1];
                time.replace(":", "");
                return (date + "_" + time);
            }
            else
            {
                auto ftime = std::filesystem::last_write_time(pathToFile);
                auto datetime = std::format("{0:%Y-%m-%d}_{0:%H%M%S}", ftime);
                QString dt = datetime.c_str();
                dt = dt.remove(QRegularExpression("(\\.\\d+)$"));
                return dt;
            }
        }
    }
    catch (Exiv2::Error &e)
    {
        qWarning() << "getPhotoDateTimeHuman Caught Exiv2 exception " << e.what() << "\n";
        if (std::filesystem::exists(pathToFile))
        {
            auto ftime = std::filesystem::last_write_time(pathToFile);
            auto datetime = std::format("{0:%Y-%m-%d}_{0:%H%M%S}", ftime);
            QString dt = datetime.c_str();
            dt = dt.remove(QRegularExpression("(\\.\\d+)$"));
            return dt;
        }
        else
        {
            return "";
        }
    }
    return "";
}

QString Photo::getImgNewTimestampName()
{
    QString imgIn = imgStruct.fileAbolutePath + "/" + imgStruct.fileBasename + "." + imgStruct.fileSuffix;
    QString dt = getPhotoDateTimeHuman();
    if (dt.length() < 2)
    {
        return imgIn;
    }
    QString imgOut = imgStruct.fileAbolutePath + "/" + dt + "/" + imgStruct.fileSuffix;

    return imgOut;
}

QString Photo::getPathToImageName()
{
    // return imgStruct.fileAbolutePath + "/" + imgStruct.fileBasename + "." + imgStruct.fileSuffix;
    qDebug() << "Photo::getPathToImageName : " << pathToNewImageFile;
    return pathToNewImageFile;
}

const std::tuple<bool, QString> Photo::renameImageToTimestamp()
{
    QString imgIn = imgStruct.fileAbolutePath + "/" + imgStruct.fileBasename + "." + imgStruct.fileSuffix;
    qDebug() << "Photo::renameImageToTimestamp imgIn: " << imgIn;
    QString dt = getPhotoDateTimeHuman();
    qDebug() << "Photo::renameImageToTimestamp renameImageToTimestamp: " << dt;
    QString imgOut = imgStruct.fileAbolutePath + "/" + dt + "." + imgStruct.fileSuffix.toLower();
    qDebug() << "Photo::renameImageToTimestamp imgOut: " << imgOut;

    pathToNewImageFile = imgOut;

    QFile file(imgIn);
    if (file.rename(imgOut) == true)
    {
        qDebug() << "Photo::renameImageToTimestamp rename true: " << dt;
        // imgStruct.fileBasename = dt;
        pathToNewImageFile = imgOut;
        qDebug() << "Photo::renameImageToTimestamp rename true pathToNewImageFile: "
                 << pathToNewImageFile;
        return std::make_tuple(true, imgOut);
    }
    else
    {
        qWarning() << "Photo::renameImageToTimestamp NOK: " << imgIn << " " << dt;
        qWarning() << "Photo::renameImageToTimestamp NOK: " << imgOut << " "
                   << imgStruct.fileBasename;
        pathToNewImageFile = "";
        return std::make_tuple(false, "");
    }
}

const bool Photo::convertImage(const int &targetSize, const int &quality)
{
    QString imgIn = imgStruct.fileAbolutePath + "/" + imgStruct.fileBasename + "." + imgStruct.fileSuffix;

    QString imgOutName{""};
    if (renameToTimestamp_bool)
    {
        imgOutName = getPhotoDateTimeHuman() + "__" + QString::number(targetSize) + "." + imgStruct.newSuffix;
    }
    else
    {
        imgOutName = imgStruct.fileBasename + "__" + QString::number(targetSize) + "." + imgStruct.newSuffix;
    }

    QString imgOut = imgStruct.fileAbolutePath + "/" + imgStruct.newFolder;
    imgOut = imgOut + "/" + imgOutName;
    QImage imageInput{imgIn};

    if (!checkImgWidth(imageInput, targetSize))
    {
        // qDebug() << "img is < " << targetSize;
        //  well, the confused exit gate
        if (!oversizeSmallerPicture_bool)
        {
            return false;
        }
        // qDebug() << "request: img will be increased";
    }

    if (checkImgTargetExists(imgOut))
    {
        // qDebug() << "target exists: " << imgOut.toStdString();
        if (!overwriteExitingWebp_bool)
        {
            return false;
        }
        // qDebug() << "request: existing img will be overwritten";
    }
    else
    {
        // qDebug() << "target doesn't exists: " << imgOut.toStdString();
        if (!createWebpPath())
        {
            std::cerr << "convertImage failed to create webp folder: " << imgOut.toStdString();
            return false;
        }
    }

    QImage imageOutput = imageInput.scaledToWidth(targetSize, Qt::SmoothTransformation);

    if (watermarkWebp_bool)
    {
        QImage reducedCopy{":/resources/img/reduced_copy.png"};
        QPainter painter(&imageOutput);
        painter.drawImage(0, 0, reducedCopy);
    }

    if (!imageOutput.save(imgOut, "WEBP", quality))
    {
        std::cerr << "convertImage failed to save webp image: " << imgOut.toStdString() << " ";
        std::filesystem::remove(imgOut.toStdString());
        return false;
    }
    return true;
}

const bool Photo::convertImages(const int &quality)
{
    bool ret{false};
    for (const auto &size : imgStruct.webpSizes)
    {
        QFuture<bool> future = QtConcurrent::run(&Photo::convertImage, this, size, quality);
        ret = future.result();
        /*if (!convertImage(size, quality)) {
            ret = false;
        } else {
            ret = true;
        }*/
    }
    return ret;
}

bool Photo::rotateImage(const int &turn)
{
    QString imgIn = imgStruct.fileAbolutePath + "/" + imgStruct.fileBasename + "." + imgStruct.fileSuffix;
    // qDebug() << "rotateImage: " << imgIn;

    backupOrigFile();

    QImage pix(imgIn);
    QImage pix_rotated = pix.transformed(QTransform().rotate(turn), Qt::SmoothTransformation);
    if (!pix_rotated.save(imgIn, nullptr, 100))
    {
        std::cerr << "rotateImage failed: " << imgIn.toStdString();
        return false;
    }
    else
    {
        std::cerr << "rotateImage successfully: " << imgIn.toStdString();
    }
    /*
    painter.setFont(QFont("Helvetia", 12));
    painter.setPen(QPen(Qt::red, 1));
    painter.drawText(20, 10, "rotated");
    */

    return true;
}

QList<int> Photo::getWebSizes()
{
    return imgStruct.webpSizes;
}

QString Photo::getSuffix()
{
    return imgStruct.fileSuffix;
}

void Photo::setOversizeSmallerPicture(const bool oversizeSmallerPicture)
{
    oversizeSmallerPicture_bool = oversizeSmallerPicture;
}

void Photo::setOverwriteExistingWebp(const bool overwriteExitingWebp)
{
    overwriteExitingWebp_bool = overwriteExitingWebp;
}

void Photo::setWatermarkWebp(const bool watermarkWebp)
{
    watermarkWebp_bool = watermarkWebp;
}

void Photo::setRenameToTimestamp(const bool renameToTimestamp)
{
    renameToTimestamp_bool = renameToTimestamp;
}

QList<QString> Photo::srcPics(const QString &srcPath)
{
    QDirIterator srcPics(srcPath,
                         {"*.jpg", "*.jpeg", "*.png", "*.bmp", "*.tiff"},
                         QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable);

    QList<QString> fileList;
    while (srcPics.hasNext())
    {
        fileList.append(srcPics.next());
    }

    return fileList;
}

QList<QString> Photo::srcPicsRecursive(const QString &srcPath)
{
    QDirIterator srcPics(srcPath,
                         {"*.jpg", "*.jpeg", "*.png", "*.bmp", "*.tiff"},
                         QDir::Files,
                         QDirIterator::Subdirectories);

    QList<QString> fileList;
    while (srcPics.hasNext())
    {
        fileList.append(srcPics.next());
    }

    return fileList;
}

bool Photo::writeXmp(QString &key, QString &value)
{
    QString pathToFile = imgStruct.fileAbolutePath + "/" + imgStruct.fileName;

    Exiv2::XmpParser::initialize();
    ::atexit(Exiv2::XmpParser::terminate);
#ifdef EXV_ENABLE_BMFF
    Exiv2::enableBMFF();
#endif
    try
    {
        auto xmp_image = Exiv2::ImageFactory::open(pathToFile.toStdString());
        xmp_image->readMetadata();
        Exiv2::XmpData &xmpData = xmp_image->xmpData();
        xmpData[key.toStdString()] = value.toStdString();
        xmp_image->setXmpData(xmpData);
        xmp_image->writeMetadata();
        // qDebug() << "writeXmp: write OK";
        return true;
    }
    catch (...)
    {
        qCritical() << "writeXmp: write NOK";
        return false;
    }
}

bool Photo::writeExif(QString &key, QString &value)
{
    QString pathToFile = imgStruct.fileAbolutePath + "/" + imgStruct.fileName;

    Exiv2::XmpParser::initialize();
    ::atexit(Exiv2::XmpParser::terminate);
#ifdef EXV_ENABLE_BMFF
    Exiv2::enableBMFF();
#endif
    try
    {
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
    }
    catch (Exiv2::Error &e)
    {
        qCritical() << "Caught Exiv2 exception " << e.what() << "\n";
        return false;
    }
}

bool Photo::writeIptc(QString &key, QString &value)
{
    QString pathToFile = imgStruct.fileAbolutePath + "/" + imgStruct.fileName;

    Exiv2::XmpParser::initialize();
    ::atexit(Exiv2::XmpParser::terminate);
#ifdef EXV_ENABLE_BMFF
    Exiv2::enableBMFF();
#endif
    try
    {
        Exiv2::XmpParser::initialize();
        ::atexit(Exiv2::XmpParser::terminate);
        auto iptc_image = Exiv2::ImageFactory::open(pathToFile.toStdString());
        iptc_image->readMetadata();
        Exiv2::IptcData &iptcData = iptc_image->iptcData();
        iptcData[key.toStdString()] = value.toStdString();
        iptc_image->setIptcData(iptcData);
        iptc_image->writeMetadata();
        return true;
    }
    catch (Exiv2::Error &e)
    {
        qCritical() << "Caught Exiv2 exception " << e.what() << "\n";
        return false;
    }
}

QString Photo::getXmpCopyrightOwner()
{
    QString owner{""};
    QString pathToFile = imgStruct.fileAbolutePath + "/" + imgStruct.fileName;

    Exiv2::XmpParser::initialize();
    ::atexit(Exiv2::XmpParser::terminate);
#ifdef EXV_ENABLE_BMFF
    Exiv2::enableBMFF();
#endif
    try
    {
        auto xmp_image = Exiv2::ImageFactory::open(pathToFile.toStdString());
        xmp_image->readMetadata();
        Exiv2::XmpData &xmpData = xmp_image->xmpData();
        if (xmpData.empty())
        {
            qInfo() << "No XMP data found in file " << pathToFile;
            return owner;
        }
        else
        {
            auto ret = xmpData["Xmp.dc.CopyrightOwner"];
            // qDebug() << "Owner: " << ret.value().toString();
            owner = ret.value().toString().c_str();
        }
    }
    catch (Exiv2::Error &e)
    {
        qCritical() << "Caught Exiv2 exception " << e.what() << "\n";
        return owner;
    }
    return owner;
}

bool Photo::writeToAllCopyrightOwner(const QString &owner)
{
    QString key{""};
    QString val = owner;

    key = "Xmp.dc.CopyrightOwner";
    writeXmp(key, val);
    key = "Exif.Image.Copyright";
    writeExif(key, val);
    key = "Iptc.Application2.Copyright";
    writeIptc(key, val);

    return true;
}

QString Photo::getGpsString()
{
    std::string pathToFile = imgStruct.fileAbolutePath.toStdString() + "/" + imgStruct.fileName.toStdString();
    QString lat{""}, lng{""};
    auto exif_image = Exiv2::ImageFactory::open(pathToFile);

    exif_image->readMetadata();
    Exiv2::ExifData &exifData = exif_image->exifData();

    if (exifData.empty())
    {
        return "";
    }

    std::tie(lat, lng) = getGpsToDecimalString(exifData["Exif.GPSInfo.GPSLatitude"],
                                               exifData["Exif.GPSInfo.GPSLongitude"]);

    return lat + "," + lng;
}

std::tuple<QString, QString> Photo::getGpsToDecimalString(Exiv2::Metadatum &lat,
                                                          Exiv2::Metadatum &lng)
{
    QString latDec{""}, lngDec{""};
    char r[500];

    switch (lat.typeId())
    {
    case Exiv2::unsignedRational:
    {
        double decimal = 0;
        double denom = 1;
        for (int i = 0; i < lat.count(); i++)
        {
            Exiv2::Rational rational = lat.toRational(i);
            decimal += lat.toFloat(i) / denom;
            denom *= 60;
        }
        latDec = QString::number(decimal, 'f', 6);
    }
    break;

    default:
        break;
    }

    switch (lng.typeId())
    {
    case Exiv2::unsignedRational:
    {
        double decimal = 0;
        double denom = 1;
        for (int i = 0; i < lng.count(); i++)
        {
            Exiv2::Rational rational = lng.toRational(i);
            decimal += lng.toFloat(i) / denom;
            denom *= 60;
        }
        lngDec = QString::number(decimal, 'f', 6);
    }
    break;

    default:
        break;
    }

    return std::make_tuple(latDec, lngDec);
}

void Photo::writeDefaultGpsData(const exifGpsStruct &gpsData)
{
    QString pathToFile = imgStruct.fileAbolutePath + "/" + imgStruct.fileName;

    Exiv2::XmpParser::initialize();
    ::atexit(Exiv2::XmpParser::terminate);
#ifdef EXV_ENABLE_BMFF
    Exiv2::enableBMFF();
#endif
    try
    {
        Exiv2::XmpParser::initialize();
        ::atexit(Exiv2::XmpParser::terminate);

        auto exif_image = Exiv2::ImageFactory::open(pathToFile.toStdString());
        exif_image->readMetadata();
        Exiv2::ExifData &exifData = exif_image->exifData();

        exifData["Exif.GPSInfo.GPSLatitudeRef"] = gpsData.GPSLatitudeRef.toStdString();
        exifData["Exif.GPSInfo.GPSLatitude"] = gpsData.GPSLatitude.toStdString();
        exifData["Exif.GPSInfo.GPSLongitudeRef"] = gpsData.GPSLongitudeRef.toStdString();
        exifData["Exif.GPSInfo.GPSLongitude"] = gpsData.GPSLongitude.toStdString();
        exifData["Exif.GPSInfo.GPSAltitudeRef"] = gpsData.GPSAltitudeRef.toStdString();
        exifData["Exif.GPSInfo.GPSAltitude"] = gpsData.GPSAltitude.toStdString();

        // qDebug() << "Photo::writeDefaultGpsData setExifData";
        exif_image->setExifData(exifData);
        // qDebug() << "Photo::writeDefaultGpsData writeMetaData";
        exif_image->writeMetadata();
        return;
    }
    catch (Exiv2::Error &e)
    {
        qCritical() << "Caught Exiv2 exception " << e.what() << "\n";
        return;
    }
}

Photo::exifGpsStruct Photo::getGpsData() const
{
    exifGpsStruct gpsStruct;
    std::string pathToFile = imgStruct.fileAbolutePath.toStdString() + "/" + imgStruct.fileName.toStdString();

    try
    {
        auto exif_image = Exiv2::ImageFactory::open(pathToFile);
        exif_image->readMetadata();
        Exiv2::ExifData &exifData = exif_image->exifData();
        if (exifData.empty())
        {
            return gpsStruct;
        }

        gpsStruct.GPSTag = exifData["Exif.Image.GPSTag"].toString().c_str();
        gpsStruct.GPSLatitudeRef = exifData["Exif.GPSInfo.GPSLatitudeRef"].toString().c_str();
        gpsStruct.GPSLatitude = exifData["Exif.GPSInfo.GPSLatitude"].toString().c_str();
        gpsStruct.GPSLongitudeRef = exifData["Exif.GPSInfo.GPSLongitudeRef"].toString().c_str();
        gpsStruct.GPSLongitude = exifData["Exif.GPSInfo.GPSLongitude"].toString().c_str();
        gpsStruct.GPSAltitudeRef = exifData["Exif.GPSInfo.GPSAltitudeRef"].toString().c_str();
        gpsStruct.GPSAltitude = exifData["Exif.GPSInfo.GPSAltitude"].toString().c_str();
        gpsStruct.GPSMapDatum = exifData["Exif.GPSInfo.GPSMapDatum"].toString().c_str();
        gpsStruct.GPSTimeStamp = exifData["Exif.GPSInfo.GPSTimeStamp"].toString().c_str();
        gpsStruct.GPSDateStamp = exifData["Exif.GPSInfo.GPSDateStamp"].toString().c_str();
        gpsStruct.DateTimeOriginal = exifData["Exif.Photo.DateTimeOriginal"].toString().c_str();

        return gpsStruct;
    }
    catch (Exiv2::Error &e)
    {
        qCritical() << "Caught Exiv2 exception " << e.what() << "\n";
        return gpsStruct;
    }
}

bool Photo::isValidMetaImageType()
{
    QString photoExtension = imgStruct.fileSuffix.toLower();

    if (validMetaImageTypes.contains(photoExtension))
    {
        return true;
    }
    else
    {
        return false;
    }
}
