#ifndef PORTA_BROWSER_DATABASEMANAGER_H
#define PORTA_BROWSER_DATABASEMANAGER_H

#include <QString>

class DatabaseManager
{
public:
    DatabaseManager() = default;
    ~DatabaseManager();

    bool open(const QString &path);
    bool createTables();
    bool addurl(const QString &name, const QString &url);
    bool savesearchengine(const QString &searchengineurl);
    QString loadsearchengine();
    void printHistory();

private:
    QString m_connectionName;
};

#endif // PORTA_BROWSER_DATABASEMANAGER_H
