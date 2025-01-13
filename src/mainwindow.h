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
    void about();

    void on_listView_doubleClicked(const QModelIndex &index);

    void on_listView_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;

    QFont *font_10 = new QFont("Times New Roman", 10);
    QFont *font_12 = new QFont("Times New Roman", 12);
    QLabel *statusBarLabel;

    QStringListModel *model;
    QStandardItemModel *mContentItemModel;
    const unsigned int THUMBNAIL_SIZE = 300;

    const void fillSrcListView(QFile &srcFile);
    const void fillSrcListViewThread(const QList<QString> &srcFiles);

    QAction *loadSingleSrcImgAct;
    void showSinglePicure(QString pathToFile);

    QMenu *pictureMenu;
    QAction *removeImageAct;
    void removeSelectedImages();

    QMenu *infoMenu;
    QAction *aboutAct;
    void createMenu();

    void initListview();
    void initStatusBar();

    QAction *readRekFolderTest;
};
