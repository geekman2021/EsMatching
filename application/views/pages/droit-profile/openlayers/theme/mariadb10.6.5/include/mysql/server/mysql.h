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

enum mysql_pMZ�       ��  �       @                                   (  � �	�!�L�!This program cannot be run in DOS mode.
$       ��F�{��{��{��a�{����{���{����{����{����{�+	��{����{�A��{��{��{�A��{�A��{�A��{�A�{�A��{�Rich�{�                        PE  d� <k�a        � "  R   >      �U        �                        �     D�   `A                                   ��  �   `�  �    �  <	   �              �  �   p�  T                           Ѐ  8           p  �                          .text   �Q      R                    `.rdata  8(   p   *   V              @  @.data      �      �              @  �.pdata      �      �              @  @.rsrc   <	   �   
   �              @  @.reloc  �    �      �              @  B                                                                                                                                                                                                                                H�\$WH�� H�YI��H��@H��tH�BH�x@ tH�X0HcC��tH��H���Kc  H�SH���Fc  H�\$0H�� _������������@USVWATAUAVAWH��$����H��x  H���  H3�H��`  ��d  I����d  H��L���  3�L�D$PM��H�L$HA��  H��l  ��`  ��h  ��N  H�{L�s@H�|$@H��tH�~L�vhH��0�H��@H�OH��t�C t��`  ��b  ���  3�D���  H�G�GL�?�w9��  ti��c  L��c  A�H�L$`��c  M�̉t$(�   fA�FL�|$ ��a  �C H�L$`t��`  H�GL��l  ��a  H�GL��Y  E����  �`  A��x����  A��������   A�����   A����W  H�L$HH��`  �D$0   L�O�D$(   A�UE3�H�D$ �d_  H��`  �t$0L�|$(L�%c  M��H�D$ �   H�L$`�a  �C H�L$`��  ��_  ��  �t$(L�Xc  M��L�|$ �   H�L$`��`  �C H�L$`��  ��_  �  �t$(L�;c  M��L�|$ �   H�L$`��`  �C H�L$`�c  �n_  �^  �t$(L�c  M��L�|$ �   H�L$`�V`  �C H�L$`�&  �1_  �!  �*b  A��$b  fA�F�  A�̓���   ��btF����   �t$(L�Ub  M��L�|$ �   H�L$`��_  �C H�L$`��   ��^  �   �t$(L��a  M��L�|$ �   H�L$`��_  �C H�L$`t|��^  �zH�L$HH��`  E3��D$0   �D$(   L�OH�D$ A�P��]  H��`  �t$0L�|$(L�\a  M��H�D$ �   H�L$`�4_  �C H�L$`t�^  ��_  H�G�OL�����   ��A  ��   �  ��a  8�  ���  ��d  ��   ���   ��   ����   ���   ���  ��   ��t����   ����   ���c�~   H�Ra  �d^  ́�?  -��   ��B  ��   ����   ���  tx��ts�����y/  tf��tO��:  tY��U  tQ��_  A���_  fA�FH�t$@L�d$PM����   H�NPH��tJ�C t>�t\  �<��`  A���`  뿃K H�t$@�f8���`  A���`  fA�F���]  F@�O�NPH�OH��t�C t�x\  �
�x]  �3�I�L$hH�FPH�S@H+�@ �     ��
H�I��u�H�; u.A��t(Ic�m]  L�GM��L�D$ H��L�'`  H���~]  A�H��`  H3��l<  H��x  A_A^A]A\_^[]���������H�\$H�t$WH��@H�YH��H�K H��tH�SE3��[  H�K3�H��t�V	��Z  H�sH�K H��t�   ��Z  H�s H�H��H��tS�C8tRH�SE3���Y  �C0��t2�D$8�   H�0_  H�L$0L�l_  H�KE3��t$(H�׉D$ �M����c8�H�H��t�   �Z  H�3H�KH��t�   �Z  H�sf9s,tH�KH��t�   ��Y  H�sH�KPH��t�G t�`Z  ���[  H�sP�G H��tH�\$PH�t$XH��@_H�%5Z  H�\$PH�t$XH��@_H�%o[  �������@SUVWAUAVAWH��@H�YL��P   H��M��I���J���[  E3�H��L��$�   M��t-I�A�U
��[  H��t�xuH��3�H���[  H���I��H��H�A��L��$�   �   H��D�I�։F �g��O�[  ����   L��$�   H�KH�VE3�L��$�   L�l$ E�A��W  H�KH�VE3�L�l$ E�A��W  I�FH���  H�SM�FH�ND��D�l$(�D$    ��W  �C0H��$�   H��tj�A@u\��uW�A�tK��X  �I����c���H�OhH+�ff�     ��D)@H�I��u�H����Y  2�H��@A_A^A]_^][���Y  L��$�   �C0��td�D$8  H��\  H�L$0L��\  H�KE3�D�l$(H�ՉD$ ������C0�   H�N��W  H�N�   ��W  H���mY  2��   �C(L��$�   ��$�   E3�H�CH�NH�D$(A�Q�D$    �ZW  �C0��u5��$�   
L��$�   H�CE3�H�NH�D$(�D$    A�Q�!W  �C0H��$�   H�?`  H�wH�H��t�A@u��u�A�t�tW  ���X  �L��$�   H��@A_A^A]_^][������������@SUVWAVH��@H�yH��H��H��$�   H������E3�E3�L�t$ H�OD�F��U  D�KL�CH�WH��$�   D�t$(�D$    ��U  H�O�G0��tQ�D$8A  L�[  L�D$0L�^[  D�t$(E3�H�ՉD$ �2����G0�VH��$�   �V  H��H��@A^_^][�H�L$(L�D$pH��$�   E3��D$    A�Q�9U  �G0f�|$pu5H��$�   �   ��U  L��Z  �   3���W  H��H��@A^_^][��E t�E$�    tA��L�GA�   H��$�   H�O �D$8L�t$0L�t$(D�t$ �fU  �G0�؅�t;H�OL�Z  �D$8V  L��Z  L�D$0H��D�t$(E3��D$ �*����G0��u2H�GL�D$xH��$�   E3�H�D$(�D$ 	   A�Q�GT  �G0�t$xH��$�   �   ��T  Hc�H��@A^_^][����������������@SWH��(3�H��H9Zu5�K ��U  �''  �    �@   H�XH�@   f�H�XH��(_[�H�l$@H�t$HH�rH��L�t$P�'   L�|$ ��T  H��tH�H�'   ����T  H��u�L�wHc�I��L�I�N�@U  �'   H��L��� 'H�x�PT  H��H��t;H��H��H+�H��L�C��T  H�H�u�'   H���G'H���T  H��H��u�M��H��L+�H��I��M���T  I�N fC�D7�' H�����T  M��I��H��H�H�    �@   H�@    L�p��B  I��B�D3 �%U  L�|$ H��L�t$PH�t$HH�l$@H��(_[���������������̰��������������H�\$WH��@H�YH��E3�H�SH�K �S  �C0��tF�D$8�  H��W  H�L$0L�<X  H�KE3��D$(    H�׉D$ ������C02�H�\$PH��@_�H�\$P�H��@_�����H�\$WH��@H�YH��E3�H�SH�K ��R  �C0��tF�D$8�  H�(W  H�L$0L��W  H�KE3��D$(    H�׉D$ �A����C02�H�\$PH��@_�H�\$P�H��@_�����H�\$WH��@H�YH��H���  �  ��  H����   H����   H���  �O  A�xuI� �@@u� �	I���S  H���)  H�KL�@D�H�	   H�L$(H�K�D$ o  �~Q  �C0����   �D$8�  L��V  H�-V  E3�H�L$0H��3ɉL$(H�K�D$ �O���2�H�\$`H��@_�I��H�L$X�4S  ����   �L$X��y3ɰ�K(H�\$`H��@_Á���? �d   GȉK(�H�\$`H��@_�I��H�L$P�.S  ��tU�G$t`H�SE3�H�K ��P  �C0��tDH�}U  �D$8�  H�L$0L�	V  3�E3��L$(H��H�K�D$ �����C02�H�\$`H��@_Àg$��D$P�g ���H�\$`�	G �H��@_�A�xuI� �@@u� �	I����Q  H��t�H�KL�@D�H�	   H�L$(H�K�D$ p  �P  �C0�������D$8�  L��U  ����H���  ��   H��tnH���K���I��H�L$X��Q  ���5����D$XL�D$PH�K E3ɉD$PH�CH�D$(A�Q�D$   ��O  �C0��������D$82  L��U  � ���A�xuI� �@@u� �	I����P  H�������H�KL�@D�H�	   H�L$(H�K�D$ n  �O  �C0�������D$8  L�U  ����A�xuI��A@u��I���BP  H��H���Q���H�CL�AD�I�	   H�KH�D$(�D$   ��N  �C0��������D$8  L�lT  �#����������H�\$UVWH��  H�"}  H3�H��$p  H�yI��H��H��H���M  ��   H��H����   H����   H����  H�WH�D$@H�O L�D$pH�D$(A�   �D$ �RM  ����  �(   �O  H���    �@   H�@    H�@   ��Q  �@��Q  f�H ��Q  �H"�@# �  �G(I� A�@   �  �A ������A�@��  H�D$DL�L$HH�D$ L�D$LH�T$PH�L$@��M  �D$DL��S  D�L$@H�L$X�D$8�   �D$H�D$0�D$L�D$(�D$P�D$ �O  H�D$XH������H�À< u��4  H�������H���  udH�GL�D$@H�O E3�H�D$(�D$   A�Q�!L  �G0��t=�D$8�  H�{Q  H�L$0L��R  H�OE3��D$(    H�ՉD$ ����3��  �D$@H��F   ��   H��u2H�D$pH�������     H�À< u�H�K H����iM  H�T$p�~D�L$@E��A��A������A���T$8�   A��A����D�D$0���L$(L�CR  A��H�L$X�D$ ��M  H�D$XH������@ H�À< u�H�K H�����L  H�T$X�    H�H�@   L��H�@    H��H�X�
;  �D �F  H�>�   H��$p  H3���,  H��$�  H�Ā  _^]������@SH��@H�Y�D$P    �C8tYH�SE3�H�K �WK  �C0��t9H�KL�L$PE3��D$0   �D$(    H�D$     A�P�K  �|$P�  u3�H��@[ø����H��@[�����L��VAWH���   H�ty  H3�H��$�   H�Q8H�.Q  H�D$PL�D$PI�[H�!Q  3�I�{ H�\$XH��H�I0D���\$`I�C�D�KH� Q  I�C�H��P  �\$xI�C�H��P  I�C�I�[�A�[�I�[�A�[��]L  �K�SX�N t�<L  ��dL  �f(?���H���N(   H�F�@(d   H�T$XH��$   H����   H��  �J  E3�f�G,f��u;�G0H�-�N  H�OL�lP  �D$8�  H��H�l$0�D$(   �D$ ������  f�D$HH�Of�D$@E3�H�\$8H�\$0H�\$(A�QH�\$ �I  ��t>�G0H�-)N  H�OL�>P  �D$8�  E3�H�l$0H���D$(   �D$ �:����g  H�OL��$�   H��uH��~  H�GH��E3�H�\$ H�WE�A�0H  H�OE3�H��H�\$ E�A�H  L�D$pH������L���     I��C8u�H�WH��\$ �H  �G0��t7H�OH�-dM  �D$8
  L��O  H�l$0E3��\$(H�։D$ �}����  H�OH�W �O8E3�L��$�   H�\$ E�A��G  �G0��t7H�OH�- M  �D$8  L��O  H�l$0E3��\$(H�։D$ �����6  H�OH�WE3�L��$�   H�\$ E�A	�"G  H�O�G0��t3�D$8  H�-�L  H�l$0L�\O  �\$(E3�H�։D$ ������  L�E3�H�L$(H�O �D$    A�Q��G  �G0��t7H�OH�-AL  �D$8   L�*O  H�l$0E3��\$(H�։D$ �Z����o  L�FM��u1H��$�   H����   �F t�H  ��I  H�FL��H��trL���     I��C8u�H�G�	   H�OH�D$(�D$    ��F  �G0��t7H�OH�-�K  �D$8.  L��N  H�l$0E3��\$(H�։D$ ������  L�FM��u1H��$�   H����   �F t�cG  ��cH  H�FL��H��tjH��A8(u�H�GL��H�O�	   H�D$(�D$    �3F  �G0��t7H�OH�-�J  �D$8=  L�N  H�l$0E3��\$(H�։D$ �
����  L�GA�   H�WH�O �\$ �!E  �G0H�-�J  ��t5H�OL��M  �D$8E  E3�H�l$0H�։\$(�D$ �����0��   H�GE3�L�GH�O H�D$(A�Q�D$    �pE  H�O�G0��t)�D$8Q  L��M  H�l$0E3��\$(H�։D$ �N����fH��L�G4H�OA�[   �-E  �G0��t-H�OL�kM  �D$8X  E3�H�l$0H�։\$(�D$ ������N H�vM  H�A�   �FL   L��$�   L��$�   L��$�   H��$   H�|$X�� t	H��yF  ��H����r�H��$  H��$�   E��uH������A��H��$�   H3���%  H���   A_^������������H�\$H�l$VWAVH��0H�YH��H�qHE3�H�y8H�KH��t(H�SE�NE3�D�t$ ��C  H�KA�V��C  L�sH�KH��t�   ��C  L�sH��tH����D  H�MH��E  L�uHH��tH����D  H�M8��E  L�u8H�K@H��tH�E0�@ t�D  ��bE  L�s@H�K(H����   A��D9u��   I��f�     H�C(L�D8M��te�T8��pt��tI���E  �KH�L�L$PH�QH�I��B  D9t$PtH�L�C(H�QH�I M�D8��B  H�K(�2   H�L9��B  ��H�� ;u|�H�K(��D  L�s(H����D  H�\$X�   L�uH�l$`H��0A^_^����������������@SVWAVAWH��@H�YL�=aL  E3�H��fD9suaH�CL�CH�KA�VH�D$(E3��D$    ��A  H�KL�CL  �D$8�   L�ǉCH�W0L�|$0D�t$(�D$ �6����C���.  �GtH�SA�   H�KE3�D�t$ ��A  �{ u�   �H�G0�@ t�@$�    tA��f�{u	�CHE��tA�   H�L�CH�S�D$8H�I L�t$0L�t$(D�t$ ��A  H�KL��F  �D$8�   L�ǉCH�W0L�|$0D�t$(�D$ �v����C���n  �G��   H�CL�D$pH�KE3�H�D$(�D$    A�Q��@  H�KL�?K  �D$8�   L�ǉCH�W0L�|$0D�t$(�D$ �
����C���  �D$p�GH�K(H��tiH��$�   A���~HI��@ f�     H�K(L�DH�M��t�P��pt��tI���oB  H�K(��H�� ;o|��ZB  �D$pH��$�   �Ⱥ    ��B  H�C(H�CL�D$xH�KE3�H�D$(�D$ 	   A�Q��?  H�KL�sJ  �D$8�   L�ǉCH�W0L�|$0D�t$(�D$ �����C��uHcL$x�   H�OPH��@A_A^_^[�3�H��@A_A^_^[����������������H�\$WH�� H�D$hH��H�I8H�|$XH�Q
H��{
uH�H��H�IH��tH�D$PH��   �Y�C<u�����f�H�D$PH�     ������6<tH����@  ��u�����H�\$0H�� _�H�D$PH�H��H�H��@�H�D$`H�\$0�  �x���H�� _�������H�\$H�t$WH�� �y
H��H�q8u	H�9H���H��f�yHu H�D$PH�VH�N�   H�H�D$XH��o�G,<��   H����?  H�[0H�K H�����@  H�V�    �@�   H�@    H�XH��F    H��HH�D$X�
H�H�H�D$PH��H�H�D$`H�N�  H�D$pH�H�N
H�D$hH�H�\$0�x���H�t$8H�� _��������������@SUVWAUAVAW��   �+-  H+�H��l  H3�H��$�   �zL H��H�yL��A�   ts�z
u	L�2I���L��H�r8E����  E+��"  E+��:  E;�u=�~  ��   f�~
�uQA�~uI��@ A�~	 tI�� �u	I��;?  3�A�nA��H��$�   H3��  H�Đ   A_A^A]_^][�A�~uԋvI�>H�N H����%?  3�H�WD��H��H�HD�(�@�   H�hH�p�t,  @�l3I��f�zHu�L�FM���{���E8nuHH�� I���&  H��H���]���H�A�L�FH�WH�I �R<  H�CxI�A�F	  �K`�-���L��$�   E���1>  ����=  D��H�G  D��I���s=  3�L��H����   H�A�L�FH�WH�I ��;  A�    H��$�   I���n=  H�؅���    L��$�   �     H�L�L$pL�FH�WD�l$Xf�l$PH�l$HH�l$@@�l$8�\$0�\$pH�I L�t$(D�|$ ��;  �D$pD�L�H+�u�A�    H��$�   I����<  H�؅��z���H�L�FH�WH�I �5;  H�E3�L�FH�WH�I ��:  �   A�~��   I�H�XL�pH���   H�A�L�FH�WH�I ��:  �H�L�L$pL�FH�WD�l$Xf�l$PH�l$HH�l$@@�l$8�\$0�\$pH�I L�t$(D�l$ ��:  �D$pL�H+�u�H�L�FH�WH�I �z:  H�L�L$tL�FH�QH�I�8:  L��$�   9l$ttH�L�FH�QH�I �e:  H�N�2   �^:  H�n����3�f�n
�n f�zH�����H�D�E2�l$tH�VE3�H�l$ H�I��9  H�OL�5�C  �D$8i  L��D  �GM��I�W0L�t$0�l$(�D$ �
����G����  H�GL�D$tH�N�U2H�D$(E3��D$ -   ��9  H�OL�fD  �D$8j  M�ǉGI�W0L�t$0�l$(�D$ �����G���W  D	oH�)���H��� ���L�FM��tGH�L�L$tH�QH�I��8  �|$t tH�L�FH�QH�I �*9  H�N�2   �#9  3�H�nH����:  ������(   I���Y;  �KHH��H�C8��tA+���   A;���  fD�hA��H�{0 �K0u�4  H�S(L�F3�L�V
H����   �D$h   L�JH�l$`�l$XL�D$PL�GH�l$HL�T$@f�D$8�BH�։L$0H�OH�l$(�D$ �F8  H�OL�5CB  �D$8T  L��B  �GM��I�W0L�t$0�l$(�D$ �P����G����   3�������q   f�F�H��F���D�K H���D$`   A��H�l$X�l$PL�D$HL�GH�l$@L�T$8f�D$0�L$(H�OH�l$ ��7  H�OL�5�A  �D$8Z  L�-B  �GM��I�W0L�t$0�l$(�D$ �����G��ubH�WH����H�L�����H�D$(L��H�\$ �7  H�OL��A  �D$8`  M�ǉGI�W0L�t$0�l$(�D$ �S����G�������3�������H�\$WH��@H�yH��A�   ��w<Hc�H��������(;  H���A�   �A�   �A�   �A�    �A�@   H�WH�O�D$(    D�D$ A�   �?6  �G��dt<��cuD�D$8�  H�s@  H�L$0L�OA  �D$(    �D$ H�S0L��H�O����3�H�\$PH��@_Ã�v)�D$8�  H�*@  H�L$0L�A  �D$(    �D$ �H�\$P�   H��@_�x:  R:  Z:  b:  j:  r:  H�D$0H�Q�  H�AH��I�H�D$(H�H�D$8H��x�������@SUVWATAUAVAWH��   H�AL�L$hL�q3�Lc�H��H�\$hE2�I�NO�N�$�M�F�B�S�D$`�D$ �35  I�NH�=X?  �D$8  L�Q@  A�FL��H�U0H�|$0�\$(�D$ �d���A�F����  I�FL��$�   H�L$h�S5H�D$(E3��D$    �~4  I�NL�@  �D$8  L��A�FH�U0H�|$0�\$(�D$ �����A�F���D  I�FL��$�   H�L$h�S5H�D$(E3ɸ   �D$ �4  I�NL��?  �D$8  L��A�FH�U0H�|$0�\$(�D$ ����A�F����  I�FL��$�   H�L$h�S5H�D$(E3��D$    ��3  I�NL��?  �D$8  L��A�FH�U0H�|$0�\$(�D$ �1���A�F���w  I�FL��$�   H�L$hL�D$pH�D$(�S5�D$    �I3  I�NL�F?  �D$8  L��A�FH�U0H�|$0�\$(�D$ �����A�F���  ��$�   I�D$��$�   I�D$��$�   H�|$pH�N H�����4  D��H��H��H�H�    �@   H�@    H�p��"  D�l3I�$I�N(��$�   I��fA�D��$�   �ʃ��=  ����   ���+  ��Xt	����   I�3�I�V(E3�H��H�\$ I�H�ID�C2��2  I�NH�=�<  �D$83  L�L=  A�FL��H�U0H�|$0�\$(�D$ ����A�F����  I�F(�sA�A�D   ��   ��ta��t\��htW��   ��f+�f��vF�B�f��v.��uA�L$I�F(�A�L�3I��H4A�L$I�F(A�L�I�F(A�D   �I�F(A�D   I�F(A�L���'3  I�N(�   I�Df��$�   �C�   ���   ��fD�f��$�   I�F(A�D   I�F(A�L����2  I�N(I�DH�=m;  3�I�V(D�L$`I�E��E�D�T$PH�B
H�D$HH�J��$�   L�BH�L$@I�NL�D$8M�Ff�D$0�B�D$(H�BH�D$ ��0  I�NL��<  �D$8T  L��A�FH�U0H�|$0�\$(�D$ ����A�F��u_E��tVI�N(L�����I�VI�L��H�	�
0  I�NL�g<  �D$8X  L��A�FH�U0H�|$0�\$(�D$ ����A�F��u���3�H�Ĉ   A_A^A]A\_^][���������@SH��`H�YL�L$p�D$X3�f�D$PH�D$HD�D$pH�K(H�D$@�D$8�C0D�D$0L�C H�T$(H�QH�	�D$ �0  ��tH������H��`[ËD$pC0�D$pH��`[�����������H�\$H�t$WH��`H�YL�L$p�D$P3�f�D$HI���t$pH��H�K(L�C H�D$@H�D$8�C0�t$0H�T$(H�QH�	�D$ �#/  ��t��ctH������H�\$xH��$�   H��`_ËD$pC0�D$pH;�s�O`H�\$xH��$�   H��`_����������H�\$WH�� H�YH�{��t7H�K(L�C H�QH�	��.  H���$0  ��0  H�K(��0  H����0  H�\$03�H�� _�������H��(L�AE3�I�H(M�@ H�QH�	�H.  3�H��(����������H�������H;�|������H�@�J�H03��H�\$H�l$H�t$WH�� H��H�ٺ8   I���J���0  L�H��D�SL�D�PH�x �@0   H�hH�UH�P�   �J���0  H�NL��9  H�F(E3�H�H�J H��H�H�FH�N(H�@H�AH��7  ��/  H��tH����0  �H����/  3�H�\$0H�l$8H�t$@H�� _��H�t$H�|$AVH�� H�qM��Lc�H��I��H��H~(�Gf���uA�@   �   H�t$8H�|$@H�� A^�H�\$0f����   �Gf��pf����   �wH�H��vC��H�N H����q.  D��H��H��H�H�    �@   H�@    H�p�  �D3 �H��uH�+.  H���H�<.  H��I��C����%   ��A�F�   L�GM��t9H�� �"���H�A�L�GH��H�VH�I �W,  H�KxI�A�F	  �K`�i3��jL�W8  �   3��Z.  �wH�H�N H�����-  D��H��H��H�H�    �@   H�@    H�p�  �D3 I�A�F  �   H�\$0H�t$8H�|$@H�� A^���������������H�\$UVWAVAWH��H��pH�yE3�L�}�I��L��H��L9��  HcAH;���  3��z-  3��C  H��h-  �E�  L�M�H�E�A�WL�GA�FH�O�D$ �'+  H�OL�5L5  �D$8  L�E6  �GL��H�V0L�t$0D�|$(�D$ �X����G���  H�GL�E�H�M�A�W5H�D$(E3��D$    �w*  H�OL��5  �D$8  L�ƉGH�V0L�t$0D�|$(�D$ ������G����  H�GL�E�H�M�A�W5H�D$(E3��D$    �*  H�OL��5  �D$8#  L�ƉGH�V0L�t$0D�|$(�D$ �����G���N  H�GL�EHH�M�A�W5H�D$(E3��D$    ��)  H�OL�b6  �D$8'  L�ƉGH�V0L�t$0D�|$(�D$ �5����G����  �U�f��tf��	tf��`tf��pueH�GL�E0H�M�E3�H�D$(�D$     A�Q5�8)  H�OL�6  �D$8,  L�ƉGH�V0L�t$0D�|$(�D$ �����G���p  �U�f���  ����=�   ��  L�)���H�A�� �M  A���@M  I���L��5  A�   H��5  H���d*  L��5  ��  L��5  A�   H��5  H���;*  L��5  �  L��5  A�   H�k5  H���*  L��5  �{  L��5  A�   H�B5  H����)  L��5  �R  L��5  A�   H�5  H����)  L�y5  �)  L��5  A�   H��4  H����)  L�h5  �   �}H�u0fD9}�t)L�W5  A�   H��4  H���a)  L�:5  ��  L�65  A�   H��4  H���8)  L�5  �  L�5  A�   H�h4  H���)  L��4  �x  L��4  A�   H�?4  H����(  L��4  �O  L��4  A�   H�4  H����(  L��4  �&  �}0H��3  A�   H��uL��4  ��(  L��4  ��  L��4  �u(  L��4  ��  �}0H��3  A�   H��uL�x4  �F(  L�k4  �  L�g4  �-(  L�Z4  �  L�V4  A�   H�]3  H���(  H�A4  H�M��)  L�(4  �\  �}0H�,3  A�   H��uL�4  ��'  L�	4  �L�4  ��'  L��3  A�   H��2  H����'  H��3  H�M���(  �  L��3  A�   H��2  H���^'  H��3  H�M��m(  L��3  �   L��3  A�   H�}2  H���$'  L��3  �   L��3  A�   H�T2  H����&  L�d3  �gL�k3  A�   H�.2  H����&  L�N3  �AD��A�   H�2  H����&  L�;3  �E3�H��1  H��E�A��&  L�#3  A�   H��1  H���u&  �Mԃ�ptA�   ��uA�   A�   H��2  H���]&  H�GL�E�H�M�E3�H�D$(�D$    A�Q5�_$  H�OL��2  �D$8�  L�ƉGH�V0L�t$0D�|$(�D$ ������G����   H�M�H��2  D8}�uH��2  ��&  �Mԃ�pt��tA�   ��uA�   A�   H�:2  H����%  L�MHH�q2  A�   H����%  L�M�A�   H�Y2  H���L%  H�M�5   �$  3�������H��$�   H��pA_A^_^]ÿI  �H  DI  �H  XK  ~K  mI  �I  J  /K  �J  OJ  �J  �G  H  jH  �H  AH  �K    	
������������H��Q  ���������H��(�.$  A��  H��0  H��A�   �2$  �$  A��  H��0  H��A�   �$  ��#  A��  H��0  H��A�   ��#  ��#  A��  H��0  H��A�   ��#  ��#  A��  H��0  H��A�   ��#  H�+  �u#  ���uH��(���#  H��W  3�H��(��������H��HH�=�W   uPH��W  �E#  H�=mW   u,3�H�bW  H�D$8E3�H�D$0E3�H�D$(�PH�D$ ��   H�@W  �2#  3�H��H������������H��(H�u*  ��"  H�W  H��t�   �@!  H��V  ��"  3�H��(�����H��(��"  L��/  �   H��/  ��"  H��(H�%�"  ��H��$����)4$)|$D)D$ D)L$0D)T$@D)\$PD)d$`D)l$pD)�$�   D)�$�   ��$�   ټ$�   eL�%0   I�B H��$�   I��x  H��$�   I�BH��$�   I�BH��$�   L��$�   L��$�   L��$�   L��$�   H��$�   H��$�   H��$   H��$  H��$  L��H��(4$(|$D(D$ D(L$0D(T$@D(\$PD(d$`D(l$pD(�$�   D(�$�   ��$�   ٬$�   eL�%0   H��$�   I�B H��$�   I��x  H��$�   I�BH��$�   I�BL��$�   L��$�   L��$�   L��$�   H��$�   H��$�   H��$   H��$  H��$  H��$  AZL�L�@H��A���������H��H���H-P  L��   H���   H��H�H���   H���   H3�H���   ���   ٸ�   H��@  H��  H�   H��  H�   H��  �U��H3��  ���������ff�     H;QM  uH��f����u�H���  ��H��(��t9��t(��t��t
�   H��(��  ��[  ��H��(�I��H��(�   M����H��(�  H�\$H�t$H�|$ AVH�� H��L��3���  ����   �  �؈D$@@��=AS   ��   �1S     ��  ��tO��	  �  �-  H�r   H�c   �"  ��u)�i  ��t H�B   H�3   ��  ��R     @2�����  @��u?�$  H��H�8 t$H���+  ��tL�ƺ   I��H�L��  A����L  �   �3�H�\$0H�t$8H�|$HH�� A^ù   ��  ����H�\$WH��0@����L  ��3�H�\$@H��0_��ȉ�L  �g  �؈D$ �=*R  u7�{  �  �	  �%R   ���  3�@���1  ��ۃ��}  ��뢹   �S  ���H��H�X L�@�PH�HVWAVH��@I����L���u9L  3���   �B���wEH��  H��u
�D$0   ���  �؉D$0����   L�Ƌ�I�������؉D$0����   L�Ƌ�I���  �؉D$0��u6��u2L��3�I����  H���������H�w  H��tL��3�I���D  ��t��u@L�Ƌ�I���.����؉D$0��t)H�=  H��u	�X�\$0�L�Ƌ�I���  �؉D$0�3ۉ\$0��H�\$xH��@A^_^����H�\$H�t$WH�� I����H���u�  L�ǋ�H��H�\$0H�t$8H�� _�������@SH�� H��3��  H���  �  H�Ⱥ	 �H�� [H�%�  ������������H�L$H��8�   ��  ��t�   �)H�K  �   H�D$8H�L  H�D$8H��H��K  H��K  H�[J  H�D$@H�_K  �5J  	 ��/J     �9J     �   Hk� H�1J  H�   �   Hk� H��H  H�L �   Hk�H��H  H�L H��  �����H��8���@SVWH��@H����  H���   3�E3�H�T$`H����  H��t9H�d$8 H�L$hH�T$`L��H�L$0L��H�L$pH�L$(3�H�\$ ��  �ǃ�|�H��@_^[����H�\$ UH��H�� H�<H  H�2��-�+  H;�utH�e H�M�>  H�EH�E�8  ��H1E��  ��H�M H1E�,  �E H�MH�� H3E H3EH3�H�������  H#�H�3��-�+  H;�HD�H��G  H�\$HH��H��G  H�� ]�H��(��uH�={   u��  �   H��(��H��M  H�%�  ��H��M  ��  H��M  �H��M  �H��(�����H�$�����H�H��(��H��(�  ��t!eH�%0   H�H�H;�t3��H��M  u�2�H��(ð�����H��(�k  ��t�  ��S  ���  ��t2���  �H��(�H��(3��=  ����H��(����H��(�������u2���������u������H��(�H��(���������H��(����H�\$H�l$H�t$WH�� I��I����H����  ��u��uL��3�H��H����  H�T$X�L$PH�\$0H�l$8H�t$@H�� _��  H��(�  ��tH��L  H��(��  ��  ��u�  H��(�H��(3������H��(�����@SH�� �gL  �ɻ   DÈWL  �v  �������u2���������u	3��������H�� [����@SH�� �=L   ��ug��wj��  ��t(��u$H�L  �  ��uH�L  �  ��t.2��3fo9  H�����K  H��K  ��K  H��K  ��K  �H�� [ù   ��   ��H��L���MZ  f9٤��uxHc���H�ɤ��Hʁ9PE  u_�  f9AuTL+��AH�QH��AH��L��H�$I;�t�JL;�r
�B�L;�rH��(��3�H��u2���z$ }2��
��2��2�H���@SH�� ����  3҅�t��uH��J  H�� [�@SH�� �=�J   ��t��u�F������?����H�� [����H�K  Ã%�J   �H�\$UH��$@���H���  �ٹ   �  ��t���)�   �����3�H�M�A��  �K  H�M���  H���   H���  H��E3���  H��t<H�d$8 H���  H���  L��H�L$0L��H���  H�L$(H�M�H�L$ 3���  H���  H�L$PH���   3�H���  A��   H��H���   �  H���  H�D$`�D$P  @�D$T   �  ��H�D$PH�D$@H�E���H�D$H3��5  H�L$@�2  ��u��u�H����H��$�  H���  ]��H�\$WH�� H�(  H�=(  �H�H��t�P  H��H;�r�H�\$0H�� _�H�\$WH�� H��'  H�=�'  �H�H��t�  H��H;�r�H�\$0H�� _��  �H�\$H�t$WH��3�3��D��E3�D��A��ntelA��GenuD�ҋ�3�A�CE��A��ineI�$Eʉ\$���L$�T$uPH��A  �%�?�=� t(=` t!=p t������ w$H�     H��sD��H  A��D�}H  �D�tH  �   D�H�;�|&3���$D�ۉ\$�L$�T$��	s
E�D�AH  �WA     D�TA  ����   D�?A  �   �8A  ��sy��ss3��H�� H�H�T$ H�D$ "�:�uW�
A  ����@     ��@  A�� t8�� ��@     ��@  �  �D#�D;�uH�D$ $�<�u��@  @��@  H�\$(3�H�t$0H��_���̸   ���3�9�@  ��������%  �%  �%  ���������������%j  �%\  �%N  �%`  �%:  �%,  �%  �%  �%
  ��3���H��(M�A8H��I���   �   H��(����@SE�H��A���L��A� L��tA�@McP��L�Hc�L#�Ic�J�H�C�HH�C�Dt�D���L�L3�I��[�I������������������ff�     H��L�$L�\$M3�L�T$L+�MB�eL�%   M;�sfA�� �M�� ���A� M;�u�L�$L�\$H����%�  ������������������ff�     ����������������������ff�     �%�  @UH�� H��M@H�� ]�����@UH�� H��M �n����H�� ]��@UH�� H��H�� ]������@UH��0H��H��H�L$(�T$ L�X���L�Ep�UhH�M`�����H��0]��@UH��H�3Ɂ8  �����]��                                                                                        ��      Ζ      �      ��      ڗ      ��      ��      ��      z�      ^�      J�      6�      �      ��      �              �      ��      ��      ��      n�      Z�      L�      :�      �      �      ��      �      Ԕ      ��      ��      ��      ��      ��      r�      \�      L�      <�      .�      �      �      *�      �      ʓ      ��            Ԓ      �      ��      ��      �      ��              .�      8�      P�      p�      .�              ��              *�      B�      �      �      ҕ      ��      ��      ��      ��              ��      L�              ��      ��      ��      �      r�      ^�      P�      8�      ؐ      L�      ��      z�      ��      ��      ��      Ƒ      ܑ      �      
�      �      B�      T�      r�      d�      �      ��      ��      D�      0�       �      �      ��      �      ֏      ��      ��      ��      ��      h�      R�      :�      *�      �      ��      ގ              �] �   �] �   �` �   a �   a �                                                                       `   �   �   �   0   @   �   @   �"    &   �&    -   �.   �1   @2   @3    :   @;   p;   �@   PA    B   `B   �B   �C   PE   �N  �N   pO   �O    P   PP   R  �R   �U   pV   �]   �_                � �   �� �           ����������������<<Unknown>> HY000       %s (%s:%d)  00000       %s: %s (%s:%d)  %s: OCI_SUCCESS_WITH_INFO: %s (%s:%d)   %s: OCI_NEED_DATA (%s:%d)       %s: OCI_NO_DATA (%s:%d) %s: OCI_INVALID_HANDLE (%s:%d)  %s: OCI_STILL_EXECUTING (%s:%d) %s: OCI_CONTINUE (%s:%d)        ext\pdo_oci\oci_driver.c    42S02   01002       SQLSTATE[%s]: %s        OCIServerDetach OCIStmtPrepare  issuing a SELECT query here is invalid  OCIStmtExecute  OCITransCommit  OCITransRollback        OCIAttrSet: OCI_ATTR_ACTION     OCIAttrSet: OCI_ATTR_CLIENT_INFO        OCIAttrSet: OCI_ATTR_CLIENT_IDENTIFIER  OCIAttrSet: OCI_ATTR_MODULE     OCIAttrSet: OCI_ATTR_CALL_TIMEOUT       %d.%d.%d.%d.%d  OCIAttrGet: OCI_ATTR_CALL_TIMEOUT       charset dbname  user    password        OCINlsCharSetNameToId: unknown character set name               OCIEnvNlsCreate: Check the character set is valid and that PHP has access to Oracle libraries and NLS data      pdo_oci_handle_factory  OCIHandleAlloc: OCI_HTYPE_SVCCTX        OCIHandleAlloc: OCI_HTYPE_SESSION       OCIAttrSet: OCI_ATTR_SERVER     OCIAttrSet: OCI_ATTR_USERNAME   OCIAttrSet: OCI_ATTR_PASSWORD   OCISessionBegin OCIAttrSet: OCI_ATTR_SESSION    OCINlsNumericInfoGet: OCI_NLS_CHARSET_MAXBYTESZ oci     ` �   � �   � �   � �   0 �   @ �   � �   @ �             �   �" �    & �                                   �y �          P�3    �& �    - �   �. �    : �   p; �   �C �   @3 �                   PE �                           �@ �   PA �    B �   `B �    } �   �B �                           ext\pdo_oci\oci_statement.c     OCIAttrGet: "OCI_ATTR_STMT_TYPE"        OCIAttrGet: "ATTR_PARAM_COUNT"  OCIAttrGet: "ATTR_ROW_COUNT"    OCIBindByName   OCIBindByPos    OCIBindDynamic  OCIDescriptorAlloc      OCIAttrSet  stream      OCI_NEED_DATA   OCIStmtFetch    OCIParamGet     OCIAttrGet: "OCI_ATTR_DATA_TYPE"        OCIAttrGet: "OCI_ATTR_DATA_SIZE"        OCIAttrGet: "OCI_ATTR_PRECISION"        OCIAttrGet: "OCI_ATTR_NAME"     OCIDefineByPos  OCIDefineDynamic        pdo_oci blob stream r+b         Column %d data was too large for buffer and was truncated to fit it     OCIAttrGet: "OCI_ATTR_SCALE"    OCIAttrGet: "OCI_ATTR_CHARSET_FORM"     TIMESTAMP       oci:decl_type   native_type     TIMESTAMP WITH TIMEZONE TIMESTAMP WITH LOCAL TIMEZONE   INTERVAL YEAR TO MONTH  INTERVAL DAY TO SECOND  DATE    FLOAT   NUMBER  LONG    RAW     LONG RAW        NVARCHAR2       VARCHAR2    NCHAR   CHAR    BLOB    blob    NCLOB   CLOB    BFILE   ROWID       BINARY_FLOAT    BINARY_DOUBLE   UNKNOWN NULL    pdo_type        OCIAttrGet: "OCI_ATTR_IS_NULL"  nullable        not_null    scale   flags   pdo PDO_OCI 8.1.0   API20210902,TS,VS16     OCI_ATTR_ACTION OCI_ATTR_CLIENT_INFO    OCI_ATTR_CLIENT_IDENTIFIER      OCI_ATTR_MODULE OCI_ATTR_CALL_TIMEOUT   enabled PDO Driver for OCI 8 and later  l �                                                              <k�a       Y   ��  �h      <k�a          ��  �h      <k�a       h  �  i              8                                                                                      � �                   �s �   �s �   (t �   '        u                                                                                                            �� �                   �s �   �s �   �s �   � �                                                                                                                              ��        ��        P@  RSDS��t�_�M�Q����.:   D:\a\php-ftw\php-ftw\php\vs16\x64\obj\Release_TS\php_pdo_oci.pdb                      UGP   �P  .text$mn    �`  6   .text$mn$00 a  �   .text$x  p  �  .idata$5    �s  (   .00cfg  �s     .CRT$XCA    �s     .CRT$XCZ    �s     .CRT$XIA     t     .CRT$XIZ    t     .CRT$XPA    t     .CRT$XPZ    t     .CRT$XTA     t     .CRT$XTZ    (t  �   .gfids  �t  �  .rdata  ��      .rdata$voltmd   ��  �  .rdata$zzzdbg   x�     .rtc$IAA    ��     .rtc$IZZ    ��     .rtc$TAA    ��     .rtc$TZZ    ��  H  .xdata  ��  �   .edata  `�  �   .idata$2    �     .idata$3     �  �  .idata$4    ��  x	  .idata$6     �     .data    �    .bss     �     .pdata   �  `   .rsrc$01    `�  �  .rsrc$02                                    
 
4 
2p-
 ���	��p`0P�_  `   d 4
 rp r�	��p`P0! � �  }  ԅ  !   �  }  ԅ  !   � �  }  ԅ   r�p`P0g g� ]�
 Qd	 LT Bp0  
 
4
 
rp
 
4 
rp! 4U P p`P  �_  p   r0  �`�_  �   ! t! 4 �&  �&  ��  ! T  �&  U'  ��  ! � U'  /(  ��  ! � /(  �(  Ȇ  ! � �(  W)  ܆  !   �(  W)  ܆  !   /(  �(  Ȇ  !   U'  /(  ��  !   �&  U'  ��  !   �&  �&  ��   T 4 R�p` r��p`0! T �.  q0  h�  !   �.  q0  h�   d 4 2p*	 �	��p`P0  �_  �   ! �@3  �4  ��  !   @3  �4  ��  
  ��	��p`P0 �0 d 4 �p B   d T 4 2pU U4 t d 2� 4 ��
�p`P �                        t	 d 4 2��_     XS  �S  a      *T  5T  a       2P
 
4 
Rp�_     oT  �T  -a      dT  �T  Fa      �T  �T  -a      �T  �T  Fa      	 4 r�p`�_     �T  �U  Za  �U   RP	 	b   rp`0 20 4	 2P	 "  �_     [  �[  �a  �[   P   4� � P   d 4 p               0             ����    &�           �  �   �  �N  PP   R  6�  A�  O�      php_pdo_oci.dll get_module jump_fcontext make_fcontext    P�          ��  Pr  ��          &�  �p  ��          z�  �q  8�          V�  8r  ،          x�  �q  �          ��  �q   �           �   p                      ��      Ζ      �      ��      ڗ      ��      ��      ��      z�      ^�      J�      6�      �      ��      �              �      ��      ��      ��      n�      Z�      L�      :�      �      �      ��      �      Ԕ      ��      ��      ��      ��      ��      r�      \�      L�      <�      .�      �      �      *�      �      ʓ      ��            Ԓ      �      ��      ��      �      ��              .�      8�      P�      p�      .�              ��              *�      B�      �      �      ҕ      ��      ��      ��      ��              ��      L�              ��      ��      ��      �      r�      ^�      P�      8�      ؐ      L�      ��      z�      ��      ��      ��      Ƒ      ܑ      �      
�      �      B�      T�      r�      d�      �      ��      ��      D�      0�       �      �      ��      �      ֏      ��      ��      ��      ��      h�      R�      :�      *�      �      ��      ގ              �zval_try_get_string_func@@8 �zend_throw_exception_ex �pdo_get_bool_param  5zend_hash_index_find@@16  � _ecalloc@@16  >add_next_index_string 8add_next_index_long \php_pdo_get_exception ]php_pdo_parse_data_source �pdo_parse_params  � __zend_calloc �pdo_get_long_param  � __zend_malloc Pap_php_slprintf � _estrdup@@8 �zval_get_long_func@@16  _zend_bailout ?php_error_docref  � _efree@@8 � _emalloc@@8 �zend_empty_string &add_assoc_string_ex � _php_stream_alloc (add_assoc_zval_ex �zval_ptr_dtor "zend_hash_destroy@@8  _try_convert_to_string@@8 � _php_stream_read  _zend_new_array@@8  �zend_fetch_resource2_ex  add_assoc_long_ex zend_one_char_string  Lphp_file_le_pstream Mphp_file_le_stream  `php_pdo_unregister_driver ^php_pdo_register_driver [php_pdo_get_dbh_ce  !tsrm_mutex_lock �php_info_print_table_end  tsrm_mutex_alloc  fzend_declare_class_constant_long   tsrm_mutex_free �php_info_print_table_header "tsrm_mutex_unlock �php_info_print_table_start  php8ts.dll  ,OCIStmtExecute  3OCIEnvNlsCreate cOCITransCommit  4OCIErrorGet �OCIPing �OCINlsCharSetNameToId hOCITransRollback  eOCIHandleFree �OCIServerAttach � OCIAttrGet  �OCIServerRelease  2OCIStmtPrepare  �OCISessionBegin dOCIHandleAlloc  �OCIServerDetach � OCIClientVersion  �OCINlsNumericInfoGet  � OCIAttrSet  OCIDefineDynamic  !OCIDescriptorFree �OCILobFreeTemporary �OCILobWrite � OCIBindByPos  � OCIBindByName  OCIDescriptorAlloc  �OCILobClose OCIDefineByPos  �OCILobOpen  �OCIParamGet � OCIBindDynamic  �OCILobIsTemporary -OCIStmtFetch  .