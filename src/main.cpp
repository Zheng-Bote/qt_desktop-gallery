#include "mainwindow.h"
#include <QApplication>

#include "includes/rz_config.h"

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
    w.show();
    return a.exec();
}
