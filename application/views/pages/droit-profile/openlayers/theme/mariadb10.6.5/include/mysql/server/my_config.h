/* Copyright (c) 2009, 2013, Oracle and/or its affiliates. All rights reserved.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; version 2 of the License.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1335  USA */

#ifndef MY_CONFIG_H
#define MY_CONFIG_H
#define DOT_FRM_VERSION 6
/* Headers we may want to use. */
#define STDC_HEADERS 1
/* #undef _GNU_SOURCE */
/* #undef HAVE_ALLOCA_H */
/* #undef HAVE_ARPA_INET_H */
/* #undef HAVE_ASM_TERMBITS_H */
/* #undef HAVE_CRYPT_H */
/* #undef HAVE_CURSES_H */
/* #undef HAVE_BFD_H */
/* #undef HAVE_NDIR_H */
/* #undef HAVE_DIRENT_H */
/* #undef HAVE_DLFCN_H */
/* #undef HAVE_EXECINFO_H */
#define HAVE_FCNTL_H 1
/* #undef HAVE_FENV_H */
#define HAVE_FLOAT_H 1
/* #undef HAVE_FNMATCH_H */
/* #undef HAVE_FPU_CONTROL_H */
/* #undef HAVE_GRP_H */
/* #undef HAVE_IA64INTRIN_H */
/* #undef HAVE_IEEEFP_H */
/* #undef HAVE_INTTYPES_H */
/* #undef HAVE_KQUEUE */
#define HAVE_LIMITS_H 1
/* #undef HAVE_LINK_H */
/* #undef HAVE_LINUX_UNISTD_H */
/* #undef HAVE_LINUX_MMAN_H */
#define HAVE_LOCALE_H 1
#define HAVE_MALLOC_H 1
#define HAVE_MEMORY_H 1
/* #undef HAVE_NETINET_IN_H */
/* #undef HAVE_PATHS_H */
/* #undef HAVE_POLL_H */
/* #undef HAVE_PWD_H */
/* #undef HAVE_SCHED_H */
/* #undef HAVE_SELECT_H */
/* #undef HAVE_SOLARIS_LARGE_PAGES */
#define HAVE_STDDEF_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STDARG_H 1
/* #undef HAVE_STRINGS_H */
#define HAVE_STRING_H 1
/* #undef HAVE_STDINT_H */
/* #undef HAVE_SYNCH_H */
/* #undef HAVE_SYSENT_H */
/* #undef HAVE_SYS_DIR_H */
/* #undef HAVE_SYS_FILE_H */
/* #undef HAVE_SYS_FPU_H */
/* #undef HAVE_SYS_IOCTL_H */
/* #undef HAVE_SYS_MALLOC_H */
/* #undef HAVE_SYS_MMAN_H */
/* #undef HAVE_SYS_NDIR_H */
/* #undef HAVE_SYS_PTE_H */
/* #undef HAVE_SYS_PTEM_H */
/* #undef HAVE_SYS_PRCTL_H */
/* #undef HAVE_SYS_RESOURCE_H */
/* #undef HAVE_SYS_SELECT_H */
/* #undef HAVE_SYS_SOCKET_H */
/* #undef HAVE_SYS_SOCKIO_H */
/* #undef HAVE_SYS_UTSNAME_H */
#define HAVE_SYS_STAT_H 1
/* #undef HAVE_SYS_STREAM_H */
/* #undef HAVE_SYS_SYSCALL_H */
#define HAVE_SYS_TIMEB_H 1
/* #undef HAVE_SYS_TIMES_H */
/* #undef HAVE_SYS_TIME_H */
#define HAVE_SYS_TYPES_H 1
/* #undef HAVE_SYS_UN_H */
/* #undef HAVE_SYS_VADVISE_H */
/* #undef HAVE_SYS_STATVFS_H */
/* #undef HAVE_UCONTEXT_H */
/* #undef HAVE_TERM_H */
/* #undef HAVE_TERMBITS_H */
/* #undef HAVE_TERMIOS_H */
/* #undef HAVE_TERMIO_H */
/* #undef HAVE_TERMCAP_H */
#define HAVE_TIME_H 1
/* #undef HAVE_UNISTD_H */
/* #undef HAVE_UTIME_H */
#define HAVE_VARARGS_H 1
#define HAVE_SYS_UTIME_H 1
/* #undef HAVE_SYS_WAIT_H */
/* #undef HAVE_SYS_PARAM_H */

