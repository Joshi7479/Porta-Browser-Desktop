#include "MainWindow.h"

#include <QApplication>
#include <QtQuick>
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
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QLayoutItem>
#include <QPushButton>
#include <QFontMetrics>
#include <QResizeEvent>
#include <QCheckBox>


namespace {
    class ElidedButton : public QPushButton {
    public:
        ElidedButton(const QString &fullText, QWidget *parent = nullptr)
            : QPushButton(parent), m_full(fullText)
        {
            setFlat(true);
            setCursor(Qt::PointingHandCursor);
            setToolTip(fullText);
            setStyleSheet("text-align: left; padding: 4px; border: none;");
            setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
            updateText();
        }

    protected:
        void resizeEvent(QResizeEvent *e) override {
            QPushButton::resizeEvent(e);
            updateText();
        }

    private:
        void updateText() {
            const QFontMetrics fm(font());
            QString t = fm.elidedText(m_full, Qt::ElideRight, qMax(0, width() - 12));
            t.replace("&", "&&");
            setText(t);
        }

        QString m_full;
    };
}

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
    auto *saveSettings = new QToolButton(central);
    auto *linedit = new QLineEdit(central);
    auto *tabwidget = new QTabWidget(central);
    auto *newtab = new QToolButton(tabwidget);
    auto *closeall = new QToolButton(tabwidget);
    auto *menu = new QToolButton(tabwidget);
    auto *goback = new QToolButton(tabwidget);
    auto *goforward = new QToolButton(tabwidget);
    auto *reload = new QToolButton(tabwidget);
    auto *rightcorner = new QWidget(tabwidget);
    auto *rightlayout = new QHBoxLayout(rightcorner);
    auto *leftcorner = new QWidget(tabwidget);
    auto *leftlayout = new QHBoxLayout(leftcorner);
    auto *star = new QToolButton(tabwidget);
    auto *bookmarkbutton = new QToolButton(tabwidget);
    auto *disableclosebutton = new QCheckBox("Keep browser open when closing last tab", central);

    rightlayout->setContentsMargins(0, 0, 0, 0);
    rightlayout->setSpacing(2);
    rightlayout->addWidget(bookmarkbutton);
    rightlayout->addWidget(star);
    rightlayout->addWidget(goback);
    rightlayout->addWidget(reload);
    rightlayout->addWidget(goforward);
    rightlayout->addWidget(menu);

    leftlayout->addWidget(closeall);
    leftlayout->addWidget(newtab);
    leftlayout->setContentsMargins(0, 0, 0, 0);
    leftlayout->setSpacing(2);

    bookmarkbutton->setText("📚");
    star->setText("⭐");
    menu->setText("≡");
    closesettings->setText("X");
    saveSettings->setText("💾");
    closeall->setText("X");
    newtab->setText("+");
    goback->setText("<");
    reload->setText("⟳");
    goforward->setText(">");
    newtab->setAutoRaise(true);

    tabwidget->setTabsClosable(true);
    tabwidget->setMovable(true);
    tabwidget->setCornerWidget(rightcorner, Qt::TopRightCorner);
    tabwidget->setCornerWidget(leftcorner, Qt::TopLeftCorner);

    searchengine->setPlaceholderText("Search-Engine");
    searchengine->setText(currentsearchengine);
    linedit->setPlaceholderText("URL or search");

    auto *settingsBar = new QWidget(central);
    settingsBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    layout->setSpacing(4);
    auto *settingsBarLayout = new QHBoxLayout(settingsBar);
    settingsBarLayout->setContentsMargins(0, 0, 0, 0);
    settingsBarLayout->setSpacing(2);
    settingsBarLayout->addStretch();
    settingsBarLayout->addWidget(disableclosebutton);
    settingsBarLayout->addWidget(saveSettings);
    settingsBarLayout->addWidget(closesettings);
    auto *historyScroll = new QScrollArea(central);
    historyScroll->setWidgetResizable(true);
    auto *historyContent = new QWidget;
    m_historyLayout = new QVBoxLayout(historyContent);
    historyScroll->setWidget(historyContent);
    historyScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto *bookmarkscroll = new QScrollArea(central);
    bookmarkscroll->setWidgetResizable(true);
    auto *bookmarkContent = new QWidget;
    m_bookmarkLayout = new QVBoxLayout(bookmarkContent);
    bookmarkscroll->setWidget(bookmarkContent);
    bookmarkscroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    layout->addWidget(settingsBar);
    layout->addWidget(searchengine);
    layout->addWidget(historyScroll, 1);
    layout->addWidget(bookmarkscroll, 1);
    layout->addWidget(linedit);
    layout->addWidget(tabwidget, 1);
    setCentralWidget(central);

    bookmarkContent->hide();
    bookmarkscroll->hide();
    settingsBar->hide();
    searchengine->hide();
    historyScroll->hide();
    disableclosestate = m_db.loaddisableclosestate();
    disableclosebutton->setChecked(disableclosestate);

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
            this, tr("Download file to"), dlDir + "/" + download->suggestedFileName());

        if (path.isEmpty()) {
            download->cancel();
            return;
        }

        const QFileInfo fi(path);
        download->setDownloadDirectory(fi.absolutePath());
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

    connect(goback, &QToolButton::clicked, this, [tabwidget]() {
        auto *v = qobject_cast<QWebEngineView *>(tabwidget->currentWidget());
        if (v) v->back();
    });

    connect(goforward, &QToolButton::clicked, this, [tabwidget]() {
        auto *v = qobject_cast<QWebEngineView *>(tabwidget->currentWidget());
        if (v) v->forward();
    });

    connect(reload, &QToolButton::clicked, this, [tabwidget]() {
        auto *v = qobject_cast<QWebEngineView *>(tabwidget->currentWidget());
        if (v) v->reload();
    });

    connect(disableclosebutton, &QToolButton::clicked, this, [this, tabwidget](bool checked) {
        m_db.savedisableclosestate(checked);
        disableclosestate = true;
    });

    auto addNewTab = [this, tabwidget, star, linedit](const QUrl &url) {
        auto *view = new QWebEngineView(tabwidget);
        auto *page = new QWebEnginePage(m_profile, view);
        view->setPage(page);

        connect(view, &QWebEngineView::titleChanged, this, [tabwidget, view](const QString &title) {
            const int i = tabwidget->indexOf(view);
            const QString cuttitle = (title.length() > 10) ? title.left(10) + "..." : title;
            if (i != -1) tabwidget->setTabText(i, cuttitle);
        });

        connect(view, &QWebEngineView::urlChanged, this, [this, tabwidget, star, view, linedit](const QUrl &u) {
            if (tabwidget->currentWidget() == view)
                linedit->setText(u.toString());

            if (m_db.bookmarkExists(linedit->text())) {
                star->setText("🌟");
            } else {
                star->setText("⭐");
            }
        });

        connect(view, &QWebEngineView::loadFinished, this, [this, view](bool ok) {
            if (!ok) return;
            const QString pageUrl = view->url().toString();
            if (pageUrl.isEmpty() || pageUrl == "about:blank") return;
            m_db.addurl(view->title(), pageUrl);
        });

        const int index = tabwidget->addTab(view, "New Tab");
        tabwidget->setCurrentIndex(index);
        view->setUrl(url);

    };

    m_openUrl = [addNewTab, closesettings](const QUrl &u) {
        addNewTab(u);
        closesettings->click();
    };

    connect(star, &QToolButton::clicked, this, [this, star, tabwidget]() {
    auto *v = qobject_cast<QWebEngineView *>(tabwidget->currentWidget());
    if (!v) return;
    const QString url = v->url().toString();
    if (url.isEmpty() || url == "about:blank") return;
    if (m_db.bookmarkExists(url)) {
        m_db.deletebookmarks(url);
        star->setText("⭐");
    } else {
        m_db.addbookmark(v->title(), url);
        star->setText("🌟");
    }
    });

    connect(tabwidget, &QTabWidget::tabCloseRequested, this, [this, addNewTab, tabwidget](int index) {
        QWidget *w = tabwidget->widget(index);
        tabwidget->removeTab(index);
        w->deleteLater();
        disableclosestate = m_db.loaddisableclosestate();
        if (tabwidget->count() == 0 && !disableclose && !disableclosestate) {
            close();
        } else if (tabwidget->count() == 0) {
            addNewTab(QUrl(currentsearchengine));
        };

    });

    connect(searchengine, &QLineEdit::returnPressed, this, [this, searchengine]() {
        text = searchengine->text().trimmed();
        if (text.isEmpty()) return;
        m_db.savesearchengine(text);
        currentsearchengine = text;
    });

    connect(saveSettings, &QToolButton::clicked, this, [this, searchengine]() {
        text = searchengine->text().trimmed();
        if (text.isEmpty()) return;
        m_db.savesearchengine(text);
        currentsearchengine = text;
    });

    connect(menu, &QToolButton::clicked, this,
            [this, tabwidget, linedit, closesettings, searchengine, disableclosebutton, settingsBar, historyScroll]() {

        tabwidget->hide();
        linedit->hide();
        closesettings->show();
        settingsBar->show();
        searchengine->show();
        historyScroll->show();
        refreshHistory();
    });

    connect(bookmarkbutton, &QToolButton::clicked, this, [this, tabwidget, bookmarkContent, linedit, closesettings, settingsBar, bookmarkContent, bookmarkscroll]() {
        tabwidget->hide();
        linedit->hide();
        bookmarkContent->show();
        closesettings->show();
        settingsBar->show();
        bookmarkscroll->show();
        refreshBookmarks();
    });

    connect(closesettings, &QToolButton::clicked, this,
            [searchengine, closesettings, tabwidget, linedit, settingsBar, bookmarkscroll, historyScroll]() {
        searchengine->hide();
        closesettings->hide();
        historyScroll->hide();
        settingsBar->hide();
        tabwidget->show();
        linedit->show();
        bookmarkscroll->hide();
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

void MainWindow::refreshHistory()
{
    while (QLayoutItem *item = m_historyLayout->takeAt(0)) {
        if (QWidget *w = item->widget())
            w->deleteLater();
        delete item;
    }

    const QList<HistoryEntry> entries = m_db.loadhistory();
    for (const HistoryEntry &e : entries) {
        auto *row = new QWidget;
        auto *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);

        const QString shown = e.name.isEmpty() ? e.url
                                               : QString("%1 – %2").arg(e.name, e.url);
        auto *linkBtn = new ElidedButton(shown);
        auto *deleteBtn = new QPushButton("X");

        rowLayout->addWidget(linkBtn, 1);
        rowLayout->addWidget(deleteBtn);

        const int id = e.id;
        const QString url = e.url;

        connect(linkBtn, &QPushButton::clicked, this, [this, url]() {
            if (m_openUrl) m_openUrl(QUrl(url));
        });

        connect(deleteBtn, &QPushButton::clicked, this, [this, id]() {
            if (m_db.deletehistory(id))
                refreshHistory();
        });

        m_historyLayout->addWidget(row);
    }
    m_historyLayout->addStretch();
}
void MainWindow::refreshBookmarks()
{
    while (QLayoutItem *item = m_bookmarkLayout->takeAt(0)) {
        if (QWidget *w = item->widget())
            w->deleteLater();
        delete item;
    }

    const QList<bookmarks> entries = m_db.loadbookmarks();
    for (const bookmarks &e : entries) {
        auto *row = new QWidget;
        auto *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);

        const QString shown = e.name.isEmpty() ? e.url
                                               : QString("%1 – %2").arg(e.name, e.url);
        auto *linkBtn = new ElidedButton(shown);
        auto *deleteBtn = new QPushButton("X");

        rowLayout->addWidget(linkBtn, 1);
        rowLayout->addWidget(deleteBtn);

        const int id = e.id;
        const QString url = e.url;

        connect(linkBtn, &QPushButton::clicked, this, [this, url]() {
            if (m_openUrl) m_openUrl(QUrl(url));
        });

        connect(deleteBtn, &QPushButton::clicked, this, [this, url]() {
            if (m_db.deletebookmarks(url))
                refreshBookmarks();
        });

        m_bookmarkLayout->addWidget(row);
    }
    m_bookmarkLayout->addStretch();
}