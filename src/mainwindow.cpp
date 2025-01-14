#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>

#include "./picture_widget.h"
#include "./ui_mainwindow.h"
#include "includes/rz_config.h"
#include "includes/rz_photo.hpp"
#include "mainwindow.h"

#include <QFuture>
#include <QFutureWatcher>
#include <QThread>
#include <QtConcurrent>

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
    createLanguageMenu();
    initListview();
    initStatusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openSrcFolderRekursive()
{
    //statusBarLabel->setText("openReadFolder");
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Open Folder"),
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);

    Photo phot;
    QFutureWatcher<QList<QString>> watcher;
    QFuture<QList<QString>> future = QtConcurrent::run(&Photo::srcPicsRecursive, phot, dir);
    watcher.setFuture(future);
    watcher.waitForFinished();
    QList<QString> result = future.result();

    qDebug() << "found: " << result.size();
    if (result.size() > ALBUM_LIMIT) {
        showAlbumLimitMsg(result.size());
        return;
    }
    ui->album_label->setText(tr("processing") + " " + QString::number(result.size()) + " "
                             + tr("items") + "...");
    statusBarLabel->setText(tr("processing") + " " + QString::number(result.size()) + " "
                            + tr("items") + "...");

    QFuture<void> futureListView = QtConcurrent::run(&MainWindow::fillSrcListViewThread,
                                                     this,
                                                     result);
}

void MainWindow::openSrcFolder()
{
    QString srcPath = QFileDialog::getExistingDirectory(this,
                                                        tr("Open Folder"),
                                                        QDir::homePath(),
                                                        QFileDialog::ShowDirsOnly
                                                            | QFileDialog::DontResolveSymlinks);

    Photo phot;
    QFutureWatcher<QList<QString>> watcher;
    QFuture<QList<QString>> future = QtConcurrent::run(&Photo::srcPics, phot, srcPath);
    watcher.setFuture(future);
    watcher.waitForFinished();
    QList<QString> result = future.result();

    qDebug() << "found: " << result.size();
    if (result.size() > ALBUM_LIMIT) {
        showAlbumLimitMsg(result.size());
        return;
    }

    ui->album_label->setText(tr("processing") + " " + QString::number(result.size()) + " "
                             + tr("items") + "...");
    statusBarLabel->setText(tr("processing") + " " + QString::number(result.size()) + " "
                            + tr("items") + "...");

    QFuture<void> futureListView = QtConcurrent::run(&MainWindow::fillSrcListViewThread,
                                                     this,
                                                     result);

    /*
    QDirIterator srcPics(srcPath,
                         {"*.jpg", "*.jpeg", "*.png", "*.bmp", "*.tiff"},
                         QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable);

    while (srcPics.hasNext()) {
        QFile srcFile(srcPics.next());
        fillSrcListView(srcFile);
    }
    */
}

void MainWindow::openReadFolder()
{
    //    qInfo() << "openSrcFolderRekursive on: " << QThread::currentThread();

    QString srcPath = QFileDialog::getExistingDirectory(this,
                                                        tr("Open Folder"),
                                                        QDir::homePath(),
                                                        QFileDialog::ShowDirsOnly
                                                            | QFileDialog::DontResolveSymlinks);

    QDirIterator srcPics(srcPath,
                         {"*.jpg", "*.jpeg", "*.png", "*.bmp", "*.tiff"},
                         QDir::Files,
                         QDirIterator::Subdirectories);

    qDebug() << "QDiroperator: " << srcPath;

    while (srcPics.hasNext()) {
        QFile srcFile(srcPics.next());
        qDebug() << "while: " << srcFile.fileName();
        statusBarLabel->setText("loading " + srcFile.fileName());
        fillSrcListView(srcFile);
    }
}

void MainWindow::loadSingleSrcImg()
{
    QString pathToFile
        = QFileDialog::getOpenFileName(this,
                                       tr("Open File"),
                                       QDir::homePath(),
                                       tr("Image (*.jpg *.jpeg *.png *.bmp *.tiff)"));

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
        try {
            mContentItemModel->clear();
            resetDefaultMeta();
            refreshStatusBar();
        } catch (...) {
            qDebug() << "clearSrcAlbum: something went wrong";
        }
    }
}