/* Libraries */
/* #undef HAVE_LIBWRAP */
/* #undef HAVE_SYSTEMD */
/* #undef HAVE_SYSTEMD_SD_LISTEN_FDS_WITH_NAMES */

/* Does "struct timespec" have a "sec" and "nsec" field? */
/* #undef HAVE_TIMESPEC_TS_SEC */

/* Readline */
/* #undef HAVE_HIST_ENTRY */
/* #undef USE_LIBEDIT_INTERFACE */
/* #undef USE_NEW_READLINE_INTERFACE */

/* #undef FIONREAD_IN_SYS_IOCTL */
/* #undef GWINSZ_IN_SYS_IOCTL */
/* #undef TIOCSTAT_IN_SYS_IOCTL */
/* #undef FIONREAD_IN_SYS_FILIO */

/* Functions we may want to use. */
/* #undef HAVE_ACCEPT4 */
#define HAVE_ACCESS 1
/* #undef HAVE_ALARM */
#define HAVE_ALLOCA 1
/* #undef HAVE_BFILL */
/* #undef HAVE_INDEX */
/* #undef HAVE_CLOCK_GETTIME */
/* #undef HAVE_CRYPT */
/* #undef HAVE_CUSERID */
/* #undef HAVE_DLADDR */
/* #undef HAVE_DLERROR */
/* #undef HAVE_DLOPEN */
/* #undef HAVE_FCHMOD */
/* #undef HAVE_FCNTL */
/* #undef HAVE_FDATASYNC */
/* #undef HAVE_DECL_FDATASYNC */
/* #undef HAVE_FEDISABLEEXCEPT */
/* #undef HAVE_FESETROUND */
/* #undef HAVE_FP_EXCEPT */
/* #undef HAVE_FSEEKO */
/* #undef HAVE_FSYNC */
#define HAVE_FTIME 1
/* #undef HAVE_GETIFADDRS */
#define HAVE_GETCWD 1
/* #undef HAVE_GETHOSTBYADDR_R */
/* #undef HAVE_GETHRTIME */
/* #undef HAVE_GETPAGESIZE */
/* #undef HAVE_GETPAGESIZES */
/* #undef HAVE_GETPASS */
/* #undef HAVE_GETPASSPHRASE */
/* #undef HAVE_GETPWNAM */
/* #undef HAVE_GETPWUID */
/* #undef HAVE_GETRLIMIT */
/* #undef HAVE_GETRUSAGE */
/* #undef HAVE_GETTIMEOFDAY */
/* #undef HAVE_GETWD */
#define HAVE_GMTIME_R 1
/* #undef gmtime_r */
/* #undef HAVE_IN_ADDR_T */
/* #undef HAVE_INITGROUPS */
#define HAVE_LDIV 1
/* #undef HAVE_LRAND48 */
#define HAVE_LOCALTIME_R 1
/* #undef HAVE_LSTAT */
/* #undef HAVE_MEMALIGN */
/* #undef HAVE_MLOCK */
/* #undef HAVE_NL_LANGINFO */
/* #undef HAVE_MADVISE */
/* #undef HAVE_DECL_MADVISE */
/* #undef HAVE_DECL_MHA_MAPSIZE_VA */
/* #undef HAVE_MALLINFO */
/* #undef HAVE_MALLINFO2 */
/* #undef HAVE_MALLOC_ZONE */
#define HAVE_MEMCPY 1
#define HAVE_MEMMOVE 1
/* #undef HAVE_MKSTEMP */
/* #undef HAVE_MKOSTEMP */
/* #undef HAVE_MLOCKALL */
/* #undef HAVE_MMAP */
/* #undef HAVE_MMAP64 */
#define HAVE_PERROR 1
/* #undef HAVE_POLL */
/* #undef HAVE_POSIX_FALLOCATE */
/* #undef HAVE_FALLOC_PUNCH_HOLE_AND_KEEP_SIZE */
/* #undef HAVE_PREAD */
/* #undef HAVE_READ_REAL_TIME */
/* #undef HAVE_PTHREAD_ATTR_CREATE */
/* #undef HAVE_PTHREAD_ATTR_GETGUARDSIZE */
/* #undef HAVE_PTHREAD_ATTR_GETSTACKSIZE */
/* #undef HAVE_PTHREAD_ATTR_SETSCOPE */
/* #undef HAVE_PTHREAD_ATTR_SETSTACKSIZE */
/* #undef HAVE_PTHREAD_CONDATTR_CREATE */
/* #undef HAVE_PTHREAD_GETAFFINITY_NP */
/* #undef HAVE_PTHREAD_KEY_DELETE */
/* #undef HAVE_PTHREAD_KILL */
/* #undef HAVE_PTHREAD_RWLOCK_RDLOCK */
/* #undef HAVE_PTHREAD_SIGMASK */
/* #undef HAVE_PTHREAD_YIELD_NP */
/* #undef HAVE_PTHREAD_YIELD_ZERO_ARG */
/* #undef PTHREAD_ONCE_INITIALIZER */
#define HAVE_PUTENV 1
/* #undef HAVE_READDIR_R */
/* #undef HAVE_READLINK */
/* #undef HAVE_REALPATH */
#define HAVE_RENAME 1
/* #undef HAVE_RWLOCK_INIT */
/* #undef HAVE_SCHED_YIELD */
#define HAVE_SELECT 1
/* #undef HAVE_SETENV */
#define HAVE_SETLOCALE 1
/* #undef HAVE_SETUPTERM */
/* #undef HAVE_SIGSET */
/* #undef HAVE_SIGACTION */
/* #undef HAVE_SIGTHREADMASK */
/* #undef HAVE_SIGWAIT */
/* #undef HAVE_SIGWAITINFO */
/* #undef HAVE_SLEEP */
#define HAVE_SNPRINTF 1
/* #undef HAVE_STPCPY */
#define HAVE_STRERROR 1
#define HAVE_STRCOLL 1
#define HAVE_STRNLEN 1
#define HAVE_STRPBRK 1
#define HAVE_STRTOK_R 1
#define HAVE_STRTOLL 1
#define HAVE_STRTOUL 1
#define HAVE_STRTOULL 1
#define HAVE_TELL 1
/* #undef HAVE_THR_SETCONCURRENCY */
/* #undef HAVE_THR_YIELD */
#define HAVE_TIME 1
/* #undef HAVE_TIMES */
/* #undef HAVE_VIDATTR */
#define HAVE_VIO_READ_BUFF 1
/* #undef HAVE_VASPRINTF */
#define HAVE_VSNPRINTF 1
/* #undef HAVE_FTRUNCATE */
#define HAVE_TZNAME 1
/* Symbols we may use */
/* #undef HAVE_SYS_ERRLIST */
/* used by stacktrace functions */
/* #undef HAVE_BACKTRACE */
/* #undef HAVE_BACKTRACE_SYMBOLS */
/* #undef HAVE_BACKTRACE_SYMBOLS_FD */
/* #undef HAVE_PRINTSTACK */
#define HAVE_IPV6 1
/* #undef ss_family */
/* #undef HAVE_SOCKADDR_IN_SIN_LEN */
/* #undef HAVE_SOCKADDR_IN6_SIN6_LEN */
#define STRUCT_TIMESPEC_HAS_TV_SEC 1
#define STRUCT_TIMESPEC_HAS_TV_NSEC 1

