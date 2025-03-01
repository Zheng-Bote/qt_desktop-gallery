/**
 * @file main.cpp
 * @author ZHENG Robert (robert.hase-zheng.net)
 * @brief Desktop gallery-app for Linux, MacOS, Windows to show/edit Exif/IPTC/XMP and export to WebP
 * @version 0.23.0
 * @date 2025-01-01
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 */
#include <QApplication>

#include "includes/rz_config.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName(PROG_EXEC_NAME);
    qApp->setApplicationDisplayName(PROG_NAME);
    QCoreApplication::setApplicationVersion(PROG_VERSION);
    QCoreApplication::setOrganizationName(PROG_ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(PROG_ORGANIZATION_DOMAIN);

    QApplication a(argc, argv);
    QApplication::setWindowIcon(QIcon(":/resources/img/qt_desktop-gallery_32x31.png"));
    MainWindow w;

    QString locale = QLocale::system().name();
    locale.truncate(locale.lastIndexOf('_'));
    w.loadLanguage(locale);

    w.show();
    return a.exec();
}
