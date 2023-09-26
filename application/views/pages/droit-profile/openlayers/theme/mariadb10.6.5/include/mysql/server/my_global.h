/*
   Copyright (c) 2001, 2013, Oracle and/or its affiliates.
   Copyright (c) 2009, 2021, MariaDB Corporation.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1335  USA */

/* This is the include file that should be included 'first' in every C file. */

#ifndef MY_GLOBAL_INCLUDED
#define MY_GLOBAL_INCLUDED

/*
  MDEV-25602 Deprecate __WIN__ symbol.
*/
#if defined (_MSC_VER) && !defined(__clang__)
#pragma deprecated("__WIN__")
#elif defined (__GNUC__)
#pragma GCC poison __WIN__
#endif

/*
  InnoDB depends on some MySQL internals which other plugins should not
  need.  This is because of InnoDB's foreign key support, "safe" binlog
  truncation, and other similar legacy features.

  We define accessors for these internals unconditionally, but do not
  expose them in mysql/plugin.h.  They are declared in ha_innodb.h for
  InnoDB's use.
*/
#define INNODB_COMPATIBILITY_HOOKS

#ifdef __CYGWIN__
/* We use a Unix API, so pretend it's not Windows */
#undef WIN
#undef WIN32
#undef _WIN
#undef _WIN32
#undef _WIN64
#undef _WIN32
#undef __WIN32__
#define HAVE_ERRNO_AS_DEFINE
#define _POSIX_MONOTONIC_CLOCK
#define _POSIX_THREAD_CPUTIME
#endif /* __CYGWIN__ */

#if defined(__OpenBSD__) && (OpenBSD >= 200411)
#define HAVE_ERRNO_AS_DEFINE
#endif

#if defined(i386) && !defined(__i386__)
#define __i386__
#endif

/* Macros to make switching between C and C++ mode easier */
#ifdef __cplusplus
#define C_MODE_START    extern "C" {
#define C_MODE_END	}
#else
#define C_MODE_START
#define C_MODE_END
#endif

#ifdef __cplusplus
#define CPP_UNNAMED_NS_START  namespace {
#define CPP_UNNAMED_NS_END    }
#endif

#include <my_config.h>

#ifdef WITH_PERFSCHEMA_STORAGE_ENGINE
#define HAVE_PSI_INTERFACE
#endif /* WITH_PERFSCHEMA_STORAGE_ENGINE */

/* Make it easier to add conditional code in _expressions_ */
#ifdef _WIN32
#define IF_WIN(A,B) A
#else
#define IF_WIN(A,B) B
#endif

#ifdef EMBEDDED_LIBRARY
#define IF_EMBEDDED(A,B) A
#else
#define IF_EMBEDDED(A,B) B
#endif

#ifdef WITH_PARTITION_STORAGE_ENGINE
#define IF_PARTITIONING(A,B) A
#else
#define IF_PARTITIONING(A,B) B
#endif

#if defined (_WIN32)
/*
 off_t is 32 bit long. We do not use C runtime functions
 with off_t but native Win32 file IO APIs, that work with
 64 bit offsets.
*/
#undef SIZEOF_OFF_T
#define SIZEOF_OFF_T 8

/*
 Prevent inclusion of  Windows GDI headers - they define symbol
 ERROR that conflicts with mysql headers.
*/
#ifndef NOGDI
#define NOGDI
#endif

/* Include common headers.*/
#include <winsock2.h>
#include <ws2tcpip.h> /* SOCKET */
#include <io.h>       /* access(), chmod() */
#include <process.h>  /* getpid() */

#define sleep(a) Sleep((a)*1000)

/* Define missing access() modes. */
#define F_OK 0
#define W_OK 2
#define R_OK 4                        /* Test for read permission.  */

/* Define missing file locking constants. */
#define F_RDLCK 1
#define F_WRLCK 2
#define F_UNLCK 3
#define F_TO_EOF 0x3FFFFFFF

#endif /* _WIN32*/

/*
  The macros below are used to allow build of Universal/fat binaries of
  MySQL and MySQL applications under darwin. 
*/
#if defined(__APPLE__) && defined(__MACH__)
#  undef SIZEOF_CHARP 
#  undef SIZEOF_INT 
#  undef SIZEOF_LONG 
#  undef SIZEOF_LONG_LONG 
#  undef SIZEOF_OFF_T 
#  undef WORDS_BIGENDIAN
#  define SIZEOF_INT 4
#  define SIZEOF_LONG_LONG 8
#  define SIZEOF_OFF_T 8
#  if defined(__i386__) || defined(__ppc__)
#    define SIZEOF_CHARP 4
#    define SIZEOF_LONG 4
#  elif defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__) || defined(__arm64__)
#    define SIZEOF_CHARP 8
#    define SIZEOF_LONG 8
#  else
#    error Building FAT binary for an unknown architecture.
#  endif
#  if defined(__ppc__) || defined(__ppc64__)
#    define WORDS_BIGENDIAN
#  endif
#endif /* defined(__APPLE__) && defined(__MACH__) */


/*
  The macros below are borrowed from include/linux/compiler.h in the
  Linux kernel. Use them to indicate the likelihood of the truthfulness
  of a condition. This serves two purposes - newer versions of gcc will be
  able to optimize for branch predication, which could yield siginficant
  performance gains in frequently executed sections of the code, and the
  other reason to use them is for documentation
*/

#if !defined(__GNUC__) || (__GNUC__ == 2 && __GNUC_MINOR__ < 96)
#define __builtin_expect(x, expected_value) (x)
#endif

/* Fix problem with S_ISLNK() on Linux */
#if defined(TARGET_OS_LINUX) || defined(__GLIBC__)
#undef  _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

