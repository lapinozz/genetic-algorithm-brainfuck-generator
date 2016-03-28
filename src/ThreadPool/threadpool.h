#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <iostream>
#include <functional>
#include <stdexcept>
#include <cassert>
#include <chrono>

using namespace std::literals;

class ThreadPool {
public:
    ThreadPool(size_t);
    void enqueue(std::function<void(void)> func)
    {
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace_back(func);
        }

        condition.notify_one();
    }

    void wait()
    {
        while(true)
        {
            if(!workingCount && !tasks.size())
                break;

                 std::this_thread::sleep_for(1ms);
//            assert(workingCount || tasks.size());
////            std::cout << workingCount << " : " << tasks.size() << std::endl;
        };
    }

    ~ThreadPool();
private:
    std::atomic<int> workingCount;

    std::vector<std::thread> workers;
    std::vector<std::function<void(void)>> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
};

#endif
