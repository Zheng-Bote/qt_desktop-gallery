#include "defaultmeta_widget.h"
#include <QInputDialog>
#include <QMessageBox>
#include "ui_defaultmeta_widget.h"

#include "includes/rz_metadata.hpp"

DefaultMeta::DefaultMeta(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DefaultMeta)
{
    ui->setupUi(this);

    createXmpTableWidget();
    createExifTableWidget();
}

DefaultMeta::~DefaultMeta()
{
    delete ui;
}

void DefaultMeta::setDefaultMeta(rz_metaDefaultData::defaultMetaStruct &_defaultMetaPtr)
{
    defaultMetaPtr = &_defaultMetaPtr;

    QHashIterator<QString, QString> xmp(_defaultMetaPtr.xmpDefault);
    while (xmp.hasNext()) {
        xmp.next();
        markXmp(xmp.key());
    }

    QHashIterator<QString, QString> exif(_defaultMetaPtr.exifDefault);
    while (exif.hasNext()) {
        exif.next();
        markExif(exif.key());
    }
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

void DefaultMeta::createXmpTableWidget()
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

    for (auto i = rz_metaDefaultData::xmpDefaultTags.cbegin(),
              end = rz_metaDefaultData::xmpDefaultTags.cend();
         i != end;
         ++i) {
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

    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();
}

void DefaultMeta::createExifTableWidget()
{
    QFont font_11_bold("Times New Roman", 11);
    font_11_bold.setBold(true);

    ui->exif_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->exif_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->exif_tableWidget->insertRow(0);
    ui->exif_tableWidget->insertColumn(0);
    ui->exif_tableWidget->insertColumn(1);
    ui->exif_tableWidget->insertColumn(2);

    QTableWidgetItem *tlbCol1 = new QTableWidgetItem();
    tlbCol1->setText(tr("Key"));
    tlbCol1->setBackground(Qt::lightGray);
    tlbCol1->setFont(font_11_bold);
    tlbCol1->setTextAlignment(Qt::AlignCenter);
    ui->exif_tableWidget->setHorizontalHeaderItem(0, tlbCol1);

    QTableWidgetItem *tlbCol2 = new QTableWidgetItem();
    tlbCol2->setText(tr("Value"));
    tlbCol2->setBackground(Qt::lightGray);
    tlbCol2->setFont(font_11_bold);
    tlbCol2->setTextAlignment(Qt::AlignCenter);
    ui->exif_tableWidget->setHorizontalHeaderItem(1, tlbCol2);

    QTableWidgetItem *tlbCol3 = new QTableWidgetItem();
    tlbCol3->setText(tr("Description"));
    tlbCol3->setBackground(Qt::lightGray);
    tlbCol3->setFont(font_11_bold);
    tlbCol3->setTextAlignment(Qt::AlignCenter);
    ui->exif_tableWidget->setHorizontalHeaderItem(2, tlbCol3);

    ui->exif_tableWidget->removeRow(0);

    for (auto i = rz_metaDefaultData::exifDefaultTags.cbegin(),
              end = rz_metaDefaultData::exifDefaultTags.cend();
         i != end;
         ++i) {
        //i.key()
        //i.value()
        ui->exif_tableWidget->insertRow(ui->exif_tableWidget->rowCount());

        QTableWidgetItem *tlbCol1val = new QTableWidgetItem();
        tlbCol1val->setText(i.key());
        tlbCol1val->setFlags(tlbCol1val->flags() ^ Qt::ItemIsEditable);
        ui->exif_tableWidget->setItem(ui->exif_tableWidget->rowCount() - 1, 0, tlbCol1val);
        tlbCol1val = nullptr;

        QTableWidgetItem *tlbCol2val = new QTableWidgetItem();
        tlbCol2val->setText("");

        //tlbCol2val->setTextAlignment(Qt::AlignRight);
        ui->exif_tableWidget->setItem(ui->exif_tableWidget->rowCount() - 1, 1, tlbCol2val);
        tlbCol2val = nullptr;

        QTableWidgetItem *tlbCol3val = new QTableWidgetItem();
        tlbCol3val->setText(i.value());
        tlbCol3val->setFlags(tlbCol3val->flags() ^ Qt::ItemIsEditable);
        ui->exif_tableWidget->setItem(ui->exif_tableWidget->rowCount() - 1, 2, tlbCol3val);
        tlbCol3val = nullptr;
    }

    ui->exif_tableWidget->resizeColumnsToContents();
    ui->exif_tableWidget->resizeRowsToContents();
}

void DefaultMeta::markXmp(QString searchFor)
{
    QList<QTableWidgetItem *> LTempTable = ui->tableWidget->findItems(searchFor, Qt::MatchEndsWith);

    if (LTempTable.isEmpty()) {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());

        QTableWidgetItem *tlbCol1val = new QTableWidgetItem();
        tlbCol1val->setText(searchFor);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, tlbCol1val);
        tlbCol1val = nullptr;
    }

    QTableWidgetItem *rowPtr = new QTableWidgetItem();
    LTempTable = ui->tableWidget->findItems(searchFor, Qt::MatchEndsWith);
    foreach (rowPtr, LTempTable) {
        int rowNumber = rowPtr->row();
        QTableWidgetItem *tlbCol2val = new QTableWidgetItem();
        tlbCol2val->setText(defaultMetaPtr->xmpDefault.value(searchFor));
        ui->tableWidget->setItem(rowNumber, 1, tlbCol2val);
        tlbCol2val = nullptr;
    }
}

