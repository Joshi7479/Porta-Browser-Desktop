#include "DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QUuid>
#include <QFileInfo>
#include <QDir>

DatabaseManager::~DatabaseManager()
{
    if (!m_connectionName.isEmpty()) {
        {
            QSqlDatabase db = QSqlDatabase::database(m_connectionName, false);
            if (db.isOpen()) {
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(m_connectionName);
    }
}

bool DatabaseManager::open(const QString &path)
{
    QFileInfo fileInfo(path);
    QDir().mkpath(fileInfo.absolutePath());

    m_connectionName = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    db.setDatabaseName(path);

    if (!db.open()) {
        qWarning() << "DB: Failed:" << db.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::createTables()
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return false;

    QSqlQuery q(db);

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
    if (!q.exec("CREATE TABLE IF NOT EXISTS bookmarks ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "name TEXT NOT NULL, "
                "url TEXT NOT NULL)")) {
        qWarning() << q.lastError().text();
        return false;
                }

    return true;
}

bool DatabaseManager::addurl(const QString &name, const QString &url)
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return false;

    QSqlQuery q(db);
    q.prepare("INSERT INTO history (name, url) VALUES (:name, :url)");
    q.bindValue(":name", name);
    q.bindValue(":url", url);

    if (!q.exec()) {
        qWarning() << q.lastError().text();
        return false;
    }
    return true;
}
bool DatabaseManager::addbookmark(const QString &name, const QString &url)
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return false;

    QSqlQuery q(db);
    q.prepare("INSERT INTO bookmarks (name, url) VALUES (:name, :url)");
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
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return false;

    QSqlQuery q(db);
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
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return QString();

    QSqlQuery q(db);
    if (!q.exec("SELECT searchengineurl FROM searchengine WHERE id = 1")) {
        qWarning() << q.lastError().text();
        return QString();
    }
    return q.next() ? q.value(0).toString() : QString();
}

void DatabaseManager::printHistory()
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return;

    QSqlQuery q(db);
    q.exec("SELECT id, name, url FROM history");
    while (q.next()) {
        qDebug() << q.value(0).toInt()
                 << q.value(1).toString()
                 << q.value(2).toString();
    }
}

QList<HistoryEntry> DatabaseManager::loadhistory()
{
    QList<HistoryEntry> result;

    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return result;

    QSqlQuery q(db);
    if (!q.exec("SELECT id, name, url FROM history ORDER BY id DESC")) {
        qWarning() << q.lastError().text();
        return result;
    }

    while (q.next()) {
        HistoryEntry e;
        e.id   = q.value(0).toInt();
        e.name = q.value(1).toString();
        e.url  = q.value(2).toString();
        result.append(e);
    }
    return result;
}

bool DatabaseManager::deletehistory(int id)
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return false;

    QSqlQuery q(db);
    q.prepare("DELETE FROM history WHERE id = :id");
    q.bindValue(":id", id);

    if (!q.exec()) {
        qWarning() << q.lastError().text();
        return false;
    }
    return true;
}

QList<bookmarks> DatabaseManager::loadbookmarks()
{
    QList<bookmarks> result;

    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return result;

    QSqlQuery q(db);
    if (!q.exec("SELECT id, name, url FROM bookmarks ORDER BY id DESC")) {
        qWarning() << q.lastError().text();
        return result;
    }

    while (q.next()) {
        bookmarks e;
        e.id   = q.value(0).toInt();
        e.name = q.value(1).toString();
        e.url  = q.value(2).toString();
        result.append(e);
    }
    return result;
}

bool DatabaseManager::deletebookmarks(int id)
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return false;

    QSqlQuery q(db);
    q.prepare("DELETE FROM bookmarks WHERE id = :id");
    q.bindValue(":id", id);

    if (!q.exec()) {
        qWarning() << q.lastError().text();
        return false;
    }
    return true;
}
