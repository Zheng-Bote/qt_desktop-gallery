#pragma once

#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QList>
#include <QMatrix2x2>
#include <QPainter>
#include <QString>

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
    QList<int> webpSizes = {480, 680, 800};
};

class Photo
{
public:
    Photo();
    Photo(const QString &imageInput);
    ~Photo();

    bool convertImage(const int &targetSize, const int &quality = 75);
    const bool convertImages(const int &quality = 75);
    bool rotateImage(const int &turn);
    QList<int> getWebSizes();

    QString getSuffix();

    QList<QString> srcPics(const QString &srcPath);
    QList<QString> srcPicsRecursive(const QString &srcPath);

private:
    const QList<QString> validMetaImageTypes = {"jpg", "jpeg", "png", "webp", "tiff"};
    bool isValidMetaImageType();

    imageStruct imgStruct;
    void setImageStruct(const QString &imageInput);

    bool createWebpPath();
    bool checkImgTargetExists(const QFile &pathToTargetImage);
    bool checkImgWidth(const QImage &imageInput, const int &targetWidth);

    bool backupOrigFile();

    void readExif();
};
