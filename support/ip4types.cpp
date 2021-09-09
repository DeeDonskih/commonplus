/*
 * Copyright (c) 2019-2021 by Dmitry Donskih
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */
#include "ip4types.h"

#include <arpa/inet.h>
#include <netinet/in.h>

//#include "socket.h" //in case of lwIP


IPv4Address::IPv4Address()
    :m_address(0)
{}

IPv4Address::IPv4Address(const char *ip)
    :m_address(inet_addr(ip))
{}

IPv4Address::IPv4Address(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
    :m_address(from4bytes(a,b,c,d))
{}

IPv4Address::IPv4Address(const uint8_t (&ip)[4])
    :IPv4Address(ip[0],ip[1],ip[2],ip[3])
{}

IPv4Address::IPv4Address(uint32_t ip)
    :m_address(IPv4Address::toNetwork(ip))
{}

int IPv4Address::operator[](const size_t index)
{
    if(index<4)return *(reinterpret_cast<uint8_t*>(&m_address)+index);
    return -1;
}

IPv4Address::operator uint32_t()
{
    return m_address;
}



#ifdef UFW_IPTOOLS_UNDEFINED
#include <ctype.h>
#include <cassert>
/**
 * @brief "C"-array to "network address"
 * @copyright This is function reimplementation from lwIP stack IPv4 address tools.
 *            Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 *            All rights reserved.
 * @author Adam Dunkels <adam@sics.se>
 */
int
ip4addr_aton(const char *cp, uint32_t *addr)
{
    uint32_t val;
    uint8_t base;
    char c;
    uint32_t parts[4];
    uint32_t *pp = parts;

    c = *cp;
    for (;;) {
        /*
     * Collect number up to ``.''.
     * Values are specified as for C:
     * 0x=hex, 0=octal, 1-9=decimal.
     */
        if (!isdigit(c)) {
            return 0;
        }
        val = 0;
        base = 10;
        if (c == '0') {
            c = *++cp;
            if (c == 'x' || c == 'X') {
                base = 16;
                c = *++cp;
            } else {
                base = 8;
            }
        }
        for (;;) {
            if (isdigit(c)) {
                val = (val * base) + (uint32_t)(c - '0');
                c = *++cp;
            } else if (base == 16 && isxdigit(c)) {
                val = (val << 4) | (uint32_t)(c + 10 - (islower(c) ? 'a' : 'A'));
                c = *++cp;
            } else {
                break;
            }
        }
        if (c == '.') {
            /*
       * Internet format:
       *  a.b.c.d
       *  a.b.c   (with c treated as 16 bits)
       *  a.b (with b treated as 24 bits)
       */
            if (pp >= parts + 3) {
                return 0;
            }
            *pp++ = val;
            c = *++cp;
        } else {
            break;
        }
    }
    /*
   * Check for trailing characters.
   */
    if (c != '\0' && !isspace(c)) {
        return 0;
    }
    /*
   * Concoct the address according to
   * the number of parts specified.
   */
    switch (pp - parts + 1) {

    case 0:
        return 0;       /* initial nondigit */

    case 1:             /* a -- 32 bits */
        break;

    case 2:             /* a.b -- 8.24 bits */
        if (val > 0xffffffUL) {
            return 0;
        }
        if (parts[0] > 0xff) {
            return 0;
        }
        val |= parts[0] << 24;
        break;

    case 3:             /* a.b.c -- 8.8.16 bits */
        if (val > 0xffff) {
            return 0;
        }
        if ((parts[0] > 0xff) || (parts[1] > 0xff)) {
            return 0;
        }
        val |= (parts[0] << 24) | (parts[1] << 16);
        break;

    case 4:             /* a.b.c.d -- 8.8.8.8 bits */
        if (val > 0xff) {
            return 0;
        }
        if ((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff)) {
            return 0;
        }
        val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
        break;
    default:
        assert(0);
        break;
    }
    if (addr) {
        *addr=IPv4Address::htonl(val);
//        ip4_addr_set_u32(addr, lwip_htonl(val));
    }
    return 1;
}

#endif
