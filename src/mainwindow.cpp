#include <QActionGroup>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>

#include "./picture_widget.h"
#include "./ui_mainwindow.h"
#include "includes/rz_config.h"
#include "includes/rz_hwinfo.h"

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

void MainWindow::slotLanguageChanged(QAction *action)
{
    if (0 != action) {
        // load the language dependant on the action content
        loadLanguage(action->data().toString());
        //setWindowIcon(action->icon());
    }
}

void switchTranslator(QTranslator &translator, const QString &filename)
{
    // remove the old translator
    qApp->removeTranslator(&translator);

    // load the new translator
    QString path = QApplication::applicationDirPath();
    path.append("/languages/");
    if (translator.load(
            path
            + filename)) //Here Path and Filename has to be entered because the system didn't find the QM Files else
        qApp->installTranslator(&translator);
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
        ui->album_label->setText(tr("processing") + " " + QString::number(ALBUM_LIMIT) + " of "
                                 + QString::number(result.size()) + " " + tr("items") + "...");
        //return;
    } else {
        ui->album_label->setText(tr("processing") + " " + QString::number(result.size()) + " "
                                 + tr("items") + "...");
    }
    statusBarLabel->setText(tr("processing") + " " + QString::number(result.size()) + " "
                            + tr("items") + "...");

    //QThreadPool::globalInstance()->setMaxThreadCount(1);
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
        ui->album_label->setText(tr("processing") + " " + QString::number(ALBUM_LIMIT) + " of "
                                 + QString::number(result.size()) + " " + tr("items") + "...");
        //return;
    } else {
        ui->album_label->setText(tr("processing") + " " + QString::number(result.size()) + " "
                                 + tr("items") + "...");
    }
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
        // kiss
        //QFile srcFile(pathToFile);
        //fillSrcListView(srcFile);
        QList<QString> file;
        file.append(pathToFile);
        fillSrcListViewThread(file);
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

    setInformativeText.append(
        "<p>" + tr("Desktop gallery-app to show/edit Exif/IPTC/XMP and export to WebP") + "<p>");
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
    //pictureMenu = menuBar()->addMenu(tr("Pictures"));

    ui->showDefaultExifAct->setIcon(QIcon(":/resources/img/icons8-edit-file-50.png"));
    ui->showDefaultExifAct->setIconVisibleInMenu(true);
    connect(ui->showDefaultExifAct, &QAction::triggered, this, &MainWindow::showDefaultExifMeta);
    //ui->pictureMenu->addAction(showDefaultExifAct);

    ui->clearDefaultExifAct->setIcon(QIcon(":/resources/img/icons8-file-elements-50.png"));
    ui->clearDefaultExifAct->setIconVisibleInMenu(true);
    connect(ui->clearDefaultExifAct, &QAction::triggered, this, &MainWindow::clearDefaultExifMeta);
    //pictureMenu->addAction(clearDefaultExifAct);

    //pictureMenu->addSeparator();

    ui->showDefaultIptcAct->setIcon(QIcon(":/resources/img/icons8-edit-file-50.png"));
    //showDefaultIptcAct->setDisabled(true);
    ui->showDefaultIptcAct->setIconVisibleInMenu(true);
    connect(ui->showDefaultIptcAct, &QAction::triggered, this, &MainWindow::showDefaultIptcMeta);
    //pictureMenu->addAction(showDefaultIptcAct);

    ui->clearDefaultIptcAct->setIcon(QIcon(":/resources/img/icons8-file-elements-50.png"));
    ui->clearDefaultIptcAct->setIconVisibleInMenu(true);
    connect(ui->clearDefaultIptcAct, &QAction::triggered, this, &MainWindow::clearDefaultIptcMeta);
    //pictureMenu->addAction(clearDefaultIptcAct);

    ui->writeDefaultGpsMetaToSelectedImagesAct->setIcon(
        QIcon(":/resources/img/icons8-send-file-50.png"));
    ui->writeDefaultGpsMetaToSelectedImagesAct->setIconVisibleInMenu(true);
    connect(ui->writeDefaultGpsMetaToSelectedImagesAct,
            &QAction::triggered,
            this,
            &MainWindow::writeDefaultExifGpsToSelected);

    //pictureMenu->addSeparator();

    ui->selectAllImagesAct->setIcon(QIcon(":/resources/img/icons8-image-file-add-50.png"));
    ui->selectAllImagesAct->setIconVisibleInMenu(true);
    //selectAllImagesAct->setDisabled(true);
    connect(ui->selectAllImagesAct, &QAction::triggered, this, &MainWindow::selectAllImages);
    //pictureMenu->addAction(selectAllImagesAct);

    ui->writeDefaultOwnerToSelectedImagesAct->setIcon(
        QIcon(":/resources/img/icons8-send-file-50.png"));
    ui->writeDefaultOwnerToSelectedImagesAct->setIconVisibleInMenu(true);
    ui->writeDefaultOwnerToSelectedImagesAct->setDisabled(false);
    connect(ui->writeDefaultOwnerToSelectedImagesAct,
            &QAction::triggered,
            this,
            &MainWindow::writeDefaultOwnerToSelectedImages);
    //pictureMenu->addAction(writeDefaultMetaToSelectedImagesAct);

    //pictureMenu->addSeparator();

    ui->removeImagesAct->setIcon(QIcon(":/resources/img/icons8-image-file-remove-50.png"));
    ui->removeImagesAct->setIconVisibleInMenu(true);
    connect(ui->removeImagesAct, &QAction::triggered, this, &MainWindow::removeSelectedImages);
    //pictureMenu->addAction(removeImagesAct);

    // About - Info
    //infoMenu = menuBar()->addMenu(tr("Info"));

    ui->aboutAct->setIcon(QIcon(":/resources/img/icons8-info-48.png"));
    ui->aboutAct->setIconVisibleInMenu(true);
    ui->aboutAct->setShortcuts(QKeySequence::WhatsThis);
    connect(ui->aboutAct, &QAction::triggered, this, &MainWindow::about);
    //infoMenu->addAction(aboutAct);

    ui->hw_infoAct->setIcon(QIcon(":/resources/img/icons8-info-48.png"));
    ui->hw_infoAct->setIconVisibleInMenu(true);
    connect(ui->hw_infoAct, &QAction::triggered, this, &MainWindow::hwInfoMsgbox);
    //infoMenu->addAction(hw_infoAct);
}