void MainWindow::removeSelectedImageFromAlbum(const QModelIndex &index)
{
    mContentItemModel->removeRow(index.row());
    resetDefaultMeta();
    refreshStatusBar();
}

void MainWindow::about()
{
    QString text = tr(qApp->applicationDisplayName().toStdString().c_str()) + "\n\n";
    QString setInformativeText = "<p>" + qApp->applicationName() + " v" + qApp->applicationVersion()
                                 + "</p>";
    setInformativeText.append("<p>" + tr(PROG_DESCRIPTION) + "<p>");
    setInformativeText.append("<p>Copyright (c) 2024 ZHENG Robert</p>");
    setInformativeText.append("<br><a href=\"");
    setInformativeText.append(PROG_HOMEPAGE);
    setInformativeText.append("\" alt=\"Github repository\">");
    setInformativeText.append(qApp->applicationName() + " v" + qApp->applicationVersion() + " "
                              + tr("at") + " Github</a>");

    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("About") + " " + qApp->applicationDisplayName());
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
    ui->menuload_Folder->setIcon(QIcon(":/resources/img/icons8-images-folder-50.png"));

    ui->actionsingle_Folder->setIcon(QIcon(":/resources/img/icons8-opened-folder-50.png"));
    ui->actionsingle_Folder->setIconVisibleInMenu(true);
    ui->actionsingle_Folder->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));
    connect(ui->actionsingle_Folder, &QAction::triggered, this, &MainWindow::openSrcFolder);

    ui->actionrekursive_Folders->setIcon(QIcon(":/resources/img/icons8-file-submodule-50.png"));
    ui->actionrekursive_Folders->setIconVisibleInMenu(true);
    connect(ui->actionrekursive_Folders,
            &QAction::triggered,
            this,
            &MainWindow::openSrcFolderRekursive);

    ui->actionclear_Album->setIcon(QIcon(":/resources/img/icons8-delete-list-50.png"));
    ui->actionclear_Album->setIconVisibleInMenu(true);
    connect(ui->actionclear_Album, &QAction::triggered, this, &MainWindow::clearSrcAlbum);

    ui->actionload_Picture->setIcon(QIcon(":/resources/img/icons8-image-file-add-50"));
    ui->actionload_Picture->setIconVisibleInMenu(true);
    ui->actionload_Picture->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_P));
    connect(ui->actionload_Picture, &QAction::triggered, this, &MainWindow::loadSingleSrcImg);

    // Pictures
    pictureMenu = menuBar()->addMenu(tr("Pictures"));
    //pictureMenu->addAction(ui->actionload_Picture);

    showDefaultExifAct = new QAction(QIcon(":/resources/img/icons8-edit-file-50.png"),
                                     tr("show default Exif meta data"),
                                     this);
    //showDefaultExifAct->setDisabled(true);
    connect(showDefaultExifAct, &QAction::triggered, this, &MainWindow::showDefaultExifMeta);
    pictureMenu->addAction(showDefaultExifAct);

    clearDefaultExifAct = new QAction(QIcon(":/resources/img/icons8-file-elements-50.png"),
                                      tr("clear default Exif meta data"),
                                      this);
    connect(clearDefaultExifAct, &QAction::triggered, this, &MainWindow::clearDefaultExifMeta);
    pictureMenu->addAction(clearDefaultExifAct);

    pictureMenu->addSeparator();

    showDefaultIptcAct = new QAction(QIcon(":/resources/img/icons8-edit-file-50.png"),
                                     tr("show default IPTC meta data"),
                                     this);
    //showDefaultIptcAct->setDisabled(true);
    connect(showDefaultIptcAct, &QAction::triggered, this, &MainWindow::showDefaultIptcMeta);
    pictureMenu->addAction(showDefaultIptcAct);

    clearDefaultIptcAct = new QAction(QIcon(":/resources/img/icons8-file-elements-50.png"),
                                      tr("clear default Exif meta data"),
                                      this);
    connect(clearDefaultIptcAct, &QAction::triggered, this, &MainWindow::clearDefaultIptcMeta);
    pictureMenu->addAction(clearDefaultIptcAct);

    pictureMenu->addSeparator();

    selectAllImagesAct = new QAction(QIcon(":/resources/img/icons8-image-file-add-50.png"),
                                     tr("select all Pictures"),
                                     this);
    //selectAllImagesAct->setDisabled(true);
    connect(selectAllImagesAct, &QAction::triggered, this, &MainWindow::selectAllImages);
    pictureMenu->addAction(selectAllImagesAct);

    writeDefaultMetaToSelectedImagesAct
        = new QAction(QIcon(":/resources/img/icons8-send-file-50.png"),
                      tr("write default Meta to selected Pictures"),
                      this);
    writeDefaultMetaToSelectedImagesAct->setDisabled(true);
    pictureMenu->addAction(writeDefaultMetaToSelectedImagesAct);

    pictureMenu->addSeparator();

    removeImagesAct = new QAction(QIcon(":/resources/img/icons8-image-file-remove-50.png"),
                                  tr("remove selected Pictures from Album"),
                                  this);
    connect(removeImagesAct, &QAction::triggered, this, &MainWindow::removeSelectedImages);
    pictureMenu->addAction(removeImagesAct);

    // About - Info
    aboutAct = new QAction(QIcon(":/resources/img/icons8-info-48.png"), tr("About"), this);
    aboutAct->setIconVisibleInMenu(true);
    aboutAct->setShortcuts(QKeySequence::WhatsThis);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
    infoMenu = menuBar()->addMenu(tr("Info"));
    infoMenu->addAction(aboutAct);
}

