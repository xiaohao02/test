#ifndef SAFEQUEUE_H
#define SAFEQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class SafeQueue
{
public:
    SafeQueue() = default;
    void putValue(T value)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock,[this](){
            return m_queue.size()<50;
        });
        m_queue.push(value);
        m_cond.notify_one();
    }

    bool getValue(T &value)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if(m_isStop && m_queue.size()<=0){
            return false;
        }
        m_cond.wait(lock,[this](){
            return m_queue.size()>0||m_isStop;
        });
        value = std::move(m_queue.front());
        m_queue.pop();
        m_cond.notify_one();
        return true;
    }

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool m_isStop = false;
};

#endif // SAFEQUEUE_H
