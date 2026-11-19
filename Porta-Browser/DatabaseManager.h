#ifndef PORTA_BROWSER_DATABASEMANAGER_H
#define PORTA_BROWSER_DATABASEMANAGER_H

#include <QString>
#include <QList>

struct HistoryEntry {
    int id;
    QString name;
    QString url;
};

struct bookmarks {
    int id;
    QString name;
    QString url;
};

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
    QList <HistoryEntry> loadhistory();
    bool deletehistory(int id);
    QList <bookmarks> loadbookmarks();
    bool deletebookmarks(int id);
    bool addbookmark(const QString &name, const QString &url);

private:
    QString m_connectionName;
};

#endif // PORTA_BROWSER_DATABASEMANAGER_H
