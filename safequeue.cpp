#include "safequeue.h"

template<typename T>
void SafeQueue<T>::putValue(T value)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cond.wait(m_queue,[this](){
        return m_queue.size()<50;
    });
    m_queue.push(value);
    m_cond.notify_one();
}

template<typename T>
bool SafeQueue<T>::getValue(T &value)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if(m_isStop && m_queue.size()<=0){
        return false;
    }
    m_cond.wait(m_mutex,[this](){
        return m_queue.size()>0||m_isStop;
    });
    value = std::move(m_queue.front());
    m_queue.pop();
    m_cond.notify_one();
    return true;
}
