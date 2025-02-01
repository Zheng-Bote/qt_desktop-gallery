#pragma once

#include <QFile>
#include <QFutureWatcher>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QMouseEvent>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QTranslator>

//#include "includes/rz_metadata.hpp"
#include "includes/rz_photo.hpp"

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

    void loadLanguage(const QString &rLanguage);

public slots:
    void slotProgressBarFinished();

protected slots:
    void slotLanguageChanged(QAction *action);

private slots:
    void openReadFolder();
    void openSrcFolder();
    void openSrcFolderRekursive();
    void loadSingleSrcImg();
    void clearSrcAlbum();
    void removeSelectedImageFromAlbum(const QModelIndex &index);

    void about();
    void hwInfoMsgbox();

    void selectAllImages();

    void on_listView_doubleClicked(const QModelIndex &index);

    void _on_listView_clicked(const QModelIndex &index);
    void showViewContextMenu(const QPoint &pt);

    void setDefaultExifMeta(const QModelIndex &index);
    void setDefaultIptcMeta(const QModelIndex &index);
    void setDefaultXmpCopyRightOwner(const QModelIndex &index);
    void writeDefaultOwnerToSelectedImages();
    void setDefaultGpsData(const QModelIndex &index);
    void writeDefaultExifGpsToSelected();

    void resetDefaultMeta();

    void showDefaultExifMeta();
    void clearDefaultExifMeta();
    void showDefaultIptcMeta();
    void clearDefaultIptcMeta();

    void on_progressBar_valueChanged(int value);

protected:
    void changeEvent(QEvent *);
    bool eventFilter(QObject *sender, QEvent *event);

private:
    Ui::MainWindow *ui;

    QFutureWatcher<void> FutureWatcher;

    QFont *font_10 = new QFont("Times New Roman", 10);
    QFont *font_12 = new QFont("Times New Roman", 12);
    QLabel *statusBarLabel;

    const int ALBUM_LIMIT = 4096;
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
    QAction *writeDefaultGpsMetaToSelectedImagesAct;
    QAction *removeImagesAct;
    void removeSelectedImages();

    QMenu *infoMenu;
    QAction *aboutAct;
    QAction *hw_infoAct;
    void createMenu();

    QMenu *i18nMenu;
    void createLanguageMenu(void);
    QTranslator m_translator;
    QTranslator m_translatorQt;
    QString m_currLang;
    QString m_langPath;
    //void loadLanguage(const QString &rLanguage);
    void switchTranslator(QTranslator &translator, const QString &filename);

    void initListview();
    QAction *contextShowPictureDetailsAct;
    QAction *contextSetExifAsDefaultAct;
    QAction *contextSetIptcAsDefaultAct;
    QAction *contextSetXmpCopyRightOwnerAsDefaultAct;
    QAction *contextSetGpsMetaAsDefaultAct;
    QAction *contextRemovePictureFromAlbumAct;

    void initStatusBar();

    // Meta
    bool hasDefaultExifMeta{false};
    bool hasDefaultIptcMeta{false};
    QModelIndex rowWithDefaultExifMeta;
    QModelIndex rowWithDefaultIptcMeta;

    bool hasDefaultCopyRightOwner{false};
    QModelIndex rowWithDefaultCopyRightQwnerMeta;

    bool hasDefaultExifGpsData{false};
    QModelIndex rowWithDefaultGpsMeta;

    void refreshStatusBar();

    QAction *readRekFolderTest;

    void setProgressbar(int val);
    int totalCount{0};
};
