/* Copyright (c) 2005, 2012, Oracle and/or its affiliates.
   Copyright (c) 2009, 2017, MariaDB Corporation.

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

#ifndef _sql_plugin_h
#define _sql_plugin_h

/*
  the following #define adds server-only members to enum_mysql_show_type,
  that is defined in plugin.h
*/
#define SHOW_always_last SHOW_KEY_CACHE_LONG, \
            SHOW_HAVE, SHOW_MY_BOOL, SHOW_HA_ROWS, SHOW_SYS, \
            SHOW_LONG_NOFLUSH, SHOW_LEX_STRING, SHOW_ATOMIC_COUNTER_UINT32_T, \
      /* SHOW_*_STATUS must be at the end, SHOW_LONG_STATUS being first */ \
            SHOW_LONG_STATUS, SHOW_DOUBLE_STATUS, SHOW_LONGLONG_STATUS, \
            SHOW_UINT32_STATUS
#include "mariadb.h"
#undef SHOW_always_last

#include "m_string.h"                       /* LEX_STRING */
#include "my_alloc.h"                       /* MEM_ROOT */

class sys_var;
enum SHOW_COMP_OPTION { SHOW_OPTION_YES, SHOW_OPTION_NO, SHOW_OPTION_DISABLED};
enum enum_plugin_load_option { PLUGIN_OFF, PLUGIN_ON, PLUGIN_FORCE,
  PLUGIN_FORCE_PLUS_PERMANENT };
extern const char *global_plugin_typelib_names[];

extern volatile int global_plugin_version;
extern ulong dlopen_count;

#include <my_sys.h>
#include "sql_list.h"

#ifdef DBUG_OFF
#define plugin_ref_to_int(A) A
#define plugin_int_to_ref(A) A
#else
#define plugin_ref_to_int(A) (A ? A[0] : NULL)
#define plugin_int_to_ref(A) &(A)
#endif

/*
  the following flags are valid for plugin_init()
*/
#define PLUGIN_INIT_SKIP_PLUGIN_TABLE    1U
#define PLUGIN_INIT_SKIP_INITIALIZATION  2U

#define INITIAL_LEX_PLUGIN_LIST_SIZE    16

typedef enum enum_mysql_show_type SHOW_TYPE;
typedef struct st_mysql_show_var SHOW_VAR;

#define MYSQL_ANY_PLUGIN         -1

/*
  different values of st_plugin_int::state
  though they look like a bitmap, plugin may only
  be in one of those eigenstates, not in a superposition of them :)
  It's a bitmap, because it makes it easier to test
  "whether the state is one of those..."
*/
#define PLUGIN_IS_FREED         1U
#define PLUGIN_IS_DELETED       2U
#define PLUGIN_IS_UNINITIALIZED 4U
#define PLUGIN_IS_READY         8U
#define PLUGIN_IS_DYING         16U
#define PLUGIN_IS_DISABLED      32U

struct st_ptr_backup {
  void **ptr;
  void *value;
  void save(void **p) { ptr= p; value= *p; }
  void save(const char **p) { save((void**)p); }
  void restore() { *ptr= value; }
};

/* A handle for the dynamic library containing a plugin or plugins. */

struct st_plugin_dl
{
  LEX_CSTRING dl;
  void *handle;
  struct st_maria_plugin *plugins;
  st_ptr_backup *ptr_backup;
  uint nbackups;
  uint ref_count;            /* number of plugins loaded from the library */
  int mysqlversion;
  int mariaversion;
  bool   allocated;
};

/* A handle of a plugin */

struct st_plugin_int
{
  LEX_CSTRING name;
  struct st_maria_plugin *plugin;
  struct st_plugin_dl *plugin_dl;
  st_ptr_backup *ptr_backup;
  uint nbackups;
  uint state;
  uint ref_count;               /* number of threads using the plugin */
  uint locks_total;             /* how many times the plugin was locked */
  void *data;                   /* plugin type specific, e.g. handlerton */
  MEM_ROOT mem_root;            /* memory for dynamic plugin structures */
  sys_var *system_vars;         /* server variables for this plugin */
  enum enum_plugin_load_option load_option; /* OFF, ON, FORCE, F+PERMANENT */
};


extern mysql_mutex_t LOCK_plugin;

