#include "defaultmeta_widget.h"
#include <QInputDialog>
#include <QMessageBox>
#include "ui_defaultmeta_widget.h"

#include "includes/rz_photo.hpp"

DefaultMeta::DefaultMeta(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DefaultMeta)
{
    ui->setupUi(this);

    createTableWidget();
}

DefaultMeta::~DefaultMeta()
{
    delete ui;
}

void DefaultMeta::closeEvent(QCloseEvent *event)
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
            }
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
    //QWidget::closeEvent(event);
}

void DefaultMeta::createTableWidget()
{
    QFont font_11_bold("Times New Roman", 11);
    font_11_bold.setBold(true);

    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->tableWidget->insertRow(0);
    ui->tableWidget->insertColumn(0);
    ui->tableWidget->insertColumn(1);
    ui->tableWidget->insertColumn(2);

    QTableWidgetItem *tlbCol1 = new QTableWidgetItem();
    tlbCol1->setText(tr("Key"));
    tlbCol1->setBackground(Qt::lightGray);
    tlbCol1->setFont(font_11_bold);
    tlbCol1->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setHorizontalHeaderItem(0, tlbCol1);

    QTableWidgetItem *tlbCol2 = new QTableWidgetItem();
    tlbCol2->setText(tr("Value"));
    tlbCol2->setBackground(Qt::lightGray);
    tlbCol2->setFont(font_11_bold);
    tlbCol2->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setHorizontalHeaderItem(1, tlbCol2);

    QTableWidgetItem *tlbCol3 = new QTableWidgetItem();
    tlbCol3->setText(tr("Description"));
    tlbCol3->setBackground(Qt::lightGray);
    tlbCol3->setFont(font_11_bold);
    tlbCol3->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setHorizontalHeaderItem(2, tlbCol3);

    ui->tableWidget->removeRow(0);

    Photo photo;
    for (auto i = photo.xmpMetaTags.cbegin(), end = photo.xmpMetaTags.cend(); i != end; ++i) {
        //i.key()
        //i.value()
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());

        QTableWidgetItem *tlbCol1val = new QTableWidgetItem();
        tlbCol1val->setText(i.key());
        tlbCol1val->setFlags(tlbCol1val->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, tlbCol1val);
        tlbCol1val = nullptr;

        QTableWidgetItem *tlbCol2val = new QTableWidgetItem();
        tlbCol2val->setText("");
        //tlbCol2val->setTextAlignment(Qt::AlignRight);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, tlbCol2val);
        tlbCol2val = nullptr;

        QTableWidgetItem *tlbCol3val = new QTableWidgetItem();
        tlbCol3val->setText(i.value());
        tlbCol3val->setFlags(tlbCol3val->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 2, tlbCol3val);
        tlbCol3val = nullptr;
    }

    for (auto i = photo.exifGpsTags.cbegin(), end = photo.exifGpsTags.cend(); i != end; ++i) {
        //i.key()
        //i.value()
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());

        QTableWidgetItem *tlbCol1val = new QTableWidgetItem();
        tlbCol1val->setText(i.key());
        tlbCol1val->setFlags(tlbCol1val->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, tlbCol1val);
        tlbCol1val = nullptr;

        QTableWidgetItem *tlbCol2val = new QTableWidgetItem();
        tlbCol2val->setText("");
        //tlbCol2val->setTextAlignment(Qt::AlignRight);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, tlbCol2val);
        tlbCol2val = nullptr;

        QTableWidgetItem *tlbCol3val = new QTableWidgetItem();
        tlbCol3val->setText("");
        tlbCol3val->setFlags(tlbCol3val->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 2, tlbCol3val);
        tlbCol3val = nullptr;
    }

    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();
}

void DefaultMeta::on_close_Btn_clicked()
{
    this->close();
}

void DefaultMeta::on_save_Btn_clicked()
{
    // save data in listView

    dataModified_bool = false;
}

void DefaultMeta::on_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    bool ok;

    int row = item->row();

    QString text = QInputDialog::getText(this,
                                         tr("Edit Metadata"),
                                         tr("Please enter the new value for") + " "
                                             + ui->tableWidget->item(row, 0)->text() + ":",
                                         QLineEdit::Normal,
                                         ui->tableWidget->item(row, 1)->text(),
                                         &ok);
    /*
    if (ok && !text.isEmpty()) {
dataModified_bool = true;
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(text));

        //QFile file(pathToImage);
        QString key = ui->tableWidget->item(row, 0)->text();

        // map meta data
        Photo photo;

        if (photo.iptc_to_xmp.contains(key)) {
            //qDebug() << "on_exifTableWidget_itemDoubleClicked: iptc_to_xmp key found: " << key << " " << rz_metaData::iptc_to_xmp[key];
            mapMetaData(file.fileName(), key, text, "iptc");
        }
        if (photo.iptc_to_exif.contains(key)) {
            // qDebug() << "on_exifTableWidget_itemDoubleClicked: iptc_to_exif key found: " << key << " " << rz_metaData::iptc_to_exif[key];
            mapMetaData(file.fileName(), key, text, "iptc");
        }

    }
*/
}
