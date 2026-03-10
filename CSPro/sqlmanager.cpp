#include "SqlManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QThread>
#include <QDebug>
/**
 * 作用：QThreadStorage 是 Qt 提供的线程局部存储模板类，为每个线程存储一份独立的 QSqlDatabase 对象。
 * 这样可以确保每个线程都有自己的数据库连接，避免多线程同时使用同一连接导致的冲突。
 * 为什么需要：SQLite 在写操作时会锁定数据库文件，如果多个线程共用同一个连接，会导致锁竞争和不可预期的行为。
 * 通过为每个线程维护独立连接，可以实现并发读（SQLite 的 WAL 模式允许多个读与一个写并发）。
     */
QThreadStorage<QSqlDatabase> SqlManager::s_threadDb;
/**
 * 参数：dbPath 是 SQLite 数据库文件的路径（默认 "server.db"），parent 是 Qt 父对象。
 * 功能：仅保存数据库路径并初始化成员变量，不进行实际连接或表创建，真正的初始化在 initialize() 中完成。
 *
 * m_initialized：标记是否已初始化，避免重复执行初始化代码。
*/
SqlManager::SqlManager(const QString &dbPath, QObject *parent)
    : QObject(parent)
    , m_dbPath(dbPath)
    , m_initialized(false)
{

}

