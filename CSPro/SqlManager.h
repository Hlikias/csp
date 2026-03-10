#ifndef SQLMANAGER_H
#define SQLMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QThreadStorage>
#include <QReadWriteLock>
#include <QList>
#include <QDateTime>
#include "NetManager/connectionlog.h"


/**
 * @brief SQLite 数据库管理器类
 *
 * 提供线程安全的数据库连接管理，以及对 connection_log 和 processed_data 表的增删改查操作。
 * 使用 WAL 模式以支持高并发读。
 */
class SqlManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param dbPath SQLite 数据库文件路径，默认为 "server.db"
     * @param parent Qt 父对象
     */
    explicit SqlManager(const QString &dbPath = "server.db", QObject *parent = nullptr);
    ~SqlManager();

    /**
     * @brief 初始化数据库，创建表并设置参数（必须在任何数据库操作前调用一次）
     * @return 成功返回 true，否则 false
     */
    bool initialize();

    // -------------------- connection_log 表操作 --------------------
    /**
     * @brief 插入一条连接日志
     * @param log ConnectionLog 对象
     * @return 成功返回 true
     */
    bool insertConnectionLog(const ConnectionLog &log);

    /**
     * @brief 查询连接日志，支持按 IP、时间范围过滤
     * @param clientIp 客户端 IP（为空则忽略）
     * @param from 起始时间（无效则忽略）
     * @param to 结束时间（无效则忽略）
     * @param limit 最大返回条数，默认 100
     * @return 符合条件的日志列表
     */
    QList<ConnectionLog*> selectConnectionLogs(const QString &clientIp = QString(),
                                              const QDateTime &from = QDateTime(),
                                              const QDateTime &to = QDateTime(),
                                              int limit = 100);

    // -------------------- processed_data 表操作 --------------------
    /**
     * @brief 插入一条处理后的数据
     * @param data ProcessedDataEntity 对象
     * @return 成功返回 true
     */
    bool insertProcessedData(const ProcessedDataEntity &data);

    /**
     * @brief 查询处理数据，支持按发送者 IP、时间范围过滤
     * @param senderIp 发送者 IP（为空则忽略）
     * @param from 起始时间（无效则忽略）
     * @param to 结束时间（无效则忽略）
     * @param limit 最大返回条数，默认 100
     * @return 符合条件的数据列表
     */
    QList<ProcessedDataEntity*> selectProcessedData(const QString &senderIp = QString(),
                                                   const QDateTime &from = QDateTime(),
                                                   const QDateTime &to = QDateTime(),
                                                   int limit = 100);

private:
    // 获取当前线程的数据库连接
    QSqlDatabase getThreadLocalConnection();

    QString m_dbPath;                     ///< 数据库文件路径
    static QThreadStorage<QSqlDatabase> s_threadDb;  ///< 线程本地存储的连接
    QReadWriteLock m_initLock;             ///< 保护初始化过程
    bool m_initialized;                    ///< 是否已初始化
};

#endif // SQLMANAGER_H
