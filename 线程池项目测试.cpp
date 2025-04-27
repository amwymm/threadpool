//
// Created by 王强 on 25-4-22.
//
#include<iostream>
#include <thread>

#include"threadpool.h"

/*
 * 有些场景，是希望能够获取线程执行任务的返回值
 * 举例：
 * 1+.....+30000的和
 * thread1  1+...+10000
 * thread2  10001+...+20000
 * .....
 *
 * main thread: 给每一个线程分配计算的区间，并等待他们算完返回的结果，合并最终的结果即可
 */
class MyTask: public Task {
public:
    MyTask(int begin, int end)
    :begin_(begin)
    ,end_(end){};

    ~MyTask(){};
    //问题一：怎么设计run函数的返回值，可以接收任意值
    Any run() {
        std::cout<<"tid:"<<std::this_thread::get_id()<<"begin!"<<std::endl;
        //std::this_thread::sleep_for(std::chrono::seconds(6));
        int sum = 0;
        for (int i = begin_; i < end_; ++i) {
            sum += i;
        }
        return sum;
        std::cout<<"tid:"<<std::this_thread::get_id()<<"end!"<<std::endl;
    }

private:
    int begin_;
    int end_;
};

int main(){
    ThreadPool pool;
    pool.start(2);

    //如何设计这里的Result机制呢？
    Result res = pool.submitTask(std::make_shared<MyTask>());

    int sum=res.get().cast_<int>();  //返回any类型，怎么转成具体类型呢？

    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());
    //
    //std::this_thread::sleep_for(std::chrono::seconds(5));
    getchar();

}