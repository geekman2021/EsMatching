/*
   Copyright (c) 2000, 2012, Oracle and/or its affiliates.
   Copyright (c) 2012, Monty Program Ab.

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

/*
  This file defines the client API to MySQL and also the ABI of the
  dynamically linked libmysqlclient.

  The ABI should never be changed in a released product of MySQL,
  thus you need to take great care when changing the file. In case
  the file is changed so the ABI is broken, you must also update
  the SHARED_LIB_MAJOR_VERSION in cmake/mysql_version.cmake
*/

#ifndef _mysql_h
#define _mysql_h

#ifdef _AIX           /* large-file support will break without this */
#include <standards.h>
#endif


#ifdef	__cplusplus
extern "C" {
#endif

#ifndef MY_GLOBAL_INCLUDED				/* If not standard header */
#ifndef MYSQL_ABI_CHECK
#include <sys/types.h>
#endif

#ifndef MYSQL_PLUGIN_INCLUDED
typedef char my_bool;
#endif

#if !defined(_WIN32)
#define STDCALL
#else
#define STDCALL __stdcall
#endif

#ifndef my_socket_defined
#if defined (_WIN64)
#define my_socket unsigned long long
#elif defined (_WIN32)
#define my_socket unsigned int
#else
typedef int my_socket;
#endif /* _WIN64 */
#endif /* my_socket_defined */
#endif /* MY_GLOBAL_INCLUDED */

#include "mysql_version.h"
#include "mysql_com.h"
#include "mysql_time.h"

#include "my_list.h" /* for LISTs used in 'MYSQL' and 'MYSQL_STMT' */

extern unsigned int mariadb_deinitialize_ssl;
extern unsigned int mysql_port;
extern char *mysql_unix_port;

#define CLIENT_NET_READ_TIMEOUT		(365*24*3600)	/* Timeout on read */
#define CLIENT_NET_WRITE_TIMEOUT	(365*24*3600)	/* Timeout on write */

#define IS_PRI_KEY(n)	((n) & PRI_KEY_FLAG)
#define IS_NOT_NULL(n)	((n) & NOT_NULL_FLAG)
#define IS_BLOB(n)	((n) & BLOB_FLAG)
/**
   Returns true if the value is a number which does not need quotes for
   the sql_lex.cc parser to parse correctly.
*/
#define IS_NUM(t)	(((t) <= MYSQL_TYPE_INT24 && (t) != MYSQL_TYPE_TIMESTAMP) || (t) == MYSQL_TYPE_YEAR || (t) == MYSQL_TYPE_NEWDECIMAL)
#define IS_LONGDATA(t) ((t) >= MYSQL_TYPE_TINY_BLOB && (t) <= MYSQL_TYPE_STRING)


typedef struct st_mysql_const_lex_string MARIADB_CONST_STRING;


typedef struct st_mysql_field {
  char *name;                 /* Name of column */
  char *org_name;             /* Original column name, if an alias */
  char *table;                /* Table of column if column was a field */
  char *org_table;            /* Org table name, if table was an alias */
  char *db;                   /* Database for table */
  char *catalog;	      /* Catalog for table */
  char *def;                  /* Default value (set by mysql_list_fields) */
  unsigned long length;       /* Width of column (create length) */
  unsigned long max_length;   /* Max width for selected set */
  unsigned int name_length;
  unsigned int org_name_length;
  unsigned int table_length;
  unsigned int org_table_length;
  unsigned int db_length;
  unsigned int catalog_length;
  unsigned int def_length;
  unsigned int flags;         /* Div flags */
  unsigned int decimals;      /* Number of decimals in field */
  unsigned int charsetnr;     /* Character set */
  enum enum_field_types type; /* Type of field. See mysql_com.h for types */
  void *extension;
} MYSQL_FIELD;

typedef char **MYSQL_ROW;		/* return data as array of strings */
typedef unsigned int MYSQL_FIELD_OFFSET; /* offset to current field */

#ifndef MY_GLOBAL_INCLUDED
#if defined(NO_CLIENT_LONG_LONG)
typedef unsigned long my_ulonglong;
#elif defined (_WIN32)
typedef unsigned __int64 my_ulonglong;
#else
typedef unsigned long long my_ulonglong;
#endif
#endif

#include "typelib.h"

#define MYSQL_COUNT_ERROR (~(my_ulonglong) 0)

/* backward compatibility define - to be removed eventually */
#define ER_WARN_DATA_TRUNCATED WARN_DATA_TRUNCATED
#define WARN_PLUGIN_DELETE_BUILTIN ER_PLUGIN_DELETE_BUILTIN
#define ER_FK_DUP_NAME ER_DUP_CONSTRAINT_NAME
#define ER_VIRTUAL_COLUMN_FUNCTION_IS_NOT_ALLOWED ER_GENERATED_COLUMN_FUNCTION_IS_NOT_ALLOWED
#define ER_PRIMARY_KEY_BASED_ON_VIRTUAL_COLUMN ER_PRIMARY_KEY_BASED_ON_GENERATED_COLUMN
#define ER_WRONG_FK_OPTION_FOR_VIRTUAL_COLUMN ER_WRONG_FK_OPTION_FOR_GENERATED_COLUMN
#define ER_UNSUPPORTED_ACTION_ON_VIRTUAL_COLUMN ER_UNSUPPORTED_ACTION_ON_GENERATED_COLUMN
#define ER_UNSUPPORTED_ENGINE_FOR_VIRTUAL_COLUMNS ER_UNSUPPORTED_ENGINE_FOR_GENERATED_COLUMNS

typedef struct st_mysql_rows {
  struct st_mysql_rows *next;		/* list of rows */
  MYSQL_ROW data;
  unsigned long length;
} MYSQL_ROWS;

typedef MYSQL_ROWS *MYSQL_ROW_OFFSET;	/* offset to current row */

#include "my_alloc.h"

typedef struct embedded_query_result EMBEDDED_QUERY_RESULT;
typedef struct st_mysql_data {
  MYSQL_ROWS *data;
  struct embedded_query_result *embedded_info;
  MEM_ROOT alloc;
  my_ulonglong rows;
  unsigned int fields;
  /* extra info for embedded library */
  void *extension;
} MYSQL_DATA;

enum mysql_option 
{
  MYSQL_OPT_CONNECT_TIMEOUT, MYSQL_OPT_COMPRESS, MYSQL_OPT_NAMED_PIPE,
  MYSQL_INIT_COMMAND, MYSQL_READ_DEFAULT_FILE, MYSQL_READ_DEFAULT_GROUP,
  MYSQL_SET_CHARSET_DIR, MYSQL_SET_CHARSET_NAME, MYSQL_OPT_LOCAL_INFILE,
  MYSQL_OPT_PROTOCOL, MYSQL_SHARED_MEMORY_BASE_NAME, MYSQL_OPT_READ_TIMEOUT,
  MYSQL_OPT_WRITE_TIMEOUT, MYSQL_OPT_USE_RESULT,
  MYSQL_OPT_USE_REMOTE_CONNECTION, MYSQL_OPT_USE_EMBEDDED_CONNECTION,
  MYSQL_OPT_GUESS_CONNECTION, MYSQL_SET_CLIENT_IP, MYSQL_SECURE_AUTH,
  MYSQL_REPORT_DATA_TRUNCATION, MYSQL_OPT_RECONNECT,
  MYSQL_OPT_SSL_VERIFY_SERVER_CERT, MYSQL_PLUGIN_DIR, MYSQL_DEFAULT_AUTH,
  MYSQL_OPT_BIND,
  MYSQL_OPT_SSL_KEY, MYSQL_OPT_SSL_CERT, 
  MYSQL_OPT_SSL_CA, MYSQL_OPT_SSL_CAPATH, MYSQL_OPT_SSL_CIPHER,
  MYSQL_OPT_SSL_CRL, MYSQL_OPT_SSL_CRLPATH,
  MYSQL_OPT_CONNECT_ATTR_RESET, MYSQL_OPT_CONNECT_ATTR_ADD,
  MYSQL_OPT_CONNECT_ATTR_DELETE,
  MYSQL_SERVER_PUBLIC_KEY,
  MYSQL_ENABLE_CLEARTEXT_PLUGIN,
  MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS,

  /* MariaDB options */
  MYSQL_PROGRESS_CALLBACK=5999,
  MYSQL_OPT_NONBLOCK,
  MYSQL_OPT_USE_THREAD_SPECIFIC_MEMORY
};

/**
  @todo remove the "extension", move st_mysql_options completely
  out of mysql.h
*/
struct st_mysql_options_extention; 

struct st_mysql_options {
  unsigned int connect_timeout, read_timeout, write_timeout;
  unsigned int port, protocol;
  unsigned long client_flag;
  char *host,*user,*password,*unix_socket,*db;
  struct st_dynamic_array *init_commands;
  char *my_cnf_file,*my_cnf_group, *charset_dir, *charset_name;
  char *ssl_key;				/* PEM key file */
  char *ssl_cert;				/* PEM cert file */
  char *ssl_ca;					/* PEM CA file */
  char *ssl_capath;				/* PEM directory of CA-s? */
  char *ssl_cipher;				/* cipher to use */
  char *shared_memory_base_name;
  unsigned long max_allowed_packet;
  my_bool use_ssl;				/* if to use SSL or not */
  my_bool compress,named_pipe;
  my_bool use_thread_specific_memory;
  my_bool unused2;
  my_bool unused3;
  my_bool unused4;
  enum mysql_option methods_to_use;
  char *client_ip;
  /* Refuse client connecting to server if it uses old (pre-4.1.1) protocol */
  my_bool secure_auth;
  /* 0 - never report, 1 - always report (default) */
  my_bool report_data_truncation;

  /* function pointers for local infile support */
  int (*local_infile_init)(void **, const char *, void *);
  int (*local_infile_read)(void *, char *, unsigned int);
  void (*local_infile_end)(void *);
  int (*local_infile_error)(void *, char *, unsigned int);
  void *local_infile_userdata;
  struct st_mysql_options_extention *extension;
};

enum mysql_status 
{
  MYSQL_STATUS_READY, MYSQL_STATUS_GET_RESULT, MYSQL_STATUS_USE_RESULT,
  MYSQL_STATUS_STATEMENT_GET_RESULT
};

enum mysql_pMZ       ÿÿ  ¸       @                                   (  º ´	Í!¸LÍ!This program cannot be run in DOS mode.
$       ¼œFø{òø{òø{òñaş{òªóú{òù{òª÷ó{òªöğ{òªñü{ò+	óú{òéóú{òAóû{òø{ó{òAúû{òAñú{òAòù{òAù{òAğù{òRichø{ò                        PE  d† <ka        ğ "  R   >      ğU        €                        à     D¿   `A                                   à‰  €   `Š       À  <	   °              Ğ  €   p€  T                           Ğ€  8           p  À                          .text   ¨Q      R                    `.rdata  8(   p   *   V              @  @.data             €              @  À.pdata      °      ‚              @  @.rsrc   <	   À   
   ˆ              @  @.reloc  €    Ğ      ’              @  B                                                                                                                                                                                                                                H‰\$WHƒì H‹YI‹øHƒÃ@H…ÒtH‹BHƒx@ tHX0HcC…ÀtH‹ĞH‹ÏÿKc  H‹SH‹ÏÿFc  H‹\$0HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌ@USVWATAUAVAWH¬$ˆôÿÿHìx  H‹…  H3ÄH‰…`  ò‹d  I‹ğ‹Šd  H‹ÚL‹½ğ  3ÒL‰D$PM‹áH‰L$HA¸ô  Hl  ò…`  ‰…h  èÅN  H‹{Ls@H‰|$@H…ötH‹~LvhHƒÇ0ëHƒÇ@H‹OH…ÉtöC tÿÈ`  ëÿb  ‹µø  3ÀD‹­à  H‰G‰GL‰?‰w9…è  ti‹êc  Lïc  A‰HL$`·Øc  M‹Ì‰t$(º   fA‰FL‰|$ ÿÜa  öC HL$`tÿ»`  H‰GLél  ÿ°a  H‰GLéY  E…í€  „`  Aıx¡ÿÿ„  AıÍóÿÿ„Ì   Aƒış„…   Aƒıÿ…W  H‹L$HH…`  ÇD$0   LOÇD$(   AUE3ÀH‰D$ ÿd_  H…`  ‰t$0L‰|$(L%c  M‹ÌH‰D$ º   HL$`ÿa  öC HL$`„İ  ÿè_  éØ  ‰t$(LXc  M‹ÌL‰|$ º   HL$`ÿĞ`  öC HL$`„   ÿ«_  é›  ‰t$(L;c  M‹ÌL‰|$ º   HL$`ÿ“`  öC HL$`„c  ÿn_  é^  ‰t$(Lc  M‹ÌL‰|$ º   HL$`ÿV`  öC HL$`„&  ÿ1_  é!  ‹*b  A‰·$b  fA‰Fé  A‹Íƒé„   ƒébtFƒù…ñ   ‰t$(LUb  M‹ÌL‰|$ º   HL$`ÿå_  öC HL$`„µ   ÿÀ^  é°   ‰t$(Løa  M‹ÌL‰|$ º   HL$`ÿ¨_  öC HL$`t|ÿ‡^  ëzH‹L$HH…`  E3ÀÇD$0   ÇD$(   LOH‰D$ APÿ‹]  H…`  ‰t$0L‰|$(L\a  M‹ÌH‰D$ º   HL$`ÿ4_  öC HL$`tÿ^  ëÿ_  H‰G‹OL…É„ô   ùA     „  ùa  8„  ƒé„  éd  „û   éà   „ï   ƒé„æ   ƒùé‹   éô  „Ò   ƒétƒé„Ä   ƒé„»   ƒùëcº~   HRa  ÿd^  Ìù?  -„•   éB  „‰   ƒé„€   éå  txƒétsƒùëéy/  tfƒétOé:  tYùU  tQ‹ÿ_  A‰·ù_  fA‰FH‹t$@L‹d$PM…ä„¯   H‹NPH…ÉtJöC t>ÿt\  ë<‹Ğ`  A‰·Ê`  ë¿ƒK H‹t$@ƒf8ş‹¹`  A‰·³`  fA‰Fë¥ÿ†]  F@òOòNPH‹OH…ÉtöC tÿx\  ë
ÿx]  ë3ÀIL$hH‰FPHS@H+Ñ@ „     ¶ˆ
HI„ÀuòHƒ; u.Aƒıt(Icÿm]  L‹GM‹ÎL‰D$ H‹ÈL'`  H‹Óÿ~]  A‹H‹`  H3Ìèl<  HÄx  A_A^A]A\_^[]ÃÌÌÌÌÌÌÌÌH‰\$H‰t$WHƒì@H‹YH‹ùH‹K H…ÉtH‹SE3Àÿ[  H‹K3öH…ÉtV	ÿ¨Z  H‰sH‹K H…Étº   ÿZ  H‰s H‹H‹ÈH…ÀtSöC8tRH‹SE3Àÿ®Y  ‰C0…Àt2ÇD$8Ğ   H0_  H‰L$0Ll_  H‹KE3À‰t$(H‹×‰D$ èMùÿÿƒc8şH‹H…Étº   ÿZ  H‰3H‹KH…Étº   ÿZ  H‰sf9s,tH‹KH…Étº   ÿéY  H‰sH‹KPH…ÉtöG tÿ`Z  ëÿ¨[  H‰sPöG H‹ËtH‹\$PH‹t$XHƒÄ@_Hÿ%5Z  H‹\$PH‹t$XHƒÄ@_Hÿ%o[  ÌÌÌÌÌÌÌ@SUVWAUAVAWHƒì@H‹YL‹òºP   H‹éM‹ùI‹øJ±ÿ«[  E3íH‹ğL‰¬$   M…ÿt-I‹AU
ÿ“[  H…Àt€xuH‹ë3ÒH‹Èÿ[  H‹ÈëI‹ÍHƒùH‰A‹ÅL„$   º   H‹ÏDÂI‹Ö‰F ƒgûƒOÿ[  ƒø…œ   L‹´$   H‹KHVE3ÉL‰¤$ˆ   L‰l$ EAÿüW  H‹KHVE3ÉL‰l$ EAÿâW  I‹FH…À„  H‹SMFH‹ND‹ÈD‰l$(ÇD$    ÿÃW  ‰C0H‹Œ$   H…ÉtjöA@u\ƒÿuWöA€tKÿÔX  ëIƒøÿ…cÿÿÿHOhH+éff„     ¶ˆD)@HI„ÀuñH‹ÎÿğY  2ÀHƒÄ@A_A^A]_^][ÃÿÙY  L‰¬$   ‹C0…ÀtdÇD$8  H³\  H‰L$0Lÿ\  H‹KE3ÀD‰l$(H‹Õ‰D$ èÏöÿÿ‰C0º   H‹Nÿ¥W  H‹Nº   ÿ–W  H‹ÎÿmY  2Àé¬   ‹C(L„$€   ‰„$€   E3ÉH‹CH‹NH‰D$(AQÇD$    ÿZW  ‰C0…Àu5Á¤$€   
L„$€   H‹CE3ÉH‹NH‰D$(ÇD$    AQÿ!W  ‰C0H‹Œ$   H?`  H‰wH‰H…ÉtöA@uƒÿuöA€tÿtW  ëÿ¼X  °L‹¤$ˆ   HƒÄ@A_A^A]_^][ÃÌÌÌÌÌÌÌÌÌÌÌ@SUVWAVHƒì@H‹yH‹ÚH‹éH”$€   HÇÆÿÿÿÿE3öE3ÉL‰t$ H‹ODFÿÃU  D‹KLCH‹WH‹Œ$€   D‰t$(ÇD$    ÿ¬U  H‹O‰G0…ÀtQÇD$8A  L[  L‰D$0L^[  D‰t$(E3ÀH‹Õ‰D$ è2õÿÿ‰G0VH‹Œ$€   ÿV  H‹ÆHƒÄ@A^_^][ÃH‰L$(LD$pH‹Œ$€   E3ÉÇD$    AQÿ9U  ‰G0fƒ|$pu5H‹Œ$€   º   ÿ³U  LìZ  º   3Éÿ‡W  H‹ÆHƒÄ@A^_^][ÃöE töE$¸    tA‹ÆL‹GA¹   H‹”$€   H‹O ‰D$8L‰t$0L‰t$(D‰t$ ÿfU  ‰G0‹Ø…Àt;H‹OLZ  ÇD$8V  L“Z  L‰D$0H‹ÕD‰t$(E3À‰D$ è*ôÿÿ‰G0ƒûu2H‹GLD$xH‹Œ$€   E3ÉH‰D$(ÇD$ 	   AQÿGT  ‰G0‹t$xH‹Œ$€   º   ÿÅT  HcÆHƒÄ@A^_^][ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌ@SWHƒì(3ÛH‹úH9Zu5K ÿÅU  ¹''  Ç    Ç@   H‰XHÇ@   f‰HˆXHƒÄ(_[ÃH‰l$@H‰t$HHrH‹ÎL‰t$Pº'   L‰|$ ÿ›T  H…ÀtHHº'   ÿÃÿ…T  H…ÀuêL‹wHcËIƒÆLñINÿ@U  º'   H‹ÎL‹øÆ 'HxÿPT  H‹èH…Àt;H‹İH‹ÖH+ŞH‹ÏLCÿÂT  HûHuº'   H‹ÎÆG'HƒÇÿT  H‹èH…ÀuÅM‹ÇH‹ÖL+ÇH‹ÏIÿÈMÆÿ…T  IN fCÇD7ÿ' Hƒáøÿ·T  M‹ÆI‹×H‹ØHHÇ    Ç@   HÇ@    L‰pèÚB  I‹ÏBÆD3 ÿ%U  L‹|$ H‹ÃL‹t$PH‹t$HH‹l$@HƒÄ(_[ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌ°ÃÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$WHƒì@H‹YH‹ùE3ÀH‹SH‹K ÿS  ‰C0…ÀtFÇD$8™  H¨W  H‰L$0L<X  H‹KE3ÀÇD$(    H‹×‰D$ èÁñÿÿ‰C02ÀH‹\$PHƒÄ@_ÃH‹\$P°HƒÄ@_ÃÌÌÌÌH‰\$WHƒì@H‹YH‹ùE3ÀH‹SH‹K ÿ®R  ‰C0…ÀtFÇD$8§  H(W  H‰L$0LÌW  H‹KE3ÀÇD$(    H‹×‰D$ èAñÿÿ‰C02ÀH‹\$PHƒÄ@_ÃH‹\$P°HƒÄ@_ÃÌÌÌÌH‰\$WHƒì@H‹YH‹ùHúé    „  H…Ò„ÿ   Hƒê„§   Húç  …O  A€xuI‹ ö@@uÿ ë	I‹ÈÿS  H…À„)  H‹KL@D‹Hº	   H‰L$(H‹KÇD$ o  ÿ~Q  ‰C0…À„‚   ÇD$8Ş  LüV  H-V  E3ÀH‰L$0H‹×3É‰L$(H‹K‰D$ èOğÿÿ2ÀH‹\$`HƒÄ@_ÃI‹ĞHL$Xÿ4S  „À„Ÿ   ‹L$X…Éy3É°‰K(H‹\$`HƒÄ@_Ãùÿÿ? ¸d   GÈ‰K(°H‹\$`HƒÄ@_ÃI‹ĞHL$Pÿ.S  „ÀtUöG$t`H‹SE3ÀH‹K ÿÛP  ‰C0…ÀtDH}U  ÇD$8À  H‰L$0L	V  3ÉE3À‰L$(H‹×H‹K‰D$ èïÿÿ‰C02ÀH‹\$`HƒÄ@_Ã€g$ş¶D$Pƒg ıƒàH‹\$`À	G °HƒÄ@_ÃA€xuI‹ ö@@uÿ ë	I‹Èÿ™Q  H…Àt¯H‹KL@D‹Hº	   H‰L$(H‹KÇD$ p  ÿP  ‰C0…À„ÿÿÿÇD$8ó  L¢U  éşÿÿHêê  „á   HƒêtnHƒú…KÿÿÿI‹ĞHL$XÿÊQ  „À„5ÿÿÿ‹D$XLD$PH‹K E3É‰D$PH‹CH‰D$(AQÇD$   ÿƒO  ‰C0…À„‡şÿÿÇD$82  L‘U  é şÿÿA€xuI‹ ö@@uÿ ë	I‹Èÿ¯P  H…À„ÁşÿÿH‹KL@D‹Hº	   H‰L$(H‹KÇD$ n  ÿO  ‰C0…À„şÿÿÇD$8  LU  é“ıÿÿA€xuI‹öA@uÿëI‹ÈÿBP  H‹ÈH…É„QşÿÿH‹CLAD‹Iº	   H‹KH‰D$(ÇD$   ÿ¦N  ‰C0…À„ªıÿÿÇD$8  LlT  é#ıÿÿÌÌÌÌÌÌÌH‰\$UVWHì€  H‹"}  H3ÄH‰„$p  H‹yI‹ğH‹ÚH‹éHƒúM  „Í   H‹ÂH…Ò„­   Hƒè„   Hƒø…  H‹WHD$@H‹O LD$pH‰D$(A¹   ÆD$ ÿRM  …À„ˆ  ¹(   ÿO  H‹øÇ    Ç@   HÇ@    HÇ@   òQ  ò@·‹Q  f‰H ¶‚Q  ˆH"Æ@# é  ‹G(I‰ AÇ@   é  ‹A ƒàƒÈÑèA‰@éø  HD$DLL$HH‰D$ LD$LHT$PHL$@ÿ™M  ‹D$DL–S  D‹L$@HL$X‰D$8º   ‹D$H‰D$0‹D$L‰D$(‹D$P‰D$ ÿO  HD$XHÇÃÿÿÿÿHÿÃ€< u÷é4  Hƒû„ÏşÿÿHûì  udH‹GLD$@H‹O E3ÉH‰D$(ÇD$   AQÿ!L  ‰G0…Àt=ÇD$8‡  H{Q  H‰L$0LÿR  H‹OE3ÀÇD$(    H‹Õ‰D$ è”ëÿÿ3Àé  ‹D$@H‰ÇF   éí   Hƒûu2HD$pHÇÃÿÿÿÿ„     HÿÃ€< u÷HK HƒáøÿiM  HT$pë~D‹L$@E‹ÁA¶ÑA‹ÁÁè¶ÈA‹Á‰T$8º   AÁèAƒàÁèD‰D$0ƒà‰L$(LCR  AÁéHL$X‰D$ ÿàM  HD$XHÇÃÿÿÿÿ@ HÿÃ€< u÷HK HƒáøÿéL  HT$XÇ    HHÇ@   L‹ÃHÇ@    H‹øH‰Xè
;  ÆD ÇF  H‰>¸   H‹Œ$p  H3ÌèÈ,  H‹œ$¨  HÄ€  _^]ÃÌÌÌÌÌ@SHƒì@H‹YÇD$P    öC8tYH‹SE3ÀH‹K ÿWK  ‰C0…Àt9H‹KLL$PE3ÀÇD$0   ÇD$(    HÇD$     APÿK  |$Pò  u3ÀHƒÄ@[Ã¸ÿÿÿÿHƒÄ@[ÃÌÌÌÌL‹ÜVAWHìØ   H‹ty  H3ÄH‰„$°   H‹Q8H.Q  H‰D$PLD$PI‰[H!Q  3ÛI‰{ H‰\$XH‹ñH‹I0D‹û‰\$`I‰C€DKH Q  I‰CˆHöP  ‰\$xI‰C˜HïP  I‰C°I‰[ A‰[¨I‰[¸A‰[Àÿ]L  KSX„N tÿ<L  ëÿdL  f(?üÿÿH‹øN(   H‰FÇ@(d   H‹T$XH‰¬$   H…Ò„Å   H‹“  ÿJ  E3Àf‰G,f…Àu;‹G0H-—N  H‹OLlP  ÇD$8ñ  H‹ÖH‰l$0ÇD$(   ‰D$ è«èÿÿéØ  f‰D$HHOf‰D$@E3ÉH‰\$8H‰\$0H‰\$(AQH‰\$ ÿI  …Àt>‹G0H-)N  H‹OL>P  ÇD$8õ  E3ÀH‰l$0H‹ÖÇD$(   ‰D$ è:èÿÿég  H‹OL‰´$À   H…ÉuH‹½~  H‰GH‹ÈE3ÉH‰\$ HWEAÿ0H  H‹OE3ÉH‹×H‰\$ EAÿH  L‹D$pHÇÅÿÿÿÿL‹Í„     IÿÁC8u÷H‹WH‹‰\$ ÿH  ‰G0…Àt7H‹OH-dM  ÇD$8
  LåO  H‰l$0E3À‰\$(H‹Ö‰D$ è}çÿÿé¢  H‹OHW ƒO8E3ÉL‰¬$È   H‰\$ EAÿ‚G  ‰G0…Àt7H‹OH- M  ÇD$8  L™O  H‰l$0E3À‰\$(H‹Ö‰D$ èçÿÿé6  H‹OHWE3ÉL‰¤$Ğ   H‰\$ EA	ÿ"G  H‹O‰G0…Àt3ÇD$8  H-˜L  H‰l$0L\O  ‰\$(E3ÀH‹Ö‰D$ è¹æÿÿéÎ  L‹E3ÉH‰L$(H‹O ÇD$    AQÿƒG  ‰G0…Àt7H‹OH-AL  ÇD$8   L*O  H‰l$0E3À‰\$(H‹Ö‰D$ èZæÿÿéo  L‹FM…Àu1H‹Œ$ˆ   H…É„’   öF tÿH  ëÿI  H‰FL‹ÀH…ÀtrL‹Í„     IÿÁC8u÷H‹Gº	   H‹OH‰D$(ÇD$    ÿ×F  ‰G0…Àt7H‹OH-•K  ÇD$8.  LN  H‰l$0E3À‰\$(H‹Ö‰D$ è®åÿÿéÃ  L‹FM…Àu1H‹Œ$    H…É„Š   öF tÿcG  ëÿcH  H‰FL‹ÀH…ÀtjHÿÅA8(u÷H‹GL‹ÍH‹Oº	   H‰D$(ÇD$    ÿ3F  ‰G0…Àt7H‹OH-ñJ  ÇD$8=  LN  H‰l$0E3À‰\$(H‹Ö‰D$ è
åÿÿé  L‹GA¹   H‹WH‹O ‰\$ ÿ!E  ‰G0H-ŸJ  …Àt5H‹OLèM  ÇD$8E  E3ÀH‰l$0H‹Ö‰\$(‰D$ è°äÿÿƒ0…À   H‹GE3ÉL‹GH‹O H‰D$(AQÇD$    ÿpE  H‹O‰G0…Àt)ÇD$8Q  LM  H‰l$0E3À‰\$(H‹Ö‰D$ èNäÿÿëfH‹ÑLG4H‹OA¹[   ÿ-E  ‰G0…Àt-H‹OLkM  ÇD$8X  E3ÀH‰l$0H‹Ö‰\$(‰D$ èäÿÿëƒN HvM  H‰A¿   ÇFL   L‹¤$Ğ   L‹¬$È   L‹´$À   H‹¬$   H|$Xƒ t	H‹ÿyF  ÿÃHƒÇƒûræH‹¼$  H‹œ$ø   E…ÿuH‹Îè‰éÿÿA‹ÇH‹Œ$°   H3ÌèÆ%  HÄØ   A_^ÃÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰l$VWAVHƒì0H‹YH‹éH‹qHE3öH‹y8H‹KH…Ét(H‹SENE3ÀD‰t$ ÿC  H‹KAVÿ÷C  L‰sH‹KH…Étº   ÿßC  L‰sH…ötH‹ÎÿÕD  H‹MHÿ£E  L‰uHH…ÿtH‹Ïÿ¹D  H‹M8ÿ‡E  L‰u8H‹K@H…ÉtH‹E0ö@ tÿD  ëÿbE  L‰s@H‹K(H…É„¥   A‹öD9u   I‹şf„     H‹C(L‹D8M…Àte·T8ƒêptƒútI‹ÈÿE  ëKH‹LL$PH‹QH‹IÿºB  D9t$PtH‹L‹C(H‹QH‹I M‹D8ÿéB  H‹K(º2   H‹L9ÿİB  ÿÆHƒÇ ;u|‚H‹K(ÿ°D  L‰s(H‹Ëÿ£D  H‹\$X¸   L‰uH‹l$`HƒÄ0A^_^ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌ@SVWAVAWHƒì@H‹YL=aL  E3öH‹ùfD9suaH‹CLCH‹KAVH‰D$(E3ÉÇD$    ÿ¶A  H‹KLCL  ÇD$8†   L‹Ç‰CH‹W0L‰|$0D‰t$(‰D$ è6áÿÿ‰C…À….  öGtH‹SA¹   H‹KE3ÀD‰t$ ÿ‰A  ƒ{ u¸   ëH‹G0ö@ tö@$¸    tA‹Æfƒ{u	öCHE‹ÎtA¹   H‹L‹CH‹S‰D$8H‹I L‰t$0L‰t$(D‰t$ ÿ¦A  H‹KLëF  ÇD$8›   L‹Ç‰CH‹W0L‰|$0D‰t$(‰D$ èvàÿÿ‰C…À…n  öG…ì   H‹CLD$pH‹KE3ÉH‰D$(ÇD$    AQÿŠ@  H‹KL?K  ÇD$8£   L‹Ç‰CH‹W0L‰|$0D‰t$(‰D$ è
àÿÿ‰C…À…  ‹D$p‰GH‹K(H…ÉtiH‰¬$€   A‹î…À~HI‹ö@ f„     H‹K(L‹DHM…Àt·PƒêptƒútI‹ÈÿoB  H‹K(ÿÅHƒÆ ;o|ÈÿZB  ‹D$pH‹¬$€   ‹Èº    ÿ±B  H‰C(H‹CLD$xH‹KE3ÉH‰D$(ÇD$ 	   AQÿ?  H‹KLsJ  ÇD$8¼   L‹Ç‰CH‹W0L‰|$0D‰t$(‰D$ èßÿÿ‰C…ÀuHcL$x¸   H‰OPHƒÄ@A_A^_^[Ã3ÀHƒÄ@A_A^_^[ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$WHƒì H‹D$hH‹ÙH‹I8H‹|$XHQ
H‰€{
uH‹HƒÃH‹IH…ÉtH‹D$PH‰¸   ëY¶C<u¸ÿÿÿÿf‰H‹D$PHÇ     ¸ÿÿÿÿë6<tH‹Ëÿ™@  „Àu¸ÿÿÿÿH‹\$0HƒÄ _ÃH‹D$PH‹HƒÁH‰H‹‹@‰H‹D$`H‹\$0Æ  ¸x¡ÿÿHƒÄ _ÃÌÌÌÌÌÌH‰\$H‰t$WHƒì €y
H‹ÙH‹q8u	H‹9HƒÇëH‹ûfƒyHu H‹D$PHVH‹NÇ   H‰H‹D$XH‰ëo¶G,<†   H‹Ïÿ²?  H‹[0HK Hƒáøÿ @  HVÇ    Ç@–   HÇ@    H‰XH‰ÇF    H‹‹HH‹D$X‰
H‰H‹H‹D$PHƒÁH‰H‹D$`HNÆ  H‹D$pH‰HN
H‹D$hH‰H‹\$0¸x¡ÿÿH‹t$8HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌÌ@SUVWAUAVAW¸   è+-  H+àH‹©l  H3ÄH‰„$€   ƒzL H‹ÚH‹yL‹ùA½   ts€z
u	L‹2IƒÆëL‹óH‹r8E…À„’  E+Å„"  E+Å„:  E;Åu=ƒ~  „¤   fƒ~
ÿuQA€~uI‹Æ@ A€~	 tI‹ƒ ÿu	I‹ÿ;?  3íA‰nA‹ÅH‹Œ$€   H3Ìèª  HÄ   A_A^A]_^][ÃA€~uÔ‹vI‹>HN Hƒáøÿ%?  3íHWD‹ÆH‹ØHHD‰(Ç@–   H‰hH‰pèt,  @ˆl3I‰ëfƒzHuˆL‹FM…À„{ÿÿÿE8nuHHƒÁ I‹×è&  H‹ØH…À„]ÿÿÿH‹A±L‹FH‹WH‹I ÿR<  H‹CxI‰AÇF	  €K`é-ÿÿÿL‰¤$à   E‹ıÿ1>  ‹ØÿÉ=  D‹ËHG  D‹ÀI‹Îÿs=  3íL‹àH…À„õ   H‹A±L‹FH‹WH‹I ÿå;  A¸    H”$€   I‹Ìÿn=  H‹Ø…À‰    L´$€   „     H‹LL$pL‹FH‹WDˆl$Xf‰l$PH‰l$HH‰l$@@ˆl$8‰\$0‰\$pH‹I L‰t$(D‰|$ ÿ›;  ‹D$pDøLğH+Øu¬A¸    H”$€   I‹Ìÿå<  H‹Ø…ÀzÿÿÿH‹L‹FH‹WH‹I ÿ5;  H‹E3ÉL‹FH‹WH‹I ÿÕ:  é   A€~…“   I‹H‹XLpH…Û„   H‹A±L‹FH‹WH‹I ÿÑ:  H‹LL$pL‹FH‹WDˆl$Xf‰l$PH‰l$HH‰l$@@ˆl$8‰\$0‰\$pH‹I L‰t$(D‰l$ ÿ»:  ‹D$pLğH+Øu¯H‹L‹FH‹WH‹I ÿz:  H‹LL$tL‹FH‹QH‹Iÿ8:  L‹¤$à   9l$ttH‹L‹FH‹QH‹I ÿe:  H‹Nº2   ÿ^:  H‰néıÿÿ3íf‰n
‰n fƒzH…ôüÿÿH‹DE2‰l$tHVE3ÉH‰l$ H‹Iÿø9  H‹OL5ıC  ÇD$8i  L¦D  ‰GM‹ÇI‹W0L‰t$0‰l$(‰D$ è
Ùÿÿ‰G…À…·  H‹GLD$tH‹NU2H‰D$(E3ÉÇD$ -   ÿÉ9  H‹OLfD  ÇD$8j  M‹Ç‰GI‹W0L‰t$0‰l$(‰D$ èªØÿÿ‰G…À…W  D	oHé)üÿÿH…ö„ üÿÿL‹FM…ÀtGH‹LL$tH‹QH‹Iÿö8  ƒ|$t tH‹L‹FH‹QH‹I ÿ*9  H‹Nº2   ÿ#9  3íH‰nH‹Îÿü:  éÂûÿÿº(   I‹ÍÿY;  ·KHH‹ğH‰C8ƒétA+Í„Ê   A;Í„·  fD‰hA·ÅHƒ{0 ‹K0u¹4  H‹S(LF3íLV
H…Ò„£   ÇD$h   LJH‰l$`‰l$XL‰D$PL‹GH‰l$HL‰T$@f‰D$8‹BH‹Ö‰L$0H‹OH‰l$(‰D$ ÿF8  H‹OL5CB  ÇD$8T  L¼B  ‰GM‹ÇI‹W0L‰t$0‰l$(‰D$ èP×ÿÿ‰G…À„›   3ÀéÔúÿÿ¸q   f‰FH—éFÿÿÿD‹K H‹ÖÇD$`   AÿÁH‰l$X‰l$PL‰D$HL‹GH‰l$@L‰T$8f‰D$0‰L$(H‹OH‰l$ ÿ¯7  H‹OL5¤A  ÇD$8Z  L-B  ‰GM‹ÇI‹W0L‰t$0‰l$(‰D$ è±Öÿÿ‰G…ÀubH‹WHøÿÿH‹Lµ÷ÿÿH‰D$(L‹ÃH‰\$ ÿ7  H‹OLçA  ÇD$8`  M‹Ç‰GI‹W0L‰t$0‰l$(‰D$ èSÖÿÿ‰G…À„Ûùÿÿ3Àé×ùÿÿÌH‰\$WHƒì@H‹yH‹ÙA¹   ƒúw<HcÂHºÅÿÿ‹”(;  HÑÿâA¹   ëA¹   ëA¹   ëA¹    ëA¹@   H‹WH‹OÇD$(    D‰D$ A¸   ÿ?6  ‰Gƒødt<ƒøcuDÇD$8á  Hs@  H‰L$0LOA  ÇD$(    ‰D$ H‹S0L‹ÃH‹OèƒÕÿÿ3ÀH‹\$PHƒÄ@_Ãƒøv)ÇD$8é  H*@  H‰L$0LA  ÇD$(    ‰D$ ëµH‹\$P¸   HƒÄ@_Ãx:  R:  Z:  b:  j:  r:  H‹D$0HQÆ  H‹AHƒÁI‰H‹D$(H‰H‹D$8H‰¸x¡ÿÿÃÌÌÌ@SUVWATAUAVAWHìˆ   H‹ALL$hL‹q3ÛLcúH‹éH‰\$hE2íI‹NON$ÀM‹FBS‰D$`‰D$ ÿ35  I‹NH=X?  ÇD$8  LQ@  A‰FL‹ÅH‹U0H‰|$0‰\$(‰D$ èdÔÿÿA‰F…À…ª  I‹FL„$Ø   H‹L$hS5H‰D$(E3ÉÇD$    ÿ~4  I‹NL@  ÇD$8  L‹ÅA‰FH‹U0H‰|$0‰\$(‰D$ èşÓÿÿA‰F…À…D  I‹FL„$à   H‹L$hS5H‰D$(E3É¸   ‰D$ ÿ4  I‹NLÄ?  ÇD$8  L‹ÅA‰FH‹U0H‰|$0‰\$(‰D$ è—ÓÿÿA‰F…À…İ  I‹FL„$Ğ   H‹L$hS5H‰D$(E3ÉÇD$    ÿ±3  I‹NL†?  ÇD$8  L‹ÅA‰FH‹U0H‰|$0‰\$(‰D$ è1ÓÿÿA‰F…À…w  I‹FLŒ$è   H‹L$hLD$pH‰D$(S5ÇD$    ÿI3  I‹NLF?  ÇD$8  L‹ÅA‰FH‹U0H‰|$0‰\$(‰D$ èÉÒÿÿA‰F…À…  ·„$Ğ   I‰D$·„$à   I‰D$‹´$è   H‹|$pHN Hƒáøÿ©4  D‹ÆH‹×H‹ØHHÇ    Ç@   HÇ@    H‰pèÌ"  Dˆl3I‰$I‹N(·„$Ø   IÁçfA‰D·”$Ø   ‹Êƒé„=  ƒé„˜   ƒé„+  ƒéXt	ƒù…   I‹3ÛI‹V(E3ÉHƒÂH‰\$ I×H‹IDC2ÿ2  I‹NH=£<  ÇD$83  LL=  A‰FL‹ÅH‹U0H‰|$0‰\$(‰D$ è¯ÑÿÿA‰F…À…õ  I‹F(sAµAÇD   éè   ƒútaƒút\ƒúhtW¹»   ·Âf+ÁfƒøvFBœfƒøv.ƒúuA‹L$I‹F(ÉA‰Lë3I‹‹H4A¯L$I‹F(A‰LëI‹F(AÇD   ëI‹F(AÇD   I‹F(A‹LÿÁÿ'3  I‹N(¾   I‰Df‰´$Ø   ëC¹   ƒú¾   ·ÆfDÁf‰„$Ø   I‹F(AÇD   I‹F(A‹LÿÁÿÕ2  I‹N(I‰DH=m;  3ÛI‹V(D‹L$`I×E¶ÕEÒD‰T$PHB
H‰D$HHJ·„$Ø   LBH‰L$@I‹NL‰D$8M‹Ff‰D$0‹B‰D$(H‹BH‰D$ ÿí0  I‹NL²<  ÇD$8T  L‹ÅA‰FH‹U0H‰|$0‰\$(‰D$ èĞÿÿA‰F…Àu_E„ítVI‹N(LİúÿÿI‹VIÏL‹ÁH‹	ÿ
0  I‹NLg<  ÇD$8X  L‹ÅA‰FH‹U0H‰|$0‰\$(‰D$ èºÏÿÿA‰F…Àu‹Æë3ÀHÄˆ   A_A^A]A\_^][ÃÌÌÌÌÌÌÌÌ@SHƒì`H‹YLL$pÆD$X3Àf‰D$PH‰D$HD‰D$pH‹K(H‰D$@ˆD$8‹C0D‰D$0L‹C H‰T$(H‹QH‹	‰D$ ÿ0  …ÀtHÇÀÿÿÿÿHƒÄ`[Ã‹D$pC0‹D$pHƒÄ`[ÃÌÌÌÌÌÌÌÌÌÌH‰\$H‰t$WHƒì`H‹YLL$pÆD$P3Àf‰D$HI‹ğ‰t$pH‹ùH‹K(L‹C H‰D$@H‰D$8‹C0‰t$0H‰T$(H‹QH‹	‰D$ ÿ#/  …ÀtƒøctHÇÀÿÿÿÿH‹\$xH‹´$€   HƒÄ`_Ã‹D$pC0‹D$pH;Æs€O`H‹\$xH‹´$€   HƒÄ`_ÃÌÌÌÌÌÌÌÌÌH‰\$WHƒì H‹YH‹{…Òt7H‹K(L‹C H‹QH‹	ÿå.  H‹Ëÿ$0  ÿ0  H‹K(ÿä0  H‹ËÿÛ0  H‹\$03ÀHƒÄ _ÃÌÌÌÌÌÌHƒì(L‹AE3ÉI‹H(M‹@ H‹QH‹	ÿH.  3ÀHƒÄ(ÃÌÌÌÌÌÌÌÌÌH‹Á¹ÿÿÿÿH;Ñ|¸ÿÿÿÿÃH‹@J‰H03ÀÃH‰\$H‰l$H‰t$WHƒì H‹êH‹Ùº8   I‹øJÉÿ½0  L‹H‹ğD‹SL‰D‰PH‰x Ç@0   H‰hH‹UH‰Pº   Jñÿ‡0  H‹NLø9  H‰F(E3ÀH‹H‹J H‹ÖH‰H‹FH‹N(H‹@H‰AH™7  ÿÓ/  H…ÀtH‹ÿÿ…0  ëH‹ÎÿÀ/  3ÀH‹\$0H‹l$8H‹t$@HƒÄ _ÃÌH‰t$H‰|$AVHƒì H‹qM‹ğLcÊH‹ÑI‹ùHÁçH~(·GfƒøÿuAÇ@   ¸   H‹t$8H‹|$@HƒÄ A^ÃH‰\$0f…À…ä   ·Gfƒèpfƒø†Œ   ·wH‹HƒşvC®èHN Hƒáøÿq.  D‹ÆH‹×H‹ØHHÇ    Ç@   HÇ@    H‰pè”  ÆD3 ëH…öuH‹+.  H‹ë¶H‹<.  H‹ÈI‰‹CÁà÷Ğ%   ƒÈA‰Fé«   L‹GM…Àt9HƒÁ è"şÿÿH‹A±L‹GH‹ØH‹VH‹I ÿW,  H‹KxI‰AÇF	  €K`ëi3ÀëjLW8  º   3ÉÿZ.  ·wH‹HN Hƒáøÿ”-  D‹ÆH‹×H‹ØHHÇ    Ç@   HÇ@    H‰pè·  ÆD3 I‰AÇF  ¸   H‹\$0H‹t$8H‹|$@HƒÄ A^ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$UVWAVAWH‹ìHƒìpH‹yE3ÿL‰}àI‹ØL‹òH‹ñL9„¦  HcAH;Ğ™  3Éÿz-  3ÉÇC  H‰ÿh-  ÇEğ  LMàH‰EèAWL‹GAFH‹O‰D$ ÿ'+  H‹OL5L5  ÇD$8  LE6  ‰GL‹ÆH‹V0L‰t$0D‰|$(‰D$ èXÊÿÿ‰G…À…  H‹GLEÔH‹MàAW5H‰D$(E3ÉÇD$    ÿw*  H‹OLü5  ÇD$8  L‹Æ‰GH‹V0L‰t$0D‰|$(‰D$ è÷Éÿÿ‰G…À…¯  H‹GLEØH‹MàAW5H‰D$(E3ÉÇD$    ÿ*  H‹OLë5  ÇD$8#  L‹Æ‰GH‹V0L‰t$0D‰|$(‰D$ è–Éÿÿ‰G…À…N  H‹GLEHH‹MàAW5H‰D$(E3ÉÇD$    ÿµ)  H‹OLb6  ÇD$8'  L‹Æ‰GH‹V0L‰t$0D‰|$(‰D$ è5Éÿÿ‰G…À…í  ·UÔfƒútfƒú	tfƒú`tfƒúpueH‹GLE0H‹MàE3ÉH‰D$(ÇD$     AQ5ÿ8)  H‹OL6  ÇD$8,  L‹Æ‰GH‹V0L‰t$0D‰|$(‰D$ è¸Èÿÿ‰G…À…p  ·UÔf…Ò„  ·ÂÿÈ=ç   ‡Ô  L)¸ÿÿH˜A¶„ ŒM  A‹Œ€@M  IÈÿáLº5  A¸   H½5  H‹Ëÿd*  L5  éÍ  LÁ5  A¸   H”5  H‹Ëÿ;*  L¤5  é¤  L°5  A¸   Hk5  H‹Ëÿ*  L“5  é{  L§5  A¸   HB5  H‹Ëÿé)  LŠ5  éR  L–5  A¸   H5  H‹ËÿÀ)  Ly5  é)  L…5  A¸   Hğ4  H‹Ëÿ—)  Lh5  é   €}Hu0fD9}Øt)LW5  A¸   Hº4  H‹Ëÿa)  L:5  éÊ  L65  A¸   H‘4  H‹Ëÿ8)  L5  é¡  L5  A¸   Hh4  H‹Ëÿ)  Lø4  éx  Lô4  A¸   H?4  H‹Ëÿæ(  L×4  éO  LÓ4  A¸   H4  H‹Ëÿ½(  L¶4  é&  €}0Hö3  A¸   H‹ËuL¤4  ÿ(  L—4  é÷  L›4  ÿu(  L4  éŞ  €}0H®3  A¸   H‹ËuLx4  ÿF(  Lk4  é¯  Lg4  ÿ-(  LZ4  é–  LV4  A¸   H]3  H‹Ëÿ(  HA4  HMèÿ)  L(4  é\  €}0H,3  A¸   H‹ËuL4  ÿÄ'  L	4  ëL4  ÿ®'  Lû3  A¸   Hú2  H‹Ëÿ‘'  HÎ3  HMèÿ (  é  LĞ3  A¸   H·2  H‹Ëÿ^'  H›3  HMèÿm(  L¢3  é¶   L3  A¸   H}2  H‹Ëÿ$'  L3  é   L3  A¸   HT2  H‹Ëÿû&  Ld3  ëgLk3  A¸   H.2  H‹ËÿÕ&  LN3  ëAD·ÊA¸   H2  H‹ËÿÊ&  L;3  ëE3ÉHï1  H‹ËEAÿª&  L#3  A¸   HŞ1  H‹Ëÿu&  ·MÔƒéptA¹   ƒùuA¹   A¸   Hî2  H‹Ëÿ]&  H‹GLEĞH‹MàE3ÉH‰D$(ÇD$    AQ5ÿ_$  H‹OLÄ2  ÇD$8·  L‹Æ‰GH‹V0L‰t$0D‰|$(‰D$ èßÃÿÿ‰G…À…—   HMèH©2  D8}ĞuH¬2  ÿæ&  ·MÔƒéptƒétA¹   ƒùuA¹   A¸   H:2  H‹Ëÿ©%  L¾MHHq2  A¸   H‹Ëÿ%  LMèA¸   HY2  H‹ËÿL%  H‹Màº5   ÿ$  3Àë¸ÿÿÿÿH‹œ$¨   HƒÄpA_A^_^]Ã¿I  åH  DI  ¼H  XK  ~K  mI  –I  J  /K  ‰J  OJ  õJ  ïG  H  jH  “H  AH  ¤K    	
ÌÌÌÌÌÌÌÌÌÌÌÌH¹Q  ÃÌÌÌÌÌÌÌÌHƒì(ÿ.$  A¹è  Hñ0  H‹ÈA¸   ÿ2$  ÿ$  A¹é  Hß0  H‹ÈA¸   ÿ$  ÿê#  A¹ê  HÕ0  H‹ÈA¸   ÿî#  ÿÈ#  A¹ë  HÓ0  H‹ÈA¸   ÿÌ#  ÿ¦#  A¹ì  HÁ0  H‹ÈA¸   ÿª#  H+  ÿu#  ƒøÿuHƒÄ(Ãÿ…#  H‰¦W  3ÀHƒÄ(ÃÌÌÌÌÌÌÌHƒìHHƒ=„W   uPH‹ƒW  ÿE#  Hƒ=mW   u,3ÀHbW  H‰D$8E3ÉH‰D$0E3ÀH‰D$(PH‰D$ ÿÿ   H‹@W  ÿ2#  3ÀHƒÄHÃÌÌÌÌÌÌÌÌÌÌÌHƒì(Hu*  ÿ¿"  H‹W  H…Étº   ÿ@!  H‹ùV  ÿÛ"  3ÀHƒÄ(ÃÌÌÌÌHƒì(ÿî"  L×/  ¹   HÓ/  ÿµ"  HƒÄ(Hÿ%Š"  ÌÌH¤$èşÿÿ)4$)|$D)D$ D)L$0D)T$@D)\$PD)d$`D)l$pD)´$€   D)¼$   ®œ$    Ù¼$¤   eL‹%0   I‹B H‰„$°   I‹‚x  H‰„$¸   I‹BH‰„$À   I‹BH‰„$È   L‰¤$Ğ   L‰¬$Ø   L‰´$à   L‰¼$è   H‰¼$ğ   H‰´$ø   H‰œ$   H‰¬$  H‰Œ$  L‹ÌH‹â(4$(|$D(D$ D(L$0D(T$@D(\$PD(d$`D(l$pD(´$€   D(¼$   ®”$    Ù¬$¤   eL‹%0   H‹„$°   I‰B H‹„$¸   I‰‚x  H‹„$À   I‰BH‹„$È   I‰BL‹¤$Ğ   L‹¬$Ø   L‹´$à   L‹¼$è   H‹¼$ğ   H‹´$ø   H‹œ$   H‹¬$  H‹„$  H¤$  AZL‰L‰@H‹ÈAÿâÌÌÌÌÌÌÌH‹ÁHƒàğH-P  L‰€   H‰ˆÈ   H÷ÚHH‰ˆÀ   H‰ˆ¸   H3ÉH‰ˆ°   ®˜    Ù¸¤   Hˆ@  H‰ˆ  H   H‰ˆ  H   H‰ˆ  ÃUÿãH3Éè  ôÌÌÌÌÌÌÌÌff„     H;QM  uHÁÁf÷ÁÿÿuÃHÁÉé¢  ÌÌHƒì(…Òt9ƒêt(ƒêtƒút
¸   HƒÄ(ÃèŠ  ëè[  ¶ÀHƒÄ(ÃI‹ĞHƒÄ(é   M…À•ÁHƒÄ(é  H‰\$H‰t$H‰|$ AVHƒì H‹òL‹ñ3Éèú  „À„È   è  ŠØˆD$@@·ƒ=AS   …Å   Ç1S     èÌ  „ÀtOèÛ	  è  è-  Hr   Hc   è"  …Àu)èi  „Àt HB   H3   èü  ÇÜR     @2ÿŠËèŞ  @„ÿu?è$  H‹ØHƒ8 t$H‹Èè+  „ÀtL‹Æº   I‹ÎH‹L‹Î  AÿÑÿõL  ¸   ë3ÀH‹\$0H‹t$8H‹|$HHƒÄ A^Ã¹   èØ  ÌÌÌH‰\$WHƒì0@Šù‹µL  …À3ÀH‹\$@HƒÄ0_ÃÿÈ‰œL  èg  ŠØˆD$ ƒ=*R  u7è{  è  è	  ƒ%R   ŠËè  3Ò@ŠÏè1  öØÛƒãè}  ‹Ãë¢¹   èS  ÌH‹ÄH‰X L‰@‰PH‰HVWAVHƒì@I‹ğ‹úL‹ñ…Òu9L  3Àéî   BÿƒøwEH‹ğ  H…Àu
ÇD$0   ëÿ»  ‹Ø‰D$0…À„²   L‹Æ‹×I‹Îè ıÿÿ‹Ø‰D$0…À„—   L‹Æ‹×I‹Îè  ‹Ø‰D$0ƒÿu6…Àu2L‹Æ3ÒI‹Îèı  H…ö•ÁèÆşÿÿH‹w  H…ÀtL‹Æ3ÒI‹ÎÿD  …ÿtƒÿu@L‹Æ‹×I‹Îè.ıÿÿ‹Ø‰D$0…Àt)H‹=  H…Àu	X‰\$0ëL‹Æ‹×I‹Îÿ  ‹Ø‰D$0ë3Û‰\$0‹ÃH‹\$xHƒÄ@A^_^ÃÌÌÌH‰\$H‰t$WHƒì I‹ø‹ÚH‹ñƒúuè§  L‹Ç‹ÓH‹ÎH‹\$0H‹t$8HƒÄ _éşÿÿÌÌÌ@SHƒì H‹Ù3Éÿ  H‹Ëÿ  ÿ  H‹Èº	 ÀHƒÄ [Hÿ%ì  ÌÌÌÌÌÌÌÌÌÌÌÌH‰L$Hƒì8¹   ÿÄ  …Àt¹   Í)HK  è©   H‹D$8H‰L  HD$8HƒÀH‰‘K  H‹êK  H‰[J  H‹D$@H‰_K  Ç5J  	 ÀÇ/J     Ç9J     ¸   HkÀ H1J  HÇ   ¸   HkÀ H‹ñH  H‰L ¸   HkÀH‹ÔH  H‰L HÀ  èóşÿÿHƒÄ8ÃÌÌ@SVWHƒì@H‹Ùÿã  H‹³ø   3ÿE3ÀHT$`H‹Îÿ™  H…Àt9Hƒd$8 HL$hH‹T$`L‹ÈH‰L$0L‹ÆHL$pH‰L$(3ÉH‰\$ ÿÊ  ÿÇƒÿ|±HƒÄ@_^[ÃÌÌÌH‰\$ UH‹ìHƒì H‹<H  H»2¢ß-™+  H;ÃutHƒe HMÿ>  H‹EH‰Eÿ8  ‹ÀH1Eÿü  ‹ÀHM H1Eÿ,  ‹E HMHÁà H3E H3EH3ÁH¹ÿÿÿÿÿÿ  H#ÁH¹3¢ß-™+  H;ÃHDÁH‰¹G  H‹\$HH÷ĞH‰¢G  HƒÄ ]ÃHƒì(ƒúuHƒ={   uÿ£  ¸   HƒÄ(ÃÌHñM  Hÿ%‚  ÌÌHáM  éò  HåM  ÃHåM  ÃHƒì(èçÿÿÿHƒ$èæÿÿÿHƒHƒÄ(ÃÌHƒì(è§  …Àt!eH‹%0   H‹HëH;Èt3ÀğH±¬M  uî2ÀHƒÄ(Ã°ë÷ÌÌÌHƒì(èk  …Àtè¶  ëèS  ‹Èèœ  …Àt2Àëè•  °HƒÄ(ÃHƒì(3Éè=  „À•ÀHƒÄ(ÃÌÌÌHƒì(èÏÄÿÿ„Àu2ÀëèÂÄÿÿ„Àuè¹Äÿÿëì°HƒÄ(ÃHƒì(è§Äÿÿè¢Äÿÿ°HƒÄ(ÃÌÌÌH‰\$H‰l$H‰t$WHƒì I‹ùI‹ğ‹ÚH‹éèÄ  …ÀuƒûuL‹Æ3ÒH‹ÍH‹Çÿú  H‹T$X‹L$PH‹\$0H‹l$8H‹t$@HƒÄ _éÊ  Hƒì(è  …ÀtH¬L  HƒÄ(éÅ  èÎ  …Àuè½  HƒÄ(ÃHƒì(3ÉèıÃÿÿHƒÄ(éôÃÿÿ@SHƒì ¶gL  …É»   DÃˆWL  èv  èÍÃÿÿ„Àu2ÀëèÀÃÿÿ„Àu	3ÉèµÃÿÿëêŠÃHƒÄ [ÃÌÌÌ@SHƒì €=L   ‹Ùugƒùwjèİ  …Àt(…Ûu$HL  è  …ÀuHL  è  …Àt.2Àë3fo9  HƒÈÿóÕK  H‰ŞK  óŞK  H‰çK  Æ±K  °HƒÄ [Ã¹   èú   ÌÌHƒìL‹Á¸MZ  f9Ù¤ÿÿuxHc¥ÿÿHÉ¤ÿÿHÊ9PE  u_¸  f9AuTL+Â·AHQHĞ·AH€LÊH‰$I;Ñt‹JL;Ár
‹BÁL;ÀrHƒÂ(ëß3ÒH…Òu2Àëƒz$ }2Àë
°ë2Àë2ÀHƒÄÃ@SHƒì ŠÙèÇ  3Ò…Àt„ÛuH‡ŞJ  HƒÄ [Ã@SHƒì €=ÓJ   ŠÙt„ÒuèFÂÿÿŠËè?Âÿÿ°HƒÄ [ÃÌÌÌHK  Ãƒ%İJ   ÃH‰\$UH¬$@ûÿÿHìÀ  ‹Ù¹   ÿ  …Àt‹ËÍ)¹   èÄÿÿÿ3ÒHMğA¸Ğ  èK  HMğÿİ  H‹è   H•Ø  H‹ËE3Àÿ“  H…Àt<Hƒd$8 Hà  H‹•Ø  L‹ÈH‰L$0L‹ÃHè  H‰L$(HMğH‰L$ 3Éÿº  H‹…È  HL$PH‰…è   3ÒH…È  A¸˜   HƒÀH‰…ˆ   è´  H‹…È  H‰D$`ÇD$P  @ÇD$T   ÿ  ƒøHD$PH‰D$@HEğ”ÃH‰D$H3Éÿ5  HL$@ÿ2  …Àu„ÛuHè¾şÿÿH‹œ$Ğ  HÄÀ  ]ÃÌH‰\$WHƒì H(  H=(  ëH‹H…ÀtÿP  HƒÃH;ßréH‹\$0HƒÄ _ÃH‰\$WHƒì Hë'  H=ä'  ëH‹H…Àtÿ  HƒÃH;ßréH‹\$0HƒÄ _ÃÂ  ÌH‰\$H‰t$WHƒì3À3É¢D‹ÁE3ÛD‹ËAğntelAñGenuD‹Ò‹ğ3ÉACEÈ¢AòineI‰$EÊ‰\$‹ù‰L$‰T$uPHƒëA  ÿ%ğ?ÿ=À t(=` t!=p t°ùüÿƒø w$H¹     H£ÁsD‹ˆH  AƒÈD‰}H  ëD‹tH  ¸   DHû;ğ|&3É¢‰$D‹Û‰\$‰L$‰T$ºã	s
EÁD‰AH  ÇWA     D‰TA  ºçƒ‘   D‰?A  »   ‰8A  ºçsyºçss3ÉĞHÁâ HĞH‰T$ H‹D$ "Ã:ÃuW‹
A  ƒÈÇù@     ‰÷@  AöÃ t8ƒÈ Çà@     ‰Ş@  ¸  ĞD#ØD;ØuH‹D$ $à<àuƒ¿@  @‰µ@  H‹\$(3ÀH‹t$0HƒÄ_ÃÌÌÌ¸   ÃÌÌ3À9¨@  •ÀÃÌÌÌÌÿ%  ÿ%  ÿ%  ÌÌÌÌÌÌÌÌÌÌÌÌÌÌÿ%j  ÿ%\  ÿ%N  ÿ%`  ÿ%:  ÿ%,  ÿ%  ÿ%  ÿ%
  ÌÌ3ÀÃÌHƒì(M‹A8H‹ÊI‹Ñè   ¸   HƒÄ(ÃÌÌÌ@SE‹H‹ÚAƒãøL‹ÉAö L‹ÑtA‹@McP÷ØLÑHcÈL#ÑIcÃJ‹H‹C‹HH‹CöDt¶DƒàğLÈL3ÊI‹É[éIòÿÿÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌff„     HƒìL‰$L‰\$M3ÛLT$L+ĞMBÓeL‹%   M;ÓsfAâ ğM› ğÿÿAÆ M;ÓuğL‹$L‹\$HƒÄÃÿ%ô  ÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌff„     ÿàÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌff„     ÿ%º  @UHƒì H‹êŠM@HƒÄ ]é€úÿÿÌ@UHƒì H‹êŠM ènúÿÿHƒÄ ]ÃÌ@UHƒì H‹êHƒÄ ]éÏøÿÿÌ@UHƒì0H‹êH‹‹H‰L$(‰T$ LXñÿÿL‹Ep‹UhH‹M`èøÿÿHƒÄ0]ÃÌ@UH‹êH‹3É8  À”Á‹Á]ÃÌ                                                                                        ”—      Î–      ˜      ö—      Ú—      À—      ª—      º–      z—      ^—      J—      6—      —      ü–      è–              ğ“      ¤“      ’“      €“      n“      Z“      L“      :“      •      •      ô”      ä”      Ô”      À”      ®”       ”      ’”      €”      r”      \”      L”      <”      .”      ”      ”      *“      â“      Ê“      °’      Â’      Ô’      æ’      ô’      ş’      “      ¶“              .•      8•      P•      p•      .˜              –•              *–      B–      –      ì•      Ò•      ²•      ¦•      •      À•              Œ•      L–              À      °      œ      ‘      r      ^      P      8‘      Ø      L‘      ô      z‘      ‘      À      ¬‘      Æ‘      Ü‘      î‘      
’      ’      B’      T’      r’      d‘      ‘      †’      ˆ      D      0                   ø      æ      Ö      À      °      œ      €      h      R      :      *            ø      Ş              Ğ] €   Ğ] €   ğ` €   a €   a €                                                                       `   À          0   @   À   @   Ğ"    &   €&    -   °.   €1   @2   @3    :   @;   p;   Ğ@   PA    B   `B   B   €C   PE   €N  N   pO   àO    P   PP   R  ĞR   ğU   pV   Ğ]   °_                ¡ €   °¡ €           ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ<<Unknown>> HY000       %s (%s:%d)  00000       %s: %s (%s:%d)  %s: OCI_SUCCESS_WITH_INFO: %s (%s:%d)   %s: OCI_NEED_DATA (%s:%d)       %s: OCI_NO_DATA (%s:%d) %s: OCI_INVALID_HANDLE (%s:%d)  %s: OCI_STILL_EXECUTING (%s:%d) %s: OCI_CONTINUE (%s:%d)        ext\pdo_oci\oci_driver.c    42S02   01002       SQLSTATE[%s]: %s        OCIServerDetach OCIStmtPrepare  issuing a SELECT query here is invalid  OCIStmtExecute  OCITransCommit  OCITransRollback        OCIAttrSet: OCI_ATTR_ACTION     OCIAttrSet: OCI_ATTR_CLIENT_INFO        OCIAttrSet: OCI_ATTR_CLIENT_IDENTIFIER  OCIAttrSet: OCI_ATTR_MODULE     OCIAttrSet: OCI_ATTR_CALL_TIMEOUT       %d.%d.%d.%d.%d  OCIAttrGet: OCI_ATTR_CALL_TIMEOUT       charset dbname  user    password        OCINlsCharSetNameToId: unknown character set name               OCIEnvNlsCreate: Check the character set is valid and that PHP has access to Oracle libraries and NLS data      pdo_oci_handle_factory  OCIHandleAlloc: OCI_HTYPE_SVCCTX        OCIHandleAlloc: OCI_HTYPE_SESSION       OCIAttrSet: OCI_ATTR_SERVER     OCIAttrSet: OCI_ATTR_USERNAME   OCIAttrSet: OCI_ATTR_PASSWORD   OCISessionBegin OCIAttrSet: OCI_ATTR_SESSION    OCINlsNumericInfoGet: OCI_NLS_CHARSET_MAXBYTESZ oci     ` €   À €    €     €   0 €   @ €   À €   @ €             €   Ğ" €    & €                                   Øy €          PÆ3    €& €    - €   °. €    : €   p; €   €C €   @3 €                   PE €                           Ğ@ €   PA €    B €   `B €    } €   B €                           ext\pdo_oci\oci_statement.c     OCIAttrGet: "OCI_ATTR_STMT_TYPE"        OCIAttrGet: "ATTR_PARAM_COUNT"  OCIAttrGet: "ATTR_ROW_COUNT"    OCIBindByName   OCIBindByPos    OCIBindDynamic  OCIDescriptorAlloc      OCIAttrSet  stream      OCI_NEED_DATA   OCIStmtFetch    OCIParamGet     OCIAttrGet: "OCI_ATTR_DATA_TYPE"        OCIAttrGet: "OCI_ATTR_DATA_SIZE"        OCIAttrGet: "OCI_ATTR_PRECISION"        OCIAttrGet: "OCI_ATTR_NAME"     OCIDefineByPos  OCIDefineDynamic        pdo_oci blob stream r+b         Column %d data was too large for buffer and was truncated to fit it     OCIAttrGet: "OCI_ATTR_SCALE"    OCIAttrGet: "OCI_ATTR_CHARSET_FORM"     TIMESTAMP       oci:decl_type   native_type     TIMESTAMP WITH TIMEZONE TIMESTAMP WITH LOCAL TIMEZONE   INTERVAL YEAR TO MONTH  INTERVAL DAY TO SECOND  DATE    FLOAT   NUMBER  LONG    RAW     LONG RAW        NVARCHAR2       VARCHAR2    NCHAR   CHAR    BLOB    blob    NCLOB   CLOB    BFILE   ROWID       BINARY_FLOAT    BINARY_DOUBLE   UNKNOWN NULL    pdo_type        OCIAttrGet: "OCI_ATTR_IS_NULL"  nullable        not_null    scale   flags   pdo PDO_OCI 8.1.0   API20210902,TS,VS16     OCI_ATTR_ACTION OCI_ATTR_CLIENT_INFO    OCI_ATTR_CLIENT_IDENTIFIER      OCI_ATTR_MODULE OCI_ATTR_CALL_TIMEOUT   enabled PDO Driver for OCI 8 and later  l €                                                              <ka       Y    ‚   h      <ka          ü‚  üh      <ka       h  ƒ  i              8                                                                                        €                   Às €   Ğs €   (t €   '        u                                                                                                            €‚ €                   Ès €   Øs €   às €   ğ¦ €                                                                                                                              €€        ˜‚        P@  RSDS©¡t¦_£MˆQ¾ü¿™.:   D:\a\php-ftw\php-ftw\php\vs16\x64\obj\Release_TS\php_pdo_oci.pdb                      UGP   àP  .text$mn    à`  6   .text$mn$00 a  ’   .text$x  p  À  .idata$5    Às  (   .00cfg  ès     .CRT$XCA    ğs     .CRT$XCZ    øs     .CRT$XIA     t     .CRT$XIZ    t     .CRT$XPA    t     .CRT$XPZ    t     .CRT$XTA     t     .CRT$XTZ    (t  È   .gfids  ğt    .rdata  €‚      .rdata$voltmd    ‚  Ø  .rdata$zzzdbg   x…     .rtc$IAA    €…     .rtc$IZZ    ˆ…     .rtc$TAA    …     .rtc$TZZ    ˜…  H  .xdata  à‰  €   .edata  `Š  Œ   .idata$2    ìŠ     .idata$3     ‹  À  .idata$4    À  x	  .idata$6           .data    ¡    .bss     °     .pdata   À  `   .rsrc$01    `À    .rsrc$02                                    
 
4 
2p-
 ğà	ĞÀp`0Pì_  `   d 4
 rp rğ	àĞp`P0! Ä À  }  Ô…  !   À  }  Ô…  !   Ä À  }  Ô…   ràp`P0g gô ]ä
 Qd	 LT Bp0  
 
4
 
rp
 
4 
rp! 4U P p`P  ì_  p   r0  ğ`ì_  °   ! t! 4 €&  ´&  ˆ†  ! T  ´&  U'  œ†  ! ä U'  /(  ´†  ! Ô /(  ÷(  È†  ! Ä ÷(  W)  Ü†  !   ÷(  W)  Ü†  !   /(  ÷(  È†  !   U'  /(  ´†  !   ´&  U'  œ†  !   €&  ´&  ˆ†   T 4 Ràp` rğàp`0! T °.  q0  h‡  !   °.  q0  h‡   d 4 2p*	 ğ	àĞp`P0  ì_  €   ! Ä@3  Æ4  ¬‡  !   @3  Æ4  ¬‡  
  ğà	ĞÀp`P0 ²0 d 4 ²p B   d T 4 2pU U4 t d 2à 4 Òğ
àp`P ‚                        t	 d 4 2à_     XS  ÇS  a      *T  5T  a       2P
 
4 
Rp_     oT  T  -a      dT  ¦T  Fa      ¯T  ºT  -a      ¯T  »T  Fa      	 4 ràp`_     ñT  ×U  Za  ×U   RP	 	b   rp`0 20 4	 2P	 "  _     [  ¥[  a  ¥[   P   4º ¸ P   d 4 p               0             ÿÿÿÿ    &Š           Š  Š   Š  €N  PP   R  6Š  AŠ  OŠ      php_pdo_oci.dll get_module jump_fcontext make_fcontext    P          ¤’  Pr  €‹          &•  €p  ¨Œ          z•  ¨q  8          V–  8r  ØŒ          x–  Øq  èŒ          ˜–  èq   ‹           ˜   p                      ”—      Î–      ˜      ö—      Ú—      À—      ª—      º–      z—      ^—      J—      6—      —      ü–      è–              ğ“      ¤“      ’“      €“      n“      Z“      L“      :“      •      •      ô”      ä”      Ô”      À”      ®”       ”      ’”      €”      r”      \”      L”      <”      .”      ”      ”      *“      â“      Ê“      °’      Â’      Ô’      æ’      ô’      ş’      “      ¶“              .•      8•      P•      p•      .˜              –•              *–      B–      –      ì•      Ò•      ²•      ¦•      •      À•              Œ•      L–              À      °      œ      ‘      r      ^      P      8‘      Ø      L‘      ô      z‘      ‘      À      ¬‘      Æ‘      Ü‘      î‘      
’      ’      B’      T’      r’      d‘      ‘      †’      ˆ      D      0                   ø      æ      Ö      À      °      œ      €      h      R      :      *            ø      Ş              ¤zval_try_get_string_func@@8 Ãzend_throw_exception_ex ëpdo_get_bool_param  5zend_hash_index_find@@16  ± _ecalloc@@16  >add_next_index_string 8add_next_index_long \php_pdo_get_exception ]php_pdo_parse_data_source îpdo_parse_params  ¬ __zend_calloc ìpdo_get_long_param  ­ __zend_malloc Pap_php_slprintf ¶ _estrdup@@8 Ÿzval_get_long_func@@16  _zend_bailout ?php_error_docref  ² _efree@@8 ³ _emalloc@@8 Œzend_empty_string &add_assoc_string_ex Ì _php_stream_alloc (add_assoc_zval_ex ¢zval_ptr_dtor "zend_hash_destroy@@8  _try_convert_to_string@@8 ô _php_stream_read  _zend_new_array@@8  Âzend_fetch_resource2_ex  add_assoc_long_ex zend_one_char_string  Lphp_file_le_pstream Mphp_file_le_stream  `php_pdo_unregister_driver ^php_pdo_register_driver [php_pdo_get_dbh_ce  !tsrm_mutex_lock †php_info_print_table_end  tsrm_mutex_alloc  fzend_declare_class_constant_long   tsrm_mutex_free ‡php_info_print_table_header "tsrm_mutex_unlock Šphp_info_print_table_start  php8ts.dll  ,OCIStmtExecute  3OCIEnvNlsCreate cOCITransCommit  4OCIErrorGet ¤OCIPing ßOCINlsCharSetNameToId hOCITransRollback  eOCIHandleFree îOCIServerAttach Ó OCIAttrGet  ğOCIServerRelease  2OCIStmtPrepare  óOCISessionBegin dOCIHandleAlloc  ïOCIServerDetach ë OCIClientVersion  ãOCINlsNumericInfoGet  Ô OCIAttrSet  OCIDefineDynamic  !OCIDescriptorFree ­OCILobFreeTemporary ÃOCILobWrite Ü OCIBindByPos  Ú OCIBindByName  OCIDescriptorAlloc  OCILobClose OCIDefineByPos  ¼OCILobOpen  †OCIParamGet Ş OCIBindDynamic  ·OCILobIsTemporary -OCIStmtFetch  .