#define USE_MB 1
#define USE_MB_IDENT 1

/* this means that valgrind headers and macros are available */
/* #undef HAVE_VALGRIND_MEMCHECK_H */

/* this means WITH_VALGRIND - we change some code paths for valgrind */
/* #undef HAVE_valgrind */

/* Types we may use */
#ifdef __APPLE__
  /*
    Special handling required for OSX to support universal binaries that 
    mix 32 and 64 bit architectures.
  */
  #if(__LP64__)
    #define SIZEOF_LONG 8
  #else
    #define SIZEOF_LONG 4
  #endif
  #define SIZEOF_VOIDP   SIZEOF_LONG
  #define SIZEOF_CHARP   SIZEOF_LONG
  #define SIZE{
    "name": "google/recaptcha",
    "description": "Client library for reCAPTCHA, a free service that protects websites from spam and abuse.",
    "type": "library",
    "keywords": ["recaptcha", "captcha", "spam", "abuse"],
    "homepage": "https://www.google.com/recaptcha/",
    "license": "BSD-3-Clause",
    "support": {
        "forum": "https://groups.google.com/forum/#!forum/recaptcha",
        "source": "https://github.com/google/recaptcha"
    },
    "require": {
        "php": ">=5.5"
    },
    "require-dev": {
        "phpunit/phpunit": "^4.8.36|^5.7.27|^6.59|^7.5.11",
        "friendsofphp/php-cs-fixer": "^2.2.20|^2.15",
        "php-coveralls/php-coveralls": "^2.1"
    },
    "autoload": {
        "psr-4": {
            "ReCaptcha\\": "src/ReCaptcha"
        }
    },
    "extra": {
        "branch-alias": {
            "dev-master": "1.2.x-dev"
        }
    },
    "scripts": {
        "lint": "vendor/bin/php-cs-fixer -vvv fix --using-cache=no --dry-run .",
        "lint-fix": "vendor/bin/php-cs-fixer -vvv fix --using-cache=no .",
        "test": "vendor/bin/phpunit --colors=always",
        "serve-examples": "@php -S localhost:8080 -t examples"
    },
    "config": {
        "process-timeout": 0
    }
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         # Contributing

