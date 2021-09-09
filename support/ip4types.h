 /*
 *   Copyright (c) 2019-2020 by Dmitry Donskih
 *   All rights reserved.
 *
 *   Attention Software modification allowed only with
 *   the permission of the author.
 *
 */
#pragma once

#include <cstdint>
#include <cstddef>



#define IP4_ADDR(ipaddr, a,b,c,d)  (ipaddr)->addr = PP_HTONL(LWIP_MAKEU32(a,b,c,d))


class IPv4Address
{
private:
    uint32_t m_address=0;
    inline uint32_t
    from4bytes(uint8_t a,uint8_t b,uint8_t c,uint8_t d);
public:
    /**
     * @brief IPv4Address default constructor (IP=0.0.0.0)
     */
    IPv4Address();
    /**
     * @brief IPv4Address from "C" string
     * @param ip
     */
    IPv4Address(const char* ip);
    /**
     * @brief IPv4Address from four uints (IP=a.b.c.d)
     */
    IPv4Address(uint8_t a,uint8_t b,uint8_t c,uint8_t d);
    /**
     * @brief IPv4Address from array of 4 (IP=ip[0].ip[1].ip[2].ip[3])
     */
    IPv4Address(const uint8_t (&ip)[4]);
    /**
     * @brief IPv4Address from
     * @param ip
     */
    IPv4Address(uint32_t ip);
    /**
     * @brief operator []
     * @param index
     * @return
     */
    int operator[](const size_t index);
    /**
     * @brief operator uint32_t
     */
    operator uint32_t();


    /**
     * @brief htonl - unsigned int byte swap to internet endian
     * @param x - int in little endian
     * @return
     */
    static inline uint32_t
    toNetwork(uint32_t x);
};


inline uint32_t IPv4Address::from4bytes(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    return (((uint32_t)((a) & 0xff) << 24)|
            ((uint32_t)((b) & 0xff) << 16)|
            ((uint32_t)((c) & 0xff) << 8)|
             (uint32_t)((d) & 0xff));
}

inline uint32_t IPv4Address::toNetwork(uint32_t x)
{
    return (((x) & 0x000000ffU) << 24)|
           (((x) & 0x0000ff00U) <<  8)|
           (((x) & 0x00ff0000U) >>  8)|
           (((x) & 0xff000000U) >> 24);
}
