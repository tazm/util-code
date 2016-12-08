#ifndef _PCQUEUE_HPP_
#define _PCQUEUE_HPP_

#include <queue>
#include <vector>

#include "autolock.h"

using namespace std;

template <class Task>
class PCQueue
{
public:
    PCQueue()
    {
        task_queue_not_empty_ = CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    ~PCQueue()
    {
        if (task_queue_not_empty_ != NULL)
        {
            CloseHandle(task_queue_not_empty_);
            task_queue_not_empty_ = NULL;
        }
    }

    void Pop(Task &t)
    {
        task_queue_lock_.lock();
        while (task_queue_.empty())
        {
            task_queue_lock_.unlock();
            DWORD ret = WaitForSingleObject(task_queue_not_empty_, INFINITE);
            task_queue_lock_.lock();
        }

        t = task_queue_.front();
        task_queue_.pop();
        task_queue_lock_.unlock();
    }

    void Push(const Task &t)
    {
        autolock<CriticalSection> lock(task_queue_lock_);
        task_queue_.push(t);
        SetEvent(task_queue_not_empty_);
    }

    void Push(const vector<Task> &vec)
    {
        autolock<CriticalSection> lock(task_queue_lock_);
        for (size_t i = 0; i < vec.size(); i++)
        {
            task_queue_.push(vec.at(i));
        }
        SetEvent(task_queue_not_empty_);
    }

    int size() const
    {
        return task_queue_.size();
    }

    std::queue<Task> task_queue_;

    CriticalSection task_queue_lock_;
    HANDLE task_queue_not_empty_;
};

/*
PCQueue<string> pcQueue;

DWORD WINAPI RunThread(void* param)
{
    while(true)
    {
        string str;
        pcQueue.Pop(str);
        printf("pcQueue %s\n", str.c_str());
    }
}

TEST(PcQueu, Start) 
{
    CreateThread(
        NULL, 
        0, 
        RunThread, 
        NULL, 
        0, 
        NULL
        );

    int i = 0;
    while(true)
    {
        getchar();
        i++;
        string str = google::StringPrintf("task %d", i);
        pcQueue.Push(str);
    }
}
*/

#endif  // PCQUEUE_HPP_


