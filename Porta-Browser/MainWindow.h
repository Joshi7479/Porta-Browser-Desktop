#ifndef PORTA_BROWSER_MAINWINDOW_H
#define PORTA_BROWSER_MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QMainWindow>
#include <QEvent>

#include "DatabaseManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void refreshHistory();
    void refreshBookmarks();
    std::function<void(const QUrl &)> m_openUrl;
    QString text = "";
    QString text2 = "";
    QString currenturl = "https://duckduckgo.com";
    QString currentsearchengine = QStringLiteral("https://www.duckduckgo.com");
    QString currenthotkeyurl = currentsearchengine;
    bool disableclose = false;
    bool disableclosestate = false;
    bool invertlayout = false;
    bool invertlayoutstate = false;
    bool m_wasMaximized = false;
    bool savehistory = true;
    bool currentsavehistorystate = true;

    QVBoxLayout *m_historyLayout = nullptr;
    QVBoxLayout *m_bookmarkLayout = nullptr;
    QWebEngineProfile *m_profile = nullptr;
    DatabaseManager m_db;
};

#endif // PORTA_BROWSER_MAINWINDOW_H