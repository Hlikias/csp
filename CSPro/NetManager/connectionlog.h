#ifndef CONNECTIONLOG_H
#define CONNECTIONLOG_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QByteArray>

/**
 * @brief 连接日志实体类，对应数据库表 connection_log
 */
class ConnectionLog : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionLog(QObject *parent = nullptr);
    ConnectionLog(const QString &clientIp, quint16 clientPort,
                  const QString &protocol, const QString &eventType,
                  const QDateTime &createdAt = QDateTime::currentDateTime(),
                  QObject *parent = nullptr);

    // Getters
    qint64 id() const;
    QString clientIp() const;
    quint16 clientPort() const;
    QString protocol() const;
    QString eventType() const;
    QDateTime createdAt() const;

    // Setters
    void setId(qint64 id);
    void setClientIp(const QString &ip);
    void setClientPort(quint16 port);
    void setProtocol(const QString &protocol);
    void setEventType(const QString &eventType);
    void setCreatedAt(const QDateTime &createdAt);

signals:
    // 可根据需要添加信号

private:
    qint64 m_id;
    QString m_clientIp;
    quint16 m_clientPort;
    QString m_protocol;
    QString m_eventType;
    QDateTime m_createdAt;
};

/**
 * @brief 处理后的数据实体类，对应数据库表 processed_data
 */
class ProcessedDataEntity : public QObject
{
    Q_OBJECT
public:
    explicit ProcessedDataEntity(QObject *parent = nullptr);
    ProcessedDataEntity(const QByteArray &originalData, const QString &decodedMessage,
                        const QString &senderIp, quint16 senderPort,
                        const QString &protocol,
                        const QDateTime &createdAt = QDateTime::currentDateTime(),
                        QObject *parent = nullptr);

    // Getters
    qint64 id() const;
    QByteArray originalData() const;
    QString decodedMessage() const;
    QString senderIp() const;
    quint16 senderPort() const;
    QString protocol() const;
    QDateTime createdAt() const;

    // Setters
    void setId(qint64 id);
    void setOriginalData(const QByteArray &data);
    void setDecodedMessage(const QString &message);
    void setSenderIp(const QString &ip);
    void setSenderPort(quint16 port);
    void setProtocol(const QString &protocol);
    void setCreatedAt(const QDateTime &createdAt);

signals:
    // 可根据需要添加信号

private:
    qint64 m_id;
    QByteArray m_originalData;
    QString m_decodedMessage;
    QString m_senderIp;
    quint16 m_senderPort;
    QString m_protocol;
    QDateTime m_createdAt;
};
#endif // CONNECTIONLOG_H
