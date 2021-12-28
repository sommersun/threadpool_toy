#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <thread>
#include<future>
#include <vector>
#include <queue>
class ThreadPool{
  public:
    ThreadPool(unsigned short threadCount = 0) {
        if (threadCount == 0) {
            threadCount = std::thread::hardware_concurrency();// #computer cpu 
        }
        addThread(threadCount);
    };
    ~ThreadPool() {
        _terminate = true;
        _jobsAvailable.notify_all();
        for (auto &t:_threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    };
    //not copyable
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator = (const ThreadPool&) = delete;
    //but movable
    ThreadPool(ThreadPool&&) = default;
    ThreadPool& operator = (ThreadPool&&) = default;
    template<typename Func, typename... Args>
    auto commit(Func&& func, Args&& ... args)->std::future<typename std::result_of<Func(Args...)>::type>
    {
        using PackedTask = std::packaged_task<typename std::result_of<Func(Args...)>::type()>;
        auto task = std::make_shared<PackedTask>(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
        auto result = task->get_future();
        {
            std::lock_guard<std::mutex> lock{ jobsMutex };
            _jobs.emplace([task]() {
                (*task)();
                });
        }
        _jobsAvailable.notify_one();
        return result;
    }
    private:
        std::vector<std::thread> _threads;
        std::condition_variable _jobsAvailable;
        std::atomic<bool> _terminate;
        mutable std::mutex _jobsMutex;
        using Job = std::function<void()>;
        std::queue<Job> _jobs;
        std::atomic<unsigned short> _threadsWaiting;
        void addThread(unsigned short threadCount) {
        }
};
#endif
