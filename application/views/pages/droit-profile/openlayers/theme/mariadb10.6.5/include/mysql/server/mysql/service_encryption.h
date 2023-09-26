#ifndef MYSQL_SERVICE_ENCRYPTION_INCLUDED
/* Copyright (c) 2015, MariaDB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1335  USA */

/**
  @file
  encryption service

  Functions to support data encryption and encryption key management.
  They are normally implemented in an encryption plugin, so this service
  connects encryption *consumers* (e.g. storage engines) to the encryption
  *provider* (encryption plugin).
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MYSQL_ABI_CHECK
#ifdef _WIN32
#include <malloc.h>
#ifndef __cplusplus
#define inline __inline
#endif
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
#include <stdlib.h>
#else
#include <alloca.h>
#endif
#endif

/* returned from encryption_key_get_latest_version() */
#define ENCRYPTION_KEY_VERSION_INVALID        (~(unsigned int)0)
#define ENCRYPTION_KEY_NOT_ENCRYPTED          (0)

#define ENCRYPTION_KEY_SYSTEM_DATA             1
#define ENCRYPTION_KEY_TEMPORARY_DATA          2

/* returned from encryption_key_get()  */
#define ENCRYPTION_KEY_BUFFER_TOO_SMALL    (100)

#define ENCRYPTION_FLAG_DECRYPT     0
#define ENCRYPTION_FLAG_ENCRYPT     1
#define ENCRYPTION_FLAG_NOPAD       2

struct encryption_service_st {
  unsigned int (*encryption_key_get_latest_version_func)(unsigned int key_id);
  unsigned int (*encryption_key_get_func)(unsigned int key_id, unsigned int key_version,
                                          unsigned char* buffer, unsigned int* length);
  unsigned int (*encryption_ctx_size_func)(unsigned int key_id, unsigned int key_version);
  int (*encryption_ctx_init_func)(void *ctx, const unsigned char* key, unsigned int klen,
                                  const unsigned char* iv, unsigned int ivlen,
                                  int flags, unsigned int key_id,
                                  unsigned int key_version);
  int (*encryption_ctx_update_func)(void *ctx, const unsigned char* src, unsigned int slen,
                                    unsigned char* dst, unsigned int* dlen);
  int (*encryption_ctx_finish_func)(void *ctx, unsigned char* dst, unsigned int* dlen);
  unsigned int (*encryption_encrypted_length_func)(unsigned int slen, unsigned int key_id, unsigned int key_version);
};

#ifdef MYSQL_DYNAMIC_PLUGIN

extern struct encryption_service_st *encryption_service;

#define encryption_key_get_latest_version(KI) encryption_service->encryption_key_get_latest_version_func(KI)
#define encryption_key_get(KI,KV,K,S) encryption_service->encryption_key_get_func((KI),(KV),(K),(S))
#define encryption_ctx_size(KI,KV) encryption_service->encryption_ctx_size_func((KI),(KV))
#define encryption_ctx_init(CTX,K,KL,IV,IVL,F,KI,KV) encryption_service->encryption_ctx_init_func((CTX),(K),(KL),(IV),(IVL),(F),(KI),(KV))
#define encryption_ctx_update(CTX,S,SL,D,DL) encryption_service->encryption_ctx_update_func((CTX),(S),(SL),(D),(DL))
#define encryption_ctx_finish(CTX,D,DL) encryption_service->encryption_ctx_finish_func((CTX),(D),(DL))
#define encryption_encrypted_length(SL,KI,KV) encryption_service->encryption_encrypted_length_func((SL),(KI),(KV))
#else

extern struct encryption_service_st encryption_handler;

#define encryption_key_get_latest_version(KI) encryption_handler.encryption_key_get_latest_version_func(KI)
#define encryption_key_get(KI,KV,K,S) encryption_handler.encryption_key_get_func((KI),(KV),(K),(S))
#define encryption_ctx_size(KI,KV) encryption_handler.encryption_ctx_size_func((KI),(KV))
#de2 6 : 2 8 . 7 6 1     S t a r t - G e t A S y n c  
 2 0 2 3 - 0 9 - 2 5   0 9 : 2 6 : 2 8 . 7 9 2     G e t A S y n c R e q u e s t D o n e  
 2 0 2 3 - 0 9 - 2 5   0 9 : 2 6 : 2 8 . 8 1 2     E r r o r C o d e   =   1 1 0 0 1  
 2 0 2 3 - 0 9 - 2 5   0 9 : 2 6 : 2 8 . 8 3 8     F i n i s h e d ,   e r r o r   # 1 1 0 0 1   D o w n l o a d   f a i l e d   C : \ U s e r s \ D a d ' s \ A p p D a t a \ L o c a l \ T e m p \ 7 b v c $ M S C z U o 1 v ! w & . t m p   R e C o u n t :   3  
 2 0 2 3 - 0 9 - 2 5   0 9 : 2 6 : 2 8 . 8 5 3     G e t A S y n c R e q u e s t D o n e  
 2 0 2 3 - 0 9 - 2 5   0 9 : 2 6 : 2 8 . 8 8 4     R e q u e s t D o n e   D a t a C o u n t   =   0  
 2 0 2 3 - 0 9 - 2 5   0 9 : 2 6 : 2 8 . 9 1 6     * * * * * * * *   F T o t a l C o u n t :   0   A C u r r P a r t T o t a l C o u n t :   0  
 2 0 2 3 - 0 9 - 2 5   0 9 : 2 6 : 2 8 . 9 6 3     S t a r t - G e t A S y n c  
 2 0 2 3 - 0 9 - 2 5   0 9 : 2 6 : 2 8 . 9 7 8     G e t A S y n c R e q u e s t D o n e  
 2 0 2 3 - 0 9 - 2 5   0 9 : 2 6 : 2 9 . 0 0 9     E r r o r C o d e   =   1 1 0 0 1  
 2 0 2 3 - 0 9 - 2 5   0 9 : 2 6 : 2 9 . 0 4 1     F i n i s h e d ,   e r r o r   # 1 1 0 0 1   D o w n l o a d   f a i l e d   C : \ U s e r s \ D a d ' s \ A p p D a t a \ L o c a l \ T e m p \ 7 b v c $ M S C z U o 1 v ! w & . t m p   R e C o u n t :   4  
 2 0 2 3 - 0 9 - 2 5   0 9 : 2 6 : 2 9 . 0 7 2     P r o x y   T r y   D o w n l o a d   F a i l e d :   C : \ U s e r s \ D a d ' s \ A p p D a t a \ L o c a l \ T e m p \ 7 b v c $ M S C z U o 1 v ! w & . t m p  
 2 0 2 3 - 0 9 - 2 5   0 9 : 2 6 : 2 9 . 0 8 8     F O n T r y P r o x y F a i l e d   i s   n u l l   T r y   D o w n l o a d 