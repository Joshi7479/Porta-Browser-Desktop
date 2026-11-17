//
// Created by joshua on 16.11.26.
//

#ifndef PORTA_BROWSER_DATABASEMANAGER_H
#define PORTA_BROWSER_DATABASEMANAGER_H

#pragma once

#include <QSqlDatabase>

class DatabaseManager
{
public:
    DatabaseManager() = default;
    ~DatabaseManager();

    bool open(const QString &path);
    bool createTables();
    bool addurl(const QString &name, QString url);
    bool savesearchengine(const QString &searchengineurl);
    QString loadsearchengine();
    void printHistory();

private:
    QSqlDatabase m_db;
};


#endif //PORTA_BROWSER_DATABASEMANAGER_H
