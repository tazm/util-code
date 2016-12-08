#ifndef SECPLATFORM_SRC_COMMON_TASK_EXECUTER_H_
#define SECPLATFORM_SRC_COMMON_TASK_EXECUTER_H_

#include "autolock.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <queue>
#include <tuple>
#include <string>

using namespace std;

class TaskInterface
{
public:
    TaskInterface() {};
    virtual ~TaskInterface() {};

    virtual void Run() = 0;
};

class TaskExecuter
{
public:
    TaskExecuter();
     ~TaskExecuter();

    int Start();
    void Stop();

    bool AppendTask(TaskInterface* task, const std::string task_name, bool delete_on_finish);

    void WorkRoutine();

private:

    static DWORD WINAPI RunThread(void* param);

    typedef std::tr1::tuple<TaskInterface* /*task*/, 
        std::string /*name*/, bool /*delete_on_finish*/> TaskItem;

    typedef std::queue<TaskItem> TaskQueue;
    TaskQueue task_queue_;
    bool suspended_;

    CriticalSection task_queue_lock_;

    HANDLE task_queue_not_empty_;
    HANDLE worker_thread_;

    bool exit_;
};

/*
Ê¾Àý
class TaskA : public TaskInterface
{
public:
TaskA();
virtual ~TaskA();

virtual void Run();
};

void TaskA::Run()
{
printf("TaskA:Run\n");
}

TaskA::TaskA()
{
printf("TaskA\n");

}

TaskA::~TaskA()
{

printf("~TaskA\n");
}

class TaskB : public TaskInterface
{
public:
TaskB() {};
virtual ~TaskB() {};

virtual void Run();
};

void TaskB::Run()
{
printf("TaskB:Run\n");
}

TaskExecuter task_executer;

task_executer.AppendTask(new TaskA(), "TaskA", true);
task_executer.AppendTask(new TaskB(), "TaskA", true);
task_executer.Start();


TaskA taskA;
task_executer.AppendTask(&taskA, "taskA", false);
task_executer.AppendTask(new TaskB(), "TaskA", true);

system("pause");

task_executer.AppendTask(new TaskA(), "TaskA", true);

system("pause");

task_executer.Stop();

*/
#endif // SECPLATFORM_SRC_COMMON_TASK_EXECUTER_H_
