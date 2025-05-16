/**
 * @file threadpool.hpp
 * @author Dmitrii Donskikh (deedonskihdev@gmail.com)
 * @brief Thread pool implementation for parallel task execution
 * @brief Provides thread management and task scheduling functionality
 * @version 0.1
 * @date 2019-01-10
 *
 * @copyright Copyright (c) 2019-2024 Dmitrii Donskikh
 *            Licensed under MIT License (https://opensource.org/licenses/MIT)
 */
#ifndef UFW_THREADPOOL_UTILS_HPP
#define UFW_THREADPOOL_UTILS_HPP

#include <condition_variable>
#include <functional>
#include <future>
#include <limits>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <vector>

namespace utils
{

  class ThreadPool
  {
  public:
    explicit ThreadPool(size_t);
    ~ThreadPool();

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::optional<std::future<typename std::invoke_result<F, Args...>::type>>
    {
      using return_type = typename std::invoke_result<F, Args...>::type;

      auto task = std::make_shared<std::packaged_task<return_type()>>(
              std::bind(std::forward<F>(f), std::forward<Args>(args)...));
      std::future<return_type> res = task->get_future();
      {
        std::unique_lock<std::mutex> lock(queue_mutex);

        if ((stop) || (tasks.size() >= m_queue_limit)) return std::nullopt;

        tasks.emplace([task]() { (*task)(); });
      }
      condition.notify_one();
      return res;
    }

    void add_threads(size_t count);
    void remove_threads(size_t count);

    void set_queue_limit(size_t limit);

    size_t get_thread_count() const noexcept;
    size_t get_queue_size() noexcept;
    void wait();
    void clean_queue();

  private:
    // Worker threads
    std::vector<std::thread> workers;

    // Task queue
    std::queue<std::function<void()>> tasks;

    // Sync primitives
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop = false;
    size_t stop_workers = 0;  // Workers to kill

    size_t m_queue_limit = std::numeric_limits<size_t>::max();
  };

}  // namespace utils

#endif  // THREADPOOL_H
