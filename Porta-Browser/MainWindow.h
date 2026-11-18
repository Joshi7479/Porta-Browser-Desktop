//
// Created by joshua on 16.11.26.
//

#include "DatabaseManager.h"
#include <QWebEngineView>

#ifndef PORTA_BROWSER_MAINWINDOW_H
#define PORTA_BROWSER_MAINWINDOW_H

#pragma once
#include <QMainWindow>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
private:
    QString text = "";
private:
    QString currenturl = "https://duckduckgo.com";
private:
    bool disableclose = false;
private:
    QString currentsearchengine = QStringLiteral("https://www.duckduckgo.com");
private:
    QWebEngineProfile *m_profile = nullptr;
private:
DatabaseManager m_db;
};

#endif //PORTA_BROWSER_MAINWINDOW_H
