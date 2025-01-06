#pragma once

#include <QFile>
#include <QLabel>
#include <QMainWindow>
#include <QStandardItemModel>
#include <QStringListModel>

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

private:
    Ui::MainWindow *ui;

    QFont *font_10 = new QFont("Times New Roman", 10);
    QFont *font_12 = new QFont("Times New Roman", 12);
    QLabel *statusBarLabel;

    QStringListModel *model;
    QStandardItemModel *mContentItemModel;
    const unsigned int THUMBNAIL_SIZE = 300;

    void fillSrcListView(QFile &srcFile);

    QMenu *pictureMenu;
    QAction *loadSingleSrcImgAct;
    void showSinglePicure(QString pathToFile);

    QMenu *infoMenu;
    QAction *aboutAct;
    void createMenu();

    void initListview();
    void initStatusBar();
};
