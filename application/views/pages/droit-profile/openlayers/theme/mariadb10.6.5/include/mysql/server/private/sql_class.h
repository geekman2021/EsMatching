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
    DBUG_ASSERT(‘   Ck  `H  €DRbÊ¦u   pp  `  ‚DRbÖFL   db  ``   ‚DRbâ†Å?   zo  `²  Rbæ¦   Yo  `ä  €DRbîÆ§   Xb  `´    DRbgE   Kl  `¾  ‚DRb"Ç*§   Xk  `n  Rb&G#Ø   Yh  `2  ŠDRbB‡¹M   tm  `  €DRbJçÑƒ   xc  `ø   RbN'å.   ie  `ª  €DRbVÇŸ   Tc  `   RbZç1Õ   Ei  `t  €DRbb®®   cf  `  ‚DRbnÇq8   Km  `(  †DRb‚r~   yb  `‚   Rb‚Góì   Vd  `  €D ¡¥`¢  Rb’ÇÃ   ch  `Ô  Rb–'a<   dk  `  RbšGÿø   Pi  `Š  †DRb®çö   xl  `¢  €DRb¶gÑ   jo  `  Rbº'Ó   zj  `Ø  Rb¾ç+V   Ma  `>   RbÂç‡H   Kd  `|  €DRbÊ'š   yi  `h  RbÊ§ğœ   Vp  `H  RbÒ§üÛ   sb  `x   RbÖ§Z¹   Di  `r  €DRbŞçÑŒ   lm  `ò  RbŞµ   $p  `Z  RbæÇf   fj  `°  €DRbîg³U   pi  `V  ŠDRb
èy¡   bd  `0  RbHX   gp  `ö  RbhC   rj  `È  Rb(    Gp  `$  €DRbHßÏ   el  `z  Rb"HåÏ   ek  `  Rb"Høj   Qm  `4  €DRb.(©Ú   Qe  `ì  ‚D á¤`n  ‚DRbF¨Tú   nq  `j  Rb"hö   Lp  `*  €DRbRˆt†   Sj  `  RbVè3"   Zk  `r  €DRb^(=J   fa  `
   €D á®`²   RbjˆMx   gk  `  €DRbrÈÖ˜   Yi  `  Rbvÿg   Fp  `4  DRbªz”   xn  `D  DRÎ(–¶   pa  `   RbÒ(å1   jj  `´  RbÖˆÔ   xo  `®  €DRbŞ¨^   tb  `z   RbŞ¨©$   Ud  `  Rbâ¨i   ck  `  Rbêí!   jc  `Ò   †DRbşèü   sc  `ì   RbÉ²Ç   Ch  `
  Rbéùb   Hg  `¬  €DRb)È^   Wn  `x  ‚DRbi&x   Xj  `  RbiÖÈ   vm  `
  ŠDRb:IPŸ   yf  `0  DRb^‰‰   Jn  `\  ‚DRbjéã|   sn  `:  Rbn©ºÀ   Gi  `x  RbrI,Û   fq  `^  †DRb†	XK   Qn  `l  ‚DRb’©œâ   wl  `   ŒDRb²)†¡   qk  `.  €DRbºé'@   Wg  `Æ  €D a¥`f  RbÂé   nh  `ê  RbÆ‰(   Ci  `p  RbÎ‰j&   Eo  `¼  ‚DRbÚ‰e    Nm  `.  ‚DRbæ‰§   In  `Z  RbêIK   ne  `²  €DRbò©»¸   Fd  `r  ‚DRbş)¿x   Fg  `¦  „DRb
¿&   pl  `’  ‚DRbjp   Nf  `L  €DRb"
N    ig  `|  Rb&ª‰7   Mm  `,  Rb*
¦æ   Ed  `p  Rb*Jt€   Nk  `\  Rb2*MŞ   Va  `N   Rb2
€—   ui  ``  Rb6*Iå   Uj  `  Rb:ª©,   Il  `º  RbB
+Î   Vf  `\  RbFŠŞ   lb  `n   RbFª¢6   Ef  `8  RbJJ¡R   bo  `„  RbRê5#   Zd  `˜  RbVJtg   Sa  `J   RbZJs   Lg  `²  €DRbbª}:   Dd  `j  €DRbj
ï   Bp  `  Rbnê¨Ú   De  `Ò  €DRbvÊÀF   Ek  `J  ŠDRb’jëK   ko  `–  Rb–jÀ   $i  `¤  †DRbªJË[   fk  `  Rb®
œ   kc  `Ô   €DRb¶
,o   Bk  `D  €DRb¾ªß,   Ic  `
  †DRbÒêv£   Za  `T   RbÖŠš±   Gd  `t  RbÖêßc   Hk  `P  RbÚJÑT   am  `à  RbÖŠü¬   Al  `¨  Rbæ
Z   Kb  `š   Rbê*êƒ   Hb  `”   RbêêîÇ   Jj  `ì  Rbæª¤   Vb  `ª   Rbò* ¾   dd  `4  „DRb+Y   Xe  `ú  ŠDRb"Ë    bm  `ğ  €DRb*+ q   Nd  `‚  ŒD !£`ğ   RbN+8¢   qd  `P  RbNK0Å   op  `  †DRbf+*h   Jg  `´  €DRbnKo    yk  `>  Rbr«Ğß   tk  `4  €D ¡¡`Î   Rbz«vª   Ko  `È  ŒDRbkDx   Sf  `T  €DRb¦+KE   aj  `¦  €DÙ`  €DRb¶kê   gc  `È   RbºKš¼   kh  `ä  ‚DRbÆ«¹n   lq  `d  €DRbÎKG   Of  `P  €DRbÖkW   Om  `0  €DRbŞ‹û   Ri  `  DRb¬z¸   Mf  `J  Rbìu[   Dn  `P  €D !§`d  RbÌn   se  `¼  „DRb"ÌÎ   ph  `î  Rb"ŒÕ©   Fi  `v  Rb*ÌˆÔ   He  `Ø  Rb.¬c   Sh  `&  †DRbB²L   Qg  `¾  ‚DRbN^[   Pp  `<  †DRbblÅ^   df  `  RbfÌ°   jh  `â  RjŒel   no  `š  RbnŒÍú   Yd  `–  Rbr¬“   Ta  `L   €DRbzÌ¶Ô   Hi  `z   ¡¦`   Rb‚lxÑ   ic  `Ğ   DRb¦Û   Ki  `€  €DRb®Œ-T   wn  `B  „DRb¾,p§   wc  `ú   Rb¾LWô   Ln  ``  RbÆÌÇ8   Jl  `¼  RbÊ¬vç   wb  `€   RbÎy   xe  `Æ  „DRbŞŒú   gf  `  ˆDRböLÌ{   Pd  `„  Rböì2ñ   yh  `   RbşÇĞ   Wl  `Ö  RböLê   Xl  `Ø  €DRb
-¸‚   Gf  `<  Rb
Í”î   sg  `  €DRb
J   Xg  `È  €DRb-–_   Vh  `,  €DRbş ¡   ip  `ú  Rb*¨   Eh  `  €DRbölôå   ao  `‚  €DRb:í.«   Yb  `Ê   DRb^íw~   sp  `  †DRbrR   Wb  `®   RbrÔĞ   td  `V  Rbz-ÇL   fb  `b   Rbrmp±   ym  `  €DRb†­<²   No  `Î  ˆDRbím+   Yk  `p  RbmCm   fo  `Š  ‚DR®àr   Gn  `V  €DRb¶-÷J   gm  `è  RbºM­±   $c  `,  Rb¾mÇı   Qk  ``  Rb¶íÌ   mp  `  RbÆÍ,²   We  `ø  ‚DRbÒbë   md  `F  „DRbâ­{¸   Em  `  RbâÍœó   qp  `
  DRb®Ø‚   eq  `X  €DRbÄ   zm  `  „DRb*NÜ1   $o  `è  Rb.ÎÒá   yo  `°  ˆDRbF.ø€   Bn  `L  €DRbN®Šé   uc  `ô   RbN®
¥   Le  `à  RbVÎ   bb  `\   €DRb^î_Z   Cf  `4  DRb‚Î,Ï   Ae  `Ì  †DRb–® ì   vb  `~   †DRbª.H»   mi  `P  €DRb²®Ái   ul  `œ  €DRbº>–   ra  `   Rbº.À©   ub  `|   RbÂÈc   Og  `º  RbÆN/²   $d  `š  †DRbÚG9   hm  `ê  ŒDRbúLà   lo  `˜  ŠDRb/ıD   bk  `  ŒDRb6Oß;   vk  `8  ˆDRbNOPĞ   dh  `Ö  RbR¯ÕA   Bh  `  RbV¯Ìç   xh  `ş  €DRbRew   aq  `R  Rbb9   Ie  `Ú  Rbbo¥4   zf  `.  Rbf¯W   Tf  `V  ˆDRb‚oåã   ee  `¢  Rb‚OFÇ   og  `„  ‚DRb’Oôw   Uh  `*  €DRbšOÈÒ   cb  `^   ‚DRb¦¯Ïp   li  `N  €DRb®ß=   ff  `  „DRb¾Ã2   cm  `â  RbÂOè   Rb  `¢   €DRbÊ\·   ah  `Ğ  „DRbÚOöU   Zi  `¢  RbŞ¯,   Kh  `  Rbâ-İ   Pe  `ê  Rbâow   af  `  RbæïĞß   Am  `  ŒDRb
šğ   dq  `r  †DRbĞCK   Cm  `  Rb"P{:   ug  `  €DRb*°@Â   Bb  `ˆ   Rb*pM   Pn  `j  „DRb>0«Û   Xc  `&  „DRbN0şh   Jo  `Æ  RbR0æ   Lf  `H  †DRbfps   Td  `Œ  DRbš#   wo  `¬  „DRbª0'   Bi  `n  ŒDRbÊ”k   Cl  `®  D á£`D  Rbò0d€   Ah  `  RböPvÁ   ll  `Š  ‚DRbñù§   ae  `œ  Rbñ´   pe  `¶  Rb
Q

   Ej  `Ü  Rbqé   Oh  `  ‚DRbñ¼Á   Ve  `ö  RbÑ…Œ   mk  `&  €DRb&‘%   Oj  `ö  „DRb6Qûú   Jd  `z  Rb:ñ–õ   zl  `¦  Rb>Ñ\ÿ   ke  `t  †DRbR‘œì   mj  `¾  ‚DRb^f]   Kk  `T  RbbÑ¦å   zi  `j  ˆDRbz‘¥   Oa  `B   €DRb‚Ñãx   xj  `Ô  Rb†Q   Md  `€  RbŠ1±    Ab  `†   €DRb†ñÿ“   ii  `H   a¦`œ  Rb–q·G   rq  `t  RbQî¸   xk  `<  Rb¢ñN   Pl  `Ä  Rb¦Ú6   Zf  `d  Rba½   Yl  `Ú  ‚DRb¶qYV   jg  `~  RbºqõB   tn  `<  ŠDRbÖ±¡S   yj  `Ö  €DRbŞ1…Í   Ye  `ü  ˆDRböq©a   da  `   RböÑdû   uf  `$  €DRbr™ü   Xh  `0  å`*  €DRb²”   dl  `ˆ  ‚DRb2Ç   Zh  `4  Rb²2ÿ   nc  `Ü   €DRb&r%   Gm  `    a­`¬   Rb*òºÍ   Kf  `F  €DRb6RĞ½   vc  `ö   €DRb>RŠ»   up  `  RbBò´   qa  `   RbF’Şß   ki  `L  ‚DRbR2`i   Uc  `"  RbV²«x   oa  `   RbZrIC   tg  `Œ  ‚DRbf2î¯   kl  `†  „DRbvòqB   Vl  `Ô  Rbz²îÓ   ya  `(   €DRb‚rlÊ   wf  `(  ˆDRbš’Ï   Xo  `â  RbRWò   Vi  `–  Rb¢2`¡   sj  `Ğ  Rb¢RÈ‹   Mk  `Z  Rbª²õ   To  `Ú  Rb®Ò.   hh  `Ş  €DRb¶’Yt   cg  `n  €DRb¾R•   Gh  `  ŒDRbŞr€É   xd  `l  †DRbò2é   Hd  `v  Rbò2è~   Dl  `¬  ˆD a¢`à   Rbó@   Qc  `  RbY·   Zo  `æ  Rb3Ân   Pg  `¼  RbÓ_›   hd  `<  €DRb³Âÿ   nl  `  Rb*³'   Ll  `À  Rb.³eä   $a  `V   Rb2ó^]   Fj  `Ş  Rb6“nu   eh  `Ø  ˆDRbN³L•   Th  `(  €DRbVÿ   zp  `   RbZsÎí   gd  `:  RbZóˆ   rd  `R  Rb^óÖÉ   Un  `t  €DRbj3c&   qm  `   €DRbr“Ÿ   Lc  `  Rbv3 u   Ng  `¸  €DRb~3¶g   Lm  `*  Rb‚ÓÈ   fi  `@  €DRbr”q   Ao  `´  €DRb’3Ô|   Ca  `0   Rb–“å   ia  `   €DRbóÍˆ   rk  `0  €DRb¦3¬Ë   hk  `  ŠDRbÂ3WG   bh  `Ò  RbÆ“ÿ}   gl  `~  „DRbÖS}   Af  `2  RbÖ³/n   vj  `Ì  RbÚdx   Ho  `À  ‚DRbê³yy   oj  `Â  RbêÓ…v   qj  `Æ  RbòÓo4   Wi  `’  Rbê³ıc   tp  `  Rbî‚>   Yp  `L  RbşSIì   So  `Ø  Rbô´   jp  `ü  DRb.t`…   Wf  `^  Rb2Ìa   Im  `$  Rb6ôÕ   kb  `l   ŒDRbV«)   od  `J  RbV”$˜   Lj  `ğ  ŠDRbv8™   rh  `ò  €DRb~´Å»   El  `°  DRb²4èN   Yn  `€  ‚DRb¾TŒ”   Bd  `h  Rb¾t×(   Hm  `"  ˆDRbÚôâß   Dp  `0  „DRbêÔ°   ik  `  †DRbş49<   po  `¤  DRb*uâœ   Mb  `   Rb.µp†   eg  `r  ±`>  €DRb:ÕèÏ   Da  `2   €DRbBuëN   vo  `ª  €DRbJUÿå   Tb  `¦   RbJÕL   dc  `Æ   ŒDRbnuµS   le  `®  Rbr¤™   ro  `¢  €DRbzĞ’   sa  `    Rbzuöà   nb  `p   Rb‚8¨   je  `¬  Rb†Õğ   gb  `d   Rb†µ¹W   zg  `˜  RbÕö¿   Pj  `ø  RbzUP_   Yg  `Ê  RbŠÕú³   Vj  `ü  Rbš5ş   Fn  `R  Rb5İã   Vo  `Ş  RbzUv   Kn  `^  Rb¦Õi“   dj  `¬  RbªµÆ×   zb  `„   ‚DRb¶Õ4   Vc  `  €DRb¾µv   im  `ì  ˆDRbÖÕaP   Dj  `ä  RbÚ•b   fm  `î  ‚DRbæUíU   ci  `B  ˆDRbşU+<   wd  `\  ŠDRbÖÅŒ   pq  `n  ‚DRb& s   Ad  `d  †DRb:–—o   mf  `  Rb:–`ß   vh  `ú  „DRbNvŸ   Mi  `‚  RbN7   sl  `–  €D á¢`è   Rb^–em   pb  `t   Rb^ÖõÎ   Fl  `²  RbZvãO   ve  `Â  €DRbnCî   Cc  `  RbZ¶   ej  `®  €DRbbö&   mm  `ø  €DRb‚6£ö   Nb  `    Rb‚ö­   hf  `  Rb†öµ`   Dg  `¤  €DRb’V¯   Ni  `†  R–ö0É   La  `<   Rb–ÖO   Fb  `   Rb6Çí   Je  `Ü  €DRbšvN{   Rn  `n  ‚DRb²Ö   hq  ``  €DRbºÖ   pc  `â   †DRbÎvo   gi  `D  ‚DRbÚv?I   wh  `ü  ŒDRbúö²ç   Ga  `6   Rbş–3   Aj  `â   DRb&7%(   qf  `  Rb*·Ê›   Re  `î  €DRb&÷Ó   Io  `Â  „D !¡`¾   €DRbJÒ[   Ul  `Ò  €DRbR·4³   Ij  `ê  ‚DRb^7Ëİ   Sp  `B  †DRbr÷ˆ-   hb  `f   Rbr·Æ›   Rg  `À  Rbz×ÃW   Gk  `N  ‚DRb†÷R   qh  `ğ  RbŠ7@¹   jl  `‚  €DRb’½/   mg  `€  Rb’—”V   $g  `Î  €DRb’ı1   ni  `R  „DRb®÷LÆ   Wk  `l  ‚DRbº×ƒ'   Tj  `
  Rb¾T¹   bf  `  €DRbÆwYş   Sd  `Š  RbÊ·î›   Rl  `Ì  €DRbÆ÷XŞ   Mo  `Ì  „DRbâw<~   Sm  `8  RbæW¶   ti  `^  RbêW´   Fe  `Ô  RbîWs[   ac  `º   RbòÓÜ   Jm  `&  Rbê×~   om  `ü  ‚DRbXZ   Zc  `*  Rb8{@   me  `°  ‚DRbêf¹   bq  `T  Rb˜|ù   bp  `ì  Rb˜Äí   pk  `,  (DRbV8#5   la  `   RbZØ©›   ab  `Z   RbZøıe   Bo  `¶  „DRbn8Âs   Ik  `R  „DRb~XP´   sf  `   ‚DRbŠx+
   Lb  `œ   RbŠÕó   Ai  `l  Rb’˜ ã   kd  `@  Rb’X•÷   Wd  `’  ‚DRb¢X4   sm  `  ‚DRb®xE$   Ac  `ş   ŒD a¬`    RbÒ¸ø=   Ea  `4   RbÖáî   Mn  `b  RbÎ˜HÛ   rb  `v   RbÚÕ   uk  `6  Rbâ˜×^   ua  `$   RbÎ8³   Dc  `  €DRbî˜A×   $h  `6  Rbò¥ö   Be  `Î  RböøG   $e  `   Rbú˜°¸   Ge  `Ö  „DRb
¹    Xd  `”  ˆDRbÎ˜ÎŞ   uj  `Ê  Rb&´œ   xg  `”  €DRb.ù9“   kp  `ş  ‚DRb:Ù	H   Nc  `  Rb>¹ú   jk  `   €DRbFy   Ji  `~  RbJ™H¡   Kc  `  ‚DRbV™„ş   hl  `„  ‚DRbbÙB   Pc  `  €DRbjù*¿   Sg  `Â  €DRbrÙÃ,   Rj  `ş  DRb¹w…   de  `   ‚DRbªùbÔ   If  `B  ‚DRb¶ùÜª   Zb  `¶   Rb¶™¢   Pf  `Z  Rb¾ùˆ%   dg  `p  Rb¾¹²   Po  `Ò  €DRbÊ™dì   oc  `Ş   ‚DRbÖÙJÈ   Nl  `Æ  RbÚÙÇ   ea  `   RbŞÙ«   pj  `Ä  RbŞ¹w|   oo  `œ  ŠDRbşY[   Zj  `  RbÚºç   yl  `¤  €DRb
šÜ   ri  `Z  Rb
z¢q   Yj  `  Rbšƒ   fc  `Ä   Rb: š   zh  `  RbZ@   Cp  `"  €DRb
ÚÚ˜   em  `æ  RbşÙkI   On  `h  Rb*Ú   cj  `ª   ¡¤`N  Rb2zHã   np  `  ‚DRb>ºÌé   ql  `”  ˆDRbVú/   ha  `   RbZº¿#   Qd  `†  €DRbbzQM   xm  `  ‚DRbn:Ûñ   Kp  `,  ˆDRb†š.Ü   rp  `  RbŠ:b§   ta  `"   Rb:QJ   ba  `    á¬`.   Rb’úP½   Ra  `H   Rb–Ú2´   Ql  `È  ‚DRb¦zL‹   Li  `„  RbªHŠ   ze  `Ê  ‚DRb¶zÏj   be  `  Rb¶Òh   Nn  `f  „D á¦`´  RbÎÚ-   Rc  `  ‚DRbÚZêÊ   ji  `J  RbÚú(-   ap  `ê  ŒDRşúïª   bg  `l  †DRb[)é   Tp  `D  ˆDRb*{£   Dh  `  „DRb:›mx   cp  `î  €DRbB[\¨   yp  `  €DRbJûD   Qj  `ú  RbN;{+   wk  `:  ˆDRbf[{5   so  `   €DRbn›Œ·   Bm  `  Rbr›K   Yc  `(  †DRb†[”»   wj  `Î  Rb†Ğ   Sk  `d  €DRb’[ºª   za  `*   €DRbš;Ğ‰   Gj  `æ  Rbš{A,   Hl  `¸  Rb¢{åE   sd  `T  €DRbª{K,   Hh  `  Rb®ÛK¤   fp  `ô  €D !¥`@  ŠDRbÒ[^“   Do  `º  „DRbâ8P   Sb  `¤   €DRbêYé   Ne  `æ  Rbîûº/   ho  `Œ  €DRbö{.ä   ng  `†  Rbö;1   Cj  `Ú  ‚DRb<1   Ba  `,   €DRbÖ¦   lp  `   €DRb¼¡$   Qo  `Ô  ‚DRb"<4Ä   nf  `,  „DRb2Üı   rm  `  €DRb:|â   mo  `’  €DRbB<…0   ud  `X  RbBÜË–   di  `:  RbJ<”h   Wp  `P  RbN|TÆ   sk  `2  ˆDRbfœ`   Uk  `h  Rbj¼sy   ih  `à  RbnÜy   $b  `¸   Rbf< °   um  `  ŠDRb<rˆ   Bg  `  Rbü‘’   vn  `@  ˆDRbªœ'8   Bl  `ª  €DRb²\PÇ   lc  `Ö   Rb¶¼ÿ2   Ka  `:   „DRbÆ¼™J   Gg  `ª  RbÆ|Xâ   mh  `è  €DRbÆÜñE   Ih  `  €DRbÚ<Ú   Tg  `Ä  ŒDRbúÜ¡   ef  `
  †DRb½?u   kf  `  €DRb   Xf  ``  Rb	>   ag  `j  Rbı!f   Ak  `B  €DRb&]“î   Rp  `@  Rb*İ]   Zp  `N  €D á¡`Ú   Rb2 î   oq  `l  €DRb>İ$   oh  `ì  ŒDRb^ı£{   zc  `ü   DRbİÃ™   cc  `À   Rb’=Å7   lj  `¼  Rb–]¨–   An  `J  Rbš}2‘   Jb  `˜   Rbš½	4   Ph  `   €DRbš]F   Jk  `V  ‚DRb²}ôÈ   te  `¾  Rb¿   zn  `H  ˆDRbÎ½Ë‰   cl  `x  ‚DRbÚ]xr   Xa  `P   RbŞi~   ai  `8  €DRbæ]“   Kg  `°  Rbêİ>±   Sn  `p  €DRbÚı´‰   $n  `|  €DRbú½Ù   hj  `²  Rbş}…^   oe  `´  €DRbú}ml   bl  `v  „DRb~    Ep  `2  ŠDRb2,«   Oo  `Ğ  Rb6¾)(   Vn  `v  Rb:>ja   Si  `  ˆDRbRJê   io  `”  ˆDRbjø2   Fo  `Ä  ŒDRbŠŞ==   Lo  `Ê  Rbş˜Ô   Pa  `D   €DRb–>Å   Me  `â  ‚DRb¢şh3   gj  `¸  ŒDRbÂx™   tl  `š  RbÆ^Ü   Hj  `è  €DRbÂ‡   xp  `  €DRbÖ^(Ñ   pg  `‚  RbÚ^àÜ   bc  `¼   ‚DRbæ‘   tc  `î   €DRbîşµ   fg  `v  €DRböşR«   ec  `Â   Rbö¾Â©   Wo  `à  €DRbÿ4ú   ye  `È  RbŸC³   Fh  `  Rb
LÇ   sq  `x  Rb_(Æ   km  `ö  €DRb_½   Cg  `   €DRbŸ~ó   mq  `\  Rb"¿	}   jb  `j   Rb&¿Q   pm  `ş  Rb*Ÿ*–   Qa  `F    á­`°   ‚DRb:Ÿ¯_   xf  `*  ŒDRbZ?%û   Np  `8  †DRbnñß   Ue  `ô  €DRbvÿÄ5   fe  `¤  Rbv_ìÑ   pf  `  ŠDRb–_>   Hf  `>  Rb–_İ7   sh  `ô  €DRb¢¤   wm  `  Rb¢ŸÕ;   Go  `¾  €DRb®ßk|   qc  `ä   Rb®ÿz   yn  `F  Rb¶Ÿl–   Qf  `N  €DRb¾Ÿo±   Ec  `  Rb¾Oõ   Df  `6  DRbæ¿*°   Ub  `¨   Rbæ_bİ   bj  `¨  RbîŸà–   Qh  `$  „DRbşx´   mc  `Ø   õ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ äÿ ¤ÿ ¤ÿ äÿ ¤ÿ ¤ÿ ¤ÿ Àÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ äÿ ¤ÿ äÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ äÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ Àÿ äÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ Àÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ Àÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ äÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ Àÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ Àÿ ¤ÿ ¤ÿ Àÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ €ÿ ¤ÿ ‚ÿ ¤ÿ ¤ÿ Àÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ‚ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ €ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ äÿ ¤ÿ ¤ÿ ‚ÿ ¤ÿ ¤ÿ €ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ €ÿ ¤ÿ ¤ÿ €ÿ Àÿ Àÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ¤ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ €ÿ ‚ÿ €ÿ ¤ÿ €ÿ ‚ÿ ‚ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ €ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ¤ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ €ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ €ÿ ‚ÿ ‚ÿ ‚ÿ ‚ÿ €ÿ Àÿ Àÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ €ÿ äÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ €ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ €ÿ ¤ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ €ÿ ‚ÿ ¤ÿ ¤ÿ ‚ÿ ‚ÿ ¤ÿ ¤ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ Àÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ €ÿ ¤ÿ ‚ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ €ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ €ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ €ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ €ÿ ‚ÿ ‚ÿ ‚ÿ ¤ÿ ¤ÿ ‚ÿ ¤ÿ €ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ €ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ €ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ‚ÿ ‚ÿ ¤ÿ ¤ÿ ‚ÿ ‚ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ äÿ äÿ äÿ Àÿ ‚ÿ ‚ÿ Àÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ Àÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ ¤ÿ I`şÿÿÿDa   Ú ,TX`n   L`   áøùyéDp(          -  ÃÁ^øöÅÄ-mùšù//ú	ÅšªùPÄŠ úª (SbÄã‚        
`şÿÿÿDa8  ğ  ˜d       P     H­
 lQhz¶ğn]   https://pagead2.googlesyndication.com/pagead/js/adsbygoogle.js?client=ca-pub-4830067276561639   a        Db       j  D`    Y=`¨   ¥,TÙ
Ia  N  Á¹e     @    ^1      ,TIaY  ¢  ’“e     @    _1      ,T@¨`:   L`   ÍeéDj          VÅ Äúkù šU—
Äúkùª   (SbÄã‚        I
`şÿÿÿDaZ  Ô  ’˜b       1  “e    @    `1      ,TÙ
Iaõ  @  •¹e     @    a1      ,T“Ia€  ®  ¡•–e     @    b1      ,Tİ
Ia¹  ¿   èf       © @Ÿ¾ @ 
    ¥	•–e     @    c1      €D,T“IaÊ  R  à•–e     @    d1      ,TIa_  ç  ‘d
       ¢	æ	 @ €  •–e     @    e1      D,TÙ
Iaò  Ç  M¹e     @    f1      ,T$¨`   ]éDc           ª (SbÄã‚        ©`şÿÿÿDa¤  ¾  —e    @    g1      ,T•Ia  *  —e     @    h1      D,T8‘`.   L`   ,RiNµºÛ   ^[\s\xa0]*([\s\S]*?)[\s\xa0]*$  9’Dh          y    Ä-ùÅ^úùÅ/úª (SbÄã‚        u`şÿÿÿDal  Ø  ˜c       ³     ¹e   @    i1      ,TÙ
Iay  6  ™
’e     @    j1      ,T“IaA  ]  á	’e     @    k1      ,T8¨`0   L`   RdU’	   navigator   RdJr–y	   userAgent   IéDh          Ä-ù  Å˜-úÅ˜ú‹ª(SbÄã‚         ]`şÿÿÿDa\  Ä  ˜b         ¹e    @    l1      ,TÙ
Ia+	  s	  %‘“e     @    m1      D,T<–`2   L`   è—Di          ÿÅÃaø Ã-ø Ä^ùøkúUª   (SbÄã‚         á¬`şÿÿÿDaú  6  ‘’c       @    “e  
 @    n1      ,T@–`<   L`   Rcö¿í¿   brands  y—Dj          ˜xšÅÄ-ù  Ä-ùmú‹ª  (SbÄã‚         á`şÿÿÿDaN     ‘’b       “e     @    o1      ,TD¨`D   L`   RcÂÔ[f   Trident RbFú¸7   MSIEéDk          Åaú ˜‹Å Äbúù—ÅÄbúùª  (SbÄã‚         ]`şÿÿÿDa¶  
  ˜b       @ ¹e     @    p1      ,TÙ
Ia
  >
  Ñ	“•e      @    q1      ,T˜`ì   L`   RcZàf   Safari  RcF*w   Coast   Rcò+‚   Opera   Rb†ÜgÉ   Edge‘D„          Å Äbúù T™iÅaú—aÅaú˜‹ÅÄbúù—JÅaú˜‹ÅÄbúù
—3Åaú˜‹ÅÄbúù—Åaú—Åaú˜ÅÄbúùª  (SbÄã‚         Rb>`à   Fa  `şÿÿÿDa’  l  “”e       @ @ @   •e     @    r1      ,Tp`   L`
   Rc‚+™   ChromiumRc>Íâ   Chrome  Rc6®¼G   CriOS   éRb*§<ª   Silk‘Dv          Åaú ˜Å Äbúù‹<ÅÄbúù—ÅÄbúù˜Åaú˜‹ÅÄbúù
T—ÅÄbúùª (SbÄã‚         !	`şÿÿÿDa‚  2  “”d       @ @    •e     @    s1      ,T–Ia$  s  ½“•e     @    t1      D,T–Ia€    m•e      @ "   u1      ,Tx¨`°   L`   yèéDx(           š& šÄ-  kù™ÿ‹-ÄÁ_ùöªÅ- mú	š"rú
šú/ÄlùšúªúPÅŠ% ÿª(SbÄã‚        	`şÿÿÿDa  H  ”˜d       Ä €˜ Dx¹e    @ #   v1      ,Tt•`¦   L`   –áøIáà—DwH          -  Å š-ÂÀ^÷õ‹ÄÃúmøš-ùrøš-	ÂÀø/ù¿öøóò\÷öøPÃŠ. ª (SbÄã‚        =`şÿÿÿDa^  <  ”‘d       @ 1€ x’e   @ $   w1      ,TÙ
Ia)  Í  5’e     @ %   x1      ,T„¨`È   L`
   yíáøIáàéD{`          -  Å!ÁböúÄ š-Á¿^öô‹ÃÂúm÷
š4ør÷š$-¿½÷/ø¼ó÷ğï\ôó¿4ù÷ô÷PÂŠ5 ùª(SbÄã‚        á`şÿÿÿDa´  È   –˜e       @   Àx¹e   @ &   y1      ,TÙ
Iao  ó  	•e     @ '   z1      ,T¨¨`  L`
   yáøIáà‘òéD”@          -  Å š-ÃÁ^øö‹ÄúQÅÃúoøš6ùrúš&-
ÃÁú/ùÀ÷úôó\ø÷˜	ú‹úQÅŠ9 ÿÃnøš‹ š-Ã^ø‹/ª (SbÄã‚        Y`şÿÿÿDaü!  v#   ˜f       @ 0&P  /±     ¹e   @ (   {1      ,T0‘`   ]“Df          Å Äcù oúª  (SbÄã‚        ¡
`şÿÿÿDa#  À#  •b       D  –e    @ )   |1      ,TÙ
Iaë  I  ]–e     @ *   }1      ,T0¨`    L`    á˜éDf          (Ä-ù  Å^úùª(SbÄã‚        -`şÿÿÿDaª$  Ú$  ’˜b         ¹e    @ +   ~1      ,T$“`   ]”Dc           ª  (SbÄã‚         I`şÿÿÿQb      Ta. aú$  %  ’—e     @ ,   /3      ,Tİ
IaÃ  ö  èd
       İ%ë% @    Í—e     @ -   1      D,T‘Ia  :  ’d       ‘&®' @ 5P ”—e     @ /   €1      D,TÙ
IaG  7  ”¹e      @ 1   1      ,T<¨`2   L`   í}ÑæáàéDi          !  Ä-ùÄ-ùÄ-ùÅ^úùª   (SbÄã‚        ©`şÿÿÿDa†*  Ø*  ˜c
       P   e   @ 2   ‚1      ,Td‘`†   L`    !­ÉAÛ”Ds           .« /J  ÅÄúLlùUš*!Ã-øÄ^ùø	˜-Äbù.ÃúJÂ4ø÷ª (SbÄã‚        ©	`şÿÿÿDa+   +  –—d          :  ¹e    @ 3   ƒ1      ,TH¨`N   L`   AÛ !­”Dl          !  Ä-ùÅ^úù˜-Åbú.«Ä4ùª (SbÄã‚        é`şÿÿÿDa¶+  ,  –—c
       @ 8  ’e   @ 4   „1      ,T4“`$   L`    !­éDg          .« Ä/ J4ùª  (SbÄã‚        I`şÿÿÿDa4,  ^,  ˜b       @ ’e   @ 5   …1      ,T4“`$   L`    !­•Dg          .« Ä/ J 4ùª  (SbÄã‚        å`şÿÿÿDat,   ,  —b       @ ’e   @ 6   †1      ,TÙ
Ia[  q  •—¹e     @ 7   ‡1      ,T’Ia|  “  —”e     @ 8   ˆ1      ,TD¨`B   ]éDk          N Lÿ  Ålúš
    ‹ª   (SbÄã‚        A	`şÿÿÿDa<-  -  ˜b       F  ”e   @ 9   ‰1      ,TL–`R   L`   í!â]É—Dm          Uš$ š!  Ä-ùÅ^úùTš-Å!lú
ª   (SbÄã‚        Q`şÿÿÿDa¼-  d.  ‘’c       @ ¹e   @ :   Š1      ,T4¨`"   L`   éDg          L ˜
! Åaú¨ª   (SbÄã‚        y`şÿÿÿDaê.  /  ˜b       Ğ  —e   @ ;   ‹1      ,TÙ
Ia›    ”¹e     @ <   Œ1      ,T–Ia*  T  Ù”e      @ =   1      ,T–Ia_  ‹  Ù	”e      @ >   1      ,TX¨`p   L`   (Rh›t˜   Expected boolean but got    Rb‚
(ò   :   éDp          ™2 ™'!  ÅÄÃbøw8ùÄ8ùÄw8ùÄbúù¨xª(SbÄã‚        M`şÿÿÿDa.1  
2  ˜c       @    ¹e    @ ?   1      ,TL“`T   L`   ½!ì•Dm            š!  Ä-ùÅ^úù—xš š
!Åbúª  (SbÄã‚        `şÿÿÿDat2  &3  —c
       @   ‘e    @ @   1      ,T@¨`>   L`   ’!ì•Dj          Uš!  Ä-ùÅ^úù—<Åaúª (SbÄã‚        õ`şÿÿÿDa<3  3  —c       @    ‘e   @ A   ‘1      ,TÙ
IaÚ  ç  M‘e     @ B   ’1      ,TH”`L   L`   ’!ìéDl            ™	=Åaú ¨! Ä-ùÅ^úù—<Åaúª  (SbÄã‚        `şÿÿÿDaä3  t4  ˜c
       P   ¹e   @ C   “1      ,T4¨`"   ]‘Dg          š‹	CÅbú ª   (SbÄã‚        ¥`şÿÿÿDaŠ4  À4  “b          ”e   @ D   ”1      ,TH•`J   ]‘Dl           šª š—ªt   šªª   (SbÄã‚        í`şÿÿÿDaÚ4  5  “b          ”e   @ E   •1      ,TH•`J   ]‘Dl           šª š—ªt   šªª   (SbÄã‚        `şÿÿÿDa´5  x6  “b          ”e   @ F   –1      ,T8•`,   L`   éDh          ™ ™
!  Åaú¨ª  (SbÄã‚        É`şÿÿÿDa6  7  ‘˜b         ¹e   @ G   —1      ,T4¨`"   ]“Dg           ™ š‹ª   (SbÄã‚        H`şÿÿÿDa7  |7  ‘–e   @ H   ˜1      ,Tø—`¬   L`   í!â¥		 !® !­éDä   (       Å™( š"!  Ã-øÄ^ùøÅ—-Ä6lùšªú—^˜TL	˜BJ«/
˜	‹.iú -.«Ã/J"4øJÃ4ø‹iú ‹ª.«/J ÅÄlùšL >úúùÅL >úúùÅlú!Uš.Ãú4ø"úi$ª  (SbÄã‚        Í`şÿÿÿDa–7  †9  (˜h&       @ Dˆ € Ğ "!Âè    ¹e   @ I   ™1      ,T8¨`.   L`   `•Dh           K« %Ki %Kª (SbÄã‚        ¥`şÿÿÿDaÎ9   :  —b          ‘e   @ J   š1      ,T(’`   ]•Dd          NÅbú ª(SbÄã‚        ¥`şÿÿÿDa8:  ^:  —b          ‘e   @ K   ›1      ,T’`Ò  @L`   Á5Í!ìÅí!â)	Iy1ñİ‚YRb~_   btoaéDøP          VÂ l÷ ™l÷ ™"l÷ ™%‹Í!Âb÷˜‹
!Âb÷ª˜‹ª˜¨!	Á-öÂ^÷ö—˜™“!qš‹ÅÄ-	 E ( Ãømùš;!À-õ
À-õÁ¿-¾ù D ( ùñÄ_óñù¾_öõôó 8úú÷ÅŠ< "!À-õ
À-õ$Á¿ùú÷˜-¾^óù&‹¾_öõôó(8÷#Å!*Âb÷ú,ªª   (SbÄã‚        µ`şÿÿÿDat:  .=  ,˜i.       Ñ € 
€ *°€ € p!@   ¹e   @ L   œ1      ,TÙ
Ia£  q  ¡’e     @ M   1      ,T¨`Ø  (L`   í!ây !­É}ÁİáàéDø   8       ™ç!  À-õÁ^öõ˜Z˜Á-köš.«/J 
L	˜‹5˜.«/J L˜‹QÁU½õôóñğ`öõ‹|7Ábö˜l~ÄŸ_vö¢ö£õ½¤òóšQ¥öòõCÅÂ!»-ğ»-ğ»-ğ¼_ñğú˜&Pº÷/ ¹íìëêé`ïî"º4ù÷$ï¦ò½ŠP &ù‹	c'ªª(SbÄã‚        ¹	`şÿÿÿDaø>  A  (˜h)       @ D B€ j P  @ \ ¹e   @ N   1      ,T¨`Ş   L`    !­y’D~X   8       —˜.« /J  ‹Å˜
úL x‹-ÃbøÄ-mùš,PÁù/	Àôóòñğ`öõ4ùùPÄŠ/ ˜cúª (SbÄã‚        U`şÿÿÿDaA  (B  ”•e       H DÀx	    –e   @ O   Ÿ1      ,TÙ
Ia!  ‘!  ñ”–e     @ P    1      ,T@—`>   L`   ¥		éDj          -  Å6lúš-Å]ú‹	NÅbúª (SbÄã‚        Y`şÿÿÿDa8C  „C  ˜c	       Ä€   ¹e   @ Q   ¡1      ,TÙ
aı!  "  I’e      KĞ R   03      ,T“Ia("  3#  e’e     @ S   ¢1      ,T“Ia>#  ›#  •’e     @ T   £1      D,TX¨`j   L`    !® !­]éDp@          -  Å.«/úÄL˜"LÃ-ÂVÁ¾úõùô`öõÁcø÷ö	‹ª   (SbÄã‚        ­`şÿÿÿDaLG  ÎG  ˜c         €  ¹e   @ V   ¤1      ,T@—`:   L`    !® !­‘Dj           -  YÅ.«/Ãù÷`úùª   (SbÄã‚         a­`şÿÿÿDaäG  $H  “”b        @ •e  
 @ W   ¥1      ,T —`   L`   y‘DŒ   (       ÿÄlù šª5Ãbøpš L  ˜- E/Ä/ù
ª‹K- Å˜" L  ˜úE/Ä/ù™ª L  xtE8úmš/ªª(SbÄã‚        `şÿÿÿDa:H  ¤I  “ ”f       ‘€! € "€ B    •e   @ X   ¦1      ,TL¨`V   L`    !® !­‘Dm8           -  Å.«/úÄ:Ãbøù[Ãú÷ùöõô`ø÷ª (SbÄã‚         á­`şÿÿÿDa¸I  "J  “”c        @    •e  
 @ Y   §1      ,TÔ`b  L`   – !­‘DÀ    0       5Äbù Åúp™˜e L  ˜- E/Å‹.šª L  xtE
8ú	Ã~Â4ø÷ú J  4úlUš.«Ã4ø‹; L  xtE8Ã4ø L  ˜- E/ršWª   (SbÄã‚ 
        á®`şÿÿÿDa:J  èK  “$”g!       D !Ğè Ñ!Ğ   •e  
 @ Z   ¨1      ,TÙ
Iaÿ%  $&  ñ“•e     @ [   ©1      ,T¨`  L`   í!â !­éD”(           L ÅYÄø÷ö`ùø! Ã-øÄ^ùø—9.«/
J 	L—	1Äbùú˜L—.Ã/J"Â4ø÷‹$L ˜LTšú.Ã/úLßÂ4úø÷ª (SbÄã‚        1
`şÿÿÿDa^L  fM   ˜f        
€ ˆ  ˆ Ğ :  ¹e   @ \   ª1      ,TD¨`H   ]”Dk          XÅcú š‹ ™  šx‹ª(SbÄã‚        U`şÿÿÿDa€M  *N  –—b          e   @ ]   «1      ,T=‘`8   L`    !® !­ÉAÛyñÚéD)`           -  .«/ÅLÄ]¾òúñğ`óòÃ./øJ ÂL
—¢!½-ò¾^óòø˜ Â-¾bóøÃ[¾òúñğøï`óòÁÀ-ømöš*ö/ø½bò¿Ušõu½PÀô4øòôöPÁŠ-  ömõ!š
õ2ø"õ÷J$Âù˜÷J"%Â.½÷4øò&÷L(˜!½-ò)¾^óòø+ùTšD÷L-—!½-ò¾^óòø.˜--¾bóø0Ã.½/2J44ò5[¾òúñğï`óò7øª(SbÄã‚        …	`şÿÿÿDa@N  ºP  4˜k9         	 "   €€"  Ş¸ : € H   €   ¹e   @ ^   ¬1      ,T¸¨`.  L`
    !® !­“AÛ”•D¤@           -  Å.«/úÄ:Âb÷ùš[Âúöùõô`÷ö‹j./
J 	L—I!Á-öÂ^÷ö˜-Âb÷Ã-møšCÀø/¿bõô4øøPÃŠ .Á4ö[Âúöùõôó`÷ö!ª (SbÄã‚        %`şÿÿÿDaÔP  4R  —$g#        @ @ˆ @ €  × ‘e   @ _   ­1      ,Tp’`œ   L`    !® !­y	½éDv@   (       -  Å.«/úÄ:Ãbøù[Ã¿lôú÷ùöõš¿!¾bó	lô‹l˜‹¿`ø÷ª  (SbÄã‚         !¡`şÿÿÿDaHR   S  —d        @ Ñ "‘   ‘e  
 @ `   ®1      ,T€’`¾   L`    !® !­–Dz8   (       -  Å.«/úÄ:ÃbøùeÃú÷ùöõ`ø÷˜!lUšUš[Ãú÷ùöõ`ø÷	[Ãú÷ùöõô`ø÷ª (SbÄã‚        ©`şÿÿÿDaS  ÔS  ˜d        @ D    ¹e   @ a   ¯1      ,TL¨`V   L`    !® !­–Dm            -  eÅ.«/Ãù÷`úùÅlúš‹ÿª (SbÄã‚        1`şÿÿÿDaêS  @T  ‘c        @    ’e   @ b   °1      ,T@“`:   L`    !® !­–Dj           -  eÅ.«/Ãù÷`úùª   (SbÄã‚        e
`şÿÿÿDaVT  –T  ‘b        @ ’e   @ c   ±1      ,Tt“`¨   L`   yéDw8           ÅÄ-  mùšFù/ÃYÂöõøô`÷öUš"Âúl÷Uš[Âöõúô`÷öøúúùP
ÄŠI úª(SbÄã‚        i`şÿÿÿDa°T  ‚U  ˜c       D  /¹e   @ d   ²1      ,TÙ
IaÌ*  2+  á‘“e     @ e   ³1      ,T€¨`À   L`    !® !­—Dz@   (       -  .«/ÅYÃ÷úöõô`ø÷ÄIÃÀù÷öúô`ø÷ùlUšUš[Ã÷úöõôó`ø÷	ª(SbÄã‚        …`şÿÿÿDazV  :W  ‘’c        @ D “e   @ f   ´1      ,T|–`´   L`    !® !­éDy@           gÃ¿÷öõ`ø÷ šª- .«/ÅL—+WÃbøÄlù	Ušù[Ã¾÷úöõùô`ø÷
ª  (SbÄã‚         a¡`şÿÿÿDaNW  DX  ‘’c       € €H “e  
 @ g   µ1      ,T¡–`ş  $L`    !® !­ÉAÛñÚy`F  D€           -  Å.«/úÄLx˜‹ÃùLxÂ]ºúîùíì`ïîÁ9lö™Ï./öJ 
L	˜ÂºølïUš°÷˜<ºølïš0./öJ Ã-ºbïöÁ.¹ø4öî[ºúîùíìöë`ïî‹r!¹-îº^ïîöÂºølïš÷—!¹-îº^ïîö!‹E./ö$J #ÃLİ&÷˜ -ºbïö'ÁÃ[ºúîùíìöë`ïî)lø+Uš.¹4öî,öŒöõùL.xÁ./õ1J 0L/x¿õ÷öTšô˜
-ºbïõ3Àôùï˜‹>ï5¾ô—¿½¼-õ6mò8šHIºò/õ9¹·íóë`ïî;»Uš$ô—-ğ >º./ï@L=¿ñuB¹PB¼ğ4õîCğòPE½ŠK FòmñGš
ñ2õHñõ./KJ JÀJM¾ô˜	óL÷N‹óJO¿ôkõPUš0ºcïôQõl÷SUš[ºúîùíìõë`ïîTö˜ºølïVU—
ºølïW˜!¹-îº^ïîõXõ—P./\J [LZ—C-^m`š*/aÅWºbïúcÄùlúeUš
ù4fùPhŠ- i.¹/jJl4îmª (SbÄã‚         ¡¡`şÿÿÿDa\X  ]  X˜to         !D Fˆ 8@ @ ±  @ : ! D € Ğx#@ F"ÄH "P "À‹ Ğ  ¹e  
 @ h   ¶1      ,T<–`4   ]Di            šZÅùø÷`úù ª  (SbÄã‚        5`şÿÿÿDa´]  ^  “”b          •e    @ i   ·1      ,TÙ
Ia/  B/  Á“•e     @ j   ¸1      ,TÄ¨`B  L`    !® !­’éD°P           -  Å.«/úÄ:Àbõù™k-xÃÂ-m÷š'÷/	Áø˜-ö À./õJ LTÃ÷PÂŠ* ./J ÂJÁø˜	öJ‹öL÷JÃ÷køUš0Àcõøš[Àúôùóòñ`õôª   (SbÄã‚        `şÿÿÿDa^  F`  $˜g        @ D °  " !F    ¹e    @ k   ¹1      ,T4‘`"   ]’Dg          EÅXÄcù Äbúùª   (SbÄã‚        i`şÿÿÿDa\`  Œ`  ”•b         –e   @ l   º1      ,TL‘`X   ]’Dm          XÅcú š‹@Åbú˜  š‹‹ª(SbÄã‚        ¡`şÿÿÿDa¢`  4a  ”•b         –e    @ m   »1      ,T4‘`"   ]’Dg          HÅXÄcù Äbúùª   (SbÄã‚         á¡`şÿÿÿDaHa  xa  ”•b         –e  
 @ n   ¼1      ,TÙ
IaÇ0  Û0  -”–e     @ o   ½1      ,T(¨`   ]éDd           Ÿ‹ª(SbÄã‚        A
`şÿÿÿDaÌa  ìa  ¹e   @ p   ¾1      ,T@“`>   ]”Dj            úù‹Ã^Âc÷úù Ÿ‹øª (SbÄã€         a¢`şÿÿÿDa b  :b  –˜b          e  
 @ q   ¿1      ,T4“`"   ]”Dg          qÅmÄcù Äbúùª   (SbÄã‚        	`şÿÿÿDaTb  †b  –‘b         e   @ r   À1      ,T “`ş    L`    !® !­‰ÉQ½éDŒ@          -  .«/ÅYÂöúõô`÷öÄšù‹1ù  ™ Âùl÷™Âùl÷™Âùl÷š!	Âb÷ù‹ÃUšùløUš[Âöúõôøó`÷öøŸ‹ª (SbÄã‚        `şÿÿÿDaœb  öc  ˜d        @ 1F   ¹e    @ s   Á1      ,T0¨`   L`   I—Df          oÅcú Ÿ‹ ª (SbÄã‚         ¡¢`şÿÿÿDa
d  :d  ‘’b          “e  
 @ t   Â1      ,T0”`   ]—Df          XÅcú Ÿ‹ª  (SbÄã‚         á¢`şÿÿÿDaNd  |d  ‘’b          “e  
 @ u   Ã1      ,TD”`B   ]—Dk8   (       hÅcÂöõô`÷ö Âùø`úùª   (SbÄã‚        ¬`şÿÿÿDa’d  Öd  ‘’b         “e    @ v   Ä1      ,TÙ
Iax2  "3  Å‘“e     @ w   Å1      ,T–Ia-3  Ë3  )‘“e     @ x   Æ1      ,T–IaÕ3  ó3   !£‘“e  
   @ y   Ç1      ,TI”`J  $L`   `í!â !­y !®éD5P          šK« ½K« ò%Kš`Å| ‹ñ!¼-ñ½^òñ—
!½aò	¨.«/J ÅL@˜‹ÒùúJ@Å-Ã˜·øEÂ÷/ùÃ7½bòø˜¢ú J  Å L  xtEÁö9÷÷òÂ  ½÷oòš[ ÿ½ö8òÂ-ùÀ÷ôôõmôš+ô/ù¾Ušøló!Ušö9ô"¼ó4øñ#óôP%¿Š, &÷D'2ù(ø4ù÷* ÿÂúLÿàÿ-   ½÷ Lÿ/ M.>ò,Å.«¼ú4ñ022ª   (SbÄá         a£`şÿÿÿ a£ah   k  0˜j4       Ó€À  @X  B! ^¸ @!,  ¹e   ÃÕz   13      ,Th¨`   L`   ) !®•Dt@          8« ˜{Ä- ÂÁø`ùøÅ‹,QÄ-ÃSÂÁÀ¿¾`ùøÅ{ÄÁøú÷`ùøÅúª(SbÄáÚ         	`şÿÿÿDa,k  øk  –—c
       P   e    ÖÑ {   23      ,T9‘`0  DL`   ] ¡­ !® !­yÉa×¡å`  }Áİáàí!âÑæéD%Ğ           -  ç-çÅ5ç˜-‹æ.«/ææbçæÄÃú–—n-çbç
-˜\7ç-E/Âbç÷˜GÃ-úmøš<ø/úçùpçš%!æ-æç-Eä~ã4ä_çæã÷ ‹øP"ÃŠ? #ÃùT-ç./ç$Â5çbç÷&÷ L ) xt*E(Âè-úmè+šàè/ú,Àmõ.šN÷8õ/õçÀõ/ù0¿š&ùçõæ˜9‹1å|2äbåä3å4çæ5å‹˜9lô7Uš
/çbçô8‹}ö—3¾-ù˜7ç-ùE:/ù;¾bçó=˜	óöö‹-ù?ç~Á^çùöAõ/öC¿õ/öEš&öçõæ˜9‹1å|GäbåäHå4çæJå‹˜9lôLUš
/çbçôMèPOèŠã P-Å—ª½¼7çúEQ/RÁbçöT˜Áöğ~¾ÄöŸvvç¢ç£æVã¤ãäšg¥çãæVXºé!á-á	Wá-á
Yá-á[â_âáğï]˜6é/ğ_!aá-ácâ^âáe˜kgUšÄ™4óéh‹Ä¦ããŠf jù˜/óŸ)vç¢ç£ækã¤ãäš¥çãæk¹¶óğğ‹¦ããŠ l»ökğmUš½úQnÅçúmçošúEp/qÁ™‹¼úQsÅŠ tò—	ñ—ªø˜	íí‹(!aæ-æ	Wæ-æuæ-æwçäåúã\çæy¸íø˜
ú2{úğ˜-?ç^çğ}ª(SbÄã‚        ğ`şÿÿÿDa:l  s  d˜w       P @ @ €ˆ Ñ€  / !0"!L  €€ &8	 ^ˆ Ğ
  À€ :p)7Â Ş
€€    ¹e   @ |   È1      ,TÙ
Ia—9  Œ:  á“•e     @ }   É1      ,TX¨`p   L`
   A a“IÅéDp           « Å%b —*ú˜aúÄ8ù‹!Ä!Âb÷
8Ãbùø¨ª(SbÄã‚        9`şÿÿÿDa0u  Ìu  ˜d       @ @    ¹e    @ ~   Ê1      ,T,¨`   ]”De           UšUª (SbÄà®        `şÿÿÿ’aŞu  v  –e     KĞ    33      ,T0‘`   L`   0SbÄã‚        !©`$   RbŞÕ£e   yc  `şÿÿÿDa@v  >w  –,T„‘`Ä    L`   Ib/²í!âéD{          ™ Åkú šÅiúú ‹F!Ä-ùÅ^úù!	Ä-ùÅ^úù—!ÅÄbúù¨ÿLÅÄÿ2ÃbøÃcúùøª  (SbÄà®        I`şÿÿÿDaVv  <w  Ñ˜e       ‘ 
€ 
€ €  ¹e    KĞ    `5      –Df          „ ú% ª  —a       ‘e    @ €   Ë1      Ù,TÙ
¥a§;  §;  ”‘e       EÔ ‚   43      ,T“Í	aÀ;  À;  –•‘e       EÔ ƒ   53      ,T“Iaê;  <  	•‘e     @ „   Ì1      ,Tİ
Ia*<  `<  èd	       ½xßx @    •¹e     @ …   Í1      D,T<¨`4   L`   @SbÄã‚        !© a© ¡©b$   Àÿ Àÿ Rbôf   Gc  `şÿÿÿDaÖx  Ly  •,T@”`:   L`    a© ¡©éDj          « —Åaú Å«ú%%«ª   (SbÄã‚         I`şÿÿÿDay  Jy  !˜b          “e    @ ˆ   _5      •Di          „ ú%%%%% ª  –—a       “e    @ ‡   Î1      ),Tİ
Ia±<  ì<  èd
       Ìyëy @    ”¹e     @ ‰   Ï1      D,T@¨`:   L`   ReR¼u«   addEventListeneréDj(           -  ˜-  ÅÁùø÷\úùª   (SbÄã‚        ‰`şÿÿÿDağy  ^z  ”˜b         •e    @ ‹   Ğ1      ,TÙ
Ia:=  …=  ™”•e     @ Œ   Ñ1      ,T’Ia‘=  á=  
¹e     @    Ò1      ,Td¨`†   L`   áàéDsp          Ÿ?v÷¢÷£ö ¾¤óôš1¥÷óö #ÅÃ- ½»ú/ºñúîí\òñ˜ª¦ó¾Š0 ª (SbÄã‚        Í`şÿÿÿDaØ{  `|  •˜c       ´ à  –e    @    Ó1      ,T\—`v   ]Dq`          | ÅÄŸ1võ¢õ£ô¼¤ñòš#¥õñôÃÁùuºPÄø/4úï¦ñ¼Š" úª (SbÄã‚        RbÒÃ4Ä   Mc  `şÿÿÿDav|  ğ|  •’c       “Bà  –e   @    Ô1      ,TP—``   ]DnH          ~ÅŸ)vö¢ö£õ ½¤òóš¥öòõ ÄÂù/4úù¦ò½Š úª(SbÄã‚        	
`şÿÿÿDa}  l}  •’b       .–e    @    Õ1      ,T(—`   L`    áªDd           2  ª(SbÄá        
`şÿÿÿ”a¬}  Æ}  •’b          –e    ÃÕ‘   63      ,TÙ
Iaë>  ÿ>  	¹e      ÖÑ ’   73      ,T„¨`È   L`
   9–I ¡  ¡™éD{8          Ã-ø  ÄÁböÁ-öÂ]÷öÂ^ùø÷/
—Å’Ä/Ã“ÂÁ/—Àô`÷ö8øÃ“ÂÁc÷öú8øÃbùøª(SbÄã‚        ‘`şÿÿÿDa~  Ô~  —˜e       P €  @  H e    @ “   Ö1      ,T<‘`8   L`   ] áª,RiÊ=2§   type_error:TrustedResourceUrl   “Di          q š- Ålúš-‹ª(SbÄã‚        µ`şÿÿÿDaê~    •b       µ€X ¹e    @ ”   ×1      ,TÈ¨`J  4L`   Rd8ıŒ   trustedTypesRddÏ”   createPolicyR2áÈ<	   goog#html   $¤a      RdB+N
   createHTML  CRdjlñ   createScriptCRe2Y/Ò   createScriptURL CSbJ €         á©`¤ÿ y’rIKd       ?   	     D´8          Œ}ÅÃ-ø  Ä˜m-ù˜gÿø-ùÂÀ})¿3ô3ô3ô	_÷ùõôÅ‹6Âƒ÷Ã§ø÷÷ Á-ö˜÷ À-õÀ-õ	Á¿-ô
¿^öõô÷ú%Œ‹ú%ŒŒÅ˜-úÃ^øú‹Ã‘Áø÷iø÷ª   (SbÄã‚        `şÿÿÿDa€  (‚   ˜f       0€ €
€€€   ¹e    @ •   Ø1      ,T¨`ì  <L`   BpIÉ}Áİáàí!â]
`    La       y aš ! ÅéDp           šª š˜! ¿bô8 ‹ªŸÏvõ¢õ£ô¼¤ñòšÁ¥õñô³ÁÅ!º-ïº-ï
º-ï»_ğïö˜–ú/Ä!º-ï»^ğïù˜ù‹z%»ºù6ğïğÄÃ-ù	møšaø/ùÂUšN—-	º-	!nï#ğš
‹º! ¹bîú$8ï&º8ï'º! ¹!(¸bí÷*¸bîí,8ï.8ğ øP/ÃŠd0¦ñ¼ŠÀ 1ª  (SbÄã‚        I`şÿÿÿDaB‚  è„  0˜j2       Ğ (P € @ ó"À"@h  !÷  ¹e    @ –   Ù1      ,TÙ
©aB  šB  ’–e     ÃÕ —   83      ,T‘Ia¢B  ¾B  	–e      ÖÑ ˜   93      ,Tİ
IaËB  C  èd       ı…š†
 @   ‰e     @ ™   Ú1      D,T”Ia(C  D  •e       º†ú† @ 
@   é
—¹e     @ ›   Û1      D,TÙ
IašD  ½D  ‘	—‘e     @    Ü1      ,T’Ua      “”a      ù RfÆ%   getElementsByTagNameaçD  E  I—‘e     @    :3      ,T’”a      “”a      •ReV9ÛÌ   createElement   aEE  ¹E  I—‘e     @ Ÿ   ;3      ,T’”a      “”a      •Reî±æ£   createTextNode  aŞE  
F  I—‘e     @     <3      ,T’Qd   	   Zc.append   a'F  iF  I—‘e     @ ¡   =3      ,T’Qd      Zc.contains aŠF  fG  I—‘e     @ ¢   >3      ,T’IarG  ÊG  ±—‘e      @ £   İ1      ,T’IaÕG  HH  u‘e      @ ¤   Ş1      ,T9¨`.  0L`   ‘øyèáò ašboÑæá÷Rb±   \+   á˜éD%P          -  Á›¿^öôÅÄ-ÃÁ-À_õùÄoö
šqúmùšj-ÁùE¿^öôÂ&Á÷kö™?Á÷köš5-Áø8ù¿^öôÂ˜ =Á÷kö™&Á÷kö™#Á÷köš‹øD8ùùöÄŠ~ ÿÄÁùnöšª-Á¿_öôùÃÁønö™	únø šúøø-D"8ù!ùöÄ!#Á-%¿ÿ¼ølñ'ùòó™ø‹¼_ôóòñ(¿-ô*Ày, ¾	½_õôóò-Àböõ/ª (SbÄã‚        ±`şÿÿÿDaà‘  n”  0˜j1       P #@"#Â	 F €H L    ¹e    @ ¥   ß1      ,TÙ
IaCJ  rJ  ¼–e     @ ¦   à1      ,T`¨`~   L`   Râzœ   location ÂRr‹ş0   foo SbJ €         ¡©`¤ÿ ‘(Kh@      ;   Ù        )   I     Dr(          ÿùxš-  Ã-øUÅ˜$ÿø(Â-Áb÷öÅ‹Âƒ÷Ã§ø÷÷Åúª§ª (SbÄã‚        ½`şÿÿÿDa–  †—  ‘•c       P    –e    @ §   á1      ,T8’`,   L`   R®áøó   top éDh          ŸÅ-  Äbúù˜-  ‹ª  (SbÄã‚        `şÿÿÿDaœ—  Ü—  ‘˜b         ¹e    @ ¨   â1      ,TÔ’`f  LL`"   R©-   SCRIPT  R²İlç   src RexÍe   ownerDocument   Rdšéº   defaultView Rc6î?­   window  RâöÒû   documentRe®Göq   querySelector   R2I]´   script[nonce]   RcnYÈø   nonce   RdÂ\â   getAttributeIRd.²@æ   setAttribute‰mRd‚>‡¸
   parentNode  Rd&¬   insertBefore”DÀ           ²Ä Ãcùø ÅÃbø2úÄŸ-ù‹—_-ú˜-úÃ-ø
—!Ã-øÃ-øÄŸÂ^ùø÷‹˜-	—-
Ä	Â^ù÷—‹˜-úÄ	Â_ùú÷-ÄÂ^ù÷ Ä/ù"˜-$˜-$Ã-ø&Ä_ùøú(ú‹ª (SbÄã‚        ±`şÿÿÿDaö—  „š  (–h*       ° P P P @ € @ —e    @ ©   ã1      ,T<¨`4   L`   ReîÈÂ¢   getComputedStyleRdÆåW   currentStyleéDi           -  ˜-  ÅÂ_ú÷‹-ª  (SbÄã‚        Á	`şÿÿÿDašš  .›  ”˜b       P ¹e   @ ª   ä1      ,Tˆ•`Ì    L`   RcÛØÏ   crypto  0â6%	Rer5l   getRandomValues Kd       V   ±     D|           !  Ä-ù—!Ã-øÄ]ùøªÿù!
ÃÂøiø÷Å!  Â-÷Â-÷Ã^ø÷ú/úG      G      ª§!Â-÷Ã]ø÷ª  (SbÄã‚         ´`şÿÿÿDaH›  ”œ  e       ` ` @ @H ‘e     @ «   å1      ,Tt¨`¨   L`   É}ÁİáàéDw`          ˜PŸLv÷¢÷£ö ¾¤óôš>¥÷óö 0ÅÃ! ¼-ñ¼-ñ¼-ñ½_òñú	˜ø/¼øğï`ñ¦ó¾Š= ª(SbÄã‚         á£`şÿÿÿDa¨œ  N  •˜d       Ô€
  €€ ¹e  
 @ ¬   æ1      ,Tˆ–`Ğ   L`   yáòYa  àÿÿÿïAYa      ğA‘D|0          -  ÅÂúk÷šªxV4ÄÃúmøš4ùMÁùN8öÁ-	À^õø8öÁ@ö?ùù÷ÄøPÃŠ5 Âùm÷šù‹
Âù8÷ª(SbÄã‚        Á`şÿÿÿDad  „  ’d       Ä!Â Â!“e   @ ­   ç1      ,TÙ
Ia~O  ÀO  M–“e     @ ®   è1      ,T<¨`8   L`   9éDi          ¦Ä-ù  Å^úù˜/t‹ª(SbÄã‚         ¡¤`şÿÿÿDa”Ÿ  ÜŸ  –˜c       €    ¹e  
 @ ¯   é1      ,T„‘`È   L`   RZgå   frames  RFnúÙ   parent  (Kh@      #   )     3   H   Q     D{0          Ã2Áønö šXÿö- T™- À/õTTÅ‹§Åú˜ªÿö-Â˜k÷™	÷ùù‹
‹§öÄù—‹øPÃŠ\ 	ª(SbÄà®        Á`şÿÿÿ–aêŸ  `¡  ”c
       ± 
 Â •e    KĞ °   ?3      ,TÙ
Ia·P  ÊP  I—•e       KĞ ±   @3      ,T@‘`>   L`   0SbÄà®        !©`$   `şÿÿÿ‘a¡  *¢  —¤a      R2ßÄ   display ,T@¨`:   L`   uRdöL(î   setProperty Rr™åó	   important   éDj(          Ä-ù  Ä-ùÅÁø÷\úùª   (SbÄà®        I`şÿÿÿDaÌ¡  &¢  =˜b       @ ¹e     KĞ ³   Í5      –Dj           „ ú%ú¤ Ä} )Ã Âcùø÷ª —b      “   ‘e     KĞ ²   A3      I,TÙ
=a)Q  `Q  ”‘e       KĞ ´   B3      ,TD¨`D   L`   0â6!Ya      0CéDk           !  Ä-ùÅ!  Ã-øÄaúÂ:÷Â^ùø÷	ª  (SbÄà®         p`şÿÿÿ‘aÔ¢  <£  •˜c       P   ¹e      KĞ µ   C3      ,Tp–`   $L`   Rº5±s
   goog_pvsid  É½¤a
      C‰H—SbJ €         a©`¤ÿ ½Kd       .         Dv8          -    ™7ÿú!Ã-øÄ Á})À¯¿aô3õ	÷\ùø‹ÄƒùÅ§úùù!Å-  Äbúù—ÿª (SbÄà®        ½`şÿÿÿ“aD£  r¤  ‘˜d       P “ À  ¹e     KĞ ¶   D3      ,TÙ
Õa=R  (S  —‘•e      KĞ ·   E3      ,TP¨`Z   L`   YÅáüéDn0          ú!  ‹Ä-ùÃ!ÀbõúÀ-õÁ]öõ
Á^øùöª   (SbÄã€        ­`şÿÿÿDal¦  ê¦  ˜d       ` @    ¹e    @ ¸   ê1      ,TÙ
IaS  ¦S  Q–e     @ ¹   ë1      ,T‘Ia±S  	U  Å–e     @ º   ì1      D,T‘ÉaU  ¢U  ”–e      KĞ ¼   F3      D,T‘Aa¦U  2V  •–e      KĞ ¾   G3      ,T9¨`,  LL`"    Rb`îÀ   google_logging_queue¡å]
`    Lb           1	RZÅZ'   hashèRŠGµ¹   google_logging  RdŞŞå+   localStorageRc6³Œù   getItem SbJ €         !ª`¤ÿ YPRr²y'B   https://pagead2.googlesyndication.com/pagead/js/logging_library.js  ] ¡ª áªReÜº   type_error:ConstKd    c   ­   y     D%H          ú| ‹ÄÃÁ-ö —ÃÁ|À2ö õÀ-õ À-õÁz%¿¾ú6ôó	¾ù6ôó	^öõôøúú˜kº«ša%ºÿö À¿bõôÂ˜%ÿÀ-÷¾-ó¾-ó¿½^ôóòlõUš%º¿-ô¿-ô	À¾^õôó˜%º‹Àƒõ
Á§öõõºÅú˜SÁ-öÂ‚Á„À¿öiöõ Å’Á‚qú"š -ú#À‚lõ%š-ú&Àƒlõ(š-ú)‹Àböõ+Å¡Ácö÷ú-ª  (SbÄà¬        `şÿÿÿ•aÖ¬  t°  ’,˜i/       ³€¹ Ì€€
  "@ P €  ¹e     KĞ ¿   H3      ,T|¨`¸   L`
   ¢wR¾Éè   AMP_CONTEXT_DATARdâ´MÃ
   pageViewId  Rd6[%Ì   canonicalUrlKd       0   ‘     Dy           ‹Å-ú  —-úÄ— ÿø-úÂ-÷ —-úÂ-÷Ä‹§øùùøŸ-ø
‹˜ùùøŸ-ø‹˜ù‹ª(SbÄã€        ‘`şÿÿÿDaŒ°  ¸±  –d       P P    —e     @ À   í1      ,TL`T   L`   RcvÔ"2   master  éDm          
¼Äaù ‹Å˜ŸÄ-ú Ãbùø˜-ú ‹‹ª  (SbÄã€        ‰`şÿÿÿDaÎ±  2²  “–b       @ —e     @ Á   î1      ,T„`Ä   L`   yBp”D{@          ‰ÃúøùÀ-ø  lõš’À/ú¿bõôª/úÂÁ-ù  mö	š,!¿ö/ù¾bôó¿öD/ú8ô8÷
÷õÂöPÁŠ/ ’Àbõ÷ª  (SbÄã€        é`şÿÿÿDaJ²  H³  “ ˜f       D ! @ x	    ¹e   @ Â   ï1      ,TL¨`V   L`   Rú^#   google_unique_id”Dm          ½Å¼Äbù Äbúù—- ú—D2ú - ª (SbÄà®        É`şÿÿÿ“aX³  ´  c	        `  ‘e     KĞ Ã   I3      ,T0’`    L`   …éDf           -    š‹ª(SbÄà®        A`şÿÿÿ—a´  ‚´  ”b          ‘e     KĞ Ä   J3      ,TÙ
)aEZ  ®Z  ‘”¹e       KĞ Å   K3      ,TX¨`p   L`   IáüRb²…à   ca- ù–Dp(          — ª- Å]úÅ-ÄÂÁ_ù÷ökúUšÅ8ú	ª(SbÄà®        é`şÿÿÿ”adµ  
¶  ˜c
       P D ’e     KĞ Æ   L3      ,TYa[  €[  ½•’e     ÃÕ Ç   M3      ,TÙ
—aˆ[  ¤[  —•’e      KĞ È   N3      ,T‘a\  #\  ’•¹e     ÃÕ É   O3      ,T‘m	a9\  q\  ”•“e     ÃÕ Ê   P3      ,T˜¨`î   ,L`   •I-	SbJ €         a©`¤ÿ ñõReÚ©„¬   \bdeid=([\d,]+) SbJ €         !©`¤ÿ ’(Kh@      e   9        %   9     D„0          È« œp%ÈÿøÅÿ÷Á-ö Á-öÁ-öÅ‹ÁƒöÂ§÷öö À-õÀ-õÅöú˜ -ú
Ây  À^÷úõÄ˜	/ù‹%È‹Âƒ÷Ã§ø÷÷Èª (SbÄã‚         ¹`şÿÿÿDa¹  ^º  ’˜d       P P “  ¹e     @ Ë   ğ1      ,TÙ
Ia;]  ¦]  !’•e      @ Ì   ñ1      ,T@¨`:   L`   RdâŒvG   performance réDj          Ä-ù  Å˜-ú˜-úÄ]ùú‹ª   (SbÄã‚         Å	`şÿÿÿDab»  Î»  ”b       @ •e     @ Í   ò1      ,Th`Š   ,L`   RcŞXqH   label   ™Rcî¦W   durationRcV¿¬7   slotId  RcNGµ   taskId  0â6RcjØXa   uniqueId‘Dt           ËÄaù —ÊÄaùÅ2 2ú22
Â2÷2!Â-÷Ã]ø÷2ª   (SbÄá        ù	`şÿÿÿ–a ¼  ½  “˜e       ° ,° ,` ° ¹e    ÃÕÎ   Q3      ,TL¨`V   L`   YèRbä    1337éDm(          Î« Å˜#ÉÄaù Å-úxšÄ-úÃÁ^øúöoùÅúª (SbÄà®         I`şÿÿÿDaª½  8¾  —c       @    e      KĞ Ï   R3      ,TÙ
Ia)_  ¤_  ­”e     @ Ğ   ó1      ,Td‘`„    L`    ¡ª áª !«(R$¢ğÀ   google_js_reporting_queue   y“Ds          2  -Å-Ä-ùkúUš)Ï«Åaú	˜!Å-ÄĞÃcúùø-ÅÄ2úùª  (SbÄã‚        ±
`şÿÿÿDa^¿  ü¿  ”˜d       P D`  ¹e    @ Ñ   ô1      ,T„¨`È   (L`    áª !«}$R ZG¹Ë   google_measure_js_timing—0â6 ¡ªéD{(          | 2 ù—2ùÅ˜-ù—|2ù-2 
-ÅÏ«Ãaøù—ú™ú‹Ã!Á-öÂ]÷önø2ùª(SbÄá        Rbª±Ò   ce  `şÿÿÿ]a(À  ÆÁ   ’f       s
`.° `     “e    ÃÕÒ   S3      ,T|”`¸   ,L`    ¡ªRc¼G   goog_   5 ¡¨ERc*ƒ%   _start  )RbFr2®   mark–Dy          -  —ªÌÅúùøiúùÅ-w8úÅ8úÅ-w8úÅ8úÍ«˜Ï«Åaú	˜ÍÄ-ùÅ^úùª(SbÄáÚ        Rc
©   start   `şÿÿÿDaĞÁ  ²Â  ˜d          
€   ¹e    ÖÑ Ó   T3      ,T´¨`(  @L`    ¡ª9±5 ¡¨ERb\Ğ‘   _end)¹ áªy¡åéD           -  ˜-  š†ËÃaøù—ÊÃaøÃ-9ø2ù	Ä-w8ùÄ8ùÄ-w8ùÄ8ùÅÍ«˜Ï«	Äaù˜ÍÃ-ø
Ä^ùøú- T™$  Ä-Ã-ømù™-Ã-øÄ^ùø!ª(SbÄáÚ        Rbª0Y{   end `şÿÿÿDa¸Â  (Ä  $˜g#        	€€ @ P    ¹e    ÖÑ Ô   U3      ,TÙ
Ia!b  Cb  )
’”e     @ Õ   õ1      ,Tİ
IaNb  ­b  èe       ñÄšÅ
 @ ª    =’”e     @ Ö   ö1      D,T•Iaºb  d  i”e     @ Ø   ÷1      ,T•Iad  md   “¹e     @ Ù   ø1      ,T—Iazd  ûe   f       âÉóÉ
 @ T @   @ @q“•e     @ Ú   ù1      D,TÙ
Ua      mQd      .constructoraf  :f  	“•e      ÃÕ Ü   V3      ,T—IaGf  g  }•e     @ İ   ú1      ,TX¨`l   $L`    a® !¡ áª ¡ª !« á¢ !¬éDp           úù‹Ãú2  ù2ø222-
2ª  (SbÄáŒ        ‘`şÿÿÿ‘anÏ  Ğ  ”˜d       ,° ,P    ¹e    ÃÕŞ   W3      ,T(•`   L`    !¬–Dd           2  ª(SbÄáÚ        Rb¢l+â   eb  `şÿÿÿDa
Ğ  $Ğ  ”—b          e    ÖÑ ß   X3      ,T(•`   L`    ¡ª–Dd           2  ª(SbÄáÚ        õ
`şÿÿÿDa(Ğ  BĞ  ”—b          e    ÖÑ à   Y3      ,T(•`   L`    !«–Dd           2  ª(SbÄáÚ         a«`şÿÿÿDaDĞ  ^Ğ  ”—b          e    ÖÑ á   Z3      ,T•`ò  HL`     áª ¡ªÁ	İSbJ €         aª`¤ÿ  !¡ !¬½¤a      ICSbJ €         ¡ª`¤ÿ  á¢Uy’r(Kh@      O   ‰     e   ¬   q     D	h           ÅÄÿø-  ˜@- Â-÷˜5- Á-öÂ-
À]õÀ¿_÷öõôÄaÅ- Á-öÂ^÷öù‹aÅ‹§Âƒ÷Ã§ø÷-ÿö÷Ğ Àbõù-À÷Ã «½¼}	 )»÷Ø º¹bïî!3ğ
#òiòñ%½¼ôóğ\õô'‹"ÀƒõÁ§öõ-)¿ Ù ½¼_ôòñ+õ˜!!-À-õ/ÀŸ-õ1ÁŸ¿^öõô3‹‹¨÷úª   (SbÄáÚ        I
`şÿÿÿDabĞ  TÒ  •0˜j5       P P @ @ @ L€ 
€ 
   ¹e   ÖÑ â   [3      ,T8¨`,   L`   @SbÄâÚ       ı a© !©b¤ÿ ¤   $   `şÿÿÿDaXÒ  ÂÒ  ,TL—`R   L`   0SbÄà¬         ¡©`€ÿ I`şÿÿÿDapÒ  ÀÒ  ±”,T8—`0   L`   İ ¡©éDh           ÿÄ-ù  ÅÃ«Â_úùø÷ª(SbÄà®         I`şÿÿÿDa”Ò  ¾Ò  Á˜b         ¹e      KĞ å   ªC      •Dm0          „ ù‰Å%ú%ù Â-÷ Ãù Á À_ø÷öõª   –—b        e      KĞ ä   ·5      •Dh          „ ú%%% ª  “—a       e    ÖÑ ã   \3      µÅ,TÙ
Iabi  ¯n   á¢e     ÖÑ æ   ]3      ,T@¨`<   L`   8SbÄâÚ       ı !©a¤ÿ $    a§`şÿÿÿDa`İ  8Ş  ”Ò,T“Ia¾n  o  Iõe      KĞ è   š5      éDj           „ ú%%- Ä Â^ù÷ª  —˜b        ¹e    ÖÑ ç   ^3      ù,T(•`
   ]Dd            ª   (SbÄà®        •	`şÿÿÿ”aJŞ  vŞ  ’e     KĞ é   _3      ,T$¨`   ]Dc           ª(SbÄà®        `şÿÿÿa~Ş  ˜Ş  •–’e     KĞ ê   `3      ,TH—`L   ]éDlH      úÿÿÿ‰ÃşùZùúóøõóù÷úôòö™®öôjöõ Áöª  (SbÄó–         å`şÿÿÿ’a¨Ş  ¨Ş  •˜b          ¹e     EÔë   a3      ,T0—`   ]‘Df           ™T‹ª   (SbÄã‚        -`şÿÿÿDa(ß  Xß  ”e    @ ì   û1      ,T\¨`x   L`   yéDq           ÅÄ-  mùš'ù/Â]÷ÃløšøªøšÅùPÄŠ* 	ú—Tªª(SbÄã‚        `şÿÿÿDanß  Dà  –˜c
       D Â ¹e   @ í   ü1      ,TÈ`P  L`   0SbÄã‚        a©`¤   Á`şÿÿÿDaZà  ®á  ‘,T0`   L`   0SbÄà®        á©`$   I`şÿÿÿDaá  8á  Y,T4¨`&   ]’Dg          ÿáÅÄÿÃcúùø ª (SbÄà®         I`şÿÿÿDa"á  8á  i”b          •e     KĞ ğ   œ5      éDf          „ ú% ª  ’”a       •e    KĞ ï   ›5      “D´0          „ ù%ùi ÃùÜ ÁÀ÷`ø÷ Å-ú—ùâ ÃÁcøöªùv ÃÁcøöÃløšùß Ãùá Â/ú	ÁÀc÷öõÂbø÷ªù# Ã ÁcøúöÅlø™lø™‹ùà ÃÁcøúöªùà ÃÁcøúöªª”e      @ D€ "  •e   @ î   ı1      ]m,Te‘`‚   L`   SbJ €         aª`¤ÿ —Å=Ra|  X  Kd    ˜   ²   ¡     DQP          dÂİÀc÷õ Ålúú÷™l÷™(l÷™>‹YvÂcÀİ¾½ô`õôÀc÷õÄ‹>vÂcÀİ¾½ô`õôÀc÷õ	Ä‹!vÂcÀİ¾½ô`õôÀc÷õÄ‹Äù–ú/˜ù/–ÿ÷À_¿½H¼ó`ôó¿dõÃ‹Áƒö Â§÷öªvÂÀc÷õÂl÷šøxªÂl÷šøUªÂl÷š uÂcÀŞ¾½ô`õôÀc÷õ‹Tlú ú÷™l÷ ™"‹>tÂcÀŞ¾½ô`õô!Àc÷õ#‹#uÂcÀŞ¾½ô`õô%Àc÷õ'‹™×Âl÷)šlø*ª	Âl÷+šøUšÂÁ!,Àbõø.Àcöõ0l÷2ªø™›l3÷™ l÷3™l÷3™
l÷3™@l÷3™W‹smø4ªnø5ªÚÂb÷6˜!ÚÂb÷ø8˜!:Áöiö<Á-ö>Â^÷öø@ªøUšÿÂÁ!,ÀbõøBÀcöõDl÷FªøUšÂÁ!,ÀbõøGÀcöõIl÷Kªª   (SbÄã‚        •`şÿÿÿDaÈá  ç  @˜nL       D€ € €  ÆH D€ "1@ Ä#` @ 	€€ ¹e   @ ñ   ş1      ,T8¨`*   ]éDh          T™T™áÅcú TTª   (SbÄã‚        á`şÿÿÿDa"ç  bç  ‘’b          “e   @ ò   ÿ1      ,TH”`L   ]•DlH      úÿÿÿ‰ÃşùZùúóøõóù÷úôòö™®öôjöõ Áöª  (SbÄó–         4`şÿÿÿ—arç  rç  ‘’b          “e     EÔó   b3      ,TÙ
Iaós  
t  Rcê‹“   getValue‘“e      ÖÑ ô   d3      ,TH”`L   ]•DlH      úÿÿÿ‰ÃşùZùúóøõóù÷úôòö™®öôjöõ Áöª  (SbÄó–         Q`şÿÿÿ‘a¶ç  ¶ç  ˜b          ¹e      EÔõ   c3      ,TH¨`L   ]éDlH      úÿÿÿ‰ÃşùZùúóøõóù÷úôòö™®öôjöõ Áöª  (SbÄó–         9`şÿÿÿ—a&è  &è  ’“b          ”e     EÔö   e3      ,Tí	amt  xt  ’”e      ÆÕ ÷   f3      ,TÙ
Iat  ³t  ”e     @ ø    2      ,T‘maÕt  àt  ”“¹e      ÆÕ ù   g3      ,T‘Iaèt  üt  É“•e      ÖÑ ú   h3      ,T‘Ia	u   u  	“•e     @ û   2      ,T‘Ia+u  Bu  õ	“•e     @ ü   2      ,T‘adu  ou  ‘“•e      ÆÕ ı   i3      ,TÙ
Iawu  u  Rc7-Ş   getWidth“•e      ÖÑ ş   j3      ,T’Ia™u  ²u  Rd²Úus	   getHeight   “•e   	   ÖÑ ÿ   k3      ,T’Ia¿u  Öu  1•e     @    2      ,T’Iaáu  øu  		”¹e     @   2      ,T’Iav  v  Õ”–e     @   2      ,T’Iav  ;v  å”–e     @   2      ,T’a]v  hv  ‘”–e      ÆÕ   l3      ,T’Iauv  ‰v  Reªœú   getContentUrl   ”–e      ÖÑ   m3      ,T’]a’v  ’v  ’”–e       EÔ   n3      ,TÙ
a«v  «v  ”–e       EÔ   o3      ,T“Åaßv  êv  –•¹e      ÆÕ   p3      ,T“Ia÷v  w  m•—e      ÖÑ 	  q3      ,T“Iaw  9w  i•—e     @ 
  2      ,T“a[w  fw  ’•—e      ÆÕ   r3      ,T“Iasw  w  -	•—e     @   2      ,TÙ
=a¯w  ºw  •—e      ÆÕ   s3      ,T”eaów  şw  —–¹e      ÆÕ   t3      ,T”Iax  !x  ½–e     @   	2      ,T”Ia,x  Bx  ½–e     @   
2      ,T”µadx  ox  “–e      ÆÕ   u3      ,TH¨`N   ]éDl8      úÿÿÿşùZùúõõùøúöô÷™®÷öi÷ú  Â­÷ª (SbÄñš         `şÿÿÿ–a&ñ  <ñ  ˜b          ¹e    ÆÕ  v3      ,TÙ
Ia´x  Ëx  ñ	—‘e     @   2      ,T’IaÖx  íx  m—‘e     @   2      ,T’Iaøx  y  i	—‘e     @   2      ,T’Iay  1y  —‘e     @   2      ,T’Ia<y  Ry  ‘e     @   2      ,T’aty  y  ‘¹e      ÆÕ   w3      ,TH¨`N   ]éDl8      úÿÿÿşùZùúõõùøúöô÷™®÷öi÷ú  Â­÷ª (SbÄñš         Q`şÿÿÿ•a`ó  vó  ˜b          ’e    ÆÕ  x3      ,TÙ
õaùy  z  ’e      ÆÕ   y3      ,T—Iaz  Az  A‘¹e     @   2      ,T—…acz  nz  ”‘“e      ÆÕ   z3      ,T—Ia…z  z  $Rgz­°   getTagSessionCorrelator ‘“e      ÖÑ   {3      ,T—Ia¸z  ëz  É‘“e     @   2      D,T—Iaöz  ){  Ù‘“e     @    2      D,T—Ia5{  q{  m‘“e     @ "  2      D,TÙ
)	ay{  |  ‘“e      KĞ $  |3      D,T4¨`$   L`    !¬ !«éDg           -  2 -2ª  (SbÄá‚         Rbv×šú   rf  `şÿÿÿDa&ø  dø  ’˜c       ,P    ¹e     @ &  2      ,TIaO|  y|  µ	’–e     @ '  2      ,TIa…|  ­|  0’–e     @ (  2      D,TÙ
Ia¸|  Û|  –e     @ *  2      ,T‘Iaæ|  ~  Ñ“–e     @ +  2      ‚D,T‘Ia~  5~  a“–e     @ 0  2      ,T‘]aZ~  „~  –“¹e     ÆÕ 1  }3      ,T‘Ia‘~  &  1“—e     @ 2  2      D,T‘Ia1  “  5“—e     @ 4  2      D,TÙ
Ia  /€  İ—e     @ 6  2      ,T’Ia:€  X€  ù”—e     @ 7  2      D,TP¨`Z   $L`    !¡ a¡ !¬ a® a« ¡ª áªéDn             2  222|2	|22ª   (SbÄá        Rb~õä   Bf  `şÿÿÿİaä  ~ ”˜d       ,° Ì&,  ¹e    ÃÕ9  ~3      ,Tx–`ª   ]—Dxh       úÿÿÿşù è‹Ãd‹Â‹Áöùõøó÷ò˜¼Zõúïïúğîô™®ôğiôó ¿­ôª   (SbÄñ˜        í`şÿÿÿ“aÀ  ”‘b          ’e     ÆÕ:  3      ,TÙ
Ia  \  ‘’e     @ ;  2      ,T]¨`r  4L`   8SbÄã‚        ¡© ¡ªa$  ¤ÿ Q`şÿÿÿDaÎ ” – !« ¡ªÑæ áªê,T”Ia‚  ‚  I	’e      KĞ =  –5      ¡åy1æ`T  éDI8   8       „ ú%- –
úş Äúı Ãúÿ Âi÷ú Âcø÷ÃÂcùø÷% úÿ  Ä úÿ  Ã úÿ  Â úÿ  Á úÿ  À úÿ  ¿iôú ¿cõô
ÀcöõÁÀc÷öõÂ-À-õÁ]öõÁcø÷öÃcùø úÿ  Äbù úÿ  Ä-Ã úÿ  Âc÷Âcùø÷ Älù"™,Älù#™"Älù$šI-Ã-ø%Ä Â^ùø÷'—3-Ã-ø)ÄÂ^ùø÷+dÄ-Ã-ø-mù/š-Ã-ø	0Ä]ùø2ª   ‘0˜j4      @ @ P @ P #± 
€€"  ¹e   @ <  2      ,TÙ
IaU‚  Ì‚  Ù•e     @ >   2      ,T—IaÙ‚  Nƒ  i‘•e     @ ?  !2      ,T|¨`´   L`
    !¬ a« áª ¡ª !«éDy8   (       úùø Áöiöù ‹Âú2 ø2÷2|2	Àúmõöš£ÀaõÀ¿ú;ômõ2öª  (SbÄáŒ        q`şÿÿÿ•aÌ h ‘˜d       ,° Ì"À¸ ¹e    ÃÕ@  €3      ,TL“`V   L`   Rb¦Ñşø   xa  Áİ”Dm           Å-  ˜-Ä^ùú˜-  ªiú Åú2 úª (SbÄà®         !¥`şÿÿÿ !¥ax  ‘–c
       @ ,  —e    KĞ A  3      ,TL“`V   L`   í a¢”Dm          }  )Äsø~7ùø sø~7ùø sø~7ùø ù2ª (SbÄá         -
`şÿÿÿ‘aF ˆ –c	       S	$`  —e     ÃÕB  ‚3      ,T°“`  ]”Dœ0          l ú™lú ™-lú ™Alú ™U‹nvÅcÃèÁÀ÷`ø÷ÃcúøªvÅcÃèÁÀ÷`ø÷ÃcúøªvÅcÃèÁÀ÷`ø÷	ÃcúøªvÅcÃèÁÀ÷`ø÷Ãcúøªª (SbÄã‚        5`şÿÿÿDaŞ 
 ’–d       ‘ € €  —e   @ C  "2      ,T”¨`ä   ]éD           —ªl ú™ lú ™%lú ™*lú ™/lú ™4‹DrÅÃcúøªuÅÃcúøªtÅÃcúøªuÅÃcúøª_ÅÃHÂù`úù	ªª  (SbÄã‚        ‘`şÿÿÿDa.
 x ’˜c       ‘ €  ¹e   @ D  #2      ,Th”`Š    L`   UReÆÉ¿.   sessionStorage  RcfK}   GGDFSSK b/²Kd       =   ù     Dt(           $—~ªÿù!  Ã-ø˜!  Â-÷Â-÷ÃÁ^ø÷öÅ˜!Â-÷
Ã^ø÷úª‹§ù~ª   (SbÄà®         I`şÿÿÿDa ¦ —d       P P    e      KĞ E  ƒ3      ,Tø”`ª  $L`   í !«É}ùb/¢/éDäP   (       úùø‹Â "À *« ¿bõô À-õÀ "½ *¼bòñ½-ò½÷/ò÷ô½Ÿ-ò
¾^óòù‹Ÿ‹!½iòú ½-ò¾^óòù4õô '«ÀaõÁù/ö™ù/öª /Àbõ÷Àù/õ Ä—øªçÀ!"¾-ó$¿^ôóù&¿bõô(Ä )Àbõù*Ä &Àcõùú,Å™ú‹øª   (SbÄã€        %`şÿÿÿDaÄ 8 “,˜i.       @  P @ à  €  ` @   ¹e   @ F  $2      ,Tœ¨`ö   L`   0SbÄã‚        a©`‚ÿ ‰`şÿÿÿDaN @  a¢,TH”`L   L`    a©Dl(          ÿãÅÿhÄÿÜÂÁø`ùø Ä« Ãcúùøª  (SbÄà®        I`şÿÿÿDa¬ Ğ É’b         “e     KĞ H  ™5      ÉéDˆ8          „ ù% ùÿ"  Ã ùÿ#  Âbø÷ Ã-ø%˜9ù% Ãùi Âùå À¿ö`÷öÂ Ácø÷öÅ˜-úÃ]øú
Ÿ‹ªùh Ãùä ÁÀ÷`ø÷Ÿ‹ª —’d      @ @    “e   @ G  %2      Í,T@”`>   L`
    áª a« !«5 ¡ª‘Dj          ~2  |2~2!Äiùú 	2ª (SbÄá         í`şÿÿÿ !«ah È –˜d       ÌÀ     ¹e     ÃÕI  „3      ,TH¨`P   ]‘Dl@           ú‹Äø (ÂÁúõùôó`÷ö xª(SbÄã€        `şÿÿÿDaà  •b          –e   @ J  &2      ,Td—`‚   L`   ½ÑìéDsH           ú‹Äø!  Â (ÁÀúôùóò`öõÁb÷öÅ!Âb÷ú˜ù‹úª   (SbÄã€        Ù`şÿÿÿDa8   ‘•c
       @   –e    @ K  '2      ,TT—`f   L`   I“Do@           ú ‹Äø (ÂÁúõùôó`÷ö Å šú‹ùª (SbÄã€        9`şÿÿÿDa¶ $ ‘˜b          ¹e    @ L  (2      ,T`¨`~   L`   í!â“Dr@           ú| ‹Äø (ÂÁúõùôó`÷öÅ! Á-öÂ^÷öú˜ú‹ùª (SbÄã€        Å`şÿÿÿDa: ¤ –c	       “ 
€   —e    @ M  )2      ,T\`t   L`   í áªéDq           "Å *« Äbúù Å-úÅ/ú— "Å *ÄbúùÅ-úÅ~Ã4ú
øª  (SbÄã‚        5
`şÿÿÿDaº  “˜c       € à ¹e   @ N  *2      ,TH¨`P   L`   0SbÄã‚        ¡©`‚ÿ I`şÿÿÿDa l “,TÙ
Ia'‰  4‰  Iee      KĞ P  5      •Dl           „ ú% úÿ/  Äbù %ú¤ Ä Âcù÷ª”—b        e    @ O  +2      i,T ‘`ş   L`   @SbÄã€ 
       !© á© !ªb€ÿ €ÿ €ÿ !`şÿÿÿDa† 2 $SbÌ €         aª ¡ªa‚ÿ ‚ÿ ,TX‘`l   L`   8SbÄà®        áª ¡«a$   ‚ÿ I`şÿÿÿDaÎ Ú … !©,Tù¨`®  DL`   í ¡ªuÑæ !«™ aª¡å !ª‰5 á© ¡«	`l  éDåH          ÿdÁÿè¿cöô Å ÿÿ% ÁcöúÄ– ÿÿ" ¾ ÿÿ* « ½bóò¾-ó¾-ó¿½^ôóò
¿Ÿ-ôÀ^õôùÀŸ-õÁ¿^öõô‹Ÿ‹|Ã ÿÿ Áiöú Âlúúö™lö™7lö™Ulö™s‹–ÿbÁ¿ ÿÿ	 ¾ÿA½bòù½÷õ`öõ‹uÿbÁ¿ ÿÿ	 ¾ÿA½bòù½÷õ`öõ‹PÿbÁ¿ ÿÿ	 ¾ÿA½bòù ½÷õ`öõ"‹+ÿbÁ¿ ÿÿ	 ¾ÿA½bòù$½÷õ`öõ&‹Å‹ÿ`Á¿÷õøó`öõ(÷úúúøø˜7 ÿÿ" À ÿÿ* « ¿bõô*À-õ,À/õ.ÀŸ-õ0Á^öõù2‹xÃø˜ÿ«À-õ4Á^öõú6úøø˜? ÿÿ" ¿ ÿÿ* « ¾bôó8¿-ô:¿-ô<À¾^õôó>ÀŸ-õ@Á^öõùB‹xÃø˜ÿ«À-õDÁ^öõúFÿ«—Á ÿÿ" Á ÿÿ* « ÀböõHÅ-úJÀ-õLÁ¿^öõôN—-úJÀ-õ	PÁ¿!
R¾ióú T¾_öõôóV-úJ¿-ôXÀ¾^õôóZÀ-õ\Á^öõù^—#-úJ¿-ô`À¾^õôóbÀ-õ	dÁ|f¾_öõùógÿ«˜/-úJ¾-ói¿½^ôóòk¿-ômÀ^õôùoÀ-õqÁÿ¿^öõôs«Á-u¿]ôw4öùyª (SbÄà®        I`şÿÿÿDaü Ö •`˜v{       @ @ P L"@ @ @ € P P P @ P ` P @ P “€  
€€À  ¹e    KĞ S  ‘5      ‘Dp           „ ú%% ÿÿ/ ÄÃbùø %ÿ!Äÿ«Ã Âcùø÷ª  ’“b        ”e    KĞ R  5      y‘DŒH   0       „ ø%%%%úù%‹%‚÷%%| %|%ø! Á ¿cöúôÁ-ö—	Á-ö˜# øÿ  Á¿¾÷ ùõŸ‹½`öõ÷ª “c
      sP   ”e    @ Q  ,2      ‰™,TÙ
Ia¤‹  Œ  ‰”e     @ T  -2      D,TT¨`f   L`
   Éí áª,T,”`   L`   ½éDe          !  Åbúª   (SbÄà®        I`şÿÿÿDav  “˜b         ¹e     KĞ W  ·C      ‘Do(          #Å!  Ã-øÄ "Â *«Áb÷öÂ-÷Â^ùø÷Ä Ãcúùø
ª (SbÄã‚         ‘`şÿÿÿDa2 ’ ’c      @ @ “e     @ V  .2      É,T\¨`r   L`   í a«‘Dq          &Å "Ä *« Ãbùø Ä-ùÄcúù— 0Å /ÄÃbùøÄcúùª   (SbÄã‚        `şÿÿÿDa¨ æ •’c
       @   “e    @ X  /2      ,TT—`f    L`   ÁİÉ½Å¤a      C–éDo(           -  Å^ú—'!Ä-ùÅ!Âb÷
Â})Á3öø\úùª (SbÄã‚         a¥`şÿÿÿDaü  ˜d       ` @ “ ¹e  
 @ Y  02      ,T,¨`   ]—De            / —ª  (SbÄã‚        Y`şÿÿÿDa¦ Ô ‘b          ’e   @ Z  12      ,T “`   ]—DŒ            5ÅÄ +Ã÷`úù  5ÅÄ ,Ã÷`úù 5ÅÄ -Ã÷`úù 5ÅÄ .Ã÷`úù 5ÅÄ 2Ã÷`úù 5ÅÄ 4Ã÷`úù
ª(SbÄã‚        `şÿÿÿDaê n ‘c       @ @ ’e    @ [  22      ,TŒ“`Ö   L`
   ,TÙ
IaJ  X  I’e      KĞ ]  y5      ,T–Ia`  “  I’e      KĞ ^  z5      ,T–Iaœ  Ï  I’e      KĞ _  {5      ,T–IaØ    I’e      KĞ `  |5      ,TÈ“`L  (L`   ]
`    Mc         
   aÍÉa× a¢U(Kh@   #   h   p  	   {      €  
   D´p           "Á #Àböõ Åz %¿²ôÀ-õÁ¿ÿñ¿]öõ	» eË ğ-ğ—3-ğ»¿ğùù÷!º-ï»-ú¹ù/î¹ù/¸_ğïîíŠA ÿ½¾‹½¾§¼ô—#ÿğ-õŸº]ïõ ¹eË î‹»ó™ğ©ñ§óšò©ª  (SbÄà®        I`şÿÿÿDa( ª $˜g        Ì  
  
  €€[    ¹e    KĞ a  }5      éD}            5ÅÄ  Ã÷`úù  5Å	ÄÃ÷`úù 5Å
ÄÃ÷`úù 5ÅÄÃ÷`úù 5ÅÄÃ÷`úùª (SbÄã‚        Å
`şÿÿÿDa„ ² •–c
      @   —e    @ \  32      = E M U ] ,TL¨`T   L`
   ÁİRdrUù	   init-done   É½¤a      GéDm(          -  ÅÃ^úø—!Ä-ùÅÂ})Áø\úù	ª  (SbÄã‚        }`şÿÿÿDaÌ r ˜c       ` L ¹e    @ b  42      ,Td¨`ˆ   ,L`   8SbÄã‚        ¡© a©a$  ¤   9
`şÿÿÿDaŠ r –,TÙ
IaØ  Ş  I­ e       KĞ d  ~5       !«,TL‘`R   L`   ,T“Iaò  ø  I”e       KĞ f  ”5      éDm(           ÿÿ6 ÄÃÂ  Á`ùø ÅÃù÷`úùª   (SbÄà®        I`şÿÿÿQb      a.j aÈ   ”—b        e     KĞ e  5       a«,TD‘`D   L`   ,T“Ia    I”e       KĞ h  ©5      •Dk(           ÿÿ6 ÄÃÂ  Á`ùø ÅÄbúùª  (SbÄà®         I`şÿÿÿQb      a.g a
 4 ”—b        e      KĞ g  €5       ¡ª,TD‘`H   L`   ,T“Ia+  1  I”e       KĞ j  ’5      •Dk(           ÿÿ6 ÄÃÂ  Á`ùø ÅÃcúøª(SbÄà®        I`şÿÿÿQb      a.h a> p ”—b        e     KĞ i  5       áª•Ds0           „ ú%% úÿ6  ÃÂÁ À`ø÷ 2222ª”—c
      ,° ,  e    @ c  52      ± ¹ Å Ù å ù !,TÙ
IaC  G   !«¹e      ÖÑ k  †3      ,T•IaH  L   áª–—e      ÖÑ l  ‡3      ,T•IaM  Y   a«–—e      ÖÑ m  ˆ3      ,T•IaZ  f   ¡ª–—e      ÖÑ n  ‰3      ,T$¨`   ]éDc           ª  (SbÄã’         Y`şÿÿÿ]ar r –—e      DÔo  …3      ,TÔ`f  0L`    ¡ª0â6Ya{®Gáz„?SbL €         aª`Àÿ –	,TÙ
IaĞ    IY!¹e      KĞ q  ª5       Rf2ıË   /pagead/gen_204?id=  ašSbJ €         aª`¤ÿ Kd    <   ¢   !     DÀX   0       úùø‹Âö÷˜-ú  ‹!¾-ó¿]ôó¿ö—môšzÿô‚ó%%ó× «qø	šø%‹ó× ½iòú 
%ó¤ ½ »còøğóÖ ½¼»ù8ğ»8ğ»còñğÀ˜ó³ ½ó ¼còñõó‹½ƒò	¿§ôòòª (SbÄã€ 
       Ñ`şÿÿÿDaæ    ‘˜e      P ÄJ @H    e    @ p  62      ]!,T@¨`:   L`    ¡ªéDj          Åoú šÅpúš
2 ª   (SbÄã‚        -`şÿÿÿDa¶  ê  ‘“b       1.  e    @ r  72      ,T4•`&   L`   0â6 ¡ª–Dg          !  Ã-øÄ]ùø2ª (SbÄá         9`şÿÿÿDa! B! ‘“c       @    ¹e     ÃÕs  Š3      ,T•`è  PL`$   !URz^.	
   google_srt  0â6	)õ
,TÙ
IaQ‘  W‘  I‘e       KĞ u  ³4       a«RcîAnÃ   completeYRdBf¨ƒ
   readyState  • ¡ªâS,T<¨`4   L`   U’	éDi          !  Å-ú—ÑÅ A«Äbúùª  (SbÄà®         I`şÿÿÿDa# ê# ˜b       @ ¹e      KĞ v  ´4      –D           Ÿ‹ >« Åiúú  Å ?«ú %?!Å-ú  Uš!Å!Ã-øÄ]ùø
Ä2úù =Å ?Ä!Ã-øÃcúùøÙÅ ?ÄÃ A«ÂúiúùÅ @«ú %@ @Ä-ùÅ	 Ã^úùø @Ä-ù
ÅÃ^úùøÅ!Ä-ùÄ-ùkúš!Å-ú—ÑÅ AÄbúù!‹! AÅ-ú#˜‡Å!ÄÃÂ`úù%ª(SbÄà®        I`şÿÿÿDaŒ! î# (—h'      P @ @ P P ± 
€   e     KĞ t  ‹3      ±!Á!,TÙ
Iae’  ’  U	‘e     @ w  82      ,T”!a¶’  Ö’  –‘e      ÃÕ x  Œ3      ,T”Iaİ’  “  e‘e     ÖÑ y  3      ,T”Ia“   “  A‘¹e     @ z  92      ,T”a¸“  Å“  ’‘e     ÃÕ {  3      ,T”IaÉ“  ”  i“‘e     ÖÑ |  3      ,TÙ
a;”  Q”  –“‘e     ÃÕ }  3      ,T•IaX”  „”  Qè“‘e     ÖÑ ~  ‘3      D,T•IaŠ”  ª”  ¡è“‘e     ÖÑ €  ’3      ,T•Ia¯”  Õ”  İ“‘e     ÖÑ   “3      ,T•IaØ”  í”  u“‘e     ÖÑ ‚  ”3      ,T•Iağ”  '•  É“¹e     ÖÑ ƒ  •3      ,T•Ia4•  •  Q’e     @ „  :2      ,T•Qaš•  ±•  •”’e      ÃÕ …  –3      ,TÙ
Ia´•  à•  ‰”’e     ÖÑ †  —3      ,T–Iaã•  –  ”’e     ÖÑ ‡  ˜3      ,T–Ia–  3–  òa”’e      ÖÑ ˆ  ™3      ,T–Ia@–  r–  m”’e     @ ‰  ;2      ,T<¨`8   L`   …¤a      C”éDi           P« Å} )Ä3ùÃúiúùª(SbÄã‚        }`şÿÿÿDaü, @- ˜b       “ ¹e    @ Š  <2      ,T4¨`(   L`   …•Dg           P« ÅÄúøiúù ª(SbÄã‚        ¸`şÿÿÿDaV- Š- —b          ‘e    @ ‹  =2      ,TÙ
IaĞ–  ş–  
—‘e     @ Œ  >2      ,T•Ia	—  0—  ‘e     @   ?2      ,T<’`2   L`    ¡ªÉéDi          -  Uš-Ä]ùÄbùª   (SbÄã‚        …`şÿÿÿDav. Ê. ˜c       @    ¹e    @   @2      ,T4¨`(   L`    ¡ª áª’Dg          -  U™-Äbùª(SbÄã‚        Y`şÿÿÿDaà. "/ ”b       @ •e    @   A2      ,T0–`   L`    ¡ª áª’Df           2  2ª  (SbÄá        …`şÿÿÿUa      ]Qd      .constructoraJ/ z/ $Sb@ €        bÂÿ "/ X0 ”b       ,  •e    ÃÕ  š3      ,TÙ
IaÅ—  Ü—  Éé"¹e      ÖÑ ‘  ›3      ,TX¨`l   L`    ¡ª•…éDp          -  ™.-Ä]ùÄbù«qš‹ ÿÿJ Åbú	‹ª  (SbÄáÚ        é`şÿÿÿDa¾/ V0 –˜c       @ T —e    ÖÑ ’  œ3      ,T”]aG˜  ˆ˜  ”—e     ÃÕ “  3      ,TÙ
Ia‹˜   ˜  É•¹e     ÖÑ ”  3      ,T–Ia¨˜  Ì˜  Rcÿ•ƒ   contains•e     ÖÑ •  Ÿ3      ,T–Qc      S.getId aâ˜  ó˜  I•e      ÃÕ –   3      ,T–Iaö˜  7™  ‰•e     ÖÑ —  ¡3      ,T–IaT™  h™  RcŠƒ?‘   getId   •e      ÖÑ ˜  £3      ,T– ¡¥a?™  ?™   ¡¥•e       EÔ ™  ¢3      ,T– a©a†™  Î™  ù•e     ÃÕ š  ¤3      ,T–aÙ™  çš  ”•e      KĞ ›  ¥3      D,T–Eaëš  '›  •e      KĞ   ¦3      ,TÙ
qa+›  ˜›  –¹e      KĞ   §3      ,T—Ia»›  Ö›  Rd¶g§Ñ   setLocation –‘e     ÖÑ Ÿ  ©3      ,T—Ía ›   ›  ’–‘e       EÔ    ¨3      ,T—Iaã›  —œ  5–‘e     @ ¡  B2      €D,T—Ia¢œ  ª  Õ–‘e     @ ¤  C2      ,T—Ia·  n  a–‘e     @ ¥  D2      ,T0¨`   L`   ¡
ééDf           2  2ª  (SbÄá        É`şÿÿÿUa      ’“a      ù(Rh^o°   google_reactive_ad_format   a= 8= ˜b       ,  ¹e    ÃÕ¦  ª3      ,TÙ
IaŸ  Ÿ  `    •—e       EÔ §  «3      ,T	a Ÿ   Ÿ  ‘•—e       EÔ ¨  ¬3      ,T™a9Ÿ  9Ÿ  ’•—e       EÔ ©  ­3      ,TaRŸ  RŸ  “•—e       EÔ ª  ®3      ,TE	a~Ÿ  ~Ÿ  ”•—e       EÔ «  ¯3      ,T!
a²Ÿ  ½Ÿ  •—e      ÆÕ ¬  °3      ,TIaÓŸ  Á   }–¹e     @ ­  E2      ,T<¨`6   L`    ¡ªRdâ±í   defaultValueéDi          ú‹Äú2  ù2ª (SbÄáŒ         á¥`şÿÿÿ á¥a²A şA –˜b       ,  e   ÃÕ®  ±3      ,T<‘`6   L`    ¡ª$’Di          ú‹Äú2  ù2ª (SbÄáŒ        RbzùáL   Ug  `şÿÿÿ$a*B tB –“b       ,  e    ÃÕ¯  ²3      ,T<‘`8   L`    ¡ª”’Di          ú| ‹Äú2 ù2ª(SbÄáŒ        Rbj¿¼¨   Vg  `şÿÿÿ5$a B ìB “b       s e    ÃÕ°  ³3      ,T<‘`8   L`   I ¡ª”’Di          ú ‹Äú2 ù2ª(SdÄáŒ            şÿÿÿDa’C ŞC —“b       ,  e    ÃÕ±  ´3      ,TX‘`j   4L`   0SbÄá         !©`‚ÿ ¥`şÿÿÿafR S —,TÙ
Qb      Ah.iaH©  `©  I]$¹e      KĞ ³  ‡5       !«,T‘Qb      Ah.gah©  €©  I’“e      KĞ ´  ˆ5       ¡ª,T‘Qb      Ah.jaˆ©   ©  I’“e      KĞ µ  ‰5       a«,T‘Qb      Ah.ha¨©  À©  I’“e      KĞ ¶  Š5       áª,T‘Qb      Ah.maÈ©  Î©  I’“e       KĞ ·  ‹5       !¬éDp          „ ú%~% 2 222	2
ª   ’˜c
      ,° ,  “e     ÃÕ²  µ3      a$m$y$…$‘$,TH¨`J   L`    !« ¡ª$éDl            "Ä „Ãbùø Ä-ù Å-Ã-Â_úùø÷ª   (SbÄã‚         !¦`şÿÿÿDa¶S T ˜c
       P   ¹e  
 @ ¸  F2      ,TP“`^   L`    a« ¡ª”•Dn            "Ä „Ãbùø Ä-ù Å hÃ-øÃ hÂ-÷Â_úùø÷ª (SbÄã‚         %`şÿÿÿDaT jT –—c
       P   e     @ ¹  G2      ,TÙ
IaAª  ««  É–e     @ º  H2      ,T’Ia·«  l­  Í
–e     @ »  I2      „D,T’Iay­  Ø­  }–e     @ Â  J2      ,T’Iaã­  ÿ­  =e     @ Ã  K2      ,TL¨`R   L`
   YRdÊ1åŠ
   CSS1Compat  Rd’n
   compatMode  Re~¬4y   documentElement Rbº|CÉ   bodyéDm          -  ~Å˜Ä-kùš-‹-Åú—~ª   (SbÄã‚        y`şÿÿÿDaH] ^ —˜c	           ¹e    @ Ä  L2      ,T0`   L`   Rd6‰{0   clientWidth ’Df           Åbú Å-ú ª (SbÄã‚        `şÿÿÿDa"^ \^ —”b         •e    @ Å  M2      ,TÙ
ea6¯  Ì¯  •e      KĞ Æ  ¶3      ,TH¨`N   L`   ¡ôRezù0;   parentElement   éDl          ¢Äcù Å˜ Ä-úkùšª-˜Š! ª (SbÄà®        À`şÿÿÿ•a¨_ h` ‘˜c       à  ¹e     KĞ Ç  ·3      ,TÙ
Ia@°  ñ°  ¹—e     @ È  N2      ,T¹aú°  p±  “’—e      KĞ É  ¸3      ,T8¨`,   L`    !¯éDh           ”Åcú ˜- ‹ª  (SbÄà®        }`şÿÿÿ–aèb c ’˜b         ¹e     KĞ Ê  ¹3      ,Tx”`¬   ,L`   Yı$$R †°¦   getBoundingClientRect   ¤a
       á®C !¯CRÂdÓ‹   left á®- !¯SbJ €         ¡©`¤ÿ Kd       I   I     Dx(          ÿø-  Á-öÁ-öÂ]÷öÅ-Â]÷
Ä})Â-ùÁ-ú9ö3÷-ùÁ-ú9ö3÷÷ªÂƒ÷Ã§ø÷ª  (SbÄà®        `şÿÿÿ–a c ^d • —f       P @ P À
€  e     KĞ Ë  º3      ,TÙ
a3²  u²  •¹e      KĞ Ì  »3      ,Tø¨`°  ,L`   ”Rd¢wœl   innerHeight Rd^û¼d
   innerWidth  	(Rhô2w   google_full_width_responsive9%RæøSƒ   width   RcÚ)s³   visible ñéDä   0       -  l™ Äbù˜‹ª èÄ Ãbønù™ª-Ä-
pù™ª ÄbùÅ˜9úÄú;ùÄnùš‹rÄ-kùU˜X- Äbù˜@¢Äcù˜*©Ä-Ãbùø˜p™Ä- kù"™‹-#Š> %˜‹ª(SbÄà® 
       µ`şÿÿÿ—aòd ²g (˜h&       D P ‘ #Ä€€ à  ¹e     KĞ Í  ¼3      ,T°¨`   L`   Ya333333Ó?	­%–Dœ8   (        –Ä Àø÷öô`ùø Åú™	ú‹iÄ-kù™ Äcù˜M Äbù9	˜Äoù
š‹'˜!öÄnùš‹Änùš‹‹
‹	ª(SbÄà®        E`şÿÿÿ–aºg úh ‘d       	€ #   ’e     KĞ Î  ½3      ,TÙ
ua´  ½´  ’e      KĞ Ï  ¾3      ,T—œaÉ´  Ïµ  ’‘¹e      KĞ Ğ  ¿3      ,T—!aÓµ  ÿµ  ”‘“e      KĞ Ñ  À3      ,T—ıa	¶  Š·  •‘“e      KĞ Ò  Á3      ,T0¨`   L`    !£ !«éDf           2  2ª  (SbÄá        )`şÿÿÿaFo vo ‘˜b       ,  “e   ÃÕÓ  Â3      ,T(–`
   L`    !«—Dd           -  ª   (SbÄáÚ         R&/Æè   height  `şÿÿÿDa‚o ¤o ‘b          “e    ÖÑ Ô  Ã3      ,Td–`ˆ   L`
    !« !£0b627—Ds(           ,Åmú š ,Å- múš-‹%!Ä-ùÅ °Ã!Á-ö
Â^÷öÂ_úùø÷ª(SbÄáÚ         ¡ª`şÿÿÿDa¦o (p ’‘d       ±€X P   “e    ÖÑ Õ  Ä3      ,TÙ
Ia¸  ¸   áª’¹e      ÖÑ Ö  Å3      ,T$–`   ]—Dc           ª (SbÄà®         á©`şÿÿÿDaVp ^p ’”e     KĞ ×  å5      ,T|–`¶   L`   A&uéDy8   (       úùø  ‹ÂÁ-ú ˜-ú Àø/õ˜-ú ¿ø/ô¿b÷ô—!¢ÀcõúùÁ˜ø/ö
˜ø/ö¿b÷ô—ª (SbÄà¬        é	`şÿÿÿ‘aDp 
q ’˜d       € €   ”e    KĞ Ø  Æ3      ,T0¨`   L`   0SbÄà®        !©`$   Ñ`şÿÿÿ”aq *q ,T,“`   L`    !£De          -  Åoúª (SbÄà®        I`şÿÿÿDaq *q m&’b       D  ¹e     KĞ Ú  é5      Df          „ ú% ª  –’a       —e     KĞ Ù  Ç3      q&,TX“`l   L`   8SbÄà®        !ª aªa‚ÿ ‚ÿ A`şÿÿÿ‘a2q Àq •¹,T<“`2   L`    !ª& aªéDi          « ˜- Å]úÅ«púTª   (SbÄà®        I`şÿÿÿDa’q ¾q •&˜b       —e    KĞ Ü  ë5      ”Dp   (       „ ú%%˜ úÿ¡  Äcù % úÿ£  «Äcù% ª  •–b        —e     KĞ Û  È3      ™&,T0¨`   L`   0SbÄà®        !©`$   ı`şÿÿÿ‘aÈq îq ,T0`    L`   “”Df          -  Å]úÅoúª(SbÄà®        I`şÿÿÿDaÎq îq ½&–b       ¹e     KĞ Ş  ì5      éDf          „ ú% ª  “–a       ”e     KĞ İ  É3      Á&,T@`@   L`   RdÖ±I   clientHeight•Dj           “Åcú Å ÄbùÄ-ù Edmúª(SbÄà®        ¹`şÿÿÿ—aöq @r ’˜c       @
   ”e     KĞ ß  Ê3      ,Tì¨`˜  0L`   &uRcæD‚   inherit É0b6R1ô	   maxHeight   9%RbîKÔú   HTMLRc:/   tagName éDØ8           Ã À©¿÷ö`ø÷ Å˜úª-Ã-ø Ä-Ã2ø  Ã À©¿÷ö`ø÷Å-Ãù2ø 
ú˜úª!Å-˜©Ã-Â-÷ Âbø÷Ä˜!Â-÷Ã_ø÷úùÅ ÃÀ©¿÷ö`ø÷Ä˜!Â-÷Ã_ø÷úùÅ-˜Ã-	kø ™Šg !úª(SbÄà®        Ù`şÿÿÿ—aHr ´t $˜g"       P ° @ @ P Ä ¹e     KĞ à  Ë3      ,Th¨`Š   L`   0"6å&–Dt(          ÄÀÃbø kùÅ˜1ú˜-! Ã-øÄ ú Â Ábö	Á-öFGÁ_ùø÷ö‹ ú ª   (SbÄà®        ¹`şÿÿÿ•aÈt du ‘d       D
 B
€   ’e     KĞ á  Ì3      ,TÙ
Iaû¼  I½  ñ’e     @ â  O2      ,T–½
aQ½  Q½  ’e       EÔ ã  Í3      ,T–Eaj½  j½  ’‘¹e       EÔ ä  Î3      ,T–Ia”½  ª½   ¡ª‘“e      ÖÑ å  Ğ3      ,T–Maƒ½  ƒ½  ”‘“e       EÔ æ  Ï3      ,T–ùa³½  ³½  •‘“e       EÔ ç  Ñ3      ,T–]	aÌ½  Ì½  –‘“e       EÔ è  Ò3      ,TÙ
øaå½  å½  ‘“e       EÔ é  Ó3      ,T—aaş½  ş½  ‘“e       EÔ ê  Ô3      ,T—ôa¾  ¾  “’¹e       EÔ ë  Õ3      ,T—IaG¾  [¾  RcZ âs   getName ’”e      ÖÑ ì  ×3      ,T—]
a0¾  0¾  •’”e       EÔ í  Ö3      ,T—}	ao¾  o¾  –’”e       EÔ î  Ø3      ,T—=a˜¾  ˜¾  —’”e       EÔ ï  Ù3      ,TÙ
IaÂ¾  İ¾   ¡ª’”e      ÖÑ ğ  Û3      ,T¹a±¾  ±¾  ‘’”e       EÔ ñ  Ú3      ,TIa ¿  ¿   ¡ª’”e      ÖÑ ò  İ3      ,T…aï¾  ï¾  ’”e       EÔ ó  Ü3      ,TEa+¿  +¿  ”“¹e       EÔ ô  Ş3      ,Tİ
Iaa¿  º¿  èd       •ÿ¯ÿ @  ½	“•e     @ õ  P2      D,T–IaÅ¿  QÀ  —d       ¹€Æ€ @  “•e     @ ÷  Q2      D,TÙ
Ia\À  ±À  •e     @ ù  R2      ,T’Ia¾À  ŠÁ  Q”¹e     @ ú  S2      ,T’a¦Á  ÔÁ  —”–e     ÃÕ û  ß3      ,T’IaÙÁ  &Ã  Rc"!w   query   ”–e     ÖÑ ü  à3      ,T’Ia.Ã  Ã  	”–e      ÖÑ ı  á3      ,T’Ua       a©‘a      ù}a´Ã  >Ä  	–e      ÃÕ ş  â3      ,TÙ
Ia@Ä  MÄ  õ
•¹e     ÖÑ ÿ  ã3      ,T–IaNÄ  [Ä   a«•e     ÖÑ    ä3      ,T–Ia]Ä  jÄ  Q•e     ÖÑ   å3      ,T–IakÄ  ¤Å   á¢•e     ÖÑ   æ3      ,T–Ia¦Å  ğÅ  I
•e     ÖÑ   ç3      ,T–IaòÅ  'Æ  •e     ÖÑ   è3      €D,T–Ia(Æ  ”Æ   a§•e     ÖÑ   é3      D,T–éaŸÆ  şÆ  ”•e      KĞ 	  ê3      ,Tl¨`–   L`
   HSbÄà¬ 
       !© a© ¡© !ªc€ÿ €ÿ €ÿ €ÿ 1`şÿÿÿ–a ê‘ $SbÌ €         aª ¡ªa‚ÿ ‚ÿ e(UReÎèæ   queueMicrotask  ,T=•`8  \L`*    !ª ¡ªm(,TD•`B   L`    aª(R$¦OXE   google_rum_task_id_counter  éDk          « Å-ú —2úÅ-ú D2úª   (SbÄà®         I`şÿÿÿDağ ´ i(˜b       , ¹e      KĞ   ¼5       a©İSbJ €         á«`¤ÿ ” ¡© !© aª•é0¤a      5CC9C™C`
   	“”•–’a      AC‰((Kh@   .   u   Ø     1   D   1     D)p          ˆÂÿ« ˜«˜! Á ÀböõÿËÁaöÅÄÃÿôÿóÿ«¼-ñ½_òñ÷Ä‹1½ƒò¾§óòÃÿ«—¨ÿ¼ÿ«»ºcñğï
òÿÀÁ‹ÀÁ§¿«	¾-ó
˜„ú˜€ ÿÿ¾ «¾})½ÿ«»-ğ¼]ñğ3òú3òÿË¼añ—¼ú9ñ3òø3òÿòñ˜%«˜})º¹-î—¸2î!í3ï#ï»hñ¼cóòñ%ô§öšõ©ùª(SbÄá‚         I`şÿÿÿDaÀ è‘ i((˜h'      @ @ Ì€ 0À ³€€   ¹e     @   »5      éDu   0       „ ù%%%%%%%ú‹%‚ø%%ú—! %" U% ª e(“b        ”e     KĞ 
  ë3      q(}(,TÙ
qaûÈ  ,É  ’”e      KĞ   ì3      D,TD¨`D   L`   İéDk8            ¿ÄÁ¿ø÷õ`ùø Ä-ù Å]úùª  (SbÄã‚        
`şÿÿÿDap’ Æ’ “˜b       @ ¹e    @   T2      ,TÙ
IanÉ  ªÊ  9‘e     @   U2      ,T’IaÎÊ  éÊ  ©
”‘e     @   V2      ,T’IaË  DË  ­”‘e     @   W2      ,T’IaOË  „Ë  -”‘e     @   X2      ,T’ÈaŒË  ÜÌ  ”‘e      KĞ   í3      ,T’õaäÌ  SÏ  ‘”¹e      KĞ   î3      D,TÙ
Ia_Ï  ]Ñ  ù’e     @   Y2      D,T“IaqÑ  ‰Ñ  •’e     ÃÕ   ï3      ,T“€a“Ñ  ?Ò  —•’e      KĞ   ğ3      D,T“iaCÒ  ˆÒ  •’e      KĞ   ñ3      D,T“xaŒÒ  ´Ò  ‘•’e      KĞ   ò3      ,T“,a¸Ò  ØÒ  ’•¹e      KĞ   ó3      ,TÙ
IaúÒ  ¤Ó   a¦$Sb@ €         a¦bÂÿ ²¥ J§ •“e      ÆÕ    ô3      ,TL¨`T   L`   •aéDm          UšÅ2ú  Å-ú —ÑÅ Ó«Äbúùª  (SbÄà®        y`şÿÿÿ‘a’§ 0¨ ˜b       @ ¹e     KĞ !  õ3      ,Tü•`²  @L`   9­!0â6a ¡¦ a«¹!Y½! ¡ªâS,T,¨`   ]éDe           ÔÅaú ª  (SbÄà®         I`şÿÿÿDa¬© À© ˜b          ¹e      KĞ #  ²4      •Dè0          ú‹Äú— >« Ãiøú  Ã Ñ«ø %ÑÃ-ø  UšÃ!Á-öÂ]÷öÂ2ø
÷ =Ã ÑÂÁ-öÁcø÷öÙÃ ÑÂ Ó«Àøùöiø÷Ã Ò«ø %Ò ÒÂ-÷ÃÁ^ø÷öÃÂ-÷	Â-÷
køš ÔÃaø‹  ÓÃ-ø˜‡ÃÂÁ À`ø÷ª   (SbÄà¬        I`şÿÿÿDa4¨ Ä© –$—g      ` ° P P ‘€    e     KĞ "  ö3      Y),T<”`6   L`    ¡¦I
•Di(            Ò« Ä-ù Åø÷ö\úùª (SbÄà®        İ`şÿÿÿ“aŞ© ª –—b         e     KĞ $  ÷3      ,Tx”`¬    L`   Y ¡ªRb*Ûg   eid y	éDx8            "Ã ;« Âbø÷ Ã-øÄ]ùøÅ-Tš-ú˜-ú
Ã]øúÃ2ø <Ä Ñ«ÃÀ÷öô`ùøª  (SbÄà®        ™`şÿÿÿ‘aª ´ª ˜d       @ P ,@ ¹e     KĞ %  ø3      ,T4¨`(   L`    ¡¦ a§Dg           Ò« Ä-ù Å_úùª(SbÄà®        y
`şÿÿÿ–a¼ª Şª ’“b         ”e     KĞ &  ù3      ,TÙ
¡asÕ  ıÕ  ’”e      KĞ '  ú3      ,T—éaÖ  .Ö  ‘’”e      ÃÕ (  û3      ,T—Ia;Ö  -×  ]”e     @ )  Z2      D,T—Ia8×  h×  á“¹e     @ +  [2      ,T—Ias×  ¤×  ­“•e     @ ,  \2      ,T—Ia¯×  Ø  a“•e     @ -  ]2      ,TÙ
IaØ  qØ  “•e     @ .  ^2      ,TÍa‰Ø  ©Ø  ’“•e      ÃÕ /  ü3      ,TIa¶Ø  Ù  ­•e     @ 0  _2      ,TIa&Ù  xÙ  Õ”¹e     @ 1  `2      ,TIaƒÙ  ¿Ù  U”–e     @ 2  a2      ,TIaËÙ  
Ú  ”–e      @ 3  b2      ,TÙ
M
aÚ  Ú  ’”–e      KĞ 4  ı3      €D,T‘	a˜Ú  ?Û  “”–e      KĞ 7  ş3      ,T‘IaKÛ  †ä  m–e     @ 8  c2      ,T‘Ia“ä  ï  ı	•¹e     @ 9  d2      ,T‘Iaï  Tï  ı•—e     @ :  e2      D,T‘í
anï  ğ  ‘•—e     ÃÕ <  ÿ3      ,TÙ
Iağ  Kğ  m•—e     @ =  f2      D,T’IaXğ  ğ  =•—e     @ ?  g2      ,T’Ia˜ğ  ˜ğ  `    •—e       EÔ @   4      ,T’Ia¶ğ  9ñ  —e     @ A  h2      D,Tt¨`¨   L`   U,R(ŞşF¼   google_persistent_state_async    ¡¥„éDw            ó˜ óª½Ãaø —! Å-úÄ™ù š-ù™-ù š
ù %ó‹ ò«Âi÷ú  %óÂ2ú
÷ª(SbÄã‚         9`şÿÿÿDaŠâ ä –˜c       P ° ¹e     @ C  i2      ,T8—`.   L`   ±RdfĞÈ
   google_ps_  ’Dh           ô« Å/ú —
Å8úª (SbÄã‚        !`şÿÿÿDaä \ä ”b       @  •e    @ D  j2      ,TL¨`T   L`    ¡¥’Dm            ïÄbù - /Åa4/
‹úª  (SbÄã‚        µ`şÿÿÿDarä  å ”c       € 8€ •e    @ E  k2      ,TD‘`F   L`   0SbÄã‚        ¡©`$  ­`şÿÿÿDaå På ,T$‘`   ]éDc           ª (SbÄà®         I`şÿÿÿDaBå Lå ±*¹e      KĞ G  À5      ”Dk(           „ ú% úÿğ  Ä Áø÷`ùø ª •˜b         —e    @ F  l2      µ*,T(¨`   L`    ¡¥”Dd           ~2  ª (SbÄá         „`şÿÿÿ%	axå ’å b          —e     ÃÕH  4      ,TÙ
Ia<ó  Vó  ù
•¹e     @ I  m2      ,T–Qc      Gj.set  aqó  Nõ  I•e     @ J  4      ,T–Qc      Gj.get  ajõ  ö  I•e     @ K  4      ,T–Qd   
   Gj.isEmpty  a2ö  Jö  I•e      @ L  4      ,T–Qc      Gj.clearahö  ‘÷  I•e      @ M  4      ,T–Ia÷  á÷  •e     @ N  n2      ,T–Iaí÷  ø  9	•e     @ O  o2      ,T–©a4ø  ?ø  “•e      ÆÕ P  4      ,T–%aTø  •ù  ”•e      KĞ Q  4      ,T–Ia£ù  Üú  1e     @ R  p2      ,TD¨`H   L`    ¡ª áªRf¸p   __tcfapiLocator éDk           -  ˜-  ªªÄ-ÃÂcùø÷2 -  ª(SbÄã‚        u`şÿÿÿDaÎõ Lö –˜c       @    ¹e    @ S  q2      ,T<—`2   L`    áªR0Âv   __tcfapiDi          -  Å-ú ™ ûÅbúUª   (SbÄã‚        ‰`şÿÿÿDabö Òö –’b       @ “e    @ T  r2      ,TÙ
Iavû  Zü  Å“e     @ U  s2      D,T•Iaeü  ı  ‰—“e     @ W  t2      D,Tˆ¨`Ì   ,L`    áª ¡ª !¡ á£RdêÜÀ	   timeoutMs    a¡…	 a® a«éD|@      úÿÿÿşù~ÃZşúôôù÷úõóö™®öõiöú  Á­ö2 2~22-ø
÷Ÿ‹ ô2÷-ø÷Ÿ‹2÷2ª  (SbÄñš        Ù
`şÿÿÿQb      Vj.la`ú Bû —˜e       ,° ,P °    ¹e    ÆÕY  4      ,TÙ
Ia±ı  Æÿ  Y–e     ÖÑ Z  	4      D,T—IaÙÿ     Rf¶†2Y   removeEventListener ‘–e     ÖÑ ^  
4      ,TØ¨`n  DL`   0SbÄà¬        !©`€ÿ ‘`şÿÿÿ“aP  ¨ ‘ á« á¥…+ ¡¢åRbv-4´   ja  ,¤a       á«C á¥C•C¹+C ¡ªRcºxÃd   tcunav   áªIi,T4’`(   L`   ééDg          -  Å ÿÿ Ã^úøª(SbÄà®        I`şÿÿÿDaF ^ µ+˜b         ¹e     KĞ `  Ä5      ,T0¨`    L`   é,TÙ
Ia¿  É  I“•e      KĞ b  ˜8      ’Df          -  Å Ã^úøª(SbÄà®        I`şÿÿÿDal ” “”b        •e     KĞ a  Å5      ’DÄX          „ õ%- %-Å-Ä-Ã-ôÂ-ô
Á õÿ  ¿})¾3óú3ó÷3óö3óbôóÅ-úš	¿-ú
¾-ókô™bøú‹> õÿ  ¼»cñğù ¼-ñ"½ »^òñğ$½-ò&¾¼^óòñ(¾-ó*¿^ôóø,ª “,”i.      P P “€
 €"P @   •e     KĞ _  4      Å+Ù+å+,TÄ–`B  $L`    á« á¥å¹+,¤a       á«C á¥C“C”CÁ+éD°@          -  Å-Ä-öÃ-öÂ Á})Àú3õ 	ù3õø3õ÷3õböõ— Á ÷¿¾bôó¿cöúôª îÁaöÄ ñÁ¿cöùôÄ˜ Á ¿bôù¿cöúô‹ KÁ!À¿bõô!Àböõ#ª   (SbÄà¬        %
`şÿÿÿ‘a° À (˜h%       P L€€ € À     ¹e     KĞ c  4      ,Tô¨`¦  8L`    á« á¥å¹+Ù
 ¡ªÉ0RjFE!   Unhandled AutoGdprFeatureStatus:     ¡¦ á¢ áªéDàP          -  Å-Ä-Ã-ÂTš üÁ ÿ«ÀõiõúÀböõ
Â—šøTÃ˜ù˜† íÁböúÅ-ú™P-úÁ]öúÅ˜@XÁ¿cöúô™2XÁ¿cöúôÅlúúö™‹Å‹!ÁÀú8õÀböõ¨‹Å‹# Ò«	À-õ
Á &¿-ú ¾½¼\öõ"ÅúTÄùøøø÷÷÷˜‹ª (SbÄã€        9`şÿÿÿDaÜ * $˜g$       P @ P @ Ñ H @ ¹e    @ d  u2      ,TT¨`f   L`   ’]
`    Lb           “Do(          !  Ä-ùÅz%ÃÂ Áaö6ø÷Â Ácö	6ø÷^úùøª (SbÄã‚        `şÿÿÿDa@ ” •–d       0	à    —e    @ e  v2      ,TD`B   L`   ‘,Tp`   L`   ¤a      eC•¡å ¡¥éDv0           îÃaø Å} )Ã3øø ñÃÁ|Àú÷`ø÷Ä-ùÃ^øù
-úÃ ïÂÁb÷öÂù4ø÷ª (SbÄà®        I`şÿÿÿDaÖ h ˜d       L&@ à ¹e     KĞ g  Æ5      i–Dk          !  Ä ÃùiùøÄ-ùÅ Ã^úùøª   (SbÄã‚         Õ`şÿÿÿDaª € —c      P    ‘e     @ f  w2      Q,,T@¨`<   L`   8SbÄã‚        a© !©a¤   $   a
`şÿÿÿDaš   —,TP•`^   L`   RdVjÇ
   setTimeout  Rb¢.Za   tctoéDn8          Ä-ù  ÅÂ ÿÿK Á!À¿bõôÀböõÁø\úùª (SbÄà®        I`şÿÿÿDaÌ  },˜c
       @   ¹e     KĞ i  Ç5      ‘Dj          „ ú%%! Ä Ãùiùøª  ’“b        ”e    @ h  x2      ,,TH¨`J   L`   Rcz%~Ú   tcnull  ‘Dl          ˜ JÅbú ‹ KÅ! ÄÃbùøÄbúùª   (SbÄã‚        U
`şÿÿÿDa6 „ “c       @    ”e    @ j  y2      ,TÙ
IaÏ ª e	”e     @ k  z2      ,T‘Ia· ç A”e     @ l  {2      ,T‘½aï ï ”¹e       EÔ m  4      ,T‘a  –•e       EÔ n  4      ,T4¨`(   ]éDg           hÅ 	ÃÂù`úù ª(SbÄáÚ          ¡ª`şÿÿÿDa˜ Æ ˜b          •e     ÖÑ o  4      ,T4—`(   ]Dg           hÅ 
ÃÂù`úù ª(SbÄáÚ          áª`şÿÿÿDaÈ ö ‘’b          •e     ÖÑ p  4      ,TH—`L   ]DlH      úÿÿÿ‰ÃşùZùúóøõóù÷úôòö™®öôjöõ Áöª  (SbÄó–         ½`şÿÿÿ“av v ‘’b          •e      EÔq  4      ,TÙ
)a— ¬ •‘¹e     ÃÕ r  4      ,T”Ia¹ 5 y‘–e     @ s  |2      ,T”}aM p ‘–e     ÃÕ t  4      ,T”Iau  Á–e      ÖÑ u  4      ,T”Ia‚ H  a«’–e      ÖÑ v  4      ,T”™al w “’–e      ÆÕ w  4      ,T”
aÑ Ü ”’–e      ÆÕ x  4      ,TÙ
Iaæ û RdÅ!
   getVersion  ’–e   
   ÖÑ y  4      ,T•Ia	 h	 d’¹e     @ z  }2      D,T•Ias	 Ù	 )’—e     @ |  ~2      ,T•Iaä	 ±
 Á’—e     @ }  2      ,T•Ia½
 / —e     @ ~  €2      ,T•Ia: É å“—e     @   2      ,T•¡	aì ÷ •“—e      ÆÕ €  4      ,TÙ
ùa & —“¹e      ÆÕ   4      ,TD¨`H   ]éDk0           Äiùú  ÅaÄÂDÁböÁÀúø`ùøª(SbÄã‚        é`şÿÿÿDaf ´ ˜b       @ e    @ ‚  ‚2      ,T–Ia{  I-”e   
   ÖÑ ƒ  4      ,TH‘`L   ]’DlH      úÿÿÿ‰ÃşùZùúóøõóù÷úôòö™®öôjöõ Áöª  (SbÄó–         I`şÿÿÿ—aÂ Â ”•b          e      EÔ„  4      ,TÙ
Ña´ ¿ ‘”¹e      ÆÕ …  4      ,T@¨`:   ]éDj(           Äiùú  ÅjÄÂúøö`ùøª   (SbÄã‚        İ	`şÿÿÿDa˜ Ü ˜b         ’e    @ †  ƒ2      ,TH“`N   ]”Dl8      úÿÿÿşùZùúõõùøúöô÷™®÷öi÷ú  Â­÷ª (SbÄñš         `şÿÿÿa  6 –—b          ’e     ÆÕ‡  4      ,T(“`   ]”Dd           8 ª  (SbÄà®        I`şÿÿÿDaº Î –—b          ’e    KĞ ˆ  4      ,TÙ
Ia  Á …–¹e      @ ‰  „2      ,T‘IaÌ  ‰–“e     @ Š  …2      ,T‘İa  •–“e       EÔ ‹   4      ,T‘Ia/ Ã q“e     @ Œ  †2      ,T‘IaÎ i Ğ—“e     @   ‡2      D,T‘Iat  I	—“e     @   ˆ2      ,TÙ
Ia! ¨ •—¹e     @   ‰2      D,T’Ia¼  ±	—”e     ÃÕ ’  !4      ,T’Ùa  ƒ –—”e      KĞ “  "4      D,T’½a  —”e      KĞ •  #4      ,T’Ia6 P  ¡ª”e      ÖÑ –  %4      ,T’±a% % ‘”e       EÔ —  $4      ,T’Iaj „  ¡ª”e      ÖÑ ˜  '4      ,T’qaY Y ’”e       EÔ ™  &4      ,TÙ
5a  ”¹e       EÔ š  (4      ,T“Rbö6èw   Ok  a¦ ¦ e.•e       EÔ ›  )4      ,T“IaÖ 7 µ•e      @ œ  Š2      ,T“IaB Í e•e     @   ‹2      ,T0¨`   ]éDf          /Å/9ú ª   (SbÄà®        I`şÿÿÿDa&0 F0 ‘˜b        •e     KĞ   *4      ,T(’`   ]“Dd           / ª  (SbÄà®        I`şÿÿÿDaR0 `0 ‘”b          •e     KĞ Ÿ  +4      ,TÙ
IaR Š ‘¹e     @    Œ2      ,T•Ia— c! M‘—e     @ ¡  2      ,T•<a}! §! ‘—e      ÃÕ ¢  ,4      ,T•Ia³! Ì! Rd.Wâã   getTimestamp’—e      ÖÑ £  -4      ,T,¨`   L`   ½éDe          !  Åbúª   (SbÄà®        I`şÿÿÿDa²C ÊC ’˜b         —e    KĞ ¤  .4      ,T,“`   L`   ”•De          !  Åbúª   (SbÄà®        I`şÿÿÿDaäC üC ’–b         —e     KĞ ¥  /4      ,TÙ
Ia" 9" a	’¹e     @ ¦  2      ,T—IaF" ­$ ù‘e     @ §  2      €D,T—¡aÅ$ Ş$ ”“‘e     ÃÕ ª  04      ,T—Iaø$ %  ¡ª“‘e      ÖÑ «  24      ,T—‘aç$ ç$ •“‘e       EÔ ¬  14      ,T—Ia*% D%  ¡ª“‘e      ÖÑ ­  44      ,T—RbòÂ   $k  a% % /“‘e       EÔ ®  34      ,T—éa`% ‰& —“‘e      KĞ ¯  54      D,Td¨`ˆ   L`
   -Rdà   __uspapiRÒ­Y   __uspapiLocator %éDs          ¹Ä-ù  lùš8¹Ä-ù—/¹Ä-ùÄ-ù—! F«Äiùú 	Å ?Äbùú @Äbùúª(SbÄã‚         É`şÿÿÿDa*M ĞM ˜d       Ä
  €   ¹e     @ ±  2      ,TÙ
Iaó& ‚' u–e     @ ²  ‘2      D,T‘Ia' ^( ±–e     @ ´  ’2      D,T‘Iak( ¾( õ–e     @ ¶  “2      ,T‘IaË( …* ù–e     @ ·  ”2      D,T‘Ia’* å* Q	e     @ ¹  •2      D,T‘Iaò* - 
—¹e     @ »  –2      D,TÙ
Ia- ·- U—‘e     @ ½  —2      ,T’Ua      ©”a      ù RfÎg1   google_ad_channel   aÍ- / %‘e      ÃÕ ¾  64      ,T’íaÊ/ 70 ‘¹e      KĞ ¿  74      ,TÙ
¡a;0 v0 ”’e      KĞ À  84      D,T“µa€0 ?1 •’e      KĞ Â  94      D,T“!aC1 ½1 –’e      KĞ Ä  :4      ,T“
aÅ1 2 —’e      KĞ Å  ;4      ,T“Õa!2 L3 ’e      KĞ Æ  <4      D,T“aV3 ­3 ’‘¹e      KĞ È  =4      ,TÙ
aµ3 é3 •‘“e      KĞ É  >4      ,T”Iaõ3 w4 ı‘“e     @ Ê  ˜2      ,T4¨`(   ]éDg           hÅ ÃÂù`úù ª(SbÄáÚ          ¡ª`şÿÿÿDa i Ni ‘˜b          “e     ÖÑ Ë  @4      ,T,—`   ]De          rÅÃcúø ª(SbÄáÚ          áª`şÿÿÿDaPi xi ‘b          “e     ÖÑ Ì  A4      ,TH—`L   ]DlH      úÿÿÿ‰ÃşùZùúóøõóù÷úôòö™®öôjöõ Áöª  (SbÄó–         D`şÿÿÿ“aşh şh ’‘b          ¹e      EÔÍ  ?4      ,TÙ
IaÖ4 î4  ¡ª’”e      ÖÑ Î  C4      ,T•Iaï4 5  áª’”e      ÖÑ Ï  D4      ,T•õaÅ4 Å4 –’”e       EÔ Ğ  B4      ,T,—`   ]De          sÅÃcúø ª(SbÄáÚ         E#`şÿÿÿDaZj „j ’‘b          ”e    ÖÑ Ñ  F4      ,TH¨`L   ]éDlH      úÿÿÿ‰ÃşùZùúóøõóù÷úôòö™®öôjöõ Áöª  (SbÄó–         1`şÿÿÿ’a0j 0j ˜b          ¹e     EÔÒ  E4      ,TH`L   ]‘DlH      úÿÿÿ‰ÃşùZùúóøõóù÷úôòö™®öôjöõ Áöª  (SbÄó–         Y
`şÿÿÿ–a¨j ¨j “”b          •e     EÔÓ  G4      ,TH`L   ]‘DlH      úÿÿÿ‰ÃşùZùúóøõóù÷úôòö™®öôjöõ Áöª  (SbÄó–         Í`şÿÿÿ—aìj ìj “”b          •e      EÔÔ  H4      ,T4`(   ]‘Dg           qÅnÄÂcù÷ Äbúùª(SbÄáÚ          ¡ª`şÿÿÿDaRk „k “”b         •e     ÖÑ Õ  J4      ,T4`(   ]‘Dg           qÅnÄÂcù÷ Äbúùª(SbÄáÚ          áª`şÿÿÿDa†k ¸k “”b         •e     ÖÑ Ö  K4      ,T,`   ]‘De          rÅÃcúø ª(SbÄáÚ          !«`şÿÿÿDaºk âk “”b          •e     ÖÑ ×  L4      ,TH`L   ]‘DlH      úÿÿÿ‰ÃşùZùúóøõóù÷úôòö™®öôjöõ Áöª  (SbÄó–         `şÿÿÿa0k 0k “”b          •e      EÔØ  I4      ,TH¨`L   ]éDlH      úÿÿÿ‰ÃşùZùúóøõóù÷úôòö™®öôjöõ Áöª  (SbÄó–         e`şÿÿÿ“aôk ôk ˜b          ¹e      EÔÙ  M4      ,T<‘`2   ]’Di(          wÅ QÃÂ YÁù`úù ª   (SbÄáÚ          áª`şÿÿÿDafl l ”•b          –e     ÖÑ Ú  O4      ,TH‘`N   ]’Dl8          gÅ QÃcÂ YÀ¿ö`÷ö Âù`úùUª (SbÄáÚ          a«`şÿÿÿDa l ül ”•b         –e     ÖÑ Û  P4      ,TÙ
Ia6 ›6  ¡ª”–e      ÖÑ Ü  Q4      ,TH‘`N   ]’Dl8          gÅ RÃcÂ YÀ¿ö`÷ö Âù`úùUª (SbÄáÚ          !«`şÿÿÿDa8m ”m ”•b         –e     ÖÑ İ  R4      ,TH‘`L   ]’DlH      úÿÿÿ‰ÃşùZùúóøõóù÷úôòö™®öôjöõ Áöª  (SbÄó–         `şÿÿÿaDl Dl ”•b          –e      EÔŞ  N4      ,T<‘`4   L`   9’Di          }Å Z« ÄÛÃcúùø  %Zª  (SbÄã‚        İ`şÿÿÿDaüm "n ”•b          –e    @ ß  ™2      ,T4¨`$   L`   $R ºÃG«   google_ad_modifications éDg          -  ú—~Ä2ú ùª  (SbÄã‚         á¦`şÿÿÿDa8n ¾n ˜b       ,  ¹e  
 @ à  š2      ,TT“`f   $L`   Re6ÛX   space_collapsing$Rg’§b\   remove_ads_by_default   $¤a      áG	CuC“$Rg'gş   ablation_viewport_offset”éDo           \Äbù - —Å-˜})Äú3ù-	3ùù‹ª (SbÄã‚        ä`şÿÿÿDaÔn Îo ˜d       P “    ¹e    @ á  ›2      ,TÙ
Iaò7 g8 ı
—‘e     @ â  œ2      ,T8¨`*   L`   (Rhkö;   allow_second_reactive_tag   •Dh           \Å¹Äbúù Å2ú ª   (SbÄã‚        E`şÿÿÿDaäp .q —b       ,  ‘e    @ ã  2      ,T’Ia¢8 ı8 I—‘e      @ ä  2      ,TH”`P   L`    Rf*d¨K   head_tag_slot_vars  ReRÒ}   google_ad_host  éDl           \Åbú ÅŸ-ú ÅŸ-ú‹Ÿ‹ bÅbúª(SbÄã‚        õ`şÿÿÿDar ˆr ˜c       P    ¹e    @ å  Ÿ2      ,TP”`Z   L`
   Y]8R4áå©,   meta[name="google-adsense-platform-account"]iRcÖ|Æô   content —Dn           -  ÃŸ!-øÄÂ^ùø÷ÄŸ-ùÅÃ^úùø‹Ÿ‹ª   (SbÄã‚        ¥`şÿÿÿDar †s ‘’c
       @   “e    @ æ   2      ,T|¨`¶   L`   0SbÄà¬        a©`€ÿ ı`şÿÿÿ‘a¸s Nt I,TÙ
Ia: &: Iµ1¹e      KĞ è  á5      éDy@   (       „ ÷%ú%‹Ä‹ÃÁlö š ÷ÿe  Ácöùø˜‹ ÷ÿi  Á ¾úõùô`öõª ”˜b      ‘  •e     KĞ ç  S4      ¹1D,TÄ¨`B   L`    a« áª !«I ¡ªy–D°          ˜-  Å]ú˜rÅ-Ä]ùÄÃcúùø‹y-
Å]ú˜mÅlú™cÅ-Ã]øÃ-øÄ]ùøÄ-ùlúšC-Ä]ùÄ-ùÅ]úùÅ/ú Ålú"š#rÅ-Ã]ø#Ã-ø%Ä]ùø'ÄÃcúùø)‹‹ª   (SbÄà®        E
`şÿÿÿ“aVt u ,—i+       P P Ä  
 P  	      •e     KĞ ê  T4      ,TD‘`D   L`   RBHT   tmod–Dk          sÅÃcúø ÿÅlúUš
2 ª  (SbÄà®        ¥
`şÿÿÿ•a&u ru ”—b       Ä ¹e     KĞ ë  U4      ,TÙ
Aa½: ; ”–e      KĞ ì  V4      €D,T—Ia; ›; q	”–e     @ ï  ¡2      ,T|¨`¸   L`   @SbÄã‚        ¡© a© á©b$  ä   ‚ÿ `şÿÿÿDaPw ^x ”Å,T—Iaê; -< I2–e      KĞ ñ  â5      éDy            „ ú%%%ú  Äbù — úÿj  Äbù%˜úÂ Ä!ÃÂbø÷Ãbùø%úŠ ÄÃ Âcùø÷
ª•˜c      ` @ ¹e    @ ğ  ¢2      2,TL’`X   L`   0SbÄã‚        a©`‚ÿ Á`şÿÿÿDatx  y ,TÙ
IaU< …< I)2e      KĞ ó  ã5      –Dm           „ ú% úÿk  Äbù ~%ú¤ Ä Âcù÷ª•—b        e    @ ò  £2      -2,T¨`à   (L`   9¤a
       á«C á¥C á« á¥ ¡ªÉ	éD~           …Å eÄbúù ˜S}Å Z« Ä~«Ãcúùø Å})Ä3ù Z3ùbúù	-™ lÅ-Ä]ùÄbúù‹~‹ lÅ-Äbúùª(SbÄà®        5	`şÿÿÿ–a.y Úy ˜e       0€
     ¹e     KĞ ô  W4      ,TÙ
Iaû< Ş= Y—‘e     @ õ  ¤2      D,T’Iaé= > a—‘e     @ ÷  ¥2      ,T’ea%> W> •—‘e      KĞ ø  X4      ,T¥¨`  ,L`   | a« áª ¡ª• !«y	Rbşõİ   hostéD‘0           aÄÃbùø ˜	 c« ªøŸ-øÄ]ùø‹˜zuÄ-Ã]øÃ	Âcùø÷
ÅöŸ%-öÂ]÷öÂŸ-÷Ã]ø÷ÃŸ-øÄ]ùø‹˜kúš—	 c« ª n«ÄÃbùø_ÄÂBÁø`ùøªøŸ-øÄ]ùø‹˜Å÷Ÿ-÷Ã]ø÷!ÃŸ-ø#Ä]ùø%‹Å˜;Ä-ú'lù)š/˜+/ú*Älù,šuÄÂcù÷-ÄÃ-ø/Ã-ø1kù3™	 c« ªõŸ1-õÁ]öõ4ÁŸ%-ö6Â]÷ö8ÂŸ-÷:Ã]ø÷<ÃŸ-ø>Ä]ùø@‹—	 c« ª n«ÄÃbùøB_ÄÂBÁø`ùøDª c« ª   (SbÄà®        `şÿÿÿ“a¶|  <˜mF       @ @ @ 	€€  
€€"D
 €H P @   ¹e     KĞ ù  Y4      ,Tô¨`¢  L`   |“±ç¡å’Dà(          | Å c« Ä …Ã uÂbø÷—" …Ã tÂbø÷— …Ã vÂbø÷˜ o«ÃcøÄ …Ã uÂbø÷	˜-ùÃÁ^øùöTš-úÃÁ^øúö …Ã tÂbø÷˜-ùÃÁ^øùöTš-úÃÁ^øúö …Ã vÂbø÷˜-ùÃÁ^øùöTš-úÃÁ^øúöúª   (SbÄà®        a`şÿÿÿ‘a j€ ”$•g       “ € 
€€€ €     –e     KĞ ú  Z4      ,TH—`L   L`   ¹éDl0   (        rÅ z« Äùø÷öõiùø Äbúùª  (SbÄã‚        Œ`şÿÿÿDa‚€ ¾€ ˜b         ¹e    @ û  ¦2      ,TŒ¨`Ò   $L`   0SbÄã‚        !©`$   ™`şÿÿÿDaÔ€ R •¤a
       á«C á¥C á« ¡ª á¥,TÙ
Ia‹@ š@ Iµ2—e      KĞ ı  ¾5      ,T4`$   ]éDg           ÿÿt ÅÄbúù ª  (SbÄà®         I`şÿÿÿDa8 N ”–b          —e      KĞ ş  ¿5      •D}@          „ ú% úÿO  Ä úÿN  Ã úÿ  Â} )ÁÀ-õ3öúr À¿-ô¿¾cõôó3ö	b÷öÂ Ácø÷öÃÂcùø÷ª   ”–d      ³ 
€ €  —e    @ ü  §2      ½2Å2,T“Ia´@ [A U¹e     @ ÿ  ¨2      €D,Td¨`†   L`   0SbÄã‚        !©`$   -`şÿÿÿDaÌ‚ Lƒ ‘,TÙ
IavA —A Ií2’e      KĞ   Á5      ,T4“`$   ]éDg           ÿÿw ÅÄbúù ª  (SbÄà®         I`şÿÿÿDa2ƒ Hƒ –˜b          ’e      KĞ   Â5      —Ds(          „ ú% úÿO  Ä úÿN  Ã úÿv  ÂÁb÷ö Â Ácø÷öÃÂcùø÷ª –b      @ ’e    @   ©2      ñ2ù2,T•Ia±A ÷A q
¹e     @   ª2      ,T¨`Æ  4L`    á« ¡ªRcÂlFŸ   invtag   !« a« áªy	u2`  éDñH           aÁ-  Àböõ˜*rÁ-À¿cöõô— KÁ!À¿bõô
Àböõª-  Å-Ä-ÀŸ-õÁ]öõ‹˜óŸ%-ó¿]ôó¿Ÿ-ôÀ]õôÀŸ-õÁ]öõ‹Å˜]ÁiÀ a¾½úô`õô À-õ"mö$™1 …Á mÀböõ%˜.ÁiÀ b¾½úô`õô'À-õ)mö+š mÁÀböõ,‹ÅŒ
õŸ-õÁ]öõ.‹˜ôôŸ-ôÀ]õô0ÀŸ-õ2Á]öõ4‹ÃòŸ1-ò¾]óò6¾Ÿ%-ó8¿]ôó:¿Ÿ-ô<À]õô>ÀŸ-õ@Á]öõB‹Âø˜˜Á-øDlöFšŒ/øGÁùlöIš€÷˜|ÁiÀ a¾½÷ô`õôJÀ-õLmöN™1 …Á mÀböõO˜MÁiÀ b¾½÷ô`õôQÀ-õSmöUš.uÁ¿cöôVÁ-úXÀ-õ	Zlö\š mÁÀböõ]÷úú‹Åú˜Ù ¶Áiöú _ÄiÁ a¿¾úõ`öõalÁ¿ùõó`öõcÄiÁ ±¿¾úõ`öõelÁ¿ùõó`öõgÄ …Á mÀböõi˜dÁiÀ b¾½úô`õôkÀ-õmmöošE cÁiöú piÁ b¿¾úõ`öõrÅlÁ¿õúó`öõtÅjÁ¿ùõúó`öõv JÁböùxÅ‹ KÁ!À¿bõôzÀböõ|Åúª (SbÄã‚        ­`şÿÿÿDa„ Pˆ `˜v~       P @ P @ P P ‘ 
 @ @ @ P  P ‘ 
 P D€ €  €H @ @ ¹e    @   «2      ,Tl¨`’   $L`   0SbÄã‚        !©`$   `şÿÿÿDafˆ Ôˆ •,¤a       á«C á¥C…+`d    ¡¢C á« ¡ª á¥,T4`&   ]éDg           ÿÿx ÅÄcúù ª (SbÄà®         ¡¢`şÿÿÿQb      Vj.Ga¶ˆ Îˆ 93–b          —e     KĞ   Ã5       ¡¢”Du0          „ ú% úÿ   Ä} )ÃÂ-÷3øúr ÂÁ-öÁÀc÷öõ3ø	 3øbùøª   •–d      ³ 
€ €   —e    @   ¬2      A3,TT`d   L`   0SbÄã‚        !©`$   ­`şÿÿÿDaêˆ n‰ ,TÙ
IaƒD ’D Ie3¹e      KĞ 
  ™8      ,TH¨`L   L`   I áªéDl            ÿÿy Å-  ÄbúùÄ-ùÅÃÂ_úùø÷ª  (SbÄà®        I`şÿÿÿDa(‰ j‰ ’˜c       P    “e     KĞ   š8      –Do(          „ ú% úÿO  Ä úÿN  Ã Ácøö ÃÂcùø÷ª  ’—b        “e    @ 	  ­2      i3q3,T@”`:   L`   Re:ÆQ@   abg::amalserr   ,¤a       ²_CRcÓÜœ   guarding	RcEE	   timeout `d   Rbiª«   rateYa{®Gáz„? ²_Ya{®Gáz„?–Dj            ÖÅ Ä} )Ã3øÂ`úùª   (SbÄã‚        µ`şÿÿÿDa„‰ Š —b       “ ¹e    @   ®2      ,T<¨`4   L`    á« ¡ª !« áªéDi    (       2  222ª  (SbÄá        ¹`şÿÿÿDaBŠ Š •˜c       ,°    –e    ÃÕ  [4      ,TÙ
UaXE PF ”•–e      KĞ   \4      D,T“aZF lF •¹e      KĞ   ]4      ,T“EapF ¾F –—e      KĞ   ^4      ,T“IaÊF —G í–—e     @   ¯2      ,Tx¨`¬   L`   èPéDx          —ª-  ˜-—ªÿÅ-Ä^ùkú™ª €ÅbúÅkú	™ÿÅ-Ä^ù
kú™‹ª  (SbÄã‚        Ü`şÿÿÿDaF N ˜d       @ ‘€H    ¹e    @   °2      ,TÙ
Ia2H nH 	—‘e     @   ±2      D,T’]aŒH ½H U—‘e      ÃÕ   _4      D,T’IaÊH I É—‘e      @   ²2      ,T’IaI ŒI 5‘e      @   ³2      D,T’Ia—I ÅI É	¹e     @   ´2      D,T¤¨`  $L`   ,R(½É¦    google_reactive_ads_global_state,RiîÊ€   sideRailProcessedFixedElements  }$Rg:ÛÿJ   sideRailAvailableSpace  5 Rfú1V   sideRailPlasParam   YéD          -  ˜g-  Å-úš-  Å!Äiùú Ä2úù-  Å-ú
š-  Å!Äiùú Ä2úù-  Å-úš-  Å!Äiùú Ä2úù‹ †«Äiùú Ä2 ù-  ª (SbÄã‚        `şÿÿÿDa¢“ Æ—  ˜f       ` ,P ° ° ,  ¹e    @   µ2      ,TŒ¨`Ò   HL`     Rf*ît:   wasPlaTagProcessed  (Rh5¢`   wasReactiveAdConfigReceived Rc¥Qo   adCount  Rf¾‹ì   wasReactiveAdVisibleRd^
*   stateForType(Rhòò
å   reactiveTypeEnabledInAsfe   (RhŞã1   wasReactiveTagRequestSent   ,RiŞL+   reactiveTypeDisabledByPublisher ReÄ§   tagSpecificState$Rg²¤ıC   messageValidationEnabled Rf¦×)   floatingAdsStacking }945=4A4éD}          2  ~2~2~2~2~2
2~2~22	 ‡Äiùú 2
!Äiùú 2!Äiùú  2"!Äiùú $2&ª   (SbÄá         Y`şÿÿÿUa      ÙQd      .constructoraò— d› (˜h(       ,° ,° ,° ° ° ° ,  ¹e     ÃÕ  `4      ,T4¨`$   L`   $Rgúû¤   maxZIndexRestrictions    Rfh­/   nextRestrictionId    Rfz3b    maxZIndexListeners  ’Dg           ~2  2|2ª  (SbÄá         •`şÿÿÿ•aš› Dœ –—c       ,0    e     ÃÕ  a4      ,TÙ
ya+N ¹O “–e      KĞ    b4      D,Th‘`Š   L`   8SbÄã‚        a© !©a¤   $   
`şÿÿÿDaŠŸ   –ReFXë"   ama_ran_on_page ,TH‘`P   L`   ÑéDl            ÿÿŠ Å ÿÿ‹ « ÄÃÂùiùø Äbúùª(SbÄà®         I`şÿÿÿDaÒŸ üŸ É4˜b         ¹e      KĞ #  º5      –Dt(          „ ú%% úÿ\  Äú¹ Ãbùø Ä-ù— úÿÁ  Ä éÃ Âú Á`ùøª   —b      @ ‘e    @ "  ¶2      Ñ4,T`¨`~   $L`   0SbÄã‚        !©`$   ™`şÿÿÿDa  è   á« áª ¡ªRòp†   google_ad_clientI,Tl“`’   L`    á« ¡ªÍ4éDu8          Ã-ø  ÅÃ-øÄ ÿÿ\ Ãÿ¹Âbø÷Ã-ø— ˜ ÿÿŒ Ãú÷ùöõô`ø÷ª   (SbÄà®        I`şÿÿÿDaj  ä  õ4˜c
       @   ¹e     KĞ %  ½5      —Dr0          „ ú% úÿq  ÄÃ-ø ÃÂ-÷ÂÁ-öÁ-ö—Á À`ùøª ‘c
      P   ’e    @ $  ·2      ı4,T4¨`(   L`    á« ¡ª áª—Dg           2  22ª(SbÄá        Ñ`şÿÿÿUa      $–a      ù(R$jw¡‹   google_analytics_domain_namea¡ R¡ ˜b       ,° ¹e    ÃÕ&  c4      ,TÙ
Ia·P pS q‘“e     @ '  ¸2      D,Tô¨`¦  €L`<   YReJ©ÎF   getElementById  Rcn!İ<   _host   Rc¿ƒo   aswift_ ½Rê‹êú   google_ad_width Rt7    google_ad_heightRj?   div ±uR)¥   border  Rb^ä­   px  &±%Rb*eè4   0px R&.¤   margin  RjUîå   padding RcRº³t   relativeµ%Rb/‚r
   visibility  RdJ¡÷ÿ   transparent R2ßÙÔ   backgroundColor Rd^‰îÔ   inline-blockE¤a
      ÙC¥C“”éDà0          -  ÅÄú˜-Â8úÀ^÷õ˜ÂùuÁPÄö8÷ÅŠ' 	ú!
Â-—Áb÷öÅ!
Â-—Áb÷ö! Á-öÂÀ^÷öõÄ82ù	-ù
Ã2øwÁ8ö!2ø"úwÁ8ö$2ø%2ø'2ø)2ø+2ø-2ø/2ø1}3)Â3÷4ù3÷6÷ª (SbÄã‚        å
`şÿÿÿDaø¦ Bª 4˜k8        	 Âk P P À
`¸ p``É    ¹e    @ )  ¹2      ,T@¨`>   L`   ™
]Rb=QR   dev I•Dj          -  Å-Äú—Ãùløš‹ª (SbÄã€        y`şÿÿÿDaZª ´ª —b       ‘e    @ *  º2      ,TÙ
ÅaŞU V —‘e      KĞ +  d4      ,T–Ma…V …W ‘¹e      KĞ ,  e4      ,T–1a‰W ™X “’e      KĞ -  f4      ,T–Ia¥X "\ E’e     @ .  »2      ,T–Ia\ (] ‰	’e     @ /  ¼2      ,T	¨`Î  DL`    áª ¡ª$YyR& àT   headaÍRbÖôä(   METARdªöP×   appendChild R×P   origin-trialRd¡P”	   httpEquiv   1U(Kh@   R   ©   x  
   ¼   Ï   ˆ     Dôp           "¿ „¾bôó ¿-ô À ¾-ó¾ ½-ò½_õôóòÅ¹À-õ
Ä-ú˜ª-ù˜¤²ú¿-ôÀ¾ÿğ¾]õôº eË ï-ï—E-ïº¾ïøøöï˜0-ù˜*²º	¹bïîÅ-ù¹-î
º^ïîú 2ú"ö2ú$öŠS &ÿ¼½‹¼½§»ó—#ÿï-ô'Ÿ¹]îô) ¸eË í‹ºò™ï©ğ§òšñ©ª (SbÄã‚         `şÿÿÿDahº È» ,˜i+       P P P @ @ ° Ü    ¹e     @ 0  ½2      ,TP¨`^   L`   YRe.‹wC   featurePolicy   Rc°6	   features±çéDn(          ú!  ‹Ä-ùÁŸ-öÂ]÷öÂ-÷Ã^ø÷ú
‹xª (SbÄã€        Õ`şÿÿÿDaŞ» \¼ ‘c       P @ ’e    @ 1  ¾2      ,TP“`^   L`   ”6Re^<$¬   allowedFeatures ±ç•Dn(          ú!  ‹Ä-ùÁŸ-öÂ]÷öÂ-÷Ã^ø÷ú
‹xª (SbÄã€        -`şÿÿÿDar¼ ş¼ ˜c       P @ ¹e   @ 2  ¿2      ,TH¨`N   ]•Dl8      úÿÿÿşùZùúõõùøúöô÷™®÷öi÷ú  Â­÷ª (SbÄñš         `şÿÿÿ•aD½ Z½ ‘’b          “e     ÆÕ3  g4      ,TÙ
Ia·^ Ë^ I-‘“e   
   ÖÑ 4  h4      ,T8”`.   ]éDh           ZÅÃGÂb÷ Âù`úùª (SbÄã‚        ñ`şÿÿÿDa°½ ä½ ˜b         ¹e    @ 5  À2      ,T8¨`.   ]Dh           ZÅÃGÂb÷ Âù`úùª (SbÄã‚        `şÿÿÿDaú½ .¾ ’“b         ”e    @ 6  Á2      ,T8•`.   ]Dh           ZÅÃGÂb÷ Âù`úùª (SbÄã‚        }`şÿÿÿDaD¾ x¾ ’“b         ”e    @ 7  Â2      ,T8•`.   ]Dh           ZÅÃGÂb÷ Âù`úùª (SbÄã‚        ˆ`şÿÿÿDa¾ Â¾ ’“b         ”e    @ 8  Ã2      ,T8•`.   ]éDh           ZÅ	ÃGÂb÷ Âù`úùª (SbÄã‚        y	`şÿÿÿDaØ¾ ¿ ˜b         ¹e    @ 9  Ä2      ,T4¨`$   ]‘Dg           lÅ
Ãù÷`úù ª  (SbÄã‚        å`şÿÿÿDa"¿ R¿ “”b          •e    @ :  Å2      ,T8–`.   ]‘Dh           ZÅÃ>Âb÷ Âù`úùª (SbÄã‚        `şÿÿÿDah¿ ¿ “”b         •e    @ ;  Æ2      ,T8–`.   ]‘Dh           ZÅÃGÂb÷ Âù`úùª (SbÄã‚        Ô`şÿÿÿDa´¿ è¿ “”b         •e    @ <  Ç2      ,T8–`.   ]éDh           ZÅÃ>Âb÷ Âù`úùª (SbÄã‚        a`şÿÿÿDaş¿ 2À ˜b         ¹e    @ =  È2      ,TH¨`N   ]’Dl8      úÿÿÿşùZùúõõùøúöô÷™®÷öi÷ú  Â­÷ª (SbÄñš         =`şÿÿÿavÀ ŒÀ ”•b          –e     ÆÕ>  i4      ,T¸—`,  ,L`   0SbÄã‚         !©`‚ÿ …`şÿÿÿDašÁ ğÃ ”iReÚº¼¹   userAgentData    Rf
ËqÕ   getHighEntropyValuesR¶byA   google_tag_data Rd¢' Â   uach_promise¥i,TÙ
Ia¾a Şa Iù6¹e      KĞ @  ¢5      éD¤0          „ ù%ù¹ Ã-ø ÃŸ-øÃŸ-ø‹ ™ªù¹ Ã-øŸ‹ù¹ Ã~Â2ø÷%Ã-ø
˜
Ã-ø
ªù¹ Á-ö Á-öÁ-öÂ ùÿ¤  «À^÷öõÂ-÷Ã Á^ø÷öÅÃú2øúª  –˜e      P ,P @ ° —e     @ ?  É2      7,Tô¨`¤  8L`   R"?<S   architectureIRRW¼   bitness Rc®õh   mobile  Râ×àÏ   model   RF¡”O   platformR¾&—Ë   platformVersion R–E­€   uaFullVersion   R¦bº½   fullVersionList é,Td“`†   L`   Rc¢.Ê—   brand   Rcb30   version Ds0           ™Äiùú  ÅZÄÂGÁ- ÀböõÁúø`ùøÅZÄÂGÁ-Àböõ
Áúø`ùøª (SbÄà®        I`şÿÿÿDa†Å 
Æ ‘d       @ @    —e     KĞ B  ï5      R^Â   wow64   DàX            Å ŸÄ Ã šÂ ¡Á œÀ ¢¿ ¾ ›½ £¼iñú  ¼- —»còñğ½-—¼cóòñ¾-
—½côóò¿-—¾cõôóÀ-—¿cöõôÁ-—Àc÷öõÂ-—Ácø÷öÃ-ÁŸ-ö	 Â
 À^÷öõ"‹—|$Âcùø÷%Ä-'—Ãcúùø)ª  (SbÄã‚        Å`şÿÿÿDa
Ä 2Æ ”,‘i+      @ P @ P @ @ “€    —e    @ A  Ê2      E7,TD“`F   L`   i,T,“`   ]De           ¦Åbú ª  (SbÄà®        I`şÿÿÿDarÆ ‚Æ ”‘b          ¹e     KĞ D  ¡5      éDk           ¥Äaù ÄŸ-ù Å Ã^úùø‹Ÿ‹ª (SbÄã‚         ±`şÿÿÿDaHÆ ’Æ ”˜b      @ e     @ C  Ë2      y7,T@¨`:   L`   1‘Dj          -  — bÅbú˜
2 ª   (SbÄã‚        İ`şÿÿÿDaªÆ Ç “b       ° ¹e    @ E  Ì2      ,TÈ•`J  4L`   IR¶8Ë?   google_sa_impl  YI5R¶‡‹S   google_shimpl   R†q:v   google_sa_queue  ¡¦,T4•`$   L`   ¹7éDg           ªÅ¹Ä-ù  Äbúùª  (SbÄà®         I`şÿÿÿDa¼È îÈ ˜b         ‘e      KĞ G  ¶5       RÆ?Ş   google_process_slots±–D´@           úù ‹Ã¹Â-÷ ˜(¹Á-öÁ-öÂÀ^÷öõTš¹ÂW÷¹ÂW÷¹Â-÷—\¹Â|
2÷¹Â Ò«À-õÁ × ¿ ¾_öõôó2÷	 ¬Âøöúõùô`÷öÅ¡Â¹Á-öÁc÷öúÂÁ2÷
öª   (SbÄã€        ü`şÿÿÿDa0Ç TÉ  —f      P 0  €     ¹e    @ F  Í2      ½7,T”¨`æ   (L`   0SbÄã‚        !©`$   ı`şÿÿÿDajÉ 8Ê ’1æ ¡¦I
y,,T4•`"   ]éDg           ÿÿª ÅÄbúù ª   (SbÄà®         I`şÿÿÿDaÊ .Ê é7˜b          ¹e      KĞ I  «C      “D@          „ ù%Â-÷ Ã]ø÷Å š ùÿÒ  «Â-÷Ã Ø Á_ø÷öúÃ-ø˜5ù Â-÷
Ã ùÿÒ  «À-õÁ × ¿ ¾_öõôóÁÀ_ø÷öõª ”•d      P P @ –e    @ H  Î2      í7,TL¨`X   L`   ¹7±7¡å“Dm           -  ú—|2ú -˜b‹-  Ä-ù	Å^úùª(SbÄã‚        •`şÿÿÿDaRÊ Ë •d       Ì
€€    –e    @ J  Ï2      ,TÌ`V  4L`   É¤a
      Rcÿƒê   client  CRbF‘   plahC%8	u2)8(Rh†«ÜX   PublisherCodeNotFoundForAma —a      Rb*¾   bustC58éD¸            rÄÂcù÷ ˜- ‹-~ÅrÄÂcù÷˜D— “Ä¹Ãbùø˜$}
)Ä3ù¹Ã-øÃ-ø3ùùù‹!ÄÃbùø¨~ ­Äcùú ­Ä †Ãaø˜}	)Ã †Âa÷3ø
ø‹~Ãcùøú Äcùú"ª (SbÄã‚        `şÿÿÿDa(Ë ÆÌ $˜g$       P 0 €À €€I @ ¹e    @ K  Ğ2      ,T<¨`4   L`   0SbÄã‚        a©`¤   ©`şÿÿÿDaÜÌ 4Í ,TÙ
Iaxf ˜f IM8’e      KĞ M  ¸5      éDi           „ ú%ú¤ Ä Âcù÷ ª  –‘b         ’e    @ L  Ñ2      Q8,Tu“`¢  TL`&   ]
`    La       -¡åyRçc—
   google_esf  eSbJ €         ¡ª`¤ÿ R>Iu‹   IFRAME  ±¥µ¤a      RcfëV   hello   Rcêú    world   	IuE(Kh@   9   {   é     –   ²   ¡     DaP          z  %¿¾½-ò6ôóôú|ÄÂÁ÷u¾PÂó/úÁ˜[-ù	¿^ôùöÿô-ö˜>À-ö½-òmõš.  ¾-únóš -ú¾-ö¼õ/ñ¼^óúñõPÀŠ6‹§ôŠe ùúÄ-úmùš9ÿôù/ú¾-ó!¾-ó#Ã˜·«ø%·‹$‹¾ƒó¿§ôóóùP%ÄŠ< &¿·«ô%··«˜ª²¿¾bôó'Ã2ø	)2ø
+ …¿ x¾bôó-˜-/‹-1 …¿ q¾bôó3˜¿}5)½côò6½bò8½-ò:¾]óò<2ø>-ø@¿2ôBøª   (SbÄã‚        °`şÿÿÿDaNÍ ²Ğ <˜mD       ³€& €"Ä€ x/
 ^`
  &P ,P    ¹e   @ N  Ò2      ,Te¨`†  XL`(   @SbÄã‚        a© !© aªb¤   $   ‚ÿ `şÿÿÿDaÌĞ nÕ ’Ù¥é5 Rf–G±¦   google_start_time   A‰2¤a
      ™
Rerğ»K   m202309140101   ]C–RdFú@
   google_lrv  $Rgk>   google_async_iframe_id  RdZ'ƒ
   google_bpp  R2œİ   google_sv_map   YI5M5,TÙ
Ia»i Ái I¡8¹e      KĞ P  Ğ5      ,T’Qb      In.daÂi Ûi I“”e      KĞ Q  Ñ5      ,T¬¨`  HL`     aªRd*Üá   isConnected —Åµ8‘RcG>$   no_div  ±7$¤a      Rc²Ò£   pubWin  CRbö½õí   varsCReÿ)‰   innerInsElement Cí8ñ8õ8 ¡¦ a§éD˜(          « Å˜-ú —;Ã-øÃ-øÄ!ÂÁ-ö	Áb÷öÂ8÷Â^ùø÷Åš!ÄÃbùø¨Ã-ø	Ä}
)Â3÷3÷ú3÷^ùø÷Å˜ ÿÿÒ «Ã-øÄ Â_ùø÷ú ª  (SbÄà®         I`şÿÿÿDaÂÓ fÕ ¡8$˜g"       P Ğ€€  €É 0@   ¹e      KĞ R  Ò5      –DQH   (       „ ø%%% øÿ  ÂÁÀc÷öõ Â-÷Å-÷%-ÂÀ^÷õ øÿ°  ÂÁ¿õ`÷ö
Â-÷Ÿ‹ø «!Áiöú Á-öÂ]÷öÄÂ øÿ  Á})Àøu ¿½côò3õ	böõ2÷
Âú2÷Â2÷!Ânù#š	9ù$‹2÷%Â-÷'—~2÷)Â-÷'Â4÷ú+ øÿ…  Â øÿp  Áb÷ö-—Á-ö/Á-ö1Â8ú3À^÷öõ4˜ ‹ øÿ«  ÂÁÀô`÷ö6ª —4k8      P @ @ 0	€`"pÀ
  H    ‘e    @ O  Ó2      Á8É8Õ8,TM¨`X   L`L   Refœğ†   google_ad_outputReÂìpw   google_ad_formatU5Y5Rb¶êÛN   html á®ReBºÓD   google_ad_slot  $Rg‚SQ   google_override_format  Rb
‚´   aa   RfÚØ¾   google_loader_used  áüI(Rh¢ ¯œ   google_reactive_sra_index    RfÏ8   google_ad_unit_key  ]
`    Le
                       $Rg.«œ~   google_orig_ad_format   ReQñ§   google_ad_type   Rfv‘P>   google_orig_ad_width$Rgr¬‡X   google_orig_ad_height   RcÂ¾ ù   nodeType¡å±q Â, aŸ	 ¡œRc>-?   nodeName9%RdJ‘ó¶
   childNodes  yù`  SbJ €         áª`¤ÿ (RhÆÚš"   google_ad_dom_fingerprint   `"  Kd      ù  Ù  
   D9€           -  Å-Ä-—Ã-—Âù—»úlğUšúU—8ø	»÷8ğ
Ä-T™/-—) ”»-º8ïº-8ï/ğTš»-	lğÅù˜ú˜-ù
»]ğùÄ‹Äù2-  š-•%z %»º-6ğï!º-#—-6ğï!º-%6ğï!º-'—-6ğï!º-)—-6ğï!ğù|+ÅÃ÷÷÷˜D»ønğ,š:	»-÷-lğ/š-ú0»¹^ğúî2‹-ú0»-÷4¹^ğúî6-÷8ÂøP:ÃŠB ;-ú<»]ğú>Å˜-ù@»^ğùúB¥¹-ùD¸¶^íùëF¸bîíH¹-îJº]ïîL2N|PÄÅ– »únğQšı	»-RlğTUš-UÃ˜»ø8ğW‹Ã˜Š-X˜„-Z˜~-X¹-î\º]ïî^º-ï
`»]ğïbÂ-Z»-ğd¿¾½-ôfmòhšNò/ôi¼-ñk˜9-ñk¹-îmº]ïîoº-ï
q»]ğïs»÷lğušñlvš»ó8ğwÂ‹óPx¾òPy½ŠQzÂ-ù@»-Xùï˜-X·-ì{¸]íì}¸-í
¹]îí¹ø8îƒ¹÷8î„¹^ğïî…-ZúP‡Å Š  ˆ -ùD»]ğùŠ»8ğ‰|ŒÄ˜pÿğ-!ÁÅö˜Slö™LºúnïšB-ö"‘ÀÃ-õ“mø•š!ø/õ–l˜š-ù@º^ïùø™‹øP›ÃŠ$œö-!ÁúPÅŠQ ‹ºƒï#»§ğïï¥¹-ùD·]ìù 8Ÿ¸bîí¢¹-î¤º]ïî¦2$¨ª(SbÄã‚        á`şÿÿÿDaˆÕ âß ‘€˜~ª       P D   €X ,P Ì
 €
`Æ"P  /P @ @ ÌX ± X @ P D
  
€€!Ş    x!Ì"±€"D ^H @    ¹e   @ S  Ô2      ,TX¨`p   L`   y4¤a
      RdîF•ò	   debugCard   F RfºgÖ   debugCardRequested  HéDp            ÄÃbùø Å˜+ …ÄbùúÅ-ú Ä/ù—-ú ÄÃ})Â4ùø	÷ª(SbÄã‚         `şÿÿÿDaøß Üà “c       P L ”e     @ T  Õ2      ,TH•`L   L`   i,Tx•`¬   $L`   )b/¢/	a      ,R(ÖUcè   google_user_agent_client_hint   Kd       0   €     Dx@          Ä8« ù%8ÿ÷! À-õÁ-¿]ô¿M¾_öõôóÅÃÄ‹ÃÄ§Â%8÷§ù¡ ‹ø©‹Å¹Äú2ù
ª  (SbÄà®        I`şÿÿÿDa6á "â ’˜c       P ° ¹e     KĞ V   5      Dl            §Äaù ÅUš-ú Ä Â^ùú÷ –Äaùª  (SbÄã‚         E`şÿÿÿDaöà 0â ’–c      @    —e     @ U  Ö2      ™9,T0¨`   L`   0SbÄã‚        !©`$   Õ	`şÿÿÿDaHâ zâ ,T,’`   L`   •éDe          - Å@ú xª(SbÄà®        I`şÿÿÿDa^â xâ É9˜b       °   ¹e     KĞ X  ê5      –Df          „ ú% ª  —a       ‘e    @ W  ×2      Í9,Tl¨`˜   L`   •µ–Duh   (   úÿÿÿşùø÷ö‹ÀZùúïïùôøò÷ñúğîó™®óğióò ¾­óö2 õ2ª(SbÄñ˜         !§`şÿÿÿUa      µ
•a      ùRePÆ$   image_stacked   a¶â ã ˜b       ,° ¹e   ÆÕY  j4      ,TÙ
Ia‰q ›q M’e      ÖÑ Z  k4      ,TX¨`j   L`
   Reê\?Ú   google_ad_resizeu&e5Rb&Éã·   rpe éDp           -  ˜ …Å rÄbúù˜-Å-Ä]ù	Ä8ù2ú2ª   (SbÄáÚ         áª`şÿÿÿDa8ã Úã ˜d       P ° `  ¹e    ÖÑ [  l4      ,TÙ
IaĞt 5z ñ’”e     @ \  Ø2      ,T•IaBz { 	’”e     @ ]  Ù2      ,Tá¨`z  àL`l   •(RhNí;‹   google_responsive_formats    á¦0Rjn}äÇ#   google_safe_for_responsive_override  áª0Rjº4$   google_full_width_responsive_allowedRcf[ä   gfwrnwer !«RcÎ©Û   gfwrnher !¬U5$RgâKÓÖ   google_resizing_width    a«Y5$Rg‰Eï   google_resizing_height  © ¡ª&: á®9 a®,RiÎ »¹   google_responsive_auto_format   RbnÆµ=   armr Rfj)!\   google_ad_resizable !9(RhâÚì   google_loader_features_used e5Rc
1°í   gfwrnh   !¡Rc6­m   gfwroml  a¡RcN£X   gfwromr Rc¦Ì®=   gfwroh  Rc®W®   gfwrow   á£Rcpµ   gfwroz  U	(RhN˜ƒ½   google_responsive_dummy_ad  ]
`    (Mh               
            b/¢/¤a
      RdºT‰   googMsgType RcvïÖû   adpnt   Rd^D	   key_value   ”`   L`   •a
      Rb~sp   key Rb¦Rà   qid RcRh|O   DUMMY_AD áŸÙ(Rh¢Mø<   >window.top.postMessage('   $Rgòd¬ê   ', '*'); 
          </  8Rl^ºè’,   > 
          <div id="dummyAd" style="width:Rdr^@ 
   px;height:  ”R~ÜËˆ   px; 
            background:#ddd;border:3px solid #f00;box-sizing:border-box; 
            color:#000;"> 
            <p>Requested size:0Rj‚!à‹#   </p> 
            <p>Rendered size: $RgF•†   </p> 
          </div>  RbÇxÖ   dashéDÍ0           -  Uš-  Á2ö-Uš-Á2ö-Uš&-Â÷šÁ2
ö‹2
-Á2ö-Uš-Â÷Uš-Á2ö-	—-
ÅUš
ú2ú-—-ÅUš
ú2ú-Á]ö Á-ö"Â^÷ö$Å-Á]ö&Á-ö(Â]÷ö*Ä-,˜ …Â rÁb÷ö.˜•ú2
0ù22-Â]÷4Ã-ø6Â^÷ø8Â8÷:Â-ø;Á]öø=8÷?2@-B2D-FUš-FÁ2Hö2J2L € 2N-Â÷š"-À]õQÀ-õSÁ]öõUÁ8öPÁ2Wö-YUš-YÁ2[ö-]Uš-]Á2 _ö-Uš-Á2!aö-	Uš-	Á2"cö-#eUš-#eÁ2$gö Â!%iÁb÷ök—!%i Â-&mÁ'Àc÷öõo˜È&Âz(q%Á-BÀc÷öõr—Â-Fl÷t˜¨Â-Fl÷uUš›!)vÁ-ö*xÂ}+zÀ^÷öõ{,Á º«-w8ö}Á.8ö}Áw8ö}Á/8ö}Á ºw8ö}Á08ö}Áúw8ö}Á18ö}Áùw8ö}Á28ö}Áúw8ö}Á8ö}Áùw8ö}Á38ö}Áúw8ö}Á8ö}Áùw8ö}Á48ö}Á25~öª   (SbÄã‚        L`şÿÿÿDa`ö ¤ d˜w€       ,P ° ° ° ° P P @ ,@  € ¸ ,P ,° @
€€
``
`  &Äh L    ¹e    @ ^  Ú2      ,TÔ¨`h  8L`    a®Á• ¡ª á¦ áª !« !¡ a¡ a« !¬ á£DÀ`   h       úù‹Ã‹Â‹Á‹À	‹	¿
‹
¾‹½‹¼‹»‹ºú2  ù2ø2÷2ö2õ2
ô2ó2ò2ñ2	ğ2
ï2ª(SbÄáŒ        Å`şÿÿÿõaÌ @ ’“e       ,° ,° ,° ,° ”e    ÃÕ_  m4      ,T(•`
   L`   –éDd           -  ª   (SbÄáÚ         ©`şÿÿÿDaH l ˜b          ¹e     ÖÑ `  n4      ,TÙ
Ia[‚ ”‚  ¡ª”–e     ÖÑ a  p4      ,T—ÑaI‚ I‚ ”–e       EÔ b  o4      ,T—Ìa™‚ „ ‘”–e      KĞ c  q4      ,T—İa“„ H‡ ’”–e      KĞ d  r4      ,T—IaT‡ ˆ X”–e     @ e  Û2      ,T—Ia#ˆ ²ˆ =–e     @ f  Ü2      ,T—IaÈˆ Ùˆ  ¡ª•–e      ÖÑ g  t4      ,T—IaÚˆ 3‰  áª•–e     ÖÑ h  u4      ,T—Qd      bo.image-topa³ˆ ³ˆ •¹e       EÔ i  s4      ,TÙ
U;aK‰ i‰ RdÂ÷2ƒ	   image-top   •—e      KĞ j  v4      ,TUa      U’a      ùRdÒ'òı   image-middleay‰ ¡‰ u;—e      KĞ k  w4      ,T’a      “Qd      .image-side a¯‰ Ø‰ RdêïÉ
   image-side  –—e      KĞ l  x4      ,TQd      bo.text-onlyaå‰ Š RdbB&2	   text-only   –—e      KĞ m  y4      ,T’a      “Qd      .in-article aŠ AŠ RdB¯\P
   in-article  –—e      KĞ n  z4      ,TIa^Š ~Š  ¡ª–—e      ÖÑ o  |4      ,TaLŠ LŠ —–¹e       EÔ p  {4      ,TÙ
ìaƒŠ • ’–e      KĞ q  }4      ,T0¨`   L`   0SbÄà®        !©`$   ¥`şÿÿÿ”a:! À! –,TT“`b   L`   yéDo          Ä-ù E ÅÄúoùšÃú/øÄ^ùø—ªúQÅŠ  	ª   (SbÄà®        I`şÿÿÿDa@! À! Õ;˜c
       °€ˆ À ¹e    KĞ s  í5      –Df          „ ú% ª  —a       ‘e     KĞ r  ~4      Ù;,Tt“`¢   L`   Ñæ•–Dw@           É¿-ô  À¾^õôóÅ-úÄÃÂùm÷š.÷/úÁbö	˜˜bö˜öªøšöøø÷PÂŠ/ øª   (SbÄà®        `şÿÿÿ“aÈ! è" d       P  ^  ‘e     KĞ t  4      ,T¨`à  @L`   =­%¤a
       ¡¡C á¡`    ¡¡Rdöf‹³   autorelaxed :a
       ¡¡C á¡C á¡a
       ¡¡C á¡Ga
       ¡¡C á¡`   a
       ¡¡C á¡C9%a
       ¡¡C á¡Ca
       ¡¡C á¡CéDü8   0        Ã- køš})Ã3øøø‹Ãkøš- — ÌÃbø—-	˜P —Ã÷öõô`ø÷™})Ã3ø3øø‹})Ã Âb÷—3øø‹}	)Ã3øøø-Å-Äù™}
)Ã3øù3ø!ø‹0-#˜}%)Ãú3ø&ù3ø(ø‹}*)Ã3ø+ù3ø-øª(SbÄà® 
       5`şÿÿÿ”a& @( ,˜i/       ÄÉ ‘€`2À “ &P “
`2À “ ¹e     KĞ u  €4      ,T…¨`È  PL`$   PSbÄà® 
   
    !ª á© ¡© a© !©d$  ¤  $  ¤   $   9`şÿÿÿ‘aH( Ä* •,T@`<   ]“Dj0           ÿÿÊ ÅÄÃÂÁÀ`úù ª  (SbÄà®         I`şÿÿÿDa’( ´( E<–b          —e      KĞ w  æ5       ¡¡ á¡u±%& ¡¦ !¦MRbş‡J   Wa  RÒ
õØ
   marginLeft  e5IRÂú	   marginRight RVë›   zIndex  ÅéDqĞ   0       „ î%%%%% îÿÕ  ¸ ÷ · ¶cíìë ¸-íÅ-íÄùÃî© ¸·-ì·-ì·bíì
Âî© ¸·-ì·-ì·bíìÁ îÿÍ  ¸¶êéèúìøç`íì¸-íÀ-í¿-í	¾-í
½ îÿÎ  «¸·cíìóÃ îÿ  ¸·¶êî© é`íì¼˜	8ñ‹» îÿ  ¸·¶êî© é`íì¼˜	8ñ!‹º îÿ  ¸·¶ê`íì"—¼ îÿ¼  «¸éíøìõëóêòèùçôæğåïäöã÷âñáiíì$ªE<(˜h&      P @ @ P @  H    ¹e     KĞ v  4      I<,T<¨`6   L`   <Rm"Sê-   ^((^|,) *(horizontal|vertical|rectangle) *)+$   R’Di           Åkú ™y  Ä-ùÅ^úùª (SbÄà®        ı`şÿÿÿ‘aÌ* V+ ”b       q@ •e     KĞ x  ‚4      ,T¥—`  lL`2   XSbÄà®        aª !ª á© ¡© !© !®e¤  $  ¤  $  $   €ÿ u`şÿÿÿ“a^+ t3 ’Ya      Ğ?0b6]
`    Lb           ¡å(Rhò¢ƒ   google_max_responsive_height”`    La       ,T(—`   L`   µéDd           -  Tª  (SbÄà®        I`şÿÿÿ !¬a|- Œ- ‘<˜b          ¹e     KĞ z  ç5       a¦,Riöù¹û    No slot size for availableWidth=,T©¨`  lL`2   q:¤a
       ¡¦C !¦G ¡¦Ya333333ó?0b6$Rg¶¦'   google_resizing_allowed 	­%É&'9%'	'áàYa      à?¤a
       ¡¦C !¦C! !¦aP  Ş  –a
       ¡¦C !¦`È    !®éD•X          –¿-ô  ˜>ÿ©¿¾-ó  ¾bôóÅ˜*})¿ ÿÿÃ «¾½óúñióò3ôôúôŒ¿;ô	Å!
Ä-ùÃ¿-ô—¿¾-ó	kôš!
Â‹ƒÂ!
À ÿÿ ¿½¼ÿ©»÷ó`ôóÁ˜!
¾-ó¿_ôóõöÀ ÿÿ ¿½¼ÿ©»÷ó`ôóÁ˜!
¾-ó¿_ôóõöÀ-÷Â˜¿-÷!kô#™Šq $õ÷÷-ø%¿øóùòúñ÷ğ\ôó'Ä¾ú:ó)mù*™d¿ùnô+šúùù},)¿ ÿÿÃ «¾½!
»-ğ-¼^ñğù/¼óióò13ô3úmù5šf‹3ô6ôúú‹}8)¿«3ô9ôúôúª  (SbÄà®         I`şÿÿÿDa. Z2 ‘<4˜k;       0	€h P Ñ 
€  €"·  1Î€ "L ¹e      KĞ {  è5       ¡¦ !¦­;ReÎø   google_ad_layout	ReBúdR   #hffwroe2etoq   ,¤a       ¡¦C !¦HMCY<C%‘,—a       ¡¦C !¦CC‘CéD‘x   8       „ ó%%%%%%½kò š=½¼!º-ï» °¹ óÿ  ¸·bíì¸_ğïîí;ñpò
š‹‹ óÿ•  ½bòÅÄÃ è½ óÿ  ¼»bñğnòÂ˜9 óÿ  ½¼»còñğÅ óÿ¡  ½¼»còñğÁTšúÄö˜úÃz%½¼ óÿ  »ºbğï6òñ¼ óÿ³  »bğ6òñòö-ö½ óÿŸ  »¹¸÷ïøì`ğï»^òöğ½-ò!Uš"-ö½ óÿ   »º-ï!ºbğï#»^òöğ%z'%½¼	 6òñ(ò÷ù—ø˜/ óÿ¢  ½¼»còñğ*Ä-÷,½ óÿ   »bğù.»^ò÷ğ0 óÿÈ  ½ óÿÇ  ¼bñö2¼ óÿÇ  »bğ÷4»còñğ6%— óÿĞ  «
½¼8ñ8¼òiòñ9¨ óÿÕ  ½ ø ¼»còñğ;½-ò=À-ò?¿½¼-ñAlòCšW½-òD˜N½¼-ñD¼-ñFkòHš:}I)½ óÿÏ  «¼»º¹·õí`ñğJ3òL3òNú3òPò‹!}R)½õ3òSô3òU3òWú3òYòª‘<L˜q[      P 	€€H L€€€€€€ù @ @ @ €
 P qÀ 00€    ¹e     KĞ y  ƒ4      ­<Å<,T„¨`Ä   0L`   f            *   $   0   RcÒù{   bad maskéD{           Åkú šª  ùš+   ÀÅpùšÿÿÿ?ÅoùšùJ Ålùšú¡‹ªªªªªªª!Å	Äbúù¨  (SbÄà®        `şÿÿÿ•aŒ3 î4 ˜c	       1F€   ¹e     KĞ |  „4      ,TÙ
%a{š › ’–e      KĞ }  …4      ,T‘Q
a› Zœ “–e      KĞ ~  †4      ,Ta¨`z  \L`*   9<Rc,\   pedestal4RkæA4¯%   google_content_recommendation_ui_type   õaÍUa      AõRc`¯U   fluid   ­;ñ<	Re6¸T1   #hffwroe2etop   õ<Qµ#(Kh@   1   g   h  	   z      x  
   DMh          -  ÅÁúköš©Á-kö™ ½«¿²ôÀ-õ
Á¿ÿñ¿]öõ» eË ğ-ğ—$-ğ»¿ğùù÷ù/™½¾‹Š2 ÿ½¾‹½¾§¼ô—#ÿğ-õŸº]ïõ ¹eË î‹»ó™ğ©ñ§ó¡	 ‹ò©‹˜	‹ª ÌÁböú˜ªÁúlöšªÁúköšHÁ-löš*- ˜$Á- À-õ"kö$™Á- À-õ%kö'™‹ Ô«Ábö(ªÁ-*lö,š Ô«Ábö-ªª   (SbÄà®        I`şÿÿÿ“aÄ8 < ,˜i/       Ä"P P Ü€€ P ±€H D ¹e     KĞ   ‡4      ,T¨`ò  8L`   Rd.á¹Í   offsetWidth :±%U59q© áª&ue5éD	`   0       úùø÷‹Á-ù  —+-ø—ö˜ ¿¼©»ùó÷ò`ôó—	-ø—Á¿úlôš2ø	Å Ó«¿úóöòùñøğ÷ï`ôóÀ˜õ‹ Ë¿-ø½öó÷ñùğøï`ôóÀ-õ¾]óõ¾-ó¿÷òøñùğ\ôó »¿õóöòøñ`ôó¿úkôUš'-õ¾]óõ¾-ó	¿]ôó Å-ù
"¿8ú$¾2ô	%óª   (SbÄà¬ 
       á
`şÿÿÿ“a< 4> (˜h'       @ Ä  
€€€€H P p  ¹e     KĞ €  ˆ4      ,TU¨`h  (L`   HSbÄà® 
       !ª á© ¡© a©c$  ¤  $  ¤   q`şÿÿÿaL> A ”Y5&,TÙ
Ia‹Ÿ ­Ÿ Iy=–e       KĞ ‚  ä5       ¡¡ á¡M:Q:éDAX   0       „ ö%%%%À-õ — öÿ  À¿¾½ö© ¼`õôÅùÃÂlùõ™	lõ™lõ™
lõ™µ‹Ñ öÿÕ  À ÷ ¿ ¾cõôóÀ-õÃ-õ	Â÷š!ÀøkõUš öÿ›  À¿¾cõôó÷šÀ¿2õô‹À2õÀ÷2õ÷ öÿ¿  À¾cõøóª öÿÀ  À¿¾cõôóª öÿÆ  À¿¾½»úñ`õôª öÿĞ  À¿¾½»úñ`õôªª” ˜f      	 €",° @    ¹e     KĞ   ‰4      }=,T“Qa’  ¹  “‘e      KĞ ƒ  Š4      ,TÙ
IaÅ   ¡ y”‘e     @ „  İ2      ,T•Ia,¡ ¯¢ Ä”‘e     @ …  Ş2      ,Th¨`Š    L`   µ#(Rhª£õf   google_allow_expandable_ads ñíÑìRcN‘   ^true$  RéDt           l ú™lú ™"‹1!Å
Ãcúø!Åbú˜‹ªy	  Ä-ù
Å^úùªª   (SbÄã‚        a`şÿÿÿDavE ºF ”˜d       Ñ €€Y    ¹e    @ †  ß2      ,T1`  \L`*   iII%8ù4u21RdÒ<¤
   attributes  yRc‰é   data-   Rá÷ RfB¯¦y   data-matched-content,RiBÚF¬   google_content_recommendation   Rb2ib®   dataRc†›ÿË   google   !œ ¡¨ÁİéDX          -  Á¿^öô˜X-  Á¿^öô—ÅœÁ¿cöúôÄ˜ ×À¿cõôùÀ2
õœÁ¿cöúôÅ˜ ×À¿cõôúÀ2õ--ÅÄúmùš–ù/Ãy	  À-õ
Á-ø¿^öõô˜pÁ-ø½-ò ¾¼»_óòñğ"¾-ó$¿½¼_ôóòñ&¿-ô(Ày* ¾½_õôóò+Àböõ-Â-/Á^ö÷1— ×Á-ø3¿cö÷ô5ÃUš
ø4÷7øùP9ÄŠ— :ª (SbÄã‚        ™`şÿÿÿDaÔF bJ 4˜k;       @ ° ° `P @ P “ 
€€Àx¹e   @ ‡  à2      ,TÙ
Ia>¥ ¾¥ ±‘“e     @ ˆ  á2      ,T¡¨`ü  ØL`h   Y%µ#ñíu±%& á®Rcö]ŒO   _0ads_al9 a¦Á<:U5!9e5© áªE: !£e®     >  (      a=ù4	,RiF¢Oö   google_responsive_slot_preview  Q:]
`   M`   Rc¦-òù   static  u59%Ya333333Ó?-aN  .  Y5Rd†py   offsetHeightm:a:Rc&	âr   #gfwmrp 	­%Ya{®Gáz„?0â6Rbn“^.   efwr`ˆ  éDX   (        ØÀcõ - –- À-õ– ÑÀcõ•-•!
À-¿-ô¿
¾cõôóÅ ÖÀcõÄÀùmõ“ùnú“!
À-¿-ô¿
¾cõôóÃ ”À8ú¿ø8ô/õxÅù÷ú˜A •ÀcõùøÁ˜ö÷8÷ ¿ø8ô!¿8ô"2	#‹ Ğ«
À¿ù8ô%¿õiõô&¨2(÷2*ú—2	#¿2,ô÷ù-À÷w¿8ô.2õ/ …À r¿bõô1˜ ÕÀ¾cõó3‹ ËÀ¾ùôòñğ`õô5Âùø-÷7¿]ô÷9¿-ô;Àóòñ\õô= »À÷ôùóò`õô?-÷7À]õ÷AÃ2C-øEÀùnõGš&úTš!-øE2*-À-øEw¿8ôH¿2õIô-KÅ— À½©¼ôó`õôMÅú—	-O—Å èÀ ¿bôQnõS“% …À s¿bõôT•& ÀbõV—Ä-XÃ À-ùZ¿¾cõôó\—( …À w¿bõô^— dÀ¾ùôøòñ`õô`˜­-— -b— ÑÀcõd— ’Àcõf˜‹„˜A¢ÀcõhÄ—2j‹e&Àz l%¿-ù!m¾cõôóo—2j‹F-"qŠ? s –À#¼ôóúòğ`õôt™2j‹-$vlxš‹˜ ÕÀ¾cõóy‹‹ ÑÀcõ{Å˜ ÒÀúôóòñğ`õô}Œ3 ’Àcõ˜k¢Àcõ˜&-À-ƒ2õ…-À-‡2õ‰ ‘Àcõ‹-O—-K¿2*ô-'—-(¿2'‘ô  2)“ Ù¿bô•2*—‹ ‘À-¿cõô™-›˜+À-›¿-ô,kõŸ™À-* kõ¢š(-À-.£kõ¥š ÒÀ
¿»óòñ`õô¦‹w/À!0¨¾-ó1ª¿]ôó¬nõ®šbÀ-* lõ¯šV —À-K—!
¿-¾-ó°¾
½côóò²—-O¿óòñ`õô´™22¶2j‹
¿22¶ôª  (SbÄã‚        Ù`şÿÿÿDa–K `Y ˆ˜„¸       P ` @ ÄX @ˆ @ ° , €   
€ `"p  "P @ @ ° ³ 
àP ‘ € 
`€€
``
 €X ±€H @ 1@ ,  ¹e    @ ‰  â2      ,TÙ
IaÂ¬ v­ é—‘e     MQ Š  ã2      €D,T’Ia‚­ ½­ İ—‘e     @   ä2      ,T’IaÈ­ ® ­	—‘e     @   å2      ,T’Ia® ›® å—‘e     @   æ2      ,T’Ia¬® Ô® ‘e     MQ   ç2      D,Td¨`ˆ   L`   8SbÄã‚        !© a©a$   ¤ÿ 	`şÿÿÿDa¾] N^  !«,TÙ
Iaü® %¯ Ia>¹e       KĞ “  5      éDs           „ ú% úÿ"  Ä úÿä  Ãbùø %Ä-ù˜ úÿÕ  Ä šÃ Âcùø÷ª”˜b      @ •e    @ ’  è2      e>,T1‘`   `La,   D  8SbÄã¶        a© á©a¤ÿ ¤ÿ }`şÿÿÿDat^ šb  !« ¡ªm±ç¡å á«rRdnG¼i   scrollWidth Rd§2m   scrollHeightå& a«IYRb¦Àó   URL  áª,TÙ
IaØ° J± I}>¹e       KĞ •  Ÿ5      at  <  SbJ €        Õ`¤ÿ Kd       ú  Ó     Dh      úÿÿÿ¯ú „÷şöõhöÅÿö ÷ÿ"  À ÷ÿä  ¿bõô %À-õ–¿-ô¿-ô¿-ôÀ¾^õôó
•¿-ô¿-ô¿-ôÀ¾^õôó¿-ô¿-ô¿-ô	À]õôÄ ÷ÿß  À¿bõô¿úõhõ°úú ±úúÀhú¿ô™õ©÷ï À÷ğ ¿÷ò ¾÷ó ½iòú ½cóò¾÷í ½÷ì ¼÷î »iğú  » ÷ÿ  º¹-î¹bïî"º-ï
$ºcñğï&¼ ÷ÿ  »º-ïºbğï(»-ğ*»còñğ,½côóò.¿÷í ¾÷ì ½÷î ¼iñú 0¼ ÷ÿ  »º-ïºbğï2»còñğ4½ ÷ÿ  ¼»-ğ»bñğ6¼-ñ8¼cóòñ:¾cõôó<÷ö Àiõú >% ÷ÿ…  À ÷ÿi  ¿bõô@˜N÷a À¾÷G ½¼-ñB¼bòñD½¼ô`õôF÷a À¿¾÷G ½¼-ñB¼bòñH½¼`õôJ‹n÷a À¾÷G ½¼-ñ¼ôŸ-ñL¼Ÿ-ñN‹¼bòñP½¼`õôR÷a À¿¾÷G ½¼-ñ¼Ÿ-ñT¼Ÿ-ñV‹¼bòñX½¼`õôZ ÷ÿ  À¿-ô¿bõô\ÃÀølõ^Uš÷ñ À÷ê ¾bóø_¾cõóa ÷ÿ  À¿-ôc¿÷ù ¾ ÷ÿŞ  ½¼còñùe½cóòg¾cõôói ÷ÿ  À¿-ôc¿¾-ó¾ ½`õôk¿úõhõªÀƒõÁ§öõ¾úôhôªX˜tm      P @ @ P @ @ @ @ @ @ @ @ P @ @ 	€€€     ‘e    MQ ”  é2      >,T9¨`,  <La      !«y ¡ªRb(øì   lgdp±ç½¡å á«r áªSbJ €        ’`¤ÿ Kd               D%H       úÿÿÿ¯ú ş÷öh÷Åÿ÷- ˜à-˜Ú-À-õÀ-õÁ!
¿bô¿^öõô—»-À-õÀ-õÁ!
¿bô¿^öõô-À-õ	À-õ
Á]öõÄ ßÁbö Àúöhö°úú ±úúÁhúÀõ™ö©-"Ã ŞÁcöù$éÁiöú &ÄzÁ¿ùõó`öõ(`Á¿õó`öõ*kÁ	¿û¾õò`öõ, Ácöø.ÀúöhöªÁƒöÂ§÷ö¿úõhõª  (SbÄã¶        `şÿÿÿDa¼b Hd ‘,“i0       P ` P @ P P @ @ ¹e    MQ –  ê2      ,TÙ
Ia5² Å² 1	‘“e     MQ —  ë2      ,Tt¨`¨   L`
   U á« áª,T|–`¸   L`
    ¡ª$L¤a"      CRbvö‡   ssp C	FRb~Í#   wpc FRbFßuš   cu  Fm]
`    ]­>•`    ]Rb
­ß   psi F’å>éDy(           "Ã „Âbø÷ Ã-ø Ä gÂ-÷ Â gÁ-öÁ_ùø÷öÅ}
Äú3ùÃúmøš£ÃaøÃÂú;÷mø3ùùª(SbÄà®         I`şÿÿÿDaLf @g ˜e       P 0  #   ¹e      KĞ ™  —5       ¡ªDw(          ½Äaù ú—! 2úúŸ‹ ÄdÃdÂÁùiùø2ú ğÄ îÃaø
Ã!Â Á`ùø2ª(SbÄá        `şÿÿÿ”a¾e Dg ‘’d      ° ,@ ,  “e    ÃÕ˜  ‹4      Õ>,T,¨`   L`    ¡ª—De          -  Å-úª  (SbÄá         Qc      get i   `şÿÿÿDaNg xg ‘’b         “e    ÇÑ š  Œ4      ,TÙ
IaÁ³ Õ³ Qc      get j   ‘“e      ÇÑ ›  4      ,T—IaÚ³ ê³ Qc      set j   ‘“e     ÉÑ œ  4      ,T—Iaï³ (´ Qc      get m   ‘“e      ÇÑ   4      ,T—Ia5´ Qµ ‘“e      @   ì2      €D,T@–`>   L`    Rf¼ ˜   HTMLScriptElement   RIéDj          !  qš-Å-Ã^úø˜‹ª (SbÄã‚        Õ`şÿÿÿDaºj 8k ˜c	       Ô
€   ¹e    @ ¡  í2      ,TÔ¨`h  ,L`   YReÚ   currentScript   Rcæ”¹è   scripts aÍUa      (Kh@   2   o   ¨  	   ‚   •   ¸  
   DÀh          ¹Á-ö  Å-ú˜ æÁ-úÀcöõª-ú¿²ô
À-õÁ¿ÿñ¿]öõ» eË ğ-ğ—+-ğ»¿ğùù÷» æºcïùlğš	½¾‹Š9 ÿ½¾‹½¾§¼ô—#ÿğ-õŸº]ïõ ¹eË î‹»ó™ğ©ñ§ó¡ ‹ò©òªª(SbÄã‚        `şÿÿÿDaNk Ll  ˜f       @ @ P /  ¹e    @ ¢  î2      ,T°¨`  (L`   8SbÄã‚        a© !©a¤   $   u	`şÿÿÿDadl ®m –í,T(‘`   ]éDd          Ålú ª(SdÄà®             şÿÿÿ’a’l ¤l •?—b          e     K ¤  e5      ,TÙ
’aX¶ j¶ `    •e      K ¥  f5      ,T–’ap¶ €¶ `    •e      K ¦  g5      ,T–’a†¶ ¶ `    •e       K §  h5      ,T–’a–¶ ·¶ `    •e       K ¨  i5      ,T4‘`"   ]”Dg          ÿuÅÄÃcúùø ª   (SdÄà®             şÿÿÿ’a’m ¨m •—b          e     K ©  j5      ”Dœ(          „ ú%%} )Äsø})Â7sö 7÷ösö7÷ösö7÷ö=sö7÷ö?sö7÷ö
÷7ùø sø~7ùø sø})Âsö7÷ö÷7ùø ùª   •—e      sJ H H0%@  e    @ £  ï2      ™?©?±?¹?Á?É?,T<¨`4   L`   RJ™<f   ggeac   éDi          ‹Å-ú  —
~Ã2ú øª  (SbÄã€        `şÿÿÿDaÆm n ˜b       ,  ¹e     @ ª  ğ2      ,TŒ•`Ò   L`   áøùyKd       a        D}@          ÿö-  À¾^õóÄÃÂ™4-ùmøš+÷ø/ù/÷	 šø/ù/÷À]õ÷øPÃŠ3 úúVÀlõšúª‹§öª   (SbÄã‚        ñ`şÿÿÿDa$n po ‘e       P   7  ’e    @ «  ñ2      ,T4¨`"   ]Kd          i       Dg          ÿúÄbù Uª§ª   (SdÄà®            şÿÿÿ=
a–o Üo ‘b          ’e     K ¬  4      ,TÙ
—aó· Œ¸ `    ’e      K ­  ‘4      ,T—a’¸ ©¸ `    ‘’e       K ®  ’4      ,TP–`Z   L`   Y ¡ª½éDn           &Å "Ã ;« Âbø÷ Ã-øÄ]ùøÄ!ÃbøÃcúùø
ª   (SdÄà®            şÿÿÿ—a\q q ‘˜c       @ @ ¹e     K ¯  “4      ,T—aÎ¸ ÿ¸ `    ‘–e      K °  ”4      ,T—a¹ 9¹ `    ‘–e      K ±  •4      ,T4¨`&   L`   Y”Dg           —ÅÃ-ø  Ãcúøª (SdÄà®            şÿÿÿ=
a~r ¤r •b         –e     K ²  –4      ,T4—`&   L`   ”Dg           ˜ÅÃ-ø  Ãcúøª (SdÄà®            şÿÿÿ‘a°r Ör ’•b         –e    K ³  —4      ,TÙ
‘av¹ ~¹ `    ’–e       K ´  ˜4      ,T“‘aƒ¹ ³¹ `    ’–e      K µ  ™4      ,T0—`   L`   U	 ÂéDf          !  Å-úÅ-úª  (SdÄà®             şÿÿÿ‘a€s °s ’˜b       P ¹e      K ¶  š4      ,T“‘aİ¹ º `    ’e       K ·  ›4      ,T8¨`.   L`   É–Dh           êÅ Ãcúø ‹ª (SdÄà®            şÿÿÿ=
a6t –t —b          e     K ¸  œ4      ,T‘`Ø	  `L`,   5áÛaÍUy•©	‰uD¤a      “C”C•CRbvgçC   xb  C-C©C á¤CÍ@‘’ á¤¡åE#b@  ¶  ö  ˆK„   0   @  @
     d   ¶   ğ     Î   ã   (      X  m  x     –  ¥  h%     å  T  à"     l    $     ½  Ò   &     Dù(         !  èièú Å/ç-çè]èçæ²æ
ç-çèæÿãæ]èçâ eË â-â—ü-ââæâ÷²÷à-àáßÿÜß—%ß]áà Û eË Û-Û—-ÛÛßÛ‹¸ß—%ß]áà"Û eË Û-Û—-ÛÛßÛ‹·ÿİŞ‹İŞ§Üß—&ÿÚ-à$ŸÙ]Ùà& ØeË Ø‹ÚŞ™Ú©Ü§Şš
åİä‹Píùìø-ø)E(/ø+â-â-¶-â/ê-â	1é-ú
3âé:ê68ë5ß_âúíß7 Š	  9 ÿäå‹äå§ãæ—&ÿâ-ç:Ÿá]áç< àeË à‹âå™â©ã§åšä©²>@ç-çBèæÿãæ]èçDâ eË â-âF•-âHâæâõõóiâ Tàßõá`âáJà²àLNá-áPâàÿİà]âáRÜ eË Ü-ÜT•-ÜVÜàÜööñÜiÛ SÙØöÚ`ÛÚXÛ-ÛZlÜ\’qÜFÛXÚØcÚñØ]ÚbÛÚ_ÛbÜÛavÜÚcÜñÚc˜˜vÜÚcÜñÚeTšˆ-úgÜvÚØcÚñØiÚ^ÜúÚkŸ‹qÜFÛXÚØcÚñØnÚbÛÚpÛbÜÛrÜiÛ SÙØñÚ`ÛÚtÛ-Ûv:ÜmÄ-ú
3ÜvÚØcÚñØxÚù8zÙ_ÜúÚÙ{|}ÄÃiÛ SÙØñÚ`ÛÚ~Û-Û€mø‚š¨}ƒ)Ü3Ü„qÛFÚXÙ×cÙñ×†ÙbÚÙˆÚbÛÚŠ3ÜŒiÛ SÙØñÚ`ÛÚÛ-Û3Ü	’ø3Ü”vÛÙcÛóÙ–3Ü˜ñ3ÜšiÛ SÙØñÚ`ÛÚœÛø/Û3Ü Üğ-ù¢Ü^Üùğ¤øP¦ÃŠÃ§ íÜ/¨ÛvÚ
ØcÚñØªÚùÙ`ÜÛ¬—f íÜ/®ÛvÚØcÚñØ°ÚùÙ`ÜÛ²—@ íÜ/´ÛiÙ S×ÖñØ`ÙØ¶Ù/Ù¸Ù-ÙºÚ]ÚÙ¼ÚùÙ`ÜÛ¾ Šh À ÿŞß‹Şß§İà—&ÿÜ-áÁŸÛ]ÛáÃ ÚeË Ú‹Üß™Ü©İ§ßš
åŞä‹ Š  Å ÿäå‹äå§ãæ—&ÿâ-çÆŸá]áçÈ àeË à‹âå™â©ã§åšä©ª(SbÄã‚        -`şÿÿÿDa²t ²x ”˜Ê       € P @ P P @   
 € €[ @ P P @ @ D€    @ P @&'@ 0@ 0À 0À  À  /@ @ € @ · .  ¹e   @ ¹  ò2      ,T\¨`t   L`   ™‰u¡åéDq            —ª-  Å^ú—-Å|Â_ú÷-	Ä^ùÄ-ùÅødúùª  (SbÄã‚        h`şÿÿÿDaÈx \y ˜d       P “€   ¹e   @ º  ó2      ,T@¨`>   L`   0SbÄã€        !©`€ÿ `şÿÿÿDaty Æy —,TD’`F   L`   RbÆà‡    +   !©•Dk          ÿ¥ÅwÄ 8ùÄ«w8ùÄbúù Hè  ª (SbÄà®        I`şÿÿÿDa”y Äy ı@b         ‘e    KĞ ¼  “5      •Dj          „ ú%ú£ Äaù ‹% ª ”b         ‘e     @ »  ô2      A,T`’`|   L`    ¡ª±ç¡åéDr           -  Å/ú—-  Å|Ã4úø-  Å/ú-	Å^ú—-Å^úª  (SbÄã‚        %`şÿÿÿDaz ‚z ˜d        0    ¹e    @ ½  õ2      ,Tí¨`˜  °L`T   @SbÄã‚        a© !© áªb¤   $   ‚ÿ ™`şÿÿÿDaœz è  a« !¤u±ç a¢]
`    Mc             aÍ¡áÛU¡è,T˜¨`î   L`
   - áª á¤E#©éD„(          -  ÅlúšlÅ-úÅ-Ã-øÄ]ùø	/úxšPÿãÅÿhÄ-ÃÿÜÂÁ`ùøÄ«Ãcúùø˜'ÿãÅÿhÄ-ÃÿÜÂÁ`ùøÄÃcúùøª (SbÄà®        I`şÿÿÿDa„| } =A˜e       Ä
    €  ¹e    KĞ ¿  •5      y¡å á¤Rbf=Å/   wa  Å—-•©	Í@ !«E#l¢    r    è  0  ö  ¦  
  	  À  8  f
  ]
`    La       ÈKÄ€  Ö     (1  !   *  Ì   .  '   ^  ²  Ğ  .   Ê  ß    0   <  v  ğ  -     £  (  .   …  1  È)  -   I  ^   +  .   ä  ù  Ø/  (   5  J  `2  "   s  N  °:  !   f  ƒ  (<  "   DÙ˜  (       „ ß%%%| Å	ŞlŞUÄ˜,Ş-ŞŞ/Ş˜Ä‹Ş-ŞŞİ4ŞİÄù˜0 ßÿ  Şİ-İİÜ|
ÙØÛúÚ`Şİúª ßÿï  «İ-İŞÜ^ŞİÜÄ|Ã ßÿ"  Ş ßÿ#  İbŞİŞ-Ş%|Âz%Ü²Üİ-İŞÜÿÙÜ]ŞİØ eË Ø-Ø •(-Ø	"ØÜØğğë×-×
$×ğ/×&×-×(Ø]Ø×*Ö²Ö,.×-×0ØÖÿÓÖ]Ø×2Ò eË Ò-Ò4•%-Ò	6ÒÖÒñ²ñ8:Ğ-Ğ<ÑÏÿÌÏ—%Ï]ÑĞBË eË Ë-Ë@—-Ë	>ËÏË‹èÏ—%Ï]ÑĞDË eË Ë-Ë@—-Ë	>ËÏË‹çÿÍÎ‹ÍÎ§ÌÏ—&ÿÊ-ĞFŸÉ]ÉĞH ÈeË È‹ÊÎ™Ê©Ì§Îš
ÕÍÔŒ&èöçõ ßÿ   ÒiÒú Jæ-çLÒ Ğ^ÒçĞN¿-ôP˜™²ôRTÑ-ÑVÒĞÿÍĞ]ÒÑXÌ eË Ì-ÌZ—&-Ì	\ÌĞÌóóà-÷^Ì-ó`Ê^Ì÷ÊbŠ6dÿÎÏ‹ÎÏ§ÍĞ—&ÿÌ-ÑeŸË]ËÑg ÊeË Ê‹ÌÏ™Ì©Í§Ïš
ÕÎÔŒ'Œ$Ò-Òi•"ÒëlÒkš2/l¿ßb ÒĞ ßÿ  ÏßA ÎbÎönÎæÑ`ÒÑp‹/r¿ôôÒŸ!tÑbÑövÑbÒÑx‹Ÿ‹JÒëlÒzš)Òßv Ñ/õ{Ğ-Ğ}ĞÏcÑĞÏlÒš‹/‚Ò!tĞbĞö„Ğ^ÒĞ†Á#²õˆŠÑ-ÑŒÒĞÿÍĞ]ÒÑÌ eË Ì-Ì•!-Ì	’ÌĞÌòòä-Ì”ÌlÌ–“ -ä˜9ö—À-äš¿-äœã-äâÌõoÌ “ã:ô¡mõ¢“ã<õ£ÌâlÌ¤“ßã Ìßh Ë-ä¥ÊßÜ ÉÈ`ËÊ§ËÊcÌËÊ©–ßã Ìßh Ë-ä«ÊßÜ ÉÈ`ËÊ­ËÊcÌËÊ¯˜İßv Ì-ä¥ËÊcÌËÊ±ÀÌõlÌ³Uš«õUš¥Ì-Ì´Ì!tËbËõ¶/Ì¸¿_-ä«Ê-ÊºË]ËÊ¼lô¾™J ßÿ   ÌË-ËËÊ-Ê´Ê!tÉbÉõ¿/ÊÁÊ-ä«È-ÈÃÉ]ÉÈÅÉõÈ`ÌËÇ‹/Ì-Ì´Ì!tËbËõÉË-ä«É-ÉËÊ]ÊÉÍÊ4ÌËÏÊ-÷^Ì-ä«Ê^Ì÷ÊÑ Š¥ Ó ÿÎÏ‹ÎÏ§ÍĞ—&ÿÌ-ÑÔŸË]ËÑÖ ÊeË Ê‹ÌÏ™Ì©Í§Ïš
ÕÎÔ‹aÒßd Ñ ßÿ  ÏcÑæÏØlÒÚUš.ßa ÒĞßD ÏbÏöÛÏÎæÑ`ÒÑİ-øßÒ^Òøæá Š› ã ÿÔÕ‹ÔÕ§ÓÖ—&ÿÒ-×äŸÑ]Ñ×æ ĞeË Ğ‹ÒÕ™Ò©Ó§Õš
ÛÔÚ‹ Š@  è ÿÚÛ‹ÚÛ§ÙÜ—&ÿØ-İéŸ×]×İë ÖeË Ö‹ØÛ™Ø©Ù§ÛšÚ©²÷íïİ-İñŞÜÿÙÜ]ŞİóØ eË Ø-Øõ—Ç-Ø	÷ØÜØïïí-íùØ]Øíû-úıØ^Øúÿ ßÿğ  Ø×ùÖ˜‹Õ `Øÿ×ÿ  ßÿ1  Øßi ×ßæ ÕÔíÖ `×ÿÖÿ ×ù˜ ßÿ3  Ö aÖÿ‹ z) %ÖÕ 6ÖÿÕÿÖÖÕ-ÕÕÔ `Øÿ×ÿ 
 ŠÔ   ÿÚÛ‹ÚÛ§ÙÜ—.ÿØ -İÿ Ÿ!× ]×ÿİÿ ÖeË Ö‹ØÛ™Ø©Ù§ÛšÚ© ßÿ  Şİ-İİÜØÛúÚøÙ `Şÿİÿ úª=AÄ˜À     3€ 0	  &0'P P  P @ P @ P P P P @ P p€€"@ @ D
€€ˆ P @ P P !Æ€€€ "€ 	 € 
€ à@ D  .p€€  
 €  € &p€  ¹e   @ ¾  ö2      IA,TÙ
IaÿÀ KÁ 5—e     @ À  ÷2      €D,T¨¨`   L`   0SbÄã‚        !©`$   e`şÿÿÿDa°‚ Øƒ “,T‘IabÁ pÁ I…A—e      KĞ Ä  t5      ,T<”`6   ]éDi(            ÿÿñ ÅÄø÷ö`úù ª (SbÄà®        I`şÿÿÿDağ‚ ƒ –˜b          ¹e    KĞ Å  u5      ,TL”`V   L`    ¡ª±â—Dm          Ä-ù  Ä/ù—|Ä-ùÅÃ-ø  Ã/øÃ^úùø	ª (SbÄà®        I`şÿÿÿDa(ƒ dƒ –c        0  ‘e     KĞ Æ  v5      ,TÙ
Ia»Á ÊÁ I–‘e      KĞ Ç  w5      ,T<”`2   ]—Di            ÿÿğ ÅÄø÷`úù ª   (SbÄà®        I`şÿÿÿDa¦ƒ Ğƒ –b          ‘e     KĞ È  x5      —D”(          „ ú% úÿ5  ÄÃ Âö`ùø  úÿ5  ÄÃÂö`ùø úÿ5  ÄÃÂö`ùø úÿ5  ÄÃÂö`ùø úÿ5  ÄÃÂö`ùøª–c
      @   ‘e    @ Ã  ø2      ‰A‘A¡AµA½A,T¹¨`*  HL`    ]A-¡ !¤ a«í ¡ª áª !«áø á›aÍ½UHKp€      Ò   À     æ   û   è     !  S  È
     g  |  ğ     D¥    (       úùø~‹º-ï  Â-ï|Áú2ø2~2	÷2 })¹ùsí|7îí sí|7îí î2~2~2ÉºaïÅ˜¥-ú	º
¸^ïúí—| Å²ú!#¹-î%º¸ÿê¸]ïî'é eË é-é)—/-é+é¸éõõğ!-ébéõ/Å˜-1éç4éú3çŠ> 5ÿ¶·‹¶·§êí—&ÿé-î6Ÿè]èî8 çeË ç‹éì™é©ê§ìšë©²ö:<¹-î>º¸ÿê¸]ïî@é eË é-éB—-éDé¸éôôò-Fé4éôHŠ. Jÿ¶·‹¶·§êí—&ÿé-îKŸè]èîM çeË ç‹éì™é©ê§ìšë©ª   (SbÄáŒ        í`şÿÿÿ’a„ ¼… D˜oO       0``N	&,° P Ì  
    .P @ P Ü    ¹e    ÃÕÉ  4      ,T<¨`6   L`   ,TÙ
IaıÂ Ã I“•e       KĞ Ë  5       ¡ªéDi(           6ÄÃ  Á÷`ùø 2ª (SbÄã‚        •`şÿÿÿDaÖ… 
† “”b      ,  •e    @ Ê  ù2      õA,T,–`   L`   ,T— ¡ªa#Ã )Ã I“•e       KĞ Í  Œ5       ¡ªDe             2 ª  (SbÄá         Õ`şÿÿÿDa2† T† ˜b         ¹e     ÃÕÌ  4      B,T@¨`:   L`   “ ¡ªéDj           "Ä ö« Ãbùø Ä-ùÅ^úùª   (SbÄã‚        Á
`şÿÿÿDal† Š† ”•b       @ –e    @ Î  ú2      ,T•`æ  `L`,    a¢Rc`ø‘   config  )M !¤ ¡ª áª !«¡í5Áİ‘ ,TÙ
IaÄ !Ä I¹e       KĞ Ğ  k5      é,T’Ia1Ä >Ä I“”e      KĞ Ñ  l5      ,T’IaIÄ OÄ I“”e       KĞ Ò  m5      ,T’Ia_Ä lÄ I“”e      KĞ Ó  n5      ,T’Ia|Ä ‚Ä I“”e       KĞ Ô  o5      ]
`    La       MéD€          Å-  Å-Ä-Ã-ó
 é½aòÂ-ó
Á-ó† !½h» V¹¸úï`ğï»Ÿ-ğ¼]ñğ‹Ÿ‹¼hº V¸·úî`ïîºŸ-ï»]ğï‹Ÿ‹»h¹ V·¶úí`îí¹Ÿ-îº]ïî‹Ÿ‹ºòiòñ À-ó	"Z ì½}
$)¼sğ!%ºiïú '7ñğ )sğ!%ºiïú +7ñğ -sğ!%ºiïú /7ñğ 1i» U¹¸úï`ğï3»còñğ5¿-÷7¾¼^ó÷ñ9˜§ 6½¼ º÷ğ`òñ;¾i¼ Uº¹úğ`ñğ=¼-ñ?½»^òñğA½bóòC 6½¼º÷ğ`òñE¾i¼æº¹úğ`ñğG¼-ñI½»^òñğK½cóòöMù˜ 6½¼º÷ğ`òñO¾bóùQ ù¾cóö÷S‹ª ó¾ ô½òôñöğõïøîiòñU½cóò÷W 7¾bó÷Y 8¾bó÷[ 9¾bó÷] ù¾cóö÷_ 1¾i½æ»ºúñ`òña½zc%¼»ö6ñğdº¹õğ`óòf $—øT™-øhTT %$ ÷¾ ë½bóòjù˜ ÷¾bóùlª (SbÄã€        	`şÿÿÿDa¢† Š “X˜tn      P P P P P P Ó $€ 	@  €€€   € € € €€ù @    ”e    @ Ï  û2      EBMBUB]BeB,T”¨`è   L`   YÕ !¬éD0          ú éÃaø ‹Ä :Ã "Â ;« Áb÷öÂùöúõ`ø÷ úÃcøùú õÃ "Â ö«Áb÷öÂcø÷ù
 "Â „Áb÷öÂ-÷Ã]ø÷ª(SbÄã€        Ñ
`şÿÿÿDa"Š Š ˜d       @ @ @ ¹e    @ Õ  ü2      ,Tp—`œ   4L`   8SbÄã‚        a© !©a¤   $   !`şÿÿÿDa¦Š Œ ”,TL¨`R   L`   ,TÙ
IayÅ Å IB–e       KĞ Ø  5      éDm(           ÿÿ6 ÄÃÂ  Á`ùø ÅÂùø`úùª   (SbÄà®        I`şÿÿÿQb      c.i aÖŠ ‹ ’•b        –e    KĞ ×  ‚5       !«,TL`R   L`   ,T‘IašÅ ŸÅ I’–e       KĞ Ú  ˜5      “Dm(           ÿÿ6 ÄÃÂ  Á`ùø ÅÂùø`úùª   (SbÄà®        I`şÿÿÿQb      c.g a‹ N‹ ’•b        –e     KĞ Ù  ƒ5       ¡ª,TL`R   L`   ,T‘IaºÅ ÀÅ I’–e       KĞ Ü  ¹5      “Dm(           ÿÿ6 ÄÃÂ  Á`ùø ÅÂùø`úùª   (SbÄà®        I`şÿÿÿQb      c.j aX‹ ‹ ’•b        –e     KĞ Û  „5       a«,TL`R   L`   ,T‘IaÛÅ áÅ I’–e       KĞ Ş  £5      “Dm(           ÿÿ6 ÄÃÂ  Á`ùø ÅÂùø`úùª   (SbÄà®        I`şÿÿÿQb      c.h aš‹ Ò‹ ’•b        –e     KĞ İ  …5       áª,TD`F   L`   ,T‘IaûÅ Æ I’–e       KĞ à  5      “Dk(           ÿÿ6 ÄÃÂ  Á`ùø ÅÄbúùª (SbÄà®         I`şÿÿÿQb      c.m aÜ‹ Œ ’•b        –e      KĞ ß  †5       !¬“Dv           „ ù%% ùÿ"  Ã ùÿ„  Âbø÷ Å 2ú2ú2ú2ú	2ú

ª  ’•c      ,° ,° –e    @ Ö  ı2      ¡B­BÁBÍBáBíBCC!C-C,T¤¨`   L`   í	Y a«) a§éD8          }  )Åsù îÃ°ÁböÁ-öÂ]÷öÂbø÷7úù 	ú "Ä ;«ÃbùøÄ-ùÅ_úù @«Ä-ùÅ =Ã âÂ "Á äÀböõÁõô`÷öÂ_úùø÷ª  (SbÄã‚        @`şÿÿÿDa&Œ ¼Œ  ˜f       “€€ 	  
€     ¹e    @ á  ş2      ,TY¨`n  dL`.   Rb&›ĞĞ   plle4¤a      C a¢CABC)CM`   ’ a¢‘a
      ]ACMC(Rhv­
   google_disable_experiments  –—“”½Y !«aÍ áªU	ReF$é—   google_mc_lab   ,Ri
 Á5   google_auto_storify_swipeable   ,Ri>™‚Ç   google_auto_storify_scrollable  `  (Kh@   ğ   0  °	  
   C  V  À
     DEp            \Àbõ Å-ú ˜ ùÀ¿ é¾bó¾cõôóŒ2ú hÀ W¾½ô`õô
ÅrÀ	¾cõóÄ øÀ}¿ú3ô è¾có3ô})¾ù˜-x3óù3óó3ô é¾bó3ô	 bõô"uÀ¾cõó$˜'!
&Àbõ( "¿ ;«¾bôó*¿-ô,À^õô._À¾E½ô`õô0¾²ó24¿-ô6À¾ÿğ¾]õô8º eË ï-ï:—.-ï<º¾ïøøö "¹ ;«¸bîí>¹-î@º^ïîøBŠ< Dÿ¼½‹¼½§»ó—#ÿï-ôEŸ¹]îôG ¸eË í‹ºò™ï©ğ§òšñ© ûÀ¿cõôI ûÀ
¿cõôK ÀbõM— À-O¿¾cõôóQ˜$ "¿ ;«¾bôóS¿-ôUÀ§ª¾^õôóW À-O¿¾cõôóY˜$ "¿ ;«¾bôó[¿-ô]Às1«¾^õôó_ À-O¿¾cõôóa˜$ "¿ ;«¾bôóc¿-ôeÀr1«¾^õôógª (SbÄà®        	`şÿÿÿ—aÎŒ ` T˜si       @ ,@ L€Y 0À 0@ @ @ @ P P Ü€     €€€ 
€   ¹e     KĞ â  Ÿ4      ,TH¨`J   L`   0SbÄã‚        !©`$   i`şÿÿÿDax j‘  ¡¦õ
,TÙ
IaEÈ ³È I™C’e      KĞ ä  d5      éDl           „ ú% úÿÒ  «Ã-ø Ä Â^ùø÷ª   —‘b        ’e    @ ã  ÿ2      C,TÈ“`J  ,L`   0SbÄã‚        !©`$   `şÿÿÿDa‚‘ ´’  ¡¦b/²SbJ €         ¡©`¤ÿ µC á¢Å,TÙ
Ia5É JÉ I¹C¹e      KĞ æ  c5      Kd       L   a     D´P          „ ù% ùÿÒ  «Åÿøù} ÂÁùÚ Àc÷öõ  ùÿX  Â!À-õÁ¿^öõôÁ÷i÷öªÂƒ÷Ã§ø÷-ú
Á F¿¾!qóúõš‹!¾!½¼bòñ½bóò¾½ ¼\öõ÷ ùÿX  Ãiøú ª   ” ˜f      P P T€     ‘e    @ å   3      ½C,Tt¨`¤   L`    ¡ª a® áª a¡éDw           -  ˜-  ª-˜ -Å-Ã^úø˜-Ä2 ù‹ªÄ-Ã-
Âcùø÷Ä2 ù-  Ÿ‹ª  (SbÄã‚        y`şÿÿÿDaÌ’ f“ ’d       P ,P    ‘e    @ ç  3      ,Tt”`¦    L`    a¡ a®5 !¡ a« áª•Dw8      úÿÿÿşùZùúõõùøúöô÷™®÷öi÷ú  Â­÷2 2!Âi÷ú 2
!Âi÷ú 2¹2ª (SbÄñš        `şÿÿÿ‘a¬“ :” —’d       ,° ° ,° ¹e    ÆÕè   4      ,TÙ

a(Ê ƒÊ ”—’e      KĞ é  ¡4      D,T“Qc      lp.ub   a‹Ê ‘Ê —’e      KĞ ë  ¢4      ,T“Qc      lp.vb   a•Ê ÖÊ õ—’e      KĞ ì  £4      ,T“Qc      lp.yb   aÚÊ 2Ë ’e      KĞ í  ¤4      ,T´¨`$  0L`   …+9/,T(‘`   L`   5/éDd           -   ª (SbÄà®        I`şÿÿÿDaT— –— ˜b          ¹e     KĞ ï  «5      Y !¡‰ RfJÖXµ   getDataWithCallback 
 a«)•D 8      úÿÿÿşùZùúõõùøúöô÷™®÷öi÷ú  Â­÷~Â-÷ øŸ‹ ô2ø   ÂÁ À÷i÷ö2-
Â-÷Â-÷ÃÁ «À_ø÷öõ-Â-÷Â-÷ÃÁ «	À_ø÷öõª  (SbÄñš         ¹`şÿÿÿ”a²– J˜  –f      ° ,P @ P    —e     ÆÕî  ¥4      -D,TÙ
Ia1Ì 1Ì `    •¹e       EÔ ğ  ¦4      ,T—ÅaMÌ èÌ ‘•e      KĞ ñ  §4      D,T—Qc      pp.ub   ağÌ öÌ •e      KĞ ó  ¨4      ,T—Qc      pp.vb   aúÌ ?Í õ•e      KĞ ô  ©4      ,T—Qc      pp.yb   aCÍ ^Í •e      KĞ õ  ª4      ,T¤¨`  0L`    áª ¡ªRV¸ú7   googlefcPresent Y !¡‰AD‘ a«áéD8      úÿÿÿşùZùúõõùøúöô÷™®÷öi÷ú  Â­÷Á2÷ ö2  ÂÁ÷i÷ö2-
Â-÷Â-÷ÃÁ «À_ø÷öõ-Â-÷Â-÷ÃÁ «	À_ø÷öõª(SbÄñš         É`şÿÿÿ“a› :œ  ˜f       ,° ,P @ P    ¹e     ÆÕö  «4      ,TD¨`B   L`   0SbÄà®        !©`$   •
`şÿÿÿaLœ 0 ”UI,Td—`ˆ    L`   b/²Ù=SbJ €         á©`¤ÿ ¥DRcî„   sc-cnf  ™:Kd          Ù      Ds(          Åÿù!  Â-÷Ã-Á^ø÷öÅ‹ÃƒøÄ§ùøªúT™Ä-úlù
U™
Äcùúª(SbÄà®        I`şÿÿÿDa|œ , ”˜d       P 	    ¹e     KĞ ø  ¨5      éDk(          „ ú%ú‡ Ä! ÃÂ Á`ùøª   ”–b        —e     KĞ ÷  ¬4      ©D,TÙ
Ia¤Î ßÎ ‰—e     @ ù  3      ,T’IaêÎ Ï ”—e     @ ú  3      ,T’IaÏ ¥Ï =”—e      @ û  3      ,T’Ia°Ï Ğ ¡”¹e     @ ü  3      ,T’Ia"Ğ ÆĞ 1”e     @ ı  3      €D,T’IaÑĞ £Ñ ”e     @    3      D,TÙ
Ia®Ñ ĞÑ Øe     @   3      D,T“IaÛÑ şÑ Õ
•e     @   	3      D,T“IaÒ tÒ i•e     @   
3      D,T“IaÒ 8Ó ¹•¹e     @   3      ,T“IaEÓ .Ô m
•‘e      @ 	  3      D,T%¨`  HL`    UReòóÁ   google_plmetrics RfRø{“   PerformanceObserver ]
`   M`   RdbÖU   layout-shift$RgögG   largest-contentful-paintRdÄ?B   first-input RcRSX   longtaskRb^¤º   Aa  ]¡åRcæ—|ª   event   aÍ¤a
      ™CRcÚzØó   bufferedG“ RfÊÜõ:   durationThreshold   RcB{a   observe U(Kh@   `   º      
   Í   à        Dp           Àiõú  Å …À ‚¿bõô˜æ! Ä-ù—Ü! À-õ˜Ò2ù
z%Ä-úÀ-õ˜-ùÀ¾^õùó²ù¿-ôÀ¾ÿğ¾]õôº eË ï-ï	—H-ï
º¾ïøøö}!)ºø3ï"ïùºølï$š(¹2ù%î ¹bîú'¹-î)º^ïîù+ŠV -ÿ¼½‹¼½§»ó—#ÿï-ô.Ÿ¹]îô0 ¸eË í‹ºò™ï©ğ§òšñ© Àbõú2ª   (SbÄã‚         q`şÿÿÿDav¨ æª 0˜j4       ` ° ³€
€€  
 €É q
€€[   ¹e     @   3      ,TÙ
Ia€Õ ÓØ ¤–e     @   3      D,T‘IaàØ “Ú –e     @   3      D,T‘Ia Ú tŞ ‰–e     @   3      ,T‘IaŞ =ß ‘
–e     @   3      ,T¨`¾  œL`J    !¡ á¦Á ¡ª a«½ï½mÑ	 a¡ áª !«= !¬5RbÖ±j°   Ia  !	ñá] a® á£A !¤Rbš‹2˜   Ja  ,TÙ
Qc      Hp.Ba   adà jà I¹e       KĞ   ¤5      %Y]$R *†q   [data-google-query-id]  i RfªQ   data-google-query-id	¤a      ]HQEéDìP          ¿2  ô2ô2ô2ô2!
Â-÷Â2÷22	!
Á-öÁ2
ö2ö2¼2ñ2ñ2ñ2 ñ2"ñ2$ñ2&ñ2(2*Á2,ö2.ö202224 26!8Ã-ø:ÄÂ^ùø÷<Åúù˜-ú >Ã!Á^øúö@‹2ù"B}#D)2$Eª (SbÄá         1`şÿÿÿa®¾ ÔÁ <˜mG      ,° ,` ,° ,° ,° ,° ,° ,° ,° P @ Ì ¹e     ÃÕ  ­4      ©E,TL¨`R    L`   •RRdeZ{	   className   ÍRc¾€G   dataset  R•£   adsbygoogleStatus   éDm           ¥« Ä-ù Å-Ã^úùø˜Å-Ä-ùlú
Uª   (SbÄã‚        y`şÿÿÿDa`Â ìÂ ˜c       @ ¹e    @   3      ,T@•`@   L`   ÙE—İEDj            -  Å2ú #Åùø÷`úùª(SbÄã‚        •`şÿÿÿDaÃ lÃ ’“b       ,@ ”e    @   3      ,TÉ¨`J  XL`(   HSbÄã‚        ¡© a© !© á©c$  ¤   $   ¤ÿ =	`şÿÿÿDa†Ã $È ’URÆi_   google_spfd (Rh†S‰I   google_reactive_ads_config   RfZ*-   page_level_pubvars  (Rh*¹ÿ   page_contains_reactive_tag  ]1Rc~to°   pltais   a¦@Rn~ûÖ2   Only one 'enable_page_level_ads' allowed per page.   RfŠ0/µ   google_pgb_reactive …,T@`<   ]éDj          ÅÃ/ø ù—
Ã/ø4úùª  (SbÄà®        I`şÿÿÿDaÌÆ üÆ 	F˜b        à ¹e     KĞ   Î5      )9%9 Rf"d½   first_tag_on_page   (RhªxL   google_reactive_tag_first   ,T<¨`8   ]•Di(           ÿÿ¯ ÅÄÃÂÁ`úù ª(SbÄà®         I`şÿÿÿDaüÇ È –—b          e      KĞ   Ï5      `F  •Dµ8           „ ù%%%! %Ã-ø—Ã ùÿÚ  Â2ø÷Ã-øÅ— ùÿÚ  ÃÂÁÀ¿`ø÷ ùÿ¨  ÃÂÁcø÷ö
 ùÿ$  ÃÂÁÀ`ø÷•ú˜™-ú—~Å ùÿ\  Ãù¹ Âbø÷Ã-ø˜H ùÿ\  Ãù¹ Âbø÷Ã-ø—/-ú˜ ùÿ_  ÃÂbø÷ª ùÿĞ  «Ã	Âøiø÷¨ ùÿ\  Ãù¹ Âbø÷Ã2ø  ùÿ_  ÃÂ-ú
"l÷$Âbø÷%Ãù¿ ÂÁb÷ö'2ø)ù¤ Ã ùÿ  Â Ácø÷ö+ÃÂ-÷-lø/UšÃÂ2ø0÷ÃÂ ùÿ\  Áù¹ Àböõ2Á-ö4—l÷62ø7 ùÿÕ  Ã ¤ ÂÁcø÷ö9ª   –4—k;      ° @ @ P @ ,P ‘ €€"P q e    @   3      )FAF,TE”`D  ¤L`N   FÕE=,Ri¶<‘    (\W|^)adsbygoogle-noablate(\W|$)RáRb¢y[\   on  RéG¼   google_adtest   å&u(Rh‚‚ã.    adsbygoogle-ablated-ad-slot½8 Rf¶”«‚   google_element_uid  mÅRbN@Š   slot	i±%y	&ui5cT  H  0  (  Eµ#YReÖf3   createComment   é5LRq*¡µ>   No ad requested because of display:none on the adsbygoogle tag  %Fy"s<Rm^9<-   Adsbygoogle tag with data-reactive-ad-format=   XRt¾ë¶J    is deprecated. Check out page-level ads at https://www.google.com/adsense  éD1H           -  Å- š!À¿bõôÀ-õÁ-¿^öõô
Ä ]ÁböÃ–-ø–Á-lö’ù• “ÁcöÄ ÁböÁ-öÂÁ÷köš‹÷;ùÄ-ø	˜-ø	˜Üù—Á-ø	pöšÍ-À
8õ2-!ö—~À2ö#õÁbö%Åú2'-)À4õ+--Á¿!/¾bóú1¾_öôó3Á-ø5lö7šo¨Á-8À¾^õó:Àböõ<Uš--Á¿¾_öôó>¨Á-8À¾^õó@ÀböõBUš--Á¿¾_öôóD-FÁ2öH-FÁÀ2öJõª¢ÁcöLÃ˜SÁ-øNlöPšGÁ-löQ™;Á-RmöT™0ú—,-UÁ-öW˜- YÁ-U¾-ó[¿!½^ôóò]¿^öô_ª-"a™Á-"alöcÁ-Rlöd™Á-Rlöeš—‹8Á-ö#f˜.À-õ#fÀ-õ$hÁ%¿!/¾-R½bóòj8ôl¿&8ôm¿^öõônª  (SbÄã‚        É
`şÿÿÿDa>È ÎÑ X˜tp       ` @ P ‘ 
 Â"p€€
À€ 
 @ @ P ,@ ÄX ±€
   €#@   ¹e    @   3      ,T„¨`È   (L`   Y‰Rb(O   INS yRcN<›í   reservedÙEİE±éD{8          !  À-õÁ¿^öõôÅÃø/úÂ-úmø
šA÷ù !Ábö÷˜&Á-ùÀ-õlö™˜-÷Álöš÷ªøPÃ/úÂŠD ª(SbÄã‚        è`şÿÿÿDaäÑ RÓ  ˜f       @ 	 €"Dˆ    ¹e    @   3      ,T˜¨`î    L`   1æ,TÙ
IaÔé ÷é I”–e      KĞ    ¬5      ySbJ €         !ª`¤ÿ ”,,TIaBê Pê IÁF–e       KĞ !  ­5      Kd    6   P   ‘     D„0           ˜s Ärù ši àÄ ÃbùøÅÄ-mùšMÄúmùšDÿù *Ã- Â]÷
Âöõ`ø÷‹ÃƒøÄ§ùø!ÂÁb÷öøúQÅŠR ª (SbÄã‚        I`şÿÿÿDalÓ °Ô ˜e      	P 1@ @ ğ  ¹e    @   3      ¹FÅF,TL¨`R   L`   FR‚¹ß   adsbygoogle ÕE$Rgv†A    adsbygoogle-noablate   éDm          ²Ä Ãbùø Å2ú-úÃ8ø2ú¬Äbùúúª   (SbÄã‚         ñ`şÿÿÿDaÆÔ ŒÕ ˜c	       ,P    ¹e     @ "  3      ,T	’`Î  8L`   @SbÄã‚        !© ¡© á©b$   ‚ÿ ‚ÿ `şÿÿÿDa¦Õ Ù —ù4,T„¨`Â   L`
   $Rg²˜Û   enable_page_level_ads    ¡©Áİ á©±ç•D{          Å-ú  Åúš«ÅÃ4úø‹EÄ-ùÅ^úù˜«ÅÃ/øÃ4ú
ø‹ «Ä-ùÅ^úù˜«ÅÃ4úøª   (SbÄà®        I`şÿÿÿDaÖ ĞÖ Gd       8P  à à ‘e     KĞ $  Ê5      GF%é5¤a
      FCù4C“ RfRJ”$   pause_ad_requests   $R Æ˜‘Ü   google_pause_ad_requests,TÙ
Iafì ‰ì IG¹e      KĞ %  Ë5      éDô8          „ ø%%%~% øÿp  ÂÁ-ö Ác÷ö%ø¤ Â øÿŒ  Á Àc÷öõø ÂÁ-öÁb÷ö˜ÂÁ-öÁ2÷
ö øÿ'  Âa÷Åø¸ Á-öÁ-öÂ^÷öú})Â3÷Á-ö 3÷÷ù øÿ\  Áø¹ ÀböõÁ-ö	x2÷
 øÿ"  Âúöùõô`÷ö øÿà  ÂÁb÷ö!ª –$˜g#      @ ° P L€€€  —e    @ #  3      G)G,T	¨`Ğ  HL`    @SbÄã‚        a© !© á©b¤   $   ‚ÿ ™`şÿÿÿDa0Ù ŒÛ Y4,TÙ
Qb      Zp.caÂì Ïì  ¡©AG¹e       KĞ '  È5      Y%¹!½!Rd†mß/   interactive SbL €         !ª`‚ÿ ‘ ¡¦R*î•5   DOMContentLoadedReÒkG   MutationObserver,TUa      ™a      ùa      eEQd   
   .childList  axí í IUG¹e      KĞ (  É5      “¤a
      RdŠ¯¹	   childList   GRc‚—1   subtree GéDô8          „ ù%%% ùÿ…  Ãù Âbø÷ Ã2ø Åù Ã-ø%Ã-ø— ÃÂ-÷lø
™ÃÂ-÷løš ùÿ(  ÃÂÁcø÷ö‹s‚ø%ù† Â ùÿÒ  «	À-õ
Á ¿ ¿_öõôúÁb÷ö%ù‡ Âø ÁÀ¿`÷öù Á-öÁÀöiöõÁ-öÂø À})¿_÷öõôøªAG$˜g      ,P #@ P 0	    •e    @ &  3      EGaG,TX¨`l   L`   HSbÄã‚        ¡© a© !© á©c$  ¤   $   ‚ÿ E`şÿÿÿDa¦Û (İ ,T@”`:   L`    á©—Dj(           ÿÿ/ ÅÄ« ÃÂÁ`úù ª   (SbÄà®         I`şÿÿÿQd      Wp.e.client aÚÛ øÛ •G‘b          ¹e      KĞ *  ®5      ,TÙ
Iaıí ’î I—e      KĞ +  ¯5      éDp(           „ ú%%%%~% úÿÕ  Ä ¥ Ã ÂÁ`ùø ª  —˜b         e    @ )  3      ™G±G,TD¨`H   L`   0SbÄã‚        !©`$   -`şÿÿÿDa>İ ªŞ  RfšÌ×Î   google_checked_head ,Tˆ–`Ê   ,L`   á÷İ=Ù= ¡¨ !œy"s0Rj.] ½!   AdSense head tag doesn't support    Rdr½5R    attribute. éD|(           ÿÿ¤ Å/ú —UÅWú- ÄÂÁ_ù÷öÄ-ù Åy ÃÂ_úùø÷	ÿÄ-ùÄ-ùÅÃw8øÃ8øÃ^úùøª   (SbÄà®        I`şÿÿÿDaˆİ ¦Ş ÉG˜d       @ L
  H ¹e     KĞ -  §5      ”Dk           „ ú%ÄWùú¤ ÄÃ Âcùø÷ ª•–b         —e    @ ,  3      ÑG,T¡¨`ü  tL`6   8SbÄã‚        !© aªa$   ¤ÿ ™	`şÿÿÿDaÄŞ dä Y]XRT®oÕÀI   script[src*="/pagead/js/adsbygoogle.js?client="]:not([data-checked-head])   `RvöNÉ|Q   script[src*="/pagead/js/adsbygoogle.js"][data-ad-client]:not([data-checked-head])   m RÖŞ^¯   data-checked-head   	U}1,TÙ
Iaªğ Çğ IıG¹e      KĞ /  ¥5      ¤a
      ù4CGC–—Rc&0ëÚ   bottom  ReJ´E[   google_overlays •a      HGRc.WÏ   overlaysåFRc&ÎÄ   loaded  ¡åaç R~°Ä›   google_adbreak_test  áª,T’Ia!ò .ò I“”e       KĞ 0  ¦5      a  „  éDH          „ ÷%÷¹ À-õ À-õÁ¿^öõô—÷¹ À-õ À-õÁ¿^öõôÅ–-ú
Á¿¾_öúôó ÷ÿ\  Á!ÀböõÄ-ù	˜ ÷ÿ-  ÁböúŒ ÷ÿà  Á
 Àböõ~Ã ÷ÿØ  Ácöúø ÷ÿ+  Áböø÷‹ Áböø%2ù	} )Á-ø!3ö#ø3ö%öúÁ-ø'lö)š}*)À2ú+õWø÷¹ Á-ö-—÷¹ Á|/À2ö0õ÷¹ Á-ö-Ä-ù2˜-ù4Á^öùú6‹-ù8˜-ù8Á¿¾ùõúò\öõ:-ø<—->À]õ@ÀŸ-õBÁ]öõD‹˜ ÷ÿ;  ÁÀ¿cöõôF‹ ÷ÿ  ÁÀböõHª  “@˜nJ      @ P @ @ @ Ì€"s&P @ @ @    ”e    @ .  3      H1H,TÙ
Ia?ò ˆó Ñ¹e     @ 1  3      ,TL¨`V   L`   ™RcFY   sound   ReÂÂÒ.   preloadAdBreaks éDm            š%™ -   šª- ™
- šªª (SbÄã‚        Í`şÿÿÿDa*ç fè ˜b       P ‘e    @ 2  3      ,Tİ’`x  ´L`V   8SbÄã‚        a© !©a¤   $   Y`şÿÿÿDa€è ˆô  a¦0RjÖóC!   push() called with no parameters.   ,TÙ
Ia‰ô ¬ô ImH¹e      KĞ 4  °5       !« ¡ª4Rk2ò&%   first_slotcar_request_processing_time   ‰rA,RiÆw    adsbygoogle_execution_start_timema¡åSbL €         aª`‚ÿ ‘,TIaªõ Ãõ I…H’e       KĞ 5  ±5      Reî¦6   handleAdBreak   2Gù4@Rn–«ŞÒ2   'google_ad_client' is missing from the tag config.  ,TÙ
IaÅö èö ImH¹e      KĞ 6  ²5      RcJlßT   params  ,T’Ia÷ ÷ I“”e      KĞ 7  ³5      Rbf“kg   js  9y"sœR˜Şí—N   Ads with google_ad_output='js' have been deprecated and no longer work. Contact your AdSense account manager or switch to standard AdSense ads. }1,T<¨`8   L`   ÁİéDi          Ä-ù  Å^úù—Å4úª(SbÄà®        I`şÿÿÿDa,ğ xğ “˜b       à ”e     KĞ 8  ´5      RdE¡@   hasAttribute RZ±‘/   data-require-head    Rœ.|L=“   AdSense head tag is missing. AdSense body tags don't work without the head tag. You can copy the head tag from your account on https://adsense.com. 0Rj`#   Ad client is missing from the slot. 9F,TÙ
Iaù Àù ImH¹e      KĞ 9  µ5      !G%Gbğ  f  ¼  ’DÉ@   (       „ ù%%š ùÿĞ  «ÃÂøiø÷ ¨ ùÿà  Ã Âbø÷-Ã]ø˜5 ùÿ1  ÃÂ-À]õ
À-õÁ]öõÁùu À¾cõóÀ`ø÷ ùÿ.  ÃÂbø÷ÅÃúlø™Î ùÿ`  Ãaø-— !Á-öÂ]÷ö2!ù «	Â2
#÷ ùÿ  «š, ùÿ=  ÃÂbø÷% ùÿ  «Â-÷'ÃÁ^ø÷ö)‹cÃúlø+š/‚ø% ùÿ  % ùÿÕ  Â ÁÀc÷öõ,ø‹, ùÿ×  Â ÚÁ ùÿ  ¿-ô.À¾^õôó0Àc÷öõ2Œ*!Áiöú 4Á-ö6Â]÷ö8Âù «	÷$ù  ùÿ©  Â ùÿ2  ¿¾bôó:¿öõ`÷ö< ùÿ3  Âa÷>Â-÷@™'Â-÷B šÅ‹ ùÿĞ  «ÂÁ÷i÷öD¨Åú˜' ùÿà  ÂÁb÷öF ùÿ5  ÂÁc÷öHŒ)Â-÷JÅ˜ù¤ ÂÀc÷úõLÂÁ-öNl÷Pš!QÁ-öSÂÀ^÷öõUŒ( ùÿ6  ÂÁ-öWÁb÷öYÅ ùÿØ  ÂÀc÷úõ[ ùÿ\  Âù Áb÷ö]Â-÷_—~ù¤ ÂÀc÷õa-ú cÂ!À^÷úõe˜1 ùÿ\  Âù Áb÷ögÂ-÷i— ùÿĞ  «Â"Á÷i÷ök¨Â-÷m— ùÿĞ  «Â#Á÷i÷öo¨Â ùÿ\  Áù¹ ÀböõqÁ-ö$s—l÷uš ùÿˆ  ÂÁb÷öv˜! ùÿà  Â%Áb÷öx ùÿ4  Âb÷zÂ ùÿ\  Áù¹ Àböõ|Á-ö$~—l÷€š ùÿ\  Âù¹ Áb÷öÂÁ2÷$ƒöÂ ùÿ\  Áù¹ Àböõ…Á-ö&‡x2÷'‰ ùÿ"  ÂÀúöô`÷ö‹ª–l“y      P @ @ 	  
€€€€ P @ @ P @ @ D
€€€ 
€€€€   €H @ D€€€   —e    @ 3   3      uH‰H™H¥HµHÙH,TÙ
Ia[ú ²ú í¹e     @ :  !3      ,TD¨`F   L`   ù4¡HIéDk           -  ˜-  ‹-˜-  ˜-  ‹ª (SbÄã‚        	`şÿÿÿDazõ Jö ’˜b         “e    @ ;  "3      ,Tœ”`ô   L`   á	SbJ €         ¡©`¤ÿ Kd    )   1   ™     Dˆ(           …Ã oÂbø÷ ˜j ]Ã¹Âbø÷Å˜-ú Å—@ÿø¹Â-÷Ä‹Âƒ÷Ã§ø÷Ä÷ù˜ PÃbøù‹Ä˜ OÃbøù
˜ùTÅú— ^Ã¹ÂÁcø÷öª  (SbÄã‚         U`şÿÿÿDa`ö ^÷ ”‘d       P @    ¹e     @ <  #3      ,TÙ
Iaºû ü é”–e     @ =  $3      D,T ¨`ú   L`   9FRd²=   tag_partner RdÍ¿O   tag_partners¡åéDŒ(          Ã \Â¹Áb÷ö Â-÷ —løš \Ã¹Âbø÷ÃÂ2ø ÷-	˜2-	Å \ÃÂbø÷Ä-ùùø—|2ø-ùÂ-÷Ã^ø÷ú ‰Ãcø )Ãcøª   (SbÄã‚        }`şÿÿÿDa(ø ¤ù  ˜f       	` €¹ @    ¹e    @ ?  %3      ,T ‘`ş    L`   ± a¦0Rjê5Ã"   'element' has already been filled.  Rd®º¸	   innerHTML   0RjÉ=;$   'element' is not a good DOM element.XRtæ`(àL   All ins elements in the DOM with class=adsbygoogle already have ads in them.“DŒ          ˜\ !Åbú —3- ˜ %Å- Äbúù‹— Ğ«ÅÄúiúù¨Årú™ Ğ«ÅÄúiúù
¨‹  %Åaú— Ğ«ÅÄúiúù¨ª (SbÄã‚        ©`şÿÿÿDa¾ù ü •–d       @ $@   —e    @ @  &3      ,TØ¨`n  8L`   Ù
R–>P   __cmp   Y áª ¡ªRc"1   cmpMet  4¤a      Rc†İ…   tcfv1   CRc´Kğ   tcfv2   CRbNMÖ   usp Ce
CRb"˜ÄÒ   ptt `   }II…I–Yaü©ñÒMbP?éDÄ@           ÿ« Á¹Àöiöõ Å Áiöú Ä Áiöú Ã¹Á-ö˜‹Â üÁböú˜‹Å ÿÁ-ù
Àböõ˜‹Ä-ø— ÿÀ-ø¿bõôx2øÀ2øõ ÖÁÀ})¿÷3ôú3ôù3ô	-ø˜‹3ô
!¾`öõ#ª (SbÄã‚         …`şÿÿÿDaü ı (˜h%       @ P P ,° “      ¹e     @ A  '3      ,TÙ
IaÔş óş ”–e     @ B  (3      ,T—Iaşş ¢  Á”–e     @ C  )3      D,TD¨`F   L`   áàU,éDk           ˜-  ˜ š!Ä-ùÅÂ_úù÷ª (SbÄã‚        Ù`şÿÿÿDa^ Ø ˜c       P    ¹e    @ E  *3      ,TÙ
Ia÷  Ô u—‘e     @ F  +3      €D,T’Iaá æ õ—‘e     @ I  ,3      D,T’Iaó ß ¥—‘e     @ K  -3      D,T’Iaì ş A—‘e      @ M  .3      ,T„¨`Æ   L`
   HSbÄã€        !© a© ¡© á©c€ÿ €ÿ €ÿ €ÿ I`şÿÿÿDa ª ,T$—`   ]éDc           ª  (SbÄà®          á©`şÿÿÿDa" . éI¹e      KĞ O  °4       ¡¦Q,T=—`4  °L`T   8SbÄà®         !ª ¡ªa‚ÿ ‚ÿ I`şÿÿÿDaR ¦ ’ a© á©]
`    Lb          	 ¡©¤a
      ™
C]C !©‘’Yu?`,TÙ
Iaó ] IJ¹e      KĞ Q  a5      UåF)HRdêGIç   new_abg_tag $¤a      CRc’;'   host_v  CRdÙ®ù	   frequency   Ya{®Gáz„?•%JYa{®Gáz„?”a
      ¡åC“G,TÙ
Qc      k.push  aH	 V	 ¡åJ¹e      KĞ R  b5      ¡åÉ½(Rhæ<ÿ   requestNonPersonalizedAds   ”a      ‰C”Re"›Ê7   pauseAdRequests ¤a      ”CRczæÑÌ   onload  •a      ”C]
`   M`   EJMJaÍUı$é5a¦  ª  HKp€       ‘     «    á     8  r  À     …  ˜  Ğ     D)x          „ õ%% ÿÿş ¿õ «¾bôó % ÿÿı ¿ÿu¾½¼cóòñ¾bôó ÿÿ[ ¿ÿr¾½¼cóòñ¾bôóõ «¿aô
ÿ»¿¾z%½¼º-ï»]ğï6òñcôóòÿ½¿ÿ¼¾ÿ¹½bóò¾bôó—ÿ¹Åõ «¿ ÿÿ ¾})½õ «3òÿu¼»ºcñğï3ò	bóò ¾½côóò"%ÿ¹¿-ô
$¿-ô&œ‹ ÿÿç ¿¾-ó(¾bôó*Ä ÿÿf ¿½côúò, ÿÿü ¿½úóùñ`ôó. ÿÿ… ¿ ÿÿf ¾bôó0˜ ÿÿ> ¿aô2 ÿÿà ¿ ¾bôó4 ÿÿ× ¿ >¾ ÿÿá ½¼ùlñ6¼bòñ7½côóò9ÿ¿aô;˜¿ÿ¾ÿ½aò=½¼cóòñ?oôA“) ÿÿÔ ¿ ÿÿ… ¾ ÿÿƒ ½bóòB¾bôóD ÿÿ² ¿aôF ÿÿ> ¿aôHÿô ÿÿ ¾aóJ‹§ô ÿÿ± ¿aôL ÿÿ, ¿¾½côóòN!PÅ-úRÄ˜-ùT•( ÿÿÖ ¿¾}V)½ÿr»º¹cğïîXw3òZÿr»º¹cğïî]w3ò_¼`ôóa ÿÿ7 ¿aôc}e)¿3ôfôøøÿô!h½-òj¾»}l)º ÿÿ8 3ïmøñ\óòo!h½-òj¾»}q)º ÿÿ9 3ïrøñ\óòt!h½-òj¾ »}!v)º ÿÿ: 3ïwøñ\óòy‹§ôù˜Œz"{%½²ò|~¾-ó#€¿½ÿï½]ôó‚¹ eË î-î$„—(-î†¹½îöö÷ö/ùˆUš÷/ùŠ·4ø÷ŒìŠ6 ÿ»¼‹»¼§ºò—#ÿî-ó%Ÿ¸]íó‘ ·eË ì‹¹ñ™î©ï§ñšğ© ÿÿ& ¿½¼ùó`ôó“ø2ú•ù˜-ù —¾2ø ™ó ÿÿ… ¿ ÿÿk ¾bôó›—( ÿÿ® ¿¾bôóÂ˜!
Ÿ¾-ó&¡¾-ó'£¿^ôó÷¥ª  J|˜}§      @ @ ³ € `2@ @ P @ @ ‘ € "@ @ P LH @€ &P “&0€€Y @ € 8p€  €€ 
   ¹e      KĞ P  ±4      éD{    (       „ ú%%%%%%% ‹% úÿÒ  «Ã-ø Ä úÿØ  Â^ùø÷ úÿÕ  Ä ¦ ÃÂcùø÷ª éI—b      @ e    @ N  ®4      íIùIJ9J,Tğ¨`   DL`   Rb{¦   _fy I½
L`   LRq®FL@   https://pagead2.googlesyndication.com/pagead/managed/js/adsense/Re
.   /slotcar_libraryRbå   .js …JL`   <RmŞJì–0   https://googleads.g.doubleclick.net/pagead/html/ ¡œRe‚|Èb   /zrt_lookup.html™JL`   J ¡œRdFÂ×Ç   /zrt_lookup Rcö+W    .html   ©J<¤a      ÍCÉCCµC¥C`C–½
L`   ‰JReşÙDu   /show_ads_impl  ‘J½JÉ”L`   •$Rg*ç—   /show_ads_impl_with_ama –ÉJµ¥ RœÕ·¦‘   ^(?:https?:)?\/\/(?:pagead2\.googlesyndication\.com|securepubads\.g\.doubleclick\.net)\/pagead\/(?:js\/)?(?:show_ads|adsbygoogle)\.js(?:[?#].*)?$   `éDÜH           ÜÁsÀ¾cõó möš ÁsÀ¾cõów8ö‹ÅuÁ¿cöôÄuÁ¿cöôÃ¾Á€
Àôúó`öõ¾Á€Àøôùó`öõÂ¾Á€Àøôùóúò`öõÄ}Á3ö¾À€¿óúò`õô3ö¾À€	¿óúò`õô3ö
÷3ö ù3ö"y$  3ö%öª(SbÄã‚        I`şÿÿÿDa. Ğ (˜h'       DH 0	`0	`20	€I 0À “  ¹e    @ S  ¯4      D`   €D]De    @    ]1      ÅÍÕéñù!-5MU]y•­ÉÑù%-AU]qy¥ÉÕáéı%9MU]m•¥­Éİñù-=Q]q…•­µÉİå	%9Mau}‘¡µÉİñ-5I]q‰­½ÍÕáñ)9IQY‰¥Å	AMemu‰Éİõ-YeqÍÕİõı!9½ÑíõµÉÑ)Meå	iqåíù	%-9M…¡õ=EMU‘¡¹Ááéñù)EMUe}…•­µ½Åõı!iy©½ù	1EYuµ½åù ! 1 … ¡ I!y!İ!õ!="e"m"…"™"¡"©"½"…##•#í#¡$µ$É$Ñ$Ù$á$é$%E%)'Å'Ñ'İ'å'¹(Í(Õ(İ(å(ı(Á)É)Ñ)Ù)á)ñ)ù)*	*!*)*1*A*I*Y*a*y*‘*¥*Ù*++1+9+Q+i+q+,),E,q,¡,¹,Á,-M-U-]-e-m-…-µ-å-í-ı-...i.q.™.¡.å.í.)/E/M/U/]/e/m/u/Ñ/ù01%1I1Q1i1q11ù122Q2Y2•2©2Ù2á233-3Y3‰3å3í3444!4)4½4é455=5•5Å5Í5Õ566M6]6m6}666­6½6Í6í67m7‘7¥7İ788A8]8•89m99½9):1:9:-;5;‘=™=¡=½=é=ñ=->5>=>E>M>U>q>¡>Á>I?Q?i?‰?İ?õ?½@İ@ñ@A1AqAyAéA!B5B}B‘BECC©CÍCÍDÕDİDåDíDõDıDEEEE%EuE}E…EEÉEéEıEUF‘F­FÙFõF5G‰G½GñG=HEHaHåHíHII%IAIeI™I¡I©I½IÅIÍIÕIRb
Màæ   selfRdòNš   closure_uid_0â6‰2RÊxª   CLOSURE_FLAGS   iı6¹ á˜RcF-Ö   Android éÉñÚ,Riƒ6»   ^-?([1-9][0-9]*|0)(\.[0-9]+)?$  ReBñ£   structuredClone ù$La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
aôg  úk  „Dù](Õ
a      á]aÔ     	aÔ      ”]au}¥	¹$La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
aNw  pw  „Dù]Õ
a      á]aÔ      ]ñ$La      X‘a      ’y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a€w  ¢w  „Dù]Õ
a      á]aÔ      •]ù]
`    Mc             ¡­$La      X–a      —y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aŠ}   ~  „Dù](Õ
a      á]aÔ     	aÔ      “]­Á,Riö)Ç‡   ^([^?#]*)(\?[^#]*)?(#[\s\S]*)?  $La      X”a      •y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aø„  ~…  „Dù](Õ
a      á]aÔ     	aÔ      ’]IQy‰¡±µRc®óÖr   append  Á%#=„RzBg—x   ^(?:([^:/?#.]+):)?(?://(?:([^\\/?#]*)@)?([^\\/?#]*?)(?::([0-9]+))?(?=[\\/?#]|$))?([^?#]+)?(?:\?([^#]*))?(?:#([\s\S]*))?$Rbr_Ğ}   #|$ ˜R”Fã ÑŠ   alternate author bookmark canonical cite help icon license next prefetch dns-prefetch prerender preconnect preload prev search subresource  áøRe¶+²   ^([0-9.]+)px$    Rf¢ùª   ^(-?[0-9.]{1,30})$  	)1iq‰­ÙáYUéy‘¥­$La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
a¶  ·  „Dù]Õ
a      á]aÔ      •]ÅÍDRo’…aš5   ^https?://(\w|-)+\.cdn\.ampproject\.(net|org)(\?|/|$)   $La      X–a      —y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aô·  H¸  „Dù]Õ
a      á]aÔ      “]Õ$La      X”a      •y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aP¸  ä¸  „Dù]Õ
a      á]aÔ      ]İ$La      X‘a      ’y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aŞ»  ½  „Dù]Õ
a      á]aÔ      •])¹RcRı±   measure Rdò…ı 
   clearMarks  Q$La   
   X–a      —y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a À  *Ä  „Dù]4Õ
a      á]aÔ     ÁaÔ     İaÔ      •]‰¥É$La      X–a      —y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aöË  vÌ  „Dù]Õ
a      á]aÔ      –]$La      X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aLÏ  :Ş  „Dù]pÕ
a      á]aÔ     QaÔ     õ
aÔ      a«aÔ  
   I
aÔ     aÔ      á¢aÔ      a§aÔ      “]-AYm¥áé$La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
a¨Ş  ÊŞ  „Dù]Õ
a      á]aÔ      —]]
`    Mb         ”`    Mc         
   ”`    Mb         $La      Xa      ‘y`^
  ½`^  ¹}`~  Á¥	`Ô  u
arç  ”ç  „Dù]Õ
a      á]aÔ      •]±]
`    Ma      $La      X–a      —y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a¶ç  è  „Dù](Õ
a      á]aÔ     ÉaÔ      “]ÍÁ$La      X”a      •y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a&è  Hè  „Dù]Õ
a      á]aÔ      ’]İ]
`    Ma   
   —`    Me
                  $La      X“a      ”y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a¤è  òè  „Dù]Õ
a      á]aÔ      ]í]
`    Ma      $La      X‘a      ’y`^
  ½`^  ¹}`~  Á¥	`Ô  u
até  úé  „Dù](Õ
a      á]aÔ     ÉaÔ      –]ı$La   
   X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a’ê  fë  „Dù]4Õ
a      á]aÔ     -aÔ     9aÔ      •]%1$La      X–a      —y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a„ì  í  „Dù](Õ
a      á]aÔ      maÔ     –]]e$La      X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a$í  Fí  „Dù]Õ
a      á]aÔ      •]q$La      X–a      —y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aVí  xí  „Dù]Õ
a      á]aÔ      ’]y$La      X“a      ”y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aˆí  î  „Dù](Õ
a      á]aÔ      maÔ     —]‰$La      Xa      ‘y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a€î  Îî  „Dù]Õ
a      á]aÔ      –]™$La      X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a(ï  vï  „Dù]Õ
a      á]aÔ      “]©]
`    $Mg      
                  $La      X”a      •y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a°ï  şï  „Dù]Õ
a      á]aÔ      ‘]±$La      X’a      “y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a’ğ  àğ  „Dù]Õ
a      á]aÔ      –]É$La      X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
ağğ  >ñ  „Dù]Õ
a      á]aÔ      “]Ñ]
`    Mb         $La      X”a      •y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a²ò   ó  „Dù]Õ
a      á]aÔ      ‘]	]
`    Mc         
   $La      X’a      “y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a*ó  xó  „Dù]Õ
a      á]aÔ      —]]
`    Ma   
   ”`    Md               $La      Xa      ‘y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a¼ó  
ô  „Dù]Õ
a      á]aÔ      •]!]
`    Mb         $La      X–a      —y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aô  >õ  „Dù](Õ
a      á]aÔ     AaÔ      “]19]
`    Md      
         ] !¬$La      X”a      •y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a|ü  
ı  „Dù]Õ
a      á]aÔ      ”]¥$La      X•a      –y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aÂ  € „Dù]Õ
a      á]aÔ      ‘]Í$La      X’a      “y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aˆ 
 „Dù]Õ
a      á]aÔ      –]å$La      X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aª j „Dù]Õ
a      á]aÔ      “])=$La      X”a      •y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a$ Š „Dù]Õ
a      á]aÔ      ‘]U±=R=‰$La      X’a      “y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a@ Ê „Dù]Õ
a      á]aÔ      ‘]å$La      X’a      “y`^
  ½`^  ¹}`~  Á¥	`Ô  u
ar Î „Dù]LÕ
a      á]aÔ      !«aÔ      áªaÔ      a«aÔ   
    ¡ªaÔ     –]=!!%!-!5!$La      X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aê  D! „Dù]Õ
a      á]aÔ      —]!¡!l¤a2      •`    e`   RbE"i   Qb  `   ¹	`   Y`   U`
   `   ğ`   RbÖGŞ   Pb  `   ¡`   ñ`   ­`   $¤a      ñ`    …`   `   $La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
aJ% & „Dù](Õ
a      á]aÔ     eaÔ      –]å!í!$La      X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aN' @( „Dù](Õ
a      á]aÔ     iaÔ      •]ı!"$La      X–a      —y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aT( P* „Dù]XÕ
a      á]aÔ     QèaÔ     ¡èaÔ     İaÔ  
   uaÔ     ÉaÔ      ”]"""%"-"5"$La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
a+ h, „Dù]@Õ
a      á]aÔ     ‰aÔ     uaÔ     òaaÔ   
   “]E"M"U"]"é"$La   
   XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
a"/ X0 „Dù]4Õ
a      á]aÔ     ÉaÔ     éaÔ      ]Ñ"ñ"ù"$La   
   X‘a      ’y`^
  ½`^  ¹}`~  Á¥	`Ô  u
al0 š1 „Dù]4Õ
a      á]aÔ     ÉaÔ     %#aÔ      ]###$La      X‘a      ’y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aœ1 p2 „Dù](Õ
a      á]aÔ     ‰aÔ      ‘])#5#$La      X’a      “y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a~2 Ò2 „Dù](Õ
a      á]aÔ     E#aÔ      ]I#=#]
`    Ma      $La      X‘a      ’y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aä2 3 „Dù]Õ
a      á]aÔ      ]Q#Y#a#i#$La      X‘a      ’y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a@7 ®7 „Dù](Õ
a      á]aÔ     y#aÔ      ]}#q#$La      X‘a      ’y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aÜ< := „Dù]Õ
a      á]aÔ      —]#]
`   M`    RfÂD°   google-auto-placed  ¤a
      µ#`P    Rf¶åŠB   google_tag_origin   Rb*²¡   qs  $La      Xa      ‘y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a> .> „Dù]Õ
a      á]aÔ      —]½#$La      Xa      ‘y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a@> b> „Dù]Õ
a      á]aÔ      ”]Å#$La      X•a      –y`^
  ½`^  ¹}`~  Á¥	`Ô  u
ar> ”> „Dù]Õ
a      á]aÔ      ‘]Í#$La      X’a      “y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a¤> Æ> „Dù]Õ
a      á]aÔ      –]Õ#]
`    Me
                  $La      X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aü> ? „Dù]Õ
a      á]aÔ      ”]İ#$La      X•a      –y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a.? |? „Dù]Õ
a      á]aÔ      ‘]å#]
`    Ma      $La      X’a      “y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aA  B „Dù]Õ
a      á]aÔ      —]õ#$La      Xa      ‘y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aB vB „Dù]Õ
a      á]aÔ      ”]$$La      X•a      –y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a~B îB „Dù]Õ
a      á]aÔ      ‘]%$$La      X’a      “y`^
  ½`^  ¹}`~  Á¥	`Ô  u
apC àC „Dù]Õ
a      á]aÔ      –]=$]
`   M`   é“`   M`
   RZ]–½  A7CQXglZzTrThjGTBEn1rWTxHOEtkWivwzgea+NjyardrwlieSjVuyG44PkYgIPGs8Q9svD8sF3Yedn0BBBjXAkAAACFeyJvcmlnaW4iOiJodHRwczovL2RvdWJsZWNsaWNrLm5ldDo0NDMiLCJmZWF0dXJlIjoiUHJpdmFjeVNhbmRib3hBZHNBUElzIiwiZXhwaXJ5IjoxNjk1MTY3OTk5LCJpc1N1YmRvbWFpbiI6dHJ1ZSwiaXNUaGlyZFBhcnR5Ijp0cnVlfQ==%R!î|¨  A3vKT9yxRPjmXN3DpIiz58f5JykcWHjUo/W7hvmtjgh9jPpQgem9VbADiNovG8NkO6mRmk70Kex8/KUqAYWVWAEAAACLeyJvcmlnaW4iOiJodHRwczovL2dvb2dsZXN5bmRpY2F0aW9uLmNvbTo0NDMiLCJmZWF0dXJlIjoiUHJpdmFjeVNhbmRib3hBZHNBUElzIiwiZXhwaXJ5IjoxNjk1MTY3OTk5LCJpc1N1YmRvbWFpbiI6dHJ1ZSwiaXNUaGlyZFBhcnR5Ijp0cnVlfQ==%R!F™®Ã  A4A26Ymj79UVY7C7JGUS4BG1s7MdcDokAQf/RP0paks+RoTYbXHxceT/5L4iKcsleFCngi75YfNRGW2+SpVv1ggAAACLeyJvcmlnaW4iOiJodHRwczovL2dvb2dsZXRhZ3NlcnZpY2VzLmNvbTo0NDMiLCJmZWF0dXJlIjoiUHJpdmFjeVNhbmRib3hBZHNBUElzIiwiZXhwaXJ5IjoxNjk1MTY3OTk5LCJpc1N1YmRvbWFpbiI6dHJ1ZSwiaXNUaGlyZFBhcnR5Ijp0cnVlfQ==R³{  As0hBNJ8h++fNYlkq8cTye2qDLyom8NddByiVytXGGD0YVE+2CEuTCpqXMDxdhOMILKoaiaYifwEvCRlJ/9GcQ8AAAB8eyJvcmlnaW4iOiJodHRwczovL2RvdWJsZWNsaWNrLm5ldDo0NDMiLCJmZWF0dXJlIjoiV2ViVmlld1hSZXF1ZXN0ZWRXaXRoRGVwcmVjYXRpb24iLCJleHBpcnkiOjE3MTk1MzI3OTksImlzU3ViZG9tYWluIjp0cnVlfQ==R"NÚ  AgRYsXo24ypxC89CJanC+JgEmraCCBebKl8ZmG7Tj5oJNx0cmH0NtNRZs3NB5ubhpbX/bIt7l2zJOSyO64NGmwMAAACCeyJvcmlnaW4iOiJodHRwczovL2dvb2dsZXN5bmRpY2F0aW9uLmNvbTo0NDMiLCJmZWF0dXJlIjoiV2ViVmlld1hSZXF1ZXN0ZWRXaXRoRGVwcmVjYXRpb24iLCJleHBpcnkiOjE3MTk1MzI3OTksImlzU3ViZG9tYWluIjp0cnVlfQ==$La      X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aDR  S „Dù]Õ
a      á]aÔ      •]Q$$¤a      RdšË,	   rectangle   `   Rdº¦7”
   horizontal  `   RcvÒ¶   vertical`   L’a"      %H`   Reª€,Ö   interstitials   `   RdzĞYt	   vignettes   `   RcÒ‹(v   inserts `   Rdòµ/¨
   immersives  `   RdÌœŸ	   list_view   `
   RdR°a	   full_page   `   Rd.@¡Ë
   side_rails  `   %%-%M%U%i%‘%™%Á%Ù%á%é%ñ%$La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
a$o 4p „Dù]@Õ
a      á]aÔ     &aÔ      ¡ªaÔ       áªaÔ  
   ’]ù%&%&9&M&a&‰&±&Ù&ñ&'¬¤aR      /Gù4G1G$Rg"úä‚   google_ad_host_channel  GiFG4Rk.h0'   google_tag_for_child_directed_treatment G0RjZP*´#   google_tag_for_under_age_of_consent G Rf~îAå   google_tag_partner  G,Ri²<¥×   google_restrict_data_processing GReŞ9ì   google_page_url G Rfj3§   google_debug_params G Rf§g   google_shadow_mode  G-HG$RgJé²   google_ad_frequency_hintG,Ri:d	   google_admob_interstitial_slot  G(Rh6«Õw   google_admob_rewarded_slot  G$Rg–îçŠ   google_admob_ads_only   G(RhNŒï   google_max_ad_content_ratingG$RgÏ)   google_traffic_source   GHG$RgÎjîs   (^| )adsbygoogle($| )   $La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
a¢z Äz „Dù]Õ
a      á]aÔ      “]1'$La      X”a      •y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aÔz öz „Dù]Õ
a      á]aÔ      ]9'$La      X‘a      ’y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a{ V{ „Dù](Õ
a      á]aÔ       ¡ªaÔ     •]I'A'$La      X–a      —y`^
  ½`^  ¹}`~  Á¥	`Ô  u
af{ ˆ{ „Dù]Õ
a      á]aÔ      “]Q'$La      X”a      •y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a˜{ º{ „Dù]Õ
a      á]aÔ      ]Y'$La      X‘a      ’y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aÊ{ ì{ „Dù]Õ
a      á]aÔ      •]a'$La      X–a      —y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aü{ | „Dù]Õ
a      á]aÔ      ’]i'$La      X“a      ”y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a.| P| „Dù]Õ
a      á]aÔ      —]q'$La      Xa      ‘y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a`| ¸| „Dù](Õ
a      á]aÔ     'aÔ      ”]…'y']
`    Mc            $La      X•a      –y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aŞ|  } „Dù]Õ
a      á]aÔ      ”]']
`    Md      
         $La      X•a      –y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a0} R} „Dù]Õ
a      á]aÔ      ’]•'$La      X“a      ”y`^
  ½`^  ¹}`~  Á¥	`Ô  u
ab} ¼} „Dù](Õ
a      á]aÔ       ¡ªaÔ     —]¥'']
`    Mb         $La      Xa      ‘y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aŞ} 0~ „Dù](Õ
a      á]aÔ       ¡ªaÔ     –]µ'­']
`    Mb         $La      X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aV~ x~ „Dù]Õ
a      á]aÔ      •]½']
`    Md         
      $La   
   X–a      —y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a*ƒ >‡ „Dù]4Õ
a      á]aÔ     ı'aÔ     	aÔ      “]í'õ'($La      X”a      •y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a@‡ * „Dù]pÕ
a      á]aÔ     õ
aÔ      a«aÔ     QaÔ  
    á¢aÔ     I
aÔ     aÔ      a§aÔ      ”]	((!()(1(9(A(I(Q(Y(±(0¤j                                    0—j                                   í(õ($La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
aº¢ £ „Dù]Õ
a      á]aÔ      ”]))))%)5)$La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
a²¥ J§ „Dù]Õ
a      á]aÔ      ]-)9)M)q)…))±)$La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
a
¬ ^¬ „Dù]Õ
a      á]aÔ      –]¹)$La      X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
ağ° T± „Dù]Õ
a      á]aÔ      “]é)**$La      X”a      •y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aºŞ à „Dù]Õ
a      á]aÔ      ’]9*$La      X“a      ”y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a0á Rá „Dù]Õ
a      á]aÔ      —]Q*$La      Xa      ‘y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aPå ”å „Dù]Õ
a      á]aÔ      ”]Å*í,Ri‚fÜ_    google_prev_ad_formats_by_region0Rj’@ñŸ"   google_prev_ad_slotnames_by_region  á*‰í*uù*RcJÊÜG   isEmpty +òa$La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
a2ğ €ğ „Dù]Õ
a      á]aÔ      ’]!+]
`    Ma      )+$La   
   X“a      ”y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a"ú @  „Dù]4Õ
a      á]aÔ     YaÔ     ¥+aÔ      ‘]y+•++©+ù+$La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
aŞ   „Dù]Õ
a      á]aÔ      –]É,]
`    Mc            $La      X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a* L „Dù]Õ
a      á]aÔ      ”]Ñ,]
`    Mc            $La   
   X•a      –y`^
  ½`^  ¹}`~  Á¥	`Ô  u
av ø „Dù]4Õ
a      á]aÔ      ¡ªaÔ       áªaÔ     ’]ù,Ù,é,$La      X“a      ”y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a Z „Dù]Õ
a      á]aÔ      ‘]	-$La   
   X’a      “y`^
  ½`^  ¹}`~  Á¥	`Ô  u
ax ’ „Dù]4Õ
a      á]aÔ     ÁaÔ      a«aÔ      –]-!-)-$La      X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a¢ ğ „Dù]Õ
a      á]aÔ      –]1-$¤a       !œ`     !§`    a¤`    !œ !§ a¤$La      X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
al ø „Dù](Õ
a      á]aÔ     I-aÔ      ”]9-A-]
`    Ma      $La      X•a      –y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a¢ ğ „Dù]Õ
a      á]aÔ      ”]u-$La      X•a      –y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a  N „Dù]Õ
a      á]aÔ      ‘]}-$La      X’a      “y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aÂ " „Dù](Õ
a      á]aÔ     I-aÔ      –]-•-$La      X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a2 € „Dù]Õ
a      á]aÔ      •]­-$La      X–a      —y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aê 8 „Dù]Õ
a      á]aÔ      ’]Å-]
`    XMt(                                                               QêÕ- !©áò$La      X“a      ”y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a$  F  „Dù]Õ
a      á]aÔ      ‘]õ-$La      X’a      “y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aP% 0& „Dù]Õ
a      á]aÔ      –].%.-.$La      X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aJ, ¢, „Dù](Õ
a      á]aÔ       ¡ªaÔ     •]=.5.$La      X–a      —y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a², 
- „Dù](Õ
a      á]aÔ       ¡ªaÔ     “]M.E.$La      X”a      •y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a- <- „Dù]Õ
a      á]aÔ      ‘]U.$La      X’a      “y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aL- n- „Dù]Õ
a      á]aÔ      –]].]
`    Ma      5“`   L`   “`   Ma      Rbò$ó   usca“`   Ma      Rb^ÜµK   usva“`   Ma      Rb®”·V   usco“`   Ma      RbŠL¤²   usct!çy.é‰.$La      X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aØB šC „Dù](Õ
a      á]aÔ     ¹.aÔ      —]©.±.½.Ñ.$La      Xa      ‘y`^
  ½`^  ¹}`~  Á¥	`Ô  u
ahI ¾I „Dù]Õ
a      á]aÔ      ]õ.$La      X‘a      ’y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aÎI "J „Dù](Õ
a      á]aÔ       ¡ªaÔ     •]/ı.$La      X–a      —y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a2J ŠJ „Dù](Õ
a      á]aÔ       ¡ªaÔ     “]//]
`    Ma      !/$La      X”a      •y`^
  ½`^  ¹}`~  Á¥	`Ô  u
ar[  _ „Dù]Õ
a      á]aÔ      “]}/¤a
      /G1G‘/™/¡/©/±/¹/Á/É/$La   
   XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
aşh zi „Dù]4Õ
a      á]aÔ      ¡ªaÔ       áªaÔ     •]ù/Ù/é/$La   
   X–a      —y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aŠi j „Dù]4Õ
a      á]aÔ      ¡ªaÔ       áªaÔ     ”]0	00]
`    Ma      $La      X•a      –y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a0j †j „Dù](Õ
a      á]aÔ     E#aÔ      ”]10!0]
`    Ma      $La      X•a      –y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a¨j Êj „Dù]Õ
a      á]aÔ      ”]A0]
`    Ma      $La      X•a      –y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aìj k „Dù]Õ
a      á]aÔ      ’]Q0]
`    Ma      $La      X“a      ”y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a0k äk „Dù]@Õ
a      á]aÔ      ¡ªaÔ      áªaÔ       !«aÔ  
   ]‘0a0q00$La      X‘a      ’y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aôk l „Dù]Õ
a      á]aÔ      ]¡0]
`    Md               $La      X‘a      ’y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aDl –m „Dù]LÕ
a      á]aÔ      áªaÔ      a«aÔ      ¡ªaÔ   
    !«aÔ     –]é0±0Á0Ñ0Ù0]
`    Ma   &   —`    Mb         —`    Mc            ©1Å1Ù1ñ192a2i22$La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
aŠ  Š „Dù]Õ
a      á]aÔ      ’]¹3Í3Õ3İ3$La      X“a      ”y`^
  ½`^  ¹}`~  Á¥	`Ô  u
ağ |‘ „Dù]Õ
a      á]aÔ      ’]	4$La      X“a      ”y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aÊ— f› „Dù]Õ
a      á]aÔ      —]M4$La      Xa      ‘y`^
  ½`^  ¹}`~  Á¥	`Ô  u
ax› Fœ „Dù]Õ
a      á]aÔ      ”]•4µ4$La      X•a      –y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aè  T¡ „Dù]Õ
a      á]aÔ      ’]5,¤a      -5G$R ŞÜ`#   google_analytics_uacct  G%GG¥9G­5µ5½5<—a      Rc
«ï   120x90  GRc†âé   160x90  GRcÜşî   180x90  GRc–9ÖU   200x90  GRcÎ‚k·   468x15  GRcf»Éh   728x15  G$La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
a½ ˜½ „Dù](Õ
a      á]aÔ     I-aÔ      ]56E6$La      X‘a      ’y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a@À À „Dù]Õ
a      á]aÔ      —]İ6]
`    Mb         dRwòĞáEW   platform platformVersion architecture model uaFullVersion bitness fullVersionList wow64 $La   
   Xa      ‘y`^
  ½`^  ¹}`~  Á¥	`Ô  u
azâ Üã „Dù]4Õ
a      á]aÔ     MaÔ      áªaÔ      •]å9::T¤a&      ı9YaÅÓWÁà?Reê¯™…   image_sidebysideYaÅÓWÁĞ?,Rif{6|   mobile_banner_image_sidebyside  YaÅÓWÁĞ?(Rhâ|‚g   pub_control_image_stacked   YaÅÓWÁà?(Rhfú…„   pub_control_image_sidebysideYaÅÓWÁĞ?,RişÛî   pub_control_image_card_stacked  YaÅÓWÁà?0Rj/!   pub_control_image_card_sidebyside   Ya‘b¨¿Ñ?RevOò   pub_control_text`    $Rgş#,÷   pub_control_text_card   `    T•a&      –`    …U`    U`    •U`    U`    ¥U`ª   ­U`    µU`    ¹U`    <¤a      ‘`È   ’`  “`,  ”`ô  •`È   –`,  m$La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
a¤ n „Dù](Õ
a      á]aÔ     ©aÔ      “]å:ù:]
`   M`   ==8Rl¾Y"])   google_content_recommendation_columns_num   4RkÊ
Ô‘&   google_content_recommendation_rows_num  $La      X”a      •y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a’ * „Dù](Õ
a      á]aÔ       ¡ªaÔ     ”];;;%;$La   
   X•a      –y`^
  ½`^  ¹}`~  Á¥	`Ô  u
af h „Dù]4Õ
a      á]aÔ      ¡ªaÔ       áªaÔ     ”]M;=;E;4¤a      a;Cu;C‰;C™;C­;CY;”e;•y;–;—;$La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
a˜ ş „Dù](Õ
a      á]aÔ       ¡ªaÔ     ]¹;±;Á;É;ñ;]
`    LLq"                                                                       ‘`    LLq"                                                                       <9<m<…<==%=-=U=m=‰=$La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
aœe Rh „Dù]LÕ
a      á]aÔ      !«`V  Q
`   F a«`V  “a   
    !¬`V  “`   FD—]É>?%?1?=?@!@)@1@E@M@U@i@}@…@@¡@©@]
`    Mc         (   $La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
aæƒ ¾… „Dù]Õ
a      á]aÔ      ‘]ÑA$La      X’a      “y`^
  ½`^  ¹}`~  Á¥	`Ô  u
a
† V† „Dù]Õ
a      á]aÔ      –]BYC$La      X—a      y`^
  ½`^  ¹}`~  Á¥	`Ô  u
at“ <” „Dù]Õ
a      á]aÔ      ”]áCõC$¤a      CõCCıC“	D”D•$La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
az– L˜ „Dù]Õ
a      á]aÔ      “]!D$La      X”a      •y`^
  ½`^  ¹}`~  Á¥	`Ô  u
ab˜ „˜ „Dù]Õ
a      á]aÔ      ]MDUD$¤a      CõCC]DiDuD$La      XÕ
a      áy`^
  ½`^  ¹}`~  Á¥	`Ô  u
aÎš <œ „Dù]Õ
a      á]aÔ      —]D™D$La      Xa      ‘y`^
  ½`^  ¹}`~  Á¥	`Ô  u
aŒ¾ ÖÁ „Dù]Õ
a      á]aÔ      •]•EİI­8qJéD=          „  =ä %%%%%%
%%	%
	%
%%%%%%%%%%%%Á%%%% %!%"%#%$%% %&! %'"!%(#"%*$#%+%$%,&%%-%.'&%/('%0)(%1*)%2+*%3,+%4-,%5.-%7%8/.%:0/%;10%<21%=32%>%?43%@54%A65%B76%C87%D98%E:9%F;:%G<;%H=<%I%J%K>=%L?>%M@?%NA@%OBA%PCB%QDC%RED%SFE%UGF%VHG%WIH%XJI%YKJ%ZLK%[ML%\NM%]ON%^PO%_QP%`RQ%aSR%bTS%cUT%dVU%eWV%fXW%gYX%hZY%i[Z%j\[%k]\%l^]%m_^%n`_%oa`%pba%qcb%rdc%sed%tfe%ugf%vhg%wih%xji%ykj%zlk%{ml%|nm%}%~%on½po%‚qp%…rq¼sr%†ts%‡ut%ˆvu%‰wv%Šxw»yx%‹zy%{z%|{%’}|%“~}%•~%–€%—€%˜‚%™ƒ‚%œ„ƒ%%…„%Ÿ†…% ‡†%¡ˆ‡%¢‰ˆ%£Š‰%¤‹Š%¥Œ‹%¨Œ%©%­%®%²%³%´%·%º‘%¼’‘%½“’%¾%Ã%Å%È”“%É•”%Ê–•%Ë%Í%Î%Ï—–%Ğ˜—%Ñ™˜%Òš™%Ó›š%Ôœ›%Õœ%Ö%×%ØŸ%ß Ÿ%à¡ %á¢¡%â£¢%ã¤£%ê¥¤%ì¦¥%í§¦%ï¨§%ğ©¨%ñª©%ò«ª%÷¬«%ù­¬%ı®­%ş¯® %°¯ %±° %²± %³² %´³ %µ´ %¶µ %·¶ %¸·¹¹¸ %º¹ %»º %¼» %½¼ %¾½ %¿¾ %À¿ %ÁÀ %ÂÁ %ÃÂ %ÄÃ %ÅÄ % ÆÅ %%ÇÆ %& %'ÈÇ %(ÉÈ %) %*ÊÉ %+ËÊ %,ÌË %-ÍÌ %.ÎÍ %/ÏÎ %0ĞÏ %1ÑĞ %2ÒÑ %3ÓÒ %4ÔÓ %5ÕÔ %6ÖÕ %7×Ö %8Ø× %9ÙØ %: %;ÚÙ %<ÛÚ %= %> %? %@ %AÜÛ %BİÜ %D %FŞİ %G %HßŞ %Iàß %Jáà %Kâá %Lãâ %Mäã %Nåä %O %P %Q %R %T %U %V %Wæå %Yçæ %Zèç %[ %\ %]éè %dêé %…ëê %†ìë %‡íì %ˆîí %‰ïî %Šğï %ñğ %òñ %‘ %™ %š %£ %¥óò %¦ôó %¸õô %¹öõ %º÷ö %» %¼ %½ %¾ø÷ %Áùø %Âúù %Äûú %Æüû %Çıü %Ê %Ë %Ì %Í %Î %Ï %Ğ %Ñ %Ò %Óşı %Úÿş %Û  ÿ  %Ü   %İ  %Ş  %à  %á  %â  %ã %ä  %æ  %ç 	 %è %é 
	 %ê 
 %ë  %í  %î  %ï  %ğ  %ñ %ò %ô  %õ  %ö  %÷ %ù  %ú  %û  %ü  %ı  %ş %ÿ  %  %  %  %  %  %  % %   % % % !  % "! % #" % $# % %$ % &% % '& % % %  (' %$ )( %% *) %' +* %( ,+ %) -, %* %+ %- .- %2 /. %3 %4 0/ %5 10 %6 21 %8 32 %9 43 %> 54 %? 65 %@ 76 %A 87 %B 98 %C :9 %D ;: %E %F %G %J %K %N <; %P %Z =< %[ >= %\ ?> %] @? %^ A@ %_ BA %` CB %a DC %b %c ED %h FE %i GF %j HG %l IH %m %n %o JI %q KJ %r LK %s ML %t NM %u ON %v PO %w QP %x RQ %y %z %| %} SR %~ TS % UT %€ % VU %‚ WV %ƒ XW %„ YX %… %† ZY %‰ [Z %Š %‹ \[ %Œ ]\ % ^] % _^ %“ `_ %• a` %– ba %— cb %˜ dc %š ed %› fe %œ gf % hg % ih %Ÿ ji %  kj %¡ lk %¢ %¤ ml %¥ nm %¦ on %§ po %¨ qp %© rq %ª sr %« ts %¬ ut %­ vu %® wv %¯ xw %° yx %± zy %² {z %³ %´ %µ %¶ %· |{ %¸ }| %¹ %º ~} %» %¼ %½ ~ %Á € %Â %Ã %Ä %Î %Ï %Ó %Ô € %Õ ‚ %Ö ƒ‚ %× „ƒ %Ø …„ %Ù †… %Ú ‡† %Û ˆ‡ %Ü ‰ˆ %İ Š‰ %Ş ‹Š %ß Œ‹ %à Œ %á  %â  %ã  %å ‘ %æ ’‘ %ç “’ %è ”“ %é •” %ê –• %ì —– %í ˜— %î %ï ™˜ %ğ š™ %ñ ›š %ò œ› %ó œ %õ %ö  %÷ Ÿ %ø  Ÿ %ù ¡  %ú ¢¡ %û £¢ %ı ¤£ %ş ¥¤ %ÿ % % % % ¦¥ %	 §¦ %
 ¨§ % ©¨ % ª© % «ª % ¬« % ­¬ % ®­ % ¯® % °¯ % ±° % ²± % ³² % ´³ % µ´ % % % % %  ¶µ %! ·¶ %" ¸· %# ¹¸ %$ º¹ %% »º %& ¼» %' ½¼ %( ¾½ %) ¿¾ %* À¿ %+ ÁÀ %, ÂÁ %- ÃÂ %. ÄÃ %/ %0 ÅÄ %1 ÆÅ %2 ÇÆ %3 ÈÇ %4 ÉÈ %5 ÊÉ %6 ËÊ %7 ÌË %8 ÍÌ %9 ÎÍ %: ÏÎ %; ĞÏ %< ÑĞ %= ÒÑ %>~Å —	  !Ó  % Ôã !Õ á -áÿÖ â]âá	F Êš;   O 8ã%%	 !× âiâú â -âÿØ ã]ãâ%ã ÙâbãâÄ ˜ üa$/ùÃ ™	 ø ‹ %ã -ãÿÚ Â ˜  -÷ÿÛ  —  ‹ %(ã ÜÒ 2ãÿİ ãaãÀã ŞâbãâT ™
 ãaã!ãaã#aö%%) !ß' ãaã)%.~%6%8|+¿.â'4ôâ, !à. â -âÿá0 ã^ãâô2%9 yâ4   %? !ß' ãaã5%J%K "ã7   ™  !ã9  ‹
  äÓ%Tà æÔã åâ çÕßãáe* âáó -ãÿè; ã6 2ãÿé=  êÖ%~% ì×ã ëâãáóàe* âá%€ îØã íâãáóàe* âá%ã zï? % 2ãÿğ@ ~%ƒ~%„à òÙã ñâ óÚßãáe* âá% yôB   %~%‘à öÛã õâ ÷Üßãáe* âá%”—ã -ãÿèC ã øİ 2ãÿùE —ã -ãÿèC ã úŞ 2ãÿûG —ã -ãÿèC ã üß 2ãÿıI —ã -ãÿèC ã şà 2ãÿÿK —ã -ãÿèC ã  á 2ãÿM  !O ã âbãâQ%š yS   %› â -âÿT ã İá^ãâáV% yX   %¦ yY   %§ 	â%ª 
ãâbñâZ%« ä%¬|\%­ å%® æ%¯ ç%° è%± é%µ ê%¶%· !] %¸ !_ %¹%º ë%» ì%¿ í%À î%Á ï%Âà ğã âãáe* âá%Ã ñ%Ä !O ã âbãâa%Åà òã âãáe* âá%Æà  óã âãáe* âá%Ç%Èà "ôã !âãáe* âá%Ìã -ãÿ#c %ÍÍ ˜/ Íã -ãÿ$e  ˜ Íã -ãÿ%g  ˜ Íã -ãÿ&i x%Î 'õâbñâk%Ïà )öã (â *÷ß +øŞãáe* âáïà -ùã ,âãáe* âá%×à /úã .â 0ûß 1üŞ 2ıİ 3şÜ 4ÿÛ 5 Ú 6Ùãáe* â
á%Ù 7%Ú 8%Û :ã 9âãáóàe* âá%ÜÜã z;m % 2ãÿğn  z<p %%İ z=q %%Ş ?ã >âãáóàe* âá%ääã z@r % 2ãÿğs  Bã Aâ Cßãáóàe* âá%å Eã Dâãáóàe* âá%ææâbòâu%çæã zFw % 2ãÿğx  zGz %%è I	ã Hâãáóàe* âá%ééã zJ{ % 2ãÿğ|  L
ã Kâ Mßãáóàe* âá%ë Oã Nâ Pß QŞãáóàe* âá%î Sã Râ Tßãáóàe* âá%ó Vã Uâãáóàe* âá%ô Xã Wâãáóàe* âá%õ Zã Yâ [ßãáóàe* âá%ö ]ã \âãáóàe* âá%ø _ã ^âãáóàe* âá%ú z`~ %%û bã aâãáóàe* âá%ü dã câãáóàe* âá%ÿ fã eâãáóàe* âá %  zg % % iã hâãáóàe* âá % ã zj€ % 2ãÿğ  lã kâãáóàe* âá % ã zmƒ % 2ãÿğ„  zn† % %	 pã oâãáóàe* âá %
 
ã zq‡ % 2ãÿğˆ  sã râ tßãáóàe* âá % zuŠ % % v % -îÿè‹ ã 2ãÿw  y ã xâãáîàe* âá %à {!ã zâãáe* âáí }"ã |âãáíàe* âá %à #ã ~âãáe* âá %! €$ %"à ‚%ã âãáe* âá %# yƒ   â -âÿ„ ã …á^ãâá’ %$ †&âbñâ” %'à ˆ'ã ‡âãáe* âá %*à Š(ã ‰â ‹)ß Œ*Ş +İ ,Üãáe* âá %;à -ã âãáe* âá %> %? %@ !_ âïiïâ– %A ‘.ãaã˜ }’š )· }“› )¶à •/ã ”â –0ßãáe* âá %Cà ˜1ã —â ™2ßãáe* âá %Eà ›3ã šâ œ4ß 5Ş 6İ Ÿ7Ü  8Ûãáe* âá %Fà ¢9ã ¡â £:ß ¤;Ş ¥<İãáe* âá %H ‚¦ã%ß ¨=â §á ©>Ş ª?İâàe* áà%ã %Pà ¬@ã «â ­Aß ®BŞãáe* âá %Qà °Cã ¯â ±Dßãáe* âá %R ³Eã ²â ´Fßãáóàe* âá %S Sã zµœ % 2ãÿğ à ·Gã ¶âãáe* âá %T ¸H %U ¹I %V ºJ %W ¼Kã »â ½Lßãáóàe* âá %Xà ¿Mã ¾âãáe* âá %\ \ã zÀŸ %â }Á  )áãiãâ¡ %] ÃNâ Âáâàóße* ábòâ£ %^ ÅOã Äâãáóàe* âá %_ ÇPã Æâãáóàe* âá %` ÉQã Èâãáóàe* âá %a aã zÊ¥ % 2ãÿğ¦  ÌRã Ëâãáóàe* âá %b ÎSã Íâãáóàe* âá %c cã zÏ¨ % 2ãÿğ© à ÑTã Ğâãáe* âáêà ÓUã Òâãáe* âáéà ÕVã Ôâãáe* âáè :âáêiêâ« %e ÷âêiêâ­ %f jâdáéiéâ¯ %gà ×Wã Öâãáe* ââáiãâ± %h ßâáêiêâ³ %i äâáêiêâµ %j øâêiêâ· %k <âêiêâ¹ %l ·âáêiêâ» %m 9âêiêâ½ %n qâêiêâ¿ %o âêiêâÁ %p 	âêiêâÃ %q âêiêâÅ %r âêiêâÇ %s ÎâêiêâÉ %t ÍâêiêâË %u ÏâêiêâÍ %v Ù âêiêâÏ %w âêiêâÑ %x2÷/!â zØÓ %áèièâÔ %ykN!âêiêâÖ %zŠnâáêiêâØ %{3ç6âêiêâÚ %|Áó5âêiêâÜ %} `âêiêâŞ %~ 7âáéiéâà % 'âêiêââ %€ â zÙä %áèièâå % Ë âêiêâç %‚Tâêiêâé %ƒà ÛXã Úâãáe* âá %„ }Üë ) %‹ }İì ) %Œ ŞY % ßZ % à[ %’ á\ %“ â] %” ã^ %• ä_ %– å` %— æa %˜ çb %™ èc %š éd %›à ëeã êâ ìfß ígŞ îhİãáe* âá %œ ïi % ğj % ñk %Ÿ òl %  óm %¡ ôn %¢ õo %£ }öí ) %¤ !O ã ÷âbãâî %¥ ùpã øâãáóàe* âá %§ ûqã úâãáóàe* âá %¨ ırã üâ şsßãáóàe* âá %©  tã ÿâãáóàe* âá %ª uã âãáóàe* âá %« vã âãáóàe* âá %¬ wã âãáóàe* âá %­ xã âãáóàe* âá %® 
yã 	â zßãáóàe* âá %¯ zğ % %° {ã âãáóàe* âá %± ±ã zñ % 2ãÿğò  |ã âãáóàe* âá %² }ã â ~ßãáóàe* âá %³ zô % %´ ã â €ßãáóàe* âá %µ µã zõ % 2ãÿğö  ã âãáóàe* âá %¶ ¶âbòâø %· ¶ã zú % 2ãÿğû à ‚ã â ƒß  „Şãáe* âá %¼à "…ã !â #†ß $‡Ş %ˆİ &‰Ü 'ŠÛ (‹Ú )ŒÙãáe* â
á %½ * %¾ + %¿ , %À }-ı ) %Ã }.ş ) %Å / %È 0‘ %Éà 2’ã 1âãáe* âá %Ë 3“ %Ì 4” %Í 5• %Î 6– %Ï ‚7ã% !9ÿ ß :—â 8áâàe* áà%ã %Ğ %Ñ %Òâï iïÿâÿ  %Ó ;˜ %Ô <™ã aãÿ =š %Õ >› %Ö ?œ %× @ %Øà Bã Aâãáe* âá %Ùà DŸã Câãáe* âá %ß E  %ä F¡ %åà H¢ã Gâãáe* âá %é J£â Iáâàóße* á bòÿâÿ %ìà L¤ã Kâãáe* âá %ò %ó }M)ãsâ N 7ãÿâÿ 	sâ O 7ãÿâÿ 
ã %ô õã -ãÿèã P¥ 2ãÿQ õã -ãÿèã R¦ 2ãÿS õã -ãÿèã T§ 2ãÿU õã -ãÿèã V¨ 2ãÿW Y©ã Xâãáóàe* âá %ø øã zZ% 2ãÿğ [ª %ù ]«ã \â ^¬ß _­Şãáîàe* âá %ÿ `® %  a¯ % c°ã bâãáóàe* âá %	 	ã zd% 2ãÿğ f±ã eâãáóàe* âá %
 
ã zg% 2ãÿğ i²ã hâ j³ß k´Şãáóàe* âá %à mµã lâãáe* âá %à o¶ã nâ p·ß q¸Şãáe* âá % s¹ã râãáóàe* âá % }t) % }M )ãsâ u 7ãÿâÿ !sâ v 7ãÿâÿ #sâ w 7ãÿâÿ %ã % yºã xâ z»ßãáóàe* âá % ã z{'% 2ãÿğ( }¼ã |âãáóàe* âá % ½ã ~âãáóàe* âá % ¾ã €â ‚¿ßãáóàe* âá % „Àã ƒâãáóàe* âá % †Áã …âãáóàe* âá % z‡*% % â -âÿˆ+ã ‰Âá ^ãÿâÿáÿ- %  Šâ -âÿ‹/ã ]ãÿâÿ1 %! bğÿìÿ3 %" bğÿëÿ5 %# Ãã Œâãáóàe* âá %&à Äã âãáe* âá %+ Å %, ‘Æ %- “Çã ’â ”Èßãáóàe* âá %. –Éã •â —Êßãáóàe* âá %/ ™Ëã ˜âãáóàe* âá %0 ›Ìã šâãáóàe* âáç bòÿçÿ7 %1 zœ9% 2çÿğ: !<ß z>Şß ißÿŞÿ ?{á -áÿŸAâ  Íà ^âÿáÿàÿCâ -âÿ¡Eã ¢Îá ^ãÿâÿáÿG ã âá bâÿáÿIâ bãÿâÿK %4à ¤Ïã £â ¥Ğßãáe* âá %7 bğÿìÿMâ -âÿ¡Oã ¦Ñá ^ãÿâÿáÿQ bğÿëÿSâ -âÿ¡Uã §Òá ^ãÿâÿáÿWà ©Óã ¨âãáe* âá %: «Ôã ªâ ¬Õßãáóàe* âá %; ®Öã ­â ¯×ßãáóàe* âáæ bòÿæÿY %< z°[% 2æÿğ\ ±Ø %=à ³Ùã ²âãáe* âá %F }´^) %G µÚ %H ¶Û %I ·Ü %J ¸İ %K ¹Ş %L ºß %M »à %N ¼á %O ¾âã ½â ¿ãß ÀäŞãáóàe* âá %Q Âåã Áâ Ãæß ÄçŞãáóàe* âá %R Rã zÅ_% 2ãÿğ` Çèã Æâ Èéßãáóàe* âá %S Sã zÉb% 2ãÿğc Ëêã Êâãáóàe* âá %T Tã zÌe% 2ãÿğf Îëã Íâãáóàe* âá %U Uã zÏh% 2ãÿği Ñìã Ğâ Òíß ÓîŞ Ôïİãáóàe* âá %V Öğã Õâãáóàe* âá %W Wã z×k% 2ãÿğl Ùñã Øâ Úòß ÛóŞ Üôİ İõÜãáóàe* âá %X Xã zŞn% 2ãÿğo zßq% %Y %Z zàr% %c áö %d â÷ %e ãø %f äù %g åú %k æû %n çü %o èı %pà êşã éâãáe* âá %z ëÿ %{ ì  %| í %}õ —	  aöÿsà ïã îâãáe* âá %à ñã ğâãáe* âá %†à óã òâãáe* âá %‡ ô %ˆà öã õâãáe* âá %‹ }÷u) % ø % ù %‘ ú	 %’ }ûv) %” ı
ã üâ şßãáóàe* âá %™  ã ÿâãáóàe* âá %£ £ã zw% 2ãÿğx â -âÿzã İá ^ãÿâÿáÿ| %¤ œà ã â ß Şãáe* âá %´ }~) %µ }) %¶ }	€) %·(ã 
â bãÿâÿ %ºà ã â ßãáe* âá %¼ zƒ% %½ œà ã â ßãáe* âá %¾  %¿  %À œà ã â ß Şãáe* âá %Ã }„)ã  3ãÿ…  3ãÿ‡  3ãÿ‰  3ãÿ ‹ ! 3ãÿ"ã %Ä œà $ã #â %ßãáe* âá %Å &  %Æ '! %Ç (" %È z)%ãâ ´á ÊàZßŞá iáÿàÿ  6ãÿâÿ’â ´á ØàZßŞá iáÿàÿ ” 6ãÿâÿ’â ´á Ôà<ßŞá iáÿàÿ – 6ãÿâÿ’â ´á Pà ßŞá iáÿàÿ ˜ 6ãÿâÿ’â ´á @àdßŞá iáÿàÿ š 6ãÿâÿ’â ´á @à2ßŞá iáÿàÿ œ 6ãÿâÿ’â ´á ,à XßŞá iáÿàÿ  6ãÿâÿ’â ´á ,à ú ßŞá iáÿàÿ   6ãÿâÿ’â ´á ú à ú ßŞá iáÿàÿ ¢ 6ãÿâÿ’	â ´á ê à<ßŞá iáÿàÿ ¤ 6ãÿâÿ’
â ´á È à È ßŞá iáÿàÿ ¦ 6ãÿâÿ’â ´á ´ à – ßŞá iáÿàÿ ¨ 6ãÿâÿ’â ´á   à XßŞá iáÿàÿ ª 6ãÿâÿ’â ´á}à}ßŞá iáÿàÿ ¬ 6ãÿâÿ’â ´áxà XßŞá iáÿàÿ ® 6ãÿâÿ’â ´áxà ğ ßŞá iáÿàÿ ° 6ãÿâÿ’â ´áxàxßŞİá iáÿàÿ ² 6ãÿâÿ’ãå z*´%ãâ /åÿµ 6ãÿâÿ·â /åÿ¹ 6ãÿâÿ·â /åÿ» 6ãÿâÿ·â /åÿ½ 6ãÿâÿ·â /åÿ¿ 6ãÿâÿ·â /åÿÁ 6ãÿâÿ·â /åÿÃ 6ãÿâÿ·â /åÿÅ 6ãÿâÿ·â
 /åÿÇ 6ãÿâÿ·	â /åÿÉ 6ãÿâÿ·
â /åÿË 6ãÿâÿ·â /åÿÍ 6ãÿâÿ·â /åÿÏ 6ãÿâÿ·â /åÿÑ 6ãÿâÿ·â /åÿÓ 6ãÿâÿ·â	 /åÿÕ 6ãÿâÿ·â /åÿ× 6ãÿâÿ·ã %É +# %Ê ,$ %Ë -% %Ì .& %Í /' %Î 0( %Ï 1) %Ğ 2* %Ñ 3+ %Ò 4, %Ó 5- %Ôà 7.ã 6â 8/ß 90Ş :1İ ;2Üãáe* âá %ä }MÙ)ãsâ }MÚ)ásà <3 7áÿàÿÛ	sà =4 7áÿàÿİ
sà >5 7áÿàÿßsà ?6 7áÿàÿásà @7 7áÿàÿã<sà A8 7áÿàÿåEsà B9 7áÿàÿçFsà C: 7áÿàÿéá 7ãÿâÿ ësâ }Mí)ásà D; 7áÿàÿîsà E< 7áÿàÿğá 7ãÿâÿ òsâ }Mô)ásà F= 7áÿàÿõsà G> 7áÿàÿ÷sà H? 7áÿàÿùá 7ãÿâÿ ûã %ë zIı% %ïà K@ã Jâãáe* âá %ôà MAã Lâãáe* âá %ö NB %ü PCã Oâãáîàe* âá %  QD % }Rş)ã SE 3ãÿTÿ UF 3ãÿV WG 3ãÿXã % ZHã Yâãáîàe* âá % \Iâ [áâàóße* á bòÿâÿ % ]J % }^)ã _K 3ãÿT `L 3ãÿV
 aM 3ãÿXã % cNã bâãáîàe* âá % dO %à fPã eâãáe* âá % | % % | % !<ã iãÿúÿ   %ÿ %  %0 gQã hâ  š  ‹ á iRà `ãÿâÿ ª   q˜m  S  @  €€€ 
 €€ € `:` 0	à 
`¹ ³€`À &0'L€€
 €`.0's&Ì¹ sÎ&Ì¹ ³€&@ 0's9	€€¹ s€ € € € € € `@ @ @ “ `ÎI s.0'0`N€ &HP ,° Ì&Ì&S	$`.0€€€ `.` “€  € 
€ 
€ &Ì¹ s¹ s¹ s9	`Î¹ 0'“€9€€I @ @ @ @ @ 0à     &S	$@	$@	$`J L	$@	&“€€É 00'  ¹e    @    N1      áàı4R4ªäê’4  [2021,"r20230920","r20190131",null,null,null,null,".google.mg",null,null,null,[[[1082,null,null,[1]],[null,1130,null,[null,100]],[1270,null,null,[1]],[null,1032,null,[null,200],[[[12,null,null,null,4,null,"Android",["navigator.userAgent"]],[null,500]]]],[1247,null,null,[1]],[1252,null,null,[1]],[null,1224,null,[null,0.01]],[null,1159,null,[null,500]],[1122,null,null,[1]],[1207,null,null,[1]],[null,1263,null,[null,-1]],[null,1265,null,[null,-1]],[null,1264,null,[null,-1]],[null,66,null,[null,-1]],[null,65,null,[null,-1]],[1241,null,null,[1]],[1287,null,null,[1]],[1285,null,null,[1]],[1300,null,null,[1]],[null,null,null,[null,null,null,["en","de"]],null,1273],[1223,null,null,[1]],[null,null,null,[null,null,null,["44786015","44786016"]],null,1261],[1167,null,null,[1]],[1129,null,null,[1]],[1199,null,null,[1]],[1161,null,null,[1]],[null,1072,null,[null,0.75]],[1101,null,null,[1]],[1198,null,null,[1]],[1206,null,null,[1]],[1147,null,null,[1]],[1190,null,null,[1]],[null,1245,null,[null,3600]],[null,566560958,null,[null,30000]],[null,506864295,null,[null,300]],[null,508040914,null,[null,100]],[null,547455356,null,[null,49]],[null,null,null,[null,null,null,["1"]],null,556791602],[529362570,null,null,[1]],[null,null,549581487,[null,null,"#1A73E8"]],[null,null,549581486,[null,null,"#FFFFFF"]],[null,564316610,null,[null,0.2]],[null,561668774,null,[null,0.1]],[null,469675170,null,[null,30000]],[567362967,null,null,[1]],[null,1085,null,[null,5]],[null,63,null,[null,30]],[null,1080,null,[null,5]],[1086,null,null,[1]],[null,1027,null,[null,10]],[null,57,null,[null,120]],[null,1079,null,[null,5]],[null,1050,null,[null,30]],[null,58,null,[null,120]],[10005,null,null,[1]],[1033,null,null,[1]],[555237685,null,null,[1]],[null,472785970,null,[null,500]],[null,null,null,[null,null,null,["A7CQXglZzTrThjGTBEn1rWTxHOEtkWivwzgea+NjyardrwlieSjVuyG44PkYgIPGs8Q9svD8sF3Yedn0BBBjXAkAAACFeyJvcmlnaW4iOiJodHRwczovL2RvdWJsZWNsaWNrLm5ldDo0NDMiLCJmZWF0dXJlIjoiUHJpdmFjeVNhbmRib3hBZHNBUElzIiwiZXhwaXJ5IjoxNjk1MTY3OTk5LCJpc1N1YmRvbWFpbiI6dHJ1ZSwiaXNUaGlyZFBhcnR5Ijp0cnVlfQ==","A3vKT9yxRPjmXN3DpIiz58f5JykcWHjUo/W7hvmtjgh9jPpQgem9VbADiNovG8NkO6mRmk70Kex8/KUqAYWVWAEAAACLeyJvcmlnaW4iOiJodHRwczovL2dvb2dsZXN5bmRpY2F0aW9uLmNvbTo0NDMiLCJmZWF0dXJlIjoiUHJpdmFjeVNhbmRib3hBZHNBUElzIiwiZXhwaXJ5IjoxNjk1MTY3OTk5LCJpc1N1YmRvbWFpbiI6dHJ1ZSwiaXNUaGlyZFBhcnR5Ijp0cnVlfQ==","A4A26Ymj79UVY7C7JGUS4BG1s7MdcDokAQf/RP0paks+RoTYbXHxceT/5L4iKcsleFCngi75YfNRGW2+SpVv1ggAAACLeyJvcmlnaW4iOiJodHRwczovL2dvb2dsZXRhZ3NlcnZpY2VzLmNvbTo0NDMiLCJmZWF0dXJlIjoiUHJpdmFjeVNhbmRib3hBZHNBUElzIiwiZXhwaXJ5IjoxNjk1MTY3OTk5LCJpc1N1YmRvbWFpbiI6dHJ1ZSwiaXNUaGlyZFBhcnR5Ijp0cnVlfQ==","As0hBNJ8h++fNYlkq8cTye2qDLyom8NddByiVytXGGD0YVE+2CEuTCpqXMDxdhOMILKoaiaYifwEvCRlJ/9GcQ8AAAB8eyJvcmlnaW4iOiJodHRwczovL2RvdWJsZWNsaWNrLm5ldDo0NDMiLCJmZWF0dXJlIjoiV2ViVmlld1hSZXF1ZXN0ZWRXaXRoRGVwcmVjYXRpb24iLCJleHBpcnkiOjE3MTk1MzI3OTksImlzU3ViZG9tYWluIjp0cnVlfQ==","AgRYsXo24ypxC89CJanC+JgEmraCCBebKl8ZmG7Tj5oJNx0cmH0NtNRZs3NB5ubhpbX/bIt7l2zJOSyO64NGmwMAAACCeyJvcmlnaW4iOiJodHRwczovL2dvb2dsZXN5bmRpY2F0aW9uLmNvbTo0NDMiLCJmZWF0dXJlIjoiV2ViVmlld1hSZXF1ZXN0ZWRXaXRoRGVwcmVjYXRpb24iLCJleHBpcnkiOjE3MTk1MzI3OTksImlzU3ViZG9tYWluIjp0cnVlfQ=="]],null,1934],[1957,null,null,[1]],[null,1972,null,[]],[485990406,null,null,[]]],[[12,[[40,[[21065724],[21065725,[[203,null,null,[1]]]]],[4,null,9,null,null,null,null,["LayoutShift"]],71],[10,[[31061690],[31061691,[[83,null,null,[1]],[84,null,null,[1]]]]],null,61],[10,[[44769661],[44769662,[[1973,null,null,[1]]]]]]]],[13,[[500,[[31061692],[31061693,[[77,null,null,[1]],[78,null,null,[1]],[85,null,null,[1]],[80,null,null,[1]],[76,null,null,[1]]]]],[4,null,6,null,null,null,null,["31061691"]]],[1000,[[31067146,null,[4,null,70,null,null,null,null,["browsing-topics"]]]]],[1000,[[31067147,null,[2,[[4,null,70,null,null,null,null,["run-ad-auction"]],[4,null,70,null,null,null,null,["join-ad-interest-group"]]]]]]],[1000,[[31067148,null,[4,null,70,null,null,null,null,["attribution-reporting"]]]]],[1000,[[31067672,null,[2,[[4,null,69,null,null,null,null,["browsing-topics"]],[1,[[4,null,70,null,null,null,null,["browsing-topics"]]]]]]]]],[1000,[[31067673,null,[2,[[4,null,69,null,null,null,null,["join-ad-interest-group"]],[1,[[4,null,70,null,null,null,null,["join-ad-interest-group"]]]]]]]]],[1000,[[31067674,null,[2,[[4,null,69,null,null,null,null,["run-ad-auction"]],[1,[[4,null,70,null,null,null,null,["run-ad-auction"]]]]]]]]],[1000,[[31067675,null,[2,[[4,null,69,null,null,null,null,["attribution-reporting"]],[1,[[4,null,70,null,null,null,null,["attribution-reporting"]]]]]]]]],[1000,[[31068556,null,[4,null,70,null,null,null,null,["shared-storage"]]]]],[1000,[[31068557,null,[2,[[4,null,69,null,null,null,null,["shared-storage"]],[1,[[4,null,70,null,null,null,null,["shared-storage"]]]]]]]]],[50,[[44800311,null,[4,null,70,null,null,null,null,["attribution-reporting"]]],[44800312,null,[4,null,70,null,null,null,null,["attribution-reporting"]]]],null,77]]],[10,[[50,[[31067422],[31067423,[[null,1032,null,[]]]],[44776369],[44792510]],[3,[[4,null,8,null,null,null,null,["gmaSdk.getQueryInfo"]],[4,null,8,null,null,null,null,["webkit.messageHandlers.getGmaQueryInfo.postMessage"]],[4,null,8,null,null,null,null,["webkit.messageHandlers.getGmaSig.postMessage"]]]],69],[100,[[31076838],[31076839,[[542046278,null,null,[1]]]]]],[50,[[31076994],[31076995,[[1229,null,null,[1]]]],[31076996,[[1230,null,null,[1]]]],[31076997,[[1231,null,null,[1]]]],[31076998,[[1230,null,null,[1]],[1229,null,null,[1]],[1231,null,null,[1]]]]],null,87],[50,[[31077221],[31077222,[[1292,null,null,[1]]]]]],[100,[[31077327],[31077328,[[555237685,null,null,[]]]]]],[100,[[31077701],[31077702,[[1299,null,null,[1]]]]]],[100,[[31077969],[31077970,[[10009,null,null,[1]]]]]],[10,[[31077971],[31077972,[[1301,null,null,[1]]]]]],[1000,[[31077997,[[null,null,14,[null,null,"31077997"]]],[6,null,null,null,6,null,"31077997"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[1000,[[31077998,[[null,null,14,[null,null,"31077998"]]],[6,null,null,null,6,null,"31077998"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[10,[[31078019],[31078020]]],[1000,[[31078048,[[null,null,14,[null,null,"31078048"]]],[6,null,null,null,6,null,"31078048"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[1000,[[31078049,[[null,null,14,[null,null,"31078049"]]],[6,null,null,null,6,null,"31078049"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[1000,[[31078086,[[null,null,14,[null,null,"31078086"]]],[6,null,null,null,6,null,"31078086"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[1000,[[31078087,[[null,null,14,[null,null,"31078087"]]],[6,null,null,null,6,null,"31078087"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[1000,[[31078114,[[null,null,14,[null,null,"31078114"]]],[6,null,null,null,6,null,"31078114"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[1000,[[31078115,[[null,null,14,[null,null,"31078115"]]],[6,null,null,null,6,null,"31078115"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[1000,[[31078134,[[null,null,14,[null,null,"31078134"]]],[6,null,null,null,6,null,"31078134"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[1000,[[31078135,[[null,null,14,[null,null,"31078135"]]],[6,null,null,null,6,null,"31078135"]]],[4,null,55],63,null,null,null,null,null,null,null,null,2],[10,[[31078142],[31078143,[[562770592,null,null,[1]]]]]],[10,[[31078144],[31078145,[[45414947,null,null,[1]]]]]],[null,[[31078148],[31078149,[[1275,null,null,[1]]]]]],[1,[[42531513],[42531514,[[316,null,null,[1]]]]]],[1,[[42531644],[42531645,[[368,null,null,[1]]]],[42531646,[[369,null,null,[1]],[368,null,null,[1]]]]]],[50,[[42531705],[42531706]]],[1,[[42532242],[42532243,[[1256,null,null,[1]],[290,null,null,[1]]]]]],[1,[[42532262],[42532263,[[null,1263,null,[null,16]]]],[42532264,[[null,1263,null,[null,4294967296]]]],[42532265,[[null,1265,null,[null,60]],[null,1264,null,[null,0.2]],[1266,null,null,[1]]]],[42532266,[[null,1263,null,[null,4294967296]],[null,1265,null,[null,60]],[null,1264,null,[null,0.2]],[1266,null,null,[1]]]],[42532267,[[null,1263,null,[null,16]],[null,1265,null,[null,60]],[null,1264,null,[null,0.2]],[1266,null,null,[1]]]],[42532268,[[1266,null,null,[1]]]]]],[1,[[42532360],[42532361,[[1260,null,null,[1]],[1291,null,null,[1]]]]],null,90],[1,[[42532362],[42532363]]],[50,[[42532402],[42532403,[[1267,null,null,[1]],[1268,null,null,[1]]]]]],[null,[[42532404],[42532405]]],[1,[[44719338],[44719339,[[334,null,null,[1]],[null,54,null,[null,100]],[null,66,null,[null,10]],[null,65,null,[null,1000]]]]]],[10,[[44776368],[44779257],[44784478]],[3,[[4,null,8,null,null,null,null,["gmaSdk.getQueryInfo"]],[4,null,8,null,null,null,null,["webkit.messageHandlers.getGmaQueryInfo.postMessage"]],[4,null,8,null,null,null,null,["webkit.messageHandlers.getGmaSig.postMessage"]]]],69],[10,[[44785292],[44785293,[[1239,null,null,[1]]]]]],[10,[[44785294],[44785295]]],[1,[[44795552],[44795553,[[1260,null,null,[1]]]]],null,90],[1,[[44795554],[44795555]]],[100,[[44795921],[44795922,[[1222,null,null,[1]]]],[44798934,[[1222,null,null,[1]]]]]],[10,[[44800658],[44800659,[[1185,null,null,[1]]]]],null,76],[50,[[44801484],[44801485,[[1298,null,null,[1]]]]]],[1,[[44801778],[44801779,[[506914611,null,null,[1]]]]],[4,null,55]],[1,[[44801992],[44801993,[[1289,null,null,[1]],[1288,null,null,[1]]]]]],[1000,[[44802674,[[506852289,null,null,[1]]],[12,null,null,null,2,null,"smitmehta\\.com/"]]],[4,null,55]],[10,[[44803789],[44803790,[[1233,null,null,[1]],[1185,null,null,[1]]]]],null,76],[50,[[44803791],[44803793,[[1185,null,null,[1]]]]],null,76],[10,[[44803792],[44803794,[[1185,null,null,[1]]]]],null,76]]],[17,[[25,[[31077857],[31077859,[[557143911,null,null,[1]],[541943501,null,null,[1]],[null,550718588,null,[null,250]],[null,1245,null,[null,600]]]]],null,null,null,null,null,325,null,120],[48,[[44796896,[[541943501,null,null,[1]],[null,1245,null,[null,600]]]]],[2,[[4,null,55],[7,null,null,15,null,20230711]]],null,null,null,null,440,null,120,1],[504,[[44796897,[[541943501,null,null,[1]],[null,1245,null,[null,600]]],[4,null,71,null,null,null,null,["120","14"]]]],[2,[[4,null,55],[7,null,null,15,null,20230711]]],null,null,null,null,488,null,120,1],[1,[[44797663,[[null,506871937,null,[null,44797663]],[1120,null,null,[1]]]],[44797664,[[null,506871937,null,[null,44797664]],[160889229,null,null,[1]],[1120,null,null,[1]]]]],[4,null,55],null,null,null,null,300,null,123],[24,[[44798321,[[null,506871937,null,[null,44798321]]]],[44798322,[[545453532,null,null,[1]],[null,506871937,null,[null,44798322]],[1120,null,null,[1]]]],[44798323,[[null,506871937,null,[null,44798323]],[1120,null,null,[1]]]]],[4,null,55],null,null,null,null,null,null,123],[14,[[44801877]],[2,[[4,null,55],[5,null,8,null,null,null,null,["localStorage"]],[4,null,8,null,null,null,null,["localStorage"]],[7,null,null,15,null,20230524]]],null,null,null,null,null,null,120,1],[14,[[44801878]],[2,[[4,null,55],[5,null,8,null,null,null,null,["localStorage"]],[4,null,8,null,null,null,null,["localStorage"]],[7,null,null,15,null,20230524]]],null,null,null,null,15,null,120,1],[139,[[44801879]],[2,[[4,null,55],[5,null,8,null,null,null,null,["localStorage"]],[4,null,8,null,null,null,null,["localStorage"]],[7,null,null,15,null,20230524]]],null,null,null,null,30,null,120,1],[14,[[44801880]],[2,[[4,null,55],[5,null,8,null,null,null,null,["localStorage"]],[4,null,8,null,null,null,null,["localStorage"]],[7,null,null,15,null,20230524]]],null,null,null,null,170,null,120,1],[139,[[44801881]],[2,[[4,null,55],[5,null,8,null,null,null,null,["localStorage"]],[4,null,8,null,null,null,null,["localStorage"]],[7,null,null,15,null,20230524]]],null,null,null,null,185,null,120,1],[10,[[44802850,[[null,506871937,null,[null,44802850]],[1120,null,null,[1]]]],[44802851,[[563731719,null,null,[1]],[null,506871937,null,[null,44802851]],[1120,null,null,[1]]]]],[4,null,55],null,null,null,null,360,null,123],[1,[[44803132,[[null,506871937,null,[null,44803132]],[1120,null,null,[1]]]],[44803133,[[554474127,null,null,[1]],[null,506871937,null,[null,44803133]],[1120,null,null,[1]]]]],[4,null,55],null,null,null,null,430,null,123],[10,[[44803138,[[null,506871937,null,[null,44803138]],[1120,null,null,[1]]]],[44803139,[[558761323,null,null,[1]],[null,506871937,null,[null,44803139]],[1120,null,null,[1]]]]],[4,null,55],null,null,null,null,450,null,123],[10,[[44803467,[[null,506871937,null,[null,44803467]],[1120,null,null,[1]]]],[44803468,[[561639568,null,null,[1]],[562711461,null,null,[1]],[null,506871937,null,[null,44803468]],[1120,null,null,[1]]]],[44803469,[[564316609,null,null,[1]],[561639568,null,null,[1]],[null,564316610,null,[null,0.3]],[562711461,null,null,[1]],[null,506871937,null,[null,44803469]],[1120,null,null,[1]]]]],[4,null,55],null,null,null,null,470,null,123]]],[11,[[1000,[[31078150,null,[4,null,6,null,null,null,null,["31078148"]]]],[4,null,8,null,null,null,null,["__gpp"]],88,null,null,null,null,null,null,null,null,9],[1000,[[31078151,null,[4,null,6,null,null,null,null,["31078149"]]]],[4,null,8,null,null,null,null,["__gpp"]],88,null,null,null,null,null,null,null,null,9]]]],null,null,[null,"1000",1,"1000"]],[null,[],null,null,null,null,null,null,"ca-pub-4830067276561639"],null,null,1,"5e4daa4b14d4f7c9e425384ce33f40fc.safeframe.googlesyndication.com",89562434,[44759875,44759926,44759837]] •Di(             Ã-ø Ä!ÂÁ_ùø÷öÅª    Sbˆ`          Ibşÿÿÿ    p… —b      @ e            M1      
							ØA—Eoúô   A'’0Š     REºËVò"
å$«Æ…ƒšdú¾·èüS¡©æUÏØA—Eoúô                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       gs;
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
