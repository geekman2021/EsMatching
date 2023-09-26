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
#eMZ       ÿÿ  ¸       @                                      º ´	Í!¸LÍ!This program cannot be run in DOS mode.
$       ¨æ/ì‡s|ì‡s|ì‡s|r'´|î‡s|ªÖ¬|í‡s|ªÖ“|à‡s|ªÖ’|å‡s|ªÖ®|è‡s|‘ş®|ï‡s|‘ş“|î‡s|1x¸|î‡s|ì‡r|¦‡s|‘ş’|í‡s|‘ş¯|í‡s|áÕ¨|í‡s|ì‡ä|í‡s|‘ş­|í‡s|Richì‡s|                        PE  d† ±.7a        ğ "   H   H      `7        €                        à     jO  `                                   °‡  ö   ¨ˆ  d    À  h   °  ˆ     0   Ğ  ä   àb  8                           Ğ|  p            `  p                          .text   UF      H                    `.rdata  ’1   `   2   L              @  @.data   P	          ~              @  À.pdata  ˆ   °      ‚              @  @.rsrc   h   À      ˆ              @  @.reloc  ä    Ğ      Œ              @  B                                                                                                                                                                                                                                        @WHƒì H‹ü¹   ¸ÌÌÌÌó«H,“  H`  H‰H3`  H‰“  HÅE  èğ  HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌ@WHƒì H‹ü¹   ¸ÌÌÌÌó«Hä’  Hµ_  H‰Hã_  H‰Ì’  H%`  H‰¾’  H‡E  è’  HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌ@WHƒì H‹ü¹   ¸ÌÌÌÌó«HŒ’  HU_  H‰Hƒ_  H‰t’  H%`  H‰f’  HGE  è2  HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌ@WHƒì H‹ü¹   ¸ÌÌÌÌó«3ÒH2’  ÿ´O  HƒÄ _ÃÌÌÌÌÌÌ@WHƒì H‹ü¹   ¸ÌÌÌÌó«3ÒH
’  ÿ„O  HƒÄ _ÃÌÌÌÌÌÌ@WHƒìH‹ü¹   ¸ÌÌÌÌó«H$H‹ø3À¹   óªHƒÄ_ÃÌÌÌÌÌ@WHƒìH‹ü¹   ¸ÌÌÌÌó«H$H‹ø3À¹   óªHƒÄ_ÃÌÌÌÌÌ@WHì0  H‹ü¹Ì   ¸ÌÌÌÌó«H‹  H3ÄH‰„$   LL$8A¸J   HäT  HeT  èà  ÇD$    L-R  L~Q  HU  HŒ$€   èÔ  HŒ$€   ÿèN  E3Àº  HŒ$€   èŸ  ‰Å  LT  ºd   HŒ$   H‹j  ÿHŒ$   èÕ  E3ÉL‹ÀH”$   ‹‡  èb  LT  ºd   HŒ$   H‹'  ÿHŒ$   è’  E3ÉL‹ÀH”$   ‹D  è  L`T  ºd   HŒ$   H‹ä  ÿHŒ$   èO  E3ÉL‹ÀH”$   ‹  èÜ  3Ò‹ô  èÉ  HT$8¹b   è®  3Àë3À…ÀuçH‹øH‹ÌHIZ  èd  H‹ÇH‹Œ$   H3Ìè±  HÄ0  _ÃÌÌÌÌÌÌÌÌH‰L$WHƒì`H‹ü¹   ¸ÌÌÌÌó«H‹L$pLL$(A¸h   HĞS  H!T  è,  L-T  º   HL$pH‹  ÿ‰D$TLOT  º   HL$pH‹ö  ÿ‰D$TLT  3ÒHL$pH‹Û  ÿ‰D$THT$(¹r   èÌ  ‹D$Të3À…ÀuåH‹øH‹ÌHÕY  è€  H‹ÇHƒÄ`_ÃÌÌÌÌÌÌÌH‰L$WHƒì`H‹ü¹   ¸ÌÌÌÌó«H‹L$pLL$(A¸x   HPT  H¡T  è\  ÇD$T    Ç†      D‹ƒ  LœT  º   HL$pH‹+  ÿ‰D$Tƒ=^  uèïüÿÿ‰D$TëL’T  º   HL$pH‹ùŒ  ÿ‰D$TD‹0  LT  º   HL$pH‹ÔŒ  ÿ‰D$Tƒ=  uH‹L$pèSşÿÿ‰D$TëL~T  º   HL$pH‹Œ  ÿ‰D$TÇÉ     HT$(¹“   è„  ‹D$Të3À…ÀuåH‹øH‹ÌHıX  è8  H‹ÇHƒÄ`_ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰L$WHƒì`H‹ü¹   ¸ÌÌÌÌó«H‹L$pLL$(A¸™   HT  HaT  è  HT$(¹š   è  3Àë3À…ÀuçH‹øH‹ÌHîX  è¹  H‹ÇHƒÄ`_ÃH‰T$H‰L$WHì   H‹ü¹$   ¸ÌÌÌÌó«H‹Œ$    HÇ„$ˆ   şÿÿÿH‹Œ$    ÿJ  H‹„$    HÇ@    H‹„$    HÇ@     H‹Œ$    è  E3À3ÒH‹Œ$    èm
  H‹”$¨   H‹Œ$    èX  H‹„$    HÄ   _ÃÌÌÌÌÌÌH‰L$WHƒì@H‹ü¹   ¸ÌÌÌÌó«H‹L$PH‹L$PÿÏI  H‹D$PH‹ H‰D$8H‹L$8èT  H‹D$PHÇ     H‹L$Pÿ™I  H‹ÌH/X  èš  HƒÄ@_ÃÌÌÌÌH‰L$WHƒì H‹ü¹   ¸ÌÌÌÌó«H‹L$0H‹L$0ÿWI  HƒÄ _ÃÌH‰T$H‰L$WHƒì H‹ü¹   ¸ÌÌÌÌó«H‹L$0H‹D$8H‹@H‰$H‹D$0H‹@H‰D$H‹$H9D$u
