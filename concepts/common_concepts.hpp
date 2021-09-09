/**
 * @file common_concepts.hpp
 * @author Dmitry Donskikh (deedonskihdev@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-03-27
 * 
 * @copyright Copyright (c) 2018-2021 Dmitriy Donskikh
 *            All rights reserved.
 * 
 * 
 */

#pragma once

#define IsUfwConstructable() 
#define IsUfwDefaultConstructable(expr) template<typename = enable_if_t<std::is_default_constructible<expr>::value>>
