#include <QApplication>
#include <QIcon>
#include <QDir>
#include <QStandardPaths>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    QDir().mkpath(dir);
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icons/icon.png"));
    MainWindow w;
    w.showMaximized();
    return QApplication::exec();
}