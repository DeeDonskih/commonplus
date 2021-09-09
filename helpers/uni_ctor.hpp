/**
 * @file uni_ctor.hpp
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

///Unified constructor/destructor (libc++) new_allocator.h

#if __cplusplus >= 201103L
#include<functional>
namespace  {

	/**
	 * @brief Unified constructor(libc++) new_allocator.h
	 * perfect forwarded unified in-place construct
	 * @tparam _Up type of constructed object
	 * @tparam _Args list of argument types
	 * @param __p pointer to allocated memory
	 * @param __args arguments to constructor
	 */
    template<typename _Up, typename... _Args>
	void
	construct(_Up* __p, _Args&&... __args)
	noexcept(std::is_nothrow_constructible<_Up, _Args...>::value)
	{ ::new((void *)__p) _Up(std::forward<_Args>(__args)...); }
	/**
	 * @brief Unified desstructor(libc++) new_allocator.h
	 * destruct and clean heap at __p
	 * @tparam _Up 
	 * @param __p 
	 * NOTE Try to use std::launder
	 * Todo Add heap placement check
	 */
    template<typename _Up>
	void
	destroy(_Up* __p)
	noexcept(std::is_nothrow_destructible<_Up>::value)
	{ __p->~_Up(); }



#endif