SqlManager::~SqlManager()
{
    // QThreadStorage 会自动清理线程结束时未关闭的连接
}
/**
 * QWriteLocker：保护 m_initialized 和初始化过程，确保多线程环境下初始化只执行一次。
 * if (m_initialized)：如果已经初始化，直接返回成功。
 *
 * m_initialized：标记是否已初始化，避免重复执行初始化代码。
*/
bool SqlManager::initialize()
{
    QWriteLocker locker(&m_initLock);
    if (m_initialized) return true;

    // 使用一个临时的初始化连接
    QSqlDatabase initDb = QSqlDatabase::addDatabase("QSQLITE", "init_connection");
    // 这里是路径
    initDb.setDatabaseName(m_dbPath);
    /*
     * SqlDatabase::open() 会自动创建一个空的数据库文件（如 server.db）。
     * 执行 CREATE TABLE IF NOT EXISTS 时，因为表还不存在，会创建表。
     * 表被创建，数据表结构建立，但表中无数据。*/
    if (!initDb.open()) {
        qCritical() << "[SqlManager] Failed to open database for initialization:" << initDb.lastError().text();
        return false;
    }

    // 启用 WAL 模式，提升并发性能
    /**
     * PRAGMA journal_mode=WAL;：启用 WAL（Write-Ahead Logging）模式，允许多个读操作与一个写操作并发，极大提升并发性能。
     * PRAGMA synchronous=NORMAL;：设置同步级别为 NORMAL，在保证数据安全的同时减少磁盘写入次数，平衡性能与可靠性。
     */
    QSqlQuery query(initDb);
    if (!query.exec("PRAGMA journal_mode=WAL;")) {
        qWarning() << "[SqlManager] Failed to set WAL mode:" << query.lastError().text();
    }
    query.exec("PRAGMA synchronous=NORMAL;"); // 平衡性能与安全性

    // 创建 connection_log 表
    /**
     * id：自增主键。
     * client_ip：客户端 IP，字符串。
     * client_port：端口号。
     * protocol：协议类型（TCP/UDP）。
     * event_type：事件类型（CONNECT/DISCONNECT/HEARTBEAT）。
     * created_at：记录时间，默认当前时间。
     */
    bool ok = query.exec(R"(
        CREATE TABLE IF NOT EXISTS connection_log (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            client_ip TEXT NOT NULL,
            client_port INTEGER NOT NULL,
            protocol TEXT NOT NULL,
            event_type TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )");
    if (!ok) {
        qCritical() << "[SqlManager] Failed to create connection_log table:" << query.lastError().text();
        initDb.close();
        QSqlDatabase::removeDatabase("init_connection");
        return false;
    }

    // 创建 processed_data 表
    /**
     * original_data BLOB：原始二进制数据。
     * decoded_message TEXT：解析后的文本。
     * sender_ip、sender_port：发送者地址。
     * protocol：协议类型。
     * created_at：时间戳。
     */
    ok = query.exec(R"(
        CREATE TABLE IF NOT EXISTS processed_data (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            original_data BLOB,
            decoded_message TEXT,
            sender_ip TEXT,
            sender_port INTEGER,
            protocol TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )");
    if (!ok) {
        qCritical() << "[SqlManager] Failed to create processed_data table:" << query.lastError().text();
        initDb.close();
        QSqlDatabase::removeDatabase("init_connection");
        return false;
    }

    // 创建索引以加速查询
    // 为常用查询字段（IP、时间）创建索引，加速 WHERE 条件查询。
    query.exec("CREATE INDEX IF NOT EXISTS idx_conn_ip ON connection_log(client_ip);");
    query.exec("CREATE INDEX IF NOT EXISTS idx_conn_created ON connection_log(created_at);");
    query.exec("CREATE INDEX IF NOT EXISTS idx_data_ip ON processed_data(sender_ip);");
    query.exec("CREATE INDEX IF NOT EXISTS idx_data_created ON processed_data(created_at);");

    initDb.close();
    QSqlDatabase::removeDatabase("init_connection");

    // 完成初始化
    m_initialized = true;
    qDebug() << "[SqlManager] Database initialized successfully at" << m_dbPath;
    return true;
}

QSqlDatabase SqlManager::getThreadLocalConnection()
{
    //检查初始化：确保在调用前已执行 initialize()。
    if (!m_initialized) {
        qWarning() << "[SqlManager] Not initialized. Call initialize() first.";
        return QSqlDatabase();
    }

    /**
     * 线程本地连接：如果当前线程还没有关联的数据库连接，则创建一个。
     * 连接名使用当前线程 ID 构造，确保唯一性。
     * 打开数据库文件（如果文件不存在则自动创建，但此时表应已由 initialize() 创建好）。
     * 如果打开失败，输出错误并返回无效连接。
     */
    if (!s_threadDb.hasLocalData()) {
        // 为当前线程创建唯一的连接名（基于线程ID）
        QString connName = QString("thread_%1").arg(reinterpret_cast<quint64>(QThread::currentThreadId()));
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
        db.setDatabaseName(m_dbPath);
        if (!db.open()) {
            qCritical() << "[SqlManager] Failed to open database connection for thread:" << db.lastError().text();
            return QSqlDatabase();
        }
        // 设置线程连接的参数
        /**
         * 设置线程连接的参数：对新连接也设置 synchronous=NORMAL 和缓存大小（10MB）。
         * 存入线程局部存储：s_threadDb.setLocalData(db) 将连接与当前线程关联。
         * 返回连接：如果已存在则直接返回存储的连接。
         */
        QSqlQuery query(db);
        query.exec("PRAGMA synchronous=NORMAL;");
        query.exec("PRAGMA cache_size=10000;"); // 设置缓存大小（可选）
        s_threadDb.setLocalData(db);
    }
    return s_threadDb.localData();
}

// -------------------- connection_log 操作 --------------------
bool SqlManager::insertConnectionLog(const ConnectionLog &log)
{
    // 获取线程本地连接并检查有效性。
    QSqlDatabase db = getThreadLocalConnection();
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "[SqlManager] Database connection is invalid";
        return false;
    }

    // 准备插入语句：使用参数化查询防止 SQL 注入。
    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO connection_log (client_ip, client_port, protocol, event_type, created_at)
        VALUES (:ip, :port, :protocol, :event_type, :created_at)
    )");
    // 绑定参数：通过 log 对象的 getter 方法获取数据。
    query.bindValue(":ip", log.clientIp());
    query.bindValue(":port", log.clientPort());
    query.bindValue(":protocol", log.protocol());
    query.bindValue(":event_type", log.eventType());
    query.bindValue(":created_at", log.createdAt());

    if (!query.exec()) {
        qWarning() << "[SqlManager] Failed to insert connection log:" << query.lastError().text();
        return false;
    }
    return true;
}

