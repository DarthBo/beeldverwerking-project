#ifndef BLINDTASTIC_CONCURRENCY_H
#define BLINDTASTIC_CONCURRENCY_H
#include <queue>
#include <thread>
#include <future>

template <class T>
class Callable{
    virtual T call();
};

template <class T>
class ExecutorService{
public:
    virtual std::future<T> submit(const Callable<T>&);
};

template <class T>
/* Executes tasks in another thread*/
class SingleThreadExecutorService: public ExecutorService<T>{
protected:
    std::queue<std::packaged_task<T>*> taskQueue;
    std::thread singleThread;
    bool isInterrupted;
    bool isShutdown;
    void start();
    void run();
public:
    /* Start a single thread*/
    SingleThreadExecutorService();
    /* Submit a task to be executed in a different thread*/
    std::future<T> submit(const Callable<T>&);
    /* Finish current task and stop executing*/
    void interrupt();
    /* Finish all currenty submitted tasks and don't allow any new tasks*/
    void shutdown();
};

#endif
