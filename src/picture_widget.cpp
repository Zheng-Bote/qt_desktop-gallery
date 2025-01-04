#include "picture_widget.h"
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
            tlbCol2val->setTextAlignment(Qt::AlignRight);
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
            tlbCol2val->setTextAlignment(Qt::AlignRight);
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
