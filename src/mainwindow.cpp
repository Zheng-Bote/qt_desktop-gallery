#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>

#include "./picture_widget.h"
#include "./ui_mainwindow.h"
#include "includes/rz_photo.hpp"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFont(*font_12);
    ui->album_label->clear();

    //setWindowTitle(tr(qApp->applicationDisplayName().toStdString().c_str()) + " v"
    //               + qApp->applicationVersion().toStdString().c_str());
    //setWindowIcon(QIcon(":/res/images/icon.png"));

    createMenu();
    initListview();
    initStatusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openReadFolder()
{
    QDir dir = QFileDialog::getExistingDirectory(this,
                                                 tr("Open Folder"),
                                                 QDir::homePath(),
                                                 QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);

    QStringList filenames = dir.entryList(QStringList() << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp"
                                                        << "*.tiff",
                                          QDir::Files | QDir::NoSymLinks | QDir::NoDot
                                              | QDir::Readable);
    if (!filenames.isEmpty()) {
        for (auto filename : filenames) {
            QPixmap pixmap(dir.absoluteFilePath(filename));
            QStandardItem *listitem = new QStandardItem();
            listitem->setIcon(pixmap);
            listitem->setText(filename + "\nschaun wa ma");
            listitem->setToolTip(filename);
            listitem->setBackground(Qt::lightGray);
            listitem->setEditable(false);

            QList<QStandardItem *> items;
            items.append(listitem);
            items.append(new QStandardItem(dir.absoluteFilePath(filename)));
            mContentItemModel->appendRow(items);

            //mContentItemModel->appendRow(listitem);
        }
    }
}

void MainWindow::openSrcFolder()
{
    QString srcPath = QFileDialog::getExistingDirectory(this,
                                                        tr("Open Folder"),
                                                        QDir::homePath(),
                                                        QFileDialog::ShowDirsOnly
                                                            | QFileDialog::DontResolveSymlinks);

    QDirIterator srcPics(srcPath,
                         {"*.jpg", "*.jpeg", "*.png", "*.bmp", "*.tiff"},
                         QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable);

    while (srcPics.hasNext()) {
        QFile srcFile(srcPics.next());
        fillSrcListView(srcFile);
    }
}

void MainWindow::openSrcFolderRekursive()
{
    QString srcPath = QFileDialog::getExistingDirectory(this,
                                                        tr("Open Folder"),
                                                        QDir::homePath(),
                                                        QFileDialog::ShowDirsOnly
                                                            | QFileDialog::DontResolveSymlinks);

    QDirIterator srcPics(srcPath,
                         {"*.jpg", "*.jpeg", "*.png", "*.bmp", "*.tiff"},
                         QDir::Files,
                         QDirIterator::Subdirectories);

    while (srcPics.hasNext()) {
        QFile srcFile(srcPics.next());
        fillSrcListView(srcFile);
    }
}

void MainWindow::loadSingleSrcImg()
{
    QString pathToFile = QFileDialog::getOpenFileName(this,
                                                      tr("Open File"),
                                                      QDir::homePath(),
                                                      tr("Image (*.jpg *.jpeg *.png *.bmp *.tiff"));

    if (pathToFile.isEmpty() == false) {
        QFile srcFile(pathToFile);
        fillSrcListView(srcFile);
    }
}

void MainWindow::clearSrcAlbum()
{
    if (mContentItemModel->rowCount() < 1) {
        return;
    }
    QMessageBox::StandardButton response = QMessageBox::Cancel;

    response = QMessageBox::question(this,
                                     tr("Confirmation"),
                                     tr("Are you sure you want to clear the album?"),
                                     QMessageBox::Yes | QMessageBox::Cancel);

    if (response == QMessageBox::Yes) {
        mContentItemModel->clear();
        //statusBarLabel->setText("0 " + tr("items"));
        statusBarLabel->setText("v" + qApp->applicationVersion());
    }
}

