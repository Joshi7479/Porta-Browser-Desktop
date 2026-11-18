#ifndef PORTA_BROWSER_MAINWINDOW_H
#define PORTA_BROWSER_MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QWebEngineProfile>

#include "DatabaseManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void refreshHistory();

    std::function<void(const QUrl &)> m_openUrl;
    QString text = "";
    QString currenturl = "https://duckduckgo.com";
    QString currentsearchengine = QStringLiteral("https://www.duckduckgo.com");
    bool disableclose = false;

    QVBoxLayout *m_historyLayout = nullptr;
    QWebEngineProfile *m_profile = nullptr;
    DatabaseManager m_db;
};

#endif // PORTA_BROWSER_MAINWINDOW_H