void MainWindow::createLanguageMenu()
{
    i18nMenu = menuBar()->addMenu("六A");
    //i18nMenu->setIcon(QIcon(":/resources/img/translate.png"));
    i18nMenu->setDisabled(true);
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

    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listView,
            SIGNAL(customContextMenuRequested(QPoint)),
            this,
            SLOT(showViewContextMenu(QPoint)));
}

void MainWindow::initStatusBar()
{
    statusBarLabel = new QLabel();
    statusBarLabel->setFont(*font_10);
    statusBarLabel->setAlignment(Qt::AlignCenter);
    statusBarLabel->setText("v" + qApp->applicationVersion());
    statusBar()->addWidget(statusBarLabel, 1);
}

void MainWindow::refreshStatusBar()
{
    int rowCount = mContentItemModel->rowCount();
    if (rowCount > 0) {
        statusBarLabel->setText(QString::number(rowCount) + " " + tr("items"));
    } else {
        statusBarLabel->setText("v" + qApp->applicationVersion());
    }
}

void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    // alles OK!!

    /*
    QString itemText = index.data(Qt::DisplayRole).toString();
    qDebug() << "doubleclicked: " << itemText << " col: " << index.column();
    */

    int row = index.row();
    QModelIndex col2 = mContentItemModel->index(row, 1);
    QString itemText2 = col2.data(Qt::DisplayRole).toString();
    //qDebug() << "doubleclicked: " << itemText2 << " col: " << col2.column();
    showSinglePicture(itemText2);

    /*
    QModelIndexList selected = ui->listView->selectionModel()->selectedIndexes();
    for (QModelIndex i : selected) {
        int row = i.row();
        QModelIndex col2 = mContentItemModel->index(row, 1);
        QString itemText2 = col2.data(Qt::DisplayRole).toString();
        //qDebug() << "selected: " << itemText2 << " col: " << col2.column();
    }
    */
}

