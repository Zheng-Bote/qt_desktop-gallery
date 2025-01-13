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
        } catch (...) {
            qDebug() << "clearSrcAlbum: something went wrong";
        }

        //statusBarLabel->setText("0 " + tr("items"));
        statusBarLabel->setText("v" + qApp->applicationVersion());
    }
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

    // Picture
    pictureMenu = menuBar()->addMenu(tr("Picture"));
    pictureMenu->addAction(ui->actionload_Picture);

    removeImageAct = new QAction(QIcon(":/resources/img/icons8-delete-list-50.png"),
                                 tr("remove selected"),
                                 this);
    connect(removeImageAct, &QAction::triggered, this, &MainWindow::removeSelectedImages);
    pictureMenu->addAction(removeImageAct);

    // About - Info
    aboutAct = new QAction(QIcon(":/resources/img/icons8-info-48.png"), tr("About"), this);
    aboutAct->setIconVisibleInMenu(true);
    aboutAct->setShortcuts(QKeySequence::WhatsThis);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
    infoMenu = menuBar()->addMenu(tr("Info"));
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

        qDebug() << "MainWindow::fillSrcListViewTest: " << srcFile.fileName();
        statusBarLabel->setText(tr("add") + srcFile.fileName());

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
    }
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

// TODO: clean up
void MainWindow::removeSelectedImages()
{
    QModelIndexList selected = ui->listView->selectionModel()->selectedIndexes();
    QList<QString> selectedImages;

    for (QModelIndex i : selected) {
        int row = i.row();
        QModelIndex col2 = mContentItemModel->index(row, 1);
        QString itemText2 = col2.data(Qt::DisplayRole).toString();
        qDebug() << "to remove: " << itemText2 << " col: " << col2.column();
        selectedImages.append(itemText2);
    }

    //mContentItemModel->removeRow(i.row());
    QListIterator<QString> intItem(selectedImages);
    while (intItem.hasNext()) {
        QString item = intItem.next();
        qInfo() << "has: " << item;
        QList<QStandardItem *> items = mContentItemModel->findItems(item, Qt::MatchExactly, 1);
        foreach (auto i, items) {
            qInfo() << "found: " << i->row();
            mContentItemModel->removeRow(i->row());
        }
    }

    int rowCount = mContentItemModel->rowCount();
    statusBarLabel->setText(QString::number(rowCount) + " " + tr("items"));
}

// TODO
void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    qInfo() << "on_listView_clicked: " << Qt::MouseButtons().toInt();

    if (QMouseEvent::ContextMenu & Qt::LeftButton) {
        qInfo() << "on_listView_clicked contextMenu";
    }
    if (Qt::MouseButtons().toInt() & Qt::RightButton) {
        qInfo() << "on_listView_clicked context Menu mouse";
    }

    int row = index.row();
    QModelIndex col2 = mContentItemModel->index(row, 1);
    QString itemText2 = col2.data(Qt::DisplayRole).toString();
    qDebug() << "clicked: " << itemText2 << " col: " << col2.column();
    if (Qt::LeftButton) {
        qInfo() << "left Button click: " << Qt::LeftButton;
    }
    if (Qt::RightButton) {
        qInfo() << "right Button click " << Qt::RightButton;
    }
}
