#ifndef BLINDTASTIC_CONCURRENCY_H
#define BLINDTASTIC_CONCURRENCY_H
#include <queue>
#include <thread>
#include <future>
#include "blindtastic_core.h"

template<typename T>
bool is_ready(std::future<T>& f){
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

template <class T>
class Callable{
public:
    virtual ~Callable() {}
    virtual T call() = 0;
};

class SVMCallable: public Callable<CharacteristicValue>{
private:
    const CharacteristicDefinition* characteristicDefinition;
    const cv::Mat& image;
    bool skipDatacalculation;
public:
    SVMCallable(const CharacteristicDefinition* _charDef, const cv::Mat& _img, bool _skipDatacalc)
        : characteristicDefinition(_charDef),image(_img),skipDatacalculation(_skipDatacalc){}
    virtual ~SVMCallable() {}
    CharacteristicValue call(){
        return characteristicDefinition->getValue(image,skipDatacalculation);
    }
};

/* Bundles multiple Callables in one Callable and returns their output as an std::vector*/
template <class T>
class BundledCallable: public Callable<std::vector<T>>{
private:
    std::vector<Callable<T>*> callables;
public:
    BundledCallable(){}
    BundledCallable(std::vector<Callable<T>*>& _callables):callables(_callables){}
    virtual ~BundledCallable() { clear(); }

    void addCallable(Callable<T>* callable)
    {
        callables.push_back(callable);
    }

    void clear()
    {
        for (Callable<T>* callable : callables)
            delete callable;
        callables.clear();
    }

    std::vector<T> call()
    {
        std::vector<T> values;
        for(Callable<T>* callable : callables){
            values.push_back(callable->call());
        }
        return values;
    }
};

template <class T>
class ExecutorService{
public:
    virtual void submit(Callable<T>*) = 0;
};

template <class T>
/* Executes tasks in another thread. Expects T to be moveable.*/
class SingleThreadExecutorService: public ExecutorService<T>{
protected:
    std::queue<Callable<T>*> taskQueue;
    std::queue<T> resultQueue;
    std::mutex taskMutex;
    std::mutex resultMutex;
    std::thread singleThread;
    bool isInterrupted;
    bool isShutdown;
    void run(){
        while (true) {
            if(!isIdle()){
                taskMutex.lock();
                Callable<T>* task = taskQueue.front();
                taskMutex.unlock();

                T t = task->call();

                taskMutex.lock();
                resultMutex.lock();

                taskQueue.pop(); //only pop when task is done
                resultQueue.push(std::move(t));

                taskMutex.unlock();
                resultMutex.unlock();
            }
            else if (isShutdown){
                break;
            }
            if(isInterrupted){
                break;
            }
        }
    }
    void start(){
        singleThread = std::thread(&SingleThreadExecutorService<T>::run,this);
    }

public:
    /* Start a single thread.*/
    SingleThreadExecutorService():isInterrupted(false),isShutdown(false){ start(); }
    /* Submit a task to be executed in a different thread. Will be ignored if Executor is shutdown.*/
    void submit(Callable<T>* callable){
        if(!isShutdown){
            taskMutex.lock();
            taskQueue.push(callable);
            taskMutex.unlock();
        }
    }
    /* Returns true if nextResult is defined.*/
    bool hasNextResult(){
        resultMutex.lock();
        bool hasNext = !resultQueue.empty();
        resultMutex.unlock();
        return hasNext;
    }
    /* Returns next finished T.*/
    T nextResult(){
        resultMutex.lock();
        T result = resultQueue.front();
        resultQueue.pop();
        resultMutex.unlock();
        return result;
    }

    /* Finish current task and stop executing. This action blocks.*/
    void interrupt(){
        isInterrupted = true;
        singleThread.join();
    }

    /* Finish all currenty submitted tasks and don't allow any new tasks. This action blocks.*/
    void shutdown(){
        isShutdown = true;
        singleThread.join();
    }

    /* True if there are no tasks to run / running */
    bool isIdle()
    {
        taskMutex.lock();
        bool idle = taskQueue.empty();
        taskMutex.unlock();

        return idle;
    }
};

#endif
