/*
   Copyright (c) 2000, 2016, Oracle and/or its affiliates.
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

#ifndef SQL_CLASS_INCLUDED
#define SQL_CLASS_INCLUDED

/* Classes in mysql */

#include <atomic>
#include "dur_prop.h"
#include <waiting_threads.h>
#include "sql_const.h"
#include <mysql/plugin_audit.h>
#include "log.h"
#include "rpl_tblmap.h"
#include "mdl.h"
#include "field.h"                              // Create_field
#include "opt_trace_context.h"
#include "probes_mysql.h"
#include "sql_locale.h"     /* my_locale_st */
#include "sql_profile.h"    /* PROFILING */
#include "scheduler.h"      /* thd_scheduler */
#include "protocol.h"       /* Protocol_text, Protocol_binary */
#include "violite.h"        /* vio_is_connected */
#include "thr_lock.h"       /* thr_lock_type, THR_LOCK_DATA, THR_LOCK_INFO */
#include "thr_timer.h"
#include "thr_malloc.h"
#include "log_slow.h"       /* LOG_SLOW_DISABLE_... */
#include <my_tree.h>
#include "sql_digest_stream.h"            // sql_digest_state
#include <mysql/psi/mysql_stage.h>
#include <mysql/psi/mysql_statement.h>
#include <mysql/psi/mysql_idle.h>
#include <mysql/psi/mysql_table.h>
#include <mysql_com_server.h>
#include "session_tracker.h"
#include "backup.h"
#include "xa.h"
#include "ddl_log.h"                            /* DDL_LOG_STATE */

extern "C"
void set_thd_stage_info(void *thd,
                        const PSI_stage_info *new_stage,
                        PSI_stage_info *old_stage,
                        const char *calling_func,
                        const char *calling_file,
                        const unsigned int calling_line);

#define THD_STAGE_INFO(thd, stage) \
  (thd)->enter_stage(&stage, __func__, __FILE__, __LINE__)

#include "my_apc.h"
#include "rpl_gtid.h"

#include "wsrep_mysqld.h"
#ifdef WITH_WSREP
#include <inttypes.h>
/* wsrep-lib */
#include "wsrep_client_service.h"
#include "wsrep_client_state.h"
#include "wsrep_mutex.h"
#include "wsrep_condition_variable.h"

class Wsrep_applier_service;
#endif /* WITH_WSREP */

class Reprepare_observer;
class Relay_log_info;
struct rpl_group_info;
class Rpl_filter;
class Query_log_event;
class Load_log_event;
class Log_event_writer;
class sp_rcontext;
class sp_cache;
class Lex_input_stream;
class Parser_state;
class Rows_log_event;
class Sroutine_hash_entry;
class user_var_entry;
struct Trans_binlog_info;
class rpl_io_thread_info;
class rpl_sql_thread_info;
#ifdef HAVE_REPLICATION
struct Slave_info;
#endif

enum enum_ha_read_modes { RFIRST, RNEXT, RPREV, RLAST, RKEY, RNEXT_SAME };
enum enum_duplicates { DUP_ERROR, DUP_REPLACE, DUP_UPDATE };
enum enum_delay_key_write { DELAY_KEY_WRITE_NONE, DELAY_KEY_WRITE_ON,
			    DELAY_KEY_WRITE_ALL };
enum enum_slave_exec_mode { SLAVE_EXEC_MODE_STRICT,
                            SLAVE_EXEC_MODE_IDEMPOTENT,
                            SLAVE_EXEC_MODE_LAST_BIT };
enum enum_slave_run_triggers_for_rbr { SLAVE_RUN_TRIGGERS_FOR_RBR_NO,
                                       SLAVE_RUN_TRIGGERS_FOR_RBR_YES,
                                       SLAVE_RUN_TRIGGERS_FOR_RBR_LOGGING,
                                       SLAVE_RUN_TRIGGERS_FOR_RBR_ENFORCE};
enum enum_slave_type_conversions { SLAVE_TYPE_CONVERSIONS_ALL_LOSSY,
                                   SLAVE_TYPE_CONVERSIONS_ALL_NON_LOSSY};

/*
  MARK_COLUMNS_READ:  A column is goind to be read.
  MARK_COLUMNS_WRITE: A column is going to be written to.
  MARK_COLUMNS_READ:  A column is goind to be read.
                      A bit in read set is set to inform handler that the field
                      is to be read. If field list contains duplicates, then
                      thd->dup_field is set to point to the last found
                      duplicate.
  MARK_COLUMNS_WRITE: A column is going to be written to.
                      A bit is set in write set to inform handler that it needs
                      to update this field in write_row and update_row.
*/
enum enum_column_usage
{ COLUMNS_READ, COLUMNS_WRITE, MARK_COLUMNS_READ, MARK_COLUMNS_WRITE};

static inline bool should_mark_column(enum_column_usage column_usage)
{ return column_usage >= MARK_COLUMNS_READ; }

enum enum_filetype { FILETYPE_CSV, FILETYPE_XML };

enum enum_binlog_row_image {
  /** PKE in the before image and changed columns in the after image */
  BINLOG_ROW_IMAGE_MINIMAL= 0,
  /** Whenever possible, before and after image contain all columns except blobs. */
  BINLOG_ROW_IMAGE_NOBLOB= 1,
  /** All columns in both before and after image. */
  BINLOG_ROW_IMAGE_FULL= 2
};


/* Bits for different SQL modes modes (including ANSI mode) */
#define MODE_REAL_AS_FLOAT              (1ULL << 0)
#define MODE_PIPES_AS_CONCAT            (1ULL << 1)
#define MODE_ANSI_QUOTES                (1ULL << 2)
#define MODE_IGNORE_SPACE               (1ULL << 3)
#define MODE_IGNORE_BAD_TABLE_OPTIONS   (1ULL << 4)
#define MODE_ONLY_FULL_GROUP_BY         (1ULL << 5)
#define MODE_NO_UNSIGNED_SUBTRACTION    (1ULL << 6)
#define MODE_NO_DIR_IN_CREATE           (1ULL << 7)
#define MODE_POSTGRESQL                 (1ULL << 8)
#define MODE_ORACLE                     (1ULL << 9)
#define MODE_MSSQL                      (1ULL << 10)
#define MODE_DB2                        (1ULL << 11)
#define MODE_MAXDB                      (1ULL << 12)
#define MODE_NO_KEY_OPTIONS             (1ULL << 13)
#define MODE_NO_TABLE_OPTIONS           (1ULL << 14)
#define MODE_NO_FIELD_OPTIONS           (1ULL << 15)
#define MODE_MYSQL323                   (1ULL << 16)
#define MODE_MYSQL40                    (1ULL << 17)
#define MODE_ANSI                       (1ULL << 18)
#define MODE_NO_AUTO_VALUE_ON_ZERO      (1ULL << 19)
#define MODE_NO_BACKSLASH_ESCAPES       (1ULL << 20)
#define MODE_STRICT_TRANS_TABLES        (1ULL << 21)
#define MODE_STRICT_ALL_TABLES          (1ULL << 22)
#define MODE_NO_ZERO_IN_DATE            (1ULL << 23)
#define MODE_NO_ZERO_DATE               (1ULL << 24)
#define MODE_INVALID_DATES              (1ULL << 25)
#define MODE_ERROR_FOR_DIVISION_BY_ZERO (1ULL << 26)
#define MODE_TRADITIONAL                (1ULL << 27)
#define MODE_NO_AUTO_CREATE_USER        (1ULL << 28)
#define MODE_HIGH_NOT_PRECEDENCE        (1ULL << 29)
#define MODE_NO_ENGINE_SUBSTITUTION     (1ULL << 30)
#define MODE_PAD_CHAR_TO_FULL_LENGTH    (1ULL << 31)
/* SQL mode bits defined above are common for MariaDB and MySQL */
#define MODE_MASK_MYSQL_COMPATIBLE      0xFFFFFFFFULL
/* The following modes are specific to MariaDB */
#define MODE_EMPTY_STRING_IS_NULL       (1ULL << 32)
#define MODE_SIMULTANEOUS_ASSIGNMENT    (1ULL << 33)
#define MODE_TIME_ROUND_FRACTIONAL      (1ULL << 34)
/* The following modes are specific to MySQL */
#define MODE_MYSQL80_TIME_TRUNCATE_FRACTIONAL (1ULL << 32)


/* Bits for different old style modes */
#define OLD_MODE_NO_DUP_KEY_WARNINGS_WITH_IGNORE	(1 << 0)
#define OLD_MODE_NO_PROGRESS_INFO			(1 << 1)
#define OLD_MODE_ZERO_DATE_TIME_CAST                    (1 << 2)
#define OLD_MODE_UTF8_IS_UTF8MB3      (1 << 3)

extern char internal_table_name[2];
extern char empty_c_string[1];
extern MYSQL_PLUGIN_IMPORT const char **errmesg;

extern "C" LEX_STRING * thd_query_string (MYSQL_THD thd);
extern "C" unsigned long long thd_query_id(const MYSQL_THD thd);
extern "C" size_t thd_query_safe(MYSQL_THD thd, char *buf, size_t buflen);
extern "C" const char *thd_priv_user(MYSQL_THD thd,  size_t *length);
extern "C" const char *thd_priv_host(MYSQL_THD thd,  size_t *length);
extern "C" const char *thd_user_name(MYSQL_THD thd);
extern "C" const char *thd_client_host(MYSQL_THD thd);
extern "C" const char *thd_client_ip(MYSQL_THD thd);
extern "C" LEX_CSTRING *thd_current_db(MYSQL_THD thd);
extern "C" int thd_current_status(MYSQL_THD thd);
extern "C" enum enum_server_command thd_current_command(MYSQL_THD thd);

/**
  @class CSET_STRING
  @brief Character set armed LEX_STRING
*/
class CSET_STRING
{
private:
  LEX_STRING string;
  CHARSET_INFO *cs;
public:
  CSET_STRING() : cs(&my_charset_bin)
  {
    string.str= NULL;
    string.length= 0;
  }
  CSET_STRING(char *str_arg, size_t length_arg, CHARSET_INFO *cs_arg) :
  cs(cs_arg)
  {
    DBUG_ASSERT(cs_arg != NULL);
    string.str= str_arg;
    string.length= length_arg;
  }

  inline char *str() const { return string.str; }
  inline size_t length() const { return string.length; }
  CHARSET_INFO *charset() const { return cs; }

  friend LEX_STRING * thd_query_string (MYSQL_THD thd);
};


#define TC_HEURISTIC_RECOVER_COMMIT   1
#define TC_HEURISTIC_RECOVER_ROLLBACK 2
extern ulong tc_heuristic_recover;

typedef struct st_user_var_events
{
  user_var_entry *user_var_event;
  char *value;
  size_t length;
  Item_result type;
  uint charset_number;
  bool unsigned_flag;
} BINLOG_USER_VAR_EVENT;

/*
  The COPY_INFO structure is used by INSERT/REPLACE code.
  The schema of the row counting by the INSERT/INSERT ... ON DUPLICATE KEY
  UPDATE code:
    If a row is inserted then the copied variable is incremented.
    If a row is updated by the INSERT ... ON DUPLICATE KEY UPDATE and the
      new data differs from the old one then the copied and the updated
      variables are incremented.
    The touched variable is incremented if a row was touched by the update part
      of the INSERT ... ON DUPLICATE KEY UPDATE no matter whether the row
      was actually changed or not.
*/
typedef struct st_copy_info {
  ha_rows records;        /**< Number of processed records */
  ha_rows deleted;        /**< Number of deleted records */
  ha_rows updated;        /**< Number of updated records */
  ha_rows copied;         /**< Number of copied records */
  ha_rows accepted_rows;  /**< Number of accepted original rows
                             (same as number of rows in RETURNING) */
  ha_rows error_count;
  ha_rows touched; /* Number of touched records */
  enum enum_duplicates handle_duplicates;
  int escape_char, last_errno;
  bool ignore;
  /* for INSERT ... UPDATE */
  List<Item> *update_fields;
  List<Item> *update_values;
  /* for VIEW ... WITH CHECK OPTION */
  TABLE_LIST *view;
  TABLE_LIST *table_list;                       /* Normal table */
} COPY_INFO;


class Key_part_spec :public Sql_alloc {
public:
  LEX_CSTRING field_name;
  uint length;
  bool generated;
  Key_part_spec(const LEX_CSTRING *name, uint len, bool gen= false)
    : field_name(*name), length(len), generated(gen)
  {}
  bool operator==(const Key_part_spec& other) const;
  /**
    Construct a copy of this Key_part_spec. field_name is copied
    by-pointer as it is known to never change. At the same time
    'length' may be reset in mysql_prepare_create_table, and this
    is why we supply it with a copy.

    @return If out of memory, 0 is returned and an error is set in
    THD.
  */
  Key_part_spec *clone(MEM_ROOT *mem_root) const
  { return new (mem_root) Key_part_spec(*this); }
  bool check_key_for_blob(const class handler *file) const;
  bool check_key_length_for_blob() const;
  bool check_primary_key_for_blob(const class handler *file) const
  {
    return check_key_for_blob(file) || check_key_length_for_blob();
  }
  bool check_foreign_key_for_blob(const class handler *file) const
  {
    return check_key_for_blob(file) || check_key_length_for_blob();
  }
  bool init_multiple_key_for_blob(const class handler *file);
};