void MainWindow::showViewContextMenu(const QPoint &pt)
{
    qInfo() << "showViewContextMenu";

    QModelIndex idx = ui->listView->indexAt(pt);

    QString itemText = idx.data(Qt::DisplayRole).toString();
    qDebug() << "clicked: " << itemText << " col: " << idx.column();

    int row = idx.row();
    QModelIndex col2 = mContentItemModel->index(row, 1);
    QString itemText2 = col2.data(Qt::DisplayRole).toString();
    qDebug() << "clicked: " << itemText2;

    //modelIndexForContextMenu = idx; // optional, to let slots know what index is the base for the context menu
    QMenu menu;

    contextShowPictureDetailsAct = new QAction(QIcon(":/resources/img/icons8-view-50.png"),
                                               tr("show Picture details"),
                                               this);
    contextShowPictureDetailsAct->setIconVisibleInMenu(true);
    connect(contextShowPictureDetailsAct, &QAction::triggered, this, [this, itemText2] {
        MainWindow::showSinglePicture(itemText2);
    });

    contextSetExifAsDefaultAct = new QAction(QIcon(
                                                 ":/resources/img/icons8-regular-document-50.png"),
                                             tr("set this red Exif data as default"),
                                             this);
    connect(contextSetExifAsDefaultAct, &QAction::triggered, this, [this, idx] {
        MainWindow::setDefaultExifMeta(idx);
    });

    contextSetIptcAsDefaultAct = new QAction(QIcon(
                                                 ":/resources/img/icons8-regular-document-50.png"),
                                             tr("set this red IPTC data as default"),
                                             this);
    connect(contextSetIptcAsDefaultAct, &QAction::triggered, this, [this, idx] {
        MainWindow::setDefaultIptcMeta(idx);
    });

    contextRemovePictureFromAlbumAct = new QAction(QIcon(
                                                       ":/resources/img/icons8-delete-list-50.png"),
                                                   tr("remove this Picture from Album"),
                                                   this);
    connect(contextRemovePictureFromAlbumAct, &QAction::triggered, this, [this, idx] {
        MainWindow::removeSelectedImageFromAlbum(idx);
    });

    contextShowPictureDetailsAct->setEnabled(idx.column() == 0);
    contextSetExifAsDefaultAct->setEnabled(idx.column() == 0);
    contextSetIptcAsDefaultAct->setEnabled(idx.column() == 0);
    contextRemovePictureFromAlbumAct->setEnabled(idx.column() == 0);

    menu.addAction(contextShowPictureDetailsAct);
    menu.addSeparator();
    menu.addAction(contextSetExifAsDefaultAct);
    menu.addAction(contextSetIptcAsDefaultAct);
    menu.addSeparator();
    menu.addAction(contextRemovePictureFromAlbumAct);

    menu.exec(ui->listView->viewport()->mapToGlobal(pt));
}

void MainWindow::setDefaultExifMeta(const QModelIndex &index)
{
    rowWithDefaultExifMeta = index;
    hasDefaultExifMeta = true;
}

void MainWindow::setDefaultIptcMeta(const QModelIndex &index)
{
    rowWithDefaultIptcMeta = index;
    hasDefaultIptcMeta = true;
}

void MainWindow::resetDefaultMeta()
{
    //rowWithDefaultExifMeta = -0;
    //rowWithDefaultIptcMeta = -0;
    hasDefaultExifMeta = false;
    hasDefaultIptcMeta = false;
}

void MainWindow::showDefaultExifMeta()
{
    if (!hasDefaultExifMeta) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(tr("Exif Meta"));
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setTextFormat(Qt::RichText);
        QString text = tr("No default Exif meta data set.");
        QString setInformativeText = tr("In Album view, select a picture and open the context menu "
                                        "to define the default meta.");
        msgBox.setText(text);
        msgBox.setInformativeText(setInformativeText);
        msgBox.setFixedWidth(900);
        msgBox.exec();
    } else {
        int row = rowWithDefaultExifMeta.row();
        QModelIndex col2 = mContentItemModel->index(row, 1);
        QString itemText2 = col2.data(Qt::DisplayRole).toString();
        showSinglePicture(itemText2);
    }
}

void MainWindow::clearDefaultExifMeta()
{
    hasDefaultExifMeta = false;
}

void MainWindow::showDefaultIptcMeta()
{
    if (!hasDefaultIptcMeta) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(tr("IPTC Meta"));
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setTextFormat(Qt::RichText);
        QString text = tr("No default IPTC meta data set.");
        QString setInformativeText = tr("In Album view, select a picture and open the context menu "
                                        "to define the default meta.");
        msgBox.setText(text);
        msgBox.setInformativeText(setInformativeText);
        msgBox.setFixedWidth(900);
        msgBox.exec();
    } else {
        int row = rowWithDefaultIptcMeta.row();
        QModelIndex col2 = mContentItemModel->index(row, 1);
        QString itemText2 = col2.data(Qt::DisplayRole).toString();
        showSinglePicture(itemText2);
    }
}