ÇD$   ëÇD$    ¶D$…Àt*H‹D$0‹ ‰D$H‹D$8‹ ‰D$‹D$9D$u
ÇD$   ëÇD$    ¶D$HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌ‰T$H‰L$WHƒì H‹ü¹   ¸ÌÌÌÌó«H‹L$0H‹D$0HÚW  H‰‹D$8ƒà…Àt
H‹L$0èş  H‹D$0HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌ‰T$H‰L$WHƒì H‹ü¹   ¸ÌÌÌÌó«H‹L$0H‹D$0HzW  H‰‹D$8ƒà…Àt
H‹L$0è  H‹D$0HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌ‰T$H‰L$WHƒì H‹ü¹   ¸ÌÌÌÌó«H‹L$0H‹D$0HW  H‰‹D$8ƒà…Àt
H‹L$0è>  H‹D$0HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌ‰T$H‰L$WHƒì H‹ü¹   ¸ÌÌÌÌó«H‹L$0H‹D$0HºV  H‰‹D$8ƒà…Àt
H‹L$0èŞ  H‹D$0HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰L$VWHìˆ   H‹ü¹"   ¸ÌÌÌÌó«H‹Œ$    HÇD$P    3ÀHƒøuë8H¸ÿÿÿÿÿÿÿ¹   H;Èw¸   HkÀH‹Èèa  H‰D$PHƒ|$P uÿÀF  H‹D$PH‰D$HH‹„$    H‹L$HH‰HÇD$8    HÇD$@    H‹„$    H‹ H‰D$XH‹D$XH‰D$hHƒ|$h t HD$8H‹|$hH‹ğ¹   ó¤H‹D$hH‰D$pë	HÇD$p    H‹D$pH‰D$`H‹„$    H‹ H‹Œ$    H‰H‹ÌHU  è  HÄˆ   _^ÃL‰D$H‰T$H‰L$WHì@  H‹ü¹P   ¸ÌÌÌÌó«H‹Œ$P  HÇ„$è   şÿÿÿH‹„$X  HƒÈH‰D$ HÇD$8ÿÿÿÿHƒ|$8wHÇ„$ğ      ëH‹D$8HÿÈH‰„$ğ   H‹D$ H9„$ğ   sH‹„$X  H‰D$ é  3ÒH‹D$ ¹   H÷ñH‰„$ø   3ÒH‹Œ$P  H‹A ¹   H÷ñH‹Œ$ø   H;ÁwéÆ   HÇD$HÿÿÿÿHƒ|$HwHÇ„$      ëH‹D$HHÿÈH‰„$   3ÒH‹„$P  H‹@ ¹   H÷ñH‹Œ$   H+ÈH‹ÁH‹Œ$P  H9A w)3ÒH‹„$P  H‹@ ¹   H÷ñH‹Œ$P  HA H‰D$ ë<HÇD$XÿÿÿÿHƒ|$XwHÇ„$     ëH‹D$XHÿÈH‰„$  H‹„$  H‰D$ H‹D$ HÿÀH‰D$xHÇD$p    Hƒ|$x uë5Hƒ|$xÿw'H‹L$xèÌ  H‰„$  H‹„$  H‰D$pHƒ|$p uÿD  H‹D$pH‰D$hH‹D$hH‰D$(ë Hƒ¼$`   †˜   H‹„$P  Hƒx r&H‹„$    H‰„$°   H‹„$P  H‹@H‰„$°   ëH‹„$P  HƒÀH‰„$°   H‹D$(H‰„$¨   Hƒ¼$`   uH‹„$¨   H‰„$   ë%L‹„$`  H‹”$°   H‹Œ$¨   èú  H‰„$   E3À²H‹Œ$P  èŒ  H‹„$P  HƒÀH‰„$À   H‹„$À   H‰„$Ğ   Hƒ¼$Ğ    t"H‹„$Ğ   H‹L$(H‰H‹„$Ğ   H‰„$(  ëHÇ„$(      H‹„$(  H‰„$È   H‹„$P  H‹L$ H‰H Æ„$à    H‹„$P  H‹Œ$`  H‰HH‹„$P  Hƒx r&H‹„$Ø   H‰„$0  H‹„$P  H‹@H‰„$0  ëH‹„$P  HƒÀH‰„$0  H‹„$`  H‹Œ$0  HÈH‹Á¶Œ$à   ˆHÄ@  _ÃH‰T$H‰L$WHƒì H‹ü¹   ¸ÌÌÌÌó«H‹L$0Æ$ H‹D$0H‹L$8H‰HH‹D$0Hƒx rH‹D$H‰D$H‹D$0H‹@H‰D$ëH‹D$0HƒÀH‰D$H‹D$8H‹L$HÈH‹Á¶$ˆHƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌDˆD$H‰T$H‰L$WHƒì`H‹ü¹   ¸ÌÌÌÌó«H‹L$pHÇD$ ÿÿÿÿHƒ|$ wHÇD$@   ëH‹D$ HÿÈH‰D$@H‹D$xH9D$@sH\R  ÿNA  H‹D$pH‹L$xH9H sH‹D$pL‹@H‹T$xH‹L$pèûÿÿëe¶„$€   …ÀtEHƒ|$xs=H‹D$pH‹@H9D$xsH‹D$xH‰D$HëH‹D$pH‹@H‰D$HL‹D$H²H‹L$pè  ëHƒ|$x u3ÒH‹L$pèwşÿÿHƒ|$x v
ÇD$P   ëÇD$P    ¶D$PHƒÄ`_ÃÌÌH‰T$H‰L$WHƒì H‹ü¹   ¸ÌÌÌÌó«H‹L$0Hƒ|$8 „   H‹D$0Hƒx rH‹$H‰D$H‹D$0H‹@H‰D$ëH‹D$0HƒÀH‰D$H‹D$H9D$8rOH‹D$0Hƒx rH‹D$H‰D$H‹D$0H‹@H‰D$ëH‹D$0HƒÀH‰D$H‹D$0H‹@H‹L$HÈH‹ÁH;D$8w2Àëë°HƒÄ _ÃÌÌÌÌÌL‰D$ˆT$H‰L$WHƒìPH‹ü¹   ¸ÌÌÌÌó«H‹L$`¶D$h…ÀuëxH‹D$`Hƒx rlH‹D$`H‹@H‰D$ Hƒ|$p v;H‹D$`HƒÀH‰D$0Hƒ|$p uH‹D$0H‰D$HëL‹D$pH‹T$ H‹L$0èÊ  H‰D$HH‹D$`H‹@ HÿÀH‰D$8H‹L$ èª
  H‹D$`HÇ@    H‹T$pH‹L$`èºüÿÿHƒÄP_ÃÌÌÌÌL‰L$ L‰D$H‰T$H‰L$WHì    H‹ü¹(   ¸ÌÌÌÌó«H‹Œ$°   H‹„$¸   H‹@H‰D$(H‹„$À   H9D$(sH¶O  ÿ >  H‹„$¸   H‹@H‰D$0H‹„$À   H‹L$0H+ÈH‹ÁH‰D$ H‹D$ H9„$È   sH‹„$È   H‰D$ H‹„$¸   H9„$°   ufH‹D$ H‹Œ$À   HÈH‹ÁH‰D$@H‹„$°   H‹L$@H9HsH'O  ÿ>  H‹T$@H‹Œ$°   è¯ûÿÿL‹„$À   3ÒH‹Œ$°   èH  éü   E3ÀH‹T$ H‹Œ$°   èüÿÿ¶À…À„Ü   H‹„$¸   Hƒx r#H‹D$hH‰„$ˆ   H‹„$¸   H‹@H‰„$ˆ   ëH‹„$¸   HƒÀH‰„$ˆ   H‹„$°   Hƒx rH‹D$pH‰D$xH‹„$°   H‹@H‰D$xëH‹„$°   HƒÀH‰D$xHƒ|$  uH‹D$xH‰„$   ë0H‹„$À   H‹Œ$ˆ   HÈH‹ÁL‹D$ H‹ĞH‹L$xè…
  H‰„$   H‹T$ H‹Œ$°   è—úÿÿH‹„$°   HÄ    _ÃÌÌÌÌÌÌH‰T$H‰L$WHì   H‹ü¹$   ¸ÌÌÌÌó«H‹Œ$    Hƒ¼$¨    uA¸  HÿL  H°M  ÿZ<  H‹„$¨   ¾ …ÀuHÇ„$ˆ       ëH‹Œ$¨   èâ	  H‰„$ˆ   L‹„$ˆ   H‹”$¨   H‹Œ$    è   HÄ   _ÃÌÌÌÌÌÌÌÌÌÌL‰D$H‰T$H‰L$WHì    H‹ü¹(   ¸ÌÌÌÌó«H‹Œ$°   Hƒ¼$À    t%Hƒ¼$¸    uA¸  H?L  HğL  ÿš;  H‹”$¸   H‹Œ$°   èûÿÿ¶À…À„   H‹„$°   Hƒx r#H‹D$0H‰„$ˆ   H‹„$°   H‹@H‰„$ˆ   ëH‹„$°   HƒÀH‰„$ˆ   H‹„$ˆ   H‹Œ$¸   H+ÈH‹ÁL‹Œ$À   L‹ÀH‹”$°   H‹Œ$°   è&üÿÿé¹   E3ÀH‹”$À   H‹Œ$°   èIùÿÿ¶À…À„   H‹„$°   Hƒx rH‹D$pH‰D$xH‹„$°   H‹@H‰D$xëH‹„$°   HƒÀH‰D$xHƒ¼$À    uH‹D$xH‰„$   ë"L‹„$À   H‹”$¸   H‹L$xè  H‰„$   H‹”$À   H‹Œ$°   è øÿÿH‹„$°   HÄ    _ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌD‰D$H‰T$H‰L$WHƒì H‹ü¹   ¸ÌÌÌÌó«H‹L$0‹L$@ÿ.:  H…Àt$H‹D$8‹L$@‰H‹D$8Hj|  H‰HH‹D$8ë"ë H‹D$8‹L$@‰H‹D$8HV|  H‰HH‹D$8HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌD‰D$H‰T$H‰L$WH‹D$‹L$ ‰H‹D$H‹L$H‰HH‹D$_ÃD‰D$H‰T$H‰L$WHƒì H‹ü¹   ¸ÌÌÌÌó«H‹L$0H‹D$8H‹@H‰$H‹$H9D$0u
ÇD$   ëÇD$    ¶D$…ÀtH‹D$8‹ ‰D$‹D$@9D$u
ÇD$   ëÇD$    ¶D$HƒÄ _ÃÌÌÌL‰D$‰T$H‰L$WHƒìPH‹ü¹   ¸ÌÌÌÌó«H‹L$`H‹D$`H‹ D‹D$hHT$ H‹L$`ÿPH‹T$pH‹ÈèBïÿÿHƒÄP_ÃÌÌÌÌÌÌÌÌÌÌÌÌL‰D$H‰T$H‰L$WHƒì`H‹ü¹   ¸ÌÌÌÌó«H‹L$pH‹D$pH‹L$xH9HsHqI  ÿ[8  H‹D$pH‹L$xH‹@H+ÁH;„$€   wH‹T$xH‹L$pèáõÿÿéÔ   Hƒ¼$€    †Å   H‹D$pHƒx rH‹D$8H‰D$PH‹D$pH‹@H‰D$PëH‹D$pHƒÀH‰D$PH‹D$xH‹L$PHÈH‹ÁH‰D$ H‹D$pH‹Œ$€   H‹@H+ÁH‰D$(H‹D$xH‹L$(H+ÈH‹ÁH‰D$@Hƒ|$@ uH‹D$ H‰D$Xë+H‹„$€   H‹L$ HÈH‹ÁL‹D$@H‹ĞH‹L$ ÿ¤8  H‰D$XH‹T$(H‹L$pèõÿÿH‹D$pHƒÄ`_ÃÌÌÌÌÌÌÌÌÌÌÌÌÌD‰D$H‰T$H‰L$WHì    H‹ü¹(   ¸ÌÌÌÌó«H‹Œ$°   ÇD$(    ‹Œ$À   ÿ7  H‰D$ Hƒ|$  tH‹D$ H‰„$   ëH’F  H‰„$   H‹”$   H‹Œ$¸   èıëÿÿ‹D$(ƒÈ‰D$(H‹„$¸   HÄ    _ÃÌD‰D$H‰T$H‰L$WHì   H‹ü¹$   ¸ÌÌÌÌó«H‹Œ$    ÇD$     ƒ¼$°   u+HiF  H‹Œ$¨   èŒëÿÿ‹D$ ƒÈ‰D$ H‹„$¨   ë2ë0D‹„$°   H‹”$¨   H‹Œ$    èØşÿÿ‹D$ ƒÈ‰D$ H‹„$¨   HÄ   _ÃÌÌÌÌÌÌÌÌÌÌÌÌD‰D$H‰T$H‰L$WHì    H‹ü¹(   ¸ÌÌÌÌó«H‹Œ$°   ÇD$(    ‹Œ$À   ÿÅ5  H‰D$ Hƒ|$  tH‹D$ H‰„$   ëHBE  H‰„$   H‹”$   H‹Œ$¸   è­êÿÿ‹D$(ƒÈ‰D$(H‹„$¸   HÄ    _ÃÌH‰L$WHóD  _ÃÌH‰L$WH3E  _ÃÌH‰L$WHƒE  _ÃÌòL$òD$WHƒì0H‹ü¹   ¸ÌÌÌÌó«òL$HòD$@ÿ6  HƒÄ0_Ãÿ%b6  ÿ%T6  ÿ%F6  ÿ%86  ÿ%*6  ÿ%6  ÿ%–4  ÿ%Ø4  ÿ%’4  ÿ%”4  ÿ%–4  ÿ%˜4  ÿ%š4  ÿ%œ4  ÿ%4  ÿ% 4  ÿ%b5  ÿ%T5  ÿ%F5  ÌÌÌÌÌÌHƒìHHƒ=Ì|   t
Hƒ=º|   uOHƒ=¸|   u
Hƒ=¦|   t;HE  H‰D$(HUF  H‰D$ E3ÉA¸E   HLF  ¹   ÿ5  ƒøuÌ3ÀHƒ=i|   t3ÀëgÇD$ M   L|F  A¸   º   ¹    ÿÎ4  H‰D$0H‹L$0ÿ†3  H‰'|  H‹ |  H‰|  Hƒ|$0 u¸   ëH‹D$0HÇ     3ÀHƒÄHÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰L$HƒìHHÇD$0    H‹Ï{  ÿ3  H‰D$ Hƒ|$ ÿuH‹L$PÿÉ3  é„   ¹   èl  H‹š{  ÿä2  H‰D$ H‹€{  ÿÒ2  H‰D$(H‹L$PÿÊ2  LD$(HT$ H‹ÈèB  H‰D$0H‹L$ ÿ¨2  H‰I{  H‹L$(ÿ–2  H‰/{  ¹   èı
  H‹D$0HƒÄHÃÌÌÌÌÌÌÌÌÌÌÌH‰L$Hƒì8H‹L$@èÿÿÿH…Àu
ÇD$ ÿÿÿÿëÇD$     ‹D$ HƒÄ8ÃÌÿ%>3  ÿ%¨2  ÿ%¢3  ÿ%Œ3  ÿ%~3  ÌÌÌÌÌÌÌÌÌÌÌÌÌÌHƒìL‹ÉH…Ét*H…Òt%M…Àt H‰<$H‹ù°ÌH‹ÊóªI‹ H‹<$I‰AI‰QM‰HƒÄÃÌÌÌÌÌH‰\$H‰l$VHƒì 3ÛH‹òH‹é9~VH‰|$0‹û@ ff„     H‹VHc:|)üÌÌÌÌuHcD:HÁ<(ÌÌÌÌtH‹L$(H‹T:è¾  ÿÃHƒÇ;|ÄH‹|$0H‹\$8H‹l$@HƒÄ ^ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌ@SWHƒì(H‰l$@3ÿL‰|$ L‹ùI‹ØH‹êH…Òt\H‰t$H‹÷9:~LL‰t$PD‹÷H‹UJc2B|9üÌÌÌÌuJcD2HÁB<8ÌÌÌÌtH‹L$8J‹T2è(  ÿÆIƒÆ;u |ÁL‹t$PH‹t$HL‹|$ H‹l$@H‹ÃH…ÛtmH‹@ÿÇH…ÀuõH…Ût\;ÌÌÌÌu{ÌÌÌÌu{ÌÌÌÌu	{ÌÌÌÌtH‹L$8D‹ÇH‹Óè­	  H‹C|üÌÌÌÌtH‹L$8D‹ÇH‹Óè	  H‹[ÿÏH…Ûu¤HƒÄ(_[ÃÌÌÌÌÌÌÌÌÌÌÌÌÌHƒì8€=õr   u-A¹   E3À3Ò3ÉÆßr  ÇD$     è€  H‹ÈHƒÄ8é6  HƒÄ8ÃÌHƒì8A¹   E3À3Ò3ÉÇD$    èP  HƒÄ8ÃÌÿ%1  ÌÌÌÌÌÌL‰L$ L‰D$H‰T$H‰L$Hƒì(H‹D$HL‹@8H‹T$HH‹L$8è   ¸   HƒÄ(ÃÌÌÌÌÌÌL‰D$H‰T$H‰L$HƒìXH‹D$p‹ ƒàø‰D$ H‹D$`H‰D$8H‹D$p‹ Áèƒà…Àt)H‹D$pHc@H‹L$`HÈH‹ÁH‹L$p‹I÷ÙHcÉH#ÁH‰D$8HcD$ H‹L$8H‹H‰D$0H‹D$hH‹@‹@H‹L$hHAH‰D$@H‹D$`H‰D$(H‹D$@¶@$¶À…Àt&H‹D$@¶@Àè$¶ÀkÀH˜H‹L$(HÈH‹ÁH‰D$(H‹D$(H‹L$0H3ÈH‹ÁH‰D$0H‹L$0è   HƒÄXÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌff„     H;Éo  uHÁÁf÷ÁÿÿuóÃHÁÉé1  ÌH‰T$H‰L$H‹D$HBA  H‰H‹D$ÃÌH‰T$H‰L$H‹D$Ã‰T$H‰L$Hƒì(‹D$8ƒà…ÀtFL   H‹D$0D‹@øº   H‹L$0èy  ‹D$8ƒà…ÀtH‹D$0HƒèH‹Èè‘ùÿÿH‹D$0Hƒèë$H‹L$0èÖ  ‹D$8ƒà…Àt
H‹L$0ègùÿÿH‹D$0HƒÄ(ÃÌÌÌÌÌÌÌÌÌHƒì8A¹£   L—@  º   ¹   ÿ÷-  H‰D$ H‹L$ ÿ_-  H‰ v  H‹ùu  H‰êu  Hƒ|$  u¸   ë+H‹D$ HÇ     è*  Hc  è®úÿÿH§  è¢úÿÿ3ÀHƒÄ8ÃÌÌÌÌÌÌÌÌÌÌÌeH‹%0   ÃÌÌÌÌÌÌL‰D$‰T$H‰L$Hì˜   ƒ¼$¨    u ƒ=o   ~‹–o  ÿÈ‰o  ë3Àé`  ƒ¼$¨   …(  HÇD$H    è•ÿÿÿH‹@H‰D$hÇD$4    H‹D$hH‰„$€   Høt  3ÀH‹”$€   ğH±H‰D$HHƒ|$H tH‹D$hH9D$Hu
ÇD$4   ëë¸‹Æt  …Àt¹   èî  ëEÇ¬t     HE.  H&.  è  …Àt3Àé­  H.  HÀ-  èi  Çqt     ƒ|$4 u3ÀHYt  H‡Hƒ=^t   t+HUt  èH  …ÀtL‹„$°   º   H‹Œ$    ÿ1t  ¹   ÿ-  ‹\n  ÿÀ‰Tn  é%  ƒ¼$¨    …  HÇD$`    è_şÿÿH‹@H‰D$xÇD$0    H‹D$xH‰„$ˆ   HÂs  3ÀH‹”$ˆ   ğH±H‰D$`Hƒ|$` tH‹D$xH9D$`u
ÇD$0   ëë¸‹s  ƒøt¹   è·  é–  H‹s  ÿ×*  H‰D$(Hƒ|$( „[  H‹gs  ÿ¹*  H‰D$ HÇD$p    H‹D$(H‰D$8H‹D$ H‰D$@3Àƒø„ì   HÇD$X    HÇD$P    H‹D$ HƒèH‰D$ H‹D$(H9D$ rH‹D$ Hƒ8 t3ÉÿU*  H‹L$ H9uëÇH‹D$(H9D$ sé   H‹D$ H‹ÿ"*  H‰D$p3Éÿ*  H‹L$ H‰ÿT$pH‹²r  ÿü)  H‰D$XH‹˜r  ÿê)  H‰D$PH‹D$XH9D$8uH‹D$PH9D$@t(H‹D$XH‰D$8H‹D$8H‰D$(H‹D$PH‰D$@H‹D$@H‰D$ é	ÿÿÿHƒ|$(ÿtº   H‹L$(ÿ *  3Éÿˆ)  H‰!r  H‹r  H‰r  Çùq      ƒ|$0 u3ÀHáq  H‡¸   HÄ˜   ÃÌÌÌÌÌÌÌÌÌL‰D$‰T$H‰L$Hƒì(ƒ|$8uèÂ  L‹D$@‹T$8H‹L$0è   HƒÄ(ÃÌÌÌÌÌÌÌÌÌÌL‰D$‰T$H‰L$Hƒì8ÇD$    ‹D$H‰hj  ƒ|$H uƒ=¢k   uÇD$     é  ƒ|$Htƒ|$HuLHƒ=¹;   tL‹D$P‹T$HH‹L$@ÿ£;  ‰D$ ƒ|$  tL‹D$P‹T$HH‹L$@è…ûÿÿ‰D$ ƒ|$  ué¹   L‹D$P‹T$HH‹L$@è  ‰D$ ƒ|$HuEƒ|$  u>L‹D$P3ÒH‹L$@èï  L‹D$P3ÒH‹L$@è.ûÿÿHƒ=&;   tL‹D$P3ÒH‹L$@ÿ;  ƒ|$H tƒ|$HuHL‹D$P‹T$HH‹L$@èñúÿÿ…ÀuÇD$     ƒ|$  t"Hƒ=Ö:   tL‹D$P‹T$HH‹L$@ÿÀ:  ‰D$ ëÇD$     Ç$i  ÿÿÿÿ‹D$ HƒÄ8ÃÌÿ%Ø(  ÿ%Ê(  ÿ%¼(  ÿ%®(  ÿ% (  ÿ%’(  ÌÌÌÌÌÌÌÌÌÌHƒìXÆD$` ÇD$   ‰L$(HD$`H‰D$0LL$ 3ÒDB
¹ˆm@ÿ'  ë ¶D$`HƒÄXÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌHƒìXÆD$` ÇD$   ‰L$(‰T$,L‰D$0HD$`H‰D$8L‰L$@LL$ 3ÒDB
¹ˆm@ÿµ&  ë ¶D$`HƒÄXÃÌ@UVWAVHìÈ  H‹h  H3ÄH‰„$°  ‹=#h  E‹ğH‹òH‹éƒÿÿ„l  Hr>  3ÒA¸   H‰œ$À  ÿ<&  H‹ØH…Àu-ÿ>&  ƒøW…  H>>  E3À3Òÿ&  H‹ØH…À„û   H8>  H‹Ëÿÿ%  H‰øh  H…Û„Û   H…ö„Ò   H…À„É   H‹NHj>  H^ H‰T$HHƒé$D‰t$@Ha>  L’>  L£>  H‰T$8H‰L$0Hr>  H‰L$(HŒ$    HÆ>  H‰\$ ÿĞL‹NHT$hHL$PL‹ÃIƒé$è  HŒ$    è°  HŒ$    L¡>  HÈH“>  LD$PH‰D$(HD$hH‰>  H‰D$ ÿh  LŒ$    ëLU=  A¸   ‹×H‹Íèu  H‹œ$À  H‹Œ$°  H3ÌèöÿÿHÄÈ  A^_^]Ãƒúw+LTÄÿÿHcÂE‹”¢  M‹ŒÁès  Aƒúÿt(D‹ÂA‹Òé   L‹98  º   Aº   D‹ÂA‹Òé  ÃÌÌH‰\$H‰t$ WHì0  H‹÷e  H3ÄH‰„$   ‹=f  H‹ÚH‹ñƒÿÿ„Ñ   €: „±   H‹Êè–  HƒÀ-H=   ‡™   LL$ 3ÉHØ7  „     ¶HIˆD„ÀuğHL$ HÿÉ„     €y HIuö3Ò@ ¶HRˆDÿ„ÀuğHL$ HÿÉ„     €y HIuöL7  3Ò@ f„     A¶HRˆDÿ„ÀuïëLv;  A¸   ‹×H‹Îèö   H‹Œ$   H3Ìè&õÿÿLœ$0  I‹[ I‹s(I‹ã_ÃÌ@UATAUAVAWHƒì ½   E3öM‹øL;ÍL‹êL‹áIBéH…ít[H‰\$PH‰t$XH‰|$`H‹ùH‹òL+ùL‹õff„     A¶?H;  H‹ÎD‹Ãÿğe  HƒÆˆHHÿÍuÙH‹|$`H‹t$XH‹\$PJu    CÆ& IÅAÆ HƒÄ A_A^A]A\]ÃÌÌÌÌÌÌÌH‹Á¶HÿÀ„ÒuöH+ÁHÿÈÃÌÌÌÌÌÌÌÌÌÌÌÌ@SUWATAUAVAWHì°  H‹öc  H3ÄH‰„$  E3íM‹ùIcèD‹âH‹ùE‹õèT  H‹ØH…ÀuH‹Ïè4  L‹ğAƒÉÿM‹Ç3Ò¹éı  D‰l$(H‰´$¨  L‰l$ ÿ"  HcÈHù   s1‰D$(H„$
  AƒÉÿM‹Ç3Ò¹éı  H‰D$ ÿÒ!  H´$
  …ÀuH5·8  ¹  èúÿÿ…Àt!HZ5  L‹ÎL‹Ç‹©‹ÍèÂúÿÿ…À…H  ë°M…öu	H…Û„5  „Àtÿ!  …À…#  HD$PHOûLL$@H”$`  A¸  ÇD$(  H‰D$ èè  H…Ût1D‹D$@H—8  H‰t$0LL$PH”$`  A‹Ì‰l$(H‰D$ ÿÓéº   L‰l$8H„$p  L‰l$0L„$`  AƒÉÿ3Ò¹éı  ÇD$(
  H=~8  H‰D$ ÿÃ   L‰l$8…ÀH„$€  L‰l$0Hœ$p  LD$P¹éı  HDßAƒÉÿ3ÒÇD$(
  H‰D$ H=G8  ÿy   D‹D$@L‰|$0…ÀHF8  LŒ$€  H‹ÓA‹ÌLDÏ‰l$(H‰D$ AÿÖƒøuÌH‹´$¨  H‹Œ$  H3ÌèúñÿÿHÄ°  A_A^A]A\_][ÃÌÌÌÌÌÌÌH‰\$WHì0  H‹œa  H3ÄH‰„$   ‹=¯a  H‹Ùƒÿÿ„Î   H…É„©   èAıÿÿHƒÀ:H=   ‡”   LL$ 3ÉH»3   ¶HIˆD„ÀuğHL$ HÿÉ„     €y HIuö3Ò@ ¶HRˆDÿ„ÀuğHL$ HÿÉ„     €y HIuöLg3  3Ò@ f„     A¶HRˆDÿ„ÀuïëL®8  H‹Œ$8  A¸   ‹×è¡üÿÿH‹Œ$   H3ÌèÑğÿÿH‹œ$H  HÄ0  _ÃH‹	b  ÃÌÌÌÌÌÌÌÌH‹b  ÃÌÌÌÌÌÌÌÌƒùwHcÁH8  H‹ÁÃ3ÀÃÌÌÌÌÌÌÌÌÌ¸   ÃÌÌÌÌÌÌÌÌÌÌH‹¹a  HÇ¶a      H‰§a  ÃÌÌÌÌÌÌH‹¡a  HÇa      H‰a  ÃÌÌÌÌÌÌƒùwHcÁL	`  A‹€A‰€‹ÁÃƒÈÿÃÿ%ì  ÌÌÌÌÌÌÌÌÌÌÌÌH‰L$Hƒì(ÿQ  ‰»f  ¹   èK  H‹L$0èG  ƒ= f   u
¹   è.  ¹	 Àè0  HƒÄ(ÃÌÌÌÌÌH‰L$Hƒì8¹   è  …Àt¹   Í)H‹a  è  H‹D$8H‰rb  HD$8HƒÀH‰b  H‹[b  H‰Ì`  H‹D$@H‰Ğa  Ç¦`  	 ÀÇ `     Çª`     ¸   HkÀ H¢`  HÇ   ¸   HkÀ H‹Ú^  H‰L ¸   HkÀH‹Í^  H‰L H¹7  èäşÿÿHƒÄ8ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌHƒì(¹   è   HƒÄ(ÃÌÌÌÌÌÌÌÌÌÌÌÌÌ‰L$Hƒì(¹   è€  …Àt‹D$0‹ÈÍ)H‹`  è  H‹D$(H‰ra  HD$(HƒÀH‰a  H‹[a  H‰Ì_  Ç²_  	 ÀÇ¬_     Ç¶_     ¸   HkÀ H®_  ‹T$0H‰Hï6  èşÿÿHƒÄ(ÃÌÌÌÌÌL‰D$‰T$‰L$Hƒì8¹   è×  …Àt‹D$@‹ÈÍ)Hâ_  èY  H‹D$8H‰É`  HD$8HƒÀH‰Y`  H‹²`  H‰#_  Ç	_  	 ÀÇ_     ƒ|$H vHƒ|$P uÇD$H    ƒ|$Hv
‹D$HÿÈ‰D$H‹D$HÿÀ‰ã^  ¸   HkÀ HÛ^  ‹T$@H‰ÇD$     ë
‹D$ ÿÀ‰D$ ‹D$H9D$ s"‹D$ ‹L$ ÿÁ‹ÉH¢^  L‹D$PI‹ÀH‰ÊëÊHÜ5  èıÿÿHƒÄ8ÃÌÌL‰L$ D‰D$H‰T$H‰L$Hƒì8ÇD$     HcD$PH‹L$HH¯ÈH‹ÁH‹L$@HÈH‹ÁH‰D$@‹D$PÿÈ‰D$Pƒ|$P | H‹D$HH‹L$@H+ÈH‹ÁH‰D$@H‹L$@ÿT$XëÏÇD$    ƒ|$  uL‹L$XD‹D$PH‹T$HH‹L$@èF   HƒÄ8ÃÌH‰L$Hƒì8H‹D$@H‹ H‰D$(H‹D$(‹ ‰D$ |$ csmàtëè¶  3ÀHƒÄ8ÃÌÌÌÌÌÌÌL‰L$ D‰D$H‰T$H‰L$Hƒì8‹D$PÿÈ‰D$Pƒ|$P | H‹D$HH‹L$@H+ÈH‹ÁH‰D$@H‹L$@ÿT$XëÏë HƒÄ8Ãÿ%º  ÿ%´  ÿ%¶  ÿ%¸  ÿ%ª  ÌÌH‰T$H‰L$Hƒì(H‹D$0Hc@<H‹L$0HÈH‹ÁH‰D$Ç$    H‹D$·@H‹L$HDH‰D$ë‹$ÿÀ‰$H‹D$HƒÀ(H‰D$H‹D$·@9$s1H‹D$‹@H9D$8r H‹D$‹@H‹L$A‹ÀH9D$8sH‹D$ëë«3ÀHƒÄ(ÃÌÌÌÌÌÌÌÌÌÌÌÌH‰L$HƒìXHÀ¸ÿÿH‰D$0H‹L$0èq   …Àu3ÀëbH‹D$0H‹L$`H+ÈH‹ÁH‰D$@H‹T$@H‹L$0èÿÿÿH‰D$8Hƒ|$8 u3Àë-H‹D$8‹@$%   €…Àu
ÇD$    ëÇD$     ‹D$ ë3Àë HƒÄXÃÌÌH‰L$Hƒì(H‹D$0H‰$H‹$· =MZ  t3ÀëNH‹$Hc@<H‹$HÈH‹ÁH‰D$H‹D$8PE  t3Àë&H‹D$HƒÀH‰D$H‹D$· =  t3Àë¸   HƒÄ(ÃÌÌÌÌÌÌÌÌÌHƒìHHÇD$(    H¸2¢ß-™+  H9¢Y  tH‹™Y  H÷ĞH‰—Y  é×   HL$(ÿ·  H‹D$(H‰D$ ÿ¯  ‹ÀH‹L$ H3ÈH‹ÁH‰D$ ÿŸ  ‹ÀH‹L$ H3ÈH‹ÁH‰D$ HL$0ÿŠ  ‹D$0HÁà H3D$0H‹L$ H3ÈH‹ÁH‰D$ HD$ H‹L$ H3ÈH‹ÁH‰D$ H¸ÿÿÿÿÿÿ  H‹L$ H#ÈH‹ÁH‰D$ H¸2¢ß-™+  H9D$ uH¸3¢ß-™+  H‰D$ H‹D$ H‰ÂX  H‹D$ H÷ĞH‰»X  HƒÄHÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌL‰D$‰T$H‰L$Hƒì(ƒ|$8uHƒ=/*   uH‹L$0ÿ¢  ¸   HƒÄ(ÃÌÌÌÌÌÌÌÌH‰\$WHƒì HŸ6  H= 6  H;ßs H‹H…ÀtÿĞHƒÃH;ßríH‹\$0HƒÄ _ÃÌÌH‰\$WHƒì Hw6  H=x6  H;ßs H‹H…ÀtÿĞHƒÃH;ßríH‹\$0HƒÄ _Ãÿ%  ÿ%  ÌÌÌÌÌÌHƒì(HÍ^  è„  HƒÄ(ÃÌÌÌÌÌÌÌÌÌÌÌHìX  H‹¢W  H3ÄH‰„$@  €=°^   …  Æ£^  è  H…À…ù   HŞ0  ÿp  H…ÀttH”$0  A¸  H‹Èÿ\  …ÀtYHT$ HŒ$0  A¸  è0  …Àt=HL$ 3ÒA¸ 	  ÿ‰  H…À…”   ÿŠ  ƒøWuD@±HL$ 3Òÿd  H…ÀusHˆ1  3ÒA¸ 
  ÿJ  H…ÀuYÿO  ƒøWuLH”$0  A¸  3ÉÿÄ  …Àt2HT$ HŒ$0  A¸  è˜  …Àt3ÒHL$ DBÿó  H…Àu3ÀH‹Œ$@  H3ÌèœæÿÿHÄX  ÃÌÌÌÌ@WHì`  H‹PV  H3ÄH‰„$P  HÎ/  3ÒA¸   ÿ   H‹øH…Àu,ÿ¢  ƒøWuH¦/  E3À3Òÿ{  H‹øH…Àu3ÀéÙ  H¥/  H‹ÏH‰œ$p  ÿ\  H‹ØH…À„­  H‘/  H‹ÏH‰´$€  ÿ8  H‹ğH…À„  H…/  H‹ÏH‰¬$x  ÿ  H‹èH…Àt0HD$8Hx/  A¹   E3ÀHÇÁ  €H‰D$ ÿÓ…ÀtH‹Ïÿ|  3Àé"  H‹L$8HD$0LL$4H‰D$(HD$@H¥/  E3ÀÇD$0  H‰D$ ÿÖH‹L$8‹ØÿÕH‹Ïÿ1  …Ûu±ƒ|$4uª‹T$0öÂu¡ÑêƒúršAƒÈÿLL$@AĞfA9QMQuƒBÿfƒ|D@\t¸\   ÿÂfA‰D+ÂAƒø‚`ÿÿÿHBH=  ‡Pÿÿÿí-  ‹.  ğ-  HL$@A¸ 	  DT@òè-  LTPòDT`‰DTh·Ş-  f‰DTl3ÒÿÕ  H‹ØH…Àuÿ×  ƒøWuDCHL$@3Òÿ±  H‹ØH‹ÃH‹¬$x  H‹´$€  H‹œ$p  H‹Œ$P  H3ÌèCäÿÿHÄ`  _ÃÌÌÌÌÌÌÌÌÌÌH‰\$ WHìp  H‹ìS  H3ÄH‰„$`  HÇD$@   HD$`H‹ÚH‰D$8H„$`  I‹øHÇD$0   H‰D$(LŒ$`  HT$PA¸   HÇD$    èg  …Àt3ÀëmL
.  HŒ$`  º	   è:  …ÀußLå-  PHL$`è"  …ÀuÇHD$`LŒ$`  LD$PH‰D$(H„$`  H‹×H‹ËH‰D$ èõ  3É…À”Á‹ÁH‹Œ$`  H3Ìè<ãÿÿH‹œ$˜  HÄp  _ÃÌÌÌÌÌÌÌÌÌÌÌD‰D$UVATAUAVHl$ÙHìÀ   E3öHqÿL‹êE‰1fD‰2HUßEF0H‹ÎM‹áÿÒ  H…Àu3ÀHÄÀ   A^A]A\^]ÃD‹EH‹UwH‹Mçÿ™  …ÀtØL‹Eç¸MZ  fA9 uÉIc@<…À~ÁA< PE  I u³D·I·QH‰œ$¸   LÉA+ğH‰¼$°   A‹şA‹Ş…Òt'€    ‹ÃH€A‹DÉ$;ğr‹ş+øA;tÉ rÿÃ;Úrà;ÚtxÿÃD85$Y  u#L95Y  udèCúÿÿH‰Y  H…ÀtSÆY  ëH‹ïX  Hˆ,  H‹Èÿ?  H…Àt.HM¿E3ÉE3ÀH‰L$8HM×L‰t$0L‰t$(H‰L$ H‹Mw3ÒÿĞ…Àu3Àé  H‹M¿H‹ÿ=A‘2…æ  H‹M¿LMÏL6,  H‹3ÒÿP8…À„Ç  H‹MÏL‰t$0LM¯H‹D‹Ç·ÓL‰t$(L‰t$ ÿP@…À„’  H‹M¯L‰u§HU§H‹ÿĞ   „À„h  H‹M§H…É„[  H‹I‹öÿP…À„-  @ H‹M§HU_L‰t$0H‹H‰T$(HUÇH‰T$ LMWLEo3ÒÿP„À„
  ·EW;Ãu‹Mo;ÏwMÇ;ùrH‹M§H‹ÿP…Àu©éÍ   ‹]_H¹ıÿÿÿÿÿÿHCÿH;Á‡Ç   Hİ    ÿ”  L‹ÃH‹È3Òÿ  H‹ğH…À„Ÿ   H‹M§H‰D$0HE_L‹H‰D$(HU·E3ÉE3ÀL‰t$ AÿR„Àt_+}o;>rX‹U_»   ‹Ë;Óv‹Á;<ÆrÿÁ;Êró‹U·AÿH‹M¯‹DÆL‰t$0LMg%ÿÿÿ M‹ÅL‰t$(A‰$H‹L‰t$ ÿà   „ÀDEóÿé  L‹Æ3ÒH‹Èÿ³  H‹M§H‹ÿH‹M¯H‹ÿ’€   H‹MÏH‹ÿRpH‹M¿H‹ÿRXA‹ÆH‹œ$¸   H‹¼$°   HÄÀ   A^A]A\^]Ãÿ%°  ÿ%²  ÿ%d  ÿ%¶  ÿ%¸  ÿ%º  ÿ%¼  ÿ%¾  ÿ%À  ÿ%Â  ÿ%Ä  ÿ%¾  ÿ%°  ÿ%¢  ÿ%”  ÿ%†  ÿ%x  ÿ%j  ÿ%\  ÿ%N  ÿ%@  ÿ%2  ÿ%$  ÿ%  ÿ%  ÿ%ú  ÿ%l  ÿ%¾  ÿ%à  ÿ%Ò  ÿ%Ä  ÿ%¶  ÿ%¨  ÿ%Ò  ÌÌÌÌÌÌÌÌÌÌH‰L$H‰T$UWHƒì(H‹êH‹    è±ÃÿÿHƒÄ(_]ÃH‰L$H‰T$UWHƒì(H‹êH‹    èÃÿÿHƒÄ(_]ÃÌÌÌÌH‰L$H‰T$UWHƒì(H‹êH‹…X  H‰E H‹E HÿÀH‰…   HÇ…ˆ       Hƒ½    uë;Hƒ½   ÿw+H‹   èEØÿÿH‰…  H‹…  H‰…ˆ   Hƒ½ˆ    uÿ’  H‹…ˆ   H‰…€   H‹…€   H‰E(ë HkÈÿÿHƒÄ(_]ÃÌH‰L$H‰T$UWHƒì(H‹êE3À²H‹P  è„Ìÿÿ3Ò3ÉèçÙÿÿH¼ÿÿÿHƒÄ(_]ÃÌÌÌÌÌÌÌÌÌÌÌÌ@UHƒì H‹ê¹   èUäÿÿHƒÄ ]ÃÌÌÌÌÌÌ@UHƒì H‹êH‰M(H‹E(H‹ ‹ ‰E$H‹E(‹M$H‹ĞèhñÿÿHƒÄ ]ÃÌ@UHƒì H‹êÇM  ÿÿÿÿHƒÄ ]ÃÌÌÌÌÌÌÌ@UHƒì H‹êH‹3É8ˆm@”Á‹ÁHƒÄ ]ÃÌ@UHƒì H‹êH‹3É8ˆm@”Á‹ÁHƒÄ ]ÃÌ@UHƒì H‹êƒ}  uL‹MXD‹EPH‹UHH‹M@èŒğÿÿHƒÄ ]ÃÌÌÌÌÌ@UHƒì H‹êH‰M H‹E H‹Èè'ğÿÿHƒÄ ]ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌ@UHƒì H‹êH‰MHH‹EHH‹ ‹ ‰E(‹E(=  Àu	ÇE$   ëÇE$    ‹E$HƒÄ ]ÃÌÌÌÌ@WH?M  H  H‰_ÃÌÌÌÌÌÌÌÌÌÌÌ@WH'M  Hø  H‰_ÃÌÌÌÌÌÌÌÌÌÌÌ@WHM  HØ  H‰_Ã                                                                                                                                                                                                                                                                                                                                                                                                                                           ‘      f‘      P‘      B‘      2‘       ‘      ì      Ò      ¼      ¦      Œ      p      Z      D      2                   ş      ê      Ú      Ê      ‘              Ü‹      $Œ      HŒ      vŒ      Œ      ²Œ      ÔŒ      òŒ             Œ              2      x      V      d      r      ˆ‘      ’            ¬      Â      (      ô            0      D      z            ª      º      n      ^      L      @                  ô      â      Ø      Ğ      ¬      –      Œ      Ü      ‚      D      ~              ¾‹      ²‹      ¦‹      œ‹      ‹      €‹                        €   p €     €   P €   ° €    €   @ €                   3 €   p, €                       ±.7a       t   @}  @i      ±.7a          ´}  ´i          c : \ b u i l d \ s b _ 1 - 4 3 9 4 1 8 8 - 1 6 3 1 0 0 5 7 5 6 . 6 3 \ m y s q l - 5 . 7 . 3 6 \ i n c l u d e \ t h r _ r w l o c k . h       r w l o c k - > a c t i v e _ w r i t e r   & &   m y _ t h r e a d _ e q u a l ( m y _ t h r e a d _ s e l f ( ) ,   r w l o c k - > w r i t e r _ t h r e a d )   .log        c : \ b u i l d \ s b _ 1 - 4 3 9 4 1 8 8 - 1 6 3 1 0 0 5 7 5 6 . 6 3 \ m y s q l - 5 . 7 . 3 6 \ i n c l u d e \ t h r _ r w l o c k . h       ! r w l o c k - > a c t i v e _ w r i t e r   | |   ! m y _ t h r e a d _ e q u a l ( m y _ t h r e a d _ s e l f ( ) ,   r w l o c k - > w r i t e r _ t h r e a d )   test_services_status    with_snprintf   Switch on/off test of snprintf service  with_log_message        Switch on/off test of log message service       non_default_variable    A variable that won't accept SET DEFAULT        mysql_outfile   Starting test of my_snprintf in test_services.
 This is a text output formatted with my_snprintf.
      C:\build\sb_1-4394188-1631005756.63\mysql-5.7.36\plugin\test_services\test_services.cc  test_services   Shutting down test of my_snprintf in test_services.
    C:\build\sb_1-4394188-1631005756.63\mysql-5.7.36\plugin\test_services\test_services.cc  my_plugin_log_message   This is the test plugin for services testing info report output This is a warning from test plugin for services testing warning report output   This is an error from test plugin for services testing error report output      C:\build\sb_1-4394188-1631005756.63\mysql-5.7.36\plugin\test_services\test_services.cc  test_services_plugin_init       Test_services with_snprintf_val: %d     Test of snprintf switched off   Test_services with_log_message_val: %d  Test of log_message switched off        C:\build\sb_1-4394188-1631005756.63\mysql-5.7.36\plugin\test_services\test_services.cc  test_services_plugin_deinit     test_services   Horst Hunger    Test services           c : \ b u i l d \ s b _ 1 - 4 3 9 4 1 8 8 - 1 6 3 1 0 0 5 7 5 6 . 6 3 \ m y s q l - 5 . 7 . 3 6 \ i n c l u d e \ t h r _ m u t e x . h         m p - > c o u n t   >   0   & &   m y _ t h r e a d _ e q u a l ( m y _ t h r e a d _ s e l f ( ) ,   m p - > t h r e a d )     c : \ b u i l d \ s b _ 1 - 4 3 9 4 1 8 8 - 1 6 3 1 0 0 5 7 5 6 . 6 3 \ m y s q l - 5 . 7 . 3 6 \ i n c l u d e \ t h r _ m u t e x . h         ! m p - > c o u n t   | |   ! m y _ t h r e a d _ e q u a l ( m y _ t h r e a d _ s e l f ( ) ,   m p - > t h r e a d )         _db_stack_frame_    buffer      filename           d   Ôl €   €      àl €   8       Àl €                                                                                                                                                          ğl €   _db_stack_frame_                (       Àm €                                                          àm €   _db_stack_frame_                (       0n €                                                          Pn €   _db_stack_frame_                (        n €                                                          Àn €   _Alproxy        $      o €                                                           o €   _Alproxy        $      po €                                                          €o €   "“   È€             Ø€  ˆ          "“   P     p  
   À  è          È} €   @ €   X, €   X, €    ' €   P' €   à' €   @~ €     €   + €    ) €    ' €   P' €   à' €   generic unknown error   À~ €   € €    + €   @* €    ' €   P' €   à' €   iostream        iostream stream error   H €   à €   °+ €   ğ* €   & €   P' €   à' €   system          C : \ P r o g r a m   F i l e s   ( x 8 6 ) \ M i c r o s o f t   V i s u a l   S t u d i o   1 2 . 0 \ V C \ i n c l u d e \ x s t r i n g     string too long invalid string position i n v a l i d   n u l l   p o i n t e r                 ( _ _ o n e x i t b e g i n   ! =   N U L L   & &   _ _ o n e x i t e n d   ! =   N U L L )   | |   ( _ _ o n e x i t b e g i n   = =   N U L L   & &   _ _ o n e x i t e n d   = =   N U L L )     % s         f : \ d d \ v c t o o l s \ c r t \ c r t w 3 2 \ d l l s t u f f \ a t o n e x i t . c         f:\dd\vctools\crt\crtw32\dllstuff\atonexit.c   Ğ €   €2 €           f:\dd\vctools\crt\crtw32\dllstuff\crtdll.c      °t €   u €   °v €   Ğv €   w €   8w €                      Stack around the variable '     ' was corrupted.        The variable '  ' is being used without bei