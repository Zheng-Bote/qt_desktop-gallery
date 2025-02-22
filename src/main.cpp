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
