//
// Created by joshua on 16.11.26.
//

#include "DatabaseManager.h"

#ifndef PORTA_BROWSER_MAINWINDOW_H
#define PORTA_BROWSER_MAINWINDOW_H

#pragma once
#include <QMainWindow>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
private:
    bool disableclose = false;
private:
    QString currentsearchengine = QStringLiteral("http://www.duckduckgo.com");
private:
DatabaseManager m_db;
};

#endif //PORTA_BROWSER_MAINWINDOW_H
