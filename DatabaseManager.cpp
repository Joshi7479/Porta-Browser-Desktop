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

    if (!q.exec("CREATE TABLE IF NOT EXISTS closeifalltabsclosed ("
        "id INTEGER PRIMARY KEY CHECK (id = 1), "
        "disableclose BOOLEAN NOT NULL DEFAULT 0)")) {
        qWarning() << q.lastError().text();
        return false;
                }

    if (!q.exec("CREATE TABLE IF NOT EXISTS invertlayout ("
        "id INTEGER PRIMARY KEY CHECK (id = 1), "
        "invertlayout BOOLEAN NOT NULL DEFAULT 0)")) {
        qWarning() << q.lastError().text();
        return false;
        }

    if (!q.exec("CREATE TABLE IF NOT EXISTS searchengine ("
                "id INTEGER PRIMARY KEY CHECK (id = 1), "
                "searchengineurl TEXT NOT NULL)")) {
        qWarning() << q.lastError().text();
        return false;
    }

    if (!q.exec("CREATE TABLE IF NOT EXISTS hotkey ("
                "id INTEGER PRIMARY KEY CHECK (id = 1), "
                "hotkeyurl TEXT NOT NULL)")) {
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

bool DatabaseManager::savehotkey(const QString &hotkey)
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return false;

    QSqlQuery q(db);
    q.prepare("INSERT INTO hotkey (id, hotkeyurl) "
              "VALUES (1, :url) "
              "ON CONFLICT(id) DO UPDATE SET hotkeyurl = excluded.hotkeyurl");
    q.bindValue(":url", hotkey);

    if (!q.exec()) {
        qWarning() << q.lastError().text();
        return false;
    }
    return true;
}

QString DatabaseManager::loadhotkey()
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return QString();

    QSqlQuery q(db);
    if (!q.exec("SELECT hotkeyurl FROM hotkey WHERE id = 1")) {
        qWarning() << q.lastError().text();
        return QString();
    }
    return q.next() ? q.value(0).toString() : QString();
}

bool DatabaseManager::savedisableclosestate(const bool &disableclosestate)
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return false;

    QSqlQuery q(db);
    q.prepare("INSERT INTO closeifalltabsclosed (id, disableclose) "
          "VALUES (1, :disableclosestate) "
          "ON CONFLICT(id) DO UPDATE SET disableclose = excluded.disableclose");
    q.bindValue(":disableclosestate", disableclosestate);

    if (!q.exec()) {
        qWarning() << q.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::loaddisableclosestate()
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen())
        return false;

    QSqlQuery q(db);
    if (!q.exec("SELECT disableclose FROM closeifalltabsclosed WHERE id = 1")) {
        qWarning() << q.lastError().text();
        return false;
    }

    return q.next() ? q.value(0).toBool() : false;
}

bool DatabaseManager::saveinvertlayoutstate(const bool &invertlayout)
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return false;

    QSqlQuery q(db);
    q.prepare("INSERT INTO invertlayout (id, invertlayout) "
          "VALUES (1, :invertlayout) "
          "ON CONFLICT(id) DO UPDATE SET invertlayout = excluded.invertlayout");
    q.bindValue(":invertlayout", invertlayout);

    if (!q.exec()) {
        qWarning() << q.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::loadinvertlayoutstate()
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen())
        return false;

    QSqlQuery q(db);
    if (!q.exec("SELECT invertlayout FROM invertlayout WHERE id = 1")) {
        qWarning() << q.lastError().text();
        return false;
    }

    return q.next() ? q.value(0).toBool() : false;
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

bool DatabaseManager::deleteallhistory() {
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return false;

    QSqlQuery q(db);
    q.prepare("DELETE FROM history");
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

bool DatabaseManager::deletebookmarks(QString url)
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return false;

    QSqlQuery q(db);
    q.prepare("DELETE FROM bookmarks WHERE url = :url");
    q.bindValue(":url", url);

    if (!q.exec()) {
        qWarning() << q.lastError().text();
        return false;
    }
    return true;
}
bool DatabaseManager::bookmarkExists(const QString &url) {
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return false;

    QSqlQuery q(db);
    q.prepare("SELECT 1 FROM bookmarks WHERE url = :url LIMIT 1");
    q.bindValue(":url", url);
    if (!q.exec()) {
        qWarning() << q.lastError().text();
        return false;
    }
    return q.next();
}