void MainWindow::hwInfoMsgbox()
{
    HwInfo hwInfo;
    hwInfo.setHwInfo();

    QMap<QString, QString> hw_info = hwInfo.getHwInfo();

    QString text = qApp->applicationDisplayName() + " " + tr("is running on the following hardware")
                   + ":\n\n";
    QString setInformativeText = "<p><b>CPU:</b><br>";
    setInformativeText.append("Vendor: " + hw_info.value("CPU vendor"));
    setInformativeText.append("<br>Model: " + hw_info.value("CPU model"));
    setInformativeText.append("<br>physical Cores: " + hw_info.value("CPU physical cores"));
    setInformativeText.append("<br>logical Cores: " + hw_info.value("CPU logical cores"));
    setInformativeText.append("<br>max Frequency: " + hw_info.value("CPU max frequency"));

    setInformativeText.append("<br><br><b>Memory:</b>");
    setInformativeText.append("<br>Size [MiB]: " + hw_info.value("RAM size [MiB]"));
    setInformativeText.append("<br>Free [MiB]: " + hw_info.value("RAM free [MiB]"));
    setInformativeText.append("<br>Available [MiB]: " + hw_info.value("RAM available [MiB]"));

    setInformativeText.append("<br><br><b>Operating System:</b>");
    setInformativeText.append("<br>OS: " + hw_info.value("Operating System"));
    setInformativeText.append("<br>Version: " + hw_info.value("OS version"));
    setInformativeText.append("<br>Kernel: " + hw_info.value("OS kernel"));
    setInformativeText.append("<br>Architecture: " + hw_info.value("OS architecture"));

    /*
    QMapIterator<QString, QString> i(hw_info);
    while (i.hasNext()) {
        i.next();
        setInformativeText.append("<li>" + i.key() + ": " + i.value() + "</li>");
    }
    */
    setInformativeText.append("</p>");
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Hardware Info"));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(text);
    msgBox.setInformativeText(setInformativeText);
    msgBox.setFixedWidth(900);
    msgBox.exec();
}

