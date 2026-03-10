#ifndef COMMON_H
#define COMMON_H
#include <QObject>
#include <QByteArray>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QThread>
#include <QDebug>
#include <QHostAddress>
#include <../Common/commstruct.h>

/**
 * @brief 线程安全队列，支持最大容量限制和多生产者多消费者。
 * @tparam T 元素类型（必须支持拷贝构造和赋值）
 */
template <typename T>
class ThreadSafeQueue
{
public:
    /**
     * @brief 构造函数
     * @param maxSize 最大容量，0表示无限制
     */
    explicit ThreadSafeQueue(int maxSize = QUEUE_MAX_SIZE) : m_maxSize(maxSize) {}

    /**
     * @brief 放入数据
     * @param data 要放入的数据
     * @param block 是否阻塞等待（true 表示如果队列满则阻塞，false 表示立即返回）
     * @return true 成功放入，false 队列满且 block=false 时返回 false
     */
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

    /**
     * @brief 非阻塞尝试弹出数据
     * @param value 用于存储弹出的数据
     * @return true 成功弹出，false 队列为空
     */
    bool tryPop(T& value) {
        QMutexLocker locker(&m_mutex);
        if (m_queue.isEmpty()) {
            return false;
        }
        value = m_queue.dequeue();
        m_notFull.wakeOne();
        return true;
    }
    /**
     * @brief 阻塞弹出数据（队列空则等待）
     * @return 弹出的数据
     */
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

    /**
     * @brief 清空队列
     */
    void clear() {
        QMutexLocker locker(&m_mutex);
        m_queue.clear();
        m_notFull.wakeAll();
    }

    /**
     * @brief 获取当前队列大小
     */
    int size() {
        QMutexLocker locker(&m_mutex);
        return m_queue.size();
    }

    /**
     * @brief 获取最大容量
     */
    int maxSize() const { return m_maxSize; }

private:
    bool isFull() const {
        return m_maxSize > 0 && m_queue.size() >= m_maxSize;
    }
private:
    QQueue<T> m_queue;//数据池
    QMutex m_mutex;//互斥锁
    QWaitCondition m_notFull;//当前状态，未满时
    QWaitCondition m_notEmpty;//当前状态，未空时
    int m_maxSize;//定义最大的数据池大小
};


#endif // COMMON_H
