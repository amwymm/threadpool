//
// Created by 王强 on 25-4-1.
//

#include "threadpool.h"
#include <functional>
#include <thread>
#include <iostream>

const int TASK_MAX_SIZE = 4;

//线程池构造
ThreadPool::ThreadPool()
    : initThreadSize_(0)
    , taskSize_(0)
    , taskQueMaxThreadHold_(TASK_MAX_SIZE)
    , poolMode_(PoolMode::MODE_FIXED)
{}

//线程池析构
ThreadPool::~ThreadPool()
{}

//设置线程池工作模式
void ThreadPool::setMode(PoolMode mode)
{
    poolMode_=mode;
}

//设置初始的线程数量
void ThreadPool::setInitThreadSize(int size)
{
    initThreadSize_=size;
}

//设置task任务队列上限阈值
void ThreadPool::setTaskQueMaxThreadHold(int threadHold)
{
    taskQueMaxThreadHold_=threadHold;
}

//给线程池提交任务 用户调用该接口，传入任务对象，生产任务
void ThreadPool::submitTask(std::shared_ptr<Task> task) {
    //获取锁
    std::unique_lock<std::mutex> lock(taskQueMutex_);
    //线程通信 等待任务队列有空余
    //while (taskQue_.size() == taskQueMaxThreadHold_) {
    //     notFull_.wait(lock);
    //}
    //用户提交任务，最长不能阻塞超过1s，否则判断任务提交失败，返回
    if (!notFull_.wait_for(lock,std::chrono::seconds(1),
        [&]()->bool{return taskQue_.size()<taskQueMaxThreadHold_;})) {
        //表示notFull_等待1s，条件依然没有满足
        std::cerr<<"task queue is full, submit task fail."<<std::endl;
        return;
    }

    //如果有空余，把任务放入任务队列
    taskQue_.emplace(task);
    taskSize_++;

    //因为新放了任务，任务队列肯定不空，在notempty——上进行通知,分配线程执行任务
    notEmpty_.notify_all();

}

//开启线程池
void ThreadPool::start(int initThreadSize)
{
    //记录初始线程个数
    initThreadSize_=initThreadSize;

    //创建线程对象
    for(int i=0;i<initThreadSize_;i++)
    {
        //创建thread线程对象的时候，把线程函数给到thread线程对象
        std::unique_ptr<Thread> ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this));
        threads_.emplace_back(std::move(ptr));
    }

    //启动所有线程  std::vector<Thread*> threads_
    for(int i=0;i<initThreadSize_;i++)
    {
        threads_[i]->start();
    }

}

//定义线程函数  线程池的所有线程从任务队列里消费任务
void ThreadPool::threadFunc() {
    // std::cout<<"begin threadFunc tid:"<<std::this_thread::get_id()<<std::endl;
    // std::cout<<"end threadFunc tid:"<<std::this_thread::get_id()<<std::endl;
    for(;;) {
        std::shared_ptr<Task> task;
        {
            //先获取锁
            std::unique_lock<std::mutex> lock(taskQueMutex_);

            std::cout<<"tid:"<<std::this_thread::get_id()<<"尝试获取任务..."<<std::endl;
            //等待notempty条件
            notEmpty_.wait(lock,[&]()->bool{return taskQue_.size()>0;});

            std::cout<<"tid:"<<std::this_thread::get_id()<<"任务获取成功..."<<std::endl;

            //从任务队列中取一个任务出来
            task =taskQue_.front();
            taskQue_.pop();
            taskSize_--;

            //如果依然有剩余任务，继续通知其他线程执行任务
            if (taskQue_.size()>0) {
                notEmpty_.notify_all();
            }
            //取出一个任务，进行通知，通知可以继续提交生产任务
            notFull_.notify_all();
        }//出了右括号就把锁释放掉了，析构函数自动释放
        //当前线程负责执行这个任务
        if (task != nullptr)
        {
            task->run();
        }
    }
}

//线程方法实现
//线程构造
Thread::Thread(ThreadFunc func)
    :func_(func)
{}
//线程析构
Thread::~Thread()
{}

//启动线程
void Thread::start()
{
    //创建一个线程来执行一个线程函数
    std::thread t(func_);    //C++11来说 线程对象t 和线程函数func_
    t.detach();    //设置分离线程  pthread_detach pthread_t设置成分离线程
}