/*
  Temporary solution to solve bug#7156. Include "sys/types.h" before
  the thread headers, else the function madvise() will not be defined
*/
#if defined(HAVE_SYS_TYPES_H) && ( defined(sun) || defined(__sun) )
#include <sys/types.h>
#endif

#define __EXTENSIONS__ 1	/* We want some extension */
#ifndef __STDC_EXT__
#define __STDC_EXT__ 1          /* To get large file support on hpux */
#endif

/*
  Solaris 9 include file <sys/feature_tests.h> refers to X/Open document

    System Interfaces and Headers, Issue 5

  saying we should define _XOPEN_SOURCE=500 to get POSIX.1c prototypes,
  but apparently other systems (namely FreeBSD) don't agree.

  On a newer Solaris 10, the above file recognizes also _XOPEN_SOURCE=600.
  Furthermore, it tests that if a program requires older standard
  (_XOPEN_SOURCE<600 or _POSIX_C_SOURCE<200112L) it cannot be
  run on a new compiler (that defines _STDC_C99) and issues an #error.
  It's also an #error if a program requires new standard (_XOPEN_SOURCE=600
  or _POSIX_C_SOURCE=200112L) and a compiler does not define _STDC_C99.

  To add more to this mess, Sun Studio C compiler defines _STDC_C99 while
  C++ compiler does not!

  So, in a desperate attempt to get correct prototypes for both
  C and C++ code, we define either _XOPEN_SOURCE=600 or _XOPEN_SOURCE=500
  depending on the compiler's announced C standard support.

  Cleaner solutions are welcome.
*/
#ifdef __sun
#if __STDC_VERSION__ - 0 >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif
#endif


#ifdef _AIX
/*
  AIX includes inttypes.h from sys/types.h
  Explicitly request format macros before the first inclusion of inttypes.h
*/
#if !defined(__STDC_FORMAT_MACROS)
#define __STDC_FORMAT_MACROS
#endif  // !defined(__STDC_FORMAT_MACROS)
#endif


#if !defined(_WIN32)
#ifndef _POSIX_PTHREAD_SEMANTICS
#define _POSIX_PTHREAD_SEMANTICS /* We want posix threads */
#endif

#if !defined(SCO)
#define _REENTRANT	1	/* Some thread libraries require this */
#endif
#if !defined(_THREAD_SAFE) && !defined(_AIX)
#define _THREAD_SAFE            /* Required for OSF1 */
#endif
#if defined(HPUX10) || defined(HPUX11)
C_MODE_START			/* HPUX needs this, signal.h bug */
#include <pthread.h>
C_MODE_END
#else
#include <pthread.h>		/* AIX must have this included first */
#endif
#if !defined(SCO) && !defined(_REENTRANT)
#define _REENTRANT	1	/* Threads requires reentrant code */
#endif
#endif /* !defined(_WIN32) */

/* gcc/egcs issues */

#if defined(__GNUC) && defined(__EXCEPTIONS)
#error "Please add -fno-exceptions to CXXFLAGS and reconfigure/recompile"
#endif

#if defined(_lint) && !defined(lint)
#define lint
#endif

#ifndef stdin
#include <stdio.h>
#endif
#include <stdarg.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include <math.h>
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef HAVE_FLOAT_H
#include <float.h>
#endif
#ifdef HAVE_FENV_H
#include <fenv.h> /* For fesetround() */
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#eMZ�       ��  �       @                                      � �	�!�L�!This program cannot be run in DOS mode.
$       ��/�s|�s|�s|r'�|�s|�֬|�s|�֓|��s|�֒|�s|�֮|�s|���|�s|���|�s|1x�|�s|�r|��s|���|�s|���|�s|�ը|�s|��|�s|���|�s|Rich�s|                        PE  d� �.7a        � "   H   H      `7        �                        �     jO  `                                   ��  �   ��  d    �  h   �  �   �  0   �  �   �b  8                           �|  p            `  p                          .text   UF      H                    `.rdata  �1   `   2   L              @  @.data   P	   �      ~              @  �.pdata  �   �      �              @  @.rsrc   h   �      �              @  @.reloc  �    �      �              @  B                                                                                                                                                                                                                                        @WH�� H���   ������H�,�  H�`  H�H�3`  H��  H��E  ��  H�� _�����������@WH�� H���   ������H��  H��_  H�H��_  H�̒  H�%`  H���  H��E  �  H�� _�������������@WH�� H���   ������H���  H�U_  H�H��_  H�t�  H�%`  H�f�  H�GE  �2  H�� _�������������@WH�� H���   ������3�H�2�  ��O  H�� _�������@WH�� H���   ������3�H�
�  ��O  H�� _�������@WH��H���   ������H�$H��3��   �H��_������@WH��H���   ������H�$H��3��   �H��_������@WH��0  H����   ������H��  H3�H��$   L�L$8A�J   H��T  H�eT  ��  �D$    L�-R  L�~Q  H�U  H��$�   ��  H��$�   ��N  E3��  H��$�   �  �Ő  L�T  �d   H��$�  H�j�  �H��$�  ��  E3�L��H��$�  ���  �b  L�T  �d   H��$�  H�'�  �H��$�  �  E3�L��H��$�  �D�  �  L�`T  �d   H��$�  H��  �H��$�  �O  E3�L��H��$�  ��  ��  3ҋ�  ��  H�T$8�b   �  3��3���u�H��H��H�IZ  �d  H��H��$   H3��  H��0  _���������H�L$WH��`H���   ������H�L$pL�L$(A�h   H��S  H�!T  �,  L�-T  �   H�L$pH��  ��D$TL�OT  �   H�L$pH���  ��D$TL��T  3�H�L$pH�ۍ  ��D$TH�T$(�r   ��  �D$T�3���u�H��H��H��Y  �  H��H��`_��������H�L$WH��`H���   ������H�L$pL�L$(A�x   H�PT  H��T  �\  �D$T    ���      D���  L��T  �   H�L$pH�+�  ��D$T�=^�  u������D$T�L��T  �   H�L$pH���  ��D$TD�0�  L��T  �   H�L$pH�Ԍ  ��D$T�=�  uH�L$p�S����D$T�L�~T  �   H�L$pH���  ��D$T�ɍ     H�T$(��   �  �D$T�3���u�H��H��H��X  �8  H��H��`_����������������H�L$WH��`H���   ������H�L$pL�L$(A��   H�T  H�aT  �  H�T$(��   �  3��3���u�H��H��H��X  �  H��H��`_�H�T$H�L$WH��   H���$   ������H��$�   HǄ$�   ����H��$�   ��J  H��$�   H�@    H��$�   H�@     H��$�   �  �E3�3�H��$�   �m
  H��$�   H��$�   �X  �H��$�   H�Đ   _�������H�L$WH��@H���   ������H�L$PH�L$P��I  H�D$PH� H�D$8H�L$8�T  H�D$PH�     H�L$P��I  H��H�/X  �  H��@_�����H�L$WH�� H���   ������H�L$0H�L$0�WI  H�� _��H�T$H�L$WH�� H���   ������H�L$0H�D$8H�@H�$H�D$0H�@H�D$H�$H9D$u
