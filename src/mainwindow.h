#pragma once

#include <QFile>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QStandardItemModel>
#include <QStringListModel>

#include <QMouseEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openReadFolder();
    void openSrcFolder();
    void openSrcFolderRekursive();
    void loadSingleSrcImg();
    void clearSrcAlbum();
    void removeSelectedImageFromAlbum(const QModelIndex &index);
    void about();

    void selectAllImages();

    void on_listView_doubleClicked(const QModelIndex &index);

    void _on_listView_clicked(const QModelIndex &index);
    void showViewContextMenu(const QPoint &pt);

    void setDefaultExifMeta(const QModelIndex &index);
    void setDefaultIptcMeta(const QModelIndex &index);
    void resetDefaultMeta();

    void showDefaultExifMeta();
    void clearDefaultExifMeta();
    void showDefaultIptcMeta();
    void clearDefaultIptcMeta();

private:
    Ui::MainWindow *ui;

    QFont *font_10 = new QFont("Times New Roman", 10);
    QFont *font_12 = new QFont("Times New Roman", 12);
    QLabel *statusBarLabel;

    const int ALBUM_LIMIT = 4000;
    void showAlbumLimitMsg(int resultCount);

    QStringListModel *model;
    QStandardItemModel *mContentItemModel;
    const unsigned int THUMBNAIL_SIZE = 300;

    const void fillSrcListView(QFile &srcFile);
    const void fillSrcListViewThread(const QList<QString> &srcFiles);

    QAction *loadSingleSrcImgAct;
    void showSinglePicture(QString pathToFile);

    QMenu *pictureMenu;
    QAction *selectAllImagesAct;
    QAction *showDefaultExifAct;
    QAction *showDefaultIptcAct;
    QAction *clearDefaultExifAct;
    QAction *clearDefaultIptcAct;
    QAction *writeDefaultMetaToSelectedImagesAct;
    QAction *removeImagesAct;
    void removeSelectedImages();

    QMenu *infoMenu;
    QAction *aboutAct;
    void createMenu();

    QMenu *i18nMenu;
    void createLanguageMenu();

    void initListview();
    QAction *contextShowPictureDetailsAct;
    QAction *contextSetExifAsDefaultAct;
    QAction *contextSetIptcAsDefaultAct;
    QAction *contextRemovePictureFromAlbumAct;

    void initStatusBar();

    bool hasDefaultExifMeta{false};
    bool hasDefaultIptcMeta{false};
    QModelIndex rowWithDefaultExifMeta;
    QModelIndex rowWithDefaultIptcMeta;

    void refreshStatusBar();

    QAction *readRekFolderTest;
};
