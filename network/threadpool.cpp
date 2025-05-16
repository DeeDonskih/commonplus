/**
 * @file threadpool.cpp
 * @author Dmitrii Donskikh (deedonskihdev@gmail.com)
 * @brief Implementation of the ThreadPool class for parallel task execution.
 *
 * This file contains the implementation of the ThreadPool class methods,
 * including thread management, task scheduling, queue handling, and
 * synchronization mechanisms.
 *
 * The ThreadPool is designed to efficiently execute asynchronous tasks using
 * a fixed (or changeable) number of threads, manage a task queue, and provide
 * facilities for waiting, limiting, or cleaning the task queue as needed.
 * 
 * @version 0.1
 * @date 2019-01-10
 * 
 * @copyright Copyright (c) 2019-2024 Dmitrii Donskikh
 *            Licensed under the MIT License (https://opensource.org/licenses/MIT)
 */
#include "threadpool.hpp"

namespace utils
{
  ThreadPool::ThreadPool(size_t threads)
  {
    add_threads(threads);
  }

  void ThreadPool::add_threads(size_t count)
  {
      for (size_t i = 0; i < count; ++i) {
        workers.emplace_back([this] {
          for (;;) {
            std::function<void()> task;

            {
              std::unique_lock<std::mutex> lock(this->queue_mutex);
              this->condition.wait(lock,
                                   [this] { return this->stop || this->stop_workers > 0 || !this->tasks.empty(); });

              if (this->stop) return;

                if (this->stop_workers > 0) {
                  --this->stop_workers;
                  return;
              }

              if (this->tasks.empty()) continue;

              task = std::move(this->tasks.front());
              this->tasks.pop();
            }
            if (this->tasks.empty()) this->condition.notify_one();
            if (task) task();
          }
        });
      }
  }

  void ThreadPool::remove_threads(size_t count)
  {
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      if (count > workers.size()) count = workers.size();
      stop_workers += count;
    }

    for (size_t i = 0; i < count; ++i) condition.notify_one();

      // Удалить завершившиеся потоки из списка
      for (auto it = workers.begin(); it != workers.end();) {
          if (it->joinable()) {
            it->join();
            it = workers.erase(it);
          } else {
            ++it;
          }
      }
  }

  void ThreadPool::set_queue_limit(size_t limit)
  {
    m_queue_limit = limit;
  }

  size_t ThreadPool::get_thread_count() const noexcept
  {
    return workers.size();
  }

  size_t ThreadPool::get_queue_size() noexcept
  {
    std::unique_lock getsize_lock(queue_mutex);
    return tasks.size();
  }

  void ThreadPool::wait()
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    condition.wait(lock, [this]() { return tasks.empty(); });
  }

  void ThreadPool::clean_queue()
  {
    std::unique_lock clean_lock(queue_mutex);
    std::queue<std::function<void()>> empty;
    tasks.swap(empty);
  }

  // Деструктор
  ThreadPool::~ThreadPool()
  {
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      stop = true;
    }
    condition.notify_all();
    for (std::thread& worker: workers)
      if (worker.joinable()) worker.join();
  }
}  // namespace utils