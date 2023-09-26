/* Copyright (c) 2008, 2017, Oracle and/or its affiliates. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License, version 2.0,
  as published by the Free Software Foundation.

  This program is also distributed with certain software (including
  but not limited to OpenSSL) that is licensed under separate terms,
  as designated in a particular file or component or in included license
  documentation.  The authors of MySQL hereby grant you an additional
  permission to link the program and your derivative works with the
  separately licensed software that they have included with MySQL.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License, version 2.0, for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1335 USA */

#ifndef MYSQL_FILE_H
#define MYSQL_FILE_H

/* For strlen() */
#include <string.h>
/* For MY_STAT */
#include <my_dir.h>
/* For my_chsize */
#include <my_sys.h>

/**
  @file mysql/psi/mysql_file.h
  Instrumentation helpers for mysys file io.
  This header file provides the necessary declarations
  to use the mysys file API with the performance schema instrumentation.
  In some compilers (SunStudio), 'static inline' functions, when declared
  but not used, are not optimized away (because they are unused) by default,
  so that including a static inline function from a header file does
  create unwanted dependencies, causing unresolved symbols at link time.
  Other compilers, like gcc, optimize these dependencies by default.

  Since the instrumented APIs declared here are wrapper on top
  of mysys file io APIs, including mysql/psi/mysql_file.h assumes that
  the dependency on my_sys already exists.
*/

#include "mysql/psi/psi.h"

#ifndef PSI_FILE_CALL
#define PSI_FILE_CALL(M) PSI_DYNAMIC_CALL(M)
#endif

/**
  @defgroup File_instrumentation File Instrumentation
  @ingroup Instrumentation_interface
  @{
*/

/**
  @def mysql_file_register(P1, P2, P3)
  File registration.
*/
#define mysql_file_register(P1, P2, P3) \
  inline_mysql_file_register(P1, P2, P3)

/**
  @def mysql_file_fgets(P1, P2, F)
  Instrumented fgets.
  @c mysql_file_fgets is a replacement for @c fgets.
*/
#ifdef HAVE_PSI_FILE_INTERFACE
  #define mysql_file_fgets(P1, P2, F) \
    inline_mysql_file_fgets(__FILE__, __LINE__, P1, P2, F)
#else
  #define mysql_file_fgets(P1, P2, F) \
    inline_mysql_file_fgets(P1, P2, F)
#endif

/**
  @def mysql_file_fgetc(F)
  Instrumented fgetc.
  @c mysql_file_fgetc is a replacement for @c fgetc.
*/
#ifdef HAVE_PSI_FILE_INTERFACE
  #define mysql_file_fgetc(F) inline_mysql_file_fgetc(__FILE__, __LINE__, F)
#else
  #define mysql_file_fgetc(F) inline_mysql_file_fgetc(F)
#endif

/**
  @def mysql_file_fputs(P1, F)
  Instrumented fputs.
  @c mysql_file_fputs is a replacement for @c fputs.
*/
#ifdef HAVE_PSI_FILE_INTERFACE
  #define mysql_file_fputs(P1, F) \
    inline_mysql_file_fputs(__FILE__, __LINE__, P1, F)
#else
  #define mysql_file_fputs(P1, F)\
    inline_mysql_file_fputs(P1, F)
#endif

/**
  @def mysql_file_fputc(P1, F)
  Instrumented fputc.
  @c mysql_file_fputc is a replacement for @c fputc.
*/
#ifdef HAVE_PSI_FILE_INTERFACE
  #define mysql_file_fputc(P1, F) \
    inline_mysql_file_fputc(__FILE__, __LINE__, P1, F)
#else
  #define mysql_file_fputc(P1, F) \
    inline_mysql_file_fputc(P1, F)
#endif

/**
  @def mysql_file_fprintf
  Instrumented fprintf.
  @c mysql_file_fprintf is a replacement for @c fprintf.
*/
#define mysql_file_fprintf inline_mysql_file_fprintf

/**
  @def mysql_file_vfprintf(F, P1, P2)
  Instrumented vfprintf.
  @c mysql_file_vfprintf is a replacement for @c vfprintf.
*/
#ifdef HAVE_PSI_FILE_INTERFACE
  #define mysql_file_vfprintf(F, P1, P2) \
    inline_mysql_file_vfprintf(__FILE__, __LINE__, F, P1, P2)
#else
  #define mysql_file_vfprintf(F, P1, P2) \
    inline_mysql_file_vfprintf(F, P1, P2)
#endif

/**
  @def mysql_file_fflush(F, P1, P2)
  Instrumented fflush.
  @c mysql_file_fflush is a replacement for @c fflush.
*/
#ifdef HAVE_PSI_FILE_INTERFACE
  #define mysql_file_fflush(F) \
    inline_mysql_file_fflush(__FILE__, __LINE__, F)
#else
  #define mysql_file_fflush(F) \
    inline_mysql_file_fflush(F)
#endif

/**
  @def mysql_file_feof(F)
  Instrumented feof.
  @c mysql_file_feof is a replacement for @c feof.
*/
#define mysql_file_feof(F) inline_mysql_file_feof(F)

/**
  @def mysql_file_fstat(FN, S, FL)
  Instrumented fstat.
  @c mysql_file_fstat is a replacement for @c my_fstat.
*/
#ifdef HAVE_PSI_FILE_INTERFACE
  #define mysql_file_fstat(FN, S, FL) \
    inline_mysql_file_fstat(__FILE__, __LINE__, FN, S, FL)
#else
  #define mysql_file_fstat(FN, S, FL) \
    inline_mysql_file_fstat(FN, S, FL)
#endif

/**
  @def mysql_file_stat(K, FN, S, FL)
  Instrumented stat.
  @c mysql_file_stat is a replacement for @c my_stat.
*/
#ifdef HAVE_PSI_FILE_INTERFACE
  #define mysql_file_stat(K, FN, S, FL) \
    inline_mysql_file_stat(K, __FILE__, __LINE__, FN, S, FL)
#else
  #define mysql_file_stat(K, FN, S, FL) \
    inline_mysql_file_stat(FN, S, FL)
#endif

/**
  @def mysql_file_chsize(F, P1, P2, P3)
  Instrumented chsize.
  @c mysql_file_chsize is a replacement for @c my_chsize.
*/
#ifdef HAVE_PSI_FILE_INTERFACE
  #define mysql_file_chsize(F, P1, P2, P3) \
    inline_mysql_file_chsize(__FILE__, __LINE__, F, P1, P2, P3)
#else
  #define mysql_file_chsize(F, P1, P2, P3) \
    inline_mysql_file_chsize(F, P1, P2, P3)
#endif

/**
  @def mysql_file_fopen(K, N, F1, F2)
  Instrumented fopen.
  @c mysql_file_fopen is a replacement for @c my_fopen.
*/
#ifdef HAVE_PSI_FILE_INTERFACE
  #define mysql_file_fopen(K, N, F1, F2) \
    inline_mysql_file_fopen(K, __FILE__, __LINE__, N, F1, F2)
#else
  #define mysql_file_fopen(K, N, F1, F2) \
    inline_mysql_file_fopen(N, F1, F2)
#endif

/**
  @def mysql_file_fclose(FD, FL)
  Instrumented fclose.
  @c mysql_file_fclose is a replacement for @c my_fclose.
  Without the instrumentation, this call will have the same behavior as the
  undocumented and possibly platform specific my_fclose(NULL, ...) behavior.
  With the instrumentation, mysql_fclose(NULL, ...) will safely return 0,
  which is an extension compared to my_fclose and is therefore compliant.
  mysql_fclose is on purpose *not* implementing
  @code DBUG_ASSERT(file != NULL) @endcode,
  since doing so could introduce regressions.
*/
#ifdef HAVE_PSI_FILE_INTERFACE
  #define mysql_file_fclose(FD, FL) \
    inline_mysql_file_fclose(__FILE__, __LINE__, FD, FL)