void MainWindow::createLanguageMenu(void)
{
    i18nMenu = menuBar()->addMenu("六A");
    //i18nMenu->setIcon(QIcon(":/resources/img/translate.png"));
    //i18nMenu->setDisabled(true);

    QActionGroup *langGroup = new QActionGroup(i18nMenu);
    langGroup->setExclusive(true);

    connect(langGroup, SIGNAL(triggered(QAction *)), this, SLOT(slotLanguageChanged(QAction *)));

    QString defaultLocale = QLocale::system().name();       // e.g. "de_DE"
    defaultLocale.truncate(defaultLocale.lastIndexOf('_')); // e.g. "de"

    m_langPath = QApplication::applicationDirPath();
    m_langPath.append("/i18n");
    QDir dir(m_langPath);
    QStringList fileNames = dir.entryList(QStringList(qApp->applicationName() + "_*.qm"));

    for (int i = 0; i < fileNames.size(); ++i) {
        // get locale extracted by filename
        QString locale;
        locale = fileNames[i];                         // "TranslationExample_de.qm"
        locale.truncate(locale.lastIndexOf('.'));      // "TranslationExample_de"
        locale.remove(0, locale.lastIndexOf('_') + 1); // "de"

        //qDebug() << "locale: " << locale;

        //QString lang = QLocale::languageToString(QLocale(locale).language());
        QIcon ico(QString("%1/%2.png").arg(m_langPath).arg(locale));

        QAction *action = new QAction(ico, locale.toUpper(), this);
        action->setCheckable(true);
        action->setData(locale);

        i18nMenu->addAction(action);
        langGroup->addAction(action);

        // set default translators and language checked
        if (defaultLocale == locale) {
            action->setChecked(true);
        }
    }
}

void MainWindow::loadLanguage(const QString &rLanguage)
{
    if (m_currLang != rLanguage) {
        m_currLang = rLanguage;
        QLocale locale = QLocale(m_currLang);
        QLocale::setDefault(locale);
        QString languageName = QLocale::languageToString(locale.language());
        switchTranslator(m_translator, QString(qApp->applicationName() + "_%1.qm").arg(rLanguage));
        switchTranslator(m_translatorQt, QString("qt_%1.qm").arg(rLanguage));
        statusBar()->showMessage(tr("Current Language changed to") + " " + rLanguage.toUpper());
    }
}