void MainWindow::about()
{
    QString text = tr("File Encryption and Decryption") + "\n\n";
    QString setInformativeText = "<p><ul><li>" + tr("Encryption") + ": AES-256 CBC</li><li>"
                                 + tr("Password") + ": SHA256, " + tr("5 to 32 characters")
                                 + "</li><li>initialization vector: MD5</li></ul></p>";
    setInformativeText.append("<p><ul><li>" + QString("PROG_EXEC_NAME") + " v "
                              + QString("PROG_VERSION") + "</li><li>"
                              + tr("Desktop application for Linux, MacOS and Windows")
                              + "</li><li>Copyright (c) 2024 ZHENG Robert</li><li>OSS MIT "
                              + tr("license") + "</li></ul></p>");
    setInformativeText.append(
        "<br><a href=\"https://github.com/Zheng-Bote/qt_file_encryption-decryption\" alt=\"Github "
        "repository\">");
    setInformativeText.append(qApp->applicationName() + " v" + qApp->applicationVersion() + " "
                              + tr("at") + " Github</a>");

    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("About") + " " + tr("PROG_NAME"));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(text);
    msgBox.setInformativeText(setInformativeText);
    msgBox.setFixedWidth(900);
    msgBox.exec();
}

void MainWindow::createMenu()
{
    menuBar()->setNativeMenuBar(false);

    // Album
    ui->actionsingle_Folder->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));
    connect(ui->actionsingle_Folder, &QAction::triggered, this, &MainWindow::openSrcFolder);
    connect(ui->actionrekursive_Folders,
            &QAction::triggered,
            this,
            &MainWindow::openSrcFolderRekursive);
    connect(ui->actionclear_Album, &QAction::triggered, this, &MainWindow::clearSrcAlbum);

    // Picture
    ui->actionload_Picture->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_P));
    connect(ui->actionload_Picture, &QAction::triggered, this, &MainWindow::loadSingleSrcImg);

    // About - Info
    aboutAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout), tr("&About"), this);
    aboutAct->setShortcuts(QKeySequence::WhatsThis);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
    infoMenu = menuBar()->addMenu(tr("&Info"));
    infoMenu->addAction(aboutAct);
}

void MainWindow::initListview()
{
    mContentItemModel = new QStandardItemModel(this);
    mContentItemModel->setColumnCount(2);

    ui->listView->setModel(mContentItemModel);
    ui->listView->clearSelection();
    //ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listView->setWrapping(true);
    ui->listView->setSpacing(5);
    ui->listView->setResizeMode(QListView::Adjust);
    ui->listView->setFlow(QListView::LeftToRight);
    ui->listView->setIconSize(QSize(THUMBNAIL_SIZE, THUMBNAIL_SIZE));
}

void MainWindow::initStatusBar()
{
    statusBarLabel = new QLabel();
    statusBarLabel->setFont(*font_10);
    statusBarLabel->setAlignment(Qt::AlignCenter);
    statusBarLabel->setText("v" + qApp->applicationVersion());
    statusBar()->addWidget(statusBarLabel, 1);
}

void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    // alles OK!!

    QString itemText = index.data(Qt::DisplayRole).toString();
    qDebug() << "doubleclicked: " << itemText << " col: " << index.column();

    int row = index.row();
    QModelIndex col2 = mContentItemModel->index(row, 1);
    QString itemText2 = col2.data(Qt::DisplayRole).toString();
    qDebug() << "doubleclicked: " << itemText2 << " col: " << col2.column();
    showSinglePicure(itemText2);

    QModelIndexList selected = ui->listView->selectionModel()->selectedIndexes();
    for (QModelIndex i : selected) {
        int row = i.row();
        QModelIndex col2 = mContentItemModel->index(row, 1);
        QString itemText2 = col2.data(Qt::DisplayRole).toString();
        qDebug() << "selected: " << itemText2 << " col: " << col2.column();
    }
}

void MainWindow::fillSrcListView(QFile &srcFile)
{
    QFileInfo fileInfo(srcFile.fileName());

    QString pathToFile = fileInfo.absolutePath() + "/" + fileInfo.fileName();
    QPixmap pixmap(pathToFile);
    QStandardItem *listitem = new QStandardItem();
    listitem->setIcon(pixmap);
    listitem->setText(fileInfo.fileName() + "\nschaun wa ma");
    listitem->setToolTip(pathToFile);
    listitem->setBackground(Qt::lightGray);
    listitem->setEditable(false);

    QList<QStandardItem *> items;
    items.append(listitem);
    items.append(new QStandardItem(pathToFile));
    mContentItemModel->appendRow(items);

    ui->album_label->setFont(*font_10);
    ui->album_label->setText(tr("Doubleclick on a picture for details. Press CTRL and click to "
                                "select one or more pictures."));

    int rowCount = mContentItemModel->rowCount();
    statusBarLabel->setText(QString::number(rowCount) + " " + tr("items"));
}

void MainWindow::showSinglePicure(QString pathToFile)
{
    PictureWidget *pictureWidget = new PictureWidget();
    pictureWidget->setImage(pathToFile);
    pictureWidget->show();
}
