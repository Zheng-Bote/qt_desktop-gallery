#include "picture_widget.h"
#include <QInputDialog>
#include <QMessageBox>
#include "ui_picture_widget.h"

#include <exiv2/exiv2.hpp>

#include <QFuture>
#include <QFutureWatcher>
#include <QThread>
#include <QtConcurrent>

PictureWidget::PictureWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PictureWidget)
{
    ui->setupUi(this);

    Exiv2::XmpParser::initialize();
    ::atexit(Exiv2::XmpParser::terminate);
#ifdef EXV_ENABLE_BMFF
    Exiv2::enableBMFF();
#endif

    ui->closeBtn->setIcon(QIcon(":/resources/img/icons8-arrow-pointing-left-50.png"));
    ui->exportSrcToWebp_Btn->setIcon(QIcon(":/resources/img/icons8-send-file-50.png"));
    createRotateMenu();
    createExportMenu();
}

PictureWidget::~PictureWidget()
{
    delete ui;
}

void PictureWidget::setImage(QString pathToFile)
{
    QString pictureData{};

    pathToImage = pathToFile;
    QFile srcFile(pathToFile);
    QFileInfo fileInfo(srcFile.fileName());

    picture = new QPixmap(pathToImage);

    pictureData = fileInfo.fileName() + " \n(" + QString::number(picture->width()) + "x"
                  + QString::number(picture->height()) + " "
                  + QString::number(fileInfo.size() / 1024) + " KiB)";
    ui->filename_label->setText(pictureData);
    ui->filename_label->setToolTip(pathToFile);

    ui->picture_label->setPixmap(
        picture->scaled(ui->picture_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QFuture<void> futureExif = QtConcurrent::run(&PictureWidget::readSrcExif, this);
    QFuture<void> futureIptc = QtConcurrent::run(&PictureWidget::readSrcIptc, this);
    QFuture<void> futureXmp = QtConcurrent::run(&PictureWidget::readSrcXmp, this);
    //readSrcExif();
    //readSrcIptc();

    ui->tabWidget->adjustSize();
    ui->tabWidget->setCurrentWidget(0);
}

void PictureWidget::on_closeBtn_clicked()
{
    delete ui;
    this->close();
}

void PictureWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    resizePicture();
}

void PictureWidget::closeEvent(QCloseEvent *event)
{
    if (dataModified_bool) {
        QMessageBox::StandardButton response;

        response = QMessageBox::question(this,
                                         tr("Confirmation"),
                                         tr("Are you sure you want to exit without saving data?"),
                                         QMessageBox::Yes | QMessageBox::No);

        if (response == QMessageBox::Yes) {
            if (ui != nullptr) {
                delete ui;
                qDebug() << "destroy ExifIptc ui";
            }
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
    QWidget::closeEvent(event);
}

void PictureWidget::resizePicture()
{
    ui->picture_label->setPixmap(
        picture->scaled(ui->picture_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

bool PictureWidget::checkValidMetaImg()
{
    bool ret{false};
    Photo *photo = new Photo(pathToImage);
    QString photoExtension = photo->getSuffix().toLower();

    //qDebug() << "checkValidMetaImg Extension: " << photoExtension;

    if (validMetaImageTypes.contains(photoExtension)) {
        //  qDebug() << "checkValidMetaImg has: " << photoExtension;
        ret = true;
    } else {
        qDebug() << "checkValidMetaImg has no: " << photoExtension;
        ret = false;
    }

    photo = nullptr;
    return ret;
}

void PictureWidget::disableMetaTabs(int tab)
{
    ui->tabWidget->setTabVisible(tab, false);
}

const void PictureWidget::readSrcExif()
{
    if (!checkValidMetaImg()) {
        disableMetaTabs(1);
        return;
    }

    QFile file(pathToImage);
    auto exif_image = Exiv2::ImageFactory::open(file.fileName().toUtf8().toStdString());

    QFont font_11_bold("Times New Roman", 11);
    font_11_bold.setBold(true);

    ui->exifTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->exifTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->exifTableWidget->insertRow(0);
    ui->exifTableWidget->insertColumn(0);
    ui->exifTableWidget->insertColumn(1);
    ui->exifTableWidget->insertColumn(2);

    QTableWidgetItem *tlbCol1 = new QTableWidgetItem();
    tlbCol1->setText("EXIF key");
    tlbCol1->setBackground(Qt::lightGray);
    tlbCol1->setFont(font_11_bold);
    tlbCol1->setTextAlignment(Qt::AlignCenter);
    ui->exifTableWidget->setHorizontalHeaderItem(0, tlbCol1);

    QTableWidgetItem *tlbCol2 = new QTableWidgetItem();
    tlbCol2->setText("EXIF value");
    tlbCol2->setBackground(Qt::lightGray);
    tlbCol2->setFont(font_11_bold);
    tlbCol2->setTextAlignment(Qt::AlignCenter);
    ui->exifTableWidget->setHorizontalHeaderItem(1, tlbCol2);

    QTableWidgetItem *tlbCol3 = new QTableWidgetItem();
    tlbCol3->setText("EXIF description");
    tlbCol3->setBackground(Qt::lightGray);
    tlbCol3->setFont(font_11_bold);
    tlbCol3->setTextAlignment(Qt::AlignCenter);
    ui->exifTableWidget->setHorizontalHeaderItem(2, tlbCol3);

    ui->exifTableWidget->removeRow(0);

    exif_image->readMetadata();

    Exiv2::ExifData &exifData = exif_image->exifData();

    if (exifData.empty()) {
        qDebug() << "No EXIF data found in file " << pathToImage;
        //createExiv2CopyrightRow();
    } else {
        auto end = exifData.end();
        for (auto md = exifData.begin(); md != end; ++md) {
            ui->exifTableWidget->insertRow(ui->exifTableWidget->rowCount());

            ui->exifTableWidget->setItem(ui->exifTableWidget->rowCount() - 1,
                                         0,
                                         new QTableWidgetItem(md->key().c_str()));

            QTableWidgetItem *tlbCol2val = new QTableWidgetItem();
            tlbCol2val->setText(md->value().toString().c_str());
            //tlbCol2val->setTextAlignment(Qt::AlignRight);
            //tlbCol2val->setTextAlignment(Qt::AlignVCenter);

            ui->exifTableWidget->setItem(ui->exifTableWidget->rowCount() - 1, 1, tlbCol2val);
            tlbCol2val = nullptr;

            if (exifMetaTags.contains(md->key().c_str())) {
                QTableWidgetItem *tlbCol3val = new QTableWidgetItem();
                tlbCol3val->setText(exifMetaTags.value(md->key().c_str(), ""));
                ui->iptcTableWidget->setItem(ui->iptcTableWidget->rowCount() - 1, 2, tlbCol3val);
                tlbCol3val = nullptr;
            }
        }
    }

    // check and set default meta tags, if missing
    for (auto i = exifMetaTags.cbegin(), end = exifMetaTags.cend(); i != end; ++i) {
        markExif(i.key());
        //i.value()
    }

    // cleanup
    tlbCol1 = nullptr;
    tlbCol2 = nullptr;
    tlbCol3 = nullptr;
    file.close();

    /*
    ui->exifTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->exifTableWidget->resizeColumnsToContents();
    ui->exifTableWidget->resizeRowsToContents();

    ui->tabWidget->adjustSize();
    */
}

void PictureWidget::markExif(QString searchFor)
{
    QList<QTableWidgetItem *> LTempTable = ui->exifTableWidget->findItems(searchFor,
                                                                          Qt::MatchEndsWith);

    if (LTempTable.isEmpty()) {
        ui->exifTableWidget->insertRow(ui->exifTableWidget->rowCount());

        ui->exifTableWidget->setItem(ui->exifTableWidget->rowCount() - 1,
                                     0,
                                     new QTableWidgetItem(searchFor));

        QTableWidgetItem *tlbCol2val = new QTableWidgetItem();
        tlbCol2val->setText("");
        //tlbCol2val->setTextAlignment(Qt::AlignRight);
        ui->exifTableWidget->setItem(ui->exifTableWidget->rowCount() - 1, 1, tlbCol2val);
        tlbCol2val = nullptr;

        if (exifMetaTags.contains(searchFor)) {
            QTableWidgetItem *tlbCol3val = new QTableWidgetItem();
            tlbCol3val->setText(exifMetaTags.value(searchFor, ""));
            ui->exifTableWidget->setItem(ui->exifTableWidget->rowCount() - 1, 2, tlbCol3val);
            tlbCol3val = nullptr;
        }
    }

    QTableWidgetItem *rowPtr = new QTableWidgetItem();
    LTempTable = ui->exifTableWidget->findItems(searchFor, Qt::MatchEndsWith);
    foreach (rowPtr, LTempTable) {
        //rowPtr->setBackground(Qt::red);
        int rowNumber = rowPtr->row();
        ui->exifTableWidget->item(rowNumber, 0)->setForeground(Qt::red);

        if (ui->exifTableWidget->item(rowNumber, 1)->text().length() < 1) {
            ui->exifTableWidget->item(rowNumber, 1)->setBackground(Qt::red);
            ui->exifTableWidget->item(rowNumber, 1)->setForeground(Qt::black);
        }
    }
}

const void PictureWidget::readSrcIptc()
{
    if (!checkValidMetaImg()) {
        disableMetaTabs(2);
        return;
    }
    QFile file(pathToImage);
    auto iptc_image = Exiv2::ImageFactory::open(file.fileName().toUtf8().toStdString());

    QFont font_11_bold("Times New Roman", 11);
    font_11_bold.setBold(true);

    ui->iptcTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->iptcTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->iptcTableWidget->insertRow(0);
    ui->iptcTableWidget->insertColumn(0);
    ui->iptcTableWidget->insertColumn(1);
    ui->iptcTableWidget->insertColumn(2);

    QTableWidgetItem *tlbCol1 = new QTableWidgetItem();
    tlbCol1->setText("IPTC key");
    tlbCol1->setBackground(Qt::lightGray);
    tlbCol1->setFont(font_11_bold);
    tlbCol1->setTextAlignment(Qt::AlignCenter);
    ui->iptcTableWidget->setHorizontalHeaderItem(0, tlbCol1);

    QTableWidgetItem *tlbCol2 = new QTableWidgetItem();
    tlbCol2->setText("IPTC value");
    tlbCol2->setBackground(Qt::lightGray);
    tlbCol2->setFont(font_11_bold);
    tlbCol2->setTextAlignment(Qt::AlignCenter);
    ui->iptcTableWidget->setHorizontalHeaderItem(1, tlbCol2);

    QTableWidgetItem *tlbCol3 = new QTableWidgetItem();
    tlbCol3->setText("IPTC description");
    tlbCol3->setBackground(Qt::lightGray);
    tlbCol3->setFont(font_11_bold);
    tlbCol3->setTextAlignment(Qt::AlignCenter);
    ui->iptcTableWidget->setHorizontalHeaderItem(2, tlbCol3);

    ui->iptcTableWidget->removeRow(0);

    iptc_image->readMetadata();

    Exiv2::IptcData &iptcData = iptc_image->iptcData();

    if (iptcData.empty()) {
        qDebug() << "No IPTC data found in file " << pathToImage;
        //createIptcCopyrightRow();
    } else {
        auto end = iptcData.end();
        for (auto md = iptcData.begin(); md != end; ++md) {
            ui->iptcTableWidget->insertRow(ui->iptcTableWidget->rowCount());

            ui->iptcTableWidget->setItem(ui->iptcTableWidget->rowCount() - 1,
                                         0,
                                         new QTableWidgetItem(md->key().c_str()));

            QTableWidgetItem *tlbCol2val = new QTableWidgetItem();
            tlbCol2val->setText(md->value().toString().c_str());
            //tlbCol2val->setTextAlignment(Qt::AlignRight);
            //tlbCol2val->setTextAlignment(Qt::AlignVCenter);

            ui->iptcTableWidget->setItem(ui->iptcTableWidget->rowCount() - 1, 1, tlbCol2val);
            tlbCol2val = nullptr;

            if (iptcMetaTags.contains(md->key().c_str())) {
                QTableWidgetItem *tlbCol3val = new QTableWidgetItem();
                tlbCol3val->setText(iptcMetaTags.value(md->key().c_str(), ""));
                ui->iptcTableWidget->setItem(ui->iptcTableWidget->rowCount() - 1, 2, tlbCol3val);
                tlbCol3val = nullptr;
            }
        }
    }

    // check and set default meta tags, if missing
    for (auto i = iptcMetaTags.cbegin(), end = iptcMetaTags.cend(); i != end; ++i) {
        markIptc(i.key());
        //i.value()
    }

    // cleanup
    tlbCol1 = nullptr;
    tlbCol2 = nullptr;
    tlbCol3 = nullptr;
    file.close();

    /*
    ui->iptcTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->iptcTableWidget->resizeColumnsToContents();
    ui->iptcTableWidget->resizeRowsToContents();

    ui->tabWidget->adjustSize();
    */
}

const void PictureWidget::readSrcXmp()
{
    if (!checkValidMetaImg()) {
        disableMetaTabs(3);
        return;
    }
    QFile file(pathToImage);
    auto xmp_image = Exiv2::ImageFactory::open(file.fileName().toUtf8().toStdString());

    QFont font_11_bold("Times New Roman", 11);
    font_11_bold.setBold(true);

    ui->xmpTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->xmpTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->xmpTableWidget->insertRow(0);
    ui->xmpTableWidget->insertColumn(0);
    ui->xmpTableWidget->insertColumn(1);
    ui->xmpTableWidget->insertColumn(2);

    QTableWidgetItem *tlbCol1 = new QTableWidgetItem();
    tlbCol1->setText("XMP key");
    tlbCol1->setBackground(Qt::lightGray);
    tlbCol1->setFont(font_11_bold);
    tlbCol1->setTextAlignment(Qt::AlignCenter);
    ui->xmpTableWidget->setHorizontalHeaderItem(0, tlbCol1);

    QTableWidgetItem *tlbCol2 = new QTableWidgetItem();
    tlbCol2->setText("XMP value");
    tlbCol2->setBackground(Qt::lightGray);
    tlbCol2->setFont(font_11_bold);
    tlbCol2->setTextAlignment(Qt::AlignCenter);
    ui->xmpTableWidget->setHorizontalHeaderItem(1, tlbCol2);

    QTableWidgetItem *tlbCol3 = new QTableWidgetItem();
    tlbCol3->setText("XMP description");
    tlbCol3->setBackground(Qt::lightGray);
    tlbCol3->setFont(font_11_bold);
    tlbCol3->setTextAlignment(Qt::AlignCenter);
    ui->xmpTableWidget->setHorizontalHeaderItem(2, tlbCol3);

    ui->xmpTableWidget->removeRow(0);

    xmp_image->readMetadata();

    Exiv2::XmpData &xmpData = xmp_image->xmpData();

    if (xmpData.empty()) {
        qDebug() << "No XMP data found in file " << pathToImage;
        //createIptcCopyrightRow();
    } else {
        auto end = xmpData.end();
        for (auto md = xmpData.begin(); md != end; ++md) {
            ui->xmpTableWidget->insertRow(ui->xmpTableWidget->rowCount());

            ui->xmpTableWidget->setItem(ui->xmpTableWidget->rowCount() - 1,
                                        0,
                                        new QTableWidgetItem(md->key().c_str()));

            QTableWidgetItem *tlbCol2val = new QTableWidgetItem();
            tlbCol2val->setText(md->value().toString().c_str());
            //tlbCol2val->setTextAlignment(Qt::AlignRight);
            //tlbCol2val->setTextAlignment(Qt::AlignVCenter);

            ui->xmpTableWidget->setItem(ui->xmpTableWidget->rowCount() - 1, 1, tlbCol2val);
            tlbCol2val = nullptr;
            /*
            if (iptcMetaTags.contains(md->key().c_str())) {
                QTableWidgetItem *tlbCol3val = new QTableWidgetItem();
                tlbCol3val->setText(iptcMetaTags.value(md->key().c_str(), ""));
                ui->iptcTableWidget->setItem(ui->iptcTableWidget->rowCount() - 1, 2, tlbCol3val);
                tlbCol3val = nullptr;
            }*/
        }
    }

    /*
    // check and set default meta tags, if missing
    for (auto i = iptcMetaTags.cbegin(), end = iptcMetaTags.cend(); i != end; ++i) {
        markIptc(i.key());
        //i.value()
    }*/

    // cleanup
    tlbCol1 = nullptr;
    tlbCol2 = nullptr;
    tlbCol3 = nullptr;
    file.close();

    ui->xmpTableWidget->resizeColumnsToContents();
    ui->xmpTableWidget->resizeRowsToContents();
}

void PictureWidget::markIptc(QString searchFor)
{
    QList<QTableWidgetItem *> LTempTable = ui->iptcTableWidget->findItems(searchFor,
                                                                          Qt::MatchEndsWith);

    if (LTempTable.isEmpty()) {
        ui->iptcTableWidget->insertRow(ui->iptcTableWidget->rowCount());

        ui->iptcTableWidget->setItem(ui->iptcTableWidget->rowCount() - 1,
                                     0,
                                     new QTableWidgetItem(searchFor));

        QTableWidgetItem *tlbCol2val = new QTableWidgetItem();
        tlbCol2val->setText("");
        //tlbCol2val->setTextAlignment(Qt::AlignRight);
        ui->iptcTableWidget->setItem(ui->iptcTableWidget->rowCount() - 1, 1, tlbCol2val);
        tlbCol2val = nullptr;

        if (iptcMetaTags.contains(searchFor)) {
            QTableWidgetItem *tlbCol3val = new QTableWidgetItem();
            tlbCol3val->setText(iptcMetaTags.value(searchFor, ""));
            ui->iptcTableWidget->setItem(ui->iptcTableWidget->rowCount() - 1, 2, tlbCol3val);
            tlbCol3val = nullptr;
        }
    }

    QTableWidgetItem *rowPtr = new QTableWidgetItem();
    LTempTable = ui->iptcTableWidget->findItems(searchFor, Qt::MatchEndsWith);
    foreach (rowPtr, LTempTable) {
        //rowPtr->setBackground(Qt::red);
        int rowNumber = rowPtr->row();
        ui->iptcTableWidget->item(rowNumber, 0)->setForeground(Qt::red);

        if (ui->iptcTableWidget->item(rowNumber, 1)->text().length() < 1) {
            ui->iptcTableWidget->item(rowNumber, 1)->setBackground(Qt::red);
            ui->iptcTableWidget->item(rowNumber, 1)->setForeground(Qt::black);
        }
    }
}

void PictureWidget::markIptcCopyrightCell()
{
    QList<QTableWidgetItem *> LTempTable = ui->iptcTableWidget->findItems("Copyright",
                                                                          Qt::MatchEndsWith);

    if (LTempTable.isEmpty()) {
        ui->iptcTableWidget->insertRow(ui->iptcTableWidget->rowCount());

        ui->iptcTableWidget->setItem(ui->iptcTableWidget->rowCount() - 1,
                                     0,
                                     new QTableWidgetItem("Iptc.Application2.Copyright"));

        QTableWidgetItem *tlbCol2val = new QTableWidgetItem();
        tlbCol2val->setText("");
        //tlbCol2val->setTextAlignment(Qt::AlignRight);
        ui->iptcTableWidget->setItem(ui->iptcTableWidget->rowCount() - 1, 1, tlbCol2val);

        tlbCol2val = nullptr;
        ui->iptcTableWidget->resizeColumnsToContents();
    }

    QTableWidgetItem *rowPtr = new QTableWidgetItem();
    LTempTable = ui->iptcTableWidget->findItems("Copyright", Qt::MatchEndsWith);
    foreach (rowPtr, LTempTable) {
        //rowPtr->setBackground(Qt::red);
        int rowNumber = rowPtr->row();
        ui->iptcTableWidget->item(rowNumber, 0)->setForeground(Qt::red);

        if (ui->iptcTableWidget->item(rowNumber, 1)->text().length() < 1) {
            ui->iptcTableWidget->item(rowNumber, 1)->setBackground(Qt::red);
            ui->iptcTableWidget->item(rowNumber, 1)->setForeground(Qt::black);
        }
    }
}

void PictureWidget::createRotateMenu()
{
    ui->rotateSrcImg_Btn->setIcon(QIcon(":/resources/img/icons8-available-updates-50.png"));

    rotateMnu = new QMenu();
    //#ifdef __APPLE__

    rotate_90 = new QAction(QIcon(":/resources/img/icons8-rotate-right-50.png"),
                            tr("rotate") + " 90°",
                            this);
    rotate_90->setIconVisibleInMenu(true);
    connect(rotate_90, &QAction::triggered, this, [this] { PictureWidget::rotateSrcImg(90); });
    rotateMnu->addAction(rotate_90);

    rotate_120 = new QAction(QIcon(":/resources/img/icons8-rotate-right-50.png"),
                             tr("rotate") + " 120°",
                             this);
    rotate_120->setIconVisibleInMenu(true);
    connect(rotate_120, &QAction::triggered, this, [this] { PictureWidget::rotateSrcImg(120); });
    rotateMnu->addAction(rotate_120);

    rotateMnu->addSeparator();

    rotate_m90 = new QAction(QIcon(":/resources/img/icons8-rotate-left-50.png"),
                             tr("rotate") + " -90°",
                             this);
    rotate_m90->setIconVisibleInMenu(true);
    connect(rotate_m90, &QAction::triggered, this, [this] { PictureWidget::rotateSrcImg(-90); });
    rotateMnu->addAction(rotate_m90);

    rotate_m120 = new QAction(QIcon(":/resources/img/icons8-rotate-left-50.png"),
                              tr("rotate") + " -120°",
                              this);
    rotate_m120->setIconVisibleInMenu(true);
    connect(rotate_m120, &QAction::triggered, this, [this] { PictureWidget::rotateSrcImg(-120); });
    rotateMnu->addAction(rotate_m120);

    ui->rotateSrcImg_Btn->setMenu(rotateMnu);
}

void PictureWidget::createExportMenu()
{
    exportMnu = new QMenu();
    exportMnu->setIcon(QIcon(":/resources/img/icons8-send-file-50.png"));

    webp_size_480 = new QAction(QIcon(":/resources/img/icons8-ausgang-48.png"),
                                tr("export to size") + " 480",
                                this);
    webp_size_480->setIconVisibleInMenu(true);
    connect(webp_size_480, &QAction::triggered, this, [this] {
        PictureWidget::exportSrcImgToWebP(480);
    });
    exportMnu->addAction(webp_size_480);

    webp_size_640 = new QAction(QIcon(":/resources/img/icons8-ausgang-48.png"),
                                tr("export to size") + " 640",
                                this);
    webp_size_640->setIconVisibleInMenu(true);
    connect(webp_size_640, &QAction::triggered, this, [this] {
        PictureWidget::exportSrcImgToWebP(640);
    });
    exportMnu->addAction(webp_size_640);

    webp_size_800 = new QAction(QIcon(":/resources/img/icons8-ausgang-48.png"),
                                tr("export to size") + " 800",
                                this);
    webp_size_800->setIconVisibleInMenu(true);
    connect(webp_size_800, &QAction::triggered, this, [this] {
        PictureWidget::exportSrcImgToWebP(800);
    });
    exportMnu->addAction(webp_size_800);

    webp_size_1024 = new QAction(QIcon(":/resources/img/icons8-ausgang-48.png"),
                                 tr("export to size") + " 1024",
                                 this);
    webp_size_1024->setIconVisibleInMenu(true);
    connect(webp_size_1024, &QAction::triggered, this, [this] {
        PictureWidget::exportSrcImgToWebP(1024);
    });
    exportMnu->addAction(webp_size_1024);

    webp_size_1280 = new QAction(QIcon(":/resources/img/icons8-ausgang-48.png"),
                                 tr("export to size") + " 1280",
                                 this);
    webp_size_1280->setIconVisibleInMenu(true);
    connect(webp_size_1280, &QAction::triggered, this, [this] {
        PictureWidget::exportSrcImgToWebP(1280);
    });
    exportMnu->addAction(webp_size_1280);

    exportMnu->addSeparator();

    webp_size_all = new QAction(QIcon(":/resources/img/icons8-send-file-50.png"),
                                tr("export to all sizes"),
                                this);
    webp_size_all->setIconVisibleInMenu(true);
    connect(webp_size_all, &QAction::triggered, this, [this] {
        PictureWidget::exportSrcImgToWebpThread();
    });
    exportMnu->addAction(webp_size_all);

    exportMnu->addSeparator();

    webp_oversizeAct = new QAction(tr("increase too small picture"), this);
    webp_oversizeAct->setIconVisibleInMenu(true);
    webp_oversizeAct->setCheckable(true);
    webp_oversizeAct->setChecked(true);
    exportMnu->addAction(webp_oversizeAct);

    webp_overwriteWebpAct = new QAction(tr("overwrite existing WebP"), this);
    webp_overwriteWebpAct->setIconVisibleInMenu(true);
    webp_overwriteWebpAct->setCheckable(true);
    webp_overwriteWebpAct->setChecked(false);
    exportMnu->addAction(webp_overwriteWebpAct);

    webp_watermarkWebpAct = new QAction(tr("watermark WebP"), this);
    webp_watermarkWebpAct->setIconVisibleInMenu(true);
    webp_watermarkWebpAct->setCheckable(true);
    webp_watermarkWebpAct->setChecked(true);
    exportMnu->addAction(webp_watermarkWebpAct);

    ui->exportSrcToWebp_Btn->setMenu(exportMnu);
}

void PictureWidget::_on_resizeSmallerImage_Btn_clicked()
{
    /*
    int wSize = ui->picture_label->width();
    int hSize = ui->picture_label->height();

    ui->picture_label->setPixmap(picture->scaled(QSize(wSize - 200, hSize - 200),
                                                 Qt::KeepAspectRatio,
                                                 Qt::SmoothTransformation));
*/
    resize(width() - 100, height() - 100);
}

void PictureWidget::_on_resizeBiggerImage_Btn_clicked()
{
    resize(width() + 100, height() + 100);
}

void PictureWidget::on_exifTableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    bool ok;

    int row = item->row();
    qDebug() << "row: " << row << " val 0: " << ui->exifTableWidget->item(row, 0)->text();

    //setOkButtonText("save");
    QString text = QInputDialog::getText(this,
                                         tr("Edit Metadata"),
                                         tr("Please enter the new value for") + " "
                                             + ui->exifTableWidget->item(row, 0)->text() + ":",
                                         QLineEdit::Normal,
                                         ui->exifTableWidget->item(row, 1)->text(),
                                         &ok);
    if (ok && !text.isEmpty()) {
        ui->exifTableWidget->setItem(row, 1, new QTableWidgetItem(text));

        QFile file(pathToImage);
        std::string key = ui->exifTableWidget->item(row, 0)->text().toStdString();

        try {
            Exiv2::XmpParser::initialize();
            ::atexit(Exiv2::XmpParser::terminate);

            auto exif_image = Exiv2::ImageFactory::open(file.fileName().toUtf8().toStdString());
            exif_image->readMetadata();
            Exiv2::ExifData &exifData = exif_image->exifData();
            exifData[key] = text.toStdString();

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
            file.close();
            ui->exifTableWidget->resizeColumnsToContents();
        } catch (Exiv2::Error &e) {
            qDebug() << "Caught Exiv2 exception " << e.what() << "\n";
        }
    }
}

void PictureWidget::on_iptcTableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    bool ok;

    int row = item->row();
    qDebug() << "row: " << row << " val 0: " << ui->iptcTableWidget->item(row, 0)->text();

    /* KISS
    QInputDialog inDialog(this);
    inDialog.setOkButtonText("save");
    inDialog.setLabelText(ui->iptcTableWidget->item(row, 0)->text());
    inDialog.setTextValue(ui->iptcTableWidget->item(row, 1)->text());
    inDialog.setTextEchoMode(QLineEdit::Normal);
    */

    QString text = QInputDialog::getText(this,
                                         tr("Edit Metadata"),
                                         tr("Please enter the new value for") + " "
                                             + ui->iptcTableWidget->item(row, 0)->text() + ":",
                                         QLineEdit::Normal,
                                         ui->iptcTableWidget->item(row, 1)->text(),
                                         &ok);
    if (ok && !text.isEmpty()) {
        ui->iptcTableWidget->setItem(row, 1, new QTableWidgetItem(text));

        QFile file(pathToImage);
        std::string key = ui->iptcTableWidget->item(row, 0)->text().toStdString();
        auto iptc_image = Exiv2::ImageFactory::open(file.fileName().toUtf8().toStdString());
        iptc_image->readMetadata();
        Exiv2::IptcData &iptcData = iptc_image->iptcData();
        iptcData[key] = text.toStdString();
        iptc_image->setIptcData(iptcData);
        iptc_image->writeMetadata();
        file.close();
        ui->iptcTableWidget->resizeColumnsToContents();
    }
}

void PictureWidget::rotateSrcImg(int val)
{
    Photo *photo = new Photo(pathToImage);
    photo->rotateImage(val);
    photo = nullptr;

    picture = new QPixmap(pathToImage);

    ui->picture_label->setPixmap(
        picture->scaled(ui->picture_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    photo = nullptr;
}

void PictureWidget::exportSrcImgToWebpThread()
{
    bool oknok{false};
    //Photo *photo = new Photo(pathToImage);
    Photo phot(pathToImage);

    if (webp_oversizeAct->isChecked()) {
        phot.setOversizeSmallerPicture(true);
    }

    if (webp_overwriteWebpAct->isChecked()) {
        phot.setOverwriteExistingWebp(true);
    }

    if (webp_watermarkWebpAct->isChecked()) {
        phot.setWatermarkWebp(true);
    }

    QFuture<bool> future = QtConcurrent::run(&Photo::convertImages, phot, 75);

    oknok = future.result();

    QMessageBox msgBox(this);
    msgBox.setTextFormat(Qt::RichText);
    QString text = tr("WebP export to subfolder WebP");
    if (!oknok) {
        msgBox.setWindowTitle(tr("Error"));
        msgBox.setIcon(QMessageBox::Warning);
        text += " " + tr("failed");
    } else {
        msgBox.setWindowTitle(tr("Success"));
        msgBox.setIcon(QMessageBox::Information);
        text += " " + tr("successfull");
    }
    msgBox.setText(text);
    msgBox.setInformativeText("<i>" + pathToImage + "</i>");
    msgBox.setFixedWidth(900);
    msgBox.exec();
}

void PictureWidget::exportSrcImgToWebP(int size)
{
    QMessageBox msgBox(this);
    msgBox.setTextFormat(Qt::RichText);
    QString text = tr("WebP export to subfolder WebP");

    Photo *photo = new Photo(pathToImage);

    if (webp_oversizeAct->isChecked()) {
        photo->setOversizeSmallerPicture(true);
    }

    if (webp_overwriteWebpAct->isChecked()) {
        photo->setOverwriteExistingWebp(true);
    }

    if (webp_watermarkWebpAct->isChecked()) {
        photo->setWatermarkWebp(true);
    }

    if (!photo->convertImage(size)) {
        msgBox.setWindowTitle(tr("Error"));
        msgBox.setIcon(QMessageBox::Warning);
        text += " " + tr("failed");
    } else {
        msgBox.setWindowTitle(tr("Success"));
        msgBox.setIcon(QMessageBox::Information);
        text += " " + tr("successfull");
    }
    msgBox.setText(text);
    msgBox.setInformativeText("<i>" + pathToImage + "</i>");
    msgBox.setFixedWidth(900);
    msgBox.exec();
    photo = nullptr;
}

void PictureWidget::on_tabWidget_tabBarClicked(int index)
{
    ui->exifTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->exifTableWidget->resizeColumnsToContents();
    ui->exifTableWidget->resizeRowsToContents();

    ui->iptcTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->iptcTableWidget->resizeColumnsToContents();
    ui->iptcTableWidget->resizeRowsToContents();

    ui->xmpTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->xmpTableWidget->resizeColumnsToContents();
    ui->xmpTableWidget->resizeRowsToContents();

    ui->tabWidget->adjustSize();
}
