
#include "task_executer.h"

TaskExecuter::TaskExecuter()
:  exit_(false)
{
}

TaskExecuter::~TaskExecuter()
{
    Stop();
}

bool TaskExecuter::AppendTask(TaskInterface* task, const std::string task_name, bool delete_on_finish)
{
    if (NULL == task || exit_)
        return false;

    autolock<CriticalSection> lock(task_queue_lock_);
    task_queue_.push(TaskQueue::value_type(task, task_name, delete_on_finish));

    SetEvent(task_queue_not_empty_);

    return true;
}

void TaskExecuter::WorkRoutine()
{
    while (!exit_)
    {
        task_queue_lock_.lock();
        while(task_queue_.empty())
        {
            task_queue_lock_.unlock();
            DWORD ret = WaitForSingleObject(task_queue_not_empty_, INFINITE);
            task_queue_lock_.lock();

            if (exit_)
            {
                break;  
            }
        }

        if (exit_)
        {
            task_queue_lock_.unlock();
            break;  
        }

        TaskItem task_item = task_queue_.front();
/*        std::string current_task_name = std::tr1::get<1>(task_item);*/
        task_queue_.pop();
        task_queue_lock_.unlock();

        std::tr1::get<0>(task_item)->Run();

		if (std::tr1::get<2>(task_item))
            delete std::tr1::get<0>(task_item);
    }
}

DWORD WINAPI TaskExecuter::RunThread( void* param )
{
    TaskExecuter* pThread = (TaskExecuter* )param;

    pThread->WorkRoutine();

    return 0;
}

int TaskExecuter::Start()
{
    task_queue_not_empty_ = CreateEvent(NULL, FALSE, FALSE, NULL);
    worker_thread_ = CreateThread(NULL, 
        0, 
        RunThread, 
        (LPVOID)this, 
        0, 
        NULL
        );

    return 0;
}

void TaskExecuter::Stop()
{
    exit_ = true;

    if (task_queue_not_empty_ != NULL)
    {
        SetEvent(task_queue_not_empty_);
        CloseHandle(task_queue_not_empty_);
        task_queue_not_empty_ = NULL;
    }

    if (worker_thread_ != NULL)
    {
        WaitForSingleObject(worker_thread_, INFINITE);
        CloseHandle(worker_thread_);
         worker_thread_ = NULL;
    }

    if (!task_queue_.empty())
    {
        autolock<CriticalSection> lock(task_queue_lock_);

        while (!task_queue_.empty())
        {
            TaskItem task_item = task_queue_.front();
            task_queue_.pop();

            if (std::tr1::get<2>(task_item))
                delete std::tr1::get<0>(task_item);
        }
    }
}