class Alter_drop :public Sql_alloc {
public:
  enum drop_type { KEY, COLUMN, FOREIGN_KEY, CHECK_CONSTRAINT, PERIOD };
  const char *name;
  enum drop_type type;
  bool drop_if_exists;
  Alter_drop(enum drop_type par_type,const char *par_name, bool par_exists)
    :name(par_name), type(par_type), drop_if_exists(par_exists)
  {
    DBUG_ASSERT(�   Ck  `H  �DRbʦu   pp  `  �DRb�FL   db  ``   �DRb��?   zo  `�  Rb�   Yo  `�  �DRb���   Xb  `�    DRbgE   Kl  `�  �DRb"�*�   Xk  `n  Rb&G#�   Yh  `2  �DRbB��M   tm  `  �DRbJ�у   xc  `�   RbN'�.   ie  `�  �DRbV��   Tc  `   RbZ�1�   Ei  `t  �DRbb��   cf  `  �DRbn�q8   Km  `(  �DRb�r~   yb  `�   Rb�G��   Vd  `�  �D ��`�  Rb��Î   ch  `�  Rb�'a<   dk  `  Rb�G��   Pi  `�  �DRb���   xl  `�  �DRb�g��   jo  `�  Rb�'�   zj  `�  Rb��+V   Ma  `>   Rb��H   Kd  `|  �DRb�'��   yi  `h  Rbʧ�   Vp  `H  Rbҧ��   sb  `x   Rb֧Z�   Di  `r  �DRb��ь   lm  `�  Rb���   $p  `Z  Rb��f   fj  `�  �DRb�g�U   pi  `V  �DRb
�y�   bd  `0  RbHX   gp  `�  RbhC   rj  `�  Rb(    Gp  `$  �DRbH��   el  `z  Rb"H��   ek  `  Rb"H�j   Qm  `4  �DRb.(��   Qe  `�  �D �`n  �DRbF�T�   nq  `j  Rb"h��   Lp  `*  �DRbR�t�   Sj  `  RbV�3"   Zk  `r  �DRb^(=J   fa  `
   �D �`�   Rbj�Mx   gk  `  �DRbr�֘   Yi  `�  Rbv�g   Fp  `4  DRb�z�   xn  `D  �DR�(��   pa  `   Rb�(�1   jj  `�  Rbֈ��   xo  `�  �DRbި�^   tb  `z   Rbި�$   Ud  `�  Rb�i   ck  `  Rb��!   jc  `�   �DRb���   sc  `�   Rbɲ�   Ch  `
  Rb��b   Hg  `�  �DRb)�^   Wn  `x  �DRbi&x   Xj  `  Rbi��   vm  `
  �DRb:IP�   yf  `0  �DRb^��   Jn  `\  �DRbj��|   sn  `:  Rbn���   Gi  `x  RbrI,�   fq  `^  �DRb�	XK   Qn  `l  �DRb����   wl  `�  �DRb�)��   qk  `.  �DRb��'@   Wg  `�  �D a�`f  Rb��   nh  `�  RbƉ(   Ci  `p  RbΉj&   Eo  `�  �DRbډe    Nm  `.  �DRb执   In  `Z  Rb�IK   ne  `�  �DRb򩻸   Fd  `r  �DRb�)�x   Fg  `�  �DRb
�&   pl  `�  �DRbjp�   Nf  `L  �DRb"
N�   ig  `|  Rb&��7   Mm  `,  Rb*
��   Ed  `p  Rb*Jt�   Nk  `\  Rb2*M�   Va  `N   Rb2
��   ui  ``  Rb6*I�   Uj  `  Rb:��,   Il  `�  RbB
+�   Vf  `\  RbF��   lb  `n   RbF��6   Ef  `8  RbJJ�R   bo  `�  RbR�5#   Zd  `�  RbVJtg   Sa  `J   RbZJs   Lg  `�  �DRbb�}:   Dd  `j  �DRbj
�   Bp  `  Rbn��   De  `�  �DRbv��F   Ek  `J  �DRb�j�K   ko  `�  Rb�j�   $i  `�  �DRb�J�[   fk  `  Rb�
�   kc  `�   �DRb�
,o   Bk  `D  �DRb���,   Ic  `
  �DRb��v�   Za  `T   Rb֊��   Gd  `t  Rb���c   Hk  `P  Rb�J�T   am  `�  Rb֊��   Al  `�  Rb�
Z   Kb  `�   Rb�*�   Hb  `�   Rb����   Jj  `�  Rb檤�   Vb  `�   Rb�* �   dd  `4  �DRb+Y   Xe  `�  �DRb"�    bm  `�  �DRb*+ q   Nd  `�  �D !�`�   RbN+8�   qd  `P  RbNK0�   op  `  �DRbf+*h   Jg  `�  �DRbnKo    yk  `>  Rbr���   tk  `4  �D ��`�   Rbz�v�   Ko  `�  �DRb�kDx   Sf  `T  �DRb�+KE   aj  `�  �D�`  �DRb�k�   gc  `�   Rb�K��   kh  `�  �DRbƫ�n   lq  `d  �DRb�KG   Of  `P  �DRb�kW   Om  `0  �DRbދ�   Ri  `�  �DRb�z�   Mf  `J  Rb�u[   Dn  `P  �D !�`d  Rb�n   se  `�  �DRb"��   ph  `�  Rb"�թ   Fi  `v  Rb*̈�   He  `�  Rb.�c   Sh  `&  �DRbB�L   Qg  `�  �DRbN^[   Pp  `<  �DRbbl�^   df  `  Rbf̰   jh  `�  Rj�el   no  `�  Rbn���   Yd  `�  Rbr��   Ta  `L   �DRbz̶�   Hi  `z   ��`�  Rb�lx�   ic  `�   �DRb��   Ki  `�  �DRb��-T   wn  `B  �DRb�,p�   wc  `�   Rb�LW�   Ln  ``  Rb���8   Jl  `�  Rbʬv�   wb  `�   Rb�y   xe  `�  �DRbތ��   gf  `  �DRb�L�{   Pd  `�  Rb��2�   yh  `   Rb���   Wl  `�  Rb�L�   Xl  `�  �DRb
-��   Gf  `<  Rb
͔�   sg  `�  �DRb
J   Xg  `�  �DRb-�_   Vh  `,  �DRb���   ip  `�  Rb*��   Eh  `  �DRb�l��   ao  `�  �DRb:�.�   Yb  `�   �DRb^�w~   sp  `  �DRbrR   Wb  `�   Rbr���   td  `V  Rbz-�L   fb  `b   Rbrmp�   ym  `  �DRb��<�   No  `�  �DRb��m+   Yk  `p  Rb�mCm   fo  `�  �DR��r   Gn  `V  �DRb�-�J   gm  `�  Rb�M��   $c  `,  Rb�m��   Qk  ``  Rb���   mp  `  Rb��,�   We  `�  �DRbҍb�   md  `F  �DRb�{�   Em  `  Rb�͜�   qp  `
  DRb�؂   eq  `X  �DRb��   zm  `  �DRb*N�1   $o  `�  Rb.���   yo  `�  �DRbF.��   Bn  `L  �DRbN���   uc  `�   RbN�
�   Le  `�  RbVΞ   bb  `\   �DRb^�_Z   Cf  `4  �DRb��,�   Ae  `�  �DRb�� �   vb  `~   �DRb�.H�   mi  `P  �DRb���i   ul  `�  �DRb�>�   ra  `   Rb�.��   ub  `|   Rb��c   Og  `�  Rb�N/�   $d  `�  �DRb�G9   hm  `�  �DRb��L�   lo  `�  �DRb/�D   bk  `  �DRb6O�;   vk  `8  �DRbNOP�   dh  `�  RbR��A   Bh  `  RbV���   xh  `�  �DRbR�ew   aq  `R  Rbb�9   Ie  `�  Rbbo�4   zf  `.  Rbf�W   Tf  `V  �DRb�o��   ee  `�  Rb�OF�   og  `�  �DRb�O�w   Uh  `*  �DRb�O��   cb  `^   �DRb���p   li  `N  �DRb��=   ff  `  �DRb��2   cm  `�  Rb�O�   Rb  `�   �DRb�\�   ah  `�  �DRb�O�U   Zi  `�  Rbޯ�,   Kh  `  Rb�-�   Pe  `�  Rb�ow   af  `  Rb����   Am  `  �DRb
���   dq  `r  �DRb�CK   Cm  `  Rb"P{:   ug  `�  �DRb*�@�   Bb  `�   Rb*pM   Pn  `j  �DRb>0��   Xc  `&  �DRbN0�h   Jo  `�  RbR0�   Lf  `H  �DRbfp�s   Td  `�  DRb�#   wo  `�  �DRb�0'   Bi  `n  �DRb��k   Cl  `�  �D �`D  Rb�0d�   Ah  `  Rb�Pv�   ll  `�  �DRb���   ae  `�  Rb�   pe  `�  Rb
Q

   Ej  `�  Rbq�   Oh  `  �DRb��   Ve  `�  Rbх�   mk  `&  �DRb&�%   Oj  `�  �DRb6Q��   Jd  `z  Rb:��   zl  `�  Rb>�\�   ke  `t  �DRbR���   mj  `�  �DRb^f]   Kk  `T  RbbѦ�   zi  `j  �DRbz��   Oa  `B   �DRb���x   xj  `�  Rb�Q   Md  `�  Rb�1�    Ab  `�   �DRb����   ii  `H   a�`�  Rb�q�G   rq  `t  Rb�Q�   xk  `<  Rb��N   Pl  `�  Rb��6   Zf  `d  Rb�a�   Yl  `�  �DRb�qYV   jg  `~  Rb�q�B   tn  `<  �DRbֱ�S   yj  `�  �DRb�1��   Ye  `�  �DRb�q�a   da  `   Rb��d�   uf  `$  �DRbr��   Xh  `0  �`*  �DRb��   dl  `�  �DRb2�   Zh  `4  Rb�2�   nc  `�   �DRb&r%   Gm  `    a�`�   Rb*��   Kf  `F  �DRb6Rн   vc  `�   �DRb>R��   up  `  RbB�   qa  `   RbF���   ki  `L  �DRbR2`i   Uc  `"  RbV��x   oa  `   RbZrIC   tg  `�  �DRbf2�   kl  `�  �DRbv�qB   Vl  `�  Rbz���   ya  `(   �DRb�rl�   wf  `(  �DRb���   Xo  `�  Rb�RW�   Vi  `�  Rb�2`�   sj  `�  Rb�Rȋ   Mk  `Z  Rb���   To  `�  Rb��.   hh  `�  �DRb��Yt   cg  `n  �DRb�R�   Gh  `  �DRb�r��   xd  `l  �DRb�2�   Hd  `v  Rb�2�~   Dl  `�  �D a�`�   Rb�@   Qc  `  RbY�   Zo  `�  Rb3�n   Pg  `�  Rb�_�   hd  `<  �DRb���   nl  `�  Rb*��'   Ll  `�  Rb.�e�   $a  `V   Rb2�^]   Fj  `�  Rb6�nu   eh  `�  �DRbN�L�   Th  `(  �DRbV�   zp  `   RbZs��   gd  `:  RbZ�   rd  `R  Rb^���   Un  `t  �DRbj3c&   qm  `   �DRbr��   Lc  `  Rbv3 u   Ng  `�  �DRb~3�g   Lm  `*  Rb���   fi  `@  �DRbr�q   Ao  `�  �DRb�3�|   Ca  `0   Rb���   ia  `   �DRb��͈   rk  `0  �DRb�3��   hk  `  �DRb�3WG   bh  `�  RbƓ�}   gl  `~  �DRb�S}   Af  `2  Rbֳ/n   vj  `�  Rb�dx   Ho  `�  �DRb�yy   oj  `�  Rb�Ӆv   qj  `�  Rb��o4   Wi  `�  Rb��c   tp  `  Rb��>   Yp  `L  Rb�SI�   So  `�  Rb��   jp  `�  DRb.t`�   Wf  `^  Rb2�a   Im  `$  Rb6�Ր   kb  `l   �DRbV�)   od  `J  RbV�$�   Lj  `�  �DRbv8�   rh  `�  �DRb~�Ż   El  `�  DRb�4�N   Yn  `�  �DRb�T��   Bd  `h  Rb�t�(   Hm  `"  �DRb����   Dp  `0  �DRb�԰   ik  `  �DRb�49<   po  `�  DRb*u�   Mb  `�   Rb.�p�   eg  `r  �`>  �DRb:���   Da  `2   �DRbBu�N   vo  `�  �DRbJU��   Tb  `�   RbJ�L   dc  `�   �DRbnu�S   le  `�  Rbr��   ro  `�  �DRbzВ   sa  `    Rbzu��   nb  `p   Rb�8�   je  `�  Rb���   gb  `d   Rb���W   zg  `�  Rb����   Pj  `�  RbzUP_   Yg  `�  Rb����   Vj  `�  Rb�5��   Fn  `R  Rb�5��   Vo  `�  RbzUv   Kn  `^  Rb��i�   dj  `�  Rb����   zb  `�   �DRb��4   Vc  `  �DRb��v   im  `�  �DRb��aP   Dj  `�  Rbڕb   fm  `�  �DRb�U�U   ci  `B  �DRb�U+<   wd  `\  �DRb�Ō   pq  `n  �DRb& s   Ad  `d  �DRb:��o   mf  `  Rb:�`�   vh  `�  �DRbNv�   Mi  `�  RbN7   sl  `�  �D �`�   Rb^�em   pb  `t   Rb^���   Fl  `�  RbZv�O   ve  `�  �DRbnC�   Cc  `  RbZ�   ej  `�  �DRbb�&   mm  `�  �DRb�6��   Nb  `�   Rb���   hf  `  Rb���`   Dg  `�  �DRb�V�   Ni  `�  R��0�   La  `<   Rb��O   Fb  `�   Rb�6��   Je  `�  �DRb�vN{   Rn  `n  �DRb���   hq  ``  �DRb�֍�   pc  `�   �DRb�v�o   gi  `D  �DRb�v?I   wh  `�  �DRb����   Ga  `6   Rb��3   Aj  `�   DRb&7%(   qf  `  Rb*�ʛ   Re  `�  �DRb&��   Io  `�  �D !�`�   �DRbJ�[   Ul  `�  �DRbR�4�   Ij  `�  �DRb^7��   Sp  `B  �DRbr��-   hb  `f   Rbr�ƛ   Rg  `�  Rbz��W   Gk  `N  �DRb��R   qh  `�  Rb�7@�   jl  `�  �DRb��/   mg  `�  Rb���V   $g  `�  �DRb��1   ni  `R  �DRb��L�   Wk  `l  �DRb�׃'   Tj  `
  Rb�T�   bf  `  �DRb�wY�   Sd  `�  Rbʷ�   Rl  `�  �DRb��X�   Mo  `�  �DRb�w<~   Sm  `8  Rb�W�   ti  `^  Rb�W�   Fe  `�  Rb�Ws[   ac  `�   Rb���   Jm  `&  Rb��~   om  `�  �DRbXZ   Zc  `*  Rb8{@   me  `�  �DRb�f�   bq  `T  Rb�|�   bp  `�  Rb���   pk  `,  (DRbV8#5   la  `   RbZة�   ab  `Z   RbZ��e   Bo  `�  �DRbn8�s   Ik  `R  �DRb~XP�   sf  `   �DRb�x+
   Lb  `�   Rb���   Ai  `l  Rb�� �   kd  `@  Rb�X��   Wd  `�  �DRb�X4   sm  `  �DRb�xE$   Ac  `�   �D a�`    RbҸ�=   Ea  `4   Rb���   Mn  `b  RbΘH�   rb  `v   Rb��   uk  `6  Rb��^   ua  `$   Rb�8�   Dc  `  �DRb�A�   $h  `6  Rb���   Be  `�  Rb��G   $e  `   Rb����   Ge  `�  �DRb
�    Xd  `�  �DRbΘ��   uj  `�  Rb&��   xg  `�  �DRb.�9�   kp  `�  �DRb:�	H   Nc  `  Rb>��   jk  `   �DRbFy   Ji  `~  RbJ�H�   Kc  `  �DRbV���   hl  `�  �DRbb�B   Pc  `  �DRbj�*�   Sg  `�  �DRbr��,   Rj  `�  DRb��w�   de  `�  �DRb��b�   If  `B  �DRb��ܪ   Zb  `�   Rb���   Pf  `Z  Rb���%   dg  `p  Rb���   Po  `�  �DRbʙd�   oc  `�   �DRb��J�   Nl  `�  Rb���   ea  `   Rb�٫�   pj  `�  Rb޹w|   oo  `�  �DRb�Y[   Zj  `  Rbں�   yl  `�  �DRb
��   ri  `Z  Rb
z�q   Yj  `  Rb��   fc  `�   Rb: �   zh  `  RbZ@�   Cp  `"  �DRb
�ژ   em  `�  Rb��kI   On  `h  Rb*�   cj  `�   ��`N  Rb2zH�   np  `  �DRb>���   ql  `�  �DRbV�/   ha  `   RbZ��#   Qd  `�  �DRbbzQM   xm  `  �DRbn:��   Kp  `,  �DRb��.�   rp  `  Rb�:b�   ta  `"   Rb�:QJ   ba  `    �`.   Rb��P�   Ra  `H   Rb��2�   Ql  `�  �DRb�zL�   Li  `�  Rb�H�   ze  `�  �DRb�z�j   be  `�  Rb��h   Nn  `f  �D �`�  Rb�ڝ-   Rc  `  �DRb�Z��   ji  `J  Rb��(-   ap  `�  �DR���   bg  `l  �DRb[)�   Tp  `D  �DRb*{�   Dh  `  �DRb:�mx   cp  `�  �DRbB[\�   yp  `  �DRbJ��D   Qj  `�  RbN;{+   wk  `:  �DRbf[{5   so  `�  �DRbn���   Bm  `  Rbr�K   Yc  `(  �DRb�[��   wj  `�  Rb��   Sk  `d  �DRb�[��   za  `*   �DRb�;Љ   Gj  `�  Rb�{A,   Hl  `�  Rb�{�E   sd  `T  �DRb�{K,   Hh  `  Rb��K�   fp  `�  �D !�`@  �DRb�[^�   Do  `�  �DRb�8P   Sb  `�   �DRb�Y�   Ne  `�  Rb���/   ho  `�  �DRb�{.�   ng  `�  Rb�;1   Cj  `�  �DRb<1   Ba  `,   �DRb֦   lp  `   �DRb��$   Qo  `�  �DRb"<4�   nf  `,  �DRb2��   rm  `  �DRb:|�   mo  `�  �DRbB<�0   ud  `X  RbB�˖   di  `:  RbJ<�h   Wp  `P  RbN|T�   sk  `2  �DRbf�`�   Uk  `h  Rbj�sy   ih  `�  Rbn�y   $b  `�   Rbf<��   um  `  �DRb�<r�   Bg  `�  Rb����   vn  `@  �DRb��'8   Bl  `�  �DRb�\P�   lc  `�   Rb���2   Ka  `:   �DRbƼ�J   Gg  `�  Rb�|X�   mh  `�  �DRb���E   Ih  `  �DRb�<�   Tg  `�  �DRb�ܡ   ef  `
  �DRb�?u   kf  `  �DRb�   Xf  ``  Rb	>   ag  `j  Rb�!f   Ak  `B  �DRb&]��   Rp  `@  Rb*�]   Zp  `N  �D �`�   Rb2� �   oq  `l  �DRb>�$   oh  `�  �DRb^��{   zc  `�   DRb��Ù   cc  `�   Rb�=�7   lj  `�  Rb�]��   An  `J  Rb�}2�   Jb  `�   Rb��	4   Ph  `   �DRb�]F�   Jk  `V  �DRb�}��   te  `�  Rb��   zn  `H  �DRbνˉ   cl  `x  �DRb�]xr   Xa  `P   Rbޝi~   ai  `8  �DRb�]�   Kg  `�  Rb��>�   Sn  `p  �DRb����   $n  `|  �DRb��ِ   hj  `�  Rb�}�^   oe  `�  �DRb�}ml   bl  `v  �DRb~��   Ep  `2  �DRb2�,�   Oo  `�  Rb6�)(   Vn  `v  Rb:>ja   Si  `�  �DRbR�J�   io  `�  �DRbj��2   Fo  `�  �DRb��==   Lo  `�  Rb����   Pa  `D   �DRb�>�   Me  `�  �DRb��h3   gj  `�  �DRb�x�   tl  `�  Rb�^ܝ   Hj  `�  �DRb��   xp  `  �DRb�^(�   pg  `�  Rb�^��   bc  `�   �DRb��   tc  `�   �DRb���   fg  `v  �DRb��R�   ec  `�   Rb��©   Wo  `�  �DRb�4�   ye  `�  Rb�C�   Fh  `  Rb
L�   sq  `x  Rb_(�   km  `�  �DRb_�   Cg  `�  �DRb�~�   mq  `\  Rb"�	}   jb  `j   Rb&�Q�   pm  `�  Rb*�*�   Qa  `F    �`�   �DRb:��_   xf  `*  �DRbZ?%�   Np  `8  �DRbn��   Ue  `�  �DRbv��5   fe  `�  Rbv_��   pf  `  �DRb�_>�   Hf  `>  Rb�_�7   sh  `�  �DRb���   wm  `  Rb���;   Go  `�  �DRb��k|   qc  `�   Rb���z   yn  `F  Rb��l�   Qf  `N  �DRb��o�   Ec  `  Rb�O�   Df  `6  �DRb�*�   Ub  `�   Rb�_b�   bj  `�  Rb���   Qh  `$  �DRb�x�   mc  `�   ��� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� I`����Da   � ,TX�`n   L`   ���y�Dp(          -  ��^����-m���//�	����PĊ �� (Sb��        �
`����Da8  �  �d       P     H�
 lQhz��n]   https://pagead2.googlesyndication.com/pagead/js/adsbygoogle.js?client=ca-pub-4830067276561639   a        Db       j  D`    Y=`�   �,T�
Ia  N  ��e     @    ^1      ,T�IaY  �  ��e     @    _1      ,T@�`:   L`   �e�Dj          V� ��k� �U�
��k��   (Sb��        I
`����DaZ  �  ��b       1  �e    @    `1      ,T�
Ia�  @  ��e     @    a1      ,T�Ia�  �  ���e     @    b1      ,T�
Ia�  �   �f       �� @�� @ 
    �	��e     @    c1      �D,T�Ia�  R  ���e     @    d1      ,T�Ia_  �  �d
       �	�	 @ �  ���e     @    e1      D,T�
Ia�  �  M�e     @    f1      ,T$�`   ]�Dc           � (Sb��        �`����Da�  �  ��e    @    g1      ,T�Ia  *  ��e     @    h1      D,T8�`.   L`   ,RiN���   ^[\s\xa0]*([\s\S]*?)[\s\xa0]*$  9�Dh          y    �-��^���/�� (Sb��        u`����Dal  �  �c       �     �e   @    i1      ,T�
Iay  6  �
��e     @    j1      ,T�IaA  ]  �	��e     @    k1      ,T8�`0   L`   Rd�U�	   navigator   RdJr�y	   userAgent   I�Dh          �-�  Ř-�Ř���(Sb��         ]`����Da\  �  �b         �e    @    l1      ,T�
Ia+	  s	  %��e     @    m1      D,T<�`2   L`   �Di          ���a� �-� �^��k�U�   (Sb��         �`����Da�  6  ��c       @    �e  
 @    n1      ,T@�`<   L`   Rc���   brands  y�Dj          �x���-�  �-�m���  (Sb��         �`����DaN  �  ��b       �e     @    o1      ,TD�`D   L`   Rc��[f   Trident RbF��7   MSIE�Dk          �a� ��� �b�����b���  (Sb��         ]`����Da�  
  �b       @ �e     @    p1      ,T�
Ia
  >
  �	��e      @    q1      ,T��`�   L`   RcZ�f   Safari  RcF*w   Coast   Rc�+�   Opera   Rb��g�   Edge�D�          � �b�� T�i�a��a�a�����b���J�a�����b��
�3�a�����b����a���a����b���  (Sb��         Rb>`�   Fa  `����Da�  l  ��e       @ @ @   �e     @    r1      ,Tp�`�   L`
   Rc�+��   ChromiumRc>��   Chrome  Rc6��G   CriOS   �Rb*�<�   Silk�Dv          �a� �� �b���<��b�����b����a�����b��
T���b��� (Sb��         !	`����Da�  2  ��d       @ @    �e     @    s1      ,T�Ia$  s  ���e     @    t1      �D,T�Ia�    m�e      @ "   u1      ,Tx�`�   L`   y��Dx(           �& ��-  k����-��_����- m�	�"r�
��/�l�����PŊ% ��(Sb��        	`����Da  H  ��d       ���� Dx�e    @ #   v1      ,Tt�`�   L`   ���I���DwH          -  � �-��^������m��-�r��-	���/������\���PÊ. � (Sb��        =`����Da^  <  ��d       @ 1�� x�e   @ $   w1      ,T�
Ia)  �  5�e     @ %   x1      ,T��`�   L`
   y���I���D{`          -  �!�b��� �-��^������m�
�4�r��$-���/������\���4����P5 ��(Sb��        �`����Da�  �   ��e       @ � �x�e   @ &   y1      ,T�
Iao  �  	�e     @ '   z1      ,T��`  L`
   y��I�����D�@          -  � �-��^�����Q���o��6�r��&-
���/������\���	���QŊ9 ��n��� �-�^��/� (Sb��        Y`����Da�!  v#  � �f       @ 0&P  /�     �e   @ (   {1      ,T0�`   ]�Df          � �c� o��  (Sb��        �
`����Da�#  �#  ��b       D  �e    @ )   |1      ,T�
Ia�  I  ]�e     @ *   }1      ,T0�`    L`    ��Df          (�-�  �^���(Sb��        -`����Da�$  �$  ��b         �e    @ +   ~1      ,T$�`   ]�Dc           �  (Sb��         I`����Qb      Ta. a�$  %  ��e     @ ,   /3      ,T�
Ia�  �  �d
       �%�% @    ��e     @ -   1      D,T�Ia  :  �d       �&�' @ 5P ��e     @ /   �1      D,T�
IaG  7  ���e      @ 1   �1      ,T<�`2   L`   �}�����Di          !  �-��-��-��^���   (Sb��        �`����Da�*  �*  �c
       P   �e   @ 2   �1      ,Td�`�   L`    !��A۔Ds           .� /J  ���Ll�U�*!�-��^��	�-�b�.��J�4��� (Sb��        �	`����Da+  �+  ��d       �  :  �e    @ 3   �1      ,TH�`N   L`   �A� !��Dl          !  �-��^���-�b�.��4�� (Sb��        �`����Da�+  ,  ��c
       @ 8  �e   @ 4   �1      ,T4�`$   L`    !��Dg          .� �/ J4��  (Sb��        I`����Da4,  ^,  �b       @ �e   @ 5   �1      ,T4�`$   L`    !��Dg          .� �/ J 4��  (Sb��        �`����Dat,  �,  ��b       @ �e   @ 6   �1      ,T�
Ia[  q  ���e     @ 7   �1      ,T�Ia|  �  ���e     @ 8   �1      ,TD�`B   ]�Dk          N L�  �l��
    ��   (Sb��        A	`����Da<-  �-  �b       F  �e   @ 9   �1      ,TL�`R   L`   �!�]��Dm          U�$ �!  �-��^��T�-�!l�
�   (Sb��        Q`����Da�-  d.  ��c       @ �e   @ :   �1      ,T4�`"   L`   �Dg          L �
! �a���   (Sb��        y`����Da�.  /  �b       �  �e   @ ;   �1      ,T�
Ia�    ��e     @ <   �1      ,T�Ia*  T  ���e      @ =   �1      ,T�Ia_  �  �	��e      @ >   �1      ,TX�`p   L`   (Rh�t�   Expected boolean but got    Rb�
(�   :   �Dp          �2 �'!  ���b�w8��8��w8��b���x�(Sb��        M`����Da.1  
2  �c       @    �e    @ ?   �1      ,TL�`T   L`   �!�Dm            �!  �-��^���x� �
!�b��  (Sb��        `����Dat2  &3  ��c
       @   �e    @ @   �1      ,T@�`>   L`   �!�Dj          U�!  �-��^���<�a�� (Sb��        �`����Da<3  �3  ��c       @    �e   @ A   �1      ,T�
Ia�  �  M�e     @ B   �1      ,TH�`L   L`   �!��Dl            �	=�a� �! �-��^���<�a��  (Sb��        `����Da�3  t4  ��c
       P   �e   @ C   �1      ,T4�`"   ]�Dg          ��	C�b� �   (Sb��        �`����Da�4  �4  ��b          �e   @ D   �1      ,TH�`J   ]�Dl           �� ���t   ���   (Sb��        �`����Da�4  �5  ��b          �e   @ E   �1      ,TH�`J   ]�Dl           �� ���t   ���   (Sb��        �`����Da�5  x6  �b          �e   @ F   �1      ,T8�`,   L`   �Dh          � �
!  �a���  (Sb��        �`����Da�6  7  ��b         �e   @ G   �1      ,T4�`"   ]�Dg           � ���   (Sb��        H`����Da7  |7  ��e   @ H   �1      ,T��`�   L`   �!��		 !� !��D�   (       ��( �"!  �-��^��ŗ-�6l�����^�TL	�BJ�/
�	�.i� -.��/J"4�J�4��i� ��.�/J ��l��L >����L >����l�!U�.��4�"�i$�  (Sb��        �`����Da�7  �9  (�h&       @ D� � � "!��    �e   @ I   �1      ,T8�`.   L`   `�Dh           K� %Ki %K� (Sb��        �`����Da�9   :  ��b          �e   @ J   �1      ,T(�`   ]�Dd          N�b� �(Sb��        �`����Da8:  ^:  ��b          �e   @ K   �1      ,T�`�  @L`   �5�!���!�)	Iy1���YRb~_   btoa�D�P          V� l� �l� �"l� �%��!�b���
!�b�������!	�-��^������!q����-	 E ( ��m��;!�-�
�-���-�� D ( ���_����_���� 8���Ŋ< "!�-�
�-�$������-�^��&��_����(8�#�!*�b��,��   (Sb��        �`����Dat:  .=  ,�i.       � � 
� *���� p!@   �e   @ L   �1      ,T�
Ia�  q  ���e     @ M   �1      ,T�`�  (L`   �!�y !��}�����D��   8       ��!  �-��^���Z��-k��.�/J 
L	��5�.�/J L��Q�U������`���|7�b��l~��_v���������Q�����C��!�-��-��-��_����&P��/ ������`��"�4��$��P &��	c'��(Sb��        �	`����Da�>  A  (�h)       @ D B� j P  @ \ �e   @ N   �1      ,T��`�   L`    !�y�D~X   8       ��.� /J  ���
�L x�-�b��-m��,P��/	������`��4��PĊ/ �c�� (Sb��        U`����DaA  (B  ��e       H D�x	    �e   @ O   �1      ,T�
Ia!  �!  ���e     @ P   �1      ,T@�`>   L`   �		�Dj          -  �6l��-�]��	N�b�� (Sb��        Y`����Da8C  �C  �c	       ��   �e   @ Q   �1      ,T�
a�!  "  I��e      K� R   03      ,T�Ia("  3#  e��e     @ S   �1      ,T�Ia>#  �#  ���e     @ T   �1      D,TX�`j   L`    !� !�]�Dp@          -  �.�/��L�"L�-�V������`���c���	��   (Sb��        �`����DaLG  �G  �c         �  �e   @ V   �1      ,T@�`:   L`    !� !��Dj           -  Y�.�/���`���   (Sb��         a�`����Da�G  $H  ��b        @ �e  
 @ W   �1      ,T��`   L`   y�D�   (       ��l� ��5�b�p� L  �- E/�/�
��K- ��" L  ��E/�/��� L  xtE8�m�/��(Sb��        �`����Da:H  �I  � �f       ��! � "� B    �e   @ X   �1      ,TL�`V   L`    !� !��Dm8           -  �.�/��:�b��[�������`��� (Sb��         �`����Da�I  "J  ��c        @    �e  
 @ Y   �1      ,TԐ`b  L`   � !��D�    0       5�b� ��p��e L  �- E/ŋ.�� L  xtE
8�	�~�4��� J  4�lU�.��4��; L  xtE8�4� L  �- E/r�W�   (Sb�� 
        �`����Da:J  �K  �$�g!       D !�� �!�   �e  
 @ Z   �1      ,T�
Ia�%  $&  ���e     @ [   �1      ,T��`  L`   �!� !��D�(           L �Y����`��! �-��^���9.�/
J 	L�	1�b���L�.�/J"�4���$L �LT��.�/�L��4���� (Sb��        1
`����Da^L  fM   �f       � 
� � � � � :  �e   @ \   �1      ,TD�`H   ]�Dk          X�c� �� �  �x��(Sb��        U`����Da�M  *N  ��b          �e   @ ]   �1      ,T=�`8   L`    !� !��A�y���D)`           -  .�/�L�]�����`���./�J �L
��!�-��^���� �-�b���[�������`����-�m��*�/��b��U��u�P��4����P��-  �m�!�
�2�"��J$����J"%�.��4��&�L(�!�-�)�^���+�T�D�L-�!�-��^���.�--�b��0�.�/2J44�5[������`��7��(Sb��        �	`����Da@N  �P  4�k9         	 " � ��"  ޸ : � H   �   �e   @ ^   �1      ,T��`.  L`
    !� !��A۔�D�@           -  �.�/��:�b���[������`���j./
J 	L�I!�-��^���-�b��-m��C��/�b��4��PÊ .�4�[�������`��!� (Sb��        %`����Da�P  4R  �$�g#        @ @� @ �  � �e   @ _   �1      ,Tp�`�   L`    !� !�y	��Dv@   (       -  �.�/��:�b��[��l��������!�b�	l��l���`���  (Sb��         !�`����DaHR   S  ��d        @ � "�   �e  
 @ `   �1      ,T��`�   L`    !� !��Dz8   (       -  �.�/��:�b��e������`���!lU�U�[������`��	[�������`��� (Sb��        �`����DaS  �S  �d        @ D    �e   @ a   �1      ,TL�`V   L`    !� !��Dm            -  e�.�/���`���l����� (Sb��        1`����Da�S  @T  ��c        @    �e   @ b   �1      ,T@�`:   L`    !� !��Dj           -  e�.�/���`���   (Sb��        e
`����DaVT  �T  ��b        @ �e   @ c   �1      ,Tt�`�   L`   y�Dw8           ��-  m��F�/�Y�����`��U�"��l�U�[�����`������P
ĊI ��(Sb��        i`����Da�T  �U  �c       D  /�e   @ d   �1      ,T�
Ia�*  2+  ���e     @ e   �1      ,T��`�   L`    !� !��Dz@   (       -  .�/�Y������`���I�������`���lU�U�[�������`��	�(Sb��        �`����DazV  :W  ��c        @ D �e   @ f   �1      ,T|�`�   L`    !� !��Dy@           g�����`�� ��- .�/�L�+W�b��l�	U��[��������`��
�  (Sb��         a�`����DaNW  DX  ��c       � ��H �e  
 @ g   �1      ,T��`�  $L`    !� !��A���y`F  �D��           -  �.�/��Lx����Lx�]������`���9l���./�J 
L	����l�U����<��l��0./�J �-�b���.��4��[��������`���r!�-��^������l����!�-��^���!�E./�$J #�L�&�� -�b��'��[��������`��)l�+U�.�4��,�����L.x�./�1J 0L/x����T���
-�b��3�����>�5������-�6m�8�HI��/�9�����`��;�U�$��-� >�./�@L=��uB�PB��4��C��PE��K F�m�G�
�2�H��./KJ J�JM���	�L�N��JO��k�PU�0�c��Q�l�SU�[��������`��T����l�VU�
��l�W�!�-��^���X��P./\J [LZ�C-^m`�*/a�W�b��c��l�eU�
�4f�Ph�- i.�/jJl4�m� (Sb��         ��`����Da\X  �]  X�to         !D F� 8@ @ �  @ : ! D � �x#@ F"�H "P "�� �  �e  
 @ h   �1      ,T<�`4   ]�Di            �Z����`�� �  (Sb��        5`����Da�]  ^  ��b          �e    @ i   �1      ,T�
Ia/  B/  ���e     @ j   �1      ,T��`B  L`    !� !���D�P           -  �.�/��:�b���k-x��-m��'�/	���-� �./�J LT��P* ./J �J���	�J��L�J��k�U�0�c���[�������`���   (Sb��        �`����Da�^  F`  $�g        @ D �  " !F    �e    @ k   �1      ,T4�`"   ]�Dg          E�X�c� �b���   (Sb��        i`����Da\`  �`  ��b         �e   @ l   �1      ,TL�`X   ]�Dm          X�c� ��@�b��  ����(Sb��        �`����Da�`  4a  ��b         �e    @ m   �1      ,T4�`"   ]�Dg          H�X�c� �b���   (Sb��         �`����DaHa  xa  ��b         �e  
 @ n   �1      ,T�
Ia�0  �0  -��e     @ o   �1      ,T(�`   ]�Dd           ���(Sb��        A
`����Da�a  �a  �e   @ p   �1      ,T@�`>   ]�Dj            �����^�c��� ���� (Sb��         a�`����Da b  :b  ��b          �e  
 @ q   �1      ,T4�`"   ]�Dg          q�m�c� �b���   (Sb��        	`����DaTb  �b  ��b         �e   @ r   �1      ,T��`�    L`    !� !���Q��D�@          -  .�/�Y�����`������1�  � ��l����l����l��!	�b����U��l�U�[�������`������ (Sb��        �`����Da�b  �c  �d        @ 1F   �e    @ s   �1      ,T0�`   L`   I�Df          o�c� �� � (Sb��         ��`����Da
d  :d  ��b          �e  
 @ t   �1      ,T0�`   ]�Df          X�c� ���  (Sb��         �`����DaNd  |d  ��b          �e  
 @ u   �1      ,TD�`B   ]�Dk8   (       h�c����`�� ���`���   (Sb��        �`����Da�d  �d  ��b         �e    @ v   �1      ,T�
Iax2  "3  ���e     @ w   �1      ,T�Ia-3  �3  )��e     @ x   �1      ,T�Ia�3  �3   !���e  
   @ y   �1      ,TI�`J  $L`   `�!� !�y !��D5P          �K� �K� �%K�`�| ��!�-��^���
!�a�	�.�/J �L@�����J@�-Ø��E��/��7�b����� J  � L  xtE��9����  ��o��[ ���8��-������m��+�/��U��l�!U��9�"��4��#��P%��, &�D'2�(�4��* ���L���-   �� L�/ M.>�,�.���4�022�   (Sb��         a�`���� a�ah   k  0�j4       Ӏ�  @X  B!� ^� @!,  �e   ��z   13      ,Th�`�   L`   ) !��Dt@          8� �{�- ���`��ŋ,Q�-�S�����`���{�����`�����(Sb���         	`����Da,k  �k  ��c
       P   �e    �� {   23      ,T9�`0  DL`   ] �� !� !�y�a���`  }�����!����D%�           -  �-��5��-��.�/��b�������n-�b�
-�\7�-E/�b���G�-�m��<�/���p��%!�-��-E�~�4�_���� ��P"Ê? #��T-�./�$�5�b��&� L ) xt*E(��-�m�+���/�,�m�.�N�8�/����/�0��&����9�1�|2�b��3�4��5��9l�7U�
/�b��8�}��3�-��7�-�E:/�;�b��=�	����-�?�~�^���A�/�C��/�E�&����9�1�|G�b��H�4��J��9l�LU�
/�b��M�PO�� P-ŗ���7��EQ/R�b��T����~����vv���V���g����V�X��!�-�	W�-�
Y�-�[�_����]�6�/�_!a�-�c�^��e�kgU���4��h�Ħ��f j��/�)v���k�������k���������� l��k�mU���Qn���m�o��Ep/q�����QsŊ t�	����	��(!a�-�	W�-�u�-�w�����\��y����
�2{��-?�^��}�(Sb��        �`����Da:l  s  d�w       P @ @��� р  / !0"!L  �� &8	 ^� �
� �� :p)7� �
��    �e   @ |   �1      ,T�
Ia�9  �:  ���e     @ }   �1      ,TX�`p   L`
   A a�I��Dp           � �%b �*��a��8��!�!�b�
8�b����(Sb��        9`����Da0u  �u  �d       @ @    �e    @ ~   �1      ,T,�`   ]�De           U�U� (Sb��        `�����a�u  v  ��e     K�    33      ,T0�`   L`   0Sb��        !�`$   Rb�գe   yc  `����Da@v  >w  �,T��`�    L`   Ib/��!��D{          � �k� ��i�� �F!�-��^��!	�-��^���!��b����L���2�b��c����  (Sb��        I`����DaVv  <w  ��e       � 
� 
� �  �e    K� �   `5      �Df          � �%� �  ��a       �e    @ �   �1      �,T�
�a�;  �;  ��e       E� �   43      ,T��	a�;  �;  ���e       E� �   53      ,T�Ia�;  <  	��e     @ �   �1      ,T�
Ia*<  `<  �d	       �x�x @    ���e     @ �   �1      D,T<�`4   L`   @Sb��        !� a� ��b$   �� �� Rb��f   Gc  `����Da�x  Ly  �,T@�`:   L`    a� ���Dj          � ��a� ���%%��   (Sb��         I`����Day  Jy  !�b          �e    @ �   _5      �Di          � �%%%%%� �  ��a       �e    @ �   �1      ),T�
Ia�<  �<  �d
       �y�y @    ��e     @ �   �1      D,T@�`:   L`   ReR�u�   addEventListener�Dj(           -  �-  �����\���   (Sb��        �`����Da�y  ^z  ��b         �e    @ �   �1      ,T�
Ia:=  �=  ���e     @ �   �1      ,T�Ia�=  �=  
�e     @ �   �1      ,Td�`�   L`   ���Dsp          �?v����� �����1���� �#��- ���/�����\������0 � (Sb��        �`����Da�{  `|  ��c       � �  �e    @ �   �1      ,T\�`v   ]�Dq`          | ���1v���������#��������u�P��/4����" �� (Sb��        Rb��4�   Mc  `����Dav|  �|  ��c       �B�  �e   @ �   �1      ,TP�``   ]�DnH          ~��)v����� �������� ����/4���� ��(Sb��        	
`����Da}  l}  ��b       .�e    @ �   �1      ,T(�`   L`    ᪐Dd           2  �(Sb��        
`�����a�}  �}  ��b          �e    ���   63      ,T�
Ia�>  �>  	�e      �� �   73      ,T��`�   L`
   9�I �� ���D{8          ��-�  ���b��-��]���^���/
����/����/���`��8�����c���8��b���(Sb��        �`����Da~  �~  ��e       P �  @ � H �e    @ �   �1      ,T<�`8   L`   ] �,Ri�=2�   type_error:TrustedResourceUrl   �Di          �q �- ��l��-��(Sb��        �`����Da�~  �  �b       ��X �e    @ �   �1      ,T��`J  4L`   Rd8��   trustedTypesRddϔ   createPolicyR2��<	   goog#html   $�a      RdB+N
   createHTML  CRdjl�   createScriptCRe2Y/�   createScriptURL CSbJ �         �`�� �y�rIKd       ?   	     D�8          ��}��-�  Ęm-��g��-���})�3�3�3�	_����ŋ6������ �-��� �-��-�	��-�
�^�����%���%��Ř-��^���������i���   (Sb��        `����Da�  (�   �f       0���
���   �e    @ �   �1      ,T�`�  <L`   BpI�}�����!�]
`    La       y a� !���Dp           �� ��! �b�8 ����v������������������!�-��-�
�-��_������/�!�-��^������z%���6�����-�	m��a�/��U�N�-	�-	!n�#�
��! �b��$8�&�8�'�! �!(�b��*�b��,8�.8� �P/Êd0��� 1�  (Sb��        I`����DaB�  �  0�j2       � (P � @ �"�"@h  !�  �e    @ �   �1      ,T�
�a�B  �B  ���e     �� �   83      ,T�Ia�B  �B  	��e      �� �   93      ,T�
Ia�B  C  �d       ����
 @   ��e     @ �   �1      D,T�Ia(C  �D  �e       ���� @ 
@   �
��e     @ �   �1      D,T�
Ia�D  �D  �	��e     @ �   �1      ,T�Ua      ��a      � Rf�%   getElementsByTagNamea�D  E  I��e     @ �   :3      ,T��a      ��a      �ReV9��   createElement   aEE  �E  I��e     @ �   ;3      ,T��a      ��a      �Re��   createTextNode  a�E  
F  I��e     @ �   <3      ,T�Qd   	   Zc.append   a'F  iF  I��e     @ �   =3      ,T�Qd      Zc.contains a�F  fG  I��e     @ �   >3      ,T�IarG  �G  ���e      @ �   �1      ,T�Ia�G  HH  u�e      @ �   �1      ,T9�`.  0L`   ��y��� a�bo����Rb�   \+   ��D%P          -  ���^����-��-�_���o�
�q�m��j-��E�^���&��k��?��k��5-��8��^�� =��k��&��k��#��k����D8���Ċ~ ����n���-��_������n��	�n� ����-D"8�!���!#�-%����l�'������_����(�-�*�y, �	�_����-�b��/� (Sb��        �`����Da��  n�  �0�j1       P #@"#�	 F��H L    �e    @ �   �1      ,T�
IaCJ  rJ  ��e     @ �   �1      ,T`�`~   L`   R�z�   location Rr��0   foo SbJ �         ��`�� �(Kh@      ;   �        )   I     Dr(          ��x�-  �-�UŘ$��(�-�b��ŋ����������� (Sb��        �`����Da��  ��  ��c       P    �e    @ �   �1      ,T8�`,   L`   R����   top �Dh          ��-  �b���-  ��  (Sb��        �`����Da��  ܗ  ��b         �e    @ �   �1      ,TԒ`f  LL`"   R��-   SCRIPT  R��l�   src �Rex�e   ownerDocument   Rd��   defaultView Rc6�?�   window  R����   documentRe�G�q   querySelector   R2I]�   script[nonce]   RcnY��   nonce   Rd�\�   getAttributeIRd.�@�   setAttribute�mRd�>��
   parentNode  Rd&��   insertBefore�D�           �� �c�� ���b�2�ğ-���_-��-��-�
�!�-��-�ğ�^�����-	�-
�	�^�����-��	�_���-��^�� �/�"�-$�-$�-�&�_���(��� (Sb��        �`����Da��  ��  (�h*       � P P P @ � @ �e    @ �   �1      ,T<�`4   L`   Re��¢   getComputedStyleRdƞ�W   currentStyle�Di           -  �-  ��_���-�  (Sb��        �	`����Da��  .�  ��b       P �e   @ �   �1      ,T��`�    L`   �Rc���   crypto  0�6%	Rer5l   getRandomValues Kd       V   �     D|           !  �-��!�-��]�����!
���i���!  �-��-��^���/�G      G      ��!�-��]���  (Sb��         �`����DaH�  ��  �e       ` ` @ @H �e     @ �   �1      ,Tt�`�   L`   �}�����Dw`          �P�Lv����� �����>���� �0��! �-��-��-��_���	��/����`���= �(Sb��         �`����Da��  N�  ��d       Ԁ
� �� �e  
 @ �   �1      ,T��`�   L`   y��Ya  �����AYa      �A�D|0          -  ���k���xV4���m��4�M��N8��-	�^��8��@�?�����PÊ5 ��m����
��8��(Sb��        �`����Dad�  ��  �d       �!� �!�e   @ �   �1      ,T�
Ia~O  �O  M��e     @ �   �1      ,T<�`8   L`   9�Di          ��-�  �^���/t��(Sb��         ��`����Da��  ܟ  ��c       �    �e  
 @ �   �1      ,T��`�   L`   RZg�   frames  RFn��   parent  (Kh@      #   )     3   H   Q     D{0          �2��n� �X��- T�- �/�TTŋ�������-k��	����
��������PÊ\ 	�(Sb��        �`�����a�  `�  �c
       � 
 � �e    K� �   ?3      ,T�
Ia�P  �P  I��e       K� �   @3      ,T@�`>   L`   0Sb��        !�`$   `�����a��  *�  ��a      R2��   display ,T@�`:   L`   uRd�L(�   setProperty Rr���	   important   �Dj(          �-�  �-�����\���   (Sb��        I`����Da̡  &�  =�b       @ �e     K� �   �5      �Dj           � �%�� �} )Á �c���� ��b      �   �e     K� �   A3      I,T�
=a)Q  `Q  ��e       K� �   B3      ,TD�`D   L`   0�6!Ya      0C�Dk           !  �-��!  �-��a��:��^���	�  (Sb��         p`�����aԢ  <�  ��c       P �  �e      K� �   C3      ,Tp�`�   $L`   R�5�s
   goog_pvsid  ���a
      �C�H�SbJ �         a�`�� �Kd       .   �      Dv8          -    �7��!�-�� �})���a�3�	�\���ă������!�-  �b����� (Sb��        �`�����aD�  r�  ��d       P � �  �e     K� �   D3      ,T�
�a=R  (S  ���e      K� �   E3      ,TP�`Z   L`   Y�����Dn0          ��!  ��-��!�b���-��]��
�^����   (Sb��        �`����Dal�  �  �d       ` @    �e    @ �   �1      ,T�
Ia�S  �S  Q��e     @ �   �1      ,T�Ia�S  	U  ���e     @ �   �1      D,T��aU  �U  ���e      K� �   F3      D,T�Aa�U  2V  ���e      K� �   G3      ,T9�`,  LL`"    Rb`��   google_logging_queue��]
`    Lb           1	RZ�Z'   hash�R�G��   google_logging  Rd���+   localStorageRc6���   getItem SbJ �         !�`�� YPRr�y'B   https://pagead2.googlesyndication.com/pagead/js/logging_library.js  ] �� �Re�ܞ�   type_error:ConstKd    c   �   y     D%H          ��| ����-� ���|�2� ��-� �-��z%���6��	��6��	^�������k���a%������b��%��-��-��-���^���l�U�%��-��-�	��^����%�����
���������S�-��������i�� ����q�"� -�#��l�%�-�&��l�(�-�)��b��+���c���-�  (Sb��        `�����a֬  t�  �,�i/       ��� ���
� "@ P �  �e     K� �   H3      ,T|�`�   L`
   �wR���   AMP_CONTEXT_DATARd�M�
   pageViewId  Rd6[%�   canonicalUrlKd       0   �     Dy           ���-�  �-�ė ��-��-� �-��-�ċ������-�
������-������(Sb��        �`����Da��  ��  �d       P P    �e     @ �   �1      ,TL�`T   L`   Rcv�"2   master  �Dm          �
��a� �Ř��-� �b���-� ���  (Sb��        �`����Daα  2�  ��b       @ �e     @ �   �1      ,T��`�   L`   yBp�D{@          ������-�  l����/��b���/���-�  m�	�,!��/��b����D/�8�8�
����P��/ ��b���  (Sb��        �`����DaJ�  H�  � �f       D �! @ x	    �e   @ �   �1      ,TL�`V   L`   R�^�#   google_unique_id�Dm          ����b� �b���- ��D2� - � (Sb��        �`�����aX�  �  �c	        `  �e     K� �   I3      ,T0�`    L`   ��Df           -    ���(Sb��        A`�����a�  ��  ��b          �e     K� �   J3      ,T�
)aEZ  �Z  ���e       K� �   K3      ,TX�`p   L`   I��Rb���   ca- ���Dp(          � �- �]��-���_���k�U��8�	�(Sb��        �`�����ad�  
�  �c
       P D �e     K� �   L3      ,T�Ya[  �[  ���e     �� �   M3      ,T�
�a�[  �[  ���e      K� �   N3      ,T�a\  #\  ���e     �� �   O3      ,T�m	a9\  q\  ���e     �� �   P3      ,T��`�   ,L`   �I-	SbJ �         a�`�� ��Reک��   \bdeid=([\d,]+) SbJ �         !�`�� �(Kh@      e   9        %   9     D�0          ȫ �p%�������-� �-��-�ŋ�������� �-��-����� -�
�y  �^���Ę	/��%ȋ������Ȫ (Sb��         �`����Da�  ^�  ��d       P P �  �e     @ �   �1      ,T�
Ia;]  �]  !��e      @ �   �1      ,T@�`:   L`   Rd�vG   performance r�Dj          �-�  Ř-��-��]����   (Sb��         �	`����Dab�  λ  �b       @ �e     @ �   �1      ,Th�`�   ,L`   Rc�XqH   label   ��Rc�W   durationRcV��7   slotId  Rc�NG�   taskId  0�6Rcj�Xa   uniqueId�Dt           ��a� ���a��2 2�22
�2�2!�-��]��2�   (Sb��        �	`�����a �  �  ��e       � ,� ,` � �e    ���   Q3      ,TL�`V   L`   Y�Rb�    1337�Dm(          Ϋ Ř#��a� �-�x��-���^���o���� (Sb��         I`����Da��  8�  �c       @    �e      K� �   R3      ,T�
Ia)_  �_  ���e     @ �   �1      ,Td�`�    L`    �� � !�(R$���   google_js_reporting_queue   y�Ds          2  -�-�-�k�U�)ϫ�a�	�!�-���c���-��2���  (Sb��        �
`����Da^�  ��  ��d       P D`  �e    @ �   �1      ,T��`�   (L`    � !�}$R ZG��   google_measure_js_timing�0�6 ���D{(          | 2 ��2���-��|2�-2 
-�ϫ�a��������!�-��]��n�2��(Sb��        Rb���   ce  `����]a(�  ��   �f       s�
`.� `     �e    ���   S3      ,T|�`�   ,L`    ��Rc�G   goog_   5 ��ERc*�%   _start  )RbFr2�   mark�Dy          -  �������i���-w8��8��-w8��8�ͫ�ϫ�a�	���-��^���(Sb���        Rc
�   start   `����Da��  ��  ��d        � 
�   �e    �� �   T3      ,T��`(  @L`    ���9�5 ��ERb�\Б   _end)� �y���D�          -  ��-  ����a�����a��-9�2�	�-w8��8��-w8��8��ͫ�ϫ	�a����-�
�^���- T�$  �-�-�m��-�-��^��!�(Sb���        Rb�0Y{   end `����Da��  (�  $�g#        	�� @ P �  �e    �� �   U3      ,T�
Ia!b  Cb  )
��e     @ �   �1      ,T�
IaNb  �b  �e       ����
 @ �    =��e     @ �   �1      D,T�Ia�b  d  i�e     @ �   �1      ,T�Iad  md  ���e     @ �   �1      ,T�Iazd  �e   �f       ����
 @ T @   @ @q��e     @ �   �1      D,T�
Ua      mQd      .constructoraf  :f  	��e      �� �   V3      ,T�IaGf  �g  }�e     @ �   �1      ,TX�`l   $L`    a� !� � �� !� � !��Dp           ������2  �2�222-
2�  (Sb��        �`�����an�  �  ��d       ,� ,P    �e    ���   W3      ,T(�`   L`    !��Dd           2  �(Sb���        Rb�l+�   eb  `����Da
�  $�  ��b          �e    �� �   X3      ,T(�`   L`    ���Dd           2  �(Sb���        �
`����Da(�  B�  ��b          �e    �� �   Y3      ,T(�`   L`    !��Dd           2  �(Sb���         a�`����DaD�  ^�  ��b          �e    �� �   Z3      ,T�`�  HL`     � ���	�SbJ �         a�`��  !� !���a      IC�SbJ �         ��`�� � �Uy�r(Kh@      O   �     e   �   q     D	h           ����-  �@- �-��5- �-��-
�]���_�����a�- �-��^����aŋ������-���� �b��-��� ���}	 )��� ��b��!3�
#�i��%�����\��'�"�������-)� � ��_���+��!!-�-�/��-�1���^���3������   (Sb���        I
`����Dab�  T�  �0�j5       P P @ @ @ L� 
� 
�  �e   �� �   [3      ,T8�`,   L`   @Sb���       � a� !�b�� �   $   �`����DaX�  ��  ,TL�`R   L`   0Sb��         ��`�� I`����Dap�  ��  ��,T8�`0   L`   � ���Dh           ��-�  ����_�����(Sb��         I`����Da��  ��  ��b         �e      K� �   �C      �Dm0          � ���%�%� �-� �� �� �_�����   ��b        �e      K� �   �5      �Dh          � �%%%� �  ��a       �e    �� �   \3      ��,T�
Iabi  �n   ��e     �� �   ]3      ,T@�`<   L`   8Sb���       � !�a�� $    a�`����Da`�  8�  ��,T�Ia�n  o  I��e      K� �   �5      �Dj           � �%%- ā �^���  ��b        �e    �� �   ^3      �,T(�`
   ]�Dd            �   (Sb��        �	`�����aJ�  v�  �e     K� �   _3      ,T$�`   ]�Dc           �(Sb��        `�����a~�  ��  ���e     K� �   `3      ,TH�`L   ]�DlH      ��������Z����������������j�� ���  (Sb��         �`�����a��  ��  ��b          �e     E��   a3      ,T0�`   ]�Df           �T��   (Sb��        -`����Da(�  X�  �e    @ �   �1      ,T\�`x   L`   y�Dq           ��-  m��'�/�]��l��������PĊ* 	��T��(Sb��        `����Dan�  D�  ��c
       D � �e   @ �   �1      ,TȐ`P  L`   0Sb��        a�`�   �`����DaZ�  ��  �,T0�`   L`   0Sb��        �`$   I`����Da�  8�  Y,T4�`&   ]�Dg          ������c��� � (Sb��         I`����Da"�  8�  i�b          �e     K� �   �5      �Df          � �%� �  ��a       �e    K� �   �5      �D�0          � �%�i ��� ���`�� �-���� ��c����v ��c���l���� ��� �/�	��c����b����# Á �c����l��l����� ��c������ ��c�������e      @ D� "  �e   @ �   �1      ]m,Te�`�   L`   SbJ �         a�`�� ��=Ra|  X  Kd    �   �   �     DQP          d���c�� �l����l��(l��>�Yv�c�ݾ��`���c��ċ>v�c�ݾ��`���c��	ċ!v�c�ݾ��`���c��ċ����/��/����_��H��`���d�Ë��� �����v��c���l���x��l���U��l�� u�c�޾��`���c���Tl� ���l� �"�>t�c�޾��`��!�c��#�#u�c�޾��`��%�c��'����l�)�l�*�	�l�+��U���!,�b��.�c��0l�2����l3�� l�3�l�3�
l�3�@l�3�W�sm�4�n�5���b�6�!��b��8�!:��i�<�-�>�^���@��U����!,�b��B�c��Dl�F��U���!,�b��G�c��Il�K��   (Sb��        �`����Da��  �  @�nL       D� � �  �H D� "1@ �#` @ 	�� �e   @ �   �1      ,T8�`*   ]�Dh          T�T���c� TT�   (Sb��        �`����Da"�  b�  ��b          �e   @ �   �1      ,TH�`L   ]�DlH      ��������Z����������������j�� ���  (Sb��         4`�����ar�  r�  ��b          �e     E��   b3      ,T�
Ia�s  
t  Rc��   getValue��e      �� �   d3      ,TH�`L   ]�DlH      ��������Z����������������j�� ���  (Sb��         Q`�����a��  ��  �b          �e      E��   c3      ,TH�`L   ]�DlH      ��������Z����������������j�� ���  (Sb��         9`�����a&�  &�  ��b          �e     E��   e3      ,T��	amt  xt  ���e      �� �   f3      ,T�
Ia�t  �t  ��e     @ �    2      ,T�ma�t  �t  ���e      �� �   g3      ,T�Ia�t  �t  ���e      �� �   h3      ,T�Ia	u   u  �	��e     @ �   2      ,T�Ia+u  Bu  �	��e     @ �   2      ,T�adu  ou  ���e      �� �   i3      ,T�
Iawu  �u  Rc7-�   getWidth��e      �� �   j3      ,T�Ia�u  �u  Rd��us	   getHeight   ��e   	   �� �   k3      ,T�Ia�u  �u  1�e     @    2      ,T�Ia�u  �u  		��e     @   2      ,T�Iav  v  ���e     @   2      ,T�Iav  ;v  ���e     @   2      ,T�a]v  hv  ���e      ��   l3      ,T�Iauv  �v  Re���   getContentUrl   ��e      ��   m3      ,T�]a�v  �v  ���e       E�   n3      ,T�
a�v  �v  ��e       E�   o3      ,T��a�v  �v  ���e      ��   p3      ,T�Ia�v  w  m��e      �� 	  q3      ,T�Iaw  9w  i��e     @ 
  2      ,T��a[w  fw  ���e      ��   r3      ,T�Iasw  �w  -	��e     @   2      ,T�
=a�w  �w  ��e      ��   s3      ,T�ea�w  �w  ���e      ��   t3      ,T�Iax  !x  ���e     @   	2      ,T�Ia,x  Bx  ���e     @   
2      ,T��adx  ox  ���e      ��   u3      ,TH�`N   ]�Dl8      ������Z��������������i��  ���� (Sb��         `�����a&�  <�  �b          �e    ��  v3      ,T�
Ia�x  �x  �	��e     @   2      ,T�Ia�x  �x  m��e     @   2      ,T�Ia�x  y  i	��e     @   2      ,T�Iay  1y  ��e     @   2      ,T�Ia<y  Ry  �e     @   2      ,T��aty  y  ���e      ��   w3      ,TH�`N   ]�Dl8      ������Z��������������i��  ���� (Sb��         Q`�����a`�  v�  ��b          �e    ��  x3      ,T�
�a�y  z  ��e      ��   y3      ,T�Iaz  Az  A��e     @   2      ,T��acz  nz  ���e      ��   z3      ,T�Ia�z  �z  $Rgz��   getTagSessionCorrelator ��e      ��   {3      ,T�Ia�z  �z  ���e     @   2      D,T�Ia�z  ){  ���e     @    2      D,T�Ia5{  q{  m��e     @ "  2      D,T�
)	ay{  |  ��e      K� $  |3      D,T4�`$   L`    !� !��Dg           -  2 -2�  (Sb��         Rbvך�   rf  `����Da&�  d�  ��c       ,P    �e     @ &  2      ,T�IaO|  y|  �	��e     @ '  2      ,T�Ia�|  �|  0��e     @ (  2      D,T�
Ia�|  �|  �e     @ *  2      ,T�Ia�|  ~  ���e     @ +  2      �D,T�Ia~  5~  a��e     @ 0  2      ,T�]aZ~  �~  ���e     �� 1  }3      ,T�Ia�~  &  1��e     @ 2  2      D,T�Ia1  �  5��e     @ 4  2      D,T�
Ia�  /�  ��e     @ 6  2      ,T�Ia:�  X�  ���e     @ 7  2      D,TP�`Z   $L`    !� a� !� a� a� �� ��Dn             2  222|2	|22�   (Sb��        Rb~���   Bf  `�����a�  ~ ��d       ,� �&,  �e    ��9  ~3      ,Tx�`�   ]�Dxh       ������� ����d�������������Z������������i�� ����   (Sb��        �`�����a�  ��b          �e     ��:  3      ,T�
Ia�  \�  ��e     @ ;  2      ,T]�`r  4L`   8Sb��        �� ��a$  �� Q`����Da� � � !� ���� ��,T�Ia�  �  I	�e      K� =  �5      ��y1�`T  �DI8   8       � �%- �
�� ��� ��� �i�� �c����c���% ��  � ��  � ��  � ��  � ��  � ��  �i�� �c��
�c����c����-�-��]���c����c�� ��  �b� ��  �-� ��  �c��c��� �l�"�,�l�#�"�l�$�I-�-�%ā �^���'�3-�-�)��^���+d�-�-�-m�/�-�-�	0�]��2�   �0�j4      @ @ P @ P #� 
��"  �e   @ <  2      ,T�
IaU�  ̂  ��e     @ >   2      ,T�Iaق  N�  i��e     @ ?  !2      ,T|�`�   L`
    !� a� � �� !��Dy8   (       ���� ��i�� ���2 �2�2|2	��m�����a����;�m�2��  (Sb��        q`�����a� h ��d       ,� �"�� �e    ��@  �3      ,TL�`V   L`   Rb����   xa  �ݔDm           �-  �-�^���-  �i� ��2 �� (Sb��         !�`���� !�ax  ��c
       @ ,  �e    K� A  �3      ,TL�`V   L`   � a��Dm          }  )�s�~7�� s�~7�� s�~7�� �2� (Sb��         -
`�����aF � �c	       S	$`  �e     ��B  �3      ,T��`  ]�D�0          l ��l� �-l� �Al� �U�nv�c�����`���c���v�c�����`���c���v�c�����`��	�c���v�c�����`���c���� (Sb��        5`����Da� 
 ��d       � � �  �e   @ C  "2      ,T��`�   ]�D           ��l �� l� �%l� �*l� �/l� �4�Dr��c���u��c���t��c���u��c���_��H��`��	��  (Sb��        �`����Da.
 x ��c       � �  �e   @ D  #2      ,Th�`�    L`   URe�ɿ.   sessionStorage  RcfK}   GGDFSSK b/�Kd       =   �     Dt(           $�~���!  �-��!  �-��-���^���Ř!�-�
�^�������~�   (Sb��         I`����Da� � �d       P P    �e      K� E  �3      ,T��`�  $L`   � !��}�b/�/�D�P   (       ������ "� *� �b�� �-�� "� *�b���-���/�����-�
�^������!�i�� �-��^���4�� '��a���/���/�� /�b����/� ė����!"�-�$�^���&�b��(� )�b��*� &�c���,������   (Sb��        %`����Da� 8 �,�i.       @  P @ �  �  ` @   �e   @ F  $2      ,T��`�   L`   0Sb��        a�`�� �`����DaN @  a�,TH�`L   L`    a��Dl(          ����h������`�� �� �c����  (Sb��        I`����Da� � ��b         �e     K� H  �5      ��D�8          � �% ��"  � ��#  �b�� �-�%�9�% ��i ��� ���`�� �c���Ř-��]��
����h ��� ���`����� ��d      @ @    �e   @ G  %2      �,T@�`>   L`
    � a� !�5 ���Dj          ~2  |2~2!�i�� 	2� (Sb��         �`���� !�ah � ��d       ��     �e     ��I  �3      ,TH�`P   ]�Dl@           ����� (�������`�� x�(Sb��        `����Da�  �b          �e   @ J  &2      ,Td�`�   L`   ����DsH           �����!  � (�������`���b���!�b�������   (Sb��        �`����Da8 � ��c
       @   �e    @ K  '2      ,TT�`f   L`   I�Do@           �� ��� (�������`�� � ����� (Sb��        9`����Da� $ ��b          �e    @ L  (2      ,T`�`~   L`   �!�Dr@           ��| ��� (�������`���! �-��^�������� (Sb��        �`����Da: � �c	       � 
�   �e    @ M  )2      ,T\�`t   L`   � ��Dq           "� *� �b�� �-��/�� "� *�b���-��~�4�
��  (Sb��        5
`����Da�  ��c       � � �e   @ N  *2      ,TH�`P   L`   0Sb��        ��`�� I`����Da l �,T�
Ia'�  4�  Ie�e      K� P  �5      �Dl           � �% ��/  �b� %�� ā �c�����b        �e    @ O  +2      i,T��`�   L`   @Sb�� 
       !� � !�b�� �� �� !`����Da� 2 $Sb� �         a� ��a�� �� �,TX�`l   L`   8Sb��        � ��a$   �� I`����Da� � � !�,T��`�  DL`   � ��u�� !�� a��� !��5 � ��	`l  �D�H          �d����c�� � ��% �c��Ė ��" � ��* � �b���-��-���^���
��-��^�����-���^������|� �� �i�� �l����l��7l��Ul��s���b�� ��	 ��A�b�����`���u�b�� ��	 ��A�b�����`���P�b�� ��	 ��A�b�� ���`��"�+�b�� ��	 ��A�b��$���`��&�ŋ�`������`��(�������7 ��" � ��* � �b��*�-�,�/�.��-�0�^���2�x������-�4�^���6����? ��" � ��* � �b��8�-�:�-�<��^���>��-�@�^���B�x������-�D�^���F���� ��" � ��* � �b��H�-�J�-�L��^���N�-�J�-�	P��!
R�i�� T�_����V-�J�-�X��^���Z�-�\�^���^�#-�J�-�`��^���b�-�	d�|f�_����g���/-�J�-�i��^���k�-�m�^���o�-�q���^���s��-u�]�w4��y� (Sb��        I`����Da� � �`�v{       @ @ P L"@ @ @ � P P P @ P ` P @ P ��� 
���  �e    K� S  �5      �Dp           � �%% ��/ ��b�� %�!���Á �c����  ��b        �e    K� R  �5      y�D�H   0       � �%%%%��%��%��%%| %|%�! �� �c����-��	�-��# ��  ���� �����`���� ��c
      sP   �e    @ Q  ,2      ��,T�
Ia��  �  ��e     @ T  -2      D,TT�`f   L`
   �� �,T,�`   L`   ��De          !  �b��   (Sb��        I`����Dav � ��b         �e     K� W  �C      �Do(          #�!  �-�� "� *��b���-��^���ā �c���
� (Sb��         �`����Da2 � �c      @ @ �e     @ V  .2      �,T\�`r   L`   � a��Dq          &� "� *� �b�� �-��c��� 0� /��b���c���   (Sb��        `����Da� � ��c
       @   �e    @ X  /2      ,TT�`f    L`   ������a      �C��Do(           -  �^��'!�-��!�b�
�})�3��\��� (Sb��         a�`����Da� � �d       ` @ � �e  
 @ Y  02      ,T,�`   ]�De            / ��  (Sb��        Y`����Da� � ��b          �e   @ Z  12      ,T��`   ]�D�            5�� +��`��  5�� ,��`�� 5�� -��`�� 5�� .��`�� 5�� 2��`�� 5�� 4��`��
�(Sb��        �`����Da� n ��c       @ @ �e    @ [  22      ,T��`�   L`
   ,T�
IaJ�  X�  I��e      K� ]  y5      ,T�Ia`�  ��  I��e      K� ^  z5      ,T�Ia��  ύ  I��e      K� _  {5      ,T�Ia؍  �  I��e      K� `  |5      ,Tȓ`L  (L`   ]
`    Mc         
   a���a� a�U(Kh@   #   h   p  	   {   �   �  
   D�p           "� #�b�� �z %����-������]��	��e� �-��3-�������!�-��-���/���/�_�����A ����������#��-���]����e� ���������  (Sb��        I`����Da( � $�g        �� 
� 
� ��[    �e    K� a  }5      �D}            5�ā  ��`��  5�	ā��`�� 5�
ā��`�� 5�ā��`�� 5�ā��`��� (Sb��        �
`����Da� � ��c
      @   �e    @ \  32      = E M U ] ,TL�`T   L`
   ��Rdr�U�	   init-done   ���a      �G�Dm(          -  ��^���!�-���})��\��	�  (Sb��        }`����Da� r �c       ` L �e    @ b  42      ,Td�`�   ,L`   8Sb��        �� a�a$  �   9
`����Da� r �,T�
Ia؎  ގ  I� �e       K� d  ~5       !�,TL�`R   L`   ,T�Ia�  ��  I��e       K� f  �5      �Dm(           ��6 ��  �`�� ����`���   (Sb��        I`����Qb      a.j a�   ��b        �e     K� e  5       a�,TD�`D   L`   ,T�Ia�  �  I��e       K� h  �5      �Dk(           ��6 ��  �`�� ��b���  (Sb��         I`����Qb      a.g a
 4 ��b        �e      K� g  �5       ��,TD�`H   L`   ,T�Ia+�  1�  I��e       K� j  �5      �Dk(           ��6 ��  �`�� ��c���(Sb��        I`����Qb      a.h a> p ��b        �e     K� i  �5       ᪕Ds0           � �%% ��6  ���� �`�� 2�2�2�2���c
      ,� ,  �e    @ c  52      � � � � � � !,T�
IaC�  G�   !��e      �� k  �3      ,T�IaH�  L�   ᪖�e      �� l  �3      ,T�IaM�  Y�   a���e      �� m  �3      ,T�IaZ�  f�   ����e      �� n  �3      ,T$�`   ]�Dc           �  (Sb��         Y`����]ar r ��e      D�o  �3      ,TԐ`f  0L`    ��0�6Ya{�G�z�?SbL �         a�`�� �	,T�
IaЏ  �  IY!�e      K� q  �5       Rf2��   /pagead/gen_204?id=  a�SbJ �         a�`�� Kd    <   �   !     D�X   0       ���������-�  �!�-��]�����m��z���%%�� �q�	��%��� �i�� 
%� �� �c����� ����8��8��c������ �� �c�������	������ (Sb�� 
       �`����Da� �  ��e      P �J @H    �e    @ p  62      ]!,T@�`:   L`    ���Dj          �o� ��p��
2 �   (Sb��        -`����Da�  �  ��b       1.  �e    @ r  72      ,T4�`&   L`   0�6 ���Dg          !  �-��]��2� (Sb��         9`����Da! B! ��c       @    �e     ��s  �3      ,T�`�  PL`$   �!URz^.	
   google_srt  0�6	)�
,T�
IaQ�  W�  I�e       K� u  �4       a�Rc�An�   completeYRdBf��
   readyState  � ���S,T<�`4   L`   U�	�Di          !  �-���� A��b���  (Sb��         I`����Da�# �# ��b       @ �e      K� v  �4      �D           �� >� �i��  � ?�� %?!�-�  U�!�!�-��]��
�2�� =� ?�!�-��c����� ?�� A���i��� @�� %@ @�-�Ł	 �^��� @�-�
��^����!�-��-�k��!�-���� A�b��!�! A�-�#���!�Á�`��%�(Sb��        I`����Da�! �# (�h'      P @ @ P P � 
�   �e     K� t  �3      �!�!,T�
Iae�  ��  U	��e     @ w  82      ,T�!a��  ֒  ���e      �� x  �3      ,T�Iaݒ  �  e��e     �� y  �3      ,T�Ia�  ��  A��e     @ z  92      ,T�a��  œ  ��e     �� {  �3      ,T�Iaɓ  �  i��e     �� |  �3      ,T�
�a;�  Q�  ���e     �� }  �3      ,T�IaX�  ��  Q蓑e     �� ~  �3      D,T�Ia��  ��  �蓑e     �� �  �3      ,T�Ia��  Ք  ���e     �� �  �3      ,T�Iaؔ  �  u��e     �� �  �3      ,T�Ia�  '�  ���e     �� �  �3      ,T�Ia4�  �  Q�e     @ �  :2      ,T�Qa��  ��  ���e      �� �  �3      ,T�
Ia��  ��  ���e     �� �  �3      ,T�Ia�  �  ���e     �� �  �3      ,T�Ia�  3�  �a��e      �� �  �3      ,T�Ia@�  r�  m��e     @ �  ;2      ,T<�`8   L`   ��a      �C��Di           P� �} )�3���i���(Sb��        }`����Da�, @- �b       � �e    @ �  <2      ,T4�`(   L`   ��Dg           P� ����i�� �(Sb��        �`����DaV- �- ��b          �e    @ �  =2      ,T�
IaЖ  ��  
��e     @ �  >2      ,T�Ia	�  0�  �e     @ �  ?2      ,T<�`2   L`    ����Di          -  U�-�]��b��   (Sb��        �`����Dav. �. ��c       @    �e    @ �  @2      ,T4�`(   L`    �� ᪒Dg          -  U�-�b��(Sb��        Y`����Da�. "/ ��b       @ �e    @ �  A2      ,T0�`   L`    �� ᪒Df           2  2�  (Sb��        �`����Ua      ]Qd      .constructoraJ/ z/ $Sb@ �        �b�� "/ X0 �b       ,  �e    ���  �3      ,T�
Iaŗ  ܗ  ��"�e      �� �  �3      ,TX�`l   L`    �����Dp          -  �.-�]��b��q�� ��J �b�	��  (Sb���        ��`����Da�/ V0 ��c       @ T �e    �� �  �3      ,T�]aG�  ��  ��e     �� �  �3      ,T�
Ia��  ��  ���e     �� �  �3      ,T�Ia��  ̘  Rc���   contains��e     �� �  �3      ,T�Qc      S.getId a�  �  I��e      �� �  �3      ,T�Ia��  7�  ���e     �� �  �3      ,T�IaT�  h�  Rc��?�   getId   ��e      �� �  �3      ,T� ��a?�  ?�   ����e       E� �  �3      ,T� a�a��  Ι  ���e     �� �  �3      ,T�aٙ  �  ���e      K� �  �3      D,T�Ea�  '�  ��e      K� �  �3      ,T�
qa+�  ��  ���e      K� �  �3      ,T�Ia��  ֛  Rd�g��   setLocation ��e     �� �  �3      ,T��a��  ��  ���e       E� �  �3      ,T�Ia�  ��  5��e     @ �  B2      �D,T�Ia��  ��  ���e     @ �  C2      ,T�Ia��  n�  a��e     @ �  D2      ,T0�`   L`   �
��Df           2  2�  (Sb��        �`����Ua      ��a      �(Rh^o�   google_reactive_ad_format   a= 8= �b       ,  �e    ���  �3      ,T�
Ia�  �  `    ��e       E� �  �3      ,T�	a �   �  ���e       E� �  �3      ,T��a9�  9�  ���e       E� �  �3      ,T�aR�  R�  ���e       E� �  �3      ,T�E	a~�  ~�  ���e       E� �  �3      ,T�!
a��  ��  ��e      �� �  �3      ,T�Iaӟ  ��  }��e     @ �  E2      ,T<�`6   L`    ��Rd��   defaultValue�Di          �����2  �2� (Sb��         �`���� �a�A �A ��b       ,  �e   ���  �3      ,T<�`6   L`    ��$�Di          �����2  �2� (Sb��        Rbz��L   Ug  `����$a*B tB ��b       ,  �e    ���  �3      ,T<�`8   L`    ����Di          ��| ���2 �2�(Sb��        Rbj���   Vg  `����5$a�B �B �b       s� �e    ���  �3      ,T<�`8   L`   I ����Di          �� ���2 �2�(Sd��            ����Da�C �C ��b       ,  �e    ���  �3      ,TX�`j   4L`   0Sb��         !�`�� �`�����afR �S �,T�
Qb      Ah.iaH�  `�  I]$�e      K� �  �5       !�,T�Qb      Ah.gah�  ��  I��e      K� �  �5       ��,T�Qb      Ah.ja��  ��  I��e      K� �  �5       a�,T�Qb      Ah.ha��  ��  I��e      K� �  �5       �,T�Qb      Ah.maȩ  Ω  I��e       K� �  �5       !��Dp          � �%~%� 2 �2�2�2�	2
�   ��c
      ,� ,  �e     ���  �3      a$m$y$�$�$,TH�`J   L`    !� ��$�Dl            "� ��b�� �-� �-�-�_�����   (Sb��         !�`����Da�S T �c
       P   �e  
 @ �  F2      ,TP�`^   L`    a� ����Dn            "� ��b�� �-� � h�-�� h�-��_����� (Sb��         %`����DaT jT ��c
       P   �e     @ �  G2      ,T�
IaA�  ��  ���e     @ �  H2      ,T�Ia��  l�  �
��e     @ �  I2      �D,T�Iay�  ح  }��e     @ �  J2      ,T�Ia�  ��  =�e     @ �  K2      ,TL�`R   L`
   YRd�1�
   CSS1Compat  Rd�n
   compatMode  Re~�4y   documentElement Rb�|C�   body�Dm          -  ~���-k��-�-���~�   (Sb��        y`����DaH] ^ ��c	       �   �e    @ �  L2      ,T0�`   L`   Rd6�{0   clientWidth �Df           ��b� �-� � (Sb��        `����Da"^ \^ ��b         �e    @ �  M2      ,T�
ea6�  ̯  ��e      K� �  �3      ,TH�`N   L`   ��Rez�0;   parentElement   �Dl          ��c� Ř �-�k���-��! � (Sb��        �`�����a�_ h` ��c       �  �e     K� �  �3      ,T�
Ia@�  �  ��e     @ �  N2      ,T��a��  p�  ���e      K� �  �3      ,T8�`,   L`    !��Dh           ��c� �- ��  (Sb��        }`�����a�b c ��b         �e     K� �  �3      ,Tx�`�   ,L`   Y�$$R ���   getBoundingClientRect   �a
       �C !�CR�dӋ   left �- !�SbJ �         ��`�� Kd       I   I     Dx(          ��-  �-��-��]���-�]�
�})�-��-�9�3�-��-�9�3���������  (Sb��        �`�����a c ^d � �f       P @ P �
�  �e     K� �  �3      ,T�
a3�  u�  ���e      K� �  �3      ,T��`�  ,L`   �Rd�w�l   innerHeight Rd^��d
   innerWidth  	(Rh�2w   google_full_width_responsive9%R��S�   width   Rc�)s�   visible ��D�   0       -  l���b���� �� ��b�n���-�-
p��� ��b�Ř9���;��n���r�-k�U�X- ��b��@��c��*��-�b���p��- k�"��-#�> %���(Sb�� 
       �`�����a�d �g (�h&       D P � #��� �  �e     K� �  �3      ,T��`   L`   Ya333333�?	�%�D�8   (        �� �����`�� ���	��i�-k�� ��c��M ��b�9	��o�
��'�!��n����n����
�	�(Sb��        E`�����a�g �h ��d       	� #   �e     K� �  �3      ,T�
ua��  ��  ��e      K� �  �3      ,T��aɴ  ϵ  ���e      K� �  �3      ,T�!aӵ  ��  ���e      K� �  �3      ,T��a	�  ��  ���e      K� �  �3      ,T0�`   L`    !� !��Df           2  2�  (Sb��        )`�����aFo vo ��b       ,  �e   ���  �3      ,T(�`
   L`    !��Dd           -  �   (Sb���         R&/��   height  `����Da�o �o �b          �e    �� �  �3      ,Td�`�   L`
    !� !�0b627�Ds(           ,�m� � ,�- m��-�%!�-�� ��!�-�
�^���_�����(Sb���         ��`����Da�o (p ��d       ��X P   �e    �� �  �3      ,T�
Ia�  �   ᪒�e      �� �  �3      ,T$�`   ]�Dc           � (Sb��         �`����DaVp ^p ��e     K� �  �5      ,T|�`�   L`   A&u�Dy8   (       �����  ���-� �-� ��/��-� ��/��b���!��c������/�
��/��b���� (Sb��        �	`�����aDp 
q ��d       � �   �e    K� �  �3      ,T0�`   L`   0Sb��        !�`$   �`�����aq *q ,T,�`   L`    !��De          -  �o�� (Sb��        I`����Daq *q m&�b       D  �e     K� �  �5      �Df          � �%� �  ��a       �e     K� �  �3      q&,TX�`l   L`   8Sb��        !� a�a�� �� A`�����a2q �q ��,T<�`2   L`    !�& a��Di          � �- �]���p�T�   (Sb��        I`����Da�q �q �&�b       �e    K� �  �5      �Dp   (       � �%%� ���  �c� % ���  ��c�%� �  ��b        �e     K� �  �3      �&,T0�`   L`   0Sb��        !�`$   �`�����a�q �q ,T0�`    L`   ��Df          -  �]��o��(Sb��        I`����Da�q �q �&�b       �e     K� �  �5      �Df          � �%� �  ��a       �e     K� �  �3      �&,T@�`@   L`   Rdֱ�I   clientHeight�Dj           ��c� � ��b��-� Edm��(Sb��        �`�����a�q @r ��c       @
   �e     K� �  �3      ,T��`�  0L`   &uRc�D�   inherit �0b6R1�	   maxHeight   9%Rb�K��   HTMLRc:/   tagName �D�8           �� �����`�� Ř��-�-� �-�2�  �� �����`���-��2� 
����!�-���-�-� �b��Ę!�-��_����� �������`��Ę!�-��_�����-��-	k� ��g !��(Sb��        �`�����aHr �t $�g"       P � @ @ P � �e     K� �  �3      ,Th�`�   L`   0"6�&�Dt(          ���b� k���1��-! �-�� � � ��b�	�-�FG�_����� � �   (Sb��        �`�����a�t du ��d       D
 B
�   �e     K� �  �3      ,T�
Ia��  I�  ���e     @ �  O2      ,T��
aQ�  Q�  ��e       E� �  �3      ,T�Eaj�  j�  ���e       E� �  �3      ,T�Ia��  ��   ����e      �� �  �3      ,T�Ma��  ��  ���e       E� �  �3      ,T��a��  ��  ���e       E� �  �3      ,T�]	a̽  ̽  ���e       E� �  �3      ,T�
�a�  �  ���e       E� �  �3      ,T�aa��  ��  ��e       E� �  �3      ,T��a�  �  ���e       E� �  �3      ,T�IaG�  [�  RcZ �s   getName ��e      �� �  �3      ,T�]
a0�  0�  ���e       E� �  �3      ,T�}	ao�  o�  ���e       E� �  �3      ,T�=a��  ��  ���e       E� �  �3      ,T�
Ia¾  ݾ   ����e      �� �  �3      ,T��a��  ��  ���e       E� �  �3      ,T�Ia �  �   ����e      �� �  �3      ,T��a�  �  ��e       E� �  �3      ,T�Ea+�  +�  ���e       E� �  �3      ,T�
Iaa�  ��  �d       ���� @  �	��e     @ �  P2      D,T�Iaſ  Q�  �d       ��ƀ @  ���e     @ �  Q2      D,T�
Ia\�  ��  �e     @ �  R2      ,T�Ia��  ��  Q��e     @ �  S2      ,T�a��  ��  ���e     �� �  �3      ,T�Ia��  &�  Rc"!�w   query   ��e     �� �  �3      ,T�Ia.�  ��  	��e      �� �  �3      ,T�Ua       a��a      �}a��  >�  	�e      �� �  �3      ,T�
Ia@�  M�  �
��e     �� �  �3      ,T�IaN�  [�   a���e     ��    �3      ,T�Ia]�  j�  Q��e     ��   �3      ,T�Iak�  ��   ᢕ�e     ��   �3      ,T�Ia��  ��  I
��e     ��   �3      ,T�Ia��  '�  ���e     ��   �3      �D,T�Ia(�  ��   a���e     ��   �3      D,T��a��  ��  ���e      K� 	  �3      ,Tl�`�   L`
   HSb�� 
       !� a� �� !�c�� �� �� �� 1`�����a� � $Sb� �         a� ��a�� �� e(URe���   queueMicrotask  ,T=�`8  \L`*    !� ��m(,TD�`B   L`    a�(R$�OXE   google_rum_task_id_counter  �Dk          � �-� �2��-� D2��   (Sb��         I`����Da�� �� i(�b       , �e      K�   �5       a��SbJ �         �`�� � �� !� a���0�a      5C�C9C�C`
   	�����a      AC�(�(Kh@   .   u   �     1   D   1     D)p          ���� ���! �� �b�����a�����������-��_���ċ1������������������c���
����������	�-�
����� ��� ��})����-��]��3��3����a����9�3��3����%��})��-���2�!�3�#�h��c���%��������(Sb��         I`����Da�� � i((�h'      @ @ �� 0� ���   �e     @   �5      �Du   0       � �%%%%%%%���%��%%��! %" U%� � e(�b        �e     K� 
  �3      q(}(,T�
qa��  ,�  ��e      K�   �3      D,TD�`D   L`   ��Dk8            �������`�� �-� �]���  (Sb��        
`����Dap� ƒ ��b       @ �e    @   T2      ,T�
Ian�  ��  9�e     @   U2      ,T�Ia��  ��  �
��e     @   V2      ,T�Ia�  D�  ���e     @   W2      ,T�IaO�  ��  -��e     @   X2      ,T��a��  ��  ���e      K�   �3      ,T��a��  S�  ���e      K�   �3      D,T�
Ia_�  ]�  ��e     @   Y2      D,T�Iaq�  ��  ��e     ��   �3      ,T��a��  ?�  ���e      K�   �3      D,T�iaC�  ��  ���e      K�   �3      D,T�xa��  ��  ���e      K�   �3      ,T�,a��  ��  ���e      K�   �3      ,T�
Ia��  ��   a�$Sb@ �         a�b�� �� J� ��e      ��    �3      ,TL�`T   L`   �a�Dm          U��2�  �-� ��� ���b���  (Sb��        y`�����a�� 0� �b       @ �e     K� !  �3      ,T��`�  @L`   9��!0�6a �� a��!Y�! ���S,T,�`   ]�De           ��a� �  (Sb��         I`����Da�� �� �b          �e      K� #  �4      �D�0          ������ >� �i��  � ��� %��-�  U��!�-��]���2�
� =� ���-��c����� �� ������i��� ��� %� ��-���^�����-�	�-�
k�� ��a��  ��-������� �`���   (Sb��        I`����Da4� ĩ �$�g      ` � P P ��    �e     K� "  �3      Y),T<�`6   L`    ��I
�Di(            �� �-� ����\��� (Sb��        �`�����aީ � ��b         �e     K� $  �3      ,Tx�`�    L`   Y ��Rb*�g   eid y	��Dx8            "� ;� �b�� �-��]���-T�-��-�
�]���2� <� �������`���  (Sb��        �`�����a� �� �d       @ P ,@ �e     K� %  �3      ,T4�`(   L`    �� a��Dg           �� �-� �_���(Sb��        y
`�����a�� ު ��b         �e     K� &  �3      ,T�
�as�  ��  ���e      K� '  �3      ,T��a�  .�  ���e      �� (  �3      ,T�Ia;�  -�  ]�e     @ )  Z2      D,T�Ia8�  h�  ���e     @ +  [2      ,T�Ias�  ��  ���e     @ ,  \2      ,T�Ia��  �  a��e     @ -  ]2      ,T�
Ia�  q�  ��e     @ .  ^2      ,T��a��  ��  ���e      �� /  �3      ,T�Ia��  �  ��e     @ 0  _2      ,T�Ia&�  x�  ���e     @ 1  `2      ,T�Ia��  ��  U��e     @ 2  a2      ,T�Ia��  
�  ��e      @ 3  b2      ,T�
M
a�  ��  ���e      K� 4  �3      �D,T�	a��  ?�  ���e      K� 7  �3      ,T�IaK�  ��  m�e     @ 8  c2      ,T�Ia��  �  �	��e     @ 9  d2      ,T�Ia�  T�  ���e     @ :  e2      D,T��
an�  �  ���e     �� <  �3      ,T�
Ia�  K�  m��e     @ =  f2      D,T�IaX�  ��  =��e     @ ?  g2      ,T�Ia��  ��  `    ��e       E� @   4      ,T�Ia��  9�  �e     @ A  h2      D,Tt�`�   L`   U,R(��F�   google_persistent_state_async    ����Dw            �� ����a� �! �-���� �-��-� �
� %�� ���i��  %��2�
��(Sb��         9`����Da�� � ��c       P � �e     @ C  i2      ,T8�`.   L`   �Rdf���
   google_ps_  �Dh           �� �/� �
�8�� (Sb��        !`����Da� \� �b       @  �e    @ D  j2      ,TL�`T   L`    ���Dm            ��b� - /Şa4/
���  (Sb��        �`����Dar�  � ��c       � 8� �e    @ E  k2      ,TD�`F   L`   0Sb��        ��`$  �`����Da� P� �,T$�`   ]�Dc           � (Sb��         I`����DaB� L� �*�e      K� G  �5      �Dk(           � �% ���  ā ���`�� � ��b         �e    @ F  l2      �*,T(�`   L`    ���Dd           ~2  � (Sb��         �`����%	ax� �� �b          �e     ��H  4      ,T�
Ia<�  V�  �
��e     @ I  m2      ,T�Qc      Gj.set  aq�  N�  I��e     @ J  4      ,T�Qc      Gj.get  aj�  �  I��e     @ K  4      ,T�Qd   
   Gj.isEmpty  a2�  J�  I��e      @ L  4      ,T�Qc      Gj.clearah�  ��  I��e      @ M  4      ,T�Ia��  ��  ��e     @ N  n2      ,T�Ia��  �  9	��e     @ O  o2      ,T��a4�  ?�  ���e      �� P  4      ,T�%aT�  ��  ���e      K� Q  4      ,T�Ia��  ��  1�e     @ R  p2      ,TD�`H   L`    �� �Rf�p   __tcfapiLocator �Dk           -  �-  ���-��c���2 -  �(Sb��        u`����Da�� L� ��c       @    �e    @ S  q2      ,T<�`2   L`    �R0�v   __tcfapi�Di          -  �-� � ��b�U�   (Sb��        �`����Dab� �� ��b       @ �e    @ T  r2      ,T�
Iav�  Z�  ��e     @ U  s2      D,T�Iae�  �  ���e     @ W  t2      D,T��`�   ,L`    � �� !� �Rd���	   timeoutMs    a��	 a� a��D|@      ������~�Z��������������i��  ���2 2~22-�
��� �2�-����2�2�  (Sb��        �
`����Qb      Vj.la`� B� ��e       ,� ,P �    �e    ��Y  4      ,T�
Ia��  ��  Y�e     �� Z  	4      �D,T�Ia��     Rf��2Y   removeEventListener ��e     �� ^  
4      ,T��`n  DL`   0Sb��        !�`�� �`�����aP  � � � ��+ ���Rbv-4�   ja  ,�a       �C �C�C�+C ��Rc�x�d   tcunav   �Ii,T4�`(   L`   ���Dg          -  � �� �^���(Sb��        I`����DaF ^ �+�b         �e     K� `  �5      ,T0�`    L`   ��,T�
Ia�  �  I��e      K� b  �8      �Df          -  Ł �^���(Sb��        I`����Dal � ��b        �e     K� a  �5      �D�X          � �%- %-�-�-�-���-�
�� ��  �})�3��3��3��3�b���-��	�-�
�-�k��b���> ��  ��c��� �-�"�� �^���$�-�&���^���(�-�*�^���,� �,�i.      P P ��
��"P @   �e     K� _  4      �+�+�+,TĖ`B  $L`    � ���+,�a       �C �C�C�C�+�D�@          -  �-�-���-��� �})��3� 	�3��3��3�b��� � ���b���c���� ��a�� ���c���Ę � �b���c���� K�!��b��!�b��#�   (Sb��        %
`�����a� � (�h%       P L�� � �     �e     K� c  4      ,T��`�  8L`    � ���+�
 ���0RjFE!   Unhandled AutoGdprFeatureStatus:     �� � ��D�P          -  �-�-�-�T� �� ����i���b��
��TØ���� ��b���-��P-��]��Ř@X��c����2X��c����l�����ŋ!���8��b����ŋ# ��	�-�
� &�-� ���\��"��T����������� (Sb��        9`����Da� * $�g$       P @ P @ � H @ �e    @ d  u2      ,TT�`f   L`   �]
`    Lb           �Do(          !  �-��z%�� �a�6��� �c�	6��^���� (Sb��        �`����Da@ � ��d       0	�    �e    @ e  v2      ,TD�`B   L`   �,Tp�`�   L`   �a      eC��� ���Dv0           ��a� �} )�3�� ���|���`���-��^��
-�� ���b����4��� (Sb��        I`����Da� h �d       L&@ � �e     K� g  �5      i�Dk          !  ā ��i���-�� �^����   (Sb��         �`����Da� � ��c      P    �e     @ f  w2      Q,,T@�`<   L`   8Sb��        a� !�a�   $   a
`����Da�   �,TP�`^   L`   RdVj�
   setTimeout  Rb�.Za   tcto�Dn8          �-�  �� ��K �!��b���b����\��� (Sb��        I`����Da�  },�c
       @   �e     K� i  �5      �Dj          � �%%! ā ��i���  ��b        �e    @ h  x2      �,,TH�`J   L`   �Rcz%~�   tcnull  �Dl          � J�b� � K�! ��b���b���   (Sb��        U
`����Da6 � �c       @    �e    @ j  y2      ,T�
Ia� � e	��e     @ k  z2      ,T�Ia� � A��e     @ l  {2      ,T��a� � ���e       E� m  4      ,T�a  ���e       E� n  4      ,T4�`(   ]�Dg           h� 	���`�� �(Sb���          ��`����Da� � �b          �e     �� o  4      ,T4�`(   ]�Dg           h� 
���`�� �(Sb���          �`����Da� � ��b          �e     �� p  4      ,TH�`L   ]�DlH      ��������Z����������������j�� ���  (Sb��         �`�����av v ��b          �e      E�q  4      ,T�
)a� � ���e     �� r  4      ,T�Ia� 5 y��e     @ s  |2      ,T�}aM p ���e     �� t  4      ,T�Iau � ��e      �� u  4      ,T�Ia� H  a���e      �� v  4      ,T��al w ���e      �� w  4      ,T�
a� � ���e      �� x  4      ,T�
Ia� � Rd��!
   getVersion  ��e   
   �� y  4      ,T�Ia	 h	 d��e     @ z  }2      D,T�Ias	 �	 )��e     @ |  ~2      ,T�Ia�	 �
 ���e     @ }  2      ,T�Ia�
 / �e     @ ~  �2      ,T�Ia: � ���e     @   �2      ,T��	a� � ���e      �� �  4      ,T�
�a & ���e      �� �  4      ,TD�`H   ]�Dk0           �i��  �a��D�b�����`���(Sb��        �`����Daf � �b       @ �e    @ �  �2      ,T�Ia{ � I-��e   
   �� �  4      ,TH�`L   ]�DlH      ��������Z����������������j�� ���  (Sb��         I`�����a� � ��b          �e      E��  4      ,T�
�a� � ���e      �� �  4      ,T@�`:   ]�Dj(           �i��  �j�����`���   (Sb��        �	`����Da� � �b         �e    @ �  �2      ,TH�`N   ]�Dl8      ������Z��������������i��  ���� (Sb��         `�����a  6 ��b          �e     ���  4      ,T(�`   ]�Dd           8 �  (Sb��        I`����Da� � ��b          �e    K� �  4      ,T�
Ia� � ���e      @ �  �2      ,T�Ia�  ���e     @ �  �2      ,T��a  ���e       E� �   4      ,T�Ia/ � q�e     @ �  �2      ,T�Ia� i З�e     @ �  �2      D,T�Iat  I	��e     @ �  �2      ,T�
Ia! � ���e     @ �  �2      D,T�Ia�  �	��e     �� �  !4      ,T��a  � ���e      K� �  "4      D,T��a�  ��e      K� �  #4      ,T�Ia6 P  ����e      �� �  %4      ,T��a% % ���e       E� �  $4      ,T�Iaj �  ����e      �� �  '4      ,T�qaY Y ���e       E� �  &4      ,T�
5a� � ���e       E� �  (4      ,T�Rb�6�w   Ok  a� � e.��e       E� �  )4      ,T�Ia� 7 ���e      @ �  �2      ,T�IaB � e�e     @ �  �2      ,T0�`   ]�Df          /�/9� �   (Sb��        I`����Da&0 F0 ��b        �e     K� �  *4      ,T(�`   ]�Dd           / �  (Sb��        I`����DaR0 `0 ��b          �e     K� �  +4      ,T�
IaR � ��e     @ �  �2      ,T�Ia� c! M��e     @ �  �2      ,T�<a}! �! ��e      �� �  ,4      ,T�Ia�! �! Rd.W��   getTimestamp��e      �� �  -4      ,T,�`   L`   ��De          !  �b��   (Sb��        I`����Da�C �C ��b         �e    K� �  .4      ,T,�`   L`   ��De          !  �b��   (Sb��        I`����Da�C �C ��b         �e     K� �  /4      ,T�
Ia" 9" a	��e     @ �  �2      ,T�IaF" �$ ��e     @ �  �2      �D,T��a�$ �$ ���e     �� �  04      ,T�Ia�$ %  ����e      �� �  24      ,T��a�$ �$ ���e       E� �  14      ,T�Ia*% D%  ����e      �� �  44      ,T�Rb��   $k  a% % /��e       E� �  34      ,T��a`% �& ���e      K� �  54      D,Td�`�   L`
   -R�d�   __uspapiRҭY   __uspapiLocator %�Ds          ��-�  l��8��-��/��-��-��! F��i�� 	� ?�b�� @�b���(Sb��         �`����Da*M �M �d       �
� �   �e     @ �  �2      ,T�
Ia�& �' u��e     @ �  �2      D,T�Ia�' ^( ���e     @ �  �2      D,T�Iak( �( ���e     @ �  �2      ,T�Ia�( �* ���e     @ �  �2      D,T�Ia�* �* Q	�e     @ �  �2      D,T�Ia�* - 
��e     @ �  �2      D,T�
Ia- �- U��e     @ �  �2      ,T�Ua      ��a      � Rf�g1   google_ad_channel   a�- �/ %�e      �� �  64      ,T��a�/ 70 ���e      K� �  74      ,T�
�a;0 v0 ���e      K� �  84      D,T��a�0 ?1 ���e      K� �  94      D,T�!aC1 �1 ���e      K� �  :4      ,T��
a�1 2 ���e      K� �  ;4      ,T��a!2 L3 ��e      K� �  <4      D,T�aV3 �3 ���e      K� �  =4      ,T�
a�3 �3 ���e      K� �  >4      ,T�Ia�3 w4 ���e     @ �  �2      ,T4�`(   ]�Dg           h� ���`�� �(Sb���          ��`����Da i Ni ��b          �e     �� �  @4      ,T,�`   ]�De          r��c�� �(Sb���          �`����DaPi xi �b          �e     �� �  A4      ,TH�`L   ]�DlH      ��������Z����������������j�� ���  (Sb��         D`�����a�h �h ��b          �e      E��  ?4      ,T�
Ia�4 �4  ����e      �� �  C4      ,T�Ia�4 5  ᪒�e      �� �  D4      ,T��a�4 �4 ���e       E� �  B4      ,T,�`   ]�De          s��c�� �(Sb���         E#`����DaZj �j ��b          �e    �� �  F4      ,TH�`L   ]�DlH      ��������Z����������������j�� ���  (Sb��         1`�����a0j 0j �b          �e     E��  E4      ,TH�`L   ]�DlH      ��������Z����������������j�� ���  (Sb��         Y
`�����a�j �j ��b          �e     E��  G4      ,TH�`L   ]�DlH      ��������Z����������������j�� ���  (Sb��         �`�����a�j �j ��b          �e      E��  H4      ,T4�`(   ]�Dg           q�n��c�� �b���(Sb���          ��`����DaRk �k ��b         �e     �� �  J4      ,T4�`(   ]�Dg           q�n��c�� �b���(Sb���          �`����Da�k �k ��b         �e     �� �  K4      ,T,�`   ]�De          r��c�� �(Sb���          !�`����Da�k �k ��b          �e     �� �  L4      ,TH�`L   ]�DlH      ��������Z����������������j�� ���  (Sb��         �`�����a0k 0k ��b          �e      E��  I4      ,TH�`L   ]�DlH      ��������Z����������������j�� ���  (Sb��         e`�����a�k �k �b          �e      E��  M4      ,T<�`2   ]�Di(          w� Q�� Y��`�� �   (Sb���          �`����Dafl �l ��b          �e     �� �  O4      ,TH�`N   ]�Dl8          g� Q�c� Y���`�� ��`��U� (Sb���          a�`����Da�l �l ��b         �e     �� �  P4      ,T�
Ia6 �6  ����e      �� �  Q4      ,TH�`N   ]�Dl8          g� R�c� Y���`�� ��`��U� (Sb���          !�`����Da8m �m ��b         �e     �� �  R4      ,TH�`L   ]�DlH      ��������Z����������������j�� ���  (Sb��         �`�����aDl Dl ��b          �e      E��  N4      ,T<�`4   L`   9�Di          }� Z� ���c���  %Z�  (Sb��        �`����Da�m "n ��b          �e    @ �  �2      ,T4�`$   L`   $R ��G�   google_ad_modifications �Dg          -  ��~�2� ��  (Sb��         �`����Da8n �n �b       ,  �e  
 @ �  �2      ,TT�`f   $L`   Re6�X   space_collapsing$Rg��b\   remove_ads_by_default   $�a      �G	CuC�$Rg'g�   ablation_viewport_offset��Do           \�b� - ��-�})��3�-	3���� (Sb��        �`����Da�n �o �d       P ��    �e    @ �  �2      ,T�
Ia�7 g8 �
��e     @ �  �2      ,T8�`*   L`   (Rhk�;   allow_second_reactive_tag   �Dh           \���b�� �2� �   (Sb��        E`����Da�p .q ��b       ,  �e    @ �  �2      ,T�Ia�8 �8 I��e      @ �  �2      ,TH�`P   L`    Rf*d�K   head_tag_slot_vars  ReR�}   google_ad_host  �Dl           \�b� ş-� ş-���� b�b��(Sb��        �`����Dar �r �c       P    �e    @ �  �2      ,TP�`Z   L`
   Y]8R4��,   meta[name="google-adsense-platform-account"]iRc�|��   content �Dn           -  ß!-���^���ğ-���^�������   (Sb��        �`����Da�r �s ��c
       @   �e    @ �  �2      ,T|�`�   L`   0Sb��        a�`�� �`�����a�s Nt I,T�
Ia: &: I�1�e      K� �  �5      �Dy@   (       � �%�%�������l� � ��e  �c����� ��i  �� �����`��� ��b      �  �e     K� �  S4      �1D,T��`B   L`    a� � !�I ��y�D�          ��-  �]��r�-�]���c����y-
�]��m�l��c�-�]��-��]���-�l��C-�]��-��]���/� �l�"�#r�-�]�#�-�%�]��'��c���)���   (Sb��        E
`�����aVt u ,�i+       P P �� 
 P  	�     �e     K� �  T4      ,TD�`D   L`   RBH�T   tmod�Dk          s��c�� ��l�U�
2 �  (Sb��        �
`�����a&u ru ��b       � �e     K� �  U4      ,T�
Aa�: ; ���e      K� �  V4      �D,T�Ia; �; q	��e     @ �  �2      ,T|�`�   L`   @Sb��        �� a� �b$  �   �� `����DaPw ^x ��,T�Ia�; -< I2�e      K� �  �5      �Dy            � �%%%�� �b� � ��j  �b�%��� �!��b���b��%�� �Á �c���
���c      ` @ �e    @ �  �2      2,TL�`X   L`   0Sb��        a�`�� �`����Datx  y ,T�
IaU< �< I)2�e      K� �  �5      �Dm           � �% ��k  �b� ~%�� ā �c�����b        �e    @ �  �2      -2,T��`�   (L`   9�a
       �C �C � � ���	�D~           �� e�b�� �S}� Z� �~��c��� �})�3� Z3�b��	-� l�-�]��b���~� l�-�b���(Sb��        5	`�����a.y �y �e       0�
� �  �e     K� �  W4      ,T�
Ia�< �= Y��e     @ �  �2      D,T�Ia�= > a��e     @ �  �2      ,T�ea%> W> ���e      K� �  X4      ,T��`  ,L`   | a� � ��� !�y	Rb���   host�D�0           a��b�� �	 c� ���-��]����zu�-�]��	�c���
���%-��]��-��]��ß-��]����k���	 c� � n���b��_��B��`�����-��]�������-��]��!ß-�#�]��%�Ř;�-�'l�)�/�+/�*�l�,�u��c��-��-�/�-�1k�3�	 c� ���1-��]��4��%-�6�]��8-�:�]��<ß-�>�]��@��	 c� � n���b��B_��B��`��D� c� �   (Sb��        `�����a�|  <�mF       @ @ @ 	��� 
��"D
��H P @   �e     K� �  Y4      ,T��`�  L`   |�����D�(          | � c� � �� u�b���" �� t�b��� �� v�b��� o��c�� �� u�b��	�-���^���T�-���^��� �� t�b���-���^���T�-���^��� �� v�b���-���^���T�-���^�����   (Sb��        a`�����a j� �$�g       � � 
��� �     �e     K� �  Z4      ,TH�`L   L`   ��Dl0   (        r� z� ������i�� �b���  (Sb��        �`����Da�� �� �b         �e    @ �  �2      ,T��`�   $L`   0Sb��        !�`$   �`����DaԀ R� ��a
       �C �C � �� �,T�
Ia�@ �@ I�2�e      K� �  �5      ,T4�`$   ]�Dg           ��t ��b�� �  (Sb��         I`����Da8� N� ��b          �e      K� �  �5      �D}@          � �% ��O  � ��N  � ��  �} )��-�3��r ��-���c���3�	b�� �c���Á�c����   ��d      � 
� �  �e    @ �  �2      �2�2,T�Ia�@ [A U�e     @ �  �2      �D,Td�`�   L`   0Sb��        !�`$   -`����Dâ L� �,T�
IavA �A I�2�e      K�   �5      ,T4�`$   ]�Dg           ��w ��b�� �  (Sb��         I`����Da2� H� ��b          �e      K�   �5      �Ds(          � �% ��O  � ��N  � ��v  ��b��  �c���Á�c���� ��b      @ �e    @   �2      �2�2,T�Ia�A �A q
�e     @   �2      ,T�`�  4L`    � ��Rc�lF�   invtag   !� a� �y	u2`  �D�H           a�-  �b���*r�-��c���� K�!��b��
�b���-  �-�-��-��]������%-��]����-��]����-��]���Ř]�i� a����`�� �-�"m�$�1 �� m�b��%�.�i� b����`��'�-�)m�+� m��b��,�Ō
��-��]��.�����-��]��0��-�2�]��4���1-��]��6��%-�8�]��:��-�<�]��>��-�@�]��B������-�Dl�F��/�G��l�I����|�i� a����`��J�-�Lm�N�1 �� m�b��O�M�i� b����`��Q�-�Sm�U�.u��c��V�-�X�-�	Zl�\� m��b��]�������� ��i�� _�i� a����`��al�����`��c�i� �����`��el�����`��g� �� m�b��i�d�i� b����`��k�-�mm�o�E c�i�� pi� b����`��r�l�����`��t�j������`��v J�b��xŋ K�!��b��z�b��|��� (Sb��        �`����Da� P� `�v~       P @ P @ P P � 
 @ @ @ P  P � 
 P D� � ��H @ @ �e    @   �2      ,Tl�`�   $L`   0Sb��        !�`$   `����Daf� Ԉ �,�a       �C �C�+`d    ��C � �� �,T4�`&   ]�Dg           ��x ��c�� � (Sb��         ��`����Qb      Vj.Ga�� Έ 93�b          �e     K�   �5       ���Du0          � �% ��   �} )��-�3��r ��-���c���3�	� 3�b���   ��d      � 
� �   �e    @   �2      A3,TT�`d   L`   0Sb��        !�`$   �`����Da� n� ,T�
Ia�D �D Ie3�e      K� 
  �8      ,TH�`L   L`   I ��Dl            ��y �-  �b���-����_�����  (Sb��        I`����Da(� j� ��c       P    �e     K�   �8      �Do(          � �% ��O  � ��N  Á �c�� Á�c����  ��b        �e    @ 	  �2      i3q3,T@�`:   L`   Re:�Q@   abg::amalserr   ,�a       �_CRc�ܜ   guarding	Rc�EE	   timeout `d   Rbi��   rateYa{�G�z�? �_Ya{�G�z�?�Dj            �� �} )�3��`���   (Sb��        �`����Da�� � �b       � �e    @   �2      ,T<�`4   L`    � �� !� ��Di    (       2  222�  (Sb��        �`����DaB� �� ��c       ,�    �e    ��  [4      ,T�
UaXE PF ���e      K�   \4      D,T��aZF lF ��e      K�   ]4      ,T�EapF �F ���e      K�   ^4      ,T�Ia�F �G ���e     @   �2      ,Tx�`�   L`   �P�Dx          ��-  �-����-�^�k��� ��b��k�	���-�^�
k����  (Sb��        �`����DaF� N� �d       @ ��H    �e    @   �2      ,T�
Ia2H nH �	��e     @   �2      D,T�]a�H �H U��e      ��   _4      D,T�Ia�H I ���e      @   �2      ,T�IaI �I 5�e      @   �2      D,T�Ia�I �I �	��e     @   �2      D,T��`  $L`   ,R(�ɦ    google_reactive_ads_global_state,Ri�ʀ   sideRailProcessedFixedElements  }$Rg:��J   sideRailAvailableSpace  5 Rf��1V   sideRailPlasParam   Y�D�          -  �g-  �-��-  �!�i�� �2��-  �-�
�-  �!�i�� �2��-  �-��-  �!�i�� �2��� ���i�� �2 �-  � (Sb��        `����Da�� Ɨ  �f       ` ,P � � ,  �e    @   �2      ,T��`�   HL`     Rf*�t:   wasPlaTagProcessed  (Rh5�`   wasReactiveAdConfigReceived Rc�Qo   adCount  Rf���   wasReactiveAdVisibleRd^
*   stateForType(Rh��
�   reactiveTypeEnabledInAsfe   (Rh��1   wasReactiveTagRequestSent   ,Ri��L+   reactiveTypeDisabledByPublisher Reħ�   tagSpecificState$Rg���C   messageValidationEnabled Rf��)   floatingAdsStacking }945=4A4�D}          2  ~2~2~2~2~2
2~2~22	 ��i�� 2
!�i�� 2!�i��  2"!�i�� $2&�   (Sb��         Y`����Ua      �Qd      .constructora� d� (�h(       ,� ,� ,� � � � ,  �e     ��  `4      ,T4�`$   L`   $Rg����   maxZIndexRestrictions    Rfh�/   nextRestrictionId    Rfz3b    maxZIndexListeners  �Dg           ~2  2|2�  (Sb��         �`�����a�� D� ��c       ,0    �e     ��  a4      ,T�
ya+N �O ���e      K�    b4      D,Th�`�   L`   8Sb��        a� !�a�   $   �
`����Da�� � �ReFX�"   ama_ran_on_page ,TH�`P   L`   ��Dl            ��� � ��� � ����i�� �b���(Sb��         I`����Daҟ �� �4�b         �e      K� #  �5      �Dt(          � �%% ��\  ��� �b�� �-�� ���  � �Á �� �`���   ��b      @ �e    @ "  �2      �4,T`�`~   $L`   0Sb��        !�`$   �`����Da� �  � � ��R�p�   google_ad_clientI,Tl�`�   L`    � ���4�Du8          �-�  ��-�� ��\ ����b���-�� � ��� �������`���   (Sb��        I`����Daj� � �4�c
       @   �e     K� %  �5      �Dr0          � �% ��q  ��-� ��-���-��-���� �`��� ��c
      P   �e    @ $  �2      �4,T4�`(   L`    � �� ᪗Dg           2  22�(Sb��        �`����Ua      $�a      �(R$jw��   google_analytics_domain_namea� R� �b       ,� �e    ��&  c4      ,T�
Ia�P pS q��e     @ '  �2      D,T��`�  �L`<   YReJ��F   getElementById  Rcn!�<   _host   Rc��o   aswift_ �R���   google_ad_width Rt7    google_ad_height�Rj?   div �uR)�   border  Rb^�   px  &�%Rb*e�4   0px R&.�   margin  RjU��   padding RcR��t   relative�%Rb/�r
   visibility  RdJ���   transparent R2���   backgroundColor Rd^���   inline-blockE�a
      �C�C���D�0          -  ����-�8��^�����u�P��8�Ŋ' 	�!
�-��b���!
�-��b��! �-���^����82�	-�
�2�w�8�!2�"�w�8�$2�%2�'2�)2�+2�-2�/2�1}3)�3�4�3�6�� (Sb��        �
`����Da�� B� 4�k8        	 �k P P �
`� p�`�`��    �e    @ )  �2      ,T@�`>   L`   �
]Rb�=QR   dev I�Dj          -  �-�����l���� (Sb��        y`����DaZ� �� ��b       �e    @ *  �2      ,T�
�a�U �V ��e      K� +  d4      ,T�Ma�V �W ���e      K� ,  e4      ,T�1a�W �X ���e      K� -  f4      ,T�Ia�X "\ E��e     @ .  �2      ,T�Ia\ (] �	��e     @ /  �2      ,T	�`�  DL`    � ��$YyR&��T   heada��Rb���(   METARd��P�   appendChild R��P   origin-trialRd�P�	   httpEquiv   �1U(Kh@   R   �   x  
   �   �   �     D�p           "� ��b�� �-� � ��-�� ��-��_�������-�
�-���-������-������]����e� �-��E-��������0-��*��	�b���-��-�
�^��� 2�"�2�$��S &���������#��-�'��]��)��e� ��������� (Sb��         `����Dah� Ȼ ,�i+       P P P @ @ � �    �e     @ 0  �2      ,TP�`^   L`   YRe.�wC   featurePolicy   Rc��6	   features���Dn(          ��!  ��-���-��]���-��^���
�x� (Sb��        �`����Da޻ \� ��c       P @ �e    @ 1  �2      ,TP�`^   L`   �6Re^<$�   allowedFeatures ��Dn(          ��!  ��-���-��]���-��^���
�x� (Sb��        -`����Dar� �� �c       P @ �e   @ 2  �2      ,TH�`N   ]�Dl8      ������Z��������������i��  ���� (Sb��         `�����aD� Z� ��b          �e     ��3  g4      ,T�
Ia�^ �^ I-��e   
   �� 4  h4      ,T8�`.   ]�Dh           Z��G�b� ��`��� (Sb��        �`����Da�� � �b         �e    @ 5  �2      ,T8�`.   ]�Dh           Z��G�b� ��`��� (Sb��        `����Da�� .� ��b         �e    @ 6  �2      ,T8�`.   ]�Dh           Z��G�b� ��`��� (Sb��        }`����DaD� x� ��b         �e    @ 7  �2      ,T8�`.   ]�Dh           Z��G�b� ��`��� (Sb��        �`����Da�� ¾ ��b         �e    @ 8  �2      ,T8�`.   ]�Dh           Z�	�G�b� ��`��� (Sb��        y	`����Daؾ � �b         �e    @ 9  �2      ,T4�`$   ]�Dg           l�
���`�� �  (Sb��        �`����Da"� R� ��b          �e    @ :  �2      ,T8�`.   ]�Dh           Z��>�b� ��`��� (Sb��        `����Dah� �� ��b         �e    @ ;  �2      ,T8�`.   ]�Dh           Z��G�b� ��`��� (Sb��        �`����Da�� � ��b         �e    @ <  �2      ,T8�`.   ]�Dh           Z��>�b� ��`��� (Sb��        a`����Da�� 2� �b         �e    @ =  �2      ,TH�`N   ]�Dl8      ������Z��������������i��  ���� (Sb��         =`�����av� �� ��b          �e     ��>  i4      ,T��`,  ,L`   0Sb��         !�`�� �`����Da�� �� �iReں��   userAgentData    Rf
�q�   getHighEntropyValuesR�byA   google_tag_data Rd�'��   uach_promise�i,T�
Ia�a �a I�6�e      K� @  �5      �D�0          � �%�� �-� ß-�ß-�� ���� �-����� �~�2��%�-�
�
�-�
��� �-� �-��-�� ���  ��^����-�Á �^������2���  ��e      P ,P @ � �e     @ ?  �2      7,T��`�  8L`   R"?<S   architectureIRRW�   bitness Rc��h   mobile  R����   model   RF��O   platformR�&��   platformVersion R�E��   uaFullVersion   R�b��   fullVersionList ��,Td�`�   L`   Rc�.ʗ   brand   Rcb30   version �Ds0           ��i��  �Z��G�- �b�����`���Z��G�-�b��
���`��� (Sb��        I`����Da�� 
� �d       @ @    �e     K� B  �5      R^�   wow64   �D�X           �� �� �� �� �� �� �� �� �� ��i��  �- ��c����-��c����-
��c����-��c����-��c����-��c����-��c����-��-�	 
 �^���"��|$�c���%�-'��c���)�  (Sb��        �`����Da
� 2� �,�i+      @ P @ P @ @ ��    �e    @ A  �2      E7,TD�`F   L`   i,T,�`   ]�De           ��b� �  (Sb��        I`����Dar� �� ��b          �e     K� D  �5      �Dk           ��a� ğ-� Ł �^������� (Sb��         �`����DaH� �� ��b      @ �e     @ C  �2      y7,T@�`:   L`   �1�Dj          -  � b�b��
2 �   (Sb��        �`����Da�� � �b       � �e    @ E  �2      ,Tȕ`J  4L`   IR�8�?   google_sa_impl  YI5R���S   google_shimpl   R�q:v   google_sa_queue  ���,T4�`$   L`   �7�Dg           ����-�  �b���  (Sb��         I`����Da�� �� ��b         �e      K� G  �5       R�?�   google_process_slots��D�@           ��� ����-� �(��-��-���^���T���W���W���-��\��|
2��� ���-�� � �� �_����2�	 ��������`�������-��c�����2�
��   (Sb��        �`����Da0� T�  �f      P 0� �     �e    @ F  �2      �7,T��`�   (L`   0Sb��        !�`$   �`����Daj� 8� �1� ��I
y�,�,T4�`"   ]�Dg           ��� ��b�� �   (Sb��         I`����Da� .� �7�b          �e      K� I  �C      �D@          � �%�-� �]��� � ���  ��-�� � �_�����-��5� �-�
� ���  ��-�� � �� �_������_����� ��d      P P @ �e    @ H  �2      �7,TL�`X   L`   �7�7��Dm           -  ��|2� -�b�-  �-�	�^���(Sb��        �`����DaR� � �d       �
��    �e    @ J  �2      ,T̐`V  4L`   ���a
      Rc���   client  CRbF�   plahC%8	u2)8(Rh���X   PublisherCodeNotFoundForAma �a      Rb*��   bustC58�D�            r��c�� �- �-~�r��c���D� ����b���$}
)�3���-��-�3����!��b���~ ��c�� �� ��a��}	)� ��a�3�
��~�c��� ��c��"� (Sb��        `����Da(� �� $�g$       P 0��� ��I @ �e    @ K  �2      ,T<�`4   L`   0Sb��        a�`�   �`����Da�� 4� �,T�
Iaxf �f IM8�e      K� M  �5      �Di           � �%�� ā �c�� �  ��b         �e    @ L  �2      Q8,Tu�`�  TL`&   ]
`    La       -��yR�c�
   google_esf  eSbJ �         ��`�� R>Iu�   IFRAME  �����a      Rcf�V   hello   Rc��    world   	IuE(Kh@   9   {   �     �   �   �     DaP          z  %���-�6����|����u�P��/���[-�	�^�����-��>�-��-�m��.  �-�n�� -��-���/��^����P��6���e ���-�m��9���/��-�!�-�#Ø���%��$����������P%Ċ< &����%��������b��'�2�	)2�
+ �� x�b��-�-/�-1 �� q�b��3���}5)�c��6��b�8�-�:�]��<2�>-�@�2�B��   (Sb��        �`����DaN� �� <�mD       ��&���"�� x/�
 ^`
� &P ,P    �e   @ N  �2      ,Te�`�  XL`(   @Sb��        a� !� a�b�   $   �� `����Da�� n� ����5 Rf�G��   google_start_time   A�2�a
      �
Rer�K   m202309140101   ]C�RdF�@
   google_lrv  $Rg�k>   google_async_iframe_id  Rd�Z'�
   google_bpp  R2��   google_sv_map   YI5M5,T�
Ia�i �i I�8�e      K� P  �5      ,T�Qb      In.da�i �i I��e      K� Q  �5      ,T��`  HL`     a�Rd*��   isConnected ����8�Rc�G>$   no_div  �7$�a      Rc�ң   pubWin  CRb����   varsCRe��)�   innerInsElement C�8�8�8 �� a��D�(          � Ř-� �;�-��-��!��-�	�b���8��^�����!��b����-�	�}
)�3�3��3�^���Ř ��� ��-�� ��_���� �  (Sb��         I`����Da�� f� �8$�g"       P Ѐ� ��� 0@   �e      K� R  �5      �DQH   (       � �%%% ���  ���c��� �-��-�%-��^�� ���  ����`��
�-���� �!�i�� �-��]���� ���  �})��u ��c��3�	b��2�
��2��2�!�n�#�	9�$�2�%�-�'�~2�)�-�'�4��+ ���  � ��p  �b��-��-�/�-�1�8�3�^���4�� �� ���  �����`��6� �4�k8      P @ @ 0	�`�"p��
� H    �e    @ O  �2      �8�8�8,TM�`X  �L`L   Ref���   google_ad_outputRe��pw   google_ad_formatU5Y5Rb���N   html �ReB��D   google_ad_slot  $Rg�SQ   google_override_format  Rb
��   aa   Rf���   google_loader_used  ��I(Rh� ��   google_reactive_sra_index    Rf�8   google_ad_unit_key  ]
`    Le
                       $Rg.��~   google_orig_ad_format   ReQ�   google_ad_type   Rfv�P>   google_orig_ad_width$Rgr��X   google_orig_ad_height   Rc¾��   nodeType���q �, a�	 ��Rc>-?   nodeName9%RdJ��
   childNodes  y�`  SbJ �         �`�� (Rh�ښ"   google_ad_dom_fingerprint   `"  Kd    �  �  �  
   D9�           -  �-�-��-������l�U��U�8�	��8�
�-T�/-�) ��-�8��-8�/�T��-	l������-�
�]��ċ��2-  �-�%z %��-6��!�-#�-6��!�-%6��!�-'�-6��!�-)�-6��!��|+������D��n�,�:	�-�-l�/�-�0��^���2�-�0�-�4�^���6-�8��P:ÊB ;-�<�]��>Ř-�@�^���B��-�D��^���F�b��H�-�J�]��L2N|P��� ��n�Q��	�-Rl�TU�-UØ��8�W����-X��-Z�~-X�-�\�]��^�-�
`�]��b�-Z�-�d���-�fm�h�N�/�i�-�k�9-�k�-�m�]��o�-�
q�]��s��l�u��lv���8�w�Px��Py��Qz�-�@�-X��-X�-�{�]��}�-�
�]�큹�8�8^���-Z�P�� �  � -�D�]����8��|���p��-!�����Sl���L��nB-�"���-��m���!�/��l��-�@�^������P�Ê$��-!���P�ŊQ �����#�������-�D�]���8��b��-���]��2$��(Sb��        �`����Da�� �� ���~�       P D� ��X ,P �
��
`�"P  /P @ @ �X � X @ P D
� 
��!��  � x!�"��"D ^H @    �e   @ S  �2      ,TX�`p   L`   y4�a
      Rd�F��	   debugCard   F Rf�g�   debugCardRequested  H�Dp           ���b�� Ř+ ��b���-� �/��-� ��})�4��	��(Sb��         �`����Da�� �� �c       P L �e     @ T  �2      ,TH�`L   L`   i,Tx�`�   $L`   )b/�/	a      ,R(�Uc�   google_user_agent_client_hint   Kd       0   �     Dx@          �8� �%8��! �-��-�]��M�_������ċ����%8���� ��������2�
�  (Sb��        I`����Da6� "� ��c       P � �e     K� V  �5      �Dl            ��a� �U�-� ā �^��� ��a��  (Sb��         E`����Da�� 0� ��c      @    �e     @ U  �2      �9,T0�`   L`   0Sb��        !�`$   �	`����DaH� z� ,T,�`   L`   ��De          - �@� x�(Sb��        I`����Da^� x� �9�b       �   �e     K� X  �5      �Df          � �%� �  ��a       �e    @ W  �2      �9,Tl�`�   L`   ���Duh   (   ������������Z�����������������i�� ����2 �2�(Sb��         !�`����Ua      �
�a      �ReP�$   image_stacked   a�� � �b       ,� �e   ��Y  j4      ,T�
Ia�q �q M��e      �� Z  k4      ,TX�`j   L`
   Re�\?�   google_ad_resizeu&e5Rb&��   rpe �Dp           -  � �� r�b���-�-�]�	�8�2�2�   (Sb���         �`����Da8� �� �d       P � `  �e    �� [  l4      ,T�
Ia�t 5z ���e     @ \  �2      ,T�IaBz { 	��e     @ ]  �2      ,T��`z  �L`l   �(RhN�;�   google_responsive_formats    �0Rjn}��#   google_safe_for_responsive_override  �0Rj�4$   google_full_width_responsive_allowedRcf[�   gfwrnwer !�RcΞ��   gfwrnher !�U5$Rg�K��   google_resizing_width    a�Y5$Rg�E�   google_resizing_height  � ��&: �9 a�,RiΠ��   google_responsive_auto_format   RbnƵ=   armr Rfj)!\   google_ad_resizable !9(Rh���   google_loader_features_used e5Rc
1��   gfwrnh   !�Rc6�m   gfwroml  a�RcN�X�   gfwromr Rc�̮=   gfwroh  Rc�W�   gfwrow   �Rc�p��   gfwroz  U	(RhN���   google_responsive_dummy_ad  ]
`    (Mh               
            b/�/�a
      Rd�T��   googMsgType Rcv���   adpnt   Rd^�D�	   key_value   �`   L`   �a
      Rb~sp   key Rb�R�   qid �RcRh|O   DUMMY_AD ��(Rh�M�<   >window.top.postMessage('   $Rg�d��   ', '*'); 
          </  8Rl^��,   > 
          <div id="dummyAd" style="width:Rdr^@ 
   px;height:  �R�~�ˈ   px; 
            background:#ddd;border:3px solid #f00;box-sizing:border-box; 
            color:#000;"> 
            <p>Requested size:0Rj�!��#   </p> 
            <p>Rendered size: $RgF���   </p> 
          </div>  Rb�x�   dash�D�0           -  U�-  �2�-U�-�2�-U�&-����2
��2
-�2�-U�-��U�-�2�-	�-
�U�
�2�-�-�U�
�2�-�]� �-�"�^��$�-�]�&�-�(�]��*�-,� �� r�b��.���2
0�22-�]�4�-�6�^��8�8�:�-�;�]��=8�?2@-B2D-FU�-F�2H�2J2L � 2N-���"-�]�Q�-�S�]��U�8�P�2W�-YU�-Y�2[�-]U�-]�2 _�-U�-�2!a�-	U�-	�2"c�-#eU�-#e�2$g���!%i�b��k�!%i �-&m�'�c���o��&�z(q%�-B�c���r��-Fl�t���-Fl�uU��!)v�-�*x�}+z�^���{,� ��-w8�}�.8�}�w8�}�/8�}� �w8�}�08�}��w8�}�18�}��w8�}�28�}��w8�}�8�}��w8�}�38�}��w8�}�8�}��w8�}�48�}�25~��   (Sb��        L`����Da`� � d�w�       ,P � � � � P P @ ,@  � � ,P ,� @
��
`�`�
`� &�h L    �e    @ ^  �2      ,T��`h  8L`    a��� �� � � !� !� a� a� !� ᣐD�`   h       ��������������	��	�
��
��������������2  �2�2�2�2�2
�2�2�2�2	�2
�2�(Sb��        �`�����a� @ ��e       ,� ,� ,� ,� �e    ��_  m4      ,T(�`
   L`   ��Dd           -  �   (Sb���         �`����DaH l �b          �e     �� `  n4      ,T�
Ia[� ��  ����e     �� a  p4      ,T��aI� I� ���e       E� b  o4      ,T��a�� �� ���e      K� c  q4      ,T��a�� H� ���e      K� d  r4      ,T�IaT� � X��e     @ e  �2      ,T�Ia#� �� =�e     @ f  �2      ,T�IaȈ و  ����e      �� g  t4      ,T�Iaڈ 3�  ᪕�e     �� h  u4      ,T�Qd      bo.image-topa�� �� ���e       E� i  s4      ,T�
U;aK� i� Rd��2�	   image-top   ��e      K� j  v4      ,T�Ua      U�a      �Rd�'��   image-middleay� �� u;�e      K� k  w4      ,T��a      �Qd      .image-side a�� ؉ Rd���
   image-side  ��e      K� l  x4      ,T�Qd      bo.text-onlya� � RdbB&2	   text-only   ��e      K� m  y4      ,T��a      �Qd      .in-article a� A� RdB�\P
   in-article  ��e      K� n  z4      ,T�Ia^� ~�  ����e      �� o  |4      ,T��aL� L� ���e       E� p  {4      ,T�
�a�� �� ���e      K� q  }4      ,T0�`   L`   0Sb��        !�`$   �`�����a:! �! �,TT�`b   L`   y�Do          �-� E ���o����/��^�����QŊ  	�   (Sb��        I`����Da@! �! �;�c
       ��� � �e    K� s  �5      �Df          � �%� �  ��a       �e     K� r  ~4      �;,Tt�`�   L`   �敖Dw@           ��-�  ��^����-�����m��.�/��b�	��b����������P/ ��   (Sb��        `�����a�! �" �d       P  ^  �e     K� t  4      ,T�`�  @L`   =�%�a
       ��C �`    ��Rd�f��   autorelaxed :�a
       ��C �C ��a
       ��C �G�a
       ��C �`   �a
       ��C �C9%�a
       ��C �C�a
       ��C �C�D�8   0        �- k��})�3������k��- � ��b��-	�P ������`���})�3�3���})� ��b��3���}	)�3���-�-���}
)�3��3�!��0-#�}%)��3�&�3�(��}*)�3�+�3�-��(Sb�� 
       5`�����a& @( ,�i/       �� ��`2� � &P �
`2� � �e     K� u  �4      ,T��`�  PL`$   PSb�� 
   
    !� � �� a� !�d$  �  $  �   $   9`�����aH( �* �,T@�`<   ]�Dj0           ��� ������`�� �  (Sb��         I`����Da�( �( E<�b          �e      K� w  �5       �� �u�%& �� !�MRb��J�   Wa  R�
��
   marginLeft  e5IR��	   marginRight RV�   zIndex  ��Dq�   0       � �%%%%% ���  � � �� �c��� �-��-����� ��-��-��b��
�� ��-��-��b��� ���  ���������`���-��-��-�	�-�
� ���  ���c���� ���  ����� �`����	8��� ���  ����� �`����	8�!�� ���  ����`��"�� ���  ��������������������������i��$�E<(�h&      P @ @ P @ � H    �e     K� v  �4      I<,T<�`6   L`   <Rm"S�-   ^((^|,) *(horizontal|vertical|rectangle) *)+$   R�Di           �k� �y  �-��^��� (Sb��        �`�����a�* V+ �b       q@ �e     K� x  �4      ,T��`  lL`2   XSb��        a� !� � �� !� !�e�  $  �  $  $   �� u`�����a^+ t3 ��Ya      �?0b6]
`    Lb           ��(Rh�   google_max_responsive_height�`    La       ,T(�`   L`   ��Dd           -  T�  (Sb��        I`���� !�a|- �- �<�b          �e     K� z  �5       a�,Ri����    No slot size for availableWidth=,T��`  lL`2   q:�a
       ��C !�G� ��Ya333333�?0b6$Rg��'�   google_resizing_allowed 	�%�&'9%'	'��Ya      �?�a
       ��C !�C! !�aP  �  �a
       ��C !�`�    !��D�X          ��-�  �>����-�  �b��Ř*})� ��� ������i��3�����;�	�!
�-���-����-�	k��!
��!
� ��� ��������`����!
�-��_����� ��� ��������`����!
�-��_�����-��-�!k�#��q $���-�%���������\��'���:�)m�*�d��n�+����},)� ��� ���!
�-�-�^���/��i��13�3�m�5�f�3�6����}8)��3�9�����  (Sb��         I`����Da�. Z2 �<4�k;       0	�h P � 
� ��"�  1�� "L �e      K� {  �5       �� !��;Re��   google_ad_layout	ReB�dR   #hffwroe2etoq   ,�a       ��C !�HMCY<C%��,�a       ��C !�C�C�C�D�x   8       � �%%%%%%�k� �=��!�-�� �� ���  ��b���_����;�p�
��� ���  �b���� �� ���  ��b��n�9 ���  ���c���� ���  ���c����T�������z%�� ���  ��b��6��� ���  �b�6����-�� ���  �������`���^����-�!U�"-�� ���  ��-�!�b��#�^���%z'%���	 6��(������/ ���  ���c���*�-�,� ���  �b��.�^���0 ���  � ���  �b��2� ���  �b��4�c���6%� ���  �
��8�8��i��9� ���  � � ���c���;�-�=�-�?���-�Al�C�W�-�D�N��-�D�-�Fk�H�:}I)� ���  ��������`��J3�L3�N�3�P�!}R)��3�S�3�U3�W�3�Y��<L�q[      P 	��H L������� @ @ @��
 P q� 00�    �e     K� y  �4      �<�<,T��`�   0L`   f            *   $   0   Rc��{   bad mask�D{           �k� ��  ��+   ��p�����?�o���J �l������������!�	�b���  (Sb��        `�����a�3 �4 �c	       1F�   �e     K� |  �4      ,T�
%a{� � ���e      K� }  �4      ,T�Q
a� Z� ���e      K� ~  �4      ,Ta�`z  \L`*   9<Rc,\   pedestal4Rk�A4�%   google_content_recommendation_ui_type   �a��Ua      A�Rc`�U   fluid   �;�<	Re6�T1   #hffwroe2etop   �<Q�#(Kh@   1   g   h  	   z   �   x  
   DMh          -  ���k����-k��� ������-�
�����]����e� �-��$-��������/�����2 ����������#��-���]����e� �������	 ���	�� ��b������l�����k��H�-l��*- �$�- �-�"k�$��- �-�%k�'�� ���b�(��-*l�,� ���b�-��   (Sb��        I`�����a�8 < ,�i/       �"P P ��� P ��H D �e     K�   �4      ,T�`�  8L`   Rd.��   offsetWidth :�%U59q� �&ue5�D	`   0       �������-�  �+-���� ���������`���	-�����l��2�	� �������������`������ ��-����������`���-��]���-��������\�� ��������`����k�U�'-��]���-�	�]�� �-�
"�8�$�2�	%��   (Sb�� 
       �
`�����a< 4> (�h'       @ �� 
����H P p  �e     K� �  �4      ,TU�`h  (L`   HSb�� 
       !� � �� a�c$  �  $  �   q`�����aL> A �Y5&,T�
Ia�� �� Iy=�e       K� �  �5       �� �M:Q:�DAX   0       � �%%%%�-� � ���  ������ �`������l���	l���l���
l����� ���  � � �� �c����-��-�	���!��k�U� ���  ���c�������2���2���2�� ���  ��c���� ���  ���c���� ���  �������`��� ���  �������`����� �f      	��",� @    �e     K� �  �4      }=,T�Qa�� �� ��e      K� �  �4      ,T�
IaŠ  � y��e     @ �  �2      ,T�Ia,� �� Ĕ�e     @ �  �2      ,Th�`�    L`   �#(Rh���f   google_allow_expandable_ads ����RcN�   ^true$  R�Dt           l ��l� �"�1!�
�c��!�b����y	  �-�
�^����   (Sb��        a`����DavE �F ��d       � ��Y    �e    @ �  �2      ,T1�`  \L`*   iII%8�4u2�1Rd�<��
   attributes  yRc���   data-   R��� RfB��y   data-matched-content,RiB�F�   google_content_recommendation   Rb2ib�   dataRc����   google   !� ������DX          -  ��^���X-  ��^�������c���Ę ���c����2
����c���Ř ���c����2�--���m����/�y	  �-�
�-��^����p�-��-� ���_����"�-�$���_����&�-�(�y* ��_����+�b��-�-/�^��1� ��-�3�c���5�U�
�4�7��P9Ċ� :� (Sb��        �`����Da�F bJ 4�k;       @ � � `P @ P � 
���x�e   @ �  �2      ,T�
Ia>� �� ���e     @ �  �2      ,T��`�  �L`h   Y%�#��u�%& �Rc�]�O   _0ads_al9 a��<:U5!9e5� �E: !�e�  �  >  (      a=�4	,RiF�O�   google_responsive_slot_preview  Q:]
`   M`   Rc�-��   static  u59%Ya333333�?-aN  .  Y5Rd�py   offsetHeightm:a:Rc&	�r   #gfwmrp 	�%Ya{�G�z�?0�6Rbn�^.   efwr`�  �D�X   (        ��c� - �- �-�� ��c��-�!
�-�-��
�c���� ��c����m���n��!
�-�-��
�c���� ��8���8�/�x�����A ��c�������8� ��8�!�8�"2	#� ��
���8�%��i��&�2(�2*��2	#�2,���-��w�8�.2�/ �� r�b��1� ���c��3�� ��������`��5���-�7�]��9�-�;����\��= �������`��?-�7�]��A�2C-�E��n�G�&�T�!-�E2*-�-�Ew�8�H�2�I�-Kŗ �������`��M���	-O�� �� ��b�Qn�S�% �� s�b��T�&��b�V��-X� �-�Z��c���\�( �� w�b��^� d�������`��`��-� -b� ��c�d� ��c�f����A��c�hė2j�e&�z l%�-�!m�c���o�2j�F-"q�? s ��#������`��t�2j�-$vlx��� ���c��y�� ��c�{Ř ��������`��}�3 ��c��k��c���&-�-�2��-�-�2�� ��c��-O�-K�2*�-'��-(��2'��  2)� ��b��2*�� ��-�c���-��+�-��-�,�k����-*�k���(-�-.�k��� ��
�����`����w/�!0��-�1��]��n���b�-*�l���V ��-K�!
�-�-���
�c���-O����`����22�2j�
�22���  (Sb��        �`����Da�K `Y ����       P ` @ �X @� @ � , � � 
� `�"p�� "P @ @ � � 
�P � � 
`���
`�`
��X ��H @ 1@ ,  �e    @ �  �2      ,T�
Ia¬ v� ���e     MQ �  �2      �D,T�Ia�� �� ���e     @ �  �2      ,T�Iaȭ � �	��e     @ �  �2      ,T�Ia� �� ���e     @ �  �2      ,T�Ia�� Ԯ �e     MQ �  �2      D,Td�`�   L`   8Sb��        !� a�a$   �� 	`����Da�] N^ � !�,T�
Ia�� %� Ia>�e       K� �  �5      �Ds           � �% ��"  � ���  �b�� %�-�� ���  � �Á �c������b      @ �e    @ �  �2      e>,T1�`   `La,   D  8Sb��        a� �a�� �� }`����Dat^ �b  !� ��m���� �rRdnG�i   scrollWidth Rd��2m   scrollHeight�& a�IYRb���   URL  �,T�
Iaذ J� I}>�e       K� �  �5      at  <  SbJ �        �`�� �Kd       �  �     Dh      ������ �����h���� ��"  � ���  �b�� %�-���-��-��-���^���
��-��-��-���^����-��-��-�	�]��� ���  ��b�����h���� ����h�������� ��� ��� ��� �i�� �c����� ��� ��� �i��  � ���  ��-��b��"�-�
$�c���&� ���  ��-��b��(�-�*�c���,�c���.��� ��� ��� �i�� 0� ���  ��-��b��2�c���4� ���  ��-��b��6�-�8�c���:�c���<�� �i�� >% ���  � ��i  �b��@�N�a ���G ��-�B�b��D���`��F�a ����G ��-�B�b��H��`��J�n�a ���G ��-����-�L��-�N��b��P��`��R�a ����G ��-���-�T��-�V��b��X��`��Z ���  ��-��b��\���l�^U��� ��� �b��_�c��a ��  ��-�c��� � ���  ��c���e�c��g�c���i ��  ��-�c��-��� �`��k���h������������h���X�tm      P @ @ P @ @ @ @ @ @ @ @ P @ @ 	���     �e    MQ �  �2      �>,T9�`,  <La      !�y ��Rb(��   lgdp����� �r �SbJ �        �`�� Kd               D%H       ������ ���h����- ��-��-�-��-��!
�b��^�����-�-��-��!
�b��^���-�-�	�-�
�]��� ��b� ���h���� ����h������-"� ��c��$��i�� &�z�����`��(`����`��*k�	�����`��, �c��.���h������������h��  (Sb��        `����Da�b Hd �,�i0       P ` P @ P P @ @ �e    MQ �  �2      ,T�
Ia5� Ų 1	��e     MQ �  �2      ,Tt�`�   L`
   U � �,T|�`�   L`
    ��$L�a"      CRbv��   ssp C	FRb~�#   wpc FRbF�u�   cu  Fm]
`    ]�>�`    ]Rb
��   psi F��>�Dy(           "� ��b�� �-� � g�-� � g�-��_�����}
��3���m����a����;�m�3���(Sb��         I`����DaLf @g �e       P 0  #   �e      K� �  �5       ���Dw(          ��a� ��! 2���� �d�d���i��2� �� ��a�
�! �`��2�(Sb��        `�����a�e Dg ��d      � ,@ ,  �e    ���  �4      �>,T,�`   L`    ����De          -  �-��  (Sb��         Qc      get i   `����DaNg xg ��b         �e    �� �  �4      ,T�
Ia�� ճ Qc      get j   ��e      �� �  �4      ,T�Iaڳ � Qc      set j   ��e     �� �  �4      ,T�Ia� (� Qc      get m   ��e      �� �  �4      ,T�Ia5� Q� ���e      @ �  �2      �D,T@�`>   L`    Rf� �   HTMLScriptElement   RI�Dj          !  q�-�-�^����� (Sb��        �`����Da�j 8k �c	       �
�   �e    @ �  �2      ,T��`h  ,L`   YRe�ځ�   currentScript   Rc改�   scripts a��Ua      (Kh@   2   o   �  	   �   �   �  
   D�h          ��-�  �-�� ��-��c���-����
�-������]����e� �-��+-�������� ��c��l��	����9 ����������#��-���]����e� ������� ����(Sb��        `����DaNk Ll  �f       @ @ P /  �e    @ �  �2      ,T��`  (L`   8Sb��        a� !�a�   $   u	`����Dadl �m ��,T(�`   ]�Dd          �l� �(Sd��             �����a�l �l �?�b          �e     K �  e5      ,T�
�aX� j� `    ��e      K �  f5      ,T��ap� �� `    ��e      K �  g5      ,T��a�� �� `    ��e       K �  h5      ,T��a�� �� `    ��e       K �  i5      ,T4�`"   ]�Dg          �u���c��� �   (Sd��             �����a�m �m ��b          �e     K �  j5      �D�(          � �%%} )�s�})�7s�� 7��s��7��s��7��=s��7��?s��7��
�7�� s�~7�� s�})�s��7���7�� ��   ��e      sJ H H0%@  �e    @ �  �2      �?�?�?�?�?�?,T<�`4   L`   RJ�<f   ggeac   �Di          ���-�  �
~�2� ��  (Sb��        �`����Da�m n �b       ,  �e     @ �  �2      ,T��`�   L`   ���yKd       a        D}@          ��-  ��^������4-�m��+��/�/�	 ��/�/��]���PÊ3 ��V�l��������   (Sb��        �`����Da$n po ��e       P   7  �e    @ �  �2      ,T4�`"   ]Kd          i       Dg          ���b� U���   (Sd��            ����=
a�o �o ��b          �e     K �  �4      ,T�
�a� �� `    �e      K �  �4      ,T��a�� �� `    ��e       K �  �4      ,TP�`Z   L`   Y ����Dn           &� "� ;� �b�� �-��]���!�b��c���
�   (Sd��            �����a\q �q ��c       @ @ �e     K �  �4      ,T��aθ �� `    ��e      K �  �4      ,T��a� 9� `    ��e      K �  �4      ,T4�`&   L`   Y�Dg           ���-�  �c��� (Sd��            ����=
a~r �r �b         �e     K �  �4      ,T4�`&   L`   ��Dg           ���-�  �c��� (Sd��            �����a�r �r ��b         �e    K �  �4      ,T�
�av� ~� `    ��e       K �  �4      ,T��a�� �� `    ��e      K �  �4      ,T0�`   L`   U	 �Df          !  �-��-��  (Sd��             �����a�s �s ��b       P �e      K �  �4      ,T��aݹ � `    ��e       K �  �4      ,T8�`.   L`   ��Dh           �� �c�� ��� (Sd��            ����=
a6t �t �b          �e     K �  �4      ,T�`�	  `L`,   5��a��Uy���	�uD�a      �C�C�CRbvg�C   xb  C-C�C �C�@�� ���E#b@  �  �  �K�   0   @  @
     d   �   �     �   �   (      X  m  x     �  �  h%     �  T  �"     l  �  $     �  �  �&     D�(         !  �i�� �/�-��]����
�-������]���e� �-���-��������-�����ߗ%�]�� ۠e� �-��-���ۋ�ߗ%�]��"۠e� �-��-���ۋ���ދ����ߗ&��-�$��]��&��e� ���ޙکܧޚ
���P����-�)E(/�+�-�-�-�/�-�	1�-�
3��:�68�5�_����7 �	  9 ��������&��-�:��]��<��e� �������䩲>@�-�B�����]��D�e� �-�F�-�H������i� T����`��J��LN�-�P�����]��Rܠe� �-�T�-�V�������i� S����`��X�-�Zl�\�q�F�X��c���]�b��_�b��av��c���c��v��c���eT��-�g�v��c���i�^���k��q�F�X��c���n�b��p�b��r�i� S����`��t�-�v:�m�-�
3�v��c���x��8z�_����{|}��i� S����`��~�-��m����}�)�3��q�F�X��c��׆�b�و�b�ڊ3��i� S����`����-��3�	��3��v��c��ٖ3���3��i� S����`�����/۞3����-���^����P�Ê�� ��/��v�
�c��ت���`����f ��/��v��c��ذ���`����@ ��/��i� S����`����/ٸ�-���]�ټ���`��� �h � ��ߋ������&��-����]��à�e� ���ߙܩݧߚ
��� �  � ��������&��-�Ɵ�]��Ƞ�e� ���������(Sb��        -`����Da�t �x ����       � P @ P P @   �
�� ��[ @ P P @ @ D� �  @ P @&'@ 0@ 0� 0�  �  /@ @ � @ � .  �e   @ �  �2      ,T\�`t   L`   ��u���Dq            ��-  �^��-�|�_��-	�^��-���d���  (Sb��        h`����Da�x \y �d       P ���  �e   @ �  �2      ,T@�`>   L`   0Sb��        !�`�� `����Daty �y �,TD�`F   L`   Rb���    +   !��Dk          ���w� 8���w8��b�� H�  � (Sb��        I`����Da�y �y �@�b         �e    K� �  �5      �Dj          � �%��� �a� �%� � ��b         �e     @ �  �2      A,T`�`|   L`    �������Dr           -  �/��-  �|�4��-  �/�-	�^��-�^��  (Sb��        %`����Daz �z �d        0 ��  �e    @ �  �2      ,T��`�  �L`T   @Sb��        a� !� �b�   $   �� �`����Da�z � � a� !�u�� a�]
`    Mc             a�����U��,T��`�   L`
   - � �E#��D�(          -  �l��l�-��-�-��]��	/�x�P����h�-�����`�����c����'����h�-�����`����c���� (Sb��        I`����Da�| } =A�e       �
� � �  �e    K� �  �5      y�� �Rbf=�/   wa  ��-���	�@ !�E#l�  �  r    �  0  �  �  
  	  �  8  f
  ]
`    La       �KĀ  �     (1  !   *  �  �.  '   ^  �  �  .   �  �    0   <  v  �  -   �  �  (  .   �  1  �)  -   I  ^   +  .   �  �  �/  (   5  J  `2  "   s  N  �:  !   f  �  (<  "   D��  (       � �%%%| �	�l�UĘ,�-��/��ċ�-���4�����0 ��  ��-���|
�����`���� ���  ��-���^����|� ��"  � ��#  �b���-�%|�z%ܲ��-������]��ؠe� �-� �(-�	"�������-�
$��/�&�-�(�]��*ֲ�,.�-�0�����]��2Ҡe� �-�4�%-�	6�����8:�-�<����ϗ%�]��Bˠe� �-�@�-�	>��ˋ�ϗ%�]��Dˠe� �-�@�-�	>��ˋ���΋����ϗ&��-�F��]��H��e� ���Ιʩ̧Κ
��Ԍ&���� ��   �i�� J�-�Lҁ �^���N�-�P����RT�-�V�����]��X̠e� �-�Z�&-�	\������-�^�-�`�^���b�6d��ϋ����З&��-�e��]��g��e� ���ϙ̩ͧϚ
��Ԍ'�$�-�i�"��l�k�2/l��b �� ��  ��A �b��n���`��p�/r���ҟ!t�b��v�b��x���J��l�z�)��v �/�{�-�}��c���lҁ��/��!t�b����^�І��#�����-�������]�ю̠e� �-���!-�	�������-���l̖� -��9���-���-���-�����o̠��:��m����<����l̤��� ��h �-����� ��`�����c��ʩ��� ��h �-����� ��`�����c��ʯ���v �-����c��ʱ���l̳U���U���-���!t�b���/̸��_-���-���]�ʼl���J ��   ��-���-���!t�b���/���-���-���]������`��ǋ/�-���!t�b����-���-���]����4����-�^�-���^���� �� � ��ϋ����З&��-�ԟ�]��֠�e� ���ϙ̩ͧϚ
��ԋa��d � ��  �c����l��U�.�a ���D �b�������`���-���^���� �� � ��Ջ����֗&��-���]����e� ���ՙҩӧ՚
��ڋ �@  � ��ۋ����ܗ&��-���]����e� ���ۙة٧ۚک�����-�������]���ؠe� �-����-�	�������-���]���-���^��� ���  ���֘�� `����  ��1  ��i ��� ���� `���� ��� ��3  � a��� z) %�� 6�������-��� `���� 
 ��   ��ۋ����ܗ.�� -�� �!� ]������e� ���ۙة٧ۚک ��  ��-��������� `���� ��=A���     3� 0	� &0'P P  P @ P @ P P P P @ P p��"@ @ D
��� P @ P P !���� "� 	 �� 
� �@ D� .p��� 
��� � &p�  �e   @ �  �2      IA,T�
Ia�� K� 5�e     @ �  �2      �D,T��`   L`   0Sb��        !�`$   e`����Da�� ؃ �,T�Iab� p� I�A�e      K� �  t5      ,T<�`6   ]�Di(            ��� �����`�� � (Sb��        I`����Da�� � ��b          �e    K� �  u5      ,TL�`V   L`    ����Dm          �-�  �/��|�-���-�  �/��^���	� (Sb��        I`����Da(� d� ��c        0  �e     K� �  v5      ,T�
Ia�� �� I��e      K� �  w5      ,T<�`2   ]�Di            ��� ����`�� �   (Sb��        I`����Da�� Ѓ ��b          �e     K� �  x5      �D�(          � �% ��5  �Á ��`��  ��5  �Á��`�� ��5  �Á��`�� ��5  �Á��`�� ��5  �Á��`�����c
      @   �e    @ �  �2      �A�A�A�A�A,T��`*  HL`    ]A-� !� a�� �� � !��� �a���UHKp�   �   �   �     �   �   �     !  S  �
     g  |  �     D��   (       ����~��-�  ��-��|��2�2~2	�2 })��s�|7�� s�|7�� �2~2~2ɺa�Ř�-�	�
�^����| Ų�!#�-�%�����]��'�e� �-�)�/-�+������!-�b��/Ř-1��4��3�> 5���������&��-�6��]��8��e� �������멲�:<�-�>�����]��@�e� �-�B�-�D������-F�4��H�. J���������&��-�K��]��M��e� ���������   (Sb��        �`�����a� �� D�oO       0`�`N	&,� P �� 
� � .P @ P �    �e    ���  �4      ,T<�`6   L`   ,T�
Ia�� � I��e       K� �  �5       ���Di(           6�Á  ��`�� 2� (Sb��        �`����Daօ 
� ��b      ,  �e    @ �  �2      �A,T,�`   L`   ,T� ��a#� )� I��e       K� �  �5       ���De           �  2 �  (Sb��         �`����Da2� T� �b         �e     ���  �4      B,T@�`:   L`   � ���Dj           "� �� �b�� �-��^���   (Sb��        �
`����Dal� �� ��b       @ �e    @ �  �2      ,T��`�  `L`,    a�Rc`��   config  )M !� �� � !���5��� ,T�
Ia� !� I�e       K� �  k5      ��,T�Ia1� >� I��e      K� �  l5      ,T�IaI� O� I��e       K� �  m5      ,T�Ia_� l� I��e      K� �  n5      ,T�Ia|� �� I��e       K� �  o5      ]
`    La       M�D��          �-  �-�-�-�
 ��a��-�
��-��� !�h� V����`����-��]������h� V����`����-��]������h� V����`����-��]�������i�� �-�	"�Z ��}
$)�s�!%�i�� '7�� )s�!%�i�� +7�� -s�!%�i�� /7�� 1i� U����`��3�c���5�-�7��^���9�� 6��� ���`��;�i� U����`��=�-�?���^���A�b��C 6������`��E�i�����`��G�-�I���^���K�c���M�� 6������`��O�b��Q ��c���S�� �� �����������i��U�c���W 7�b��Y 8�b��[ 9�b��] ��c���_ 1�i�����`��a�zc%���6��d����`��f $��T�-�hTT %$ �� ��b��j�� ��b��l� (Sb��        �	`����Da�� � �X�tn      P P P P P P � $� 	@� ��� � � � � ��� @    �e    @ �  �2      EBMBUB]BeB,T��`�   L`   Y� !��D0          �� ��a� �� :� "� ;� �b�������`�� ��c��� �� "� ���b���c���
 "� ��b���-��]���(Sb��        �
`����Da"� �� �d       @ @ @ �e    @ �  �2      ,Tp�`�   4L`   8Sb��        a� !�a�   $   !`����Da�� � �,TL�`R   L`   ,T�
Iay� � I�B�e       K� �  �5      �Dm(           ��6 ��  �`�� ����`���   (Sb��        I`����Qb      c.i a֊ � ��b        �e    K� �  �5       !�,TL�`R   L`   ,T�Ia�� �� I��e       K� �  �5      �Dm(           ��6 ��  �`�� ����`���   (Sb��        I`����Qb      c.g a� N� ��b        �e     K� �  �5       ��,TL�`R   L`   ,T�Ia�� �� I��e       K� �  �5      �Dm(           ��6 ��  �`�� ����`���   (Sb��        I`����Qb      c.j aX� �� ��b        �e     K� �  �5       a�,TL�`R   L`   ,T�Ia�� �� I��e       K� �  �5      �Dm(           ��6 ��  �`�� ����`���   (Sb��        I`����Qb      c.h a�� ҋ ��b        �e     K� �  �5       �,TD�`F   L`   ,T�Ia�� � I��e       K� �  �5      �Dk(           ��6 ��  �`�� ��b��� (Sb��         I`����Qb      c.m a܋ � ��b        �e      K� �  �5       !��Dv           � �%% ��"  � ���  �b�� Ł 2��2��2��2��	2�

�  ��c      ,� ,� �e    @ �  �2      �B�B�B�B�B�BCC!C-C,T��`   L`   �	Y a�) a��D�8          }  )�s� ����b��-��]���b��7�� 	� "� ;��b���-��_�� @��-�� =� �� "� ��b�����`���_�����  (Sb��        @`����Da&� ��  �f       ��� 	� 
�     �e    @ �  �2      ,TY�`n  dL`.   Rb&���   plle4�a      C a�CABC)CM`   � a��a
      ]ACMC(Rhv�
   google_disable_experiments  �����Y !�a�� �U	ReF$�   google_mc_lab   ,Ri
��5   google_auto_storify_swipeable   ,Ri>���   google_auto_storify_scrollable  `  (Kh@   �   0  �	  
   C  V  �
     DEp            \�b� �-� � ��� ��b��c����2� h� W���`��
�r�	�c��� ��}��3� ��c�3�})���-x3��3��3� ��b�3�	 b��"u��c��$�'!
&�b�( "� ;��b��*�-�,�^��._��E��`��0���24�-�6�����]��8��e� �-�:�.-�<������ "� ;��b��>�-�@�^���B�< D���������#��-�E��]��G��e� �������� ���c��I ��
�c��K��b�M� �-O��c���Q�$ "� ;��b��S�-�U����^���W �-O��c���Y�$ "� ;��b��[�-�]�s1��^���_ �-O��c���a�$ "� ;��b��c�-�e�r1��^���g� (Sb��        	`�����aΌ `� T�si       @ ,@ L��Y 0� 0@ @ @ @ P P �� � � ��� 
�   �e     K� �  �4      ,TH�`J   L`   0Sb��        !�`$   i`����Dax� j� � ���
,T�
IaE� �� I�C�e      K� �  d5      �Dl           � �% ���  ��-� ā �^����   ��b        �e    @ �  �2      �C,Tȓ`J  ,L`   0Sb��        !�`$   `����Da�� ��  ��b/�SbJ �         ��`�� �C ��,T�
Ia5� J� I�C�e      K� �  c5      Kd       L   a     D�P          � �% ���  �����} ���� �c���  ��X  �!�-���^�����i��������-�
� F��!q�����!�!��b���b����� �\��� ��X  �i�� �   � �f      P P T�     �e    @ �   3      �C,Tt�`�   L`    �� a� � a��Dw           -  �-  �-� -�-�^���-�2 ����-�-
�c����2 �-  ���  (Sb��        y`����Da̒ f� �d       P ,P    �e    @ �  3      ,Tt�`�    L`    a� a�5 !� a� ᪕Dw8      ������Z��������������i��  ���2 2!�i�� 2
!�i�� 2�2� (Sb��        `�����a�� :� ��d       ,� � ,� �e    ���  �4      ,T�

a(� �� ���e      K� �  �4      D,T�Qc      lp.ub   a�� �� ��e      K� �  �4      ,T�Qc      lp.vb   a�� �� ���e      K� �  �4      ,T�Qc      lp.yb   a�� 2� �e      K� �  �4      ,T��`$  0L`   �+9/,T(�`   L`   5/�Dd           -   � (Sb��        I`����DaT� �� ��b          �e     K� �  �5      Y !�� RfJ�X�   getDataWithCallback 
 a�)�D�8      ������Z��������������i��  ���~�-� ��� �2�   ��� ��i��2-
�-��-��� ��_����-�-��-��� �	�_�����  (Sb��         �`�����a�� J�  �f      � ,P @ P    �e     ���  �4      -D,T�
Ia1� 1� `    ��e       E� �  �4      ,T��aM� �� ���e      K� �  �4      D,T�Qc      pp.ub   a�� �� ��e      K� �  �4      ,T�Qc      pp.vb   a�� ?� ���e      K� �  �4      ,T�Qc      pp.yb   aC� ^� ��e      K� �  �4      ,T��`  0L`    � ��RV��7   googlefcPresent Y !��AD� a���D�8      ������Z��������������i��  ����2� �2  ���i��2-
�-��-��� ��_����-�-��-��� �	�_�����(Sb��         �`�����a� :�  �f       ,� ,P @ P    �e     ���  �4      ,TD�`B   L`   0Sb��        !�`$   �
`�����aL� 0� �UI,Td�`�    L`   b/��=SbJ �         �`�� �DRc��   sc-cnf  �:Kd          �      Ds(          ���!  �-��-�^���ŋÃ�������T��-�l�
U�
�c���(Sb��        I`����Da|� ,� ��d       P 	    �e     K� �  �5      �Dk(          � �%�� �! � �`���   ��b        �e     K� �  �4      �D,T�
Ia�� �� ��e     @ �  3      ,T�Ia�� � ��e     @ �  3      ,T�Ia� �� =��e      @ �  3      ,T�Ia�� � ���e     @ �  3      ,T�Ia"� �� 1��e     @ �  3      �D,T�Ia�� �� ��e     @    3      D,T�
Ia�� �� ��e     @   3      D,T�Ia�� �� �
��e     @   	3      D,T�Ia� t� i��e     @   
3      D,T�Ia� 8� ���e     @   3      ,T�IaE� .� m
��e      @ 	  3      �D,T%�`  HL`    URe���   google_plmetrics RfR�{�   PerformanceObserver ]
`   M`   Rdb�U   layout-shift$Rg�gG   largest-contentful-paintRd�?B   first-input RcRSX   longtaskRb^��   Aa  ]��Rc�|�   event   a���a
      �CRc�z��   bufferedG� Rf���:   durationThreshold   RcB{a   observe U(Kh@   `   �      
   �   �        Dp           �i��  � �� ��b����! �-���! �-���2�
z%�-��-��-���^������-������]����e� �-�	�H-�
������}!)��3�"����l�$�(�2�%� �b��'�-�)�^���+�V -���������#��-�.��]��0��e� �������� �b��2�   (Sb��         q`����Dav� � 0�j4       ` � ��
��� 
��� q
��[   �e     @   3      ,T�
Ia�� �� ���e     @   3      D,T�Ia�� �� ���e     @   3      �D,T�Ia�� t� ���e     @   3      ,T�Ia�� =� �
��e     @   3      ,T�`�  �L`J    !� �� �� a����m�	 a� � !�= !�5Rbֱj�   Ia  !	��] a� �A !�Rb��2�   Ja  ,T�
Qc      Hp.Ba   ad� j� I�e       K�   �5      %Y]$R *��q   [data-google-query-id]  i Rf�Q�   data-google-query-id	�a      ]HQE�D�P          �2  �2�2�2�2!
�-��2�22	!
�-��2
�2�2�2�2�2�2 �2"�2$�2&�2(2*�2,�2.�202224� 26!8�-�:��^���<����-� >�!�^���@�2�"B}#D)2$E� (Sb��         1`�����a�� �� <�mG      ,� ,` ,� ,� ,� ,� ,� ,� ,� P @ � �e     ��  �4      �E,TL�`R    L`   �RRdeZ{	   className   �Rc��G   dataset  R���   adsbygoogleStatus   �Dm           �� �-� �-�^�����-�-�l�
U�   (Sb��        y`����Da`� �� �c       @ �e    @   3      ,T@�`@   L`   �E��E�Dj            -  �2� #����`���(Sb��        �`����Da� l� ��b       ,@ �e    @   3      ,T��`J  XL`(   HSb��        �� a� !� �c$  �   $   �� =	`����Da�� $� �UR�i_   google_spfd (Rh�S�I   google_reactive_ads_config   RfZ*-   page_level_pubvars  (Rh*��   page_contains_reactive_tag  ]1Rc~to�   pltais   a�@Rn~��2   Only one 'enable_page_level_ads' allowed per page.   Rf�0/�   google_pgb_reactive �,T@�`<   ]�Dj          ��/� ��
�/�4���  (Sb��        I`����Da�� �� 	F�b        � �e     K�   �5      )9%9 Rf"d�   first_tag_on_page   (Rh�xL   google_reactive_tag_first   ,T<�`8   ]�Di(           ��� �����`�� �(Sb��         I`����Da�� � ��b          �e      K�   �5      `F  �D�8           � �%%%! %�-��� ���  �2���-�ŗ ���  �����`�� ���  ���c���
 ��$  ����`������-��~� ��\  ��� �b���-��H ��\  ��� �b���-��/-�� ��_  ��b��� ���  ��	��i��� ��\  ��� �b���2�  ��_  ��-�
"l�$�b��%��� ��b��'2�)�� � ���   �c���+��-�-l�/U���2�0��� ��\  ��� �b��2�-�4�l�62�7 ���  � � �c���9�   �4�k;      � @ @ P @ ,P � ��"P q �e    @   3      )FAF,TE�`D  �L`N   F�E=,Ri��<�    (\W|^)adsbygoogle-noablate(\W|$)R�Rb�y[\   on  R��G�   google_adtest   �&u(Rh���.    adsbygoogle-ablated-ad-slot�8 Rf����   google_element_uid  m�RbN@��   slot	i�%y	&ui5cT  H  0  (  E�#YRe�f3   createComment   �5LRq*��>   No ad requested because of display:none on the adsbygoogle tag  %Fy"s<Rm^9<-   Adsbygoogle tag with data-reactive-ad-format=   XRt��J    is deprecated. Check out page-level ads at https://www.google.com/adsense  �D1H           -  �- �!��b���-��-�^���
� ]�b�Ö-���-l���� ��c�� ��b��-����k����;��-�	�-�	�����-�	p���-�
8�2-!��~�2�#��b�%��2'-)�4�+--��!/�b��1�_���3�-�5l�7�o��-8��^��:�b��<U�--���_���>��-8��^��@�b��BU�--���_���D-F�2�H-F��2�J����c�LØS�-�Nl�P�G�-l�Q�;�-Rm�T�0��,-U�-�W�- Y�-U�-�[�!�^���]�^��_�-"a��-"al�c�-Rl�d��-Rl�e���8�-�#f�.�-�#f�-�$h�%�!/�-R�b��j8�l�&8�m�^���n�  (Sb��        �
`����Da>� �� X�tp       ` @ P � 
 �"p���
��� 
 @ @ P ,@ �X ��
� ��#@   �e    @   3      ,T��`�   (L`   Y�Rb(O   INS yRcN<��   reserved�E�E��D{8          !  �-���^������/��-�m�
�A�� !�b���&�-��-�l���-��l�����P�/�D �(Sb��        �`����Da�� R�  �f       @ 	��"D�    �e    @   3      ,T��`�    L`   1�,T�
Ia�� �� I��e      K�    �5      ySbJ �         !�`�� ��,,T�IaB� P� I�F�e       K� !  �5      Kd    6   P   �     D�0           �s �r� �i �ā �b����-m��M��m��D�� *�- �]�
���`���Ã�����!�b����QŊR � (Sb��        I`����Dal� �� �e      	P 1@ @ �  �e    @   3      �F�F,TL�`R   L`   �FR���   adsbygoogle �E$Rgv�A    adsbygoogle-noablate   �Dm          �� �b�� �2�-��8�2���b����   (Sb��         �`����Da�� �� �c	       ,P �   �e     @ "  3      ,T	�`�  8L`   @Sb��        !� �� �b$   �� �� `����Da�� � ��4,T��`�   L`
   $Rg���   enable_page_level_ads    ���� ���D{          �-�  ������4���E�-��^������/��4�
�� ��-��^������4���   (Sb��        I`����Da� �� G�d       8P  � � �e     K� $  �5      GF%�5�a
      FC�4C� RfRJ�$   pause_ad_requests   $R Ƙ��   google_pause_ad_requests,T�
Iaf� �� IG�e      K� %  �5      �D�8          � �%%%~% ��p  ��-� �c��%�� � ���  �� �c���� ��-��b�����-��2�
� ��'  �a���� �-��-��^���})�3��-� 3��� ��\  ��� �b���-�	x2�
 ��"  ������`�� ���  �b��!� �$�g#      @ � P L���  �e    @ #  3      G)G,T	�`�  HL`    @Sb��        a� !� �b�   $   �� �`����Da0� �� Y4,T�
Qb      Zp.ca�� ��  ��AG�e       K� '  �5      Y%�!�!Rd�m�/   interactive SbL �         !�`�� � ���R*�5   DOMContentLoadedRe�kG�   MutationObserver,T�Ua      ��a      ��a      eEQd   
   .childList  ax� �� IUG�e      K� (  �5      ��a
      Rd���	   childList   GRc��1   subtree G�D�8          � �%%% ���  �� �b�� �2�� �� �-�%�-�� ��-�l�
���-�l�� ��(  ���c����s��%�� � ���  �	�-�
� � �_�����b��%�� �� ���`��� �-�����i���-��� �})�_������AG$�g      ,P #@ P 0	    �e    @ &  3      EGaG,TX�`l   L`   HSb��        �� a� !� �c$  �   $   �� E`����Da�� (� ,T@�`:   L`    ᩗDj(           ��/ ��� ���`�� �   (Sb��         I`����Qd      Wp.e.client a�� �� �G�b          �e      K� *  �5      ,T�
Ia�� �� I��e      K� +  �5      �Dp(           � �%%%%~% ���  � � Á �`�� �  ��b         �e    @ )  3      �G�G,TD�`H   L`   0Sb��        !�`$   -`����Da>� ��  Rf����   google_checked_head ,T��`�   ,L`   ���=�= �� !�y"s0Rj.] �!   AdSense head tag doesn't support    Rdr�5R    attribute. �D|(           ��� �/� �U�W�- ���_����-� �y ��_����	��-��-���w8��8��^����   (Sb��        I`����Da�� �� �G�d       @ L
� H �e     K� -  �5      �Dk           � �%�W��� �Á �c��� ���b         �e    @ ,  3      �G,T��`�  tL`6   8Sb��        !� a�a$   �� �	`����Da�� d� Y]XRT�o��I   script[src*="/pagead/js/adsbygoogle.js?client="]:not([data-checked-head])   `Rv�N�|Q   script[src*="/pagead/js/adsbygoogle.js"][data-ad-client]:not([data-checked-head])   m R��^�   data-checked-head   	U}1,T�
Ia�� �� I�G�e      K� /  �5      �a
      �4CGC��Rc&0��   bottom  ReJ�E[   google_overlays �a      HGRc.W�   overlays�FRc&��   loaded  ��a� R~�ě   google_adbreak_test  �,T�Ia!� .� I��e       K� 0  �5      a  �  �D�H          � �%�� �-� �-���^������ �-� �-���^���Ŗ-�
���_���� ��\  �!�b���-�	� ��-  �b��� ���  ��
 �b��~� ���  �c��� ��+  �b���� �b��%2�	} )�-�!3�#�3�%���-�'l�)�}*)�2�+�W��� �-�-��� �|/�2�0��� �-�-�-�2�-�4�^���6�-�8�-�8�������\��:-�<�->�]�@��-�B�]��D�� ��;  ���c���F� ��  ���b��H�  �@�nJ      @ P @ @ @ ���"s�&P @ @ @    �e    @ .  3      H1H,T�
Ia?� �� ��e     @ 1  3      ,TL�`V   L`   �RcFY   sound   Re���.   preloadAdBreaks �Dm            �%� -   ��- �
- ��� (Sb��        �`����Da*� f� ��b       P �e    @ 2  3      ,T��`x  �L`V   8Sb��        a� !�a�   $   Y`����Da�� �� � a�0Rj��C!   push() called with no parameters.   ,T�
Ia�� �� ImH�e      K� 4  �5       !� ��4Rk2�&%   first_slotcar_request_processing_time   �rA,Ri�w    adsbygoogle_execution_start_timema��SbL �         a�`�� �,T�Ia�� �� I�H�e       K� 5  �5      Re�6   handleAdBreak   2G�4@Rn����2   'google_ad_client' is missing from the tag config.  ,T�
Ia�� �� ImH�e      K� 6  �5      RcJl�T   params  ,T�Ia� � I��e      K� 7  �5      Rbf�kg   js  9y"s�R���N�   Ads with google_ad_output='js' have been deprecated and no longer work. Contact your AdSense account manager or switch to standard AdSense ads. }1,T<�`8   L`   ���Di          �-�  �^����4��(Sb��        I`����Da,� x� ��b       � �e     K� 8  �5      RdE�@   hasAttribute RZ��/   data-require-head   �R�.|L=�   AdSense head tag is missing. AdSense body tags don't work without the head tag. You can copy the head tag from your account on https://adsense.com. 0Rj`�#   Ad client is missing from the slot. 9F,T�
Ia�� �� ImH�e      K� 9  �5      !G%Gb�  f  �  �D�@   (       � �%%� ���  ����i�� � ���  Á �b��-�]��5 ��1  ��-�]�
�-��]����u ��c���`�� ��.  ��b�����l��� ��`  �a�-� !�-��]��2!� �	�2
#� ��  ��, ��=  ��b��% ��  ��-�'��^���)�c��l�+�/��% ��  % ���  � ���c���,��, ���  � �� ��  �-�.��^���0�c���2�*!�i�� 4�-�6�]��8�� �	�$�  ���  � ��2  ��b��:���`��< ��3  �a�>�-�@�'�-�B �ŋ ���  ����i��D����' ���  �b��F ��5  ��c��H�)�-�JŘ�� �c���L��-�Nl�P�!Q�-�S��^���U�( ��6  ��-�W�b��Y� ���  ��c���[ ��\  �� �b��]�-�_�~�� �c��a-� c�!�^���e�1 ��\  �� �b��g�-�i� ���  ��"��i��k��-�m� ���  ��#��i��o�� ��\  ��� �b��q�-�$s�l�u� ���  ��b��v�! ���  %�b��x ��4  �b�z� ��\  ��� �b��|�-�$~�l��� ��\  ��� �b�����2�$��� ��\  ��� �b����-�&�x2�'� ��"  �����`�����l�y�      P @ @ 	� 
���� P @ @ P @ @ D
��� 
����� ��H @ D���   �e    @ 3   3      uH�H�H�H�H�H,T�
Ia[� �� ��e     @ :  !3      ,TD�`F   L`   �4�HI�Dk           -  �-  �-�-  �-  �� (Sb��        	`����Daz� J� ��b         �e    @ ;  "3      ,T��`�   L`   �	SbJ �         ��`�� Kd    )   1   �     D�(           �� o�b�� �j ]���b��Ř-� ŗ@����-�ċ��������� P�b���Ę O�b��
��T��� ^����c����  (Sb��         U`����Da`� ^� ��d       P @    �e     @ <  #3      ,T�
Ia�� � ���e     @ =  $3      D,T��`�   L`   9FRd��=   tag_partner RdͿO   tag_partners���D�(          � \���b�� �-� �l�� \���b����2� �-	�2-	� \��b���-����|2�-��-��^��� ��c� )�c��   (Sb��        }`����Da(� ��  �f       	`���� @    �e    @ ?  %3      ,T��`�    L`   � a�0Rj�5�"   'element' has already been filled.  Rd���	   innerHTML   0Rj�=;$   'element' is not a good DOM element.XRt�`(�L   All ins elements in the DOM with class=adsbygoogle already have ads in them.�D�          �\ !�b� �3- � %�- �b���� �����i����r�� �����i��
��  %�a�� �����i���� (Sb��        �`����Da�� � ��d       @ $@   �e    @ @  &3      ,T��`n  8L`   �
R�>P   __cmp   Y � ��Rc"1   cmpMet  4�a      Rc���   tcfv1   CRc�K�   tcfv2   CRbNM�   usp Ce
CRb"���   ptt `   }I�I�I�Ya����MbP?�D�@           �� ����i�� � �i�� � �i�� ���-���� ��b����� ��-�
�b�����-�� ��-��b��x2��2�� ���})��3��3��3�	-���3�
!�`��#� (Sb��         �`����Da� �� (�h%       @ P P ,� ��     �e     @ A  '3      ,T�
Ia�� �� ���e     @ B  (3      ,T�Ia�� �  ���e     @ C  )3      D,TD�`F   L`   ��U�,�Dk           �-  � �!�-���_���� (Sb��        �`����Da^ � �c       P    �e    @ E  *3      ,T�
Ia�  � u��e     @ F  +3      �D,T�Ia� � ���e     @ I  ,3      D,T�Ia� � ���e     @ K  -3      D,T�Ia� � A��e      @ M  .3      ,T��`�   L`
   HSb��        !� a� �� �c�� �� �� �� I`����Da � ,T$�`   ]�Dc           �  (Sb��          �`����Da" . �I�e      K� O  �4       ��Q,T=�`4  �L`T   8Sb��         !� ��a�� �� I`����DaR � � a� �]
`    Lb          	 ���a
      �
C]C !���Yu?`,T�
Ia� ] IJ�e      K� Q  a5      U�F)HRd�GI�   new_abg_tag $�a      �CRc�;'   host_v  CRdٮ�	   frequency   Ya{�G�z�?�%JYa{�G�z�?�a
      ��C�G,T�
Qc      k.push  aH	 V	 ��J�e      K� R  b5      ����(Rh�<�   requestNonPersonalizedAds   �a      �C�Re"��7   pauseAdRequests �a      �CRcz���   onload  �a      �C]
`   M`   EJMJa�U�$�5a�  �  HKp�       �     �    �     8  r  �     �  �  �     D)x          � �%% ��� �� ��b�� % ��� ��u���c����b�� ��[ ��r���c����b��� ��a�
����z%���-��]��6��c������������b���b������� �� ��� �})�� �3��u���c���3�	b�� ��c���"%���-�
$�-�&�� ��� ��-�(�b��*� ��f ��c���, ��� ������`��. ��� � ��f �b��0� ��> �a�2 ��� �� �b��4 ��� � >� ��� ���l�6�b��7�c���9��a�;������a�=��c���?o�A�) ��� � ��� � ��� �b��B�b��D ��� �a�F ��> �a�H�� �� �a�J��� ��� �a�L ��, ���c���N!P�-�RĘ-�T�( ��� ��}V)��r���c���Xw3�Z�r���c���]w3�_�`��a ��7 �a�c}e)��3�f�����!h�-�j��}l)� ��8 3�m��\��o!h�-�j��}q)� ��9 3�r��\��t!h�-�j� �}!v)� ��: 3�w��\��y������z"{%���|~�-�#������]�󂹠e� �-�$��(-���������/��U��/���4����6 ����������#��-�%���]���e� �������� ��& �����`����2����-� ��2� �� ��� � ��k �b��( ��� ��b��!
��-�&��-�'��^�����  J|�}�      @ @ � � `2@ @ P @ @ � � "@ @ P LH @� &P �&0��Y @ � 8p� ��� 
�  �e      K� P  �4      �D{    (       � �%%%%%%%�� �% ���  ��-� � ���  �^��� ���  � � Á�c���� �I�b      @ �e    @ N  �4      �I�IJ9J,T��`�  DL`   Rb�{�   _fy I�
L`   LRq�FL@   https://pagead2.googlesyndication.com/pagead/managed/js/adsense/Re
�.   /slotcar_libraryRb�   .js �J�L`   <Rm�J�0   https://googleads.g.doubleclick.net/pagead/html/ ��Re�|�b   /zrt_lookup.html�J�L`   �J ��RdF���   /zrt_lookup Rc�+W    .html   �J<�a      �C�C�C�C�C`C��
L`   �JRe��Du   /show_ads_impl  �J�J��L`   �$Rg*痞   /show_ads_impl_with_ama ��J����R�շ��   ^(?:https?:)?\/\/(?:pagead2\.googlesyndication\.com|securepubads\.g\.doubleclick\.net)\/pagead\/(?:js\/)?(?:show_ads|adsbygoogle)\.js(?:[?#].*)?$   `�D�H           ��s��c�� m�� �s��c��w8���u��c���u��c������
����`����������`�������������`���}�3��������`��3����	����`��3�
�3� �3�"y$  3�%��(Sb��        I`����Da. � (�h'       DH 0	`0	`20	��I 0� �  �e    @ S  �4      D`   �D]De    @    ]1      ������!-5MU]y������%-AU]qy��������%9MU]m���������-=Q]q�������	%9Mau}������-5I]q���������)9IQY����	AMemu�����-Yeq�����!9���������)Me�	iq���	%-9M���=EMU��������)EMUe}����������!iy���	1EYu���� ! 1 � � I!y!�!�!="e"m"�"�"�"�"�"�#�#�#�#�$�$�$�$�$�$�$%E%)'�'�'�'�'�(�(�(�(�(�(�)�)�)�)�)�)�)*	*!*)*1*A*I*Y*a*y*�*�*�*++1+9+Q+i+q+,),E,q,�,�,�,-M-U-]-e-m-�-�-�-�-�-...i.q.�.�.�.�.)/E/M/U/]/e/m/u/�/�01%1I1Q1i1q1�1�122Q2Y2�2�2�2�233-3Y3�3�3�3444!4)4�4�455=5�5�5�5�566M6]6m6}6�6�6�6�6�6�67m7�7�7�788A8]8�89m9�9�9):1:9:-;5;�=�=�=�=�=�=->5>=>E>M>U>q>�>�>I?Q?i?�?�?�?�@�@�@A1AqAyA�A!B5B}B�BEC�C�C�C�D�D�D�D�D�D�DEEEE%EuE}E�E�E�E�E�EUF�F�F�F�F5G�G�G�G=HEHaH�H�HII%IAIeI�I�I�I�I�I�I�IRb
M��   selfRd�N�   closure_uid_0�6�2R�x�   CLOSURE_FLAGS   i�6� �RcF-�   Android ����,Ri�6�   ^-?([1-9][0-9]*|0)(\.[0-9]+)?$  ReB�   structuredClone �$La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a�g  �k  �D�](�
a      �]a�     	a�      �]au}�	�$La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
aNw  pw  �D�]�
a      �]a�      �]�$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�w  �w  �D�]�
a      �]a�      �]�]
`    Mc             ��$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�}   ~  �D�](�
a      �]a�     	a�      �]��,Ri�)Ǉ   ^([^?#]*)(\?[^#]*)?(#[\s\S]*)?  $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a��  ~�  �D�](�
a      �]a�     	a�      �]IQy������Rc���r   append  �%#=�RzBg�x   ^(?:([^:/?#.]+):)?(?://(?:([^\\/?#]*)@)?([^\\/?#]*?)(?::([0-9]+))?(?=[\\/?#]|$))?([^?#]+)?(?:\?([^#]*))?(?:#([\s\S]*))?$Rbr_�}   #|$ �R�F�ъ   alternate author bookmark canonical cite help icon license next prefetch dns-prefetch prerender preconnect preload prev search subresource  ��Re�+�   ^([0-9.]+)px$    Rf���   ^(-?[0-9.]{1,30})$  	)1iq����YU�y���$La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a�  �  �D�]�
a      �]a�      �]��DRo��a�5   ^https?://(\w|-)+\.cdn\.ampproject\.(net|org)(\?|/|$)   $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a��  H�  �D�]�
a      �]a�      �]�$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
aP�  �  �D�]�
a      �]a�      �]�$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a޻  �  �D�]�
a      �]a�      �])�RcR��   measure Rd���
   clearMarks  Q$La   
   X�a      �y`^
  ��`^  �}`~  ��	`�  u
a �  *�  �D�]4�
a      �]a�     �a�     �a�      �]���$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a��  v�  �D�]�
a      �]a�      �]$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
aL�  :�  �D�]p�
a      �]a�     Qa�     �
a�      a�a�  
   I
a�     �a�      �a�      a�a�      �]-AYm����$La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a��  ��  �D�]�
a      �]a�      �]]
`    Mb         �`    Mc         
   �`    Mb         $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
ar�  ��  �D�]�
a      �]a�      �]�]
`    Ma      $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a��  �  �D�](�
a      �]a�     �a�      �]��$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a&�  H�  �D�]�
a      �]a�      �]�]
`    Ma   
   �`    Me
                  $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a��  ��  �D�]�
a      �]a�      �]�]
`    Ma      $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
at�  ��  �D�](�
a      �]a�     �a�      �]�$La   
   X�a      �y`^
  ��`^  �}`~  ��	`�  u
a��  f�  �D�]4�
a      �]a�     -a�     9a�      �]%1$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a��  �  �D�](�
a      �]a�      ma�     �]]e$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a$�  F�  �D�]�
a      �]a�      �]q$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
aV�  x�  �D�]�
a      �]a�      �]y$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a��  �  �D�](�
a      �]a�      ma�     �]��$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a��  ��  �D�]�
a      �]a�      �]�$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a(�  v�  �D�]�
a      �]a�      �]�]
`    $Mg      
                  $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a��  ��  �D�]�
a      �]a�      �]�$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a��  ��  �D�]�
a      �]a�      �]�$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a��  >�  �D�]�
a      �]a�      �]�]
`    Mb         $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a��   �  �D�]�
a      �]a�      �]	]
`    Mc         
   $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a*�  x�  �D�]�
a      �]a�      �]]
`    Ma   
   �`    Md               $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a��  
�  �D�]�
a      �]a�      �]!]
`    Mb         $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a��  >�  �D�](�
a      �]a�     Aa�      �]19]
`    Md      
         ] !�$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a|�  
�  �D�]�
a      �]a�      �]�$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�  � �D�]�
a      �]a�      �]�$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a� 
 �D�]�
a      �]a�      �]�$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a� j �D�]�
a      �]a�      �])=$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a$ � �D�]�
a      �]a�      �]U�=R=�$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a@ � �D�]�
a      �]a�      �]�$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
ar � �D�]L�
a      �]a�      !�a�      �a�      a�a�   
    ��a�     �]=!!%!-!5!$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�  D! �D�]�
a      �]a�      �]�!�!l�a2      �`    e`   RbE"i   Qb  `   �	`   Y`   U`
   `   �`   Rb�G�   Pb  `   �`   �`   �`   $�a      �`    �`   �`   $La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
aJ% & �D�](�
a      �]a�     ea�      �]�!�!$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
aN' @( �D�](�
a      �]a�     ia�      �]�!"$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
aT( P* �D�]X�
a      �]a�     Q�a�     ��a�     �a�  
   ua�     �a�      �]"""%"-"5"$La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a+ h, �D�]@�
a      �]a�     �a�     ua�     �aa�   
   �]E"M"U"]"�"$La   
   X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a"/ X0 �D�]4�
a      �]a�     �a�     ��a�      �]�"�"�"$La   
   X�a      �y`^
  ��`^  �}`~  ��	`�  u
al0 �1 �D�]4�
a      �]a�     �a�     %#a�      �]###$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�1 p2 �D�](�
a      �]a�     �a�      �])#5#$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a~2 �2 �D�](�
a      �]a�     E#a�      �]I#=#]
`    Ma      $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�2 �3 �D�]�
a      �]a�      �]Q#Y#a#i#$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a@7 �7 �D�](�
a      �]a�     y#a�      �]}#q#$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�< := �D�]�
a      �]a�      �]�#]
`   M`    Rf�D�   google-auto-placed  �a
      �#`P    Rf��B   google_tag_origin   Rb*��   qs  $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a> .> �D�]�
a      �]a�      �]�#$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a@> b> �D�]�
a      �]a�      �]�#$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
ar> �> �D�]�
a      �]a�      �]�#$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�> �> �D�]�
a      �]a�      �]�#]
`    Me
                  $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�> ? �D�]�
a      �]a�      �]�#$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a.? |? �D�]�
a      �]a�      �]�#]
`    Ma      $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�A  B �D�]�
a      �]a�      �]�#$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
aB vB �D�]�
a      �]a�      �]$$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a~B �B �D�]�
a      �]a�      �]%$$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
apC �C �D�]�
a      �]a�      �]=$]
`   M`   ��`   M`
   RZ]��  A7CQXglZzTrThjGTBEn1rWTxHOEtkWivwzgea+NjyardrwlieSjVuyG44PkYgIPGs8Q9svD8sF3Yedn0BBBjXAkAAACFeyJvcmlnaW4iOiJodHRwczovL2RvdWJsZWNsaWNrLm5ldDo0NDMiLCJmZWF0dXJlIjoiUHJpdmFjeVNhbmRib3hBZHNBUElzIiwiZXhwaXJ5IjoxNjk1MTY3OTk5LCJpc1N1YmRvbWFpbiI6dHJ1ZSwiaXNUaGlyZFBhcnR5Ijp0cnVlfQ==%R!�|�  A3vKT9yxRPjmXN3DpIiz58f5JykcWHjUo/W7hvmtjgh9jPpQgem9VbADiNovG8NkO6mRmk70Kex8/KUqAYWVWAEAAACLeyJvcmlnaW4iOiJodHRwczovL2dvb2dsZXN5bmRpY2F0aW9uLmNvbTo0NDMiLCJmZWF0dXJlIjoiUHJpdmFjeVNhbmRib3hBZHNBUElzIiwiZXhwaXJ5IjoxNjk1MTY3OTk5LCJpc1N1YmRvbWFpbiI6dHJ1ZSwiaXNUaGlyZFBhcnR5Ijp0cnVlfQ==%R!F���  A4A26Ymj79UVY7C7JGUS4BG1s7MdcDokAQf/RP0paks+RoTYbXHxceT/5L4iKcsleFCngi75YfNRGW2+SpVv1ggAAACLeyJvcmlnaW4iOiJodHRwczovL2dvb2dsZXRhZ3NlcnZpY2VzLmNvbTo0NDMiLCJmZWF0dXJlIjoiUHJpdmFjeVNhbmRib3hBZHNBUElzIiwiZXhwaXJ5IjoxNjk1MTY3OTk5LCJpc1N1YmRvbWFpbiI6dHJ1ZSwiaXNUaGlyZFBhcnR5Ijp0cnVlfQ==R�{  As0hBNJ8h++fNYlkq8cTye2qDLyom8NddByiVytXGGD0YVE+2CEuTCpqXMDxdhOMILKoaiaYifwEvCRlJ/9GcQ8AAAB8eyJvcmlnaW4iOiJodHRwczovL2RvdWJsZWNsaWNrLm5ldDo0NDMiLCJmZWF0dXJlIjoiV2ViVmlld1hSZXF1ZXN0ZWRXaXRoRGVwcmVjYXRpb24iLCJleHBpcnkiOjE3MTk1MzI3OTksImlzU3ViZG9tYWluIjp0cnVlfQ==R"N�  AgRYsXo24ypxC89CJanC+JgEmraCCBebKl8ZmG7Tj5oJNx0cmH0NtNRZs3NB5ubhpbX/bIt7l2zJOSyO64NGmwMAAACCeyJvcmlnaW4iOiJodHRwczovL2dvb2dsZXN5bmRpY2F0aW9uLmNvbTo0NDMiLCJmZWF0dXJlIjoiV2ViVmlld1hSZXF1ZXN0ZWRXaXRoRGVwcmVjYXRpb24iLCJleHBpcnkiOjE3MTk1MzI3OTksImlzU3ViZG9tYWluIjp0cnVlfQ==$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
aDR �S �D�]�
a      �]a�      �]Q$$�a      Rd��,	   rectangle   `   Rd��7�
   horizontal  `   Rcv��   vertical`   L�a"      %H`   Re��,�   interstitials   `   Rdz�Yt	   vignettes   `   Rcҋ(v   inserts `   Rd�/�
   immersives  `   Rd̜�	   list_view   `
   RdR�a	   full_page   `   Rd.@��
   side_rails  `   %%-%M%U%i%�%�%�%�%�%�%�%$La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a$o 4p �D�]@�
a      �]a�     &a�      ��a�       �a�  
   �]�%&%&9&M&a&�&�&�&�&'��aR      �/G�4G�1G$Rg"��   google_ad_host_channel  GiFG4Rk.h�0'   google_tag_for_child_directed_treatment G0RjZP*�#   google_tag_for_under_age_of_consent G Rf~�A�   google_tag_partner  G,Ri�<��   google_restrict_data_processing GRe��9�   google_page_url G Rfj3�   google_debug_params G Rf�g   google_shadow_mode  G-HG$RgJ鐲   google_ad_frequency_hintG,Ri:d	   google_admob_interstitial_slot  G(Rh6��w   google_admob_rewarded_slot  G$Rg���   google_admob_ads_only   G(RhN��   google_max_ad_content_ratingG$Rg�)   google_traffic_source   GHG$Rg�j�s   (^| )adsbygoogle($| )   $La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a�z �z �D�]�
a      �]a�      �]1'$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�z �z �D�]�
a      �]a�      �]9'$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a{ V{ �D�](�
a      �]a�       ��a�     �]I'A'$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
af{ �{ �D�]�
a      �]a�      �]Q'$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�{ �{ �D�]�
a      �]a�      �]Y'$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�{ �{ �D�]�
a      �]a�      �]a'$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�{ | �D�]�
a      �]a�      �]i'$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a.| P| �D�]�
a      �]a�      �]q'$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a`| �| �D�](�
a      �]a�     �'a�      �]�'y']
`    Mc            $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�|  } �D�]�
a      �]a�      �]�']
`    Md      
         $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a0} R} �D�]�
a      �]a�      �]�'$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
ab} �} �D�](�
a      �]a�       ��a�     �]�'�']
`    Mb         $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�} 0~ �D�](�
a      �]a�       ��a�     �]�'�']
`    Mb         $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
aV~ x~ �D�]�
a      �]a�      �]�']
`    Md         
      $La   
   X�a      �y`^
  ��`^  �}`~  ��	`�  u
a*� >� �D�]4�
a      �]a�     �'a�     	a�      �]�'�'($La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a@� *� �D�]p�
a      �]a�     �
a�      a�a�     Qa�  
    �a�     I
a�     �a�      a�a�      �]	((!()(1(9(A(I(Q(Y(�(0�j                                    0�j                                   �(�($La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a�� � �D�]�
a      �]a�      �]))))%)5)$La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a�� J� �D�]�
a      �]a�      �]-)9)M)q)�)�)�)$La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a
� ^� �D�]�
a      �]a�      �]�)$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a� T� �D�]�
a      �]a�      �]�)**$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�� � �D�]�
a      �]a�      �]9*$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a0� R� �D�]�
a      �]a�      �]Q*$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
aP� �� �D�]�
a      �]a�      �]�*�,Ri�f�_    google_prev_ad_formats_by_region0Rj�@�"   google_prev_ad_slotnames_by_region  �*��*u�*RcJ��G   isEmpty +�a$La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a2� �� �D�]�
a      �]a�      �]!+]
`    Ma      )+$La   
   X�a      �y`^
  ��`^  �}`~  ��	`�  u
a"� @  �D�]4�
a      �]a�     Ya�     �+a�      �]y+�+�+�+�+$La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a�   �D�]�
a      �]a�      �]�,]
`    Mc            $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a* L �D�]�
a      �]a�      �]�,]
`    Mc            $La   
   X�a      �y`^
  ��`^  �}`~  ��	`�  u
av � �D�]4�
a      �]a�      ��a�       �a�     �]�,�,�,$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a Z �D�]�
a      �]a�      �]	-$La   
   X�a      �y`^
  ��`^  �}`~  ��	`�  u
ax � �D�]4�
a      �]a�     �a�      a�a�      �]-!-)-$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a� � �D�]�
a      �]a�      �]1-$�a       !�`     !�`    a�`    !� !� a�$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
al � �D�](�
a      �]a�     I-a�      �]9-A-]
`    Ma      $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a� � �D�]�
a      �]a�      �]u-$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a  N �D�]�
a      �]a�      �]}-$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a� " �D�](�
a      �]a�     I-a�      �]�-�-$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a2 � �D�]�
a      �]a�      �]�-$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a� 8 �D�]�
a      �]a�      �]�-]
`    XMt(                                                               Q��- !���$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a$  F  �D�]�
a      �]a�      �]�-$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
aP% 0& �D�]�
a      �]a�      �].%.-.$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
aJ, �, �D�](�
a      �]a�       ��a�     �]=.5.$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�, 
- �D�](�
a      �]a�       ��a�     �]M.E.$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a- <- �D�]�
a      �]a�      �]U.$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
aL- n- �D�]�
a      �]a�      �]].]
`    Ma      5�`   L`   �`   Ma      Rb�$�   usca�`   Ma      Rb^ܵK   usva�`   Ma      Rb���V   usco�`   Ma      Rb�L��   usct!�y.���.$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�B �C �D�](�
a      �]a�     �.a�      �]�.�.�.�.$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
ahI �I �D�]�
a      �]a�      �]�.$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�I "J �D�](�
a      �]a�       ��a�     �]/�.$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a2J �J �D�](�
a      �]a�       ��a�     �]//]
`    Ma      !/$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
ar[  _ �D�]�
a      �]a�      �]}/�a
      �/G�1G�/�/�/�/�/�/�/�/$La   
   X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a�h zi �D�]4�
a      �]a�      ��a�       �a�     �]�/�/�/$La   
   X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�i j �D�]4�
a      �]a�      ��a�       �a�     �]0	00]
`    Ma      $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a0j �j �D�](�
a      �]a�     E#a�      �]10!0]
`    Ma      $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�j �j �D�]�
a      �]a�      �]A0]
`    Ma      $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�j k �D�]�
a      �]a�      �]Q0]
`    Ma      $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a0k �k �D�]@�
a      �]a�      ��a�      �a�       !�a�  
   �]�0a0q0�0$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�k l �D�]�
a      �]a�      �]�0]
`    Md               $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
aDl �m �D�]L�
a      �]a�      �a�      a�a�      ��a�   
    !�a�     �]�0�0�0�0�0]
`    Ma   &   �`    Mb         �`    Mc            �1�1�1�192a2i2�2$La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a� �� �D�]�
a      �]a�      �]�3�3�3�3$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a� |� �D�]�
a      �]a�      �]	4$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
aʗ f� �D�]�
a      �]a�      �]M4$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
ax� F� �D�]�
a      �]a�      �]�4�4$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a� T� �D�]�
a      �]a�      �]5,�a      -5G$R ��`#   google_analytics_uacct  G%GG�9G�5�5�5<�a      Rc
��   120x90  GRc���   160x90  GRc���   180x90  GRc�9�U   200x90  GRc΂k�   468x15  GRcf��h   728x15  G$La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a� �� �D�](�
a      �]a�     I-a�      �]56E6$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a@� �� �D�]�
a      �]a�      �]�6]
`    Mb         dRw���EW   platform platformVersion architecture model uaFullVersion bitness fullVersionList wow64 $La   
   X�a      �y`^
  ��`^  �}`~  ��	`�  u
az� �� �D�]4�
a      �]a�     Ma�      �a�      �]�9::T�a&      �9Ya��W��?Reꯙ�   image_sidebysideYa��W��?,Rif{6|   mobile_banner_image_sidebyside  Ya��W��?(Rh�|�g   pub_control_image_stacked   Ya��W��?(Rhf���   pub_control_image_sidebysideYa��W��?,Ri���   pub_control_image_card_stacked  Ya��W��?0Rj/!   pub_control_image_card_sidebyside   Ya�b���?RevO�   pub_control_text`    $Rg�#,�   pub_control_text_card   `    T�a&      �`�   �U`    �U`    �U`�   �U`    �U`�   �U`    �U`�   �U`�   <�a      �`�   �`�  �`,  �`�  �`�   �`,  m$La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a� n �D�](�
a      �]a�     �a�      �]�:�:]
`   M`   ==8Rl�Y"])   google_content_recommendation_columns_num   4Rk�
ԑ&   google_content_recommendation_rows_num  $La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a� * �D�](�
a      �]a�       ��a�     �];;;%;$La   
   X�a      �y`^
  ��`^  �}`~  ��	`�  u
af h �D�]4�
a      �]a�      ��a�       �a�     �]M;=;E;4�a      a;Cu;C�;C�;C�;CY;�e;�y;��;��;�$La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a� � �D�](�
a      �]a�       ��a�     �]�;�;�;�;�;]
`    LLq"                                                                       �`    LLq"                                                                       <9<m<�<==%=-=U=m=�=$La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a�e Rh �D�]L�
a      �]a�      !�`V  Q
`   F a�`V  �a   
    !�`V  �`   F�D�]�>?%?1?=?@!@)@1@E@M@U@i@}@�@�@�@�@]
`    Mc         (   $La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
a� �� �D�]�
a      �]a�      �]�A$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a
� V� �D�]�
a      �]a�      �]BYC$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
at� <� �D�]�
a      �]a�      �]�C�C$�a      C�CC�C�	D�D�$La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
az� L� �D�]�
a      �]a�      �]!D$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
ab� �� �D�]�
a      �]a�      �]MDUD$�a      C�CC]DiDuD$La      X�
a      �y`^
  ��`^  �}`~  ��	`�  u
aΚ <� �D�]�
a      �]a�      �]�D�D$La      X�a      �y`^
  ��`^  �}`~  ��	`�  u
a�� �� �D�]�
a      �]a�      �]�E�I�8qJ�D�=          �  =� %�%�%�%�%�%
�%�%�	%�
	%�
%%�%�%�%�%�%�%�%�%�%�%���%�%�%�% �%!�%"�%#�%$�%%� %&�! %'�"!%(�#"%*�$#%+�%$%,�&%%-%.�'&%/�('%0�)(%1�*)%2�+*%3�,+%4�-,%5�.-%7%8�/.%:�0/%;�10%<�21%=�32%>%?�43%@�54%A�65%B�76%C�87%D�98%E�:9%F�;:%G�<;%H�=<%I%J%K�>=%L�?>%M�@?%N�A@%O�BA%P�CB%Q�DC%R�ED%S�FE%U�GF%V�HG%W�IH%X�JI%Y�KJ%Z�LK%[�ML%\�NM%]�ON%^�PO%_�QP%`�RQ%a�SR%b�TS%c�UT%d�VU%e�WV%f�XW%g�YX%h�ZY%i�[Z%j�\[%k�]\%l�^]%m�_^%n�`_%o�a`%p�ba%q�cb%r�dc%s�ed%t�fe%u�gf%v�hg%w�ih%x�ji%y�kj%z�lk%{�ml%|�nm%}%~%�on��po%��qp%��rq��sr%��ts%��ut%��vu%��wv%��xw��yx%��zy%��{z%��|{%��}|%��~}%��~%���%����%����%����%����%�%����%����%����%����%����%����%����%����%����%�%�%����%����%����%�%�%����%����%����%�%�%�%ȁ��%Ɂ��%ʁ��%�%�%�%ρ��%Ё��%с��%ҁ��%Ӂ��%ԁ��%Ձ��%�%ׁ��%؁��%߁��%����%ၢ�%⁣�%つ�%ꁥ�%쁦�%큧�%�%����%񁪩%򁫪%����%����%����%���� %��� %��� %��� %��� %��� %��� %��� %��� %������� %��� %��� %��� %��� %��� %��� %��� %��� %��� %��� %��� %��� % ��� %%��� %& %'��� %(��� %) %*��� %+��� %,��� %-��� %.��� %/��� %0��� %1��� %2��� %3��� %4��� %5��� %6��� %7��� %8��� %9��� %: %;��� %<��� %= %> %? %@ %A��� %B��� %D %F��� %G %H��� %I��� %J��� %K��� %L��� %M��� %N��� %O %P %Q %R %T %U %V %W��� %Y��� %Z��� %[ %\ %]��� %d��� %���� %���� %���� %���� %���� %���� %���� %���� %� %� %� %� %���� %���� %���� %���� %���� %� %� %� %���� %���� %���� %���� %���� %���� %� %� %� %� %� %� %� %� %� %���� %���� %� � �  %� �  %� � %� � %� � %� � %� � %� %� � %� � %� �	 %� %� �
	 %� �
 %� � %� � %� � %� � %� � %� %� %� � %� � %� � %� %� � %� � %� � %� � %� � %� %� � % � % � % � % � % � % � % % �  % % % �!  % �"! % �#" % �$# % �%$ % �&% % �'& % % %  �(' %$ �)( %% �*) %' �+* %( �,+ %) �-, %* %+ %- �.- %2 �/. %3 %4 �0/ %5 �10 %6 �21 %8 �32 %9 �43 %> �54 %? �65 %@ �76 %A �87 %B �98 %C �:9 %D �;: %E %F %G %J %K %N �<; %P %Z �=< %[ �>= %\ �?> %] �@? %^ �A@ %_ �BA %` �CB %a �DC %b %c �ED %h �FE %i �GF %j �HG %l �IH %m %n %o �JI %q �KJ %r �LK %s �ML %t �NM %u �ON %v �PO %w �QP %x �RQ %y %z %| %} �SR %~ �TS % �UT %� %� �VU %� �WV %� �XW %� �YX %� %� �ZY %� �[Z %� %� �\[ %� �]\ %� �^] %� �_^ %� �`_ %� �a` %� �ba %� �cb %� �dc %� �ed %� �fe %� �gf %� �hg %� �ih %� �ji %� �kj %� �lk %� %� �ml %� �nm %� �on %� �po %� �qp %� �rq %� �sr %� �ts %� �ut %� �vu %� �wv %� �xw %� �yx %� �zy %� �{z %� %� %� %� %� �|{ %� �}| %� %� �~} %� %� %� �~ %� �� %� %� %� %� %� %� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� %� ��� %� ��� %� ��� %� ��� %� ��� %� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� ��� %� % % % % ��� %	 ��� %
 ��� % ��� % ��� % ��� % ��� % ��� % ��� % ��� % ��� % ��� % ��� % ��� % ��� % ��� % % % % %  ��� %! ��� %" ��� %# ��� %$ ��� %% ��� %& ��� %' ��� %( ��� %) ��� %* ��� %+ ��� %, ��� %- ��� %. ��� %/ %0 ��� %1 ��� %2 ��� %3 ��� %4 ��� %5 ��� %6 ��� %7 ��� %8 ��� %9 ��� %: ��� %; ��� %< ��� %= ��� %>~� �	  !�  % �� !� � -��� �]��	F ʚ;   O 8�%%	 !� �i�� � -��� �]��%� ��b��� � �a$/�� �	 � � %� -��� � �  -���  �  � %(� ��� 2��� �a��� ��b��T �
 �a�!�a�#a�%%) !�' �a�)%.~%6%8|+�.�'4��, !�. � -���0 �^���2%9 y�4   %? !�' �a�5%J%K "�7   �  !�9  �
  ���%T� ���� �� ������e* ��� -���; �6 2���=  ���%~% ���� ������e* ��%� ���� ������e* ��%��� z�? % 2���@ ~%�~%�� ���� �� ������e* ��%� y�B   %�~%�� ���� �� ������e* ��%��� -���C � ��� 2���E �� -���C � ��� 2���G �� -���C � ��� 2���I �� -���C � ��� 2���K �� -���C � � � 2��M  !O � �b��Q%� yS   %� � -��T � ��^���V%� yX   %� yY   %� �	�%� �
��b��Z%� ��%�|\%� ��%� ��%� ��%� ��%� ��%� ��%�%� !] %� !_ %�%� ��%� ��%� ��%� ��%� ��%�� ��� ���e* ��%� ��%� !O � �b��a%�� ��� ���e* ��%�� � �� ���e* ��%�%�� �"�� !���e* ��%�� -��#c %�� �/ �� -��$e  � �� -��%g  � �� -��&i x%� �'��b��k%�� �)�� (� �*�� �+����e* ���� �-�� ,���e* ��%�� �/�� .� �0�� �1�� �2�� �3�� �4�� �5 � �6���e* �
�%� �7%� �8%� �:� 9�����e* ��%��� z;m % 2���n  z<p %%� z=q %%� �?� >�����e* ��%��� z@r % 2���s  �B� A� �C�����e* ��%� �E� D�����e* ��%���b��u%��� zFw % 2���x  zGz %%� �I	� H�����e* ��%��� zJ{ % 2���|  �L
� K� �M�����e* ��%� �O� N� �P� �Q�����e* ��%� �S� R� �T�����e* ��%� �V� U�����e* ��%� �X� W�����e* ��%� �Z� Y� �[�����e* ��%� �]� \�����e* ��%� �_� ^�����e* ��%� z`~ %%� �b� a�����e* ��%� �d� c�����e* ��%� �f� e�����e* �� %  zg % % �i� h�����e* �� % � zj� % 2����  �l� k�����e* �� % � zm� % 2����  zn� % %	 �p� o�����e* �� %
 
� zq� % 2����  �s� r� �t�����e* �� % zu� % % �v % -���� � 2��w�  �y � x�����e* �� %� �{!� z���e* ��� �}"� |�����e* �� %� �#� ~���e* �� %! ��$ %"� ��%� ����e* �� %# y��   � -���� � ��^��� %$ ��&�b�� %'� ��'� ����e* �� %*� ��(� �� ��)� ��*� ��+� ��,���e* �� %;� ��-� ����e* �� %> %? %@ !_ ��i��� %A ��.�a� }�� )� }�� )�� ��/� �� ��0���e* �� %C� ��1� �� ��2���e* �� %E� ��3� �� ��4� ��5� ��6� ��7� ��8���e* �� %F� ��9� �� ��:� ��;� ��<���e* �� %H ���%� ��=� �� ��>� ��?���e* ��%� %P� ��@� �� ��A� ��B���e* �� %Q� ��C� �� ��D���e* �� %R ��E� �� ��F�����e* �� %S S� z�� % 2���� � ��G� ����e* �� %T ��H %U ��I %V ��J %W ��K� �� ��L�����e* �� %X� ��M� ����e* �� %\ \� z�� %� }�� )��i��� %] ��N� ������e* �b�� %^ ��O� ������e* �� %_ ��P� ������e* �� %` ��Q� ������e* �� %a a� z�� % 2����  ��R� ������e* �� %b ��S� ������e* �� %c c� z�� % 2���� � ��T� ����e* ���� ��U� ����e* ���� ��V� ����e* ��� :���i��� %e ���i��� %f j�d��i��� %g� ��W� ����e* ���i��� %h ����i��� %i ����i��� %j ���i��� %k <��i��� %l ����i��� %m 9��i��� %n q��i��� %o ��i��� %p 	��i��� %q ��i��� %r ��i��� %s ���i��� %t ���i��� %u ���i��� %v � ��i��� %w ��i��� %x2�/!� z�� %��i��� %ykN!��i��� %z�n����i��� %{3�6��i��� %|��5��i��� %} `��i��� %~ 7���i��� % '��i��� %� �� z�� %��i��� %� � ��i��� %�T��i��� %�� ��X� ����e* �� %� }�� ) %� }�� ) %� ��Y %� ��Z %� ��[ %� ��\ %� ��] %� ��^ %� ��_ %� ��` %� ��a %� ��b %� ��c %� ��d %�� ��e� �� ��f� ��g� ��h���e* �� %� ��i %� ��j %� ��k %� ��l %� ��m %� ��n %� ��o %� }�� ) %� !O � ��b��� %� ��p� ������e* �� %� ��q� ������e* �� %� ��r� �� ��s�����e* �� %� � t� ������e* �� %� �u� �����e* �� %� �v� �����e* �� %� �w� �����e* �� %� �x� �����e* �� %� �
y� 	� �z�����e* �� %� z� % %� �{� �����e* �� %� �� z� % 2����  �|� �����e* �� %� �}� � �~�����e* �� %� z� % %� �� � �������e* �� %� �� z� % 2����  ��� �����e* �� %� ��b��� %� �� z� % 2���� � ��� � ��� � ����e* �� %�� �"�� !� �#�� �$�� �%�� �&�� �'�� �(�� �)����e* �
� %� �*� %� �+� %� �,� %� }-� ) %� }.� ) %� �/� %� �0� %�� �2�� 1���e* �� %� �3� %� �4� %� �5� %� �6� %� �7�% !9� � �:�� 8���e* ��%� %� %� %��� i����  %� �;� %� �<�� a�� �=� %� �>� %� �?� %� �@� %�� �B�� A���e* �� %�� �D�� C���e* �� %� �E� %� �F� %�� �H�� G���e* �� %� �J�� I�����e* � b���� %�� �L�� K���e* �� %� %� }M)�s� N 7���� 	s� O 7���� 
� %� �� -���� �P� 2��Q �� -���� �R� 2��S �� -���� �T� 2��U �� -���� �V� 2��W �Y�� X�����e* �� %� �� zZ% 2��� �[� %� �]�� \� �^�� �_������e* �� %� �`� %  �a� % �c�� b�����e* �� %	 	� zd% 2��� �f�� e�����e* �� %
 
� zg% 2��� �i�� h� �j�� �k������e* �� %� �m�� l���e* �� %� �o�� n� �p�� �q����e* �� % �s�� r�����e* �� % }t) % }M )�s� u 7���� !s� v 7���� #s� w 7���� %� % �y�� x� �z������e* �� % � z{'% 2���( �}�� |�����e* �� % ��� ~�����e* �� % ���� �� ��������e* �� % ���� ������e* �� % ���� ������e* �� % z�*% % � -���+� ���� ^������- %  �� -���/� ]����1 %! b����3 %" b����5 %# ���� ������e* �� %&� ���� ����e* �� %+ ��� %, ��� %- ���� �� ��������e* �� %. ���� �� ��������e* �� %/ ���� ������e* �� %0 ���� ������e* ��� b����7 %1 z�9% 2���: !�<� z�>�� i���� ?{� -���A� ���� ^������C� -���E� ���� ^������G � �� b����I� b����K %4� ���� �� ������e* �� %7 b����M� -���O� ���� ^������Q b����S� -���U� ���� ^������W� ���� ����e* �� %: ���� �� ��������e* �� %; ���� �� ��������e* ��� b����Y %< z�[% 2���\ ��� %=� ���� ����e* �� %F }�^) %G ��� %H ��� %I ��� %J ��� %K ��� %L ��� %M ��� %N ��� %O ���� �� ���� ��������e* �� %Q ���� �� ���� ��������e* �� %R R� z�_% 2���` ���� �� ��������e* �� %S S� z�b% 2���c ���� ������e* �� %T T� z�e% 2���f ���� ������e* �� %U U� z�h% 2���i ���� �� ���� ���� ��������e* �� %V ���� ������e* �� %W W� z�k% 2���l ���� �� ���� ���� ���� ��������e* �� %X X� z�n% 2���o z�q% %Y %Z z�r% %c ��� %d ��� %e ��� %f ��� %g ��� %k ��� %n ��� %o ��� %p� ���� ����e* �� %z ��� %{ ��  %| �� %}� �	  a��s� ��� ����e* �� %�� ��� ����e* �� %�� ��� ����e* �� %� �� %�� ��� ����e* �� %� }�u) %� �� %� �� %� ��	 %� }�v) %� ��
� �� �������e* �� %� � � ������e* �� %� �� zw% 2���x � -��z� �� ^������| %� �� �� � �� ����e* �� %� }~) %� }) %� }	�) %�(� 
� b����� %�� �� � ����e* �� %� z�% %� �� �� � ����e* �� %� � %� � %� �� �� � �� ����e* �� %� }�)� � 3��� � 3��� � 3��� � 3�� � �! 3��"�� %� �� �$� #� �%���e* �� %� �&  %� �'! %� �(" %� z)�%�� �� ��Z��� i���� � 6������ �� ��Z��� i���� � 6������ �� ��<��� i���� � 6������ �� P� ��� i���� � 6������ �� @�d��� i���� � 6������ �� @�2��� i���� � 6������ �� ,� X��� i���� � 6������ �� ,� � ��� i���� � 6������ �� � � � ��� i���� � 6�����	� �� � �<��� i���� � 6�����
� �� � � � ��� i���� � 6������ �� � � � ��� i���� � 6������ �� � � X��� i���� � 6������ ��}�}��� i���� � 6������ ��x� X��� i���� � 6������ ��x� � ��� i���� � 6������ ��x�x���� i���� � 6������� z*�%�� /��� 6������ /��� 6������ /��� 6������ /��� 6������ /��� 6������ /��� 6������ /��� 6������ /��� 6������
 /��� 6�����	� /��� 6�����
� /��� 6������ /��� 6������ /��� 6������ /��� 6������ /��� 6������	 /��� 6������ /��� 6������ %� �+# %� �,$ %� �-% %� �.& %� �/' %� �0( %� �1) %� �2* %� �3+ %� �4, %� �5- %�� �7.� 6� �8/� �90� �:1� �;2���e* �� %� }M�)�s� }M�)�s� �<3 7�����	s� �=4 7�����
s� �>5 7�����s� �?6 7�����s� �@7 7�����<s� �A8 7�����Es� �B9 7�����Fs� �C: 7������ 7���� �s� }M�)�s� �D; 7�����s� �E< 7������ 7���� �s� }M�)�s� �F= 7�����s� �G> 7�����s� �H? 7������ 7���� �� %� zI�% %�� �K@� J���e* �� %�� �MA� L���e* �� %� �NB %� �PC� O�����e* �� %  �QD % }R�)� �SE 3��T� �UF 3��V �WG 3��X� % �ZH� Y�����e* �� % �\I� [�����e* � b���� % �]J % }^)� �_K 3��T �`L 3��V
 �aM 3��X� % �cN� b�����e* �� % �dO %� �fP� e���e* �� % | % % | % !�<� i����   %� %  %0 �gQ� h�  �  � � �iR� `���� �   q�m  S  @� ��� 
��� � `:` 0	� 
`�� ��`�� &0'L��
��`.0's&̹ s�&̹ ��&@ 0's�9	��� s� � � � � � `@ @ @ � `�I s.0'0`N� &HP ,� �&�&S	$`.0��� `.` ��� � 
� 
� &̹ s�� s�� s�9	`ι 0'��9���I @ @ @ @ @ 0�     &S	$@	$@	$`J L	$@	&���� 00'  �e    @    N1      ���4R4���4  [2021,"r20230920","r20190131",null,null,null,null,".google.mg",null,null,null,[[[1082,null,null,[1]],[null,1130,null,[null,100]],[1270,null,null,[1]],[null,1032,null,[null,200],[[[12,null,null,null,4,null,"Android",["navigator.userAgent"]],[null,500]]]],[1247,null,null,[1]],[1252,null,null,[1]],[null,1224,null,[null,0.01]],[null,1159,null,[null,500]],[1122,null,null,[1]],[1207,null,null,[1]],[null,1263,null,[null,-1]],[null,1265,null,[null,-1]],[null,1264,null,[null,-1]],[null,66,null,[null,-1]],[null,65,null,[null,-1]],[1241,null,null,[1]],[1287,null,null,[1]],[1285,null,null,[1]],[1300,null,null,[1]],[null,null,null,[null,null,null,["en","de"]],null,1273],[1223,null,null,[1]],[null,null,null,[null,null,null,["44786015","44786016"]],null,1261],[1167,null,null,[1]],[1129,null,null,[1]],[1199,null,null,[1]],[1161,null,null,[1]],[null,1072,null,[null,0.75]],[1101,null,null,[1]],[1198,null,null,[1]],[1206,null,null,[1]],[1147,null,null,[1]],[1190,null,null,[1]],[null,1245,null,[null,3600]],[null,566560958,null,[null,30000]],[null,506864295,null,[null,300]],[null,508040914,null,[null,100]],[null,547455356,null,[null,49]],[null,null,null,[null,null,null,["1"]],null,556791602],[529362570,null,null,[1]],[null,null,549581487,[null,null,"#1A73E8"]],[null,null,549581486,[null,null,"#FFFFFF"]],[null,564316610,null,[null,0.2]],[null,561668774,null,[null,0.1]],[null,469675170,null,[null,30000]],[567362967,null,null,[1]],[null,1085,null,[null,5]],[null,63,null,[null,30]],[null,1080,null,[null,5]],[1086,null,null,[1]],[null,1027,null,[null,10]],[null,57,null,[null,120]],[null,1079,null,[null,5]],[null,1050,null,[null,30]],[null,58,null,[null,120]],[10005,null,null,[1]],[1033,null,null,[1]],[555237685,null,null,[1]],[null,472785970,null,[null,500]],[null,null,null,[null,null,null,["A7CQXglZzTrThjGTBEn1rWTxHOEtkWivwzgea+NjyardrwlieSjVuyG44PkYgIPGs8Q9svD8sF3Yedn0BBBjXAkAAACFeyJvcmlnaW4iOiJodHRwczovL2RvdWJsZWNsaWNrLm5ldDo0NDMiLCJmZWF0dXJlIjoiUHJpdmFjeVNhbmRib3hBZHNBUElzIiwiZXhwaXJ5IjoxNjk1MTY3OTk5LCJpc1N1YmRvbWFpbiI6dHJ1ZSwiaXNUaGlyZFBhcnR5Ijp0cnVlfQ==","A3vKT9yxRPjmXN3DpIiz58f5JykcWHjUo/W7hvmtjgh9jPpQgem9VbADiNovG8NkO6mRmk70Kex8/KUqAYWVWAEAAACLeyJvcmlnaW4iOiJodHRwczovL2dvb2dsZXN5bmRpY2F0aW9uLmNvbTo0NDMiLCJmZWF0dXJlIjoiUHJpdmFjeVNhbmRib3hBZHNBUElzIiwiZXhwaXJ5IjoxNjk1MTY3OTk5LCJpc1N1YmRvbWFpbiI6dHJ1ZSwiaXNUaGlyZFBhcnR5Ijp0cnVlfQ==","A4A26Ymj79UVY7C7JGUS4BG1s7MdcDokAQf/RP0paks+RoTYbXHxceT/5L4iKcsleFCngi75YfNRGW2+SpVv1ggAAACLeyJvcmlnaW4iOiJodHRwczovL2dvb2dsZXRhZ3NlcnZpY2VzLmNvbTo0NDMiLCJmZWF0dXJlIjoiUHJpdmFjeVNhbmRib3hBZHNBUElzIiwiZXhwaXJ5IjoxNjk1MTY3OTk5LCJpc1N1YmRvbWFpbiI6dHJ1ZSwiaXNUaGlyZFBhcnR5Ijp0cnVlfQ==","As0hBNJ8h++fNYlkq8cTye2qDLyom8NddByiVytXGGD0YVE+2CEuTCpqXMDxdhOMILKoaiaYifwEvCRlJ/9GcQ8AAAB8eyJvcmlnaW4iOiJodHRwczovL2RvdWJsZWNsaWNrLm5ldDo0NDMiLCJmZWF0dXJlIjoiV2ViVmlld1hSZXF1ZXN0ZWRXaXRoRGVwcmVjYXRpb24iLCJleHBpcnkiOjE3MTk1MzI3OTksImlzU3ViZG9tYWluIjp0cnVlfQ==","AgRYsXo24ypxC89CJanC+JgEmraCCBebKl8ZmG7Tj5oJNx0cmH0NtNRZs3NB5ubhpbX/bIt7l2zJOSyO64NGmwMAAACCeyJvcmlnaW4iOiJodHRwczovL2dvb2dsZXN5bmRpY2F0aW9uLmNvbTo0NDMiLCJmZWF0dXJlIjoiV2ViVmlld1hSZXF1ZXN0ZWRXaXRoRGVwcmVjYXRpb24iLCJleHBpcnkiOjE3MTk1MzI3OTksImlzU3ViZG9tYWluIjp0cnVlfQ=="]],null,1934],[1957,null,null,[1]],[null,1972,null,[]],[485990406,null,null,[]]],[[12,[[40,[[21065724],[21065725,[[203,null,null,[1]]]]],[4,null,9,null,null,null,null,["LayoutShift"]],71],[10,[[31061690],[31061691,[[83,null,null,[1]],[84,null,null,[1]]]]],null,61],[10,[[44769661],[44769662,[[1973,null,null,[1]]]]]]]],[13,[[500,[[31061692],[31061693,[[77,null,null,[1]],[78,null,null,[1]],[85,null,null,[1]],[80,null,null,[1]],[76,null,null,[1]]]]],[4,null,6,null,null,null,null,["31061691"]]],[1000,[[31067146,null,[4,null,70,null,null,null,null,["browsing-topics"]]]]],[1000,[[31067147,null,[2,[[4,null,70,null,null,null,null,["run-ad-auction"]],[4,null,70,null,null,null,null,["join-ad-interest-group"]]]]]]],[1000,[[31067148,null,[4,null,70,null,null,null,null,["attribution-reporting"]]]]],[1000,[[31067672,null,[2,[[4,null,69,null,null,null,null,["browsing-topics"]],[1,[[4,null,70,null,null,null,null,["browsing-topics"]]]]]]]]],[1000,[[31067673,null,[2,[[4,null,69,null,null,null,null,["join-ad-interest-group"]],[1,[[4,null,70,null,null,null,null,["join-ad-interest-group"]]]]]]]]],[1000,[[31067674,null,[2,[[4,null,69,null,null,null,null,["run-ad-auction"]],[1,[[4,null,70,null,null,null,null,["run-ad-auction"]]]]]]]]],[1000,[[31067675,null,[2,[[4,null,69,null,null,null,null,["attribution-reporting"]],[1,[[4,null,70,null,null,null,null,["attribution-reporting"]]]]]]]]],[1000,[[31068556,null,[4,null,70,null,null,null,null,["shared-storage"]]]]],[1000,[[31068557,null,[2,[[4,null,69,null,null,null,null,["shared-storage"]],[1,[[4,null,70,null,null,null,null,["shared-storage"]]]]]]]]],[50,[[44800311,null,[4,null,70,null,null,null,null,["attribution-reporting"]]],[44800312,null,[4,null,70,null,null,null,null,["attribution-reporting"]]]],null,77]]],[10,[[50,[[31067422],[31067423,[[null,1032,null,[]]]],[44776369],[44792510]],[3,[[4,null,8,null,null,null,null,["gmaSdk.getQueryInfo"]],[4,null,8,null,null,null,null,["webkit.messageHandlers.getGmaQueryInfo.postMessage"]],[4,null,8,null,null,null,null,["webkit.messageHandlers.getGmaSig.postMessage"]]]],69],[100,[[31076838],[31076839,[[542046278,null,null,[1]]]]]],[50,[[31076994],[31076995,[[1229,null,null,[1]]]],[31076996,[[1230,null,null,[1]]]],[31076997,[[1231,null,null,[1]]]],[31076998,[[1230,null,null,[1]],[1229,null,null,[1]],[1231,null,null,[1]]]]],null,87],[50,[[31077221],[31077222,[[1292,null,null,[1]]]]]],[100,[[31077327],[31077328,[[555237685,null,null,[]]]]]],[100,[[31077701],[31077702,[[1299,null,null,[1]]]]]],[100,[[31077969],[31077970,[[10009,null,null,[1]]]]]],[10,[[31077971],[31077972,[[1301,null,null,[1]]]]]],[1000,[[31077997,[[null,null,14,[null,null,"31077997"]]],[6,null,null,null,6,null,"31077997"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[1000,[[31077998,[[null,null,14,[null,null,"31077998"]]],[6,null,null,null,6,null,"31077998"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[10,[[31078019],[31078020]]],[1000,[[31078048,[[null,null,14,[null,null,"31078048"]]],[6,null,null,null,6,null,"31078048"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[1000,[[31078049,[[null,null,14,[null,null,"31078049"]]],[6,null,null,null,6,null,"31078049"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[1000,[[31078086,[[null,null,14,[null,null,"31078086"]]],[6,null,null,null,6,null,"31078086"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[1000,[[31078087,[[null,null,14,[null,null,"31078087"]]],[6,null,null,null,6,null,"31078087"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[1000,[[31078114,[[null,null,14,[null,null,"31078114"]]],[6,null,null,null,6,null,"31078114"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[1000,[[31078115,[[null,null,14,[null,null,"31078115"]]],[6,null,null,null,6,null,"31078115"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[1000,[[31078134,[[null,null,14,[null,null,"31078134"]]],[6,null,null,null,6,null,"31078134"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[1000,[[31078135,[[null,null,14,[null,null,"31078135"]]],[6,null,null,null,6,null,"31078135"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[10,[[31078142],[31078143,[[562770592,null,null,[1]]]]]],[10,[[31078144],[31078145,[[45414947,null,null,[1]]]]]],[null,[[31078148],[31078149,[[1275,null,null,[1]]]]]],[1,[[42531513],[42531514,[[316,null,null,[1]]]]]],[1,[[42531644],[42531645,[[368,null,null,[1]]]],[42531646,[[369,null,null,[1]],[368,null,null,[1]]]]]],[50,[[42531705],[42531706]]],[1,[[42532242],[42532243,[[1256,null,null,[1]],[290,null,null,[1]]]]]],[1,[[42532262],[42532263,[[null,1263,null,[null,16]]]],[42532264,[[null,1263,null,[null,4294967296]]]],[42532265,[[null,1265,null,[null,60]],[null,1264,null,[null,0.2]],[1266,null,null,[1]]]],[42532266,[[null,1263,null,[null,4294967296]],[null,1265,null,[null,60]],[null,1264,null,[null,0.2]],[1266,null,null,[1]]]],[42532267,[[null,1263,null,[null,16]],[null,1265,null,[null,60]],[null,1264,null,[null,0.2]],[1266,null,null,[1]]]],[42532268,[[1266,null,null,[1]]]]]],[1,[[42532360],[42532361,[[1260,null,null,[1]],[1291,null,null,[1]]]]],null,90],[1,[[42532362],[42532363]]],[50,[[42532402],[42532403,[[1267,null,null,[1]],[1268,null,null,[1]]]]]],[null,[[42532404],[42532405]]],[1,[[44719338],[44719339,[[334,null,null,[1]],[null,54,null,[null,100]],[null,66,null,[null,10]],[null,65,null,[null,1000]]]]]],[10,[[44776368],[44779257],[44784478]],[3,[[4,null,8,null,null,null,null,["gmaSdk.getQueryInfo"]],[4,null,8,null,null,null,null,["webkit.messageHandlers.getGmaQueryInfo.postMessage"]],[4,null,8,null,null,null,null,["webkit.messageHandlers.getGmaSig.postMessage"]]]],69],[10,[[44785292],[44785293,[[1239,null,null,[1]]]]]],[10,[[44785294],[44785295]]],[1,[[44795552],[44795553,[[1260,null,null,[1]]]]],null,90],[1,[[44795554],[44795555]]],[100,[[44795921],[44795922,[[1222,null,null,[1]]]],[44798934,[[1222,null,null,[1]]]]]],[10,[[44800658],[44800659,[[1185,null,null,[1]]]]],null,76],[50,[[44801484],[44801485,[[1298,null,null,[1]]]]]],[1,[[44801778],[44801779,[[506914611,null,null,[1]]]]],[4,null,55]],[1,[[44801992],[44801993,[[1289,null,null,[1]],[1288,null,null,[1]]]]]],[1000,[[44802674,[[506852289,null,null,[1]]],[12,null,null,null,2,null,"smitmehta\\.com/"]]],[4,null,55]],[10,[[44803789],[44803790,[[1233,null,null,[1]],[1185,null,null,[1]]]]],null,76],[50,[[44803791],[44803793,[[1185,null,null,[1]]]]],null,76],[10,[[44803792],[44803794,[[1185,null,null,[1]]]]],null,76]]],[17,[[25,[[31077857],[31077859,[[557143911,null,null,[1]],[541943501,null,null,[1]],[null,550718588,null,[null,250]],[null,1245,null,[null,600]]]]],null,null,null,null,null,325,null,120],[48,[[44796896,[[541943501,null,null,[1]],[null,1245,null,[null,600]]]]],[2,[[4,null,55],[7,null,null,15,null,20230711]]],null,null,null,null,440,null,120,1],[504,[[44796897,[[541943501,null,null,[1]],[null,1245,null,[null,600]]],[4,null,71,null,null,null,null,["120","14"]]]],[2,[[4,null,55],[7,null,null,15,null,20230711]]],null,null,null,null,488,null,120,1],[1,[[44797663,[[null,506871937,null,[null,44797663]],[1120,null,null,[1]]]],[44797664,[[null,506871937,null,[null,44797664]],[160889229,null,null,[1]],[1120,null,null,[1]]]]],[4,null,55],null,null,null,null,300,null,123],[24,[[44798321,[[null,506871937,null,[null,44798321]]]],[44798322,[[545453532,null,null,[1]],[null,506871937,null,[null,44798322]],[1120,null,null,[1]]]],[44798323,[[null,506871937,null,[null,44798323]],[1120,null,null,[1]]]]],[4,null,55],null,null,null,null,null,null,123],[14,[[44801877]],[2,[[4,null,55],[5,null,8,null,null,null,null,["localStorage"]],[4,null,8,null,null,null,null,["localStorage"]],[7,null,null,15,null,20230524]]],null,null,null,null,null,null,120,1],[14,[[44801878]],[2,[[4,null,55],[5,null,8,null,null,null,null,["localStorage"]],[4,null,8,null,null,null,null,["localStorage"]],[7,null,null,15,null,20230524]]],null,null,null,null,15,null,120,1],[139,[[44801879]],[2,[[4,null,55],[5,null,8,null,null,null,null,["localStorage"]],[4,null,8,null,null,null,null,["localStorage"]],[7,null,null,15,null,20230524]]],null,null,null,null,30,null,120,1],[14,[[44801880]],[2,[[4,null,55],[5,null,8,null,null,null,null,["localStorage"]],[4,null,8,null,null,null,null,["localStorage"]],[7,null,null,15,null,20230524]]],null,null,null,null,170,null,120,1],[139,[[44801881]],[2,[[4,null,55],[5,null,8,null,null,null,null,["localStorage"]],[4,null,8,null,null,null,null,["localStorage"]],[7,null,null,15,null,20230524]]],null,null,null,null,185,null,120,1],[10,[[44802850,[[null,506871937,null,[null,44802850]],[1120,null,null,[1]]]],[44802851,[[563731719,null,null,[1]],[null,506871937,null,[null,44802851]],[1120,null,null,[1]]]]],[4,null,55],null,null,null,null,360,null,123],[1,[[44803132,[[null,506871937,null,[null,44803132]],[1120,null,null,[1]]]],[44803133,[[554474127,null,null,[1]],[null,506871937,null,[null,44803133]],[1120,null,null,[1]]]]],[4,null,55],null,null,null,null,430,null,123],[10,[[44803138,[[null,506871937,null,[null,44803138]],[1120,null,null,[1]]]],[44803139,[[558761323,null,null,[1]],[null,506871937,null,[null,44803139]],[1120,null,null,[1]]]]],[4,null,55],null,null,null,null,450,null,123],[10,[[44803467,[[null,506871937,null,[null,44803467]],[1120,null,null,[1]]]],[44803468,[[561639568,null,null,[1]],[562711461,null,null,[1]],[null,506871937,null,[null,44803468]],[1120,null,null,[1]]]],[44803469,[[564316609,null,null,[1]],[561639568,null,null,[1]],[null,564316610,null,[null,0.3]],[562711461,null,null,[1]],[null,506871937,null,[null,44803469]],[1120,null,null,[1]]]]],[4,null,55],null,null,null,null,470,null,123]]],[11,[[1000,[[31078150,null,[4,null,6,null,null,null,null,["31078148"]]]],[4,null,8,null,null,null,null,["__gpp"]],88,null,null,null,null,null,null,null,null,9],[1000,[[31078151,null,[4,null,6,null,null,null,null,["31078149"]]]],[4,null,8,null,null,null,null,["__gpp"]],88,null,null,null,null,null,null,null,null,9]]]],null,null,[null,"1000",1,"1000"]],[null,[],null,null,null,null,null,null,"ca-pub-4830067276561639"],null,null,1,"5e4daa4b14d4f7c9e425384ce33f40fc.safeframe.googlesyndication.com",89562434,[44759875,44759926,44759837]] �Di(          �   �-� �!��_����Ū    Sb�`          Ib����    p� �b      @ �e            M1      
							�A�Eo��   A'�0�     RE��V�"
�$�ƅ��d�����S���U��A�Eo��                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       gs;
  }
};


class Abort_on_warning_instant_set
{
  THD *m_thd;
  bool m_save_abort_on_warning;
public:
  Abort_on_warning_instant_set(THD *thd, bool temporary_value)
   :m_thd(thd), m_save_abort_on_warning(thd->abort_on_warning)
  {
    thd->abort_on_warning= temporary_value;
  }
  ~Abort_on_warning_instant_set()
  {
    m_thd->abort_on_warning= m_save_abort_on_warning;
  }
};


class Check_level_instant_set
{
  THD *m_thd;
  enum_check_fields m_check_level;
public:
  Check_level_instant_set(THD *thd, enum_check_fields temporary_value)
   :m_thd(thd), m_check_level(thd->count_cuted_fields)
  {
    thd->count_cuted_fields= temporary_value;
  }
  ~Check_level_instant_set()
  {
    m_thd->count_cuted_fields= m_check_level;
  }
};


/**
  This class resembles the SQL Standard schema qualified object name:
  <schema qualified name> ::= [ <schema name> <period> ] <qualified identifier>
*/
class Database_qualified_name
{
public:
  LEX_CSTRING m_db;
  LEX_CSTRING m_name;
  Database_qualified_name(const LEX_CSTRING *db, const LEX_CSTRING *name)
   :m_db(*db), m_name(*name)
  { }
  Database_qualified_name(const LEX_CSTRING &db, const LEX_CSTRING &name)
   :m_db(db), m_name(name)
  { }
  Database_qualified_name(const char *db, size_t db_length,
                          const char *name, size_t name_length)
  {
    m_db.str= db;
    m_db.length= db_length;
    m_name.str= name;
    m_name.length= name_length;
  }

  bool eq(const Database_qualified_name *other) const
  {
    CHARSET_INFO *cs= lower_case_table_names ?
                      &my_charset_utf8mb3_general_ci :
                      &my_charset_utf8mb3_bin;
    return
      m_db.length == other->m_db.length &&
      m_name.length == other->m_name.length &&
      !cs->strnncoll(m_db.str, m_db.length,
                     other->m_db.str, other->m_db.length) &&
      !cs->strnncoll(m_name.str, m_name.length,
                     other->m_name.str, other->m_name.length);
  }
  void copy(MEM_ROOT *mem_root, const LEX_CSTRING &db,
                                const LEX_CSTRING &name);
  // Export db and name as a qualified name string: 'db.name'
  size_t make_qname(char *dst, size_t dstlen) const
  {
    return my_snprintf(dst, dstlen, "%.*s.%.*s",
                       (int) m_db.length, m_db.str,
                       (int) m_name.length, m_name.str);
  }
  // Export db and name as a qualified name string, allocate on mem_root.
  bool make_qname(MEM_ROOT *mem_root, LEX_CSTRING *dst) const
  {
    const uint dot= !!m_db.length;
    char *tmp;
    /* format: [database + dot] + name + '\0' */
    dst->length= m_db.length + dot + m_name.length;
    if (unlikely(!(dst->str= tmp= (char*) alloc_root(mem_root,
                                                     dst->length + 1))))
      return true;
    sprintf(tmp, "%.*s%.*s%.*s",
            (int) m_db.length, (m_db.length ? m_db.str : ""),
            dot, ".",
            (int) m_name.length, m_name.str);
    DBUG_SLOW_ASSERT(ok_for_lower_case_names(m_db.str));
    return false;
  }

  bool make_package_routine_name(MEM_ROOT *mem_root,
                                 const LEX_CSTRING &package,
                                 const LEX_CSTRING &routine)
  {
    char *tmp;
    size_t length= package.length + 1 + routine.length + 1;
    if (unlikely(!(tmp= (char *) alloc_root(mem_root, length))))
      return true;
    m_name.length= my_snprintf(tmp, length, "%.*s.%.*s",
                               (int) package.length, package.str,
                               (int) routine.length, routine.str);
    m_name.str= tmp;
    return false;
  }

  bool make_package_routine_name(MEM_ROOT *mem_root,
                                 const LEX_CSTRING &db,
                                 const LEX_CSTRING &package,
                                 const LEX_CSTRING &routine)
  {
    if (unlikely(make_package_routine_name(mem_root, package, routine)))
      return true;
    if (unlikely(!(m_db.str= strmake_root(mem_root, db.str, db.length))))
      return true;
    m_db.length= db.length;
    return false;
  }
};


class ErrConvDQName: public ErrConv
{
  const Database_qualified_name *m_name;
public:
  ErrConvDQName(const Database_qualified_name *name)
   :m_name(name)
  { }
  LEX_CSTRING lex_cstring() const override
  {
    size_t length= m_name->make_qname(err_buffer, sizeof(err_buffer));
    return {err_buffer, length};
  }
};

class Type_holder: public Sql_alloc,
                   public Item_args,
                   public Type_handler_hybrid_field_type,
                   public Type_all_attributes
{
  const TYPELIB *m_typelib;
  bool m_maybe_null;
public:
  Type_holder()
   :m_typelib(NULL),
    m_maybe_null(false)
  { }

  void set_type_maybe_null(bool maybe_null_arg) { m_maybe_null= maybe_null_arg; }
  bool get_maybe_null() const { return m_maybe_null; }

  decimal_digits_t decimal_precision() const
  {
    /*
      Type_holder is not used directly to create fields, so
      its virtual decimal_precision() is never called.
      We should eventually extend create_result_table() to accept
      an array of Type_holders directly, without having to allocate
      Item_type_holder's and put them into List<Item>.
    */
    DBUG_ASSERT(0);
    return 0;
  }
  void set_typelib(const TYPELIB *typelib)
  {
    m_typelib= typelib;
  }
  const TYPELIB *get_typelib() const
  {
    return m_typelib;
  }

  bool aggregate_attributes(THD *thd)
  {
    static LEX_CSTRING union_name= { STRING_WITH_LEN("UNION") };
    for (uint i= 0; i < arg_count; i++)
      m_maybe_null|= args[i]->maybe_null();
    return
       type_handler()->Item_hybrid_func_fix_attributes(thd,
                                                       union_name, this, this,
                                                       args, arg_count);
  }
};


/*
  A helper class to set THD flags to emit warnings/errors in case of
  overflow/type errors during assigning values into the SP variable fields.
  Saves original flags values in constructor.
  Restores original flags in destructor.
*/
class Sp_eval_expr_state
{
  THD *m_thd;
  enum_check_fields m_count_cuted_fields;
  bool m_abort_on_warning;
  bool m_stmt_modified_non_trans_table;
  void start()
  {
    m_thd->count_cuted_fields= CHECK_FIELD_ERROR_FOR_NULL;
    m_thd->abort_on_warning= m_thd->is_strict_mode();
    m_thd->transaction->stmt.modified_non_trans_table= false;
  }
  void stop()
  {
    m_thd->count_cuted_fields= m_count_cuted_fields;
    m_thd->abort_on_warning= m_abort_on_warning;
    m_thd->transaction->stmt.modified_non_trans_table=
      m_stmt_modified_non_trans_table;
  }
public:
  Sp_eval_expr_state(THD *thd)
   :m_thd(thd),
    m_count_cuted_fields(thd->count_cuted_fields),
    m_abort_on_warning(thd->abort_on_warning),
    m_stmt_modified_non_trans_table(thd->transaction->stmt.
                                    modified_non_trans_table)
  {
    start();
  }
  ~Sp_eval_expr_state()
  {
    stop();
  }
};


#ifndef DBUG_OFF
void dbug_serve_apcs(THD *thd, int n_calls);
#endif 

class ScopedStatementReplication
{
public:
  ScopedStatementReplication(THD *thd) :
    saved_binlog_format(thd
                        ? thd->set_current_stmt_binlog_format_stmt()
                        : BINLOG_FORMAT_MIXED),
    thd(thd)
  {}
  ~ScopedStatementReplication()
  {
    if (thd)
      thd->restore_stmt_binlog_format(saved_binlog_format);
  }

private:
  const enum_binlog_format saved_binlog_format;
  THD *const thd;
};


/** THD registry */
class THD_list: public THD_list_iterator
{
public:
  /**
    Constructor replacement.

    Unfortunately we can't use fair constructor to initialize mutex
    for two reasons: PFS and embedded. The former can probably be fixed,
    the latter can probably be dropped.
  */
  void init()
  {
    mysql_rwlock_init(key_rwlock_THD_list, &lock);
  }

  /** Destructor replacement. */
  void destroy()
  {
    mysql_rwlock_destroy(&lock);
  }

  /**
    Inserts thread to registry.

    @param thd         thread

    Thread becomes accessible via server_threads.
  */
  void insert(THD *thd)
  {
    mysql_rwlock_wrlock(&lock);
    threads.append(thd);
    mysql_rwlock_unlock(&lock);
  }

  /**
    Removes thread from registry.

    @param thd         thread

    Thread becomes not accessible via server_threads.
  */
  void erase(THD *thd)
  {
    thd->assert_linked();
    mysql_rwlock_wrlock(&lock);
    thd->unlink();
    mysql_rwlock_unlock(&lock);
  }
};

extern THD_list server_threads;

void setup_tmp_table_column_bitmaps(TABLE *table, uchar *bitmaps,
                                    uint field_count);

#endif /* MYSQL_SERVER */
#endif /* SQL_CLASS_INCLUDED */
