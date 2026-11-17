#include "DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen())
        m_db.close();
}

bool DatabaseManager::open(const QString &path)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open()) {
        qWarning() << "DB: Failed:" << m_db.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::createTables()
{
    QSqlQuery q;

    if (!q.exec("CREATE TABLE IF NOT EXISTS history ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "name TEXT NOT NULL, "
                "url TEXT NOT NULL)")) {
        qWarning() << q.lastError().text();
        return false;
                }

    if (!q.exec("CREATE TABLE IF NOT EXISTS searchengine ("
                "id INTEGER PRIMARY KEY CHECK (id = 1), "
                "searchengineurl TEXT NOT NULL)")) {
        qWarning() << q.lastError().text();
        return false;
                }

    return true;
}

bool DatabaseManager::addurl(const QString &name, QString url)
{
    QSqlQuery q;
    q.prepare("INSERT INTO history (name, url) VALUES (:name, :url)");
    q.bindValue(":name", name);
    q.bindValue(":url", url);

    if (!q.exec()) {
        qWarning() << q.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::savesearchengine(const QString &searchengineurl)
{
    QSqlQuery q;
    q.prepare("INSERT INTO searchengine (id, searchengineurl) "
              "VALUES (1, :url) "
              "ON CONFLICT(id) DO UPDATE SET searchengineurl = excluded.searchengineurl");
    q.bindValue(":url", searchengineurl);

    if (!q.exec()) {
        qWarning() << q.lastError().text();
        return false;
    }
    return true;
}

QString DatabaseManager::loadsearchengine()
{
    QSqlQuery q("SELECT searchengineurl FROM searchengine WHERE id = 1");
    return q.next() ? q.value(0).toString() : QString();
}

void DatabaseManager::printHistory()
{
    QSqlQuery q("SELECT id, name, url FROM history");
    while (q.next()) {
        qDebug() << q.value(0).toInt()
                 << q.value(1).toString()
                 << q.value(2).toInt();
    }
}
