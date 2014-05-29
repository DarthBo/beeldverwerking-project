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
    std::mutex resultMutex;
    std::thread singleThread;
    bool isInterrupted;
    bool isShutdown;
    void run(){
        while (true) {
            if(!taskQueue.empty()){
                Callable<T>* task = taskQueue.front();
                taskQueue.pop();
                T t = task->call();
                resultMutex.lock();
                resultQueue.push(std::move(t));
                resultMutex.unlock();
            }
            if(isInterrupted){
                break;
            }
            if(isShutdown && taskQueue.empty()){
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
            taskQueue.push(callable);
        }
    }
    /* Returns true if nextResult is defined.*/
    bool hasNextResult(){
        return !resultQueue.empty();
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
};

#endif