Want to contribute? Great! First, read this page (including the small print at
the end).

## Contributor License Agreement

Before we can use your code, you must sign the [Google Individual Contributor
License
Agreement](https://developers.google.com/open-source/cla/individual?csw=1)
(CLA), which you can do online. The CLA is necessary mainly because you own the
copyright to your changes, even after your contribution becomes part of our
codebase, so we need your permission to use and distribute your code. We also
need to be sure of various other things—for instance that you'll tell us if you
know that your code infringes on other people's patents. You don't have to sign
the CLA until after you've submitted your code for review (a link will be
automatically added to your Pull Request) and a member has approved it, but you
must do it before we can put your code into our codebase. Before you start
working on a larger contribution, you should get in touch with us first through
the issue tracker with your idea so that we can help out and possibly guide you.
Coordinating up front makes it much easier to avoid frustration later on.

## Linting and testing

We use PHP Coding Standards Fixer to maintain coding standards and PHPUnit to
run our tests. For convenience, there are Composer scripts to run each of these:

```sh
composer run-script lint
composer run-script test
```

These are run automatically by [Travis
CI](https://travis-ci.org/google/recaptcha) against your Pull Request, but it's
a good idea to run them locally before submission to avoid getting things
bounced back. That said, tests can be a little daunting so feel free to submit
your PR and ask for help.

## Code reviews

All submissions, including submissions by project members, require review.
Reviews are conducted on the Pull Requests. The reviews are there to ensure and
improve code quality, so treat them like a discussion and opportunity to learn.
Don't get disheartened if your Pull Request isn't just automatically approved.

### The small print

Contributions made by corporations are covered by a different agreement than the
one above, the Software Grant and Corporate Contributor License Agreement.
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         