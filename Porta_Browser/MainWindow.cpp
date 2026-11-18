#include "MainWindow.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QWebEngineView>
#include <QTabWidget>
#include <QWidget>
#include <QUrl>
#include <QToolButton>
#include "DatabaseManager.h"
#include <QDir>
#include <QStandardPaths>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QDebug>
#include <QFileInfo>
#include <QWebEngineDownloadRequest>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    m_profile = new QWebEngineProfile("MeinBrowser", this);
    m_profile->setPersistentStoragePath(dir + "/webdata");
    m_profile->setCachePath(dir + "/cache");
    m_profile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);

    if (!m_db.open(dir + "/browser.db") || !m_db.createTables()) {
        qWarning() << "Internal service: m_db.open or createTables failed!";
    } else {
        currentsearchengine = m_db.loadsearchengine();
    }

    if (currentsearchengine.isEmpty()) {
        currentsearchengine = "https://duckduckgo.com";
    }

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    auto *searchengine = new QLineEdit(central);
    auto *closesettings = new QToolButton(central);
    auto *linedit = new QLineEdit(central);
    auto *tabwidget = new QTabWidget(central);
    auto *newtab = new QToolButton(tabwidget);
    auto *closeall = new QToolButton(tabwidget);
    auto *menu = new QToolButton(tabwidget);
    auto *rightcorner = new QWidget(tabwidget);
    auto *rightlayout = new QHBoxLayout(rightcorner);

    rightlayout->setContentsMargins(0, 0, 0, 0);
    rightlayout->setSpacing(2);
    rightlayout->addWidget(newtab);
    rightlayout->addWidget(menu);

    menu->setText("≡");
    closesettings->setText("X");
    closeall->setText("X");
    newtab->setText("+");
    newtab->setAutoRaise(true);

    tabwidget->setTabsClosable(true);
    tabwidget->setMovable(true);
    tabwidget->setCornerWidget(rightcorner, Qt::TopRightCorner);
    tabwidget->setCornerWidget(closeall, Qt::TopLeftCorner);

    searchengine->setPlaceholderText("Search-Engine");
    searchengine->setText(currentsearchengine);
    linedit->setPlaceholderText("URL or search");

    layout->addWidget(searchengine);
    layout->addWidget(closesettings);
    layout->addWidget(linedit);
    layout->addWidget(tabwidget);
    setCentralWidget(central);

    searchengine->hide();
    closesettings->hide();

    connect(linedit, &QLineEdit::returnPressed, this, [this, tabwidget, linedit]() {
        auto *current = qobject_cast<QWebEngineView *>(tabwidget->currentWidget());
        if (!current) return;

        const QString input = linedit->text().trimmed();
        if (input.isEmpty()) return;

        const bool looksLikeUrl =
            !input.contains(' ') &&
            (input.contains('.') || input.startsWith("localhost") || input.contains("://"));

        if (looksLikeUrl) {
            current->load(QUrl::fromUserInput(input));
        } else {
            QString base = currentsearchengine;
            while (base.endsWith('/')) base.chop(1);
            const QString searchUrl =
                base + "/search?q=" + QString::fromUtf8(QUrl::toPercentEncoding(input));
            current->load(QUrl(searchUrl));
        }
    });

    connect(m_profile, &QWebEngineProfile::downloadRequested, this, [this](QWebEngineDownloadRequest *download) {
        const QString dlDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        const QString path = QFileDialog::getSaveFileName(
            this, tr("Save as..."), dlDir + "/" + download->suggestedFileName());

        if (path.isEmpty()) {
            download->cancel();
            return;
        }

        const QFileInfo fi(path);
        download->setDownloadDirectory(fi.absolutePath()),
        download->setDownloadFileName(fi.fileName());

        connect(download, &QWebEngineDownloadRequest::receivedBytesChanged, this, [download] {
            qDebug() << download->receivedBytes() << "/" << download->totalBytes();
        });

        connect(download, &QWebEngineDownloadRequest::isFinishedChanged, this, [download] {
            if (download->state() == QWebEngineDownloadRequest::DownloadCompleted)
                qDebug() << "Done:" << download->downloadDirectory() + "/" + download->downloadFileName();
            else if (download->state() == QWebEngineDownloadRequest::DownloadInterrupted)
                qDebug() << "Failed:" << download->interruptReasonString();
        });

        download->accept();
    });

    connect(tabwidget, &QTabWidget::currentChanged, this, [tabwidget, linedit](int i) {
        if (auto *v = qobject_cast<QWebEngineView *>(tabwidget->widget(i)))
            linedit->setText(v->url().toString());
        else
            linedit->clear();
    });

    auto addNewTab = [this, tabwidget, linedit](const QUrl &url) {
        auto *view = new QWebEngineView(tabwidget);
        auto *page = new QWebEnginePage(m_profile, view);
        view->setPage(page);

        connect(view, &QWebEngineView::titleChanged, this, [tabwidget, view](const QString &title) {
            const int i = tabwidget->indexOf(view);
            const QString cuttitle = (title.length() > 10) ? title.left(10) + "..." : title;
            if (i != -1) tabwidget->setTabText(i, cuttitle);
        });

        connect(view, &QWebEngineView::urlChanged, this, [tabwidget, view, linedit](const QUrl &u) {
            if (tabwidget->currentWidget() == view)
                linedit->setText(u.toString());
        });

        const int index = tabwidget->addTab(view, "New Tab");
        tabwidget->setCurrentIndex(index);
        view->setUrl(url);
    };

    connect(tabwidget, &QTabWidget::tabCloseRequested, this, [this, tabwidget](int index) {
        QWidget *w = tabwidget->widget(index);
        tabwidget->removeTab(index);
        w->deleteLater();
        if (tabwidget->count() == 0 && !disableclose) close();
    });

    connect(searchengine, &QLineEdit::returnPressed, this, [this, searchengine]() {
        const QString text = searchengine->text().trimmed();
        if (text.isEmpty()) return;
        m_db.savesearchengine(text);
        currentsearchengine = text;
    });

    connect(menu, &QToolButton::clicked, this, [tabwidget, linedit, closesettings, searchengine]() {
        tabwidget->hide();
        linedit->hide();
        closesettings->show();
        searchengine->show();
    });

    connect(closesettings, &QToolButton::clicked, this, [searchengine, closesettings, tabwidget, linedit]() {
        searchengine->hide();
        closesettings->hide();
        tabwidget->show();
        linedit->show();
    });

    connect(closeall, &QToolButton::clicked, this, [this, tabwidget, addNewTab]() {
        disableclose = true;
        while (tabwidget->count() > 0) {
            QWidget *w = tabwidget->widget(0);
            tabwidget->removeTab(0);
            w->deleteLater();
        }
        addNewTab(QUrl(currentsearchengine));
        disableclose = false;
    });

    connect(newtab, &QToolButton::clicked, this, [this, addNewTab]() {
        addNewTab(QUrl(currentsearchengine));
    });

    addNewTab(QUrl(currentsearchengine));
}