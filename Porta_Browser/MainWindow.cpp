#include "MainWindow.h"
#include <iostream>
#include <string>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QWebEngineView>
#include <QTabWidget>
#include <QWidget>
#include <QUrl>
#include <QToolButton>
#include "DatabaseManager.h"
#include <QDir>
#include <QStandardPaths>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    if (!m_db.open(dir + "/browser.db") || !m_db.createTables()) {
        qWarning() << "Internal service: m_db.open failed!";
    }
    m_db.createTables();

    currentsearchengine = m_db.loadsearchengine();

    auto *central = new QWidget(this);
    auto *settings = new QWidget(central);
    auto *searchengine = new QLineEdit(settings);
    auto *closesettings = new QToolButton(settings);
    auto *layout = new QVBoxLayout(central);
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
    layout->addWidget(searchengine);
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

    layout->addWidget(linedit);
    linedit->setPlaceholderText("URL or search");
    layout->addWidget(tabwidget);
    layout->addWidget(closesettings);
    layout->addWidget(settings);
    setCentralWidget(central);

    settings->hide();
    closesettings->hide();
    searchengine->hide();

    auto addNewTab = [=](const QUrl &url) {
        auto *view = new QWebEngineView();
        view->setUrl(url);
        int index = tabwidget->addTab(view, "New Tab");
        tabwidget->setCurrentIndex(index);

        connect(view, &QWebEngineView::titleChanged, this, [=](const QString &title) {
            int i = tabwidget->indexOf(view);
            QString cuttitle = (title.length() > 10) ? title.left(10) + "..." : title;
            if (i != -1) tabwidget->setTabText(i, cuttitle);
        });
    };

    connect(tabwidget, &QTabWidget::tabCloseRequested, this, [=](int index) {
        QWidget *w = tabwidget->widget(index);
        tabwidget->removeTab(index);
        w->deleteLater();
        if (tabwidget->count() == 0 && !disableclose) close();
    });

    connect(searchengine, &QLineEdit::returnPressed, this, [=]() {
        m_db.savesearchengine(searchengine->text());
        currentsearchengine = searchengine->text();
    });

    connect(menu, &QToolButton::clicked, this, [=]() {
        tabwidget->hide();
        linedit->hide();
        settings->show();
        closesettings->show();
        searchengine->show();
    });

    connect(closesettings, &QToolButton::clicked, this, [=]() {
        settings->hide();
        searchengine->hide();
        closesettings->hide();
        tabwidget->show();
        linedit->show();
    });

    connect(closeall, &QToolButton::clicked, this, [=, this]() {
    disableclose = true;
    while (tabwidget->count() > 0) {
        QWidget *w = tabwidget->widget(0);
        tabwidget->removeTab(0);
        w->deleteLater();
    }
    addNewTab(QUrl(currentsearchengine));
    disableclose = false;
});

    connect(linedit, &QLineEdit::returnPressed, this, [=]() {
        auto *current = qobject_cast<QWebEngineView *>(tabwidget->currentWidget());
        if (!current) return;

        QString input = linedit->text().trimmed();
        if (input.contains('.') && !input.contains(' ')) {
            current->load(QUrl::fromUserInput(input));
        } else {
            current->load(QUrl(currentsearchengine + "/search?q=" +
                               QUrl::toPercentEncoding(input)));
        }

    });

    addNewTab(QUrl(currentsearchengine));

    connect(newtab, &QToolButton::clicked, this, [=]() {
        addNewTab(QUrl(currentsearchengine));
    });
}