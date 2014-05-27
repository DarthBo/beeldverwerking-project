#include "blindtastic_concurrency.h"

/* SingleThreadExecutorService*/

template <class T>
void SingleThreadExecutorService<T>::start(){
    singleThread(run);
}

template <class T>
void SingleThreadExecutorService<T>::run(){
    while (!this->isInterrupted || (!isShutdown && !taskQueue.empty())) {
        if(!taskQueue.empty()){
            std::packaged_task<T>* task = taskQueue.front();
            taskQueue.pop();
            std::thread(std::move(*task)).detach();
        }
    }
}

template <class T>
std::future<T> SingleThreadExecutorService<T>::submit(const Callable<T>& callable){
    std::packaged_task<T> task = new std::packaged_task<T>([callable](){ return callable.call(); });
    taskQueue.push(task);
    return task.get_future();
}

template <class T>
SingleThreadExecutorService<T>::SingleThreadExecutorService():isInterrupted(false),isShutdown(false){ start(); }


template <class T>
void SingleThreadExecutorService<T>::interrupt(){
    isInterrupted = true;
}

template <class T>
void SingleThreadExecutorService<T>::shutdown(){
    isShutdown = true;
}


