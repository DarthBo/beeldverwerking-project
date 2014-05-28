#ifndef BLINDTASTIC_CONCURRENCY_H
#define BLINDTASTIC_CONCURRENCY_H
#include <queue>
#include <thread>
#include <future>

template <class T>
class Callable{
public:
    virtual T call() = 0;
};

template <class T>
class ExecutorService{
public:
    virtual std::future<T> submit(Callable<T>&) = 0;
};

template <class T>
/* Executes tasks in another thread*/
class SingleThreadExecutorService: public ExecutorService<T>{
protected:
    std::queue<std::packaged_task<T()>*> taskQueue;
    std::thread singleThread;
    bool isInterrupted;
    bool isShutdown;
    void run(){
        while (!this->isInterrupted || (!isShutdown && !taskQueue.empty())) {
            if(!taskQueue.empty()){
                std::packaged_task<T()>* task = taskQueue.front();
                taskQueue.pop();
                std::thread t(std::move(*task));
                t.join(); // we want this thread to be blocking
            }
        }
    }
    void start(){
        singleThread = std::thread(&SingleThreadExecutorService<T>::run,this);
    }

public:
    SingleThreadExecutorService():isInterrupted(false),isShutdown(false){ start(); }
    /* Start a single thread*/
    //SingleThreadExecutorService();
    /* Submit a task to be executed in a different thread*/
    std::future<T> submit(Callable<T>& callable){
        std::packaged_task<T()>* task = new std::packaged_task<T()>([&](){ return callable.call(); });
        taskQueue.push(task);
        return task->get_future();
    }
    /* Finish current task and stop executing*/
    void interrupt(){
        isInterrupted = true;
    }
    /* Finish all currenty submitted tasks and don't allow any new tasks*/
    void shutdown(){
        isShutdown = true;
    }
};

#endif