void DefaultMeta::markExif(QString searchFor)
{
    QList<QTableWidgetItem *> LTempTable = ui->exif_tableWidget->findItems(searchFor,
                                                                           Qt::MatchEndsWith);

    if (LTempTable.isEmpty()) {
        ui->exif_tableWidget->insertRow(ui->exif_tableWidget->rowCount());

        QTableWidgetItem *tlbCol1val = new QTableWidgetItem();
        tlbCol1val->setText(searchFor);
        ui->exif_tableWidget->setItem(ui->exif_tableWidget->rowCount() - 1, 1, tlbCol1val);
        tlbCol1val = nullptr;
    }

    QTableWidgetItem *rowPtr = new QTableWidgetItem();
    LTempTable = ui->exif_tableWidget->findItems(searchFor, Qt::MatchEndsWith);
    foreach (rowPtr, LTempTable) {
        int rowNumber = rowPtr->row();
        QTableWidgetItem *tlbCol2val = new QTableWidgetItem();
        tlbCol2val->setText(defaultMetaPtr->exifDefault.value(searchFor));
        ui->exif_tableWidget->setItem(rowNumber, 1, tlbCol2val);
        tlbCol2val = nullptr;
    }
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

    if (ok && !text.isEmpty()) {
        dataModified_bool = true;
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(text));
        defaultMetaPtr->xmpDefault[ui->tableWidget->item(row, 0)->text()] = text;
        ui->tableWidget->resizeColumnsToContents();
        ui->tableWidget->resizeRowsToContents();
    }
}

void DefaultMeta::on_exif_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    bool ok;

    int row = item->row();

    QString text = QInputDialog::getText(this,
                                         tr("Edit Metadata"),
                                         tr("Please enter the new value for") + " "
                                             + ui->exif_tableWidget->item(row, 0)->text() + ":",
                                         QLineEdit::Normal,
                                         ui->exif_tableWidget->item(row, 1)->text(),
                                         &ok);
    if (ok && !text.isEmpty()) {
        dataModified_bool = true;
        ui->exif_tableWidget->setItem(row, 1, new QTableWidgetItem(text));
        defaultMetaPtr->exifDefault[ui->exif_tableWidget->item(row, 0)->text()] = text;

        ui->exif_tableWidget->resizeColumnsToContents();
        ui->exif_tableWidget->resizeRowsToContents();
    }
}

void DefaultMeta::resizeEvent(QResizeEvent *event)
{
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();

    ui->exif_tableWidget->resizeColumnsToContents();
    ui->exif_tableWidget->resizeRowsToContents();
}
