//
// Created by 王强 on 25-4-22.
//
#include<iostream>
#include <thread>

#include"threadpool.h"

int main(){
    ThreadPool pool;
    pool.start();

    std::this_thread::sleep_for(std::chrono::seconds(5));
}