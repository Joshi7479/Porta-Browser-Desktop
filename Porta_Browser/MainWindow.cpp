#include "MainWindow.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QWebEngineView>
#include <QTabWidget>
#include <QWidget>
#include <QUrl>
#include <QToolButton>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    auto *linedit = new QLineEdit(central);
    auto *tabwidget = new QTabWidget(central);
    auto *newtab = new QToolButton(tabwidget);
    newtab->setText("+");
    newtab->setAutoRaise(true);
    tabwidget->setTabsClosable(true);
    tabwidget->setMovable(true);
    tabwidget->setCornerWidget(newtab, Qt::TopRightCorner);

    auto *webview = new QWebEngineView();

    layout->addWidget(linedit);
    layout->addWidget(tabwidget);
    setCentralWidget(central);

    connect(tabwidget, &QTabWidget::tabCloseRequested, this, [=](int index) {
        QWidget *w = tabwidget->widget(index);
        tabwidget->removeTab(index);
        w->deleteLater();
        if (tabwidget->count() == 0) close();
    });

    connect(linedit, &QLineEdit::returnPressed, this, [=]() {
        auto *current = qobject_cast<QWebEngineView *>(tabwidget->currentWidget());
        if (!current) return;

        QString input = linedit->text().trimmed();
        if (input.contains('.') && !input.contains(' ')) {
            current->load(QUrl::fromUserInput(input));
        } else {
            current->load(QUrl("https://duckduckgo.com/search?q=" +
                               QUrl::toPercentEncoding(input)));
        }
    });

    connect(webview, &QWebEngineView::titleChanged, this, [=](const QString &title) {
        int i = tabwidget->indexOf(webview);
        if (i != -1) tabwidget->setTabText(i, title);
    });

    auto addNewTab = [=](const QUrl &url) {
        auto *view = new QWebEngineView();
        view->setUrl(url);
        int index = tabwidget->addTab(view, "New Tab");
        tabwidget->setCurrentIndex(index);

        connect(view, &QWebEngineView::titleChanged, this, [=](const QString &title) {
            int i = tabwidget->indexOf(view);
            if (i != -1) tabwidget->setTabText(i, title);
        });
    };

    addNewTab(QUrl("https://github.com/Joshi7479/Porta-Browser-Desktop"));

    connect(newtab, &QToolButton::clicked, this, [=]() {
        addNewTab(QUrl("https://duckduckgo.com"));
    });
}