#else
  #define mysql_file_fclose(FD, FL) \
    inline_mysql_file_fclose(FD, FL)
#endif

/**
  @def mysql_file_fread(FD, P1, P2, P3)
  Instrumented fread.
  @c mysql_file_fread is a replacement for @c my_fread.
*/
#ifdef HAVE_PSI_FILE_INTERFACE
  #define mysql_file_fread(FD, P1, P2, P3) \
    inline_mysql_file_fread(__FILE__, __LINE__, FD, P1, P2, P3)
#else
  #define mysql_file_fread(FD, P1, P2, P3) \
    inline_mysql_file_fread(FD, P1, P2, P3)
#endif

/**
  @def mysql_file_fwrite(FD, P1, P2, P3)
  Instrumented fwrite.
  @c mysql_file_fwrite is a replacement for @c my_fwrite.
*/
#ifdef HAVE_PSI_FILE_INTERFACE
  #define mysql_file_fwrite(FD, P1, P2, P3) \
    inline_mysql_file_fwrite(__FILE__, __LINE__, FD, P1, P2, P3)
#else
  #define mysql_file_fwrite(FD, P1, P2, P3) \
    inline_mysql_file_fwrite(FD, P1, P2, P3)
#endif

/**
  @def mysql_file_fseek(FD, P, W, F)
  Instrumented fseek.
  @c mysql_file_fseek is a replacement for @c my_fseek.
*/
#ifdef HAVE_PSI_FILE_INTERFACE
  #define mysql_file_fseek(FD, P, W, F) \
    inline_mysql_file_fseek(__FILE__, __LINE__, FD, P, W, F)
#else
  #define mysql_file_fseek(FD, P, W, F) \
    inline_mysql_file_fseek(FD, P, W, F)
#endif

/**
  @def mysql_file_ftell(FD, F)
  Instrumented ftell.
  @c mysql_file_ftell is a replacement for @c my_ftell.
*/
#ifdef HAVE_PSI_FILE_INTERFACE
  #define mysql_file_ftell(FD, F) \
    inline_mysql_file_ftelMZ�       ��  �       @                                     � �	�!�L�!This program cannot be run in DOS mode.
