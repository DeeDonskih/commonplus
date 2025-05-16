/**
 * @file scopeguard.hpp
 * @author Dmitry Donskikh (deedonskihdev@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2018-03-24
 * 
 * @copyright Copyright (c) 2018-2021 Dmitriy Donskikh
 *            All rights reserved.
 * 
 * 
 */

/// Scope guards
#ifndef UFW_COMMON_UTILS_SCOPE_GUARDS_HPP_19
#define UFW_COMMON_UTILS_SCOPE_GUARDS_HPP_19

#include <utility>
#include <type_traits>
#include <tuple>
#include <functional>

  // A simple scope guard that calls a provided callable on destruction, unless released.
  template<typename F>
  class ScopeGuard
  {
  public:
    explicit ScopeGuard(F&& f) noexcept(std::is_nothrow_move_constructible<F>::value):
        func_(std::move(f)), active_(true)
    {}
    ~ScopeGuard() noexcept
    {
      if (active_) {
        try {
          func_();
        } catch (...) {}
      }
    }
    ScopeGuard(ScopeGuard&& other) noexcept: func_(std::move(other.func_)), active_(other.active_)
    {
      other.release();
    }
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    void release() noexcept
    {
      active_ = false;
    }  // Release prevents the callable from being invoked
  private:
    F func_;
    bool active_;
  };

  template<typename F>
  ScopeGuard<std::decay_t<F>>
  make_scope_guard(F&& f) noexcept(std::is_nothrow_move_constructible<std::decay_t<F>>::value)
  {
    return ScopeGuard<std::decay_t<F>>(std::forward<F>(f));
  }

  template<typename F, typename... Args>
  auto make_scope_guard(F&& f,
                        Args&&... args) noexcept(std::is_nothrow_move_constructible<std::decay_t<F>>::value &&
                                                 (std::is_nothrow_move_constructible<std::decay_t<Args>>::value && ...))
  {
    auto bound = [func = std::forward<F>(f),
                  tup = std::tuple<std::decay_t<Args>...>(std::forward<Args>(args)...)]() mutable {
      std::apply(func, tup);
    };
    return make_scope_guard(std::move(bound));
  }




#endif //UFW_COMMON_UTILS_SCOPE_GUARDS_HPP_19