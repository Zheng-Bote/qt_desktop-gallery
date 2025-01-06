#include "picture_widget.h"
#include <QInputDialog>
#include <QMessageBox>
#include "ui_picture_widget.h"

#include <exiv2/exiv2.hpp>

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

    createRotateMenu();
    createExportMenu();
}

PictureWidget::~PictureWidget()
{
    delete ui;
}

void PictureWidget::setImage(QString pathToFile)
{
    pathToImage = pathToFile;
    ui->filename_label->setText(pathToFile);
    picture = new QPixmap(pathToImage);

    ui->picture_label->setPixmap(
        picture->scaled(ui->picture_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    readSrcExif();
    readSrcIptc();
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

    qDebug() << "checkValidMetaImg Extension: " << photoExtension;

    if (validMetaImageTypes.contains(photoExtension)) {
        qDebug() << "checkValidMetaImg has: " << photoExtension;
        ret = true;
    } else {
        qDebug() << "checkValidMetaImg has no: " << photoExtension;
        ret = false;
    }

    photo = nullptr;
    return ret;
}

void PictureWidget::readSrcExif()
{
    if (!checkValidMetaImg()) {
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
        }
    }

    ui->exifTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->exifTableWidget->resizeColumnsToContents();

    ui->tabWidget->adjustSize();
    file.close();
    //markExifCopyrightCell();
}

void PictureWidget::readSrcIptc()
{
    if (!checkValidMetaImg()) {
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
        }
    }

    ui->iptcTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->iptcTableWidget->resizeColumnsToContents();

    ui->tabWidget->adjustSize();
    file.close();
    //markIptcCopyrightCell();
}

void PictureWidget::createRotateMenu()
{
    rotateMnu = new QMenu();
    //#ifdef __APPLE__

    rotate_90 = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::ObjectRotateRight),
                            tr("rotate") + " 90°",
                            this);
    connect(rotate_90, &QAction::triggered, this, [this] { PictureWidget::rotateSrcImg(90); });
    rotateMnu->addAction(rotate_90);
    rotate_m90 = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::ObjectRotateLeft),
                             tr("rotate") + " -90°",
                             this);
    connect(rotate_m90, &QAction::triggered, this, [this] { PictureWidget::rotateSrcImg(-90); });
    rotateMnu->addAction(rotate_m90);
    rotate_120 = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::ObjectRotateRight),
                             tr("rotate") + " 120°",
                             this);
    connect(rotate_120, &QAction::triggered, this, [this] { PictureWidget::rotateSrcImg(120); });
    rotateMnu->addAction(rotate_120);
    rotate_m120 = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::ObjectRotateLeft),
                              tr("rotate") + " -120°",
                              this);
    connect(rotate_m120, &QAction::triggered, this, [this] { PictureWidget::rotateSrcImg(-120); });
    rotateMnu->addAction(rotate_m120);

    ui->rotateSrcImg_Btn->setMenu(rotateMnu);
}

void PictureWidget::createExportMenu()
{
    exportMnu = new QMenu();

    webp_size_all = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSend),
                                tr("export to all sizes"),
                                this);
    connect(webp_size_all, &QAction::triggered, this, [this] {
        PictureWidget::exportSrcImgToWebP();
    });
    exportMnu->addAction(webp_size_all);

    webp_size_480 = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSend),
                                tr("export to size") + " 480",
                                this);
    connect(webp_size_480, &QAction::triggered, this, [this] {
        PictureWidget::exportSrcImgToWebP(480);
    });
    exportMnu->addAction(webp_size_480);

    webp_size_640 = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSend),
                                tr("export to size") + " 640",
                                this);
    connect(webp_size_640, &QAction::triggered, this, [this] {
        PictureWidget::exportSrcImgToWebP(640);
    });
    exportMnu->addAction(webp_size_640);

    webp_size_800 = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSend),
                                tr("export to size") + " 800",
                                this);
    connect(webp_size_800, &QAction::triggered, this, [this] {
        PictureWidget::exportSrcImgToWebP(800);
    });
    exportMnu->addAction(webp_size_800);

    ui->exportSrcToWebp_Btn->setMenu(exportMnu);
}

void PictureWidget::on_resizeSmallerImage_Btn_clicked()
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

void PictureWidget::on_resizeBiggerImage_Btn_clicked()
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
        auto exif_image = Exiv2::ImageFactory::open(file.fileName().toUtf8().toStdString());
        exif_image->readMetadata();
        Exiv2::ExifData &exifData = exif_image->exifData();
        exifData[key] = text.toStdString();
        exif_image->setExifData(exifData);
        exif_image->writeMetadata();
        file.close();
        ui->exifTableWidget->resizeColumnsToContents();
    }
}

void PictureWidget::on_iptcTableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    bool ok;

    int row = item->row();
    qDebug() << "row: " << row << " val 0: " << ui->iptcTableWidget->item(row, 0)->text();

    //setOkButtonText("save");
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

void PictureWidget::exportSrcImgToWebP()
{
    QMessageBox msgBox(this);
    msgBox.setTextFormat(Qt::RichText);
    QString text = tr("WebP export to subfolder WebP");

    Photo *photo = new Photo(pathToImage);
    if (!photo->convertImages()) {
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

void PictureWidget::exportSrcImgToWebP(int size)
{
    QMessageBox msgBox(this);
    msgBox.setTextFormat(Qt::RichText);
    QString text = tr("WebP export to subfolder WebP");

    Photo *photo = new Photo(pathToImage);
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
