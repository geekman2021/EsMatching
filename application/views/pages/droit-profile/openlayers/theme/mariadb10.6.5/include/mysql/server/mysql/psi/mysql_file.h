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
    inline_mysql_file_ftelMZ       ÿÿ  ¸       @                                     º ´	Í!¸LÍ!This program cannot be run in DOS mode.
$       ¨Óßğì²±£ì²±£ì²±£åÊ"£æ²±£¾Ú°¢î²±£rv£ï²±£¾Ú´¢à²±£¾Úµ¢ä²±£¾Ú²¢ï²±£‰Ô°¢è²±£vÛ°¢ï²±£ì²°£b²±£vÛ¹¢í²±£vÛ±¢í²±£vÛN£í²±£vÛ³¢í²±£Richì²±£                        PE  d† ş±u_        ğ "  ~   v      0‚        €                        0    i  `A                                    Ø  d   „Ø  È     ˜	     ğ             ¨  @Â  T                    Ã  (    Â                €                          .text   S|      ~                    `.rdata  @V      X   ‚              @  @.data   ü   ğ      Ú              @  À.pdata  ğ         Ü              @  @.rsrc   ˜	     
   ê              @  @.reloc  ¨         ô              @  B                                                                                                                                                                                                                                                H9à  ÃÌÌÌÌÌÌÌÌ@SHƒì H‹YH…Û„Ş   H‹KH…ÉtlH‰l$03íH‰t$8‹õf;k}DH‰|$@‹ıf„     H‹KH‹„   H…ÀtH‹Èÿ.ƒ  H‹K¿CÿÆHÇ  ;ğ|ÑH‹|$@ÿƒ  H‹t$8H‰kH‹l$0H‹H…ÉtC‹Fæ  eH‹%X   A¸   H‹ĞM‹ H‹Ü‚  HcI‹ H‹TĞøö‚0  uº   ÿı  H‹K(H…Étÿ¦‚  H‹ËHƒÄ [Hÿ%—‚  HƒÄ [ÃÌÌÌÌÌÌÌÌÌH‰\$H‰t$H‰|$AVHƒì H‹y‹½å  eH‹%X   º   L‹4ÈH‹à  LòHcM‹I‹ H‹TÈøH‹šĞ  ‹²Ø  HÁæHóH;Şt5€{ t#H‹H‹QH…Òt‹Vå  9AuH9z0uÿ…‚  HƒÃ H;ŞuÎM‹H‹ò  HcI‹ H‹LÈøö0  u@H‹_H‹Ëÿñ~  fƒøÿuA¸   H‹Ó3Éÿš~  H‹ËÿÑ~  H‹Oÿ?  H‹ÿf  H‹Ïÿ  ‹çä  H‹\$0ÿÈH‹t$8H‹|$@HcĞI‹H‹H‹ÑHÿH@HƒÄ A^ÃÌÌÌÌÌÌH‰\$H‰t$H‰|$AVHƒì H‹y‹ä  eH‹%X   º   L‹4ÈH‹°€  LòHcM‹I‹ H‹TÈøH‹šĞ  ‹²Ø  HÁæHóH;Şt5€{ t#H‹H‹QH…Òt‹&ä  9AuH9z0uÿU  HƒÃ H;ŞuÎM‹H‹Â€  HcI‹ H‹LÈøö0  u@H‹_H‹ËÿÁ}  fƒøÿuA¸   H‹Ó3Éÿj}  H‹Ëÿ¡}  H‹Oÿ~  H‹ÿ6~  H‹Ïÿ  ‹·ã  M‹ÿÈH‹\$0H‹t$8H‹|$@HcÈI‹ H‹ÈHÿJ@‹ã  ÿÈHcÈI‹ H‹ÈHÿJ8HƒÄ A^ÃHƒì(ƒúuƒyH tH‹A0ë	H‹A(H…ÀtUH‰\$ HXH…ÛtBH‹Ëÿ{~  ƒøÿuP
H,ˆ  H‹\$ HƒÄ(Hÿ%¼  H‹ÓHˆ  H‹\$ HƒÄ(Hÿ%ê  H‹\$ HƒÄ(ÃÌÌÌÌÌÌÌÌƒúuƒyH tH‹A0ë	H‹A(H…ÀtHƒÀt	HÙ‡  ëQ‹½â  eH‹%X   º   H‹ÈH‹H‹T  HcH‹H‹LÈø€¹è   tº   H¤‡  Hÿ%  H¦‡  º   Hÿ%
  ÌÌÌÌÌÌÌÌÌÌHƒì(ƒúuƒyH tH‹A0ë	H‹A(H…Àt`HHH…ÉtWÿs}  …Àt6ƒètƒøuCPH{‡  HƒÄ(Hÿ%°~  º   HT‡  HƒÄ(Hÿ%™~  º   H-‡  HƒÄ(Hÿ%‚~  HƒÄ(ÃÌÌÌÌÌÌÌÌÌÌÌÌÌHƒì(ƒúuƒyH tH‹A0ë	H‹A(H…ÀtVH‰\$ HXH…ÛtCH‹ËÿÛ|  …Àº   Hû†  H‹\$ HƒÄ(Hÿ%~  H‹ÓHñ†  H‹\$ HƒÄ(Hÿ%I~  H‹\$ HƒÄ(ÃÌÌÌÌÌÌÌHƒì(ƒúuƒyH tH‹A0ëH‹A(H…À„½   H‰\$ HXH…Û„¦   H‹ËÿS|  …Àt}ƒèt\ƒèt;ƒøtH‹ÓHŞ†  H‹\$ HƒÄ(Hÿ%Î}  º   HŠ†  H‹\$ HƒÄ(Hÿ%j}  º   H–†  H‹\$ HƒÄ(Hÿ%N}  º   Hb†  H‹\$ HƒÄ(Hÿ%2}  º   H†  H‹\$ HƒÄ(Hÿ%}  H‹\$ HƒÄ(ÃÌÌÌÌÌÌÌÌÌÌÌÌ@SHƒì H‹Ùÿy|  D‹Fà  eH‹%X   ¹   HÇC8    J‹ÂH‰HƒÄ [ÃÌÌÌÌÌÌH‰\$WHƒì0‹Ù‹úHÛ£  ÿ|  D‹ÏLã†  3ÒHùÿÿÿT|  D‹ÏLÚ†  Hûùÿÿ‰Åß  3Òÿ5|  D‹ÏLË†  Hûÿÿ‰®ß  3Éÿ|  A¹   ˆ~Ù  L»†  ‰‰ß  H¶†  ‰|$ AQÿ`{  A¹   ‰|$ E3ÀH¤†  AQÿb|  º   ‰|$ H¢†  DJğDBîÿD|  º   ‰|$ Hœ†  DJïDBîÿ&|  º   ‰|$ H–†  DJóDB^ÿ|  º   ‰|$ H†  DJñDBğÿê{  A¹   ‰|$ E3ÀH††  AQÿÌ{  º   ‰|$ H„†  DJóDBñÿ®{  º   ‰|$ H~†  DJôDBøÿ{  º   ‰|$ DJïDBíHh†  ÿr{  º   ‰|$ Hj†  DJôDBóÿT{  A¹   ‰|$ E‹ÁHX†  º   ÿ5{  º   ‰|$ HU†  DJòDBóÿ{  º   ‰|$ HO†  DJôDB÷ÿùz  A¹   ‰|$ E3ÀH=†  AQÿÛz  º   ‰|$ H;†  DJëDBéÿ½z  º   ‰|$ H=†  DJñDBğÿŸz  A¹   ‰|$ E‹ÁH3†  º   ÿ€z  º   ‰|$ H0†  DJôDBùÿbz  º   ‰|$ H"†  DJôDBóÿDz  º   ‰|$ H†  DJõDBóÿ&z  º   ‰|$ H†  DJûDBùÿz  º   ‰|$ Hø…  DJøDBÿêy  ‰|$ º   Hê…  DJôLBğÿÌy  A¹   ‰|$ E‹ÁHØ…  º   ÿ­y  º   ‰|$ HÍ…  DJøDB÷ÿy  º   ‰|$ H¿…  DJüLBòÿqy  º   ‰|$ H©…  DJøLBïÿSy  º   ‰|$ H›…  DJ÷DBùÿ5y  º   ‰|$ H…  DJøDBùÿy  º
   ‰|$ H…  DJùLBñÿùx  º   ‰|$ Hq…  DJûDBÿÿÛx  º	   ‰|$ Hc…  DJúDBıÿ½x  º
   ‰|$ HU…  DJùDBşÿŸx  º
   ‰|$ HG…  DJùLBôÿx  º   ‰|$ H9…  DJöLBğÿcx  º   ‰|$ DJòLBëH#…  ÿEx  º   ‰|$ H%…  DJûDBÿ'x  º   ‰|$ H…  DJûDBÿ	x  º   ‰|$ H	…  DJöDBşÿëw  º   ‰|$ Hû„  DJöDBNÿÍw  º   ‰|$ Hí„  DJöDBOÿ¯w  º   ‰|$ Hß„  DJñDBKÿ‘w  º	   ‰|$ HÙ„  DJúLBïÿsw  º   ‰|$ HË„  DJ÷LBëÿUw  º   ‰|$ H½„  DJóLBæÿ7w  º   ‰|$ H·„  DJõDBóÿw  º
   ‰|$ H©„  DJùDBøÿûv  A¹   ‰|$ E3ÀH—„  AQÿİv  º   ‰|$ H•„  DJîDBìÿ¿v  ‰|$ º   H„  DJòDBñÿ¡v  A¹   ‰|$ E3ÀH…„  AQ	ÿƒv  º   ‰|$ H{„  DJ÷DBõÿev  A¹   ‰|$ E3ÀHi„  AQÿGv  º   ‰|$ Hg„  DJöDBôÿ)v  º
   ‰|$ HY„  DJùDB÷ÿv  A¹   ‰|$ E3ÀHG„  AQÿíu  H‹\$@3ÀHƒÄ0_ÃHƒì(‹ÒØ  eH‹%X   A¸   H‹ÈM‹ ‹ÁØ  ÿÈHcÈI‹ H‹ÈÇAHÿÿÿÿ‹¨Ø  ÿÈHcÈI‹ H‹ÈH‹B8H‰B@‹Ø  ÿÈHcÈI‹ H‹È3À‰Bhf‰Bl3Ò‹qØ  ÿÈHcÈI‹ A¸   H‹ÈHƒÁnèêk  3ÀHƒÄ(ÃÌÌÌÌÌÌÌ3ÀÃÌÌÌÌÌÌÌÌÌÌÌÌÌHƒì(‹Êÿüt  3ÀHƒÄ(ÃÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$WHƒìPH‹Ñ  H3ÄH‰D$@H‹ùÿ¶t  LGƒ  ¹   HCƒ  ÿet  eH‹%X   L=ƒ  ‹»×  º   H‹È‹¸×  HÚÿÈº    LcÈH‹H‹N‹ÉHL$ M‹I8ÿjt  LD$ ¹   Hù‚  ÿËt  ‹u×  LŞ‚  ÿÈº    LcÈH‹H‹N‹ÉHL$ M‹I@ÿ#t  LD$ ¹   HÊ‚  ÿ„t  LE~  ¹   HÁ‚  ÿkt  A¹P  L¾‚  º    HL$ ÿÖs  LD$ ¹   H¥‚  ÿ7t  ÿt  H‹Ïÿ0s  H‹L$@H3Ìè#]  H‹\$hHƒÄP_ÃÌÌÌÌÌÌÌÌH‰\$H‰t$ WHƒì@I‹ğL‹ÚH‹ÙH…Ét	L‹H‹QëE3ÒA‹ÒeH‹%X   ‹qÖ  A¸   H‹<ÈHL$P‹hÖ  IøÿÈH‰L$8LcÈI‹ÊH‹L‹ O‹ÈA¸ÿ  fD‰D$0M‹ÃIAnIƒÁhH‰D$(HD$`H‰D$ ÿÚo  fƒøvV‹Ö  LÏ  ÿÈHcĞH‹H‹H‹Ñº   HƒÁhÿÉr  ‹ëÕ  L¬  ÿÈHcĞH‹H‹H‹Ñº   HƒÁnÿr  L‹H…Û„•   ‹´Õ  ÿÈHcÈI‹ H‹È‹Bh‰C·Blº   f‰C‹Õ  ÿÈHcÈI‹ H‹ÈHKHƒÀn H‰€    H€€   A€HI@ A H°I°@ÀAÀHĞIĞ@àAàHğIğHƒêu­‹Õ  º   ÿÈHcÈI‹ L‹È3ÉIAhIƒÁnH…öt'H‰t$(LÛ€  H‰D$ ÿ°q  H‹\$XH‹t$hHƒÄ@_ÃLá€  H‰D$ ÿq  H‹\$XH‹t$hHƒÄ@_ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌ@SUATHì   H‹¥Í  H3ÄH‰„$à   H¿QH‹Ù¹  E3Àÿ„p  ‹bÔ  E3äH‰CA‹ìeH‹%X   º   H‹ÈH‹‹HÔ  ÿÈHcÈH‹H‹ÈH‹APH‰C‹.Ô  ÿÈHcÈH‹H‹È‹AX‰C fD;cõ  H‰´$(  ¹   H‰¼$0  Aº   L‰¬$8  L-ïÜÿÿL‰´$ø   L‰¼$ğ   L‹KDuHcÅ·ñH‹A¸ó  Hiø  HD$HL‰d$0H‰D$(LÏA·ÖfD‰T$ E‹üÿôm  H‹KA¸   HÁ  E3ÉHÏA·ÖH‰L$0H‹L‰d$(fD‰d$ ÿÀm  H‹SH‹„:  HƒÀ
Hƒøw(B¶„(&  A‹Œ… &  IÍÿáL‰¤:   éÊ  ¾õ  H‹HD$@H‰D$0E3ÉL‰d$(D·ÆA·ÖfD‰d$ ÿYm  fƒø†Ã   ¸õ  f;ğ…µ   ¾€   HL$`D‹Æ3Òèaf  H‹Nƒ3ÀL‰d$8‰D$PDFf‰D$TLL$PHD$`f‰t$0H‰D$(HD$LH‰D$ ÿ?m  fD;àu.HD$`Dˆ¤$ß   LL$PH‰D$ L«~  Dˆd$UV‚3Éÿ#o  H‹HD$@H‰D$0A¿   L‰d$(E3ÉA·ÖfD‰d$ EGÿdl  H‹SH‹L$@H‹„:  Hƒø÷u"H…ÉuHÇ„:  öÿÿÿH‹CL‰¤   é–   HƒøuH…ÉuHÇ„:  ÿÿÿÿH‹CL‰¤   ëqHƒøu	HƒÁH‰L$@E…ÿtH    H‰L$@HÿÁÿTn  H‹KA¸   A·ÖH‰„   L‹KH‹D$@I‰  M‹Œ9   HÏH‰L$(HÿÀH‹H‰D$ ÿ¥k  ¿KÿÅ;éAº   ¹   ŒmıÿÿL‹¼$ğ   L‹´$ø   L‹¬$8  H‹¼$0  H‹´$(  ¸   H‹Œ$à   H3Ìè_W  HÄ   A\][Ã Ä#  Ñ#  Ö#       ÌÌÌÌÌÌÌÌÌÌÌÌÌ‹®Ğ  9AuH‹AH9  u¸   Ã3ÀÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$WHƒìP‹I,LL$xA‹øH‹ÚLD$`H |  ÿ^m  ƒøÿ„¿   H‹L$`H‰|  D‹:Ğ  H‹	ÿ±m  L‹ĞH…Àt·@f…Àu&LÑ|  º   3Éÿìl  ÇC   H‹\$hHƒÄP_ÃH‹L$xH¿ÀH;È~	LÍ|  ëÊHƒù}	Lî|  ë»E3ÀHT$@A‹ÀH‰T$0L‰D$(…ÿfD‰D$ ·ÑI‹
•ÀfƒÀE3ÉD·Àÿj  H‹D$@H‰ÇC   H‹\$hHƒÄP_ÃÌÌÌHƒì(‹I,…ÉtH¢|  ÿpl  ƒøÿ„7  ‹eÏ  eH‹%X   H‰\$0H‰t$8H‰|$@L‰t$ L‹4ÈH‹yk  º   LòHcM‹I‹ H‹TÈøH‹šĞ  ‹ºØ  HÁçHûH;ßt/f€{ tH‹Hƒy t‹êÎ  9Auÿl  HƒÃ H;ßuÖM‹H‹k  HcI‹ H‹TÈøH‹šĞ  ‹²Ø  HÁæHóH;Ştl€{ t]H‹;Hƒ tS‹G;—Î  uH‹ÏÿÄk  ë=;€Î  u5H‹Ïÿ±k  H‹²j  L‹ÇHcI‹H‹H‹LÑøH¢ıÿÿHÁø  ÿ­j  HƒÃ H;Şu”L‹t$ H‹|$@H‹t$8H‹\$0HƒÄ(ÃÌÌÌ@SHƒì ‹I,LL$@H‹ÚLD$0HGz  ÿk  ƒøÿt?H‹L$0H4z  D‹åÍ  H‹	ÿ\k  H‹ÈH…ÀuÇC   HƒÄ [Ã‹D$@‰A ÇC   HƒÄ [ÃÌÌÌ@SHƒì ‹I,LL$@H‹ÚLD$0H×y  ÿ•j  ƒøÿtAH‹L$0HÄy  D‹uÍ  H‹	ÿìj  H‹ÈH…ÀuÇC   HƒÄ [ÃH‹D$@H‰AÇC   HƒÄ [ÃÌL‹ÜAVHƒì`‹I,ICÈL‹òI‰C¸HZz  MK MCÿj  ƒøÿ„f  H‹Œ$€   H”y  D‹ùÌ  D‹öÌ  H‰l$XH‹	ÿpi  H‹èH…ÀuH‹l$XAÇF   HƒÄ`A^Ãº8   H‰t$PH‰|$HL‰|$@JÉÿj  3öL‹ÀH‹øf‰pH‰p(NH‹UÿBf  fƒøşu(H‹Ïÿ3i  L´y  3ÉVÿIi  AÇF   é£  fƒøÿuL·y  3ÒH‹Íè¥õÿÿéÍ  H‹MLD$pº   H‰t$ DJüÿãe  f…ÀuN‹D$pƒà‰GtE‹Ì  eH‹%X   º   H‹ÈH‹‹Ì  ÿÈHcÈH‹º   L‹ÈH‹M‹@`ÿjf  ë‰wH‹”$ˆ   A¸ıÿÿÿH‹ÿŞe  ¿È…Ét)H‹Ly  ƒùH‹ÍtèøôÿÿAÇF   éÒ   èæôÿÿH‹HWH‰\$xÿ\e  H‹HWÿçe  H‹\$xf9w~H‹ÏèÄöÿÿ…Àu	éØ   H‰wH‹„$€   E3ÀH‹ÿAHH¿WH‰o0‰w$ÿIg  H‰G(f;w}D€    LcÆVIÁàLG(IHI@H‰D$(MHH‰L$ H‹ÿ»e  fƒøÿt?¿GÿÆ;ğ|Ã‹ÍÊ  H‹ÏÿÄf  I‰AÇF	  H‹|$HH‹t$PL‹|$@H‹l$XHƒÄ`A^ÃH‹Lx  H‹O0èëóÿÿH‹º   ÿ}d  H‹O(ÿ+g  H‹Ïÿ"g  AÇF   ë§@SUVAT¸˜  èĞ^  H+àH‹nÃ  H3ÄH‰„$p  ‹q,LL$`L‹âH‰T$X3ÛHÂw  ‹ÎLD$h‹ëÿg  ƒøÿ„M  H‹L$hHFv  D‹÷É  H‰¼$Ğ  L‰¬$  H‹	L‰´$ˆ  L‰¼$€  ÿNg  H‹øH…À„Ü  ·Pf…Ò   ƒşuLQw  3ÉSÿ~f  é³  H‹D$`H‹¿ÂD‹ID;È}LNw  ‰D$ º   3ÉÿMf  é‚  HQ$ÿæe  H¿WE3ÀAHÿte  ·O‹ÓH‹èf;Ù}HcÊÿÂHÁáÇDÿÿÿÿ¿O;Ñ|çA¸   E‹àfD;Áø  fH‹D$`H‹HQ$ÿ.e  H‹ğH…À„Ã  D¶pA¶ÆA€ştH‹Îÿòd  ¶F<…R  H‹L‹ıMcìA¹şÿÿÿIÁåAº   IƒÅğH‹HMıI‰AÇGÿÿÿÿH‹W(H‹B·*L‹AfƒÀfƒøfEGÊfD‰L$PIƒø†×   €y'…Í   A€|'…Á   HÇÀÿÿÿÿD  HÿÀ8\u÷L;À…S  IPşHƒÁÿpd  H‹ğHÇÁÿÿÿÿfD  8\HIuöˆH‹Îÿd  H‹Î…À…ï  3Òÿ¿c  A‰Gƒøÿ„{  H‹Îÿ™d  IcGA¸   D·L$PIÇœÿÿÿH‹O(L‰|$HH‰\$@H‰D$8B·D)f‰D$0J‹D)H‰D$(B·)ëCE:òuIÇÿÿÿÿH‹H‹W(L‰|$HHAH‰\$@E‹ÂH‰D$8B·D*f‰D$0J‹D*H‰D$(B·*H‹A·Ôf‰D$ ÿ6a  fƒøÿ„p  H‹D$`H‹HQ$ÿzc  ¿GAÿÄD;à
şÿÿL‹d$XH‹3Òÿ
a  fƒøÿuH‹Lbu  H‹O0èQğÿÿH‹ÿ a  ‰_$D·ğfƒøc…  €    H‹HT$Pÿš`  D·ğfƒøc…0  ‹L$PHT$pA¸   ÿ_b  …À~ËH‹HT$pLcÀÿš`  ‹L$PHT$pA¸   ÿ5b  …ÀÖëŸL‹ÎLt  º   3Éÿ8c  H‹º   ÿJ`  f;_}#@ HcÃHÀ‹LÅƒùÿtÿãa  ¿GÿÃ;Ø|áH‹ÍÿĞb  H‹Îé2  ÿÂb  H‹º   ÿü_  f;_  fHcÃHÀ‹LÅƒùÿtÿ“a  ¿GÿÃ;Ø|áéë   H‹Lt  H‹O0èïÿÿH‹º   ÿ«_  f;_À   HcÃHÀ‹LÅƒùÿtÿCa  ¿GÿÃ;Ø|áé›   Ls  º   3ÉÿHb  H‹º   ÿZ_  f;_}s@ HcÃHÀ‹LÅƒùÿtÿó`  ¿GÿÃ;Ø|áëNL(s  º   3Éÿûa  H‹º   ÿ_  f;_}&€    HcÃHÀ‹LÅƒùÿtÿ£`  ¿GÿÃ;Ø|áH‹Íÿa  H‹D$XÇ@   éê   A¿Î…Ét;ƒét#ƒùctH‹L#s  H‹O0èîÿÿAÇD$   ëH‹Ls  H‹O0èäíÿÿf9_~FH‹º   ÿp^  ‹óf;_}'„     HcÆHÀ‹LÅƒùÿtÿ`  ¿GÿÆ;ğ|áH‹Íÿğ`  fAƒşvfAƒşdu	AÇD$   f9_u>H‹HWÿ^  f9_~H‹Ïèpïÿÿ…ÀuH‹Ïÿ«`  ëH‰_ëL‹d$XAÇD$   L‹´$ˆ  L‹¬$  H‹¼$Ğ  L‹¼$€  H‹Œ$p  H3ÌèJ  HÄ˜  A\^][ÃAVHì€  H‹¸¼  H3ÄH‰„$`  ‹I,LD$PL‹òHp  ÿu`  ƒøÿ„  H‹L$PH o  D‹QÃ  H‰œ$   H‰¼$p  H‹	ÿ¸`  H‹ø»   H…À„½  H‹H0SHD$DD‹ËLD$@H‰D$ H‹IÿÄ\  fƒø‡  ·D$@f…À„‚  ·ÈÿÁH‰´$x  ÿ¡_  D·D$@LL$DH‹H‹ĞH‹ğÿ‡\  fƒø†Ö   H‹O0HD$HL‹LL$XH‰D$8ºÿ  f‰T$0HD$`H‹QH‹	H‰D$(HD$LH‰D$ ÿD\  DCHíp  HL$Xÿ6^  …ÀuY·T$@LŞp  L‹ÿÂH‹Îÿ0_  H‹DCûH‹Öÿğ\  f…ÀtH‹LÁp  H‹O0èxëÿÿé’   HÇÃÿÿÿÿHÿÃ€< u÷ë:HD$X‹ÓLL$`H‰D$ Ln  3ÉÿÁ^  ë^HÇÃÿÿÿÿ„     HÿÃ€< u÷HK Hƒáøÿ^  L‹ÃH‹ÖH‹øÇ    HHÇ@   HÇ@    H‰Xè_V  ÆD »  I‰>H‹ÎA‰^ÿ,^  H‹´$x  ëA‰^H‹œ$   H‹¼$p  H‹Œ$`  H3Ìè¶G  HÄ€  A^ÃÌÌÌÌÌÌÌÌÌÌÌÌ@SHì°  H‹Hº  H3ÄH‰„$   ‹I,LL$H3ÀLD$PH‹Úf‰D$@H;m  f‰D$Dÿô]  ƒøÿ„Ú   H‰¼$Ø  ·|$HGÿfƒøv$D¿ÏLzo  º   3Éÿ•]  ÇC   é˜   H‹L$PH5m  D‹šÀ  D‹—À  H‰´$Ğ  H‹	ÿ]  H‹ğH…ÀtV¹È   HD$DH‰D$8LD$`f‰L$0H„$Ğ   H‰D$(·×HD$@DIœH‰D$ H‹ÿæY  f…ÀtALún  3ÒH‹Îè€éÿÿÇC   H‹´$Ğ  H‹¼$Ø  H‹Œ$   H3ÌèiF  HÄ°  [Ãfƒ|$@ tÈfƒ|$D tÀ3ÒH‹Ëÿõ[  LL$`A¸   H£n  H‹Ëÿ2]  LŒ$Ğ   A¸   Hn  H‹Ëÿ]  ëÌÌÌÌÌÌÌÌÌÌL‹ÜVHƒì`‹I,ICÈI‰CÀMK ICĞH‹òMCI‰C¸HWn  ÿe\  ƒøÿ„ı  H‹Œ$€   Hİk  D‹B¿  D‹?¿  H‰l$PH‹	ÿ¹[  H‹èH…ÀuH‹l$PÇF   HƒÄ`^Ãº8   H‰|$HJÉÿl\  H‹U¹   L‹ÀH‹øÿŸX  fƒøşuLl  º   3Éÿ­[  ëfƒøÿu1L&l  3ÒH‹ÍèèÿÿH‹Ïÿc[  H‹|$HH‹l$PÇF   HƒÄ`^ÃH‹MLD$pº   L‰t$@E3öL‰t$ DJüÿ/X  f…ÀuM‹D$pƒà‰GtE‹d¾  eH‹%X   º   H‹ÈH‹‹T¾  ÿÈHcÈH‹AVL‹ÈH‹M‹@`ÿ·X  ëD‰wH‹”$ˆ   A¸ıÿÿÿH‹ÿêX  fƒøv(fƒødt"H‹Lìl  H‹ÍèDçÿÿH‹º   ÿÖW  ë*H‹HWH‰\$XÿBX  H‹\$XfD9w~H‹Ïèéÿÿ…ÀuH‹ÏÿYZ  ÇF   ë2L‰wH‹„$€   ‹†½  H‹ÿH‹ÏH‰o0D‰w$ÿpY  H‰ÇF	  L‹t$@H‹|$HH‹l$PHƒÄ`^ÃÌD‰D$H‰T$UWATAWH‹ìHƒìx‹I,LM(H‹úHÇE(ÿÿÿÿE3ÿH-l  LE@E‹çÿZ  ƒøÿ„J  H‹M@HDi  D‹õ¼  H‰\$pH‹](H‹	H‰t$hL‰t$XÿYZ  H‹ğH…ÀtfD9`uL{i  3ÉAWÿ—Y  ÇG   éß  A¾   H‹D9`t/HE8H‰D$ LM¸H…Û~AVL‹Ãÿ…W  ëA‹ÖM‹ÆÿwW  ëÿwW  fA;Æwª3ÒL‰l$`H‹ÏÿqX  H…Û~D9ft‰^$ëÿF$E‹ïfD;~_  ¹A  H‹VA·ŞIcÅLiø  I‹„  HƒøötgHƒøû~Hƒøş~+HƒøÿtUM‹´  Iƒşÿ…5  A¾   D‰uÈA¶Îéó   ‹N …ÉH‹VY  H‹H‰MÀÇEÈ   éĞ   ƒù¹A  u»şÿÿÿHƒÀ
Hƒø	wH£ÁsHƒ~ ~ÀM…äuH‹NHÿÁÿCX  H‹VL‹àL‹NHŠ  IÏAUIÿÁH‰L$(H‹D·ÃL‰L$ M‹ÌÿeU  fƒøÿ„G  fƒø…Ñ   H‹~HO HƒáøÿçW  L‹ÇI‹ÔH‹ØD‰0HHÇ@   HÇ@    H‰xèÈO  ÆD; H‹}0H‰]ÀÇEÈ  ±H‹VI×€: …–   €ù…   H‹UÀLEÀH‹ÿW  é™   I‹¼   IN HƒáøÿdW  M‹ÆH‹×H‹ØHHÇ    Ç@   HÇ@    L‰pèBO  BÆD3 A¾   énÿÿÿH‹FI‹¼  Hƒÿÿ…ÿÿÿH‹}0±D‰uÈéZÿÿÿIÇÀÿÿÿÿ@ IÿÀB€< uöH‹LMÀÿW  ¿FAÿÅ¹A  D;èŒÙıÿÿM…ät.I‹Ìÿ®V  ë#H‹Li  H‹N0èAãÿÿI‹ÌÿV  ÇG   L‹l$`H‹t$hH‹\$pL‹t$XHƒÄxA_A\_]ÃÌÌ@SHƒì H‹ÚèRüÿÿ€{uH‹åV  H‹ËL‹H‹HƒÄ [Hÿ%àV  HƒÄ [ÃÌÌÌÌÌÌÌÌÌÌéüÿÿÌÌÌÌÌÌÌÌÌÌÌH‰T$UWAVAWH‹ìHƒìx‹I,HE8L‹òH‰D$ 3ÿHZh  LM@H‰}8LE¸D‹ÿÿ)V  ƒøÿ„…  H‹M¸HUe  D‹¹  H‰\$pH‹]8H‹	H‰t$hÿoV  H‹ğH…Àtf9xuL’e  3ÉWÿ¯U  AÇF   é%  H‹M@€yt3ÒÿĞT  A¸   H‹9~t,HE(H‰D$ LMÀH…Û~APL‹Ãÿ‹S  ëA‹Ğÿ€S  ëÿ€S  fƒøw›L‰d$`H…Û~
9~t‰^$ëÿF$·ND‹çL‰l$Xf;ùv  A¸A  A¾   fH‹VA·ŞMcìIiı  H‹„  Hƒøöt\Hƒøû~Hƒøş~'HƒøÿtJL‹´  Iƒşÿ…d  A¾   D‰uĞéç   ‹N …ÉH‹_U  H‹H‰MÈÇEĞ   éÆ   ƒùuYıHƒÀ
Hƒø	wI£ÀsHƒ~ ~ÇM…ÿuH‹NHÿÁÿST  H‹VL‹øL‹NHŠ  HÏAT$IÿÁH‰L$(H‹D·ÃL‰L$ M‹ÏÿtQ  fƒøÿ„6  fƒø…  H‹~HO HƒáøÿöS  L‹ÇI‹×H‹ØD‰0HHÇ@   HÇ@    H‰xè×K  ÆD; H‰]ÈÇEĞ  H‹M@LEÈI‹ÕH‹	ÿzS  ¿NAÿÄA¸A  D;áŒœşÿÿL‹u0M…ÿtI‹ÏÿrS  ·NH¿ÁI‰AÇF   L‹l$XL‹d$`H‹\$pH‹t$hHƒÄxA_A^_]ÃH‹¼   IN Hƒáøÿ:S  A¸   H‹×H‹ØD‰ HHM‹ÆÇ@   HÇ@    L‰pèK  BÆD3 A¾   é2ÿÿÿH‹FH‹¼  Hƒÿÿ…áşÿÿD‰uĞéÿÿÿH‹L"e  H‹N0èaßÿÿI‹Ïÿ°R  H‹E0Ç@   éAÿÿÿÌÌÌÌÌÌÌÌ@VWHƒìHH‹úHÇD$p   H‹ñHÓd  ‹I,LL$pLD$xÿ¸R  ƒøÿt_H‹L$xHça  D‹˜µ  H‰\$hH‰l$@H‹	H‹l$pÿ S  H‹ØH…Àtfƒx u,L"b  º   3Éÿ=R  H‹\$hH‹l$@ÇG   HƒÄH_^Ãƒx H‹t5ƒ~,HD$`H‰D$ LL$0vº   L‹Åÿ!P  ëº   D‹ÂÿP  ëÿP  fƒøwƒ~,v‰k$H‹\$hH‹l$@ÇG   HƒÄH_^ÃÿC$H‹\$hH‹l$@ÇG   HƒÄH_^ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌ@SUVATAUAVHƒìh‹I,LŒ$¸   »   LD$@L‹òE3íH¬c  D·ãA‹õkşÿxQ  ;Å„  H‹„$¸   H‰¼$¨   ¶H€ùu	H‹0HƒÆë€ùtH‹Èÿ¸P  H‹„$¸   ‹(ÿÍH‹L$@Hh`  D‹´  L‰|$`H‹	ÿ‹Q  H‹øH…À„’   ·@f…Àu	L§`  ësH…öt^L9ou	Lc  ë_A‹İfD;è})HcÃH‹ÖHiÈ  HOÿ™O  …Àt¿GÿÃ;Ø|Ûë‹ë…íyYL‹ÎLéb  º   3ÉÿlP  ë˜;è}…íy<Lâb  º   3ÉÿMP  AÇF   L‹|$`H‹¼$¨   HƒÄhA^A]A\^][Ã»   D9o$u7H‹D9otH„$    ‹ÓLL$PH‰D$ L‹Ãÿ!N  ëÿ!N  fƒøwÿG$H‹WHcÅLiø  J‹Œ:  HƒùötBHƒùû~Hƒùş~!Hƒùÿt0J‹´:  Hƒşÿ…E  A‰^éZÿÿÿ‹G A¼şÿÿÿƒøfDOã…À~*HƒÁ
Hƒù	‡Œ  ¸A  H£Èƒ}  H‹GH…Àf  ¸ÿ  fAƒüA”ÅLèI‹ÍL‰¬$°   ÿ&O  H‹Œ$°   UL‹GL‹ÈIÀ  H‹ğMÇL‰D$(E·ÄH‰L$ H‹ÿHL  ·Øfƒøÿt{fÿÅA½ÿ  fD  H‹OI‹”  H‹ÎHƒúÿ„î  fƒûIDÕÿ™N  f…Û„¯   H‹OL‹ÎH‹„$°   HÁ  IÏE·ÄH‰L$(·ÕH‹H‰D$ ÿÍK  ·Øfƒøÿu”H‹L²`  H‹O0èñÚÿÿH‹Îÿ@N  éşÿÿJ‹¼:   HN Hƒáøÿ5N  L‹ÆH‹×H‹ØHHÇ    Ç@   L‰hH‰pèF  Dˆl3I‰AÇF  éÌıÿÿH‹ÎÿàM  ¹   A‰NéµıÿÿH‰„$°   ë<fAƒüşU¹   A¸   fDDÁHŒ$°   H‰L$0E3ÉH‹L‰l$(fD‰l$ ÿLK  H‹_¸   H…ÛHNØH‰œ$°   HK HƒáøÿvM  UE·ÄH‹ğÇ    Ç@   L‰hH‰XHƒÀH‹Œ$°   L‹OIÁ  H‰D$HMÏL‰L$(L‹ÈH‰L$ H‹ÿzJ  ·ÈfƒøÿuH‹L__  H‹O0èÙÿÿëLH‹GIÇHƒ¸  ÿu5¶F¨u H‹Î¨tÿvK  AÇF   é¤üÿÿÿ»L  AÇF   é‘üÿÿfƒùdu"¶F¨…wüÿÿH‹Î¨„Kşÿÿÿ3K  éaüÿÿH‹€  HƒøÿtHƒøöuHÿŒ$°   fƒù„   öFH‹GI‹œ  u7‹ƒøu,HS H‹ÎHƒâøÿ§L  H‹ğH‰XL‰hI‰AÇF  éÿûÿÿÿÈ‰HK HƒáøÿL  H‹T$HLCH‹ğHHÇ    Ç@   L‰hH‰XèùC  I‰6AÇF  é³ûÿÿÌÌÌÌÌÌÌÌL‹ÜWAUAVAWHƒìXE3íIC L‹úM‰kH‹ùM‰k ‹I,H{^  MKI‰C¨MC¸E‹õÿÖK  ƒøÿ„[  H‹L$0H[  D‹²®  H‰œ$ˆ   H‹	ÿ!L  H‹ØH…ÀtfD9hu LC[  3ÉAUÿ_K  AÇG   é   H‹H‰l$PL‰d$@A¼   D9ht H„$€   A‹ÔLL$8H‰D$ E‹ÄÿCI  ëÿCI  ·èfA;ÄvHÃ]  ÿK  M‰/AÇG   é‘  H‰t$HD9g,uH³]  ÿíJ  ëH‹”$   H¬]  ÿÖJ  I‹ıfD;ks,I‹õH‹SH]  HÖÿ´J  H¿CHÿÇHÆ  H;ørØH…]  ÿ“J  fA;ì‡æ  ÿC$Hs]  ÿyJ  I‹ífD;kƒ0  I‹õH‹SA·üH‹„  HƒøötFHƒøû~Hƒøş~Hƒøÿt4Hƒ¼  ÿuPHf]  éĞ   ‹K …ÉH]  é½   A;Ì¿şÿÿÿHƒÀ
Hƒø	w4¹A  H£Ás)L9k#Hê\  éŒ   H‹”   H&]  ÿĞI  ë{M…öuH‹KÿI  H‹SL‹ğHŠ  M‹ÎHÎUH‰L$(D·ÇH‹KH‰L$ H‹ÿµF  H\  ·øÿ}I  fƒÿÿ„   fA;üunH‹SI‹ÎÿI  H¡\  ÿSI  H¿CHÿÅHÆ  H;è‚ÓşÿÿH \  ÿ.I  H‹D9ktAH„$€   A‹ÔLL$8H‰D$ M‹ÄÿG  ·èélşÿÿH‹CH‹”  Hƒúÿu„H\  ë‹ÿöF  ·èéCşÿÿH‹LìZ  H‹K0è+ÕÿÿHÜ[  ÿ¶H  I‹ÎÿmH  AÇG   ë*Hü[  ÿ–H  M…öt	I‹ÎÿHH  HcC$I‰AÇG   H‹t$HH‹l$PL‹d$@H‹œ$ˆ   HƒÄXA_A^A]_ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌ@UHƒì0‹I,LD$@H‹êHÌW  ÿ:H  ƒøÿ„Ë   H‹L$@HeW  D‹«  H‰\$HH‹	ÿˆH  H‹ØH…ÀuH‹\$HÇE   HƒÄ0]ÃH‹HH…ÉtmH‰t$PL‰t$ E3öA‹öfD;p}BH‰|$(A‹şfD  H‹KH‹„   H…ÀtH‹Èÿ^G  H‹K¿CÿÆHÇ  ;ğ|ÑH‹|$(ÿ>G  H‹t$PL‰sL‹t$ H‹L$@H‹	ÿªG  H‹\$HÇE   HƒÄ0]ÃÌÌÌÌÌÌÌÌE3Àéˆ  ÌÌÌÌÌÌÌÌA¸   éu  ÌÌÌÌÌ@SUVAVAWHìp  H‹B£  H3ÄH‰„$P  D‹¼$È  L‹ñI‹éI‹ØH‹ò¹(  E…ÿtÿG  ëÿ«F  3ÒI‰A¸(  H‹Èè|=  I‹D‰¸   I‹ÿÕC  I‹HQH‹	ÿ]D  Hc„$À  ƒøt&I‹L‹Àºn   H‹Iÿ3C  f…ÀtLçY  éU  L‰¤$`  HçY  E3äH‹ÎL‰d$@ÿ’D  H…À„Ú   H‰¼$h  I|$ÿH…ÛtMH¸Y  H‹ÎÿgD  H…Àu8H§Y  H‹ÎÿRD  H…Àu#H‰l$(L‘Y  L‹ÎH‰\$ 3ÒHL$@ÿÄE  ë-H‹ÇHÿÀD8$u÷ÿÀHcØH‹Ëÿ E  L‹ÃH‹ÖH‹ÈH‰D$@è–=  L‹D$@ºÿ  HÿÇE8$8u÷I‹HD$HfD‰d$8L‹ÏH‰D$0HD$Pf‰T$(3ÒH‹IH‰D$ ÿÁB  H‹¼$h  ë*I‹¸ıÿÿÿf‰D$0D‹ÀH‰l$(L‹ËH‹Öf‰D$ H‹Iÿ•B  H‹L$@·ØL‹¤$`  H…ÉtÿêD  fƒûv:LÅX  I‹3Òè{ÑÿÿI‹H‹IÿfB  I‹E…ÿt
ÿ`C  3Àëÿ®D  3Àë¸   H‹Œ$P  H3ÌèM.  HÄp  A_A^^][ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌ@USWAVHl$ÁHìè   H‹Ï   H3ÄH‰EHEßHÇEß   H‰D$@LM÷HEçE‹ğH‰D$8LE×HEÇH‹úH‰D$0HX  HEïH‹Ù‹I,H‰D$(HEÏH‰D$ ÿRD  ƒøÿ„ì  ƒ{,L‰¤$à   L‹eßv)Aƒür#E‹ÌL×W  º   3ÉÿòC  ÇG   é©  eH‹%X   L&N  ‹ü¦  LÁW  H‰´$   L‰¬$Ø   E3íD‰d$8H‹4È‹ß¦  º   HòL‰¼$Ğ   ÿÈHcĞH‹H‹H‹ÑHM§L9hH‹EÇH‰D$0EOîH‹EÏ3ÒH‰D$(H‹E×H‰D$ ÿIC  L‹ğE…í„ã   Mcşff„     H‹¡B  M‹ÇHcH‹H‹H‹LĞøH‹U§HÁø  ÿ/B  H…ÀtH‹ë3ÉH…É„`  ‹ ¦  9A…—  ‹%¦  H‹YÿÈHcĞH‹H‰]·H‹H‹ÑHƒx  „Ô  H‹KHE¯º   H‰D$ LEÿDJÿ‹?  f…Àu
f9E¯…¥  H‹ıA  M‹ÇHcH‹H‹H‹LÑøH‹U§HÁø  ÿóA  E…í…*ÿÿÿH‹ËA  McöM‹ÆHcH‹H‹H‹LÑøH‹U§HÁÀ  ÿVA  H…À„¤  H‹H…Ò„˜  H‹RA  ‹9J…»   H‹xA  H‹RLc H‹H‹J‹LÁøHÁÀ  ÿ™B  H…À„.  H‹ H…À„"  Hƒx „  ‹H;ğ¤  t;ä¤  …   ÿ H‹M§H‰ÇG	  ÿ|A  ëM‹Ô¤  ÿÈHcĞH‹H‹H‹ÑH‹H0HƒùÿtgL‹H@L;É|^L{U  º   3ÉÿfA  H‹M§ÿ4A  ÇG   L‹¬$Ø   H‹´$   L‹¼$Ğ   L‹¤$à   H‹MH3Ìè¹*  HÄè   A^_[]ÃH‹H(HƒùÿtL‹H8L;É|	L*U  ëL‹MÇHM·L‹EÏH‹U×ÇD$(   D‰d$ èÎùÿÿ…À„pÿÿÿ‹ô£  H‹]·L‹e§‰EH‹&@  H‰]ÇEÿÿÿÿHcH‹H‹L‹tÑø¹   Aö†   tÿĞ@  ëÿp@  EÿLM·M‹ÇòMIø   I‹ÔH‰E·òHÇE¿   ÿu@  H…À„„   Hƒ8 t~‹x£  L‹ÿÈHcÈI‹ H‹ÈHÿB8‹_£  ÿÈHcÈI‹ H‹ÈH‹ËHÿB@‹2£  ÿ(?  H‰ƒ  H‰ÇG	  ‹£  H‹Ëÿ?  H‰ƒ  H‹M§H‰ÇG	  ÿ¥?  ésşÿÿH‹Ëÿ?>  éTşÿÿH‹?  M‹ÆHcH‹H‹H‹LÑøH‹U§HÁÀ  ÿ?  ‹Ã¢  ÿÈHcĞH‹H‹L‹ÑI‹A0HƒøÿtM‹I@L;È|L²S  éêıÿÿL‹MÇHM·L‹EÏH‹U×ÇD$(    D‰d$ è+øÿÿ…À„ÍıÿÿH‹]·‹Q¢  H‹Ëÿ@>  H‰ƒ  L‹}§H‰ÇG	  Hc@H‰EH‹,>  ‹H‹[>  ‰MHcH‹H‹H‹\Ñø¹   öƒÈ  tÿ?  ëÿ®>  EÿLM·M‹ÆòMH‹À   I‹×H‰E·òHÇE¿   ÿ³>  H…À„$ıÿÿHƒ8 „ıÿÿ‹²¡  ÿÈHcĞH‹H‹H‹ÑHÿ@@H‹M§ÿ5>  éıÿÿÌÌÌÌÌÌÌÌ@SUHƒì(‹I,LD$@H‹Ú3íHéM  ÿW>  ƒøÿ„:  H‹L$@HÒM  D‹7¡  D‹4¡  H‰|$PH‹	ÿ®=  H‹øH…ÀuH‹|$PÇC   HƒÄ(][Ã‹¡  º   L‹D$@H‰t$HL‰t$ I‹9A‹é   eH‹%X   Dêº   L‹4ÈH‹	=  LòHcI‹H‹H‹DÑøH‹˜Ğ  ‹°Ø  HÁæHóH;Şt>€    €{ t#H‹H‹QH…Òt‹x   9AuH9z0uÿ§=  HƒÃ H;ŞuÎL‹D$@I‹ÿ=  H‹t$H…ít1H‹ˆ<  L‹D$@HcI‹M‹ H‹H‹LÑøHsÏÿÿHÁø  ÿ~<  L‹t$ H‹|$PHƒÄ(][ÃÌÌÌÌÌ@SHƒì ‹I,LD$0H‹ÚH|L  ÿê<  ƒøÿtKH‹L$0HL  D‹ÊŸ  H‹	ÿA=  H…ÀuÇC   HƒÄ [ÃH‹HT$@ÿ‘:  H‹D$@H‰ÇC   HƒÄ [ÃÌÌÌÌÌÌÌÌÌÌÌÌ@UHƒì0‹I,LD$@H‹êHüK  ÿj<  ƒøÿ„B  H‹L$@H•K  D‹FŸ  H‰\$HH‹	ÿ¸<  H‹ØH…ÀuH‹\$HÇE   HƒÄ0]ÃH‹HL‰t$(L‰|$ E3ÿH…Ét[H‰t$PA‹÷fD;x}=H‰|$XA‹ÿH‹KH‹„   H…ÀtH‹Èÿ;  H‹K¿CÿÆHÇ  ;ğ|ÑH‹|$Xÿn;  H‹t$PL‰{H‹D‰{$ÿØ8  ¿Èƒùv!ƒùdtH‹LÙO  H‹K0èàÇÿÿÇE   ëQH‹º   ÿi8  H‹HSÿD8  H‹HSÿÏ8  fD9{~H‹Ëè°Éÿÿ…ÀuH‹Ëÿë:  ëL‰{ÇE   L‹t$(L‹|$ H‹\$HHƒÄ0]ÃÌ@SHƒì ‹I,LD$0H‹ÚHŒJ  ÿú:  ƒøÿt=H‹L$0H)J  D‹Ú  H‹	ÿQ;  H…ÀuÇC   HƒÄ [ÃH¿@H‰ÇC   HƒÄ [ÃÌÌÌÌÌÌÌÌÌÌ@VHƒì0‹I,LL$PH‹òLD$@HÇI  ÿ…:  ƒøÿ„ı   H‹L$@H°I  D‹a  H‹	ÿØ:  H‹ĞH…Àt·@f…Àu!LøI  º   3Éÿ:  ÇF   HƒÄ0^ÃH‹L$PH¿ÀH;È~	LùI  ëÏHƒù}	LJ  ëÀH‰\$HH‰l$(Hié  H‰|$ HÇÇÿÿÿÿHjHÿÇ€¼/èşÿÿ uóHO HƒáøÿŒ9  L‹ÇH•èşÿÿH‹ØHHÇ    Ç@   HÇ@    H‰xèf1  H‹l$(ÆD; H‹|$ H‰H‹\$HÇF  HƒÄ0^ÃÌ@VHì€   H‹ˆ•  H3ÄH‰D$x‹I,LL$PH‹òLD$HH…H  ÿC9  ƒøÿ„%  H‹L$HHnH  D‹œ  H‹	ÿ–9  L‹ĞH…Àt·@f…Àu L¶H  º   3ÉÿÑ8  ÇF   éØ   H‹L$PH¿ÀH;È~	L¸H  ëĞHƒù}	LÙH  ëÁº   HÇD$0    HD$@H‰œ$    H‰D$(LL$Xf‰T$ DBïH‰¼$¨   ·ÑI‹
ÿõ5  HD$XHÇÇÿÿÿÿHÿÇ€<8 u÷HO Hƒáøÿ!8  L‹ÇHT$XH‹ØHHÇ    Ç@   HÇ@    H‰xèı/  ÆD; H‹¼$¨   H‰H‹œ$    ÇF  H‹L$xH3Ìèp!  HÄ€   ^ÃÌÌÌÌÌÌÌE3ÀéhÊÿÿÌÌÌÌÌÌÌÌA¸   éUÊÿÿÌÌÌÌÌAVHƒìP‹I,HD$xL‹òH‰D$ HëG  LL$pLD$`ÿ«7  ƒøÿtgH‹L$`HÚF  D‹‹š  H‰\$HH‰l$@H‹	H‰t$8ÿó7  H‹ğH…Àt·@f…Àu2LG  º   3Éÿ.7  AÇF   H‹l$@H‹\$HH‹t$8HƒÄPA^Ã3ÛHÇÅÿÿÿÿf;ØsÔH‰|$0‹ûfff„     H‹NH‹T$pHÏÿŞ5  …ÀuHkH¿FHÿÃHÇ  H;ØrÒH‹|$0Hƒıÿt‰I‰.AÇF   ë„ÌÌÌÌÌÌÌÌÌÌ@SUVHƒì H‹ÚLL$PH‹ñH¶H  ‹I,LD$X3íH‰l$Pÿ™6  ƒøÿ„¼   H‹L$XHF  D‹y™  D‹v™  H‰|$HH‹	ÿğ5  H‹øH…Àtdƒ~,UfH‹Hv5H9l$P@•ÅL‹Åÿá2  fƒøv	LŒJ  ë,H‹|$HÇC   HƒÄ ^][ÃLD$@ÿ{3  fƒøv%LnJ  3ÒH‹ÏèTÂÿÿH‹|$HÇC   HƒÄ ^][ÃHcD$@H‹|$HH‰ÇC   HƒÄ ^][ÃÌÌÌÌ@SHƒì ‹I,LD$0H‹ÚH<E  ÿª5  ƒøÿtuH‹L$0H)E  D‹˜  D‹‹˜  H‰|$8H‹	ÿ5  H‹øH…Àt'H‹PE3ÀH‹ÿı1  fƒøv#LøD  3ÒH‹Ïè¦ÁÿÿH‹|$8ÇC   HƒÄ [ÃH‹|$8ÇC   HƒÄ [ÃÌÌ@SHƒì ‹I,LD$0H‹ÚHœD  ÿ
5  ƒøÿtxH‹L$0H‰D  D‹î—  D‹ë—  H‰|$8H‹	ÿe4  H‹øH…Àt*H‹PA¸   H‹ÿZ1  fƒøv#LUD  3ÒH‹ÏèÁÿÿH‹|$8ÇC   HƒÄ [ÃH‹|$8ÇC   HƒÄ [ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰|$ AVHƒì A‹ØL‹òH‹ùLD$0‹I,HÌH  ÿJ4  ƒøÿ„ğ   ƒ,H‰t$8uIH‹L$0HºC  D‹—  D‹—  H‹	ÿ›3  H…ÀuAÇF   é«   …Û¾   ¹   EñHğë<eH‹%X   ‹â–  º   H‹È‹ß–  ÿÈH‹
HcÈH‹H‹4È…ÛuHƒÆhëHƒÆnHÇÇÿÿÿÿfHÿÇ€<> u÷HO HƒáøÿQ3  L‹ÇH‹ÖH‹ØHHÇ    Ç@   HÇ@    H‰xè/+  ÆD; I‰AÇF  H‹t$8H‹\$@H‹|$HHƒÄ A^ÃÌÌÌÌÌÌÌÌE3Àé¸şÿÿÌÌÌÌÌÌÌÌA¸   é¥şÿÿÌÌÌÌÌ@SHƒì@‹I,HD$`H‰D$(LL$0HD$hH‹ÚLD$PH‰D$ HgG  ÿá2  ƒøÿ„/  H‹D$0H‰|$XHƒèt|HƒøtL›G  é¥   H‹L$PHêA  D‹›•  H‹	ÿ3  H‹øH…À„Ç   L‹D$`·T$hH‹ÿ0  fƒø†¼   H‹L7G  H‹O0è¶¾ÿÿH‹|$XÇC   HƒÄ@[ÃH‹L$PHĞA  D‹5•  D‹2•  H‹	ÿ±1  H‹øH…ÀtZƒ¸    t&L™F  º   3ÉÿÜ1  H‹|$XÇC   HƒÄ@[ÃL‹D$`·T$hH‹Hÿ~.  fƒøv#L‰F  3ÒH‹Ïè¾ÿÿH‹|$XÇC   HƒÄ@[ÃH‹|$XÇC   HƒÄ@[ÃÌÌÌÌÌÌÌÌÌÌÌ@UVATHl$¹Hì°   ‹I,HEH‰D$PLME3äHEÿH‰D$HLEHEL‰eH‰D$@H‹òHEwL‰egH‰D$8H;F  HE÷L‰ewH‰D$0HEgH‰D$(HEH‰D$ L‰eÿL‰eL‰e÷L‰eL‰eÿ
1  ƒøÿ„ù  H‹MH†@  D‹ë“  D‹è“  L‰´$˜   H‹	ÿ_0  L‹ğH…ÀuÇF   é³  º8   H‰¼$    JÉÿ1  I‹V¹   L‹ÀH‹øÿH-  fƒøşu)H‹Ïÿ90  Lº@  º   3ÉÿM0  ÇF   éQ  fƒøÿu&L¼@  3ÒI‹Îèª¼ÿÿH‹Ïÿù/  ÇF   é%  H‰œ$¨   L‰¼$   L‹}wM…ÿtL9etL‹MgM…ÉtL9e÷MDÌL‰MgëL‹MgH‹]ÿH‹UH‹ÃH÷ØI‹ÇfEÛfAƒãıH÷ØfD‰\$@H‰\$8I‹ÁfEÒfAƒâıH÷ØfD‰T$0H‹ÂL‰|$(fÉfƒáıH÷Øf‰L$ H‹fEÀfAƒàıÿ=-  L‹¼$   fƒøÿuLˆD  3ÒI‹ÎèÖ»ÿÿë%H‹HWfD‰gÿâ,  fD9g~H‹ÏèÃ½ÿÿ…ÀuH‹Ïÿş.  ÇF   ë%L‰g‹3’  H‹ÏL‰w0D‰g$ÿ".  H‰ÇF	  H‹œ$¨   H‹¼$    L‹´$˜   HÄ°   A\^]ÃÌÌÌÌ@UVAWHl$¹Hì    ‹I,HEH‰D$PLM'E3ÿHEH‰D$HLE/HEL‰}'H‰D$@H‹òHEL‰}gH‰D$8H›C  HEwL‰}H‰D$0HEgH‰D$(HEH‰D$ L‰}L‰}L‰}wL‰}L‰}ÿj.  ƒøÿ„­  H‹M/Hæ=  D‹K‘  D‹H‘  L‰´$   H‹	ÿ¿-  L‹ğH…ÀuÇF   ég  º8   H‰¼$˜   JÉÿu.  I‹V¹   L‹ÀH‹øÿ¨*  fƒøşu)H‹Ïÿ™-  L>  º   3Éÿ­-  ÇF   é  fƒøÿuL>  3ÒI‹Îè
ºÿÿé²   H‹UL‹EwH‹MH…ÒtH…ÉtL‹MgM…ÉtM…ÀMDÏL‰MgëL‹Mg·Ef‰D$@H‹EH‰D$8f‰L$0H‹H‰T$(H‹U'fD‰D$ D·Eÿ©*  fƒøÿuLLB  3ÒI‹ÎèŠ¹ÿÿë5H‹HWH‰œ$È   fD‰ÿ*  H‹œ$È   fD9~H‹Ïèg»ÿÿ…ÀuH‹Ïÿ¢,  ÇF   ë%L‰‹×  H‹ÏL‰w0D‰$ÿÆ+  H‰ÇF	  H‹¼$˜   L‹´$   HÄ    A_^]ÃL‹ÜAVAWHì¨   ‹I,ICÀI‰C˜MKIC¨L‹òI‰CMC ICÈE3ÿI‰CˆH†A  IC°M‰{I‰C€ICĞH‰D$0IC¸H‰D$(ICH‰D$ M‰{ÿ9,  ƒøÿ„ö  H‹Œ$Ø   H±;  D‹  D‹  H‰¬$    H‹	ÿŠ+  H‹èH…ÀuH‹¬$    AÇF   HÄ¨   A_A^Ãº8   H‰´$˜   JÉÿ0,  H‹U¹   L‹ÀH‹ğÿc(  fƒøşu*H‹ÎÿT+  LÕ;  º   3Éÿh+  AÇF   é9  fƒøÿu'LÖ;  3ÒH‹ÍèÄ·ÿÿH‹Îÿ+  AÇF   é  L‹L$pH‹”$À   H‰œ$È   H‹\$hH‰¼$   H‹|$`H‹ÇH÷ØH‹ÃfEÛfAƒãıH÷ØfD‰\$@H‰|$8I‹ÁfEÒfAƒâıH÷ØfD‰T$0H‹ÂH‰\$(fÉfƒáıH÷Øf‰L$ H‹fEÀfAƒàıÿ©(  H‹¼$   fƒøÿuLä?  3ÒH‹Íè
·ÿÿë%H‹HVfD‰~ÿ(  fD9~~H‹Îè÷¸ÿÿ…ÀuH‹Îÿ2*  AÇF   ë&L‰~‹f  H‹ÎH‰n0D‰~$ÿU)  I‰AÇF	  H‹œ$È   H‹´$˜   H‹¬$    HÄ¨   A_A^ÃÌÌÌÌÌÌL‹ÜAUHì  ‹I,MK3ÀMƒhÿÿÿI‰CL‹êI‰C H:?  IC˜H‰D$pIƒpÿÿÿH‰D$hIC H‰D$`IƒxÿÿÿH‰D$XIC¨H‰D$PIC€H‰D$HIC°H‰D$@ICˆH‰D$8IC¸H‰D$0ICH‰D$(IC H‰D$ ÿ‹)  ƒøÿ„‹  H‹Œ$€   H9  D‹hŒ  D‹eŒ  H‰œ$(  H‹	ÿÜ(  H‰„$0  H‹ØH…ÀuH‹œ$(  AÇE   HÄ  A]Ãº8   L‰¤$ğ   JÉÿ|)  H‹S¹   L‹ÀL‹àÿ¯%  fƒøşu*I‹Ìÿ (  L!9  º   3Éÿ´(  AÇE   éÈ  fƒøÿu'L"9  3ÒH‹ËèµÿÿI‹Ìÿ_(  AÇE   é›  H‹œ$    L‹Œ$¨   H‹”$   H‰¬$  H‰´$   H‹´$˜   H‰¼$ø   L‰´$è   L‹´$   L‰¼$à   L‹¼$ˆ   I‹ÇH÷ØI‹ÆfífƒåıH÷Øf‰l$`L‰|$XH‹ÆfÿfƒçıH÷Øf‰|$PL‰t$HH‹ÃfEÛfAƒãıH÷ØfD‰\$@H‰t$8I‹ÁfEÒfAƒâıH÷ØfD‰T$0H‹ÂH‰\$(fÉfƒáıH÷Øf‰L$ I‹$fEÀfAƒàıÿå$  L‹¼$à   L‹´$è   H‹´$   H‹¬$  fƒøÿuH‹Œ$0  LØ<  3ÒèÙ³ÿÿë+I‹$IT$3ÿfA‰|$ÿà$  fA9|$~I‹ÌèÀµÿÿ…ÀuI‹Ìÿû&  AÇE   ë2I‰|$H‹„$0  I‹Ì‹#Š  I‰D$0A‰|$$ÿ&  I‰E AÇE	  H‹¼$ø   L‹¤$ğ   H‹œ$(  HÄ  A]ÃÌÌÌÌÌ@VAVHƒì(‹I,LL$@H‹òLD$PE3öHÎ8  L‰t$@ÿ»&  ƒøÿ„S  H‹L$PH66  D‹›‰  D‹˜‰  H‰\$HH‹	·\$@H‰l$Xÿ&  H‹èH…ÀuH‹\$HH‹l$XÇF   HƒÄ(A^^Ãº8   H‰|$ JÉÿ´&  H‹U¹   L‹ÀH‹øÿç"  fƒøşu)H‹ÏÿØ%  LY6  º   3Éÿì%  ÇF   éš   fƒøÿuL[6  3ÒH‹ÍèI²ÿÿëJH‹·ÓÿÛ#  fƒøÿuL6;  3ÒH‹Íè$²ÿÿë%H‹HWfD‰wÿ0#  fD9w~H‹Ïè´ÿÿ…ÀuH‹ÏÿL%  ÇF   ë%L‰w‹ˆ  H‹ÏH‰o0D‰w$ÿp$  H‰ÇF	  H‹|$ H‹\$HH‹l$XHƒÄ(A^^ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌL‹ÜVAVHìˆ   ‹I,ICÈI‰C¨MK E3öICI‰C MCÀICĞM‰s I‰C˜H‹òICM‰sI‰CHk:  IC¸M‰sI‰CˆM‰s¸ÿå$  ƒøÿ„¥  H‹L$XH`4  D‹Å‡  D‹Â‡  H‰l$xH‹	ÿ<$  H‹èH…ÀuH‹l$xÇF   HÄˆ   A^^Ãº8   H‰|$pJÉÿê$  H‹U¹   L‹ÀH‹øÿ!  fƒøşu)H‹Ïÿ$  L4  º   3Éÿ"$  ÇF   éı   fƒøÿuL‘4  3ÒH‹Íè°ÿÿéª   L‹œ$    L‹Œ$°   I‹ÃH‹”$¸   H÷ØI‹ÁfEÒfAƒâıH÷ØfD‰T$0H‹ÂL‰\$(fÉfƒáıH÷Øf‰L$ H‹fEÀfAƒàıÿN!  fƒøÿuL19  3ÒH‹Íè°ÿÿë5H‹HWH‰œ$€   fD‰wÿ!  H‹œ$€   fD9w~H‹Ïèä±ÿÿ…ÀuH‹Ïÿ#  ÇF   ë%L‰w‹T†  H‹ÏH‰o0D‰w$ÿC"  H‰ÇF	  H‹|$pH‹l$xHÄˆ   A^^ÃÌÌÌÌH‹ÄAVAWHì¨   ‹I,E3ÿL‰x¨L‹òL‰x L‰xL‰xL‰xÈL‰xÀL‰x¸L‰x°Aÿ©ûÿÿÿtÿñ"  HÄ¨   A_A^ÃHD$hH‰D$PLL$`H„$À   H‰D$HL„$ˆ   HD$pH‰D$@H˜7  H„$Ğ   H‰D$8HD$xH‰D$0H„$Ø   H‰D$(H„$€   H‰D$ ÿi"  ƒøÿ„ü  H‹Œ$ˆ   Há1  D‹F…  D‹C…  H‰¬$    H‹	ÿº!  H‹èH…ÀuH‹¬$    AÇF   HÄ¨   A_A^Ãº8   H‰´$˜   JÉÿ`"  H‹U¹   L‹ÀH‹ğÿ“  fƒøşu*H‹Îÿ„!  L2  º   3Éÿ˜!  AÇF   é?  fƒøÿu'L2  3ÒH‹Íèô­ÿÿH‹ÎÿC!  AÇF   é  L‹Œ$Ø   H‹T$`H‰œ$È   H‹œ$Ğ   H‰¼$   H‹¼$À   H‹ÇH÷ØH‹ÃfEÛfAƒãıH÷ØfD‰\$@H‰|$8I‹ÁfEÒfAƒâıH÷ØfD‰T$0H‹ÂH‰\$(fÉfƒáıH÷Øf‰L$ H‹fEÀfAƒàıÿë  H‹¼$   fƒøÿuLn6  3ÒH‹Íè4­ÿÿë%H‹HVfD‰~ÿ@  fD9~~H‹Îè!¯ÿÿ…ÀuH‹Îÿ\   AÇF   ë&L‰~‹ƒ  H‹ÎH‰n0D‰~$ÿ  I‰AÇF	  H‹œ$È   H‹´$˜   H‹¬$    HÄ¨   A_A^ÃH‹ÄVAVHìˆ   ‹I,E3öL‰p H‹òL‰pL‰pL‰pÈL‰pÀL‰p¸ƒùtƒùtÿ*   HÄˆ   A^^ÃHD$PH‰D$@LŒ$¸   H„$    H‰D$8LD$hHD$XH‰D$0Hv5  H„$°   H‰D$(HD$`H‰D$ ÿ½  ƒøÿ„¥  H‹L$hH8/  D‹‚  D‹š‚  H‰l$xH‹	ÿ  H‹èH…ÀuH‹l$xÇF   HÄˆ   A^^Ãº8   H‰|$pJÉÿÂ  H‹U¹   L‹ÀH‹øÿõ  fƒøşu)H‹Ïÿæ  Lg/  º   3Éÿú  ÇF   éı   fƒøÿuLi/  3ÒH‹ÍèW«ÿÿéª   L‹œ$    L‹Œ$°   I‹ÃH‹”$¸   H÷ØI‹ÁfEÒfAƒâıH÷ØfD‰T$0H‹ÂL‰\$(fÉfƒáıH÷Øf‰L$ H‹fEÀfAƒàıÿ  fƒøÿuL94  3ÒH‹Íèßªÿÿë5H‹HWH‰œ$€   fD‰wÿã  H‹œ$€   fD9w~H‹Ïè¼¬ÿÿ…ÀuH‹Ïÿ÷  ÇF   ë%L‰w‹,  H‹ÏH‰o0D‰w$ÿ  H‰ÇF	  H‹|$pH‹l$xHÄˆ   A^^ÃÌÌÌÌÌÌÌÌÌÌÌÌL‹ÜVAUHìÈ   ‹I,IC I‰C€MKE3íIC˜H‰D$PMC¨IC°M‰k H‰D$HH‹òICM‰kH‰D$@H`3  IC¸M‰kH‰D$8ICH‰D$0ICˆH‰D$(IC H‰D$ M‰kˆÿv  ƒøÿ„  H‹Œ$€   Hî,  D‹S€  D‹P€  H‰¬$À   H‹	L‰¤$°   D·d$xL‰´$¨   D·t$hL‰¼$    D·|$pÿ  H‹èH…ÀuÇF   é„  º8   H‰¼$¸   JÉÿS  H‹U¹   L‹ÀH‹øÿ†  fƒøşu)H‹Ïÿw  Lø,  º   3Éÿ‹  ÇF   é"  fƒøÿu&Lú,  3ÒH‹Íèè¨ÿÿH‹Ïÿ7  ÇF   éö   L‹œ$ğ   A·ÖL‹„$ø   fD‰d$HfD‰|$@H‰œ$è   H‹œ$à   H‹ÃH÷ØI‹ÃfEÒfAƒâıH÷ØfD‰T$8I‹ÀH‰\$0fÉfƒáıf‰L$(H÷ØH‹fEÉL‰\$ fAƒáıÿò  fƒøÿuLµ1  3ÒH‹ÍèC¨ÿÿë%H‹HWfD‰oÿO  fD9o~H‹Ïè0ªÿÿ…ÀuH‹Ïÿk  ÇF   ë%L‰o‹ ~  H‹ÏH‰o0D‰o$ÿ  H‰ÇF	  H‹œ$è   H‹¼$¸   L‹¤$°   H‹¬$À   L‹´$¨   L‹¼$    HÄÈ   A]^ÃÌÌÌÌÌÌÌÌÌÌL‹ÜVAWHì¨   ‹I,IC¨I‰C˜MKIC H‹òI‰CMC°ICÈE3ÿI‰CˆHã0  IC¸M‰{I‰C€ICĞH‰D$0ICÀH‰D$(ICH‰D$ M‰{ÿÚ  ƒøÿ„É  H‹L$hHU*  D‹º}  D‹·}  H‰œ$È   H‹	·œ$Ø   H‰¬$    L‰´$   D·t$`ÿ  H‹èH…ÀuÇF   éT  º8   H‰¼$˜   JÉÿÆ  H‹U¹   L‹ÀH‹øÿù  fƒøşu)H‹Ïÿê  Lk*  º   3Éÿş  ÇF   éò   fƒøÿuLm*  3ÒH‹Íè[¦ÿÿéŸ   L‹\$pL‹L$xI‹ÃH‹”$À   H÷ØfD‰t$@I‹ÁfEÒf‰\$8fAƒâıH÷ØfD‰T$0H‹ÂL‰\$(fÉfƒáıH÷Øf‰L$ H‹fEÀfAƒàıÿe  fƒøÿuLp/  3ÒH‹ÍèŞ¥ÿÿë%H‹HWfD‰ÿê  fD9~H‹ÏèË§ÿÿ…ÀuH‹Ïÿ  ÇF   ë%L‰‹;|  H‹ÏH‰o0D‰$ÿ*  H‰ÇF	  H‹¼$˜   H‹¬$    H‹œ$È   L‹´$   HÄ¨   A_^ÃÌÌÌÌÌL‹ÜVAVHìˆ   ‹I,ICÈI‰C¨MK E3öICI‰C MCÀICĞM‰s I‰C˜H‹òICM‰sI‰CH.  IC¸M‰sI‰CˆM‰s¸ÿ•  ƒøÿ„¥  H‹L$XH(  D‹u{  D‹r{  H‰l$xH‹	ÿì  H‹èH…ÀuH‹l$xÇF   HÄˆ   A^^Ãº8   H‰|$pJÉÿš  H‹U¹   L‹ÀH‹øÿÍ  fƒøşu)H‹Ïÿ¾  L?(  º   3ÉÿÒ  ÇF   éı   fƒøÿuLA(  3ÒH‹Íè/¤ÿÿéª   L‹œ$    L‹Œ$°   I‹ÃH‹”$¸   H÷ØI‹ÁfEÒfAƒâıH÷ØfD‰T$0H‹ÂL‰\$(fÉfƒáıH÷Øf‰L$ H‹fEÀfAƒàıÿ  fƒøÿuLY-  3ÒH‹Íè·£ÿÿë5H‹HWH‰œ$€   fD‰wÿ»  H‹œ$€   fD9w~H‹Ïè”¥ÿÿ…ÀuH‹ÏÿÏ  ÇF   ë%L‰w‹z  H‹ÏH‰o0D‰w$ÿó  H‰ÇF	  H‹|$pH‹l$xHÄˆ   A^^ÃÌÌÌÌÿ%"  ÌÌÌÌÌÌÌÌÌÌÿ%ò  ÌÌÌÌÌÌÌÌÌÌÿ%Ú  ÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌff„     H;±r  òuHÁÁf÷ÁÿÿòuòÃHÁÉé?   ÌÌÌ@SHƒì H‹Ù3ÉÿÛ  H‹ËÿÚ  ÿÄ  H‹Èº	 ÀHƒÄ [Hÿ%¨  ÌÌÌÌÌÌÌÌH‰L$Hƒì8¹   èÿ  …Àt¹   Í)H»s  èª   H‹D$8H‰¢t  HD$8HƒÀH‰2t  H‹‹t  H‰ür  H‹D$@H‰ t  ÇÖr  	 ÀÇĞr     ÇÚr     ¸   HkÀ HÒr  HÇ   ¸   HkÀ H‹²q  H‰L ¸   HkÀH‹•q  H‰L H¹  èøşÿÿHƒÄ8ÃÌÌÌ@SVWHƒì@H‹Ùÿ£  H‹³ø   3ÿE3ÀHT$`H‹ÎÿY  H…Àt9Hƒd$8 HL$hH‹T$`L‹ÈH‰L$0L‹ÆHL$pH‰L$(3ÉH‰\$ ÿŠ  ÿÇƒÿ|±HƒÄ@_^[ÃÌÌÌÌÌÌÌÌÌÌÌHƒì(…Òt9ƒêt(ƒêtƒút
¸   HƒÄ(Ãè  ëèã  ¶ÀHƒÄ(ÃI‹ĞHƒÄ(é   M…À•ÁHƒÄ(é  H‰\$H‰t$H‰|$ AVHƒì H‹òL‹ñ3Éè‚  „Àu3ÀH‹\$0H‹t$8H‹|$HHƒÄ A^Ãèõ  ŠØˆD$@@·ƒ=ív   …´   Çİv     è@  „ÀtOèŸ  èz  è¡  HÖ  HÇ  èº
  …Àu)èİ  „Àt H¦  H—  è”
  Çˆv     @2ÿŠËè¢  @„ÿ…[ÿÿÿèä  H‹ØHƒ8 t$H‹Èèç  „ÀtL‹Æº   I‹ÎH‹L‹>  AÿÑÿv  ¸   éÿÿÿ¹   è­  ÌÌÌÌH‰\$H‰t$WHƒì @Šñ‹Üu  3Û…À3ÀH‹\$0H‹t$@HƒÄ _ÃÿÈ‰¼u  èß  @ŠøˆD$8ƒ=Ùu  u5èò  è  èÔ  ‰Âu  è  @ŠÏèÙ  3Ò@ŠÎèó  „À•Ã‹Ãë¹   è  ÌÌÌH‹ÄH‰X L‰@‰PH‰HVWAVHƒì@I‹ğ‹úL‹ñ…Òu98u  3Àéğ   BÿƒøwEH‹   H…Àu
ÇD$0   ëÿ3  ‹Ø‰D$0…À„´   L‹Æ‹×I‹Îèıÿÿ‹Ø‰D$0…À„™   L‹Æ‹×I‹Îè‘  ‹Ø‰D$0ƒÿu8…Àu4L‹Æ3ÒI‹Îèu  L‹Æ3ÒI‹ÎèLıÿÿH‹…  H…ÀtL‹Æ3ÒI‹Îÿº  …ÿtƒÿu@L‹Æ‹×I‹Îèıÿÿ‹Ø‰D$0…Àt)H‹K  H…Àu	X‰\$0ëL‹Æ‹×I‹Îÿw  ‹Ø‰D$0ë3Û‰\$0‹ÃH‹\$xHƒÄ@A^_^ÃÌH‰\$H‰t$WHƒì I‹ø‹ÚH‹ñƒúuè   L‹Ç‹ÓH‹ÎH‹\$0H‹t$8HƒÄ _éşÿÿÌÌÌH‰\$ UH‹ìHƒì H‹„m  H»2¢ß-™+  H;ÃutHƒe HMÿ†  H‹EH‰Eÿ€  ‹ÀH1EÿD  ‹ÀHM H1Eÿt  ‹E HMHÁà H3E H3EH3ÁH¹ÿÿÿÿÿÿ  H#ÁH¹3¢ß-™+  H;ÃHDÁH‰m  H‹\$HH÷ĞH‰êl  HƒÄ ]ÃHƒì(ƒúuHƒ=   uÿë  ¸   HƒÄ(ÃÌH)s  Hÿ%Ê  ÌÌHs  é*  Hs  ÃHs  ÃHƒì(èçÿÿÿHƒèæÿÿÿHƒHƒÄ(ÃÌHƒì(è×  …Àt!eH‹%0   H‹HëH;Èt3ÀğH±är  uî2ÀHƒÄ(Ã°ë÷ÌÌÌHƒì(è›  …Àtè  ëèƒ  ‹ÈèÀ  …Àt2Àëè¹  °HƒÄ(ÃHƒì(3Éè=  „À•ÀHƒÄ(ÃÌÌÌHƒì(è¯  „Àu2Àëè¢  „Àuè™  ëì°HƒÄ(ÃHƒì(è‡  è‚  °HƒÄ(ÃÌÌÌH‰\$H‰l$H‰t$WHƒì I‹ùI‹ğ‹ÚH‹éèô  …ÀuƒûuL‹Æ3ÒH‹ÍH‹Çÿú  H‹T$X‹L$PH‹\$0H‹l$8H‹t$@HƒÄ _éî  Hƒì(è¯  …ÀtHäq  HƒÄ(éé  èŞ™ÿÿ…Àuèá  HƒÄ(ÃHƒì(3Éèİ  HƒÄ(éÔ  @SHƒì ¶Ÿq  …É»   DÃˆq  èÎ  è­  „Àu2Àëè   „Àu	3Éè•  ëêŠÃHƒÄ [ÃÌÌÌ@SHƒì@€=Tq   ‹Ù…°   ƒù‡¯   è  …Àt(…Ûu$H6q  è9  …ÀuH>q  è)  …Àts2ÀëxH‹rj  ¹@   ‹Âƒà?+ÈHƒÈÿHÓÈH3ÂH‰D$ H‰D$(D$ H‰D$0òL$0Ûp  H‰D$ H‰D$(D$ H‰D$0òÏp  òL$0Êp  òÒp  Æœp  °HƒÄ@[Ã¹   èı   ÌHƒìL‹Á¸MZ  f9ÕyÿÿuyHczÿÿHÅyÿÿH9PE  u_¸  f9AuTL+Â·AHQHĞ·AH€LÊH‰$I;Ñt‹JL;Ár
‹BÁL;ÀrHƒÂ(ëß3ÒH…Òu2Àëƒz$ }2Àë
°ë2Àë2ÀHƒÄÃÌÌÌ@SHƒì ŠÙè§  3Ò…Àt„ÛuH‡Æo  HƒÄ [Ã@SHƒì €=»o   ŠÙt„ÒuŠËèÔ  ŠËèÍ  °HƒÄ [ÃÌHåo  Ãƒ%Åo   ÃH‰\$UH¬$@ûÿÿHìÀ  ‹Ù¹   è  …Àt‹ËÍ)¹   èÅÿÿÿ3ÒHMğA¸Ğ  è4  HMğÿÖ  H‹è   H•Ø  H‹ËE3ÀÿŒ  H…Àt<Hƒd$8 Hà  H‹•Ø  L‹ÈH‰L$0L‹ÃHè  H‰L$(HMğH‰L$ 3Éÿ³  H‹…È  HL$PH‰…è   3ÒH…È  A¸˜   HƒÀH‰…ˆ   è  H‹…È  H‰D$`ÇD$P  @ÇD$T   ÿÿ  ƒøHD$PH‰D$@HEğ”ÃH‰D$H3Éÿ.  HL$@ÿ+  …Àu„ÛuHè¿şÿÿH‹œ$Ğ  HÄÀ  ]ÃÌÌH‰\$WHƒì H>  H=ˆ>  ëH‹H…Àtÿ   HƒÃH;ßréH‹\$0HƒÄ _ÃH‰\$WHƒì Hc>  H=\>  ëH‹H…ÀtÿÄ  HƒÃH;ßréH‹\$0HƒÄ _ÃÌÌÌÌÌÌÌÌÂ  ÌH‰\$H‰t$WHƒì3ÀÇg     3ÉÇg     ¢D‹Á3ÿD‹ËAğntelAñGenuD‹Ò‹ğ3ÉGEÈ¢AòineI‰$EÊ‰\$D‹Ù‰L$‰T$uPHƒÈf  ÿ%ğ?ÿ=À t(=` t!=p t°ùüÿƒø w$H¹     H£ÁsD‹Im  AƒÈD‰>m  ëD‹5m  ¸   ;ğ|&3É¢‰$‹û‰\$‰L$‰T$ºã	sAƒÈD‰m  AºãspÇ1f     Ç+f     AºãsUAºãsN3ÉĞHÁâ HĞH‰T$ H‹D$ $<u2‹ûe  ƒÈÇêe     ‰èe  @öÇ tƒÈ ÇÑe     ‰Ïe  H‹\$(3ÀH‹t$0HƒÄ_ÃÌ¸   ÃÌÌ3À9Àe  •ÀÃÌÌÌÌÌÌÌÌÌÌÌÌÿ%z  ÿ%|  ÿ%~  ÿ%¸  ÿ%º  ÿ%Ä  ÿ%Ş  ÿ%Ğ  ÿ%Â  ÿ%´  ÿ%  ÿ%€  °ÃÌHƒì(M‹A8H‹ÊI‹Ñè   ¸   HƒÄ(ÃÌÌÌ@SE‹H‹ÚAƒãøL‹ÉAö L‹ÑtA‹@McP÷ØLÑHcÈL#ÑIcÃJ‹H‹C‹HH‹CöDt¶DƒàğLÈL3ÊI‹É[é	òÿÿÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌff„     HƒìL‰$L‰\$M3ÛLT$L+ĞMBÓeL‹%   M;ÓòsfAâ ğM› ğÿÿAÆ M;ÓòuïL‹$L‹\$HƒÄòÃÿ%i  ÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌff„     ÿà@UHƒì H‹êŠM@HƒÄ ]éÌúÿÿÌ@UHƒì H‹êèéøÿÿŠM8HƒÄ ]é°úÿÿÌ@U