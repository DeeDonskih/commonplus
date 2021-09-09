/**
 * @file buffer_types.hpp
 * @author Dmitry Donskikh (deedonskihdev@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-04-26
 * 
 * @copyright Copyright (c) 2018-2021 Dmitriy Donskikh
 *            All rights reserved.
 * 
 * NOTICE:
 * This is an part of uCppFramework project.
 * Using, modifying and distribution without permit of author prohibited by law.
 *
 */

#include <cstdint>
#include <cstddef>
#include <cstdlib>

#include <common_definitions.h>

  /**
 * @brief  helpers
 * 
 */
namespace helpers
{
    /**
     * @brief 
     * @tparam Tp 
     */
    template<typename Tp>
    struct buffer_p
    {
        using type_buffer=buffer_p<Tp>; //<brief brief description
        using buffer_type=Tp; //<brief brief description
        using buffer_ptr=Tp*;
        using buffer_ref=Tp&;
        using iterator=Tp*;
        /**
     * @brief Construct a new type_buffer object
     * 
     */
        buffer_p()
          :m_data(nullptr),
          m_size(0)
            {};
        /**
     * @brief Construct a new type_buffer object
     * @param data 
     * @param size 
     */
        buffer_p(buffer_ptr data,size_t size)
          :m_data(data),
          m_size(size)
            {};
        /**
     * @brief Construct a new type_buffer object
     * @param first 
     * @param last 
     */
        buffer_p(iterator first,iterator last)
          :m_data(first),
          m_size(last-first)
        {}
        /**
     * @brief Construct a new type_buffer object
     * @param other 
     */
        buffer_p(const type_buffer& other)
          :m_data(other),
          m_size(other.size())
        {}
        /**
     * @brief Construct a new type_buffer object
     * @param other 
     */
        buffer_p(type_buffer&& other)
          :m_data(other),
          m_size(other.size())
        {
            other.data()=nullptr;
            other.size()=0;
        }

        /**
 * @brief Copy assign
 * @param other 
 * @return type_buffer& 
 */
        type_buffer& operator=(const type_buffer& other)
        {
            assert_warning(m_data,"Buffer has another pointer. Ptr may be lost!");
            if(this==&other)return *this;

            m_data=other.data();
            m_size=other.size();
            return *this;
        }
        /**
 * @brief Move assign
 * @param other 
 * @return type_buffer& 
 */
        type_buffer& operator=(type_buffer&& other)
        {
            assert_warning(m_data,"Buffer has another pointer. Ptr may be lost!");
            if(this==&other)return *this;

            m_data=other.data();
            m_size=other.size();
            other.data()=nullptr;
            other.size()=0;
            return *this;
        }


      private:
        buffer_ptr m_data;
        size_t m_size;

    };

}

using ByteBuffer=helpers::buffer_p<uint8_t>;