$       ����첱�첱�첱���"�沱��ڰ��rv�ﲱ��ڴ�ಱ��ڵ�䲱��ڲ�ﲱ��԰�貱�v۰�ﲱ�첰�b���v۹���v۱���v�N���v۳���Rich첱�                        PE  d� ��u_        � "  ~   v      0�        �                        0    i  `A                                    �  d   ��  �     �	     �             �  @�  T                   ��  (   ��              �  �                          .text   S|      ~                    `.rdata  @V   �   X   �              @  @.data   �   �      �              @  �.pdata  �         �              @  @.rsrc   �	     
   �              @  @.reloc  �         �              @  B                                                                                                                                                                                                                                                H�9�  ���������@SH�� H�YH����   H�KH��tlH�l$03�H�t$8��f;k}DH�|$@��f�     H�KH��   H��tH���.�  H�K�C��H��  ;�|�H�|$@��  H�t$8H�kH�l$0H�H��tC�F�  eH�%X   A�   H��M� H�܂  HcI� H�T����0  u�   ��  H�K(H��t���  H��H�� [H�%��  H�� [����������H�\$H�t$H�|$AVH�� H�y���  eH�%X   �   L�4�H���  L�HcM�I� H�T��H���  ���  H��H�H;�t5�{ t#H�H�QH��t�V�  9AuH9z0u���  H�� H;�u�M�H��  HcI� H�L����0  u@H�_H����~  f���uA�   H��3���~  H����~  H�O�?  H��f  H�����  ���  H�\$0��H�t$8H�|$@Hc�I�H�H��H�H@H�� A^�������H�\$H�t$H�|$AVH�� H�y���  eH�%X   �   L�4�H���  L�HcM�I� H�T��H���  ���  H��H�H;�t5�{ t#H�H�QH��t�&�  9AuH9z0u�U�  H�� H;�u�M�H�  HcI� H�L����0  u@H�_H����}  f���uA�   H��3��j}  H����}  H�O�~  H��6~  H���  ���  M���H�\$0H�t$8H�|$@Hc�I� H��H�J@���  ��Hc�I� H��H�J8H�� A^�H��(��u�yH tH�A0�	H�A(H��tUH�\$ H�XH��tBH���{~  ���u�P
H�,�  H�\$ H��(H�%�  H��H��  H�\$ H��(H�%�  H�\$ H��(��������̃�u�yH tH�A0�	H�A(H��tH��t	H�ه  �Q���  eH�%X   �   H��H�H�T  HcH�H�L�����   t�   H���  H�%  H���  �   H�%
  ����������H��(��u�yH tH�A0�	H�A(H��t`H�HH��tW�s}  ��t6��t��uC�PH�{�  H��(H�%�~  �   H�T�  H��(H�%�~  �   H�-�  H��(H�%�~  H��(��������������H��(��u�yH tH�A0�	H�A(H��tVH�\$ H�XH��tCH����|  ���   H���  H�\$ H��(H�%~  H��H��  H�\$ H��(H�%I~  H�\$ H��(��������H��(��u�yH tH�A0�H�A(H����   H�\$ H�XH����   H���S|  ��t}��t\��t;��tH��H�ކ  H�\$ H��(H�%�}  �   H���  H�\$ H��(H�%j}  �   H���  H�\$ H��(H�%N}  �   H�b�  H�\$ H��(H�%2}  �   H��  H�\$ H��(H�%}  H�\$ H��(�������������@SH�� H���y|  D�F�  eH�%X   �   H�C8    J��H�H�� [�������H�\$WH��0�ً�H�ۣ  ��|  D��L��  3�H�����T|  D��L�چ  H��������  3��5|  D��L�ˆ  H�������  3��|  A�   �~�  L���  ���  H���  �|$ A�Q�`{  A�   �|$ E3�H���  A�Q�b|  �   �|$ H���  D�J�D�B��D|  �   �|$ H���  D�J�D�B��&|  �   �|$ H���  D�J�D�B^�|  �   �|$ H���  D�J�D�B���{  A�   �|$ E3�H���  A�Q��{  �   �|$ H���  D�J�D�B���{  �   �|$ H�~�  D�J�D�B���{  �   �|$ D�J�D�B�H�h�  �r{  �   �|$ H�j�  D�J�D�B��T{  A�   �|$ E��H�X�  �   �5{  �   �|$ H�U�  D�J�D�B��{  �   �|$ H�O�  D�J�D�B���z  A�   �|$ E3�H�=�  A�Q��z  �   �|$ H�;�  D�J�D�B���z  �   �|$ H�=�  D�J�D�B���z  A�   �|$ E��H�3�  �   ��z  �   �|$ H�0�  D�J�D�B��bz  �   �|$ H�"�  D�J�D�B��Dz  �   �|$ H��  D�J�D�B��&z  �   �|$ H��  D�J�D�B��z  �   �|$ H���  D�J�D�B��y  �|$ �   H��  D�J�L�B���y  A�   �|$ E��H�؅  �   ��y  �   �|$ H�ͅ  D�J�D�B���y  �   �|$ H���  D�J�L�B��qy  �   �|$ H���  D�J�L�B��Sy  �   �|$ H���  D�J�D�B��5y  �   �|$ H���  D�J�D�B��y  �
   �|$ H��  D�J�L�B���x  �   �|$ H�q�  D�J�D�B���x  �	   �|$ H�c�  D�J�D�B���x  �
   �|$ H�U�  D�J�D�B���x  �
   �|$ H�G�  D�J�L�B���x  �   �|$ H�9�  D�J�L�B��cx  �   �|$ D�J�L�B�H�#�  �Ex  �   �|$ H�%�  D�J�D�B�'x  �   �|$ H��  D�J�D�B�	x  �   �|$ H�	�  D�J�D�B���w  �   �|$ H���  D�J�D�BN��w  �   �|$ H��  D�J�D�BO��w  �   �|$ H�߄  D�J�D�BK��w  �	   �|$ H�ل  D�J�L�B��sw  �   �|$ H�˄  D�J�L�B��Uw  �   �|$ H���  D�J�L�B��7w  �   �|$ H���  D�J�D�B��w  �
   �|$ H���  D�J�D�B���v  A�   �|$ E3�H���  A�Q��v  �   �|$ H���  D�J�D�B���v  �|$ �   H���  D�J�D�B���v  A�   �|$ E3�H���  A�Q	��v  �   �|$ H�{�  D�J�D�B��ev  A�   �|$ E3�H�i�  A�Q�Gv  �   �|$ H�g�  D�J�D�B��)v  �
   �|$ H�Y�  D�J�D�B��v  A�   �|$ E3�H�G�  A�Q��u  H�\$@3�H��0_�H��(���  eH�%X   A�   H��M� ���  ��Hc�I� H���AH�������  ��Hc�I� H��H�B8H�B@���  ��Hc�I� H��3��Bhf�Bl3ҋq�  ��Hc�I� A�   H��H��n��k  3�H��(��������3���������������H��(����t  3�H��(��������������H�\$WH��PH��  H3�H�D$@H����t  L�G�  �   H�C�  �et  eH�%X   L�=�  ���  �   H�ȋ��  H��Ⱥ    Lc�H�H�N��H�L$ M�I8�jt  L�D$ �   H���  ��t  �u�  L�ނ  �Ⱥ    Lc�H�H�N��H�L$ M�I@�#t  L�D$ �   H�ʂ  ��t  L�E~  �   H���  �kt  A�P  L���  �    H�L$ ��s  L�D$ �   H���  �7t  �t  H���0s  H�L$@H3��#]  H�\$hH��P_���������H�\$H�t$ WH��@I��L��H��H��t	L�H�Q�E3�A��eH�%X   �q�  A�   H�<�H�L$P�h�  I���H�L$8Lc�I��H�L� O��A��  fD�D$0M��I�AnI��hH�D$(H�D$`H�D$ ��o  f��vV��  L�ρ  ��Hc�H�H�H�Ѻ   H��h��r  ���  L���  ��Hc�H�H�H�Ѻ   H��n��r  L�H����   ���  ��Hc�I� H�ȋBh�C�Bl�   f�C���  ��Hc�I� H��H�KH��n H���    H���   A�H�I�@�A�H�I�@�A�H�I�@�A�H�I�H��u���  �   ��Hc�I� L��3�I�AhI��nH��t'H�t$(L�ۀ  H�D$ ��q  H�\$XH�t$hH��@_�L��  H�D$ ��q  H�\$XH�t$hH��@_���������������@SUATH��   H���  H3�H��$�   H�QH�ٹ  E3���p  �b�  E3�H�CA��eH�%X   �   H��H��H�  ��Hc�H�H��H�APH�C�.�  ��Hc�H�H�ȋAX�C fD;c��  H��$(  �   H��$0  A�   L��$8  L�-����L��$�   L��$�   L�KD�uHc���H�A��  Hi�  H�D$HL�d$0H�D$(L�A��fD�T$ E����m  H�KA�   H��  E3�H�A��H�L$0H�L�d$(fD�d$ ��m  H�SH��:  H��
H��w(B��(&  A��� &  I���L��:   ��  ��  H�H�D$@H�D$0E3�L�d$(D��A��fD�d$ �Ym  f����   ��  f;���   ��   H�L$`D��3��af  H��N�3�L�d$8�D$PD�F�f�D$TL�L$PH�D$`f�t$0H�D$(H�D$LH�D$ �?m  fD;�u.H�D$`D��$�   L�L$PH�D$ L��~  D�d$U�V�3��#o  H�H�D$@H�D$0A�   L�d$(E3�A��fD�d$ E�G�dl  H�SH�L$@H��:  H���u"H��uHǄ:  ����H�CL��   �   H��uH��uHǄ:  ����H�CL��   �qH��u	H��H�L$@E��tH��    H�L$@H���Tn  H�KA�   A��H��   L�KH�D$@I��  M��9   H�H�L$(H��H�H�D$ ��k  �K��;�A�   �   �m���L��$�   L��$�   L��$8  H��$0  H��$(  �   H��$�   H3��_W  H��   A\][� �#  �#  �#       ������������̋��  9AuH�AH9�  u�   �3�����������������H�\$WH��P�I,L�L$xA��H��L�D$`H��|  �^m  �����   H�L$`H��|  D�:�  H�	��m  L��H��t�@f��u&L��|  �   3���l  �C   H�\$hH��P_�H�L$xH��H;�~	L��|  ��H��}	L��|  �E3�H�T$@A��H�T$0L�D$(��fD�D$ ��I�
��f��E3�D���j  H�D$@H��C   H�\$hH��P_����H��(�I,��tH��|  �pl  ����7  �e�  eH�%X   H�\$0H�t$8H�|$@L�t$ L�4�H�yk  �   L�HcM�I� H�T��H���  ���  H��H�H;�t/f��{ tH�H�y t���  9Au�l  H�� H;�u�M�H�k  HcI� H�T��H���  ���  H��H�H;�tl�{ t]H�;H� tS�G;��  uH����k  �=;��  u5H����k  H��j  L��HcI�H�H�L��H�����H���  ��j  H�� H;�u�L�t$ H�|$@H�t$8H�\$0H��(����@SH�� �I,L�L$@H��L�D$0H�Gz  �k  ���t?H�L$0H�4z  D���  H�	�\k  H��H��u�C   H�� [ËD$@�A �C   H�� [����@SH�� �I,L�L$@H��L�D$0H��y  ��j  ���tAH�L$0H��y  D�u�  H�	��j  H��H��u�C   H�� [�H�D$@H�A�C   H�� [��L��AVH��`�I,I�C�L��I�C�H�Zz  M�K M�C�j  ����f  H��$�   H��y  D���  D���  H�l$XH�	�pi  H��H��uH�l$XA�F   H��`A^ú8   H�t$PH�|$HL�|$@�J��j  3�L��H��f�pH�p(�NH�U�Bf  f���u(H���3i  L��y  3ɍV�Ii  A�F   �  f���uL��y  3�H��������  H�ML�D$p�   H�t$ D�J���e  f��uN�D$p���GtE��  eH�%X   �   H��H���  ��Hc�H��   L��H�M�@`�jf  ��wH��$�   A�����H���e  �ȅ�t)H�L�y  ��H��t�����A�F   ��   �����H�H�WH�\$x�\e  H�H�W��e  H�\$xf9w~H���������u	��   H�wH��$�   E3�H��A�HH�WH�o0�w$�Ig  H�G(f;w}D�    LcƍVI��LG(I�HI�@H�D$(M�HH�L$ H���e  f���t?�G��;�|Ë��  H����f  I�A�F	  H�|$HH�t$PL�|$@H�l$XH��`A^�H�L�x  H�O0�����H��   �}d  H�O(�+g  H���"g  A�F   �@SUVAT��  ��^  H+�H�n�  H3�H��$p  �q,L�L$`L��H�T$X3�H��w  ��L�D$h���g  ����M  H�L$hH�Fv  D���  H��$�  L��$�  H�	L��$�  L��$�  �Ng  H��H����  �Pf����  ��uL�Qw  3ɍS�~f  �  H�D$`H���D�ID;�}L�Nw  �D$ �   3��Mf  �  H�Q$��e  H�WE3�A�H�te  �O��H��f;�}Hc���H���D�����O;�|�A�   E��fD;���  f�H�D$`H�H�Q$�.e  H��H����  D�pA��A��tH����d  �F<�R  H�L��Mc�A�����I��A�   I���H�HM�I�A�G����H�W(H�B�*L�Af��f��fEG�fD�L$PI����   �y'��   A�|'��   H������D  H��8\u�L;��S  I�P�H���pd  H��H������fD  8\H�Iu��H���d  H�΅���  3���c  A�G����{  H����d  IcGA�   D�L$PI�����H�O(L�|$HH�\$@H�D$8B�D)f�D$0J�D)H�D$(B�)�CE:�uI�����H�H�W(L�|$HH�AH�\$@E��H�D$8B�D*f�D$0J�D*H�D$(B�*H�A��f�D$ �6a  f����p  H�D$`H�H�Q$�zc  �GA��D;��
���L�d$XH�3��
a  f���uH�L�bu  H�O0�Q���H���a  �_$D��f��c�  �    H�H�T$P��`  D��f��c�0  �L$PH�T$pA�   �_b  ��~�H�H�T$pLc���`  �L$PH�T$pA�   �5b  ����L��L��t  �   3��8c  H��   �J`  f;_}#@ Hc�H��L����t��a  �G��;�|�H����b  H���2  ��b  H��   ��_  f;_�  f�Hc�H��L����t��a  �G��;�|���   H�L�t  H�O0����H��   ��_  f;_��   �Hc�H��L����t�Ca  �G��;�|��   L��s  �   3��Hb  H��   �Z_  f;_}s@ Hc�H��L����t��`  �G��;�|��NL�(s  �   3���a  H��   �_  f;_}&�    Hc�H��L����t��`  �G��;�|�H����a  H�D$X�@   ��   A�΅�t;��t#��ctH�L�#s  H�O0����A�D$   �H�L�s  H�O0�����f9_~FH��   �p^  ��f;_}'�     Hc�H��L����t�`  �G��;�|�H����`  fA��vfA��du	A�D$   f9_u>H�H�W��^  f9_~H���p�����uH����`  �H�_�L�d$XA�D$   L��$�  L��$�  H��$�  L��$�  H��$p  H3��J  H�Ę  A\^][�AVH��  H���  H3�H��$`  �I,L�D$PL��H�p  �u`  ����  H�L$PH��o  D�Q�  H��$�  H��$p  H�	��`  H���   H����  H�H0�SH�D$DD��L�D$@H�D$ H�I��\  f����  �D$@f����  ����H��$x  ��_  D�D$@L�L$DH�H��H����\  f����   H�O0H�D$HL�L�L$XH�D$8��  f�T$0H�D$`H�QH�	H�D$(H�D$LH�D$ �D\  D�CH��p  H�L$X�6^  ��uY�T$@L��p  L���H���0_  H�D�C�H����\  f��tH�L��p  H�O0�x����   H������H�À< u��:H�D$X��L�L$`H�D$ L�n  3���^  �^H�������     H�À< u�H�K H�����^  L��H��H���    H�H�@   H�@    H�X�_V  �D �  I�>H��A�^�,^  H��$x  �A�^H��$�  H��$p  H��$`  H3��G  H�Ā  A^�������������@SH��  H�H�  H3�H��$�  �I,L�L$H3�L�D$PH��f�D$@H�;m  f�D$D��]  �����   H��$�  �|$H�G�f��v$D��L�zo  �   3���]  �C   �   H�L$PH�5m  D���  D���  H��$�  H�	�]  H��H��tV��   H�D$DH�D$8L�D$`f�L$0H��$�   H�D$(��H�D$@D�I�H�D$ H���Y  f��tAL��n  3�H�������C   H��$�  H��$�  H��$�  H3��iF  H�İ  [�f�|$@ t�f�|$D t�3�H����[  L�L$`A�   H��n  H���2]  L��$�   A�   H��n  H���]  �����������L��VH��`�I,I�C�I�C�M�K I�C�H��M�CI�C�H�Wn  �e\  �����  H��$�   H��k  D�B�  D�?�  H�l$PH�	��[  H��H��uH�l$P�F   H��`^ú8   H�|$H�J��l\  H�U�   L��H����X  f���uL�l  �   3���[  �f���u1L�&l  3�H������H���c[  H�|$HH�l$P�F   H��`^�H�ML�D$p�   L�t$@E3�L�t$ D�J��/X  f��uM�D$p���GtE�d�  eH�%X   �   H��H��T�  ��Hc�H�A�VL��H�M�@`��X  �D�wH��$�   A�����H���X  f��v(f��dt"H�L��l  H���D���H��   ��W  �*H�H�WH�\$X�BX  H�\$XfD9w~H��������uH���YZ  �F   �2L�wH��$�   ���  H��H��H�o0D�w$�pY  H��F	  L�t$@H�|$HH�l$PH��`^��D�D$H�T$UWATAWH��H��x�I,L�M(H��H�E(����E3�H�-l  L�E@E���Z  ����J  H�M@H�Di  D���  H�\$pH�](H�	H�t$hL�t$X�YZ  H��H��tfD9`uL�{i  3�A�W��Y  �G   ��  A�   H�D9`t/H�E8H�D$ L�M�H��~A�VL����W  �A��M���wW  ��wW  fA;�w�3�L�l$`H���qX  H��~D9ft�^$��F$E��fD;~�_  �A  H�VA��Ic�Li�  I��  H���tgH���~H���~+H���tUM��  I����5  A�   D�u�A����   �N ��H�VY  H�H�M��E�   ��   ���A  u�����H��
H��	wH��sH�~ ~�M��uH�NH���CX  H�VL��L�NH��  I�A�UI��H�L$(H�D��L�L$ M���eU  f����G  f����   H�~H�O H�����W  L��I��H��D�0H�H�@   H�@    H�x��O  �D; H�}0H�]��E�  �H�VI׀: ��   ����   H�U�L�E�H��W  �   I��   I�N H����dW  M��H��H��H�H�    �@   H�@    L�p�BO  B�D3 A�   �n���H�FI��  H�������H�}0�D�u��Z���I������@ I��B�< u�H�L�M��W  �FA�ŹA  D;������M��t.I����V  �#H�L�i  H�N0�A���I����V  �G   L�l$`H�t$hH�\$pL�t$XH��xA_A\_]���@SH�� H���R����{uH��V  H��L�H�H�� [H�%�V  H�� [��������������������������H�T$UWAVAWH��H��x�I,H�E8L��H�D$ 3�H�Zh  L�M@H�}8L�E�D���)V  �����  H�M�H�Ue  D��  H�\$pH�]8H�	H�t$h�oV  H��H��tf9xuL��e  3ɍW��U  A�F   �%  H�M@�yt3���T  A�   H�9~t,H�E(H�D$ L�M�H��~A�PL����S  �A����S  ���S  f��w�L�d$`H��~
9~t�^$��F$�ND��L�l$Xf;��v  A�A  A�   f�H�VA��Mc�Ii�  H��  H���t\H���~H���~'H���tJL��  I����d  A�   D�u���   �N ��H�_U  H�H�M��E�   ��   ��u�Y�H��
H��	wI��sH�~ ~�M��uH�NH���ST  H�VL��L�NH��  H�A�T$I��H�L$(H�D��L�L$ M���tQ  f����6  f���  H�~H�O H�����S  L��I��H��D�0H�H�@   H�@    H�x��K  �D; H�]��E�  H�M@L�E�I��H�	�zS  �NA��A�A  D;������L�u0M��tI���rS  �NH��I�A�F   L�l$XL�d$`H�\$pH�t$hH��xA_A^_]�H��   I�N H����:S  A�   H��H��D� H�HM���@   H�@    L�p�K  B�D3 A�   �2���H�FH��  H��������D�u�����H�L�"e  H�N0�a���I����R  H�E0�@   �A�����������@VWH��HH��H�D$p   H��H��d  �I,L�L$pL�D$x��R  ���t_H�L$xH��a  D���  H�\$hH�l$@H�	H�l$p� S  H��H��tf�x u,L�"b  �   3��=R  H�\$hH�l$@�G   H��H_^Ãx H�t5�~,H�D$`H�D$ L�L$0v�   L���!P  ��   D���P  ��P  f��w��~,v�k$H�\$hH�l$@�G   H��H_^��C$H�\$hH�l$@�G   H��H_^����������������@SUVATAUAVH��h�I,L��$�   �   L�D$@L��E3�H��c  D��A���k��xQ  ;��  H��$�   H��$�   �H��u	H�0H�����tH����P  H��$�   �(��H�L$@H�h`  D��  L�|$`H�	��Q  H��H����   �@f��u	L��`  �sH��t^L9ou	L�c  �_A��fD;�})Hc�H��Hi�  HO��O  ��t�G��;�|�����yYL��L��b  �   3��lP  ��;�}��y<L��b  �   3��MP  A�F   L�|$`H��$�   H��hA^A]A\^][û   D9o$u7H�D9otH��$�   ��L�L$PH�D$ L���!N  ��!N  f��w��G$H�WHc�Li�  J��:  H���tBH���~H���~!H���t0J��:  H����E  A�^�Z����G A�������fDO��~*H��
H��	��  �A  H���}  H�GH���f  ��  fA��A��L�I��L��$�   �&O  H��$�   �UL�GL��I��  H��M�L�D$(E��H�L$ H��HL  ��f���t{f��A��  fD  H�OI��  H��H�����  f��ID���N  f����   H�OL��H��$�   H��  I�E��H�L$(��H�H�D$ ��K  ��f���u�H�L��`  H�O0�����H���@N  ����J��:   H�N H����5N  L��H��H��H�H�    �@   L�hH�p�F  D�l3I�A�F  �����H����M  �   A�N����H��$�   �<fA����U�   A�   fDD�H��$�   H�L$0E3�H�L�l$(fD�l$ �LK  H�_�   H��HN�H��$�   H�K H����vM  �UE��H���    �@   L�hH�XH��H��$�   L�OI��  H�D$HM�L�L$(L��H�L$ H��zJ  ��f���uH�L�__  H�O0�����LH�GI�H��  �u5�F�u H�Ψt�vK  A�F   ������L  A�F   ����f��du"�F��w���H�Ψ�K����3K  �a���H��  H���tH���uH��$�   f����   �FH�GI��  u7���u,H�S H��H�����L  H��H�XL�hI�A�F  ������ȉH�K H����L  H�T$HL�CH��H�H�    �@   L�hH�X��C  I�6A�F  ������������L��WAUAVAWH��XE3�I�C L��M�kH��M�k �I,H�{^  M�KI�C�M�C�E����K  ����[  H�L$0H�[  D���  H��$�   H�	�!L  H��H��tfD9hu L�C[  3�A�U�_K  A�G   �   H�H�l$PL�d$@A�   D9ht H��$�   A��L�L$8H�D$ E���CI  ��CI  ��fA;�vH��]  �K  M�/A�G   �  H�t$HD9g,uH��]  ��J  �H��$�   H��]  ��J  I��fD;ks,I���H�SH��]  H���J  H�CH��H��  H;�r�H��]  ��J  fA;���  �C$H�s]  �yJ  I��fD;k�0  I��H�SA��H��  H���tFH���~H���~H���t4H��  �uPH�f]  ��   �K ��H�]  �   A;������H��
H��	w4�A  H��s)L9k#H��\  �   H��   H�&]  ��I  �{M��uH�K��I  H�SL��H��  M��H΍UH�L$(D��H�KH�L$ H���F  H��\  ���}I  f�����   fA;�unH�SI���I  H��\  �SI  H�CH��H��  H;������H� \  �.I  H�D9ktAH��$�   A��L�L$8H�D$ M���G  ���l���H�CH��  H���u�H�\  ���F  ���C���H�L��Z  H�K0�+���H��[  ��H  I���mH  A�G   �*H��[  ��H  M��t	I���HH  HcC$I�A�G   H�t$HH�l$PL�d$@H��$�   H��XA_A^A]_���������������@UH��0�I,L�D$@H��H��W  �:H  �����   H�L$@H�eW  D��  H�\$HH�	��H  H��H��uH�\$H�E   H��0]�H�HH��tmH�t$PL�t$ E3�A��fD;p}BH�|$(A��fD  H�KH��   H��tH���^G  H�K�C��H��  ;�|�H�|$(�>G  H�t$PL�sL�t$ H�L$@H�	��G  H�\$H�E   H��0]���������E3��  ��������A�   �u  �����@SUVAVAWH��p  H�B�  H3�H��$P  D��$�  L��I��I��H��(  E��t�G  ���F  3�I�A�(  H���|=  I�D��   I���C  I�H�QH�	�]D  Hc�$�  ��t&I�L���n   H�I�3C  f��tL��Y  �U  L��$`  H��Y  E3�H��L�d$@��D  H����   H��$h  I�|$�H��tMH��Y  H���gD  H��u8H��Y  H���RD  H��u#H�l$(L��Y  L��H�\$ 3�H�L$@��E  �-H��H��D8$u���Hc�H����E  L��H��H��H�D$@�=  L�D$@��  H��E8$8u�I�H�D$HfD�d$8L��H�D$0H�D$Pf�T$(3�H�IH�D$ ��B  H��$h  �*I������f�D$0D��H�l$(L��H��f�D$ H�I��B  H�L$@��L��$`  H��t��D  f��v:L��X  I�3��{���I�H�I�fB  I�E��t
�`C  3����D  3���   H��$P  H3��M.  H��p  A_A^^][���������������@USWAVH�l$�H���   H�Ϡ  H3�H�EH�E�H�E�   H�D$@L�M�H�E�E��H�D$8L�E�H�E�H��H�D$0H�X  H�E�H�ًI,H�D$(H�E�H�D$ �RD  �����  �{,L��$�   L�e�v)A��r#E��L��W  �   3���C  �G   �  eH�%X   L�&N  ���  L��W  H��$   L��$�   E3�D�d$8H�4ȋߦ  �   H�L��$�   ��Hc�H�H�H��H�M�L9hH�E�H�D$0EO�H�E�3�H�D$(H�E�H�D$ �IC  L��E����   Mc�ff�     H��B  M��HcH�H�H�L��H�U�H���  �/B  H��tH��3�H���`  � �  9A��  �%�  H�Y��Hc�H�H�]�H�H��H�x  ��  H�KH�E��   H�D$ L�E�D�J��?  f��u
f9E���  H��A  M��HcH�H�H�L��H�U�H���  ��A  E���*���H��A  Mc�M��HcH�H�H�L��H�U�H���  �VA  H����  H�H����  H�RA  �9J��   H�xA  H�RLc H�H�J�L��H���  ��B  H���.  H� H���"  H�x �  �H;�  t;�  �   � H�M�H��G	  �|A  �M�Ԥ  ��Hc�H�H�H��H�H0H���tgL�H@L;�|^L�{U  �   3��fA  H�M��4A  �G   L��$�   H��$   L��$�   L��$�   H�MH3��*  H���   A^_[]�H�H(H���tL�H8L;�|	L�*U  �L�M�H�M�L�E�H�U��D$(   D�d$ ��������p������  H�]�L�e��EH�&@  H�]�E����HcH�H�L�t���   A��   t��@  ��p@  E�L�M�M���MI���   I��H�E��H�E�   �u@  H����   H�8 t~�x�  L���Hc�I� H��H�B8�_�  ��Hc�I� H��H��H�B@�2�  �(?  H��  H��G	  ��  H���?  H��  H�M�H��G	  ��?  �s���H���?>  �T���H�?  M��HcH�H�H�L��H�U�H���  �?  �â  ��Hc�H�H�L��I�A0H���tM�I@L;�|L��S  �����L�M�H�M�L�E�H�U��D$(    D�d$ �+����������H�]��Q�  H���@>  H��  L�}�H��G	  Hc@H�EH�,>  �H�[>  �MHcH�H�H�\���   ���  t�?  ���>  E�L�M�M���MH���   I��H�E��H�E�   ��>  H���$���H�8 �������  ��Hc�H�H�H��H�@@H�M��5>  ������������@SUH��(�I,L�D$@H��3�H��M  �W>  ����:  H�L$@H��M  D�7�  D�4�  H�|$PH�	��=  H��H��uH�|$P�C   H��(][Ë�  �   L�D$@H�t$HL�t$ I�9A��  eH�%X   D�   L�4�H�	=  L�HcI�H�H�D��H���  ���  H��H�H;�t>�    �{ t#H�H�QH��t�x�  9AuH9z0u��=  H�� H;�u�L�D$@I���=  H�t$H��t1H��<  L�D$@HcI�M� H�H�L��H�s���H���  �~<  L�t$ H�|$PH��(][������@SH�� �I,L�D$0H��H�|L  ��<  ���tKH�L$0H�L  D�ʟ  H�	�A=  H��u�C   H�� [�H�H�T$@��:  H�D$@H��C   H�� [�������������@UH��0�I,L�D$@H��H��K  �j<  ����B  H�L$@H��K  D�F�  H�\$HH�	��<  H��H��uH�\$H�E   H��0]�H�HL�t$(L�|$ E3�H��t[H�t$PA��fD;x}=H�|$XA���H�KH��   H��tH����;  H�K�C��H��  ;�|�H�|$X�n;  H�t$PL�{H�D�{$��8  �ȃ�v!��dtH�L��O  H�K0������E   �QH��   �i8  H�H�S�D8  H�H�S��8  fD9{~H��������uH����:  �L�{�E   L�t$(L�|$ H�\$HH��0]��@SH�� �I,L�D$0H��H��J  ��:  ���t=H�L$0H�)J  D�ڝ  H�	�Q;  H��u�C   H�� [�H�@H��C   H�� [�����������@VH��0�I,L�L$PH��L�D$@H��I  ��:  �����   H�L$@H��I  D�a�  H�	��:  H��H��t�@f��u!L��I  �   3��:  �F   H��0^�H�L$PH��H;�~	L��I  ��H��}	L�J  ��H�\$HH�l$(Hi�  H�|$ H������HjH�ǀ�/���� u�H�O H�����9  L��H������H��H�H�    �@   H�@    H�x�f1  H�l$(�D; H�|$ H�H�\$H�F  H��0^��@VH��   H���  H3�H�D$x�I,L�L$PH��L�D$HH��H  �C9  ����%  H�L$HH�nH  D��  H�	��9  L��H��t�@f��u L��H  �   3���8  �F   ��   H�L$PH��H;�~	L��H  ��H��}	L��H  ���   H�D$0    H�D$@H��$�   H�D$(L�L$Xf�T$ D�B�H��$�   ��I�
��5  H�D$XH�������H�ǀ<8 u�H�O H����!8  L��H�T$XH��H�H�    �@   H�@    H�x��/  �D; H��$�   H�H��$�   �F  H�L$xH3��p!  H�Ā   ^��������E3��h�����������A�   �U��������AVH��P�I,H�D$xL��H�D$ H��G  L�L$pL�D$`��7  ���tgH�L$`H��F  D���  H�\$HH�l$@H�	H�t$8��7  H��H��t�@f��u2L�G  �   3��.7  A�F   H�l$@H�\$HH�t$8H��PA^�3�H������f;�s�H�|$0��fff�     H�NH�T$pH���5  ��uH�kH�FH��H��  H;�r�H�|$0H���t�I�.A�F   �����������@SUVH�� H��L�L$PH��H��H  �I,L�D$X3�H�l$P��6  �����   H�L$XH�F  D�y�  D�v�  H�|$HH�	��5  H��H��td�~,�UfH�Hv5H9l$P@��L����2  f��v	L��J  �,H�|$H�C   H�� ^][�L�D$@�{3  f��v%L�nJ  3�H���T���H�|$H�C   H�� ^][�HcD$@H�|$HH��C   H�� ^][�����@SH�� �I,L�D$0H��H�<E  ��5  ���tuH�L$0H�)E  D���  D���  H�|$8H�	�5  H��H��t'H�PE3�H���1  f��v#L��D  3�H������H�|$8�C   H�� [�H�|$8�C   H�� [���@SH�� �I,L�D$0H��H��D  �
5  ���txH�L$0H��D  D��  D��  H�|$8H�	�e4  H��H��t*H�PA�   H��Z1  f��v#L�UD  3�H������H�|$8�C   H�� [�H�|$8�C   H�� [����������������H�\$H�|$ AVH�� A��L��H��L�D$0�I,H��H  �J4  �����   �,H�t$8uIH�L$0H��C  D��  D��  H�	��3  H��uA�F   �   �۾   �   E�H��<eH�%X   ��  �   H�ȋߖ  ��H�
Hc�H�H�4ȅ�uH��h�H��nH������f�H�ǀ<> u�H�O H����Q3  L��H��H��H�H�    �@   H�@    H�x�/+  �D; I�A�F  H�t$8H�\$@H�|$HH�� A^���������E3�������������A�   ���������@SH��@�I,H�D$`H�D$(L�L$0H�D$hH��L�D$PH�D$ H�gG  ��2  ����/  H�D$0H�|$XH��t|H��tL��G  �   H�L$PH��A  D���  H�	�3  H��H����   L�D$`�T$hH��0  f����   H�L�7G  H�O0趾��H�|$X�C   H��@[�H�L$PH��A  D�5�  D�2�  H�	��1  H��H��tZ��    t&L��F  �   3���1  H�|$X�C   H��@[�L�D$`�T$hH�H�~.  f��v#L��F  3�H������H�|$X�C   H��@[�H�|$X�C   H��@[������������@UVATH�l$�H��   �I,H�EH�D$PL�ME3�H�E�H�D$HL�EH�EL�eH�D$@H��H�EwL�egH�D$8H�;F  H�E�L�ewH�D$0H�EgH�D$(H�EH�D$ L�e�L�eL�e�L�eL�e�
1  �����  H�MH��@  D��  D��  L��$�   H�	�_0  L��H��u�F   �  �8   H��$�   �J��1  I�V�   L��H���H-  f���u)H���90  L��@  �   3��M0  �F   �Q  f���u&L��@  3�I��誼��H����/  �F   �%  H��$�   L��$�   L�}wM��tL9etL�MgM��tL9e�MD�L�Mg�L�MgH�]�H�UH��H��I��fE�fA���H��fD�\$@H�\$8I��fE�fA���H��fD�T$0H��L�|$(f�f���H��f�L$ H�fE�fA����=-  L��$�   f���uL��D  3�I���ֻ���%H�H�WfD�g��,  fD9g~H���ý����uH����.  �F   �%L�g�3�  H��L�w0D�g$�".  H��F	  H��$�   H��$�   L��$�   H�İ   A\^]�����@UVAWH�l$�H��   �I,H�EH�D$PL�M'E3�H�EH�D$HL�E/H�EL�}'H�D$@H��H�EL�}gH�D$8H��C  H�EwL�}H�D$0H�EgH�D$(H�EH�D$ L�}L�}L�}wL�}L�}�j.  �����  H�M/H��=  D�K�  D�H�  L��$�   H�	��-  L��H��u�F   �g  �8   H��$�   �J��u.  I�V�   L��H����*  f���u)H����-  L�>  �   3���-  �F   �  f���uL�>  3�I���
����   H�UL�EwH�MH��tH��tL�MgM��tM��MD�L�Mg�L�Mg�Ef�D$@H�EH�D$8f�L$0H�H�T$(H�U'fD�D$ D�E��*  f���uL�LB  3�I��芹���5H�H�WH��$�   fD���*  H��$�   fD9~H���g�����uH����,  �F   �%L��׏  H��L�w0D�$��+  H��F	  H��$�   L��$�   H�Ġ   A_^]�L��AVAWH��   �I,I�C�I�C�M�KI�C�L��I�C�M�C I�C�E3�I�C�H��A  I�C�M�{I�C�I�C�H�D$0I�C�H�D$(I�CH�D$ M�{�9,  �����  H��$�   H��;  D��  D��  H��$�   H�	��+  H��H��uH��$�   A�F   H�Ĩ   A_A^ú8   H��$�   �J��0,  H�U�   L��H���c(  f���u*H���T+  L��;  �   3��h+  A�F   �9  f���u'L��;  3�H���ķ��H���+  A�F   �  L�L$pH��$�   H��$�   H�\$hH��$�   H�|$`H��H��H��fE�fA���H��fD�\$@H�|$8I��fE�fA���H��fD�T$0H��H�\$(f�f���H��f�L$ H�fE�fA�����(  H��$�   f���uL��?  3�H���
����%H�H�VfD�~�(  fD9~~H���������uH���2*  A�F   �&L�~�f�  H��H�n0D�~$�U)  I�A�F	  H��$�   H��$�   H��$�   H�Ĩ   A_A^�������L��AUH��  �I,M�K3�M��h���I�CL��I�C H�:?  I�C�H�D$pI��p���H�D$hI�C�H�D$`I��x���H�D$XI�C�H�D$PI�C�H�D$HI�C�H�D$@I�C�H�D$8I�C�H�D$0I�C�H�D$(I�C H�D$ ��)  �����  H��$�   H�9  D�h�  D�e�  H��$(  H�	��(  H��$0  H��H��uH��$(  A�E   H��  A]ú8   L��$�   �J��|)  H�S�   L��L����%  f���u*I����(  L�!9  �   3���(  A�E   ��  f���u'L�"9  3�H������I���_(  A�E   �  H��$�   L��$�   H��$   H��$  H��$   H��$�   H��$�   L��$�   L��$�   L��$�   L��$�   I��H��I��f�f���H��f�l$`L�|$XH��f�f���H��f�|$PL�t$HH��fE�fA���H��fD�\$@H�t$8I��fE�fA���H��fD�T$0H��H�\$(f�f���H��f�L$ I�$fE�fA�����$  L��$�   L��$�   H��$   H��$  f���uH��$0  L��<  3��ٳ���+I�$I�T$3�fA�|$��$  fA9|$~I���������uI����&  A�E   �2I�|$H��$0  I�̋#�  I�D$0A�|$$�&  I�E A�E	  H��$�   L��$�   H��$(  H��  A]������@VAVH��(�I,L�L$@H��L�D$PE3�H��8  L�t$@��&  ����S  H�L$PH�66  D���  D���  H�\$HH�	�\$@H�l$X�&  H��H��uH�\$HH�l$X�F   H��(A^^ú8   H�|$ �J���&  H�U�   L��H����"  f���u)H����%  L�Y6  �   3���%  �F   �   f���uL�[6  3�H���I����JH�����#  f���uL�6;  3�H���$����%H�H�WfD�w�0#  fD9w~H��������uH���L%  �F   �%L�w���  H��H�o0D�w$�p$  H��F	  H�|$ H�\$HH�l$XH��(A^^����������������L��VAVH��   �I,I�C�I�C�M�K E3�I�CI�C�M�C�I�C�M�s I�C�H��I�CM�sI�C�H�k:  I�C�M�sI�C�M�s���$  �����  H�L$XH�`4  D�Ň  D�  H�l$xH�	�<$  H��H��uH�l$x�F   H�Ĉ   A^^ú8   H�|$p�J���$  H�U�   L��H���!  f���u)H���$  L��4  �   3��"$  �F   ��   f���uL��4  3�H�������   L��$�   L��$�   I��H��$�   H��I��fE�fA���H��fD�T$0H��L�\$(f�f���H��f�L$ H�fE�fA����N!  f���uL�19  3�H�������5H�H�WH��$�   fD�w�!  H��$�   fD9w~H��������uH���#  �F   �%L�w�T�  H��H�o0D�w$�C"  H��F	  H�|$pH�l$xH�Ĉ   A^^�����H��AVAWH��   �I,E3�L�x�L��L�x L�xL�xL�x�L�x�L�x�L�x��A������t��"  H�Ĩ   A_A^�H�D$hH�D$PL�L$`H��$�   H�D$HL��$�   H�D$pH�D$@H��7  H��$�   H�D$8H�D$xH�D$0H��$�   H�D$(H��$�   H�D$ �i"  �����  H��$�   H��1  D�F�  D�C�  H��$�   H�	��!  H��H��uH��$�   A�F   H�Ĩ   A_A^ú8   H��$�   �J��`"  H�U�   L��H����  f���u*H����!  L�2  �   3���!  A�F   �?  f���u'L�2  3�H�������H���C!  A�F   �  L��$�   H�T$`H��$�   H��$�   H��$�   H��$�   H��H��H��fE�fA���H��fD�\$@H�|$8I��fE�fA���H��fD�T$0H��H�\$(f�f���H��f�L$ H�fE�fA�����  H��$�   f���uL�n6  3�H���4����%H�H�VfD�~�@  fD9~~H���!�����uH���\   A�F   �&L�~���  H��H�n0D�~$�  I�A�F	  H��$�   H��$�   H��$�   H�Ĩ   A_A^�H��VAVH��   �I,E3�L�p H��L�pL�pL�p�L�p�L�p���t��t�*   H�Ĉ   A^^�H�D$PH�D$@L��$�   H��$�   H�D$8L�D$hH�D$XH�D$0H�v5  H��$�   H�D$(H�D$`H�D$ ��  �����  H�L$hH�8/  D���  D���  H�l$xH�	�  H��H��uH�l$x�F   H�Ĉ   A^^ú8   H�|$p�J���  H�U�   L��H����  f���u)H����  L�g/  �   3���  �F   ��   f���uL�i/  3�H���W����   L��$�   L��$�   I��H��$�   H��I��fE�fA���H��fD�T$0H��L�\$(f�f���H��f�L$ H�fE�fA�����  f���uL�94  3�H���ߪ���5H�H�WH��$�   fD�w��  H��$�   fD9w~H��輬����uH����  �F   �%L�w�,�  H��H�o0D�w$�  H��F	  H�|$pH�l$xH�Ĉ   A^^�������������L��VAUH���   �I,I�C�I�C�M�K�E3�I�C�H�D$PM�C�I�C�M�k H�D$HH��I�CM�kH�D$@H�`3  I�C�M�kH�D$8I�CH�D$0I�C�H�D$(I�C H�D$ M�k��v  ����  H��$�   H��,  D�S�  D�P�  H��$�   H�	L��$�   D�d$xL��$�   D�t$hL��$�   D�|$p��  H��H��u�F   �  �8   H��$�   �J��S  H�U�   L��H����  f���u)H���w  L��,  �   3���  �F   �"  f���u&L��,  3�H������H���7  �F   ��   L��$�   A��L��$�   fD�d$HfD�|$@H��$�   H��$�   H��H��I��fE�fA���H��fD�T$8I��H�\$0f�f���f�L$(H��H�fE�L�\$ fA�����  f���uL��1  3�H���C����%H�H�WfD�o�O  fD9o~H���0�����uH���k  �F   �%L�o��~  H��H�o0D�o$��  H��F	  H��$�   H��$�   L��$�   H��$�   L��$�   L��$�   H���   A]^�����������L��VAWH��   �I,I�C�I�C�M�KI�C H��I�C�M�C�I�C�E3�I�C�H��0  I�C�M�{I�C�I�C�H�D$0I�C�H�D$(I�CH�D$ M�{��  �����  H�L$hH�U*  D��}  D��}  H��$�   H�	��$�   H��$�   L��$�   D�t$`�  H��H��u�F   �T  �8   H��$�   �J���  H�U�   L��H����  f���u)H����  L�k*  �   3���  �F   ��   f���uL�m*  3�H���[����   L�\$pL�L$xI��H��$�   H��fD�t$@I��fE�f�\$8fA���H��fD�T$0H��L�\$(f�f���H��f�L$ H�fE�fA����e  f���uL�p/  3�H���ޥ���%H�H�WfD���  fD9~H���˧����uH���  �F   �%L��;|  H��H�o0D�$�*  H��F	  H��$�   H��$�   H��$�   L��$�   H�Ĩ   A_^������L��VAVH��   �I,I�C�I�C�M�K E3�I�CI�C�M�C�I�C�M�s I�C�H��I�CM�sI�C�H�.  I�C�M�sI�C�M�s���  �����  H�L$XH�(  D�u{  D�r{  H�l$xH�	��  H��H��uH�l$x�F   H�Ĉ   A^^ú8   H�|$p�J���  H�U�   L��H����  f���u)H����  L�?(  �   3���  �F   ��   f���uL�A(  3�H���/����   L��$�   L��$�   I��H��$�   H��I��fE�fA���H��fD�T$0H��L�\$(f�f���H��f�L$ H�fE�fA����  f���uL�Y-  3�H��跣���5H�H�WH��$�   fD�w��  H��$�   fD9w~H��蔥����uH����  �F   �%L�w�z  H��H�o0D�w$��  H��F	  H�|$pH�l$xH�Ĉ   A^^������%"  �����������%�  �����������%�  ����������������ff�     H;�r  �uH��f�����u��H���?   ���@SH�� H��3���  H����  ��  H�Ⱥ	 �H�� [H�%�  ��������H�L$H��8�   ��  ��t�   �)H��s  �   H�D$8H��t  H�D$8H��H�2t  H��t  H��r  H�D$@H� t  ��r  	 ���r     ��r     �   Hk� H��r  H�   �   Hk� H��q  H�L �   Hk�H��q  H�L H��  �����H��8����@SVWH��@H����  H���   3�E3�H�T$`H���Y  H��t9H�d$8 H�L$hH�T$`L��H�L$0L��H�L$pH�L$(3�H�\$ ��  �ǃ�|�H��@_^[������������H��(��t9��t(��t��t
�   H��(��  ���  ��H��(�I��H��(�   M����H��(�  H�\$H�t$H�|$ AVH�� H��L��3��  ��u3�H�\$0H�t$8H�|$HH�� A^���  �؈D$@@��=�v   ��   ��v     �@  ��tO�  �z  �  H��  H��  �
  ��u)��  ��t H��  H��  �
  ��v     @2����  @���[�����  H��H�8 t$H����  ��tL�ƺ   I��H�L�>  A���v  �   �����   �  �����H�\$H�t$WH�� @���u  3ۅ�3�H�\$0H�t$@H�� _��ȉ�u  ��  @���D$8�=�u  u5��  �  ��  ��u  �  @����  3�@����  ���Ë�랹   �  ����H��H�X L�@�PH�HVWAVH��@I����L���u98u  3���   �B���wEH�   H��u
�D$0   ��3  �؉D$0����   L�Ƌ�I�������؉D$0����   L�Ƌ�I���  �؉D$0��u8��u4L��3�I���u  L��3�I���L���H��  H��tL��3�I����  ��t��u@L�Ƌ�I�������؉D$0��t)H�K  H��u	�X�\$0�L�Ƌ�I���w  �؉D$0�3ۉ\$0��H�\$xH��@A^_^��H�\$H�t$WH�� I����H���u�   L�ǋ�H��H�\$0H�t$8H�� _�������H�\$ UH��H�� H��m  H�2��-�+  H;�utH�e H�M��  H�EH�E��  ��H1E�D  ��H�M H1E�t  �E H�MH�� H3E H3EH3�H�������  H#�H�3��-�+  H;�HD�H�m  H�\$HH��H��l  H�� ]�H��(��uH�=   u��  �   H��(��H�)s  H�%�  ��H�s  �*  H�s  �H�s  �H��(�����H������H�H��(��H��(��  ��t!eH�%0   H�H�H;�t3��H��r  u�2�H��(ð�����H��(�  ��t�  ��  ����  ��t2���  �H��(�H��(3��=  ����H��(����H��(�  ��u2���  ��u�  ��H��(�H��(�  �  �H��(����H�\$H�l$H�t$WH�� I��I����H����  ��u��uL��3�H��H����  H�T$X�L$PH�\$0H�l$8H�t$@H�� _��  H��(�  ��tH��q  H��(��  �ޙ����u��  H��(�H��(3���  H��(��  @SH�� ��q  �ɻ   DÈ�q  ��  �  ��u2���  ��u	3��  ���H�� [����@SH��@�=Tq   ����   ����   �  ��t(��u$H�6q  �9  ��uH�>q  �)  ��ts2��xH�rj  �@   ��?+�H���H��H3�H�D$ H�D$(D$ H�D$0�L$0�p  H�D$ H�D$(D$ H�D$0��p  �L$0�p  ��p  ��p  �H��@[ù   ��   �H��L���MZ  f9�y��uyHcz��H��y��H��9PE  u_�  f9AuTL+��AH�QH��AH��L��H�$I;�t�JL;�r
�B�L;�rH��(��3�H��u2���z$ }2��
��2��2�H������@SH�� ���  3҅�t��uH��o  H�� [�@SH�� �=�o   ��t��u����  ����  �H�� [��H��o  Ã%�o   �H�\$UH��$@���H���  �ٹ   �  ��t���)�   �����3�H�M�A��  �4  H�M���  H���   H���  H��E3���  H��t<H�d$8 H���  H���  L��H�L$0L��H���  H�L$(H�M�H�L$ 3���  H���  H�L$PH���   3�H���  A��   H��H���   �  H���  H�D$`�D$P  @�D$T   ��  ��H�D$PH�D$@H�E���H�D$H3��.  H�L$@�+  ��u��u�H����H��$�  H���  ]���H�\$WH�� H��>  H�=�>  �H�H��t�   H��H;�r�H�\$0H�� _�H�\$WH�� H�c>  H�=\>  �H�H��t��  H��H;�r�H�\$0H�� _����������  �H�\$H�t$WH��3��g     3��g     �D��3�D��A��ntelA��GenuD�ҋ�3ɍGE��A��ineI�$Eʉ\$D�ىL$�T$uPH��f  �%�?�=� t(=` t!=p t������ w$H�     H��sD�Im  A��D�>m  �D�5m  �   ;�|&3���$���\$�L$�T$��	sA��D�m  A��sp�1f     �+f     A��sUA��sN3��H�� H�H�T$ H�D$ $<u2��e  ����e     ��e  @�� t�� ��e     ��e  H�\$(3�H�t$0H��_�̸   ���3�9�e  ����������������%z  �%|  �%~  �%�  �%�  �%�  �%�  �%�  �%�  �%�  �%�  �%�  ���H��(M�A8H��I���   �   H��(����@SE�H��A���L��A� L��tA�@McP��L�Hc�L#�Ic�J�H�C�HH�C�Dt�D���L�L3�I��[�	������������������ff�     H��L�$L�\$M3�L�T$L+�MB�eL�%   M;��sfA�� �M�� ���A� M;��u�L�$L�\$H�����%i  ���������������ff�     ��@UH�� H��M@H�� ]������@UH�� H��������M8H�� ]�����@U