#include "rz_photo.hpp"

#include <chrono>
#include <exiv2/exiv2.hpp>
#include <iostream>

#include <QDir>
#include <QDirIterator>
#include <QFileDialog>

#include <QMessageBox>

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
    if (path.mkpath(newPath)) {
        qDebug() << "createPath OK: " << newPath.toStdString();
        return true;
    } else {
        qDebug() << "createPath NOK: " << newPath.toStdString();
        return false;
    }
}

bool Photo::checkImgTargetExists(const QFile &pathToTargetImage)
{
    return pathToTargetImage.exists();
}

bool Photo::checkImgWidth(const QImage &imageInput, const int &targetWidth)
{
    qDebug() << "width - target: " << imageInput.width() << " - " << targetWidth;
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
    std::string src = srcPath + imgStruct.fileBasename.toStdString() + "."
                      + imgStruct.fileSuffix.toStdString();
    std::string destPath = QDir::tempPath().toStdString() + "/";
    std::string dest = destPath + imgStruct.fileBasename.toStdString() + "_orig-from_" + dt + "."
                       + imgStruct.fileSuffix.toStdString();

    const auto copyOptions = std::filesystem::copy_options::update_existing;

    std::filesystem::copy(src, dest, copyOptions);
    try {
        std::filesystem::copy(src, dest, copyOptions);
        std::cout << "File copied successfully\n";
        return true;
    } catch (std::filesystem::filesystem_error &e) {
        std::cerr << e.what() << '\n';
    }

    return false;
}

void Photo::readExif()
{
    std::string pathToFile = imgStruct.fileAbolutePath.toStdString() + "/"
                             + imgStruct.fileName.toStdString();

    auto exif_image = Exiv2::ImageFactory::open(pathToFile);
    exif_image->readMetadata();

    imgStruct.exifData = exif_image->exifData();
    Exiv2::Exifdatum &tag = imgStruct.exifData["Exif.Image.Model"];
    qDebug() << "exif: " << tag.toString();
}

bool Photo::convertImage(const int &targetSize, const int &quality)
{
    QString imgIn = imgStruct.fileAbolutePath + "/" + imgStruct.fileBasename + "."
                    + imgStruct.fileSuffix;
    QString imgOutName = imgStruct.fileBasename + "__" + QString::number(targetSize) + "."
                         + imgStruct.newSuffix;
    QString imgOut = imgStruct.fileAbolutePath + "/" + imgStruct.newFolder;
    imgOut = imgOut + "/" + imgOutName;
    QImage imageInput{imgIn};

    if (!checkImgWidth(imageInput, targetSize)) {
        qDebug() << "img is < " << targetSize;
        // well, the confused exit gate
        return true;
    }

    if (checkImgTargetExists(imgOut)) {
        qDebug() << "target exists: " << imgOut.toStdString();
        // well, the confused exit gate
        return true;
    } else {
        qDebug() << "target doesn't exists: " << imgOut.toStdString();
        if (!createWebpPath()) {
            std::cerr << "convertImage failed to create webp folder: " << imgOut.toStdString();
            return false;
        }
    }

    QImage reducedCopy{":/images/reduced_copy.png"};
    QImage imageOutput = imageInput.scaledToWidth(targetSize, Qt::SmoothTransformation);
    QPainter painter(&imageOutput);
    painter.drawImage(0, 0, reducedCopy);
    if (!imageOutput.save(imgOut, "WEBP", quality)) {
        std::cerr << "convertImage failed to save webp image: " << imgOut.toStdString();
        return false;
    }
    return true;
}

const bool Photo::convertImages(const int &quality)
{
    bool ret{false};
    for (const auto &size : imgStruct.webpSizes) {
        if (!convertImage(size, quality)) {
            ret = false;
        } else {
            ret = true;
        }
    }
    return ret;
}

bool Photo::rotateImage(const int &turn)
{
    QString imgIn = imgStruct.fileAbolutePath + "/" + imgStruct.fileBasename + "."
                    + imgStruct.fileSuffix;
    qDebug() << "rotateImage: " << imgIn;

    backupOrigFile();

    QImage pix(imgIn);
    QImage pix_rotated = pix.transformed(QTransform().rotate(turn), Qt::SmoothTransformation);
    if (!pix_rotated.save(imgIn, nullptr, 100)) {
        std::cerr << "rotateImage failed: " << imgIn.toStdString();
        return false;
    } else {
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

QList<QString> Photo::srcPics(const QString &srcPath)
{
    QDirIterator srcPics(srcPath,
                         {"*.jpg", "*.jpeg", "*.png", "*.bmp", "*.tiff"},
                         QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable);

    QList<QString> fileList;
    while (srcPics.hasNext()) {
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
    while (srcPics.hasNext()) {
        fileList.append(srcPics.next());
    }

    return fileList;
}

bool Photo::isValidMetaImageType()
{
    QString photoExtension = imgStruct.fileSuffix.toLower();

    if (validMetaImageTypes.contains(photoExtension)) {
        return true;
    } else {
        return false;
    }
}