void MainWindow::switchTranslator(QTranslator &translator, const QString &filename)
{
    qApp->removeTranslator(&translator);

    // load the new translator
    QString path = QApplication::applicationDirPath();
    path.append("/i18n/");
    if (translator.load(
            path
            + filename)) //Here Path and Filename has to be entered because the system didn't find the QM Files else
        qApp->installTranslator(&translator);
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

void MainWindow::writeDefaultExifGpsToSelected()
{
    if (!hasDefaultExifGpsData) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(tr("Default GPS Meta"));
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setTextFormat(Qt::RichText);
        QString text = tr("No default GPS meta data set.");
        QString setInformativeText = tr("In Album view, select a picture and open the context menu "
                                        "to define the default meta.");
        msgBox.setText(text);
        msgBox.setInformativeText(setInformativeText);
        msgBox.setFixedWidth(900);
        msgBox.exec();
        return;
    }

    QModelIndexList selected = ui->listView->selectionModel()->selectedIndexes();
    QList<QString> selectedImages;

    QModelIndex gpsIndex = rowWithDefaultGpsMeta;
    int row = gpsIndex.row();
    QModelIndex col2 = mContentItemModel->index(row, 1);
    QString pathToSrcFile = col2.data(Qt::DisplayRole).toString();
    Photo photo(pathToSrcFile);
    Photo::exifGpsStruct _gpsData;
    _gpsData = photo.getGpsData();

    for (QModelIndex i : selected) {
        int row = i.row();
        QModelIndex col2 = mContentItemModel->index(row, 1);
        QString pathToFile = col2.data(Qt::DisplayRole).toString();
        Photo photo(pathToFile);
        QFuture<void> future = QtConcurrent::run(&Photo::writeDefaultGpsData, photo, _gpsData);
    }
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
    //qInfo() << "showViewContextMenu";

    QModelIndex idx = ui->listView->indexAt(pt);

    QString itemText = idx.data(Qt::DisplayRole).toString();
    //qDebug() << "clicked: " << itemText << " col: " << idx.column();

    int row = idx.row();
    QModelIndex col2 = mContentItemModel->index(row, 1);
    QString itemText2 = col2.data(Qt::DisplayRole).toString();
    //qDebug() << "clicked: " << itemText2;

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

    contextSetXmpCopyRightOwnerAsDefaultAct
        = new QAction(QIcon(":/resources/img/icons8-regular-document-50.png"),
                      tr("set this Copyright data as default"),
                      this);
    connect(contextSetXmpCopyRightOwnerAsDefaultAct, &QAction::triggered, this, [this, idx] {
        MainWindow::setDefaultXmpCopyRightOwner(idx);
    });

    contextSetGpsMetaAsDefaultAct
        = new QAction(QIcon(":/resources/img/icons8-regular-document-50.png"),
                      tr("set this GPS data as default"),
                      this);
    connect(contextSetGpsMetaAsDefaultAct, &QAction::triggered, this, [this, idx] {
        MainWindow::setDefaultGpsData(idx);
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
    contextSetXmpCopyRightOwnerAsDefaultAct->setEnabled(idx.column() == 0);
    contextSetGpsMetaAsDefaultAct->setEnabled(idx.column() == 0);
    contextRemovePictureFromAlbumAct->setEnabled(idx.column() == 0);

    menu.addAction(contextShowPictureDetailsAct);
    menu.addSeparator();
    menu.addAction(contextSetExifAsDefaultAct);
    menu.addAction(contextSetIptcAsDefaultAct);
    menu.addSeparator();
    menu.addAction(contextSetXmpCopyRightOwnerAsDefaultAct);
    menu.addAction(contextSetGpsMetaAsDefaultAct);
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

void MainWindow::setDefaultXmpCopyRightOwner(const QModelIndex &index)
{
    hasDefaultCopyRightOwner = true;
    rowWithDefaultCopyRightQwnerMeta = index;
}

void MainWindow::resetDefaultMeta()
{
    //rowWithDefaultExifMeta = -0;
    //rowWithDefaultIptcMeta = -0;
    hasDefaultExifMeta = false;
    hasDefaultIptcMeta = false;
    hasDefaultCopyRightOwner = false;
    hasDefaultExifGpsData = false;
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

void MainWindow::changeEvent(QEvent *event)
{
    if (0 != event) {
        switch (event->type()) {
        // this event is send if a translator is loaded
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;

            // this event is send, if the system, language changes
        case QEvent::LocaleChange: {
            QString locale = QLocale::system().name();
            locale.truncate(locale.lastIndexOf('_'));
            loadLanguage(locale);
        } break;
        }
    }
    QMainWindow::changeEvent(event);
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
    QString setInformativeText = tr("Please choose folders with a lesser amount of pictures.");

    QString detailedText = tr("Some filesystems like Microsoft FAT have issues with foldercontent "
                              "bigger than 4096 entries.");
    detailedText.append("\n"
                        + tr("Also, if you are planning to do a WebP export, the filecontent will "
                             "increase (extremly)."));
    msgBox.setText(text);
    msgBox.setInformativeText(setInformativeText);
    msgBox.setDetailedText(detailedText);
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
    int count{-1};
    //ui->listView->setUniformItemSizes(true);

    QListIterator<QString> i(srcFiles);
    while (i.hasNext()) {
        QFile srcFile(i.next());
        QFileInfo fileInfo(srcFile.fileName());

        count++;
        if (count == ALBUM_LIMIT) {
            qDebug() << "reached album limit of " + QString::number(ALBUM_LIMIT)
                            + "; stopped loading";
            break;
        }

        if (fileInfo.size() < 1) {
            continue;
        }

        statusBarLabel->setText(tr("add") + srcFile.fileName());

        QString pathToFile = fileInfo.absolutePath() + "/" + fileInfo.fileName();
        QPixmap pixmap(pathToFile);
        auto thumbnail = new QPixmap(pixmap.scaled(THUMBNAIL_SIZE,
                                                   THUMBNAIL_SIZE,
                                                   Qt::KeepAspectRatio,
                                                   Qt::SmoothTransformation));

        // Test =>
        const unsigned int BANNER_HEIGHT = 20;
        const unsigned int BANNER_COLOR = 0x303030;
        const unsigned int BANNER_ALPHA = 200;
        const unsigned int BANNER_TEXT_COLOR = 0xffffff;
        const unsigned int HIGHLIGHT_ALPHA = 100;

        QImage img(thumbnail->toImage());
        QPainter paint(&img);

        QRect bannerRect = QRect(0, 0, img.width(), BANNER_HEIGHT);
        QColor bannerColor = QColor(BANNER_COLOR);
        bannerColor.setAlpha(BANNER_ALPHA);
        paint.fillRect(bannerRect, bannerColor);
        paint.setPen(BANNER_TEXT_COLOR);
        paint.setFont(*font_10);
        paint.drawText(bannerRect, Qt::AlignCenter, fileInfo.fileName());
        QPixmap *px = new QPixmap();
        px->convertFromImage(img);
        // Test <=

        QStandardItem *listitem = new QStandardItem();

        // Test =>
        //listitem->setIcon(*thumbnail);
        listitem->setIcon(*px);
        // Test <=

        //listitem->setText(fileInfo.fileName()); // + "\nschaun wa ma");
        listitem->setToolTip(pathToFile);
        //listitem->setBackground(Qt::lightGray);
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

    //ui->listView->setUniformItemSizes(false);
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

// TODO
void MainWindow::writeDefaultOwnerToSelectedImages()
{
    //qDebug() << "writeDefaultMetaToSelectedImages";

    if (!hasDefaultCopyRightOwner) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(tr("Default Meta"));
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setTextFormat(Qt::RichText);
        QString text = tr("No default meta data set.");
        QString setInformativeText = tr("In Album view, select a picture and open the context menu "
                                        "to define the default meta.");
        msgBox.setText(text);
        msgBox.setInformativeText(setInformativeText);
        msgBox.setFixedWidth(900);
        msgBox.exec();
    }

    QModelIndexList selected = ui->listView->selectionModel()->selectedIndexes();
    QList<QString> selectedImages;

    QModelIndex ownerIndex = rowWithDefaultCopyRightQwnerMeta;
    int row = ownerIndex.row();
    QModelIndex col2 = mContentItemModel->index(row, 1);
    QString pathToSRcFile = col2.data(Qt::DisplayRole).toString();
    Photo photo(pathToSRcFile);
    QString owner = photo.getXmpCopyrightOwner();

    /*
    int row = index.row();
    QModelIndex col2 = mContentItemModel->index(row, 1);
    QString itemText2 = col2.data(Qt::DisplayRole).toString();
    //qDebug() << "doubleclicked: " << itemText2 << " col: " << col2.column();
    showSinglePicture(itemText2); 
    */

    for (QModelIndex i : selected) {
        int row = i.row();
        QModelIndex col2 = mContentItemModel->index(row, 1);
        QString pathToFile = col2.data(Qt::DisplayRole).toString();

        Photo photo(pathToFile);
        QFutureWatcher<bool> watcher;
        QFuture<bool> future = QtConcurrent::run(&Photo::writeToAllCopyrightOwner, photo, owner);
        watcher.setFuture(future);
        watcher.waitForFinished();
        bool result = future.result();
    }
}

void MainWindow::setDefaultGpsData(const QModelIndex &index)
{
    hasDefaultExifGpsData = true;
    rowWithDefaultGpsMeta = index;
}

void MainWindow::selectAllImages()
{
    ui->listView->selectAll();
}

// TODO
void MainWindow::_on_listView_clicked(const QModelIndex &index)
{
    //qInfo() << "on_listView_clicked: " << Qt::MouseButtons().toInt();

    if (QMouseEvent::ContextMenu && Qt::LeftButton) {
        //qInfo() << "on_listView_clicked left contextMenu";
    }
    if (Qt::MouseButtons().toInt() & Qt::RightButton) {
        //qInfo() << "on_listView_clicked right context Menu mouse";
    }

    int row = index.row();
    QModelIndex col2 = mContentItemModel->index(row, 1);
    QString itemText2 = col2.data(Qt::DisplayRole).toString();
    //qDebug() << "clicked: " << itemText2;

    /* Create menu and insert some actions */
    QMenu rightClickMenu(ui->listView);
    rightClickMenu.addAction("context1");
    rightClickMenu.addAction("context2");
    rightClickMenu.exec(ui->listView->viewport()->mapToGlobal(pos()));
}
