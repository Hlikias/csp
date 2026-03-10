#include "connectionlog.h"

// -------------------- ConnectionLog 实现 --------------------
ConnectionLog::ConnectionLog(QObject *parent)
    : QObject(parent)
    , m_id(0)
    , m_clientPort(0)
{
}

ConnectionLog::ConnectionLog(const QString &clientIp, quint16 clientPort,
                             const QString &protocol, const QString &eventType,
                             const QDateTime &createdAt, QObject *parent)
    : QObject(parent)
    , m_id(0)
    , m_clientIp(clientIp)
    , m_clientPort(clientPort)
    , m_protocol(protocol)
    , m_eventType(eventType)
    , m_createdAt(createdAt.isValid() ? createdAt : QDateTime::currentDateTime())
{
}

qint64 ConnectionLog::id() const { return m_id; }
QString ConnectionLog::clientIp() const { return m_clientIp; }
quint16 ConnectionLog::clientPort() const { return m_clientPort; }
QString ConnectionLog::protocol() const { return m_protocol; }
QString ConnectionLog::eventType() const { return m_eventType; }
QDateTime ConnectionLog::createdAt() const { return m_createdAt; }

void ConnectionLog::setId(qint64 id) { m_id = id; }
void ConnectionLog::setClientIp(const QString &ip) { m_clientIp = ip; }
void ConnectionLog::setClientPort(quint16 port) { m_clientPort = port; }
void ConnectionLog::setProtocol(const QString &protocol) { m_protocol = protocol; }
void ConnectionLog::setEventType(const QString &eventType) { m_eventType = eventType; }
void ConnectionLog::setCreatedAt(const QDateTime &createdAt) { m_createdAt = createdAt; }

// -------------------- ProcessedDataEntity 实现 --------------------
ProcessedDataEntity::ProcessedDataEntity(QObject *parent)
    : QObject(parent)
    , m_id(0)
    , m_senderPort(0)
{
}

ProcessedDataEntity::ProcessedDataEntity(const QByteArray &originalData,
                                         const QString &decodedMessage,
                                         const QString &senderIp,
                                         quint16 senderPort,
                                         const QString &protocol,
                                         const QDateTime &createdAt,
                                         QObject *parent)
    : QObject(parent)
    , m_id(0)
    , m_originalData(originalData)
    , m_decodedMessage(decodedMessage)
    , m_senderIp(senderIp)
    , m_senderPort(senderPort)
    , m_protocol(protocol)
    , m_createdAt(createdAt.isValid() ? createdAt : QDateTime::currentDateTime())
{
}

qint64 ProcessedDataEntity::id() const { return m_id; }
QByteArray ProcessedDataEntity::originalData() const { return m_originalData; }
QString ProcessedDataEntity::decodedMessage() const { return m_decodedMessage; }
QString ProcessedDataEntity::senderIp() const { return m_senderIp; }
quint16 ProcessedDataEntity::senderPort() const { return m_senderPort; }
QString ProcessedDataEntity::protocol() const { return m_protocol; }
QDateTime ProcessedDataEntity::createdAt() const { return m_createdAt; }

void ProcessedDataEntity::setId(qint64 id) { m_id = id; }
void ProcessedDataEntity::setOriginalData(const QByteArray &data) { m_originalData = data; }
void ProcessedDataEntity::setDecodedMessage(const QString &message) { m_decodedMessage = message; }
void ProcessedDataEntity::setSenderIp(const QString &ip) { m_senderIp = ip; }
void ProcessedDataEntity::setSenderPort(quint16 port) { m_senderPort = port; }
void ProcessedDataEntity::setProtocol(const QString &protocol) { m_protocol = protocol; }
void ProcessedDataEntity::setCreatedAt(const QDateTime &createdAt) { m_createdAt = createdAt; }
