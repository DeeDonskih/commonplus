/**
 * @file ufw.hpp
 * @author Dmitry Donskikh (deedonskihdev@gmail.com)
 * @brief renewed macro list for gcc
 * @version 0.1
 * @date 2021-03-27
 * 
 * @copyright Copyright (c) 2018-2021 Dmitriy Donskikh
 *            All rights reserved.
 * 
 * 
 */
#ifndef ufwForceInline
  #define ufwForceInline          inline __attribute__((always_inline))
#endif

#define	UFW_STRINGIFY(s)	#s

#define	UFW_CONCAT(a,b)         a ## b

#define	UFW_CONCAT3(a,b,c)	a ## b ## c

#define	UFW_CONCAT4(a,b,c,d)	a ## b ## c ## d

#define	UFW_CONCAT5(a,b,c,d,e)	a ## b ## c ## d ## e

#define UFW_ARRAY_SIZE(x)	(sizeof(x) / sizeof(x[0]))

#define ufw_unused		__attribute__((unused))
#define ufw_aligned(n)		__attribute__((aligned(n)))
#define ufw_packed              __attribute__((packed))
#define ufw_may_alias		__attribute__((__may_alias__))
#define ufw_deprecated(msg)	__attribute__((deprecated(msg)))
#define ufw_section(s)		__attribute__((section(s)))
#define ufw_fallthrough		__attribute__((fallthrough))
#define ufw_noreturn		__attribute__((noreturn))
#define ufw_weak		__attribute__((weak))

#define ufw_fastcode		ufw_section(".fastest_i")
#define ufw_extram		ufw_section(".extsram")
#define ufw_fastdata		ufw_section(".fastdata")

#ifdef __cplusplus
  #define ufw_ext_c extern "C"
#else
  #define ufw_ext_c
#endif

#define UFW_BINIMPORT(name, file, section)                                        \
		asm                                                               \
		(                                                                 \
			".section " #section "\n"                                 \
			".balign 4\n"                                             \
			".global " #name "\n"                                     \
			#name ":\n"                                               \
			".incbin \"" file "\"\n"                                  \
			".global " #name "_laddr\n"                               \
			".set " #name "_laddr, . - " #name "\n"                   \
			".balign 4\n"                                             \
			".section \".text\"\n"                                    \
		);                                                                \
		ufw_ext_c uint8_t name ## _laddr[];                               \
		ufw_ext_c const size_t name ## _length = (size_t) name ## _laddr; \
		ufw_ext_c uint8_t name[]
