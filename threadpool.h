//
// Created by 王强 on 25-4-22.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>



//线程池支持的模式
enum class PoolMode
{
    MODE_FIXED, //固定数量的线程
    MODE_CACHED,//线程数量可动态增长

  };

/*
 * example:
 * ThreadPool pool;
 * pool.start(4);
 *
 * class MyTask: public Task
 * {
 *      public:
 *          void run(){//线程代码}
 *
 * }
 * pool.submitTask(std::make_shared<MyTask>());
 */


//any类型：表示可以接受任意数据的类型
class Any {
public:

    Any()=default;
    ~Any()=default;

    Any(const Any&)=delete;
    Any& operator=(const Any&)=delete;
    Any(Any&&)=default;
    Any& operator=(Any&&)=default;

    //这个构造函数可以让Any类型接收任意其他的数据
    template<typename T>
    Any(T data):base_(std::make_unique<Derive<T>>(data)){}

    //这个方法能把Any对象里面存储的data数据提取出来
    template<typename T>
    T cast_() {
        //我们怎么从base_找到它所指的Derive对象，从它里面提取出data成员变量
        //基类指针 =》派生类指针  RTTI
        Derive<T>* pd=dynamic_cast<Derive<T>*>(base_.get());
        if(pd==nullptr) {
            throw "type is unmach!";
        }
        return pd->data_;
    }
private:
    //基类类型
    class Base {
        public:
            virtual ~Base()=default;
    };

    //派生类类型
    template<class T>
    class Derive: public Base {
        public:
            Derive(T data):data_(data)
            {}
        private:
            T data_;    //保存了任意其他类型
    };
    //定义一个基类的指针
    std::unique_ptr<Base> base_;
};


//任务抽象基类
class Task{
public:
    //用户可以自定义任意任务类型，从task继承，重写run方法
    virtual Any run() = 0;
};

//线程类型
class Thread{
public:
    //线程函数对象类型
    using ThreadFunc = std::function<void()>;
    //线程构造
    Thread(ThreadFunc func);
    //线程析构
    ~Thread();
    //启动线程
    void start();
private:
    ThreadFunc func_;
};

//线程池类型
class ThreadPool {
public:
    //线程池构造函数
    ThreadPool();
    //线程池析构函数
    ~ThreadPool();
    //设置线程池工作模式
    void setMode(PoolMode mode);

    //设置初始的线程数量
    void setInitThreadSize(int size);

    //设置task任务队列上限阈值
    void setTaskQueMaxThreadHold(int threadHold);

    //给线程池提交任务
    void submitTask(std::shared_ptr<Task> task);

    //开启线程池
    void start(int initThreadSize=4);

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

private:
    //定义线程函数
    void threadFunc();

private:
    std::vector<std::unique_ptr<Thread>> threads_; //线程列表
    size_t initThreadSize_; //初始线程数量

    std::queue<std::shared_ptr<Task> > taskQue_; //任务队列
    std::atomic_int taskSize_;	//任务数量
    int taskQueMaxThreadHold_;	//任务队列上限阈值

    std::mutex taskQueMutex_;	//保证任务队列的线程安全
    std::condition_variable notFull_;	//表示任务队列不满
    std::condition_variable notEmpty_;	//表示任务队列不空

    PoolMode poolMode_;	//当前线程池工作模式
};


#endif //THREADPOOL_H
