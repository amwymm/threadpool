// //
// // Created by 王强 on 25-4-22.
// //
#include<iostream>
#include <thread>
#include<chrono>

#include"threadpool.h"
//
// /*
//  * 有些场景，是希望能够获取线程执行任务的返回值
//  * 举例：
//  * 1+.....+30000的和
//  * thread1  1+...+10000
//  * thread2  10001+...+20000
//  * .....
//  *
//  * main thread: 给每一个线程分配计算的区间，并等待他们算完返回的结果，合并最终的结果即可
//  */
class MyTask: public Task {
public:
    MyTask(int begin, int end)
    :begin_(begin)
    ,end_(end){};

    ~MyTask(){};
    //问题一：怎么设计run函数的返回值，可以接收任意值
    Any run() {//run方法最终就在线程池分配的线程去执行
        std::cout<<"tid:"<<std::this_thread::get_id()<<"begin!"<<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        unsigned long long sum = 0;
        for (unsigned long long i = begin_; i <= end_; ++i) {
            sum += i;
        }
        std::cout<<"tid:"<<std::this_thread::get_id()<<"end!"<<std::endl;
        return sum;
    }

private:
    int begin_;
    int end_;
};

int main(){
    //问题：ThreadPool对象析构以后，怎么样把线程池相关的线程资源全部回收
    {
        ThreadPool pool;
        //用户自己设置线程池的模式
        pool.setMode(PoolMode::MODE_CACHED);
        //开始启动线程池
        pool.start(4);

        //如何设计这里的Result机制呢？
        Result res1 = pool.submitTask(std::make_shared<MyTask>(1,100000000));
        Result res2 = pool.submitTask(std::make_shared<MyTask>(100000001,200000000));
        Result res3 = pool.submitTask(std::make_shared<MyTask>(200000001,300000000));
        pool.submitTask(std::make_shared<MyTask>(1,100000000));

        pool.submitTask(std::make_shared<MyTask>(1,100000000));
        pool.submitTask(std::make_shared<MyTask>(1,100000000));

        //int sum=res.get().cast_<long>();  //返回any类型，怎么转成具体类型呢？
        auto sum1 = res1.get().cast_<unsigned long long>();
        auto sum2 = res2.get().cast_<unsigned long long>();
        auto sum3 = res3.get().cast_<unsigned long long>();

        //Master - Slave线程模型
        //Master线程用来分解任务，然后给各个Slave线程分配任务
        //等待各个Slave线程执行完任务，返回结果
        //Master线程合并各个任务结果，输出
        //
        std::cout<<"sum1+sum2+sum3:"<<sum1+sum2+sum3<<std::endl;

        // unsigned long long sum = 0;
        // for (unsigned long long i = 1; i <= 300000000; ++i) {
        //     sum += i;
        // }
        // std::cout<<"sum:"<<sum<<std::endl;
        // pool.submitTask(std::make_shared<MyTask>());
        // pool.submitTask(std::make_shared<MyTask>());
        // pool.submitTask(std::make_shared<MyTask>());
        // pool.submitTask(std::make_shared<MyTask>());
        // pool.submitTask(std::make_shared<MyTask>());
        // pool.submitTask(std::make_shared<MyTask>());
        // pool.submitTask(std::make_shared<MyTask>());
        // pool.submitTask(std::make_shared<MyTask>());
        //
        //std::this_thread::sleep_for(std::chrono::seconds(5));
        getchar();
    }

    getchar();
    return 0;
}

