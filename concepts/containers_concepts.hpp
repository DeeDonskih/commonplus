/**
 * @file containers_concepts.hpp
 * @author Dmitry Donskikh (deedonskihdev@gmail.com)
 * @brief 
 * @version 1.3
 * @date 2018-03-15
 * 
 * @copyright Copyright (c) 2018-2021 Dmitriy Donskikh
 *            All rights reserved.
 */

#pragma once

#include <type_traits>

/**
 * @brief 
 * 
 */
namespace  ufwConcepts{

/**
 * @brief IsPointerT - Check is given type a pointer
 */
template<typename T>
struct IsPointerT : std::false_type {
};
template<typename T>
struct IsPointerT<T*> : std::true_type {
  using BaseT = T;
};

/**
 * @brief 
 * 
 */
#define DEFINE_HAS_MEMBER(Member)                                   \
  template<typename, typename = std::void_t<>>                      \
  struct HasMemberT_##Member                                        \
   : std::false_type { };                                           \
  template<typename T>                                              \
  struct HasMemberT_##Member<T, std::void_t<decltype(&T::Member)>>  \
   : std::true_type { }

DEFINE_HAS_MEMBER(size);

/**
 * @brief HasPointerType - Template class checking is Type has "pointer" type
 */
template<typename,typename = std::void_t<>>
struct HasPointerType
        : std::false_type
{
    using ptr = void;
};
template<typename T>
struct HasPointerType <T,std::void_t<typename T::pointer>>
        :  std::true_type
{
    using ptr = typename T::pointer;
};

/**
 * @brief HasDataPtr
 * 
 */
template<typename,typename = std::void_t<>>
struct HasDataPtr
    : std::false_type { };
template<typename T>
struct HasDataPtr<T,std::void_t<decltype((typename HasPointerType<T>::ptr(T::*)())&T::data)>>
    : std::true_type { };

/**
 * @brief IsStdLinearContainer
 */
template<typename,typename DefaultShit = void>
struct IsStdLinearContainer
    : std::false_type { };
template<typename T>
struct IsStdLinearContainer<T,typename std::enable_if<(HasDataPtr<T>::value)&&(HasMemberT_size<T>::value)>::type>
    : std::true_type { };

}
