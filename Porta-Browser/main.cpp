#include <QApplication>
#include <QIcon>
#include <QDir>
#include <QStandardPaths>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    QDir().mkpath(dir);
    qputenv("QTWEBENGINE_DISABLE_SANDBOX", "1");
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icons/porta-browser.png"));
    MainWindow w;
    w.showMaximized();
    return QApplication::exec();
}