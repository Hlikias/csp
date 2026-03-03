#ifndef COMMON_H
#define COMMON_H
#include <QObject>
#include <QByteArray>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QThread>
#include <QDebug>

// 常量定义
const quint16 SERVER_PORT = 8888;          // 服务端端口
const QString SERVER_IP = "127.0.0.1";     // 服务端IP
const int QUEUE_MAX_SIZE = 10000;          // 缓冲区最大容量
const int HEARTBEAT_INTERVAL = 5000;       // 心跳间隔（ms）
const int RECONNECT_INTERVAL = 3000;       // 重连间隔（ms）

// 数据结构体（用于C/S通信和线程间传递）
//定义帧格式，报文格式
struct NetData {
    qintptr socketDescriptor = 0;          // 客户端Socket描述符（服务端用）
    QByteArray data;                       // 通信数据
    QString clientIp;                      // 客户端IP（服务端用）
    quint16 clientPort = 0;                // 客户端端口（服务端用）
};

// 通用线程安全队列（多生产者多消费者）
// 模板类、T任意的数据类型，但是我们这里全部转为byte
template <typename T>
class ThreadSafeQueue
{
public:
    //这里有初始话最大的缓冲区
    explicit ThreadSafeQueue(int maxSize = QUEUE_MAX_SIZE) : m_maxSize(maxSize) {}

    // 放入数据（阻塞/非阻塞）
    bool push(const T& data, bool block = true) {
        //加锁
        QMutexLocker locker(&m_mutex);
        //判断当前的长度是否大于数据池的最大数据，是否block
        if (!block && m_queue.size() >= m_maxSize) {
            return false;
        }
        //如果当前池满了，我们等待锁
        while (m_queue.size() >= m_maxSize) {
            m_notFull.wait(&m_mutex);
        }
        //压入数据
        m_queue.enqueue(data);
        //当前未满唤醒
        m_notEmpty.wakeOne();
        return true;
    }

    // 取出数据（阻塞）
    T pop() {
        //加锁
        QMutexLocker locker(&m_mutex);
        //判断当前的池内是否有数据
        while (m_queue.isEmpty()) {
            m_notEmpty.wait(&m_mutex);
        }
        //获取数据
        T data = m_queue.dequeue();
        //数据池未满唤醒
        m_notFull.wakeOne();
        return data;
    }

    // 清空队列
    void clear() {
        QMutexLocker locker(&m_mutex);
        m_queue.clear();
    }

    // 获取队列大小
    int size() {
        QMutexLocker locker(&m_mutex);
        return m_queue.size();
    }

private:
    QQueue<T> m_queue;//数据池
    QMutex m_mutex;//互斥锁
    QWaitCondition m_notFull;//当前状态，未满时
    QWaitCondition m_notEmpty;//当前状态，未空时
    int m_maxSize;//定义最大的数据池大小
};


#endif // COMMON_H
