#include "threadpool.h"

ThreadPool::ThreadPool(size_t threads) :
    stop(false),
    workingCount(0)
{
    for(size_t x = 0; x < threads; x++)
    {
        workers.emplace_back([this]()
        {
            for(;;)
            {
                std::unique_lock<std::mutex> lock(queue_mutex);

                while(!stop && tasks.empty())
                    condition.wait(lock);

                if(stop && tasks.empty())
                    return;

                workingCount++;

                std::function<void()> task(tasks.back());
                tasks.pop_back();

                lock.unlock();

                task();

                workingCount--;
            }
        });
    }
}

// the destructor joins all threads
ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }

    condition.notify_all();

    for(size_t x = 0; x < workers.size(); x++)
        workers[x].join();
}