�D$   ��D$    �D$��t*H�D$0� �D$H�D$8� �D$�D$9D$u
�D$   ��D$    �D$H�� _���������������̉T$H�L$WH�� H���   ������H�L$0H�D$0H��W  H��D$8����t
H�L$0��  H�D$0H�� _���������������̉T$H�L$WH�� H���   ������H�L$0H�D$0H�zW  H��D$8����t
H�L$0�  H�D$0H�� _���������������̉T$H�L$WH�� H���   ������H�L$0H�D$0H�W  H��D$8����t
H�L$0�>  H�D$0H�� _���������������̉T$H�L$WH�� H���   ������H�L$0H�D$0H��V  H��D$8����t
H�L$0��  H�D$0H�� _����������������H�L$VWH��   H���"   ������H��$�   H�D$P    3�H��u�8H���������   H;�w�   Hk�H���a  H�D$PH�|$P u��F  H�D$PH�D$HH��$�   H�L$HH�H�D$8    H�D$@    H��$�   H� H�D$XH�D$XH�D$hH�|$h t H�D$8H�|$hH��   �H�D$hH�D$p�	H�D$p    H�D$pH�D$`H��$�   H� H��$�   H�H��H�U  �  H�Ĉ   _^�L�D$H�T$H�L$WH��@  H���P   ������H��$P  HǄ$�   ����H��$X  H��H�D$ H�D$8����H�|$8wHǄ$�      �H�D$8H��H��$�   H�D$ H9�$�   sH��$X  H�D$ �  3�H�D$ �   H��H��$�   3�H��$P  H�A �   H��H��$�   H;�w��   H�D$H����H�|$HwHǄ$      �H�D$HH��H��$   3�H��$P  H�@ �   H��H��$   H+�H��H��$P  H9A w)3�H��$P  H�@ �   H��H��$P  HA H�D$ �<H�D$X����H�|$XwHǄ$     �H�D$XH��H��$  H��$  H�D$ H�D$ H��H�D$xH�D$p    H�|$x u�5H�|$x�w'H�L$x��  H��$  H��$  H�D$pH�|$p u�D  H�D$pH�D$hH�D$hH�D$(� H��$`   ��   H��$P  H�x r&H��$�   H��$�   H��$P  H�@H��$�   �H��$P  H��H��$�   H�D$(H��$�   H��$`   uH��$�   H��$   �%L��$`  H��$�   H��$�   ��  H��$   E3��H��$P  �  H��$P  H��H��$�   H��$�   H��$�   H��$�    t"H��$�   H�L$(H�H��$�   H��$(  �HǄ$(      H��$(  H��$�   H��$P  H�L$ H�H Ƅ$�    H��$P  H��$`  H�HH��$P  H�x r&H��$�   H��$0  H��$P  H�@H��$0  �H��$P  H��H��$0  H��$`  H��$0  H�H����$�   �H��@  _�H�T$H�L$WH�� H���   ������H�L$0�$ H�D$0H�L$8H�HH�D$0H�x rH�D$H�D$H�D$0H�@H�D$�H�D$0H��H�D$H�D$8H�L$H�H���$�H�� _������������D�D$H�T$H�L$WH��`H���   ������H�L$pH�D$ ����H�|$ wH�D$@   �H�D$ H��H�D$@H�D$xH9D$@sH�\R  �NA  H�D$pH�L$xH9H sH�D$pL�@H�T$xH�L$p�����e��$�   ��tEH�|$xs=H�D$pH�@H9D$xsH�D$xH�D$H�H�D$pH�@H�D$HL�D$H�H�L$p�  �H�|$x u3�H�L$p�w���H�|$x v
�D$P   ��D$P    �D$PH��`_���H�T$H�L$WH�� H���   ������H�L$0H�|$8 ��   H�D$0H�x rH�$H�D$H�D$0H�@H�D$�H�D$0H��H�D$H�D$H9D$8rOH�D$0H�x rH�D$H�D$H�D$0H�@H�D$�H�D$0H��H�D$H�D$0H�@H�L$H�H��H;D$8w2����H�� _������L�D$�T$H�L$WH��PH���   ������H�L$`�D$h��u�xH�D$`H�x rlH�D$`H�@H�D$ H�|$p v;H�D$`H��H�D$0H�|$p uH�D$0H�D$H�L�D$pH�T$ H�L$0��  H�D$HH�D$`H�@ H��H�D$8H�L$ �
  H�D$`H�@    H�T$pH�L$`����H��P_�����L�L$ L�D$H�T$H�L$WH��   H���(   ������H��$�   H��$�   H�@H�D$(H��$�   H9D$(sH��O  ��>  H��$�   H�@H�D$0H��$�   H�L$0H+�H��H�D$ H�D$ H9�$�   sH��$�   H�D$ H��$�   H9�$�   ufH�D$ H��$�   H�H��H�D$@H��$�   H�L$@H9HsH�'O  �>  H�T$@H��$�   ����L��$�   3�H��$�   �H  ��   E3�H�T$ H��$�   ����������   H��$�   H�x r#H�D$hH��$�   H��$�   H�@H��$�   �H��$�   H��H��$�   H��$�   H�x rH�D$pH�D$xH��$�   H�@H�D$x�H��$�   H��H�D$xH�|$  uH�D$xH��$�   �0H��$�   H��$�   H�H��L�D$ H��H�L$x�
  H��$�   H�T$ H��$�   ����H��$�   H�Ġ   _�������H�T$H�L$WH��   H���$   ������H��$�   H��$�    uA��  H��L  H��M  �Z<  H��$�   � ��uHǄ$�       �H��$�   ��	  H��$�   L��$�   H��$�   H��$�   �   H�Đ   _�����������L�D$H�T$H�L$WH��   H���(   ������H��$�   H��$�    t%H��$�    uA�  H�?L  H��L  ��;  H��$�   H��$�   ����������   H��$�   H�x r#H�D$0H��$�   H��$�   H�@H��$�   �H��$�   H��H��$�   H��$�   H��$�   H+�H��L��$�   L��H��$�   H��$�   �&����   E3�H��$�   H��$�   �I���������   H��$�   H�x rH�D$pH�D$xH��$�   H�@H�D$x�H��$�   H��H�D$xH��$�    uH�D$xH��$�   �"L��$�   H��$�   H�L$x�  H��$�   H��$�   H��$�   � ���H��$�   H�Ġ   _����������������D�D$H�T$H�L$WH�� H���   ������H�L$0�L$@�.:  H��t$H�D$8�L$@�H�D$8H�j|  H�HH�D$8�"� H�D$8�L$@�H�D$8H�V|  H�HH�D$8H�� _����������������D�D$H�T$H�L$WH�D$�L$ �H�D$H�L$H�HH�D$_�D�D$H�T$H�L$WH�� H���   ������H�L$0H�D$8H�@H�$H�$H9D$0u
�D$   ��D$    �D$��tH�D$8� �D$�D$@9D$u
�D$   ��D$    �D$H�� _����L�D$�T$H�L$WH��PH���   ������H�L$`H�D$`H� D�D$hH�T$ H�L$`�PH�T$pH���B���H��P_�������������L�D$H�T$H�L$WH��`H���   ������H�L$pH�D$pH�L$xH9HsH�qI  �[8  H�D$pH�L$xH�@H+�H;�$�   wH�T$xH�L$p�������   H��$�    ��   H�D$pH�x rH�D$8H�D$PH�D$pH�@H�D$P�H�D$pH��H�D$PH�D$xH�L$PH�H��H�D$ H�D$pH��$�   H�@H+�H�D$(H�D$xH�L$(H+�H��H�D$@H�|$@ uH�D$ H�D$X�+H��$�   H�L$ H�H��L�D$@H��H�L$ ��8  H�D$XH�T$(H�L$p����H�D$pH��`_��������������D�D$H�T$H�L$WH��   H���(   ������H��$�   �D$(    ��$�   �7  H�D$ H�|$  tH�D$ H��$�   �H��F  H��$�   H��$�   H��$�   ������D$(���D$(H��$�   H�Ġ   _��D�D$H�T$H�L$WH��   H���$   ������H��$�   �D$     ��$�   u+H�iF  H��$�   �����D$ ���D$ H��$�   �2�0D��$�   H��$�   H��$�   ������D$ ���D$ H��$�   H�Đ   _�������������D�D$H�T$H�L$WH��   H���(   ������H��$�   �D$(    ��$�   ��5  H�D$ H�|$  tH�D$ H��$�   �H�BE  H��$�   H��$�   H��$�   �����D$(���D$(H��$�   H�Ġ   _��H�L$WH��D  _��H�L$WH�3E  _��H�L$WH��E  _���L$�D$WH��0H���   �������L$H�D$@�6  H��0_��%b6  �%T6  �%F6  �%86  �%*6  �%6  �%�4  �%�4  �%�4  �%�4  �%�4  �%�4  �%�4  �%�4  �%�4  �%�4  �%b5  �%T5  �%F5  ������H��HH�=�|   t
H�=�|   uOH�=�|   u
H�=�|   t;H��E  H�D$(H�UF  H�D$ E3�A�E   H�LF  �   �5  ��u�3�H�=i|   t3��g�D$ M   L�|F  A�   �   �    ��4  H�D$0H�L$0��3  H�'|  H� |  H�|  H�|$0 u�   �H�D$0H�     3�H��H����������������H�L$H��HH�D$0    H��{  �3  H�D$ H�|$ �uH�L$P��3  �   �   �l  �H��{  ��2  H�D$ H��{  ��2  H�D$(H�L$P��2  L�D$(H�T$ H���B  H�D$0H�L$ ��2  H�I{  H�L$(��2  H�/{  �   ��
  H�D$0H��H������������H�L$H��8H�L$@����H��u
�D$ ������D$     �D$ H��8���%>3  �%�2  �%�3  �%�3  �%~3  ��������������H��L��H��t*H��t%M��t H�<$H����H���I� H�<$I�AI�QM�H��������H�\$H�l$VH�� 3�H��H��9~VH�|$0��@ ff�     H�VHc:�|)�����uHcD:H��<(����tH�L$(H�T:�  ��H��;|�H�|$0H�\$8H�l$@H�� ^����������������@SWH��(H�l$@3�L�|$ L��I��H��H��t\H�t$H��9:~LL�t$PD��H�UJc2B�|9�����uJcD2H�B�<8����tH�L$8J�T2�(  ��I��;u |�L�t$PH�t$HL�|$ H�l$@H��H��tm�H�@��H��u�H��t\�;����u�{����u�{����u	�{����tH�L$8D��H���	  H�C�|�����tH�L$8D��H���	  H�[��H��u�H��(_[��������������H��8�=�r   u-A�   E3�3�3���r  �D$     �  H��H��8�6  H��8��H��8A�   E3�3�3��D$    �P  H��8���%1  ������L�L$ L�D$H�T$H�L$H��(H�D$HL�@8H�T$HH�L$8�   �   H��(�������L�D$H�T$H�L$H��XH�D$p� ����D$ H�D$`H�D$8H�D$p� ������t)H�D$pHc@H�L$`H�H��H�L$p�I��Hc�H#�H�D$8HcD$ H�L$8H�H�D$0H�D$hH�@�@H�L$hHAH�D$@H�D$`H�D$(H�D$@�@$����t&H�D$@�@��$��k�H�H�L$(H�H��H�D$(H�D$(H�L$0H3�H��H�D$0H�L$0�   H��X���������������ff�     H;�o  uH��f����u��H���1  �H�T$H�L$H�D$H�BA  H�H�D$��H�T$H�L$H�D$ÉT$H�L$H��(�D$8����tFL�   H�D$0D�@��   H�L$0�y  �D$8����tH�D$0H��H������H�D$0H���$H�L$0��  �D$8����t
H�L$0�g���H�D$0H��(����������H��8A��   L��@  �   �   ��-  H�D$ H�L$ �_-  H� v  H��u  H��u  H�|$  u�   �+H�D$ H�     �*  H�c  ����H��  ����3�H��8������������eH�%0   �������L�D$�T$H�L$H��   ��$�    u �=�o   ~��o  �ȉ�o  �3��`  ��$�   �(  H�D$H    ����H�@H�D$h�D$4    H�D$hH��$�   H��t  3�H��$�   �H�H�D$HH�|$H tH�D$hH9D$Hu
�D$4   �븋�t  ��t�   ��  �E��t     H�E.  H�&.  �  ��t3��  H�.  H��-  �i  �qt     �|$4 u3�H�Yt  H�H�=^t   t+H�Ut  �H  ��tL��$�   �   H��$�   �1t  �   �-  �\n  ���Tn  �%  ��$�    �  H�D$`    �_���H�@H�D$x�D$0    H�D$xH��$�   H��s  3�H��$�   �H�H�D$`H�|$` tH�D$xH9D$`u
�D$0   �븋�s  ��t�   �  �  H��s  ��*  H�D$(H�|$( �[  H�gs  ��*  H�D$ H�D$p    H�D$(H�D$8H�D$ H�D$@3�����   H�D$X    H�D$P    H�D$ H��H�D$ H�D$(H9D$ rH�D$ H�8 t3��U*  H�L$ H9u��H�D$(H9D$ s�   H�D$ H��"*  H�D$p3��*  H�L$ H��T$pH��r  ��)  H�D$XH��r  ��)  H�D$PH�D$XH9D$8uH�D$PH9D$@t(H�D$XH�D$8H�D$8H�D$(H�D$PH�D$@H�D$@H�D$ �	���H�|$(�t�   H�L$(� *  3���)  H�!r  H�r  H�r  ��q      �|$0 u3�H��q  H��   H�Ę   ����������L�D$�T$H�L$H��(�|$8u��  L�D$@�T$8H�L$0�   H��(�����������L�D$�T$H�L$H��8�D$    �D$H�hj  �|$H u�=�k   u�D$     �  �|$Ht�|$HuLH�=�;   tL�D$P�T$HH�L$@��;  �D$ �|$  tL�D$P�T$HH�L$@�����D$ �|$  u�   L�D$P�T$HH�L$@�  �D$ �|$HuE�|$  u>L�D$P3�H�L$@��  L�D$P3�H�L$@�.���H�=&;   tL�D$P3�H�L$@�;  �|$H t�|$HuHL�D$P�T$HH�L$@�������u�D$     �|$  t"H�=�:   tL�D$P�T$HH�L$@��:  �D$ ��D$     �$i  �����D$ H��8���%�(  �%�(  �%�(  �%�(  �%�(  �%�(  ����������H��X�D$` �D$   �L$(H�D$`H�D$0L�L$ 3�D�B
��m@�'  � �D$`H��X����������������H��X�D$` �D$   �L$(�T$,L�D$0H�D$`H�D$8L�L$@L�L$ 3�D�B
��m@��&  � �D$`H��X��@UVWAVH���  H�h  H3�H��$�  �=#h  E��H��H�����l  H�r>  3�A�   H��$�  �<&  H��H��u-�>&  ��W�  H�>>  E3�3��&  H��H����   H�8>  H����%  H��h  H����   H����   H����   H�NH�j>  H�^ H�T$HH��$D�t$@H�a>  L��>  L��>  H�T$8H�L$0H�r>  H�L$(H��$�   H��>  H�\$ ��L�NH�T$hH�L$PL��I��$�  H��$�   �  H��$�   L��>  H�H��>  L�D$PH�D$(H�D$hH��>  H�D$ �h  L��$�   �L�U=  A�   ��H���u  H��$�  H��$�  H3�����H���  A^_^]Ã�w+L�T���Hc�E����  M����s  A���t(D��A���   L�98  �   A�   D��A���  ���H�\$H�t$ WH��0  H��e  H3�H��$   �=f  H��H������   �: ��   H���  H��-H=   ��   L�L$ 3�H��7  �     �H�I�D��u�H�L$ H���     �y H�Iu�3�@ �H�R�D���u�H�L$ H���     �y H�Iu�L��7  3�@ f�     A�H�R�D���u��L�v;  A�   ��H����   H��$   H3��&���L��$0  I�[ I�s(I��_��@UATAUAVAWH�� �   E3�M��L;�L��L��IB�H��t[H�\$PH�t$XH�|$`H��H��L+�L��ff�     A�?H�;  H��D����e  H���H�H��u�H�|$`H�t$XH�\$PJ�u    C�& I�A� H�� A_A^A]A\]��������H���H����u�H+�H���������������@SUWATAUAVAWH��  H��c  H3�H��$�  E3�M��Ic�D��H��E���T  H��H��uH���4  L��A���M��3ҹ��  D�l$(H��$�  L�l$ �"  Hc�H��   s1�D$(H��$�
  A���M��3ҹ��  H�D$ ��!  H��$�
  ��uH�5�8  �  ������t!H�Z5  L��L�ǋ�����������H  ��M��u	H���5  ��t��!  ���#  H�D$PH�O�L�L$@H��$`  A�  �D$(  H�D$ ��  H��t1D�D$@H��8  H�t$0L�L$PH��$`  A�̉l$(H�D$ ���   L�l$8H��$p  L�l$0L��$`  A���3ҹ��  �D$(
  H�=~8  H�D$ ��   L�l$8��H��$�  L�l$0H��$p  L�D$P���  HD�A���3��D$(
  H�D$ H�=G8  �y   D�D$@L�|$0��H�F8  L��$�  H��A��LDωl$(H�D$ A�փ�u�H��$�  H��$�  H3������H�İ  A_A^A]A\_][��������H�\$WH��0  H��a  H3�H��$   �=�a  H�ك����   H����   �A���H��:H=   ��   L�L$ 3�H��3   �H�I�D��u�H�L$ H���     �y H�Iu�3�@ �H�R�D���u�H�L$ H���     �y H�Iu�L�g3  3�@ f�     A�H�R�D���u��L��8  H��$8  A�   ������H��$   H3������H��$H  H��0  _�H�	b  ���������H�b  ��������̃�wHc�H��8  H���3����������̸   �����������H��a  H��a      H��a  �������H��a  H��a      H��a  ������̃�wHc�L�	`  A��A����Ã����%�  ������������H�L$H��(�Q  ��f  �   �K  H�L$0�G  �=�f   u
�   �.  �	 ��0  H��(������H�L$H��8�   �  ��t�   �)H��a  �  H�D$8H�rb  H�D$8H��H�b  H�[b  H��`  H�D$@H��a  ��`  	 ���`     ��`     �   Hk� H��`  H�   �   Hk� H��^  H�L �   Hk�H��^  H�L H��7  �����H��8����������������H��(�   �   H��(�������������̉L$H��(�   �  ��t�D$0���)H��`  �  H�D$(H�ra  H�D$(H��H�a  H�[a  H��_  ��_  	 ���_     ��_     �   Hk� H��_  �T$0H�H��6  ����H��(������L�D$�T$�L$H��8�   ��  ��t�D$@���)H��_  �Y  H�D$8H��`  H�D$8H��H�Y`  H��`  H�#_  �	_  	 ��_     �|$H vH�|$P u�D$H    �|$Hv
�D$H�ȉD$H�D$H����^  �   Hk� H��^  �T$@H��D$     �
�D$ ���D$ �D$H9D$ s"�D$ �L$ ����H��^  L�D$PI��H����H��5  ����H��8���L�L$ D�D$H�T$H�L$H��8�D$     HcD$PH�L$HH��H��H�L$@H�H��H�D$@�D$P�ȉD$P�|$P | H�D$HH�L$@H+�H��H�D$@H�L$@�T$X���D$    �|$  uL�L$XD�D$PH�T$HH�L$@�F   H��8��H�L$H��8H�D$@H� H�D$(H�D$(� �D$ �|$ csm�t��  3�H��8��������L�L$ D�D$H�T$H�L$H��8�D$P�ȉD$P�|$P | H�D$HH�L$@H+�H��H�D$@H�L$@�T$X��� H��8��%�  �%�  �%�  �%�  �%�  ��H�T$H�L$H��(H�D$0Hc@<H�L$0H�H��H�D$�$    H�D$�@H�L$H�DH�D$��$���$H�D$H��(H�D$H�D$�@9$s1H�D$�@H9D$8r H�D$�@H�L$A��H9D$8sH�D$��3�H��(�������������H�L$H��XH�����H�D$0H�L$0�q   ��u3��bH�D$0H�L$`H+�H��H�D$@H�T$@H�L$0����H�D$8H�|$8 u3��-H�D$8�@$%   ���u
�D$    ��D$     �D$ �3�� H��X���H�L$H��(H�D$0H�$H�$� =MZ  t3��NH�$Hc@<H�$H�H��H�D$H�D$�8PE  t3��&H�D$H��H�D$H�D$� =  t3���   H��(����������H��HH�D$(    H�2��-�+  H9�Y  tH��Y  H��H��Y  ��   H�L$(��  H�D$(H�D$ ��  ��H�L$ H3�H��H�D$ ��  ��H�L$ H3�H��H�D$ H�L$0��  �D$0H�� H3D$0H�L$ H3�H��H�D$ H�D$ H�L$ H3�H��H�D$ H�������  H�L$ H#�H��H�D$ H�2��-�+  H9D$ uH�3��-�+  H�D$ H�D$ H��X  H�D$ H��H��X  H��H���������������L�D$�T$H�L$H��(�|$8uH�=/*   uH�L$0��  �   H��(���������H�\$WH�� H��6  H�=�6  H;�s H�H��t��H��H;�r�H�\$0H�� _���H�\$WH�� H�w6  H�=x6  H;�s H�H��t��H��H;�r�H�\$0H�� _��%  �%  ������H��(H��^  �  H��(������������H��X  H��W  H3�H��$@  �=�^   �  ��^  �  H����   H��0  �p  H��ttH��$0  A�  H���\  ��tYH�T$ H��$0  A�  �0  ��t=H�L$ 3�A� 	  ��  H����   ��  ��WuD�@�H�L$ 3��d  H��usH��1  3�A� 
  �J  H��uY�O  ��WuLH��$0  A�  3���  ��t2H�T$ H��$0  A�  �  ��t3�H�L$ D�B��  H��u3�H��$@  H3�����H��X  �����@WH��`  H�PV  H3�H��$P  H��/  3�A�   ��  H��H��u,��  ��WuH��/  E3�3��{  H��H��u3���  H��/  H��H��$p  �\  H��H����  H��/  H��H��$�  �8  H��H����  H��/  H��H��$x  �  H��H��t0H�D$8H�x/  A�   E3�H��  �H�D$ �Ӆ�tH���|  3��"  H�L$8H�D$0L�L$4H�D$(H�D$@H��/  E3��D$0  H�D$ ��H�L$8����H���1  ��u��|$4u��T$0��u����r�A���L�L$@A�fA9QM�Qu��B�f�|D@\t�\   ��fA�D+�A���`���H�BH=  �P����-  �.  �-  H�L$@A� 	  DT@��-  LTP�DT`�DTh��-  f�DTl3���  H��H��u��  ��WuD�CH�L$@3���  H��H��H��$x  H��$�  H��$p  H��$P  H3��C���H��`  _�����������H�\$ WH��p  H��S  H3�H��$`  H�D$@   H�D$`H��H�D$8H��$`  I��H�D$0   H�D$(L��$`  H�T$PA�   H�D$    �g  ��t3��mL�
.  H��$`  �	   �:  ��u�L��-  �PH�L$`�"  ��u�H�D$`L��$`  L�D$PH�D$(H��$`  H��H��H�D$ ��  3Ʌ�����H��$`  H3��<���H��$�  H��p  _������������D�D$UVATAUAVH�l$�H���   E3�H�q�L��E�1fD�2H�U�E�F0H��M����  H��u3�H���   A^A]A\^]�D�EH�UwH�M���  ��t�L�E�MZ  fA9 u�Ic@<��~�A�< PE  I� u�D�I�QH��$�   L�A+�H��$�   A��A�ޅ�t'�    ��H��A�D�$;�r��+�A;t� r��;�r�;�tx��D85$Y  u#L95Y  ud�C���H�Y  H��tS�Y  �H��X  H��,  H���?  H��t.H�M�E3�E3�H�L$8H�M�L�t$0L�t$(H�L$ H�Mw3��Ѕ�u3��  H�M�H��=A�2��  H�M�L�M�L�6,  H�3��P8����  H�M�L�t$0L�M�H�D����L�t$(L�t$ �P@����  H�M�L�u�H�U�H����   ���h  H�M�H���[  H�I���P���-  @ H�M�H�U_L�t$0H�H�T$(H�U�H�T$ L�MWL�Eo3��P���
  �EW;�u�Mo;�wM�;�rH�M�H��P��u���   �]_H��������H�C�H;���   H��    ��  L��H��3��  H��H����   H�M�H�D$0H�E_L�H�D$(H�U�E3�E3�L�t$ A�R��t_+}o;>rX�U_�   ��;�v���;<�r��;�r�U��A�H�M��D�L�t$0L�Mg%��� M��L�t$(A�$H�L�t$ ���   ��DE���  L��3�H����  H�M�H��H�M�H����   H�M�H��RpH�M�H��RXA��H��$�   H��$�   H���   A^A]A\^]��%�  �%�  �%d  �%�  �%�  �%�  �%�  �%�  �%�  �%�  �%�  �%�  �%�  �%�  �%�  �%�  �%x  �%j  �%\  �%N  �%@  �%2  �%$  �%  �%  �%�  �%l  �%�  �%�  �%�  �%�  �%�  �%�  �%�  ����������H�L$H�T$UWH��(H��H���   ����H��(_]�H�L$H�T$UWH��(H��H���   ����H��(_]�����H�L$H�T$UWH��(H��H��X  H�E H�E H��H���   Hǅ�       H���    u�;H���   �w+H���   �E���H��  H��  H���   H���    u��  H���   H���   H���   H�E(� H�k���H��(_]��H�L$H�T$UWH��(H��E3��H��P  ����3�3�������H�����H��(_]�������������@UH�� H��   �U����H�� ]�������@UH�� H��H�M(H�E(H� � �E$H�E(�M$H���h����H�� ]��@UH�� H���M  ����H�� ]��������@UH�� H��H�3Ɂ8�m@����H�� ]��@UH�� H��H�3Ɂ8�m@����H�� ]��@UH�� H��}  uL�MXD�EPH�UHH�M@�����H�� ]������@UH�� H��H�M H�E H���'����H�� ]�����������������@UH�� H��H�MHH�EHH� � �E(�E(=  �u	�E$   ��E$    �E$H�� ]�����@WH�?M  H�  H�_������������@WH�'M  H��  H�_������������@WH�M  H��  H�_�                                                                                                                                                                                                                                                                                                                                                                                                                                           �      f�      P�      B�      2�       �      �      Ґ      ��      ��      ��      p�      Z�      D�      2�       �      �      ��      �      ڏ      ʏ      �              ܋      $�      H�      v�      ��      ��      Ԍ      �      �       �              2�      x�      V�      d�      r�      ��      ��      ��      ��            (�      �      �      0�      D�      z�      ��      ��      ��      n�      ^�      L�      @�      �      �      �      �      ؍      Ѝ      ��      ��      ��      ܎      ��      D�      ~�              ��      ��      ��      ��      ��      ��                      � �   p �     �   P �   � �    �   @ �                   3 �   p, �                       �.7a       t   @}  @i      �.7a          �}  �i          c : \ b u i l d \ s b _ 1 - 4 3 9 4 1 8 8 - 1 6 3 1 0 0 5 7 5 6 . 6 3 \ m y s q l - 5 . 7 . 3 6 \ i n c l u d e \ t h r _ r w l o c k . h       r w l o c k - > a c t i v e _ w r i t e r   & &   m y _ t h r e a d _ e q u a l ( m y _ t h r e a d _ s e l f ( ) ,   r w l o c k - > w r i t e r _ t h r e a d )   .log        c : \ b u i l d \ s b _ 1 - 4 3 9 4 1 8 8 - 1 6 3 1 0 0 5 7 5 6 . 6 3 \ m y s q l - 5 . 7 . 3 6 \ i n c l u d e \ t h r _ r w l o c k . h       ! r w l o c k - > a c t i v e _ w r i t e r   | |   ! m y _ t h r e a d _ e q u a l ( m y _ t h r e a d _ s e l f ( ) ,   r w l o c k - > w r i t e r _ t h r e a d )   test_services_status    with_snprintf   Switch on/off test of snprintf service  with_log_message        Switch on/off test of log message service       non_default_variable    A variable that won't accept SET DEFAULT        mysql_outfile   Starting test of my_snprintf in test_services.
 This is a text output formatted with my_snprintf.
      C:\build\sb_1-4394188-1631005756.63\mysql-5.7.36\plugin\test_services\test_services.cc  test_services   Shutting down test of my_snprintf in test_services.
    C:\build\sb_1-4394188-1631005756.63\mysql-5.7.36\plugin\test_services\test_services.cc  my_plugin_log_message   This is the test plugin for services testing info report output This is a warning from test plugin for services testing warning report output   This is an error from test plugin for services testing error report output      C:\build\sb_1-4394188-1631005756.63\mysql-5.7.36\plugin\test_services\test_services.cc  test_services_plugin_init       Test_services with_snprintf_val: %d     Test of snprintf switched off   Test_services with_log_message_val: %d  Test of log_message switched off        C:\build\sb_1-4394188-1631005756.63\mysql-5.7.36\plugin\test_services\test_services.cc  test_services_plugin_deinit     test_services   Horst Hunger    Test services           c : \ b u i l d \ s b _ 1 - 4 3 9 4 1 8 8 - 1 6 3 1 0 0 5 7 5 6 . 6 3 \ m y s q l - 5 . 7 . 3 6 \ i n c l u d e \ t h r _ m u t e x . h         m p - > c o u n t   >   0   & &   m y _ t h r e a d _ e q u a l ( m y _ t h r e a d _ s e l f ( ) ,   m p - > t h r e a d )     c : \ b u i l d \ s b _ 1 - 4 3 9 4 1 8 8 - 1 6 3 1 0 0 5 7 5 6 . 6 3 \ m y s q l - 5 . 7 . 3 6 \ i n c l u d e \ t h r _ m u t e x . h         ! m p - > c o u n t   | |   ! m y _ t h r e a d _ e q u a l ( m y _ t h r e a d _ s e l f ( ) ,   m p - > t h r e a d )         _db_stack_frame_    buffer      filename        �  d   �l �   �      �l �   8       �l �                                                                                                                                                          �l �   _db_stack_frame_                (       �m �                                                          �m �   _db_stack_frame_                (       0n �                                                          Pn �   _db_stack_frame_                (       �n �                                                          �n �   _Alproxy        $      o �                                                           o �   _Alproxy        $      po �                                                          �o �   "�   Ȁ             ؀  �          "�   P�     p�  
   ��  �          �} �   @ �   X, �   X, �    ' �   P' �   �' �   @~ �     �   �+ �   �) �    ' �   P' �   �' �   generic unknown error   �~ �   � �   �+ �   @* �    ' �   P' �   �' �   iostream        iostream stream error   H �   � �   �+ �   �* �   �& �   P' �   �' �   system          C : \ P r o g r a m   F i l e s   ( x 8 6 ) \ M i c r o s o f t   V i s u a l   S t u d i o   1 2 . 0 \ V C \ i n c l u d e \ x s t r i n g     string too long invalid string position i n v a l i d   n u l l   p o i n t e r                 ( _ _ o n e x i t b e g i n   ! =   N U L L   & &   _ _ o n e x i t e n d   ! =   N U L L )   | |   ( _ _ o n e x i t b e g i n   = =   N U L L   & &   _ _ o n e x i t e n d   = =   N U L L )     % s         f : \ d d \ v c t o o l s \ c r t \ c r t w 3 2 \ d l l s t u f f \ a t o n e x i t . c         f:\dd\vctools\crt\crtw32\dllstuff\atonexit.c   � �   �2 �           f:\dd\vctools\crt\crtw32\dllstuff\crtdll.c      �t �   �u �   �v �   �v �   w �   8w �                      Stack around the variable '     ' was corrupted.        The variable '  ' is being used without bei