void MainWindow::clearDefaultIptcMeta()
{
    hasDefaultIptcMeta = false;
}

void MainWindow::showAlbumLimitMsg(int resultCount)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Critical"));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setTextFormat(Qt::RichText);
    QString text = QString::number(resultCount) + " " + tr("pictures found") + " " + tr("but") + " "
                   + tr("Album limit are") + " " + QString::number(ALBUM_LIMIT) + " "
                   + tr("pictures");
    QString setInformativeText = tr("Please choose folders with lesser picture content.");
    msgBox.setText(text);
    msgBox.setInformativeText(setInformativeText);
    msgBox.setFixedWidth(900);
    msgBox.exec();
}

const void MainWindow::fillSrcListView(QFile &srcFile)
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

const void MainWindow::fillSrcListViewThread(const QList<QString> &srcFiles)
{
    QListIterator<QString> i(srcFiles);
    while (i.hasNext()) {
        QFile srcFile(i.next());
        QFileInfo fileInfo(srcFile.fileName());

        if (fileInfo.size() < 1) {
            continue;
        }

        statusBarLabel->setText(tr("add") + srcFile.fileName());

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
    }
    ui->album_label->setFont(*font_10);
    ui->album_label->setText(tr("Doubleclick on a picture for details. Press CTRL and click to "
                                "select one or more pictures."));

    int rowCount = mContentItemModel->rowCount();
    statusBarLabel->setText(QString::number(rowCount) + " " + tr("items"));
}

void MainWindow::showSinglePicture(QString pathToFile)
{
    PictureWidget *pictureWidget = new PictureWidget();
    pictureWidget->setImage(pathToFile);
    pictureWidget->show();
}

// TODO: clean up
void MainWindow::removeSelectedImages()
{
    // selectedRows() ???
    QModelIndexList selected = ui->listView->selectionModel()->selectedIndexes();
    QList<QString> selectedImages;

    for (QModelIndex i : selected) {
        int row = i.row();
        QModelIndex col2 = mContentItemModel->index(row, 1);
        QString itemText2 = col2.data(Qt::DisplayRole).toString();
        //qDebug() << "to remove: " << itemText2 << " col: " << col2.column();
        selectedImages.append(itemText2);
    }

    //mContentItemModel->removeRow(i.row());
    QListIterator<QString> intItem(selectedImages);
    while (intItem.hasNext()) {
        QString item = intItem.next();
        //qInfo() << "has: " << item;
        QList<QStandardItem *> items = mContentItemModel->findItems(item, Qt::MatchExactly, 1);
        foreach (auto i, items) {
            //  qInfo() << "found: " << i->row();
            mContentItemModel->removeRow(i->row());
        }
    }

    refreshStatusBar();
    resetDefaultMeta();
}

void MainWindow::selectAllImages()
{
    ui->listView->selectAll();
}

// TODO
void MainWindow::_on_listView_clicked(const QModelIndex &index)
{
    qInfo() << "on_listView_clicked: " << Qt::MouseButtons().toInt();

    if (QMouseEvent::ContextMenu && Qt::LeftButton) {
        qInfo() << "on_listView_clicked left contextMenu";
    }
    if (Qt::MouseButtons().toInt() & Qt::RightButton) {
        qInfo() << "on_listView_clicked right context Menu mouse";
    }

    int row = index.row();
    QModelIndex col2 = mContentItemModel->index(row, 1);
    QString itemText2 = col2.data(Qt::DisplayRole).toString();
    qDebug() << "clicked: " << itemText2;

    /* Create menu and insert some actions */
    QMenu rightClickMenu(ui->listView);
    rightClickMenu.addAction("context1");
    rightClickMenu.addAction("context2");
    rightClickMenu.exec(ui->listView->viewport()->mapToGlobal(pos()));
}