/*
  See intern_plugin_lock() for the explanation for the
  conditionally defined plMAM�j  �x���������������ت����������������������������������ȫ��������������������������������˻���������������ڨ��ʸۻ�������������Ⱥ�             И    ����    �w��    ���� � �x���� ����������w�� ����w�����������̰����  �̬����� ���
    �̀  �        ��'Bee�d�F/J�m��,f��"OE��Ƭ�������O��鍈C��6[g�,�mQ��!T���'���M���}C���N���=��������nU,ك���y��_�"C(�.�6r1и�M�G��)�r��v�1��e��G�՝�0��F��Z����iQ�qHeK��3�� G��;l:ִۚ3eR�\K�۸m�{�3=��%�aAZ�j�E�p��-<Bp���t;��.OѣʔO��J��jI���p�2��-U.?���8�A�ng��&@���?��]y��2�dv�]X�;�k��(�l�N�5��FE�������Xk1��0�#�m4�2e�o�a��H��^�n=[i�Р�[�:�&f�Ӡ!A�*�EnN�����y,IBG7��FK��3Tjq����*��T��"��&
�%��!O:��2j��N�b�}�͠�è0[%������6z�@�K���#��ou0h�ȁ�]�2Ǉ!��pY��AU;ț�AU՞�T���wڨ��dkܗX@0�S�}r��t?h�>��T�Tİa9�e\P� _��|�{J��X�Md	*Hk�Bm@�z�@ֿq���J�A-m�ܮS����nA%젖��Y��aj�ZP䏩<A�y�a-�*�w����A�`^>�p֠y��z
�~��w炉-��j@�I�����Ho.`}�W��T5o�P�
=��)�P��m���ܼ帖��ypA�����T5��NK=c����~���A���_� �5zAj�5���@9�@U_��T�O�T�������-hG�T�d+7���h�.��`�x�@�r�ʁ�{q�)E��f!� �=0:\C��U�V#�"�_AЬhY�~����`y2�A��da�'�z]Z^ǣ�8hg0[9�ս��{j��:�2�l��H�Ta�A�A�B����pA,�����I����j��b�����d�N�:h~�?P,���� n�� w�40Ae�8�g ā��D�p9T���_��#����$(�o	�bq1�
Je��]���;B��8� ׬���`A�V���Ȫ�Ǡ��Ă�0��(
HSP�[�N�]����@�V!�
�V�^ 5��Q ���)0P� 
dW��YYlhP;�j1���@�#$�̙�/L Q���0�)Pd `
hU����`se�BzP���!ŵL��ר��P�jj9P;Z�jAP�I�C-(a�%�l��`Cj�
��*l�֬��Z��j�
l��+�����h8 *�̀�����m�UY/��*[��R�Ѕ�[+��"L�zPr�
�j�� A�, �6�X܈�(�ҁi 
Ag��:��=@:�tT�b��	�A��q�-��q(�u���A��0޻�� n���zHe@6� 
Ag��ƃ����:h	p����ELh�ТD�m1-���T���jA�PN�����%��@l��h~�������Љ��hH`�(�����M�4�P�))�P� dh`�
�W�����/�0L�(
HzP{�{�w�ƞ����|�(^� �3S���@���`�+h`��k"2a�H$pH��>�*��8��=�����%�l��@4�|p��2��@.�	P:8$:`w���+����{��a(8�������|T��!��a{5h�h�쁀C�^�"Q�&��H�S�UA�k�SBZ��$W-,��n�=�@���ژʠ�6�nTc �or��	iq|��5�wY�������qAGy(wPժ�>�x��c@�X8�����I��;# [��C�@.D�������p
2V0����W�5Ə A}2`�p
1��A������&��$?z��QL߃HWb#���2������H� s0��w��0(� �9HE�;ĊhŁ+^r�?u( ���{��@�8� � -lJQ��@����A*���VH�� ��S\WF$p�����B �Qȕ�)
Np��0C4+``A����7`� �Q�d����U�� ��ҀB	���;�@p%L�(i@
�R���h�+^X *����x��F$I@	�Lx�`������CH3�T�*`L��V�+� �X*q �Ff���T�� �L34bJ��`41J�3 @�1-J�@[b*��A���P������Ƨe�@�	`p����1�H�:�:>BC]	ݾN�4�C������r>L�X��C|Hx ��ס����ľ�W"`s��6Gq���!�����V�h>,?������.����Lx�`�)(P� dh`�
U���p�(��`(�*
ZW��� ��F�4��I�P0��� )Nh`�
U�����!���z����@�ȩ+K�u��]�>hZ4�ÆE谡��lh���!��F�!�A1�Ȁ��&>�=X9��2|V�'񸃺7�K�h�����
T]�6�	��H$m���>$(�H��X�w	��0@ 7������2bz�GL��{|�}�R�����я6#@!��!|%�@�S���_p�(�Rk�+���2�#"�����B�?h1)ԓ��0��	@p�����@	�l�8�ֽF� #�i��DrA�A��LHx��5Vk�L��kV�Rz ���CÆ�U\�05���jq��M���@`�;��-,��EpTBQ�#�A���pa�$��0�*��&�A,}�����������!�A����n(7�]�7Ȅ�&��8P�3 �@�
h��ʪT���A2�6��#(\AGH4$$\�$J@&1��>��A����2b|�z~��<�rD5���r�=2�!]�q���~���5��"BnE��0�:L٣:�{ t��`� �`�����d���7po@� �B �5dbc���8��b��6F�E����FȊ �(