QList<ConnectionLog*> SqlManager::selectConnectionLogs(const QString &clientIp,
                                                        const QDateTime &from,
                                                        const QDateTime &to,
                                                        int limit)
{
    // 获取连接并检查，如果失败返回空列表。
    QList<ConnectionLog*> logs;
    QSqlDatabase db = getThreadLocalConnection();
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "[SqlManager] Database connection is invalid";
        return logs;
    }

    QString sql = "SELECT id, client_ip, client_port, protocol, event_type, created_at FROM connection_log WHERE 1=1";
    if (!clientIp.isEmpty()) {
        sql += " AND client_ip = :ip";
    }
    if (from.isValid()) {
        sql += " AND created_at >= :from";
    }
    if (to.isValid()) {
        sql += " AND created_at <= :to";
    }
    sql += " ORDER BY created_at DESC LIMIT :limit";

    QSqlQuery query(db);
    query.prepare(sql);
    if (!clientIp.isEmpty()) query.bindValue(":ip", clientIp);
    if (from.isValid()) query.bindValue(":from", from);
    if (to.isValid()) query.bindValue(":to", to);
    query.bindValue(":limit", limit);

    if (!query.exec()) {
        qWarning() << "[SqlManager] Failed to query connection logs:" << query.lastError().text();
        return logs;
    }

    while (query.next()) {
        ConnectionLog *log = new ConnectionLog();
        log->setId(query.value("id").toLongLong());
        log->setClientIp(query.value("client_ip").toString());
        log->setClientPort(static_cast<quint16>(query.value("client_port").toUInt()));
        log->setProtocol(query.value("protocol").toString());
        log->setEventType(query.value("event_type").toString());
        log->setCreatedAt(query.value("created_at").toDateTime());
        logs.append(log);
    }
    return logs;
}

// -------------------- processed_data 操作 --------------------
bool SqlManager::insertProcessedData(const ProcessedDataEntity &data)
{
    QSqlDatabase db = getThreadLocalConnection();
    if (!db.isValid() || !db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO processed_data (original_data, decoded_message, sender_ip, sender_port, protocol, created_at)
        VALUES (:orig, :decoded, :ip, :port, :protocol, :created_at)
    )");
    query.bindValue(":orig", data.originalData());
    query.bindValue(":decoded", data.decodedMessage());
    query.bindValue(":ip", data.senderIp());
    query.bindValue(":port", data.senderPort());
    query.bindValue(":protocol", data.protocol());
    query.bindValue(":created_at", data.createdAt());

    if (!query.exec()) {
        qWarning() << "[SqlManager] Failed to insert processed data:" << query.lastError().text();
        return false;
    }
    return true;
}

QList<ProcessedDataEntity*> SqlManager::selectProcessedData(const QString &senderIp,
                                                             const QDateTime &from,
                                                             const QDateTime &to,
                                                             int limit)
{
    QList<ProcessedDataEntity*> list;
    QSqlDatabase db = getThreadLocalConnection();
    if (!db.isValid() || !db.isOpen()) return list;

    QString sql = "SELECT id, original_data, decoded_message, sender_ip, sender_port, protocol, created_at FROM processed_data WHERE 1=1";
    if (!senderIp.isEmpty()) sql += " AND sender_ip = :ip";
    if (from.isValid()) sql += " AND created_at >= :from";
    if (to.isValid()) sql += " AND created_at <= :to";
    sql += " ORDER BY created_at DESC LIMIT :limit";

    QSqlQuery query(db);
    query.prepare(sql);
    if (!senderIp.isEmpty()) query.bindValue(":ip", senderIp);
    if (from.isValid()) query.bindValue(":from", from);
    if (to.isValid()) query.bindValue(":to", to);
    query.bindValue(":limit", limit);

    if (!query.exec()) {
        qWarning() << "[SqlManager] Failed to query processed data:" << query.lastError().text();
        return list;
    }

    while (query.next()) {
        ProcessedDataEntity *data = new ProcessedDataEntity();
        data->setId(query.value("id").toLongLong());
        data->setOriginalData(query.value("original_data").toByteArray());
        data->setDecodedMessage(query.value("decoded_message").toString());
        data->setSenderIp(query.value("sender_ip").toString());
        data->setSenderPort(static_cast<quint16>(query.value("sender_port").toUInt()));
        data->setProtocol(query.value("protocol").toString());
        data->setCreatedAt(query.value("created_at").toDateTime());
        list.append(data);
    }
    return list;
}
