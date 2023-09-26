/* Copyright (c) 2006, 2016, Oracle and/or its affiliates.
   Copyright (c) 2010, 2020, MariaDB Corporation.

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

#ifndef MYSQLD_INCLUDED
#define MYSQLD_INCLUDED

#include "sql_basic_types.h"			/* query_id_t */
#include "sql_mode.h"                           /* Sql_mode_dependency */
#include "sql_plugin.h"
#include "sql_bitmap.h"                         /* Bitmap */
#include "my_decimal.h"                         /* my_decimal */
#include "mysql_com.h"                     /* SERVER_VERSION_LENGTH */
#include "my_counter.h"
#include "mysql/psi/mysql_file.h"          /* MYSQL_FILE */
#include "mysql/psi/mysql_socket.h"        /* MYSQL_SOCKET */
#include "sql_list.h"                      /* I_List */
#include "sql_cmd.h"
#include <my_rnd.h>
#include "my_pthread.h"
#include "my_rdtsc.h"

class THD;
class CONNECT;
struct handlerton;
class Time_zone;

struct scheduler_functions;

typedef struct st_mysql_show_var SHOW_VAR;

/* Bits from testflag */
#define TEST_PRINT_CACHED_TABLES 1U
#define TEST_NO_KEY_GROUP	 2U
#define TEST_MIT_THREAD		4U
#define TEST_BLOCKING		8U
#define TEST_KEEP_TMP_TABLES	16U
#define TEST_READCHECK		64U	/**< Force use of readcheck */
#define TEST_NO_EXTRA		128U
#define TEST_CORE_ON_SIGNAL	256U	/**< Give core if signal */
#define TEST_SIGINT		1024U	/**< Allow sigint on threads */
#define TEST_SYNCHRONIZATION    2048U   /**< get server to do sleep in
                                           some places */

/* Keep things compatible */
#define OPT_DEFAULT SHOW_OPT_DEFAULT
#define OPT_SESSION SHOW_OPT_SESSION
#define OPT_GLOBAL SHOW_OPT_GLOBAL

extern MYSQL_PLUGIN_IMPORT MY_TIMER_INFO sys_timer_info;

/*
  Values for --slave-parallel-mode
  Must match order in slave_parallel_mode_typelib in sys_vars.cc.
*/
enum enum_slave_parallel_mode {
  SLAVE_PARALLEL_NONE,
  SLAVE_PARALLEL_MINIMAL,
  SLAVE_PARALLEL_CONSERVATIVE,
  SLAVE_PARALLEL_OPTIMISTIC,
  SLAVE_PARALLEL_AGGRESSIVE
};

/* Function prototypes */
void kill_mysql(THD *thd);
void close_connection(THD *thd, uint sql_errno= 0);
void handle_connection_in_main_thread(CONNECT *thd);
void create_thread_to_handle_connection(CONNECT *connect);
void unlink_thd(THD *thd);
void refresh_status(THD *thd);
bool is_secure_file_path(char *path);
extern void init_net_server_extension(THD *thd);
extern void handle_accepted_socket(MYSQL_SOCKET new_sock, MYSQL_SOCKET sock);
extern void create_new_thread(CONNECT *connect);

extern void ssl_acceptor_stats_update(int sslaccept_ret);
extern int reinit_ssl();

extern "C" MYSQL_PLUGIN_IMPORT CHARSET_INFO *system_charset_info;
extern MYSQL_PLUGIN_IMPORT CHARSET_INFO *files_charset_info ;
extern MYSQL_PLUGIN_IMPORT CHARSET_INFO *national_charset_info;
extern MYSQL_PLUGIN_IMPORT CHARSET_INFO *table_alias_charset;

/**
  Character set of the buildin error messages loaded from errmsg.sys.
*/
extern CHARSET_INFO *error_message_charset_info;

extern CHARSET_INFO *character_set_filesystem;

extern MY_BITMAP temp_pool;
extern bool opt_large_files;
extern bool opt_update_log, opt_bin_log, opt_error_log, opt_bin_log_compress; 
extern uint opt_bin_log_compress_min_len;
extern my_bool opt_log, opt_bootstrap;
extern my_bool opt_backup_history_log;
extern my_bool opt_backup_progress_log;
extern my_bool opt_support_flashback;
extern ulonglong log_output_options;
extern ulong log_backup_output_options;
extern bool opt_disable_networking, opt_skip_show_db;
extern bool opt_skip_name_resolve;
extern bool opt_ignore_builtin_innodb;
extern my_bool opt_character_set_client_handshake;
extern my_bool debug_assert_on_not_freed_memory;
extern MYSQL_PLUGIN_IMPORT bool volatile abort_loop;
extern my_bool opt_safe_user_create;
extern my_bool opt_safe_show_db, opt_local_infile, opt_myisam_use_mmap;
extern my_bool opt_slave_compressed_protocol, use_temp_pool;
extern ulong slave_exec_mode_options, slave_ddl_exec_mode_options;
extern ulong slave_retried_transactions;
extern ulong transactions_multi_engine;
extern ulong rpl_transactions_multi_engine;
extern ulong transactions_gtid_foreign_engine;
extern ulong slave_run_triggers_for_rbr;
extern ulonglong slave_type_conversions_options;
extern my_bool read_only, opt_readonly;
extern MYSQL_PLUGIN_IMPORT my_bool lower_case_file_system;
extern my_bool opt_enable_named_pipe, opt_sync_frm, opt_allow_suspicious_udfs;
extern my_bool opt_secure_auth;
extern my_bool opt_require_secure_transport;
extern const char *current_dbug_option;
extern char* opt_secure_file_priv;
extern char* opt_secure_backup_file_priv;
extern size_t opt_secure_backup_file_priv_len;
extern my_bool sp_automatic_privileges, opt_noacl;
extern ulong use_stat_tables;
extern my_bool opt_old_style_user_limits, trust_function_creators;
extern uint opt_crash_binlog_innodb;
extern const char *shared_memory_base_name;
extern MYSQL_PLUGIN_IMPORT char *mysqld_unix_port;
extern my_bool opt_enable_shared_memory;
extern ulong opt_replicate_events_marked_for_skip;
extern char *default_tz_name;
extern Time_zone *default_tz;
extern char *my_bind_addr_str;
extern char *default_storage_engine, *default_tmp_storage_engine;
extern char *enforced_storage_engine;
extern char *gtid_pos_auto_engines;
extern plugin_ref *opt_gtid_pos_auto_plugins;
extern bool opt_endinfo, using_udf_functions;
extern my_bool locked_in_memory;
extern bool opt_using_transactions;
extern ulong current_pid;
extern double expire_logs_days;
extern ulong binlog_expire_logs_seconds;
extern my_bool relay_log_recovery;
extern uint sync_binlog_period, sync_relaylog_period, 
            sync_relayloginfo_period, sync_masterinfo_period;
extern ulong opt_tc_log_size, tc_log_max_pages_used, tc_log_page_size;
extern ulong tc_log_page_waits;
extern my_bool relay_log_purge, opt_innodb_safe_binlog, opt_innodb;
extern my_bool relay_log_recovery;
extern uint select_errors,ha_open_options;
extern ulonglong test_flags;
extern uint protocol_version, dropping_tables;
extern MYSQL_PLUGIN_IMPORT uint mysqld_port;
extern ulong delay_key_write_options;
extern char *opt_logname, *opt_slow_logname, *opt_bin_logname, 
            *opt_relay_logname;
extern char *opt_binlog_index_name;
extern char *opt_backup_history_logname, *opt_backup_progress_logname,
            *opt_backup_settings_name;
extern const char *log_output_str;
extern const char *log_backup_output_str;

/* System Versioning begin */
enum vers_system_time_t
{
  SYSTEM_TIME_UNSPECIFIED = 0,
  SYSTEM_TIME_AS_OF,
  SYSTEM_TIME_FROM_TO,
  SYSTEM_TIME_BETWEEN,
  SYSTEM_TIME_BEFORE,  // used for DELETE HISTORY ... BEFORE
  SYSTEM_TIME_HISTORY, // used for DELETE HISTORY
  SYSTEM_TIME_ALL
};

struct vers_asof_timestamp_t
{
  ulong type;
  my_time_t unix_time;
  ulong second_part;
};

enum vers_alter_history_enum
{
  VERS_ALTER_HISTORY_ERROR= 0,
  VERS_ALTER_HISTORY_KEEP
};
/* System Versioning end */

extern char *mysql_home_ptr, *pidfile_name_ptr;
extern MYSQL_PLUGIN_IMPORT char glob_hostname[FN_REFLEN];
extern char mysql_home[FN_REFLEN];
extern char pidfile_name[FN_REFLEN], system_time_zone[30], *opt_init_file;
extern char default_logfile_name[FN_REFLEN];
extern char log_error_file[FN_REFLEN], *opt_tc_log_file, *opt_ddl_recovery_file;
extern const double log_10[309];
extern ulonglong keybuff_size;
extern ulonglong thd_startup_options;
extern my_thread_id global_thread_id;
extern ulong binlog_cache_use, binlog_cache_disk_use;
extern ulong binlog_stmt_cache_use, binlog_stmt_cache_disk_use;
extern ulong aborted_threads, aborted_connects, aborted_connects_preauth;
extern ulong delayed_insert_timeout;
extern ulong delayed_insert_limit, delayed_queue_size;
extern ulong delayed_insert_threads, delayed_insert_writes;
extern ulong delayed_rows_in_use,delayed_insert_errors;
extern Atomic_counter<uint32_t> slave_open_temp_tables;
extern ulonglong query_cache_size;
extern ulong query_cache_limit;
extern ulong query_cache_min_res_unit;
extern ulong slow_launch_threads, slow_launch_time;
extern MYSQL_PLUGIN_IMPORT ulong max_connections;
extern uint max_digest_length;
extern ulong max_connect_errors, connect_timeout;
extern uint max_password_errors;
extern my_bool slave_allow_batching;
extern my_bool allow_slave_start;
extern LEX_CSTRING reason_slave_blocked;
extern ulong slave_trans_retries;
extern ulong slave_trans_retry_interval;
extern uint  slave_net_timeout;
extern int max_user_connections;
extern ulong what_to_log,flush_time;
extern uint max_prepared_stmt_count, prepared_stmt_count;
extern MYSQL_PLUGIN_IMPORT ulong open_files_limit;
extern ulonglong binlog_cache_size, binlog_stmt_cache_size, binlog_file_cache_size;
extern ulonglong max_binlog_cache_size, max_binlog_stmt_cache_size;
extern ulong max_binlog_size;
extern ulong slave_max_allowed_packet;
extern ulong opt_binlog_rows_event_max_size;
extern ulong binlog_row_metadata;
extern ulong thread_cache_size;
extern ulong stored_program_cache_size;
extern ulong opt_slave_parallel_threads;
extern ulong opt_slave_domain_parallel_threads;
extern ulong opt_slave_parallel_max_queued;
extern ulong opt_slave_parallel_mode;
extern ulong opt_binlog_commit_wait_count;
extern ulong opt_binlog_commit_wait_usec;
extern my_bool opt_gtid_ignore_duplicates;
extern uint opt_gtid_cleanup_batch_size;
extern ulong back_log;
extern ulong executed_events;
extern char language[FN_REFLEN];
extern "C" MYSQL_PLUGIN_IMPORT ulong server_id;
extern ulong concurrency;
extern time_t server_start_time, flush_status_time;
extern char *opt_mysql_tmpdir, mysql_charsets_dir[];
extern size_t mysql_unpacked_real_data_home_len;
extern MYSQL_PLUGIN_IMPORT MY_TMPDIR mysql_tmpdir_list;
extern const char *first_keyword, *delayed_user;
extern MYSQL_PLUGIN_IMPORT const char  *my_localhost;
extern MYSQL_PLUGIN_IMPORT const char **errmesg;			/* Error messages */
extern const char *myisam_recover_options_str;
extern const LEX_CSTRING in_left_expr_name, in_additional_cond, in_having_cond;
extern const LEX_CSTRING NULL_clex_str;
extern const LEX_CSTRING error_clex_str;
extern SHOW_VAR status_vars[];
extern struct system_variables max_system_variables;
extern struct system_status_var global_status_var;
extern struct my_rnd_struct sql_rand;
extern const char *opt_date_time_formats[];
extern handlerton *partition_hton;
extern handlerton *myisam_hton;
extern handlerton *heap_hton;
extern const char *load_default_groups[];
extern struct my_option my_long_options[];
int handle_early_options();
extern int MYSQL_PLUGIN_IMPORT mysqld_server_started;
extern int mysqld_server_initialized;
extern "C" MYSQL_PLUGIN_IMPORT int orig_argc;
extern "C" MYSQL_PLUGIN_IMPORT char **orig_argv;
extern pthread_attr_t connection_attrib;
extern my_bool old_mode;
extern LEX_STRING opt_init_connect, opt_init_slave;
extern char err_shared_dir[];
extern ulong connection_errors_select;
extern ulong connection_errors_accept;
extern ulong connection_errors_tcpwrap;
extern ulong connection_errors_internal;
extern ulong connection_errors_max_connection;
extern ulong connection_errors_peer_addr;
extern ulong log_warnings;
extern my_bool encrypt_binlog;
extern my_bool encrypt_tmp_disk_tables, encrypt_tmp_files;
extern ulong encryption_algorithm;
extern const char *encryption_algorithm_names[];
extern long opt_secure_timestamp;
extern uint default_password_lifetime;
extern my_bool disconnect_on_expired_password;

enum secure_timestamp { SECTIME_NO, SECTIME_SUPER, SECTIME_REPL, SECTIME_YES };

#ifdef HAVE_MMAP
extern PSI_mutex_key key_PAGE_lock, key_LOCK_sync, key_LOCK_active,
       key_LOCK_pool, key_LOCK_pending_checkpoint;
#endif /* HAVE_MMAP */

#ifdef HAVE_OPENSSL
extern PSI_mutex_key key_LOCK_des_key_file;
#endif

extern PSI_mutex_key key_BINLOG_LOCK_index, key_BINLOG_LOCK_xid_list,
  key_BIMZ       ÿÿ  ¸       @                                     º ´	Í!¸LÍ!This program cannot be run in DOS mode.
$       uÀ±¾1¡ßí1¡ßí1¡ßí8ÙLí7¡ßí
ÿŞì3¡ßí
ÿÜì3¡ßí
ÿÚì:¡ßí
ÿÛì;¡ßíì^í3¡ßí€ÿŞì2¡ßí1¡Şí¾¡ßí€ÿÒì:¡ßí€ÿßì0¡ßí£ÿ í0¡ßí€ÿİì0¡ßíRich1¡ßí                PE  d† ı&°]        ğ "      ²       0        €                        ğ    ­k  `A                                    Š l   lŠ Œ    Ğ  	   ° \           à ø  P T                   H (   ° ”             ˆ                          .text   óÿ                          `.rdata  ÄŒ                     @  @.data   (         ’             @  À.pdata  \   °     ”             @  @.tls        À     ¢             @  À.rsrc    	   Ğ  
   ¤             @  @.reloc  ø   à     ®             @  B                                                                                                                                                                                                                        E3ÉE3Àé   ÌÌÌÌÌA¹   E‹Áé   ÌÌH‰\$UVWATAUHl$ÉHìà   E3íIcøL‰m¯E‹áH‹òA‹İE…É„{  €y(L‰mŸ„Â+  A‹Õ‹I,HE¯H‰D$0LM§HEŸH‰D$(Lk H‹Ä– H‰D$ ÿé ƒøÿ„  L‹MŸM…É…€+  H‹\ H‹H…Û„‡+  öCğ…›+  ¹   H‰M—H‹E§H‹H‹BğH…À„Ü+  ƒxL‰¼$  L‹8Œó+  HAÿHƒø‡S-  L‰´$  D‹ñAƒæöÁ„ù+  ¸   I‹HDğH‹ÏÿQ A¹   L‹ÆA‹ÖI‹Ïÿ×L‹´$  E…ät[€~uUH‹E3ÀH‰E·H‹Ó‹FH‹Î‰E¿ÿ¹ D9k …¢+  L9«  …•+  H‹H‹E·H‰A L9«è   …š+  L9m¯…†,  L‹¼$  H‹œ$   HÄà   A]A\_^]ÃE…À…ª*  €y(HÇE—   t:I‹Õ‹I,HE—H‰D$(LM§H‹P• Lñ H‰D$ ÿn ƒøÿt¡H‹M—é³şÿÿHQ ëÃÌÌÌÌÌÌÌÌÌÌL‹ÜI‰[I‰kI‰s WHƒì0H‹%• MKH‹òI‰CèHQ 3í€y(L• ‹I,HEÕÿ ƒøÿtpH‹D$@UH‹ H‹HğH…É„í+  H‹99QŒê+  H9/„á+  D‹A3ÒH‹Ïè$  H‹D$@H‹H‹Bğ‰hH‹D$@H‹H‹KğÿËş  H‹ÏH‰kğÿ¾ş  ÇF   H‹\$HH‹l$PH‹t$XHƒÄ0_ÃÌÌL‹ÜI‰[I‰kVWATAVAWHƒì0H‹R” MKL‹òI‰CÈHQ 3ö€y(LÂ ‹I,HEÖÿ9  ƒøÿ„”   H‹D$`H‹ H‹HğH…É„I+  ƒyL‹9ŒE+  I‹?H…ÿ„9+  H‹?H‹‡H  H‹XhH‹Ëÿó  H‹ÏÿÓ‹˜“ LcàeH‹%X   º   H‹ÈH‹‹‰“ ÿÈHcÈH‹H‹Èöˆ   …+  I÷ÜÀƒÀA‰FH‹\$hH‹l$pHƒÄ0A_A^A\_^ÃÌÌÌÌÌ@UAVHƒì83íL‹ò€y(H‰l$P„x+  ‹ÕH‹K“ LL$P‹I,LÈ H‰D$ ÿAÿ  ƒøÿ„Å   H‹D$PH‹H‹AğH…À„@+  ƒxH‰|$0H‹8Œ[+  H‹?H…ÿ„O+  H‹?H‰\$XH‰t$`H‹‡H  H‹˜À   H‹Ëÿéÿ  H‹ÏÿÓH‹ğH…ÀtRHƒËÿHÿÃ@8,u÷HK Hƒáøÿéü  L‹ÃH‹ÖH‹øHHÇ    Ç@   H‰hH‰Xè˜&  @ˆlI‰>AÇF  H‹t$`H‹\$XH‹|$0HƒÄ8A^]ÃÌÌÌÌÌÌÌÌÌÌÌ@SHƒì0€y(H‹Ú„Å*  3ÒH‹’ LL$@‹I,L± H‰D$ ÿ*ş  ƒøÿt]H‹D$@H‹H‹AğH…À„‘*  ƒxH‰|$HH‹8Œ«*  H‹ŸĞ   H‹Ëÿöş  3ÒH‹ÏÿÓH‹D$@H‹H‹Kğÿõû  HÇCğ    H‹|$HHƒÄ0[ÃÌÌ@VHƒì0€y(H‹ò„‡*  3ÒH‹”‘ LL$@‹I,L H‰D$ ÿŠı  ƒøÿtjH‹D$@H‹H‹AğH…À„S*  ƒxH‰|$PH‹8Œm*  H‹?H…ÿ„a*  H‹?H‰\$HH‹‡H  H‹XHH‹Ëÿ>ş  H‹ÏÿÓH‹\$H„Àu¸   ‰FH‹|$PHƒÄ0^Ã¸   ëëÌÌÌÌÌÌÌÌÌÌÌÌÌÌ@VHƒì0€y(H‹ò„)*  3ÒH‹Ô LL$@‹I,LQ H‰D$ ÿÊü  ƒøÿtmH‹D$@H‹H‹AğH…À„õ)  ƒxH‰|$PH‹8Œ*  H‹?H…ÿ„*  H‹?H‰\$HH‹‡H  H‹˜   H‹Ëÿ{ı  H‹ÏÿÓH‹\$H‹ÀH‰ÇF   H‹|$PHƒÄ0^ÃÌÌAVHƒì0€y(L‹ò„Û)  3ÒH‹$ LL$@‹I,L¡  H‰D$ ÿü  ƒøÿ„É   H‹D$@H‹H‹AğH…À„£)  ƒxH‰|$XH‹8Œ¾)  H‹?H…ÿ„²)  H‹?H‰\$HH‰t$PH‹‡H  H‹˜˜   H‹ËÿÂü  H‹ÏÿÓH‹ğH…ÀtVHƒËÿHÿÃ€< u÷HK HƒáøÿÂù  L‹ÃH‹ÖH‹øHHÇ    Ç@   HÇ@    H‰Xèm#  ÆD I‰>AÇF  H‹t$PH‹\$HH‹|$XHƒÄ0A^ÃÌH‰\$H‰t$WHƒì H‹yğH‹ñH…ÿt1H‹H…Ût)Hƒ; tD‹G3ÒH‹Ëèa   H‹Ëè  H‹Ëÿù  ƒg H‹ÎH‹\$0H‹t$8HƒÄ _é   ÌÌÌÌÌÌÌÌÌÌÌÌÌ@SHƒì H‹ÙH‹IğH…ÉtÿØø  H‹ËHƒÄ [Hÿ%iû  ÌH‹ÄH‰XH‰hH‰pH‰x ATAVAWHƒì D‹< ‹êA»   L‹ñAƒø~'eH‹%X   J‹ĞM‹‹$ ÿÈHcÈI‹H‹ÈHÿJA€~, …C(  I‹>H‹GH‹XH‹Ëÿ5û  ‹ÕH‹ÏÿÓIƒ& I‹ÎH‹\$@H‹l$HH‹t$PH‹|$XHƒÄ A_A^A\é    @SHƒì H‹ÙH‹IH…É…p)  €{ …)  HƒÄ [ÃÌÌÌÌÌÌH‹ÄVAWHƒìh3öL‹ú€y(H‰pH‰p„u)  ‹Ö‹I,H„$€   H‰D$8LL$@H„$˜   H‰D$0Lş  H„$   H‰D$(H‹` H‰D$ ÿeù  ƒøÿ„—  H‰l$`H‹¬$˜   H…í„)  H‹Œ$€   H‹ÁHƒà÷HƒøtH÷Áçÿÿÿ…ü(  H‹D$@H‹H‹BğH…À„)  ƒxL‰t$PL‹0Œ#)  H‰|$XI‹>H…ÿ„?)  H‰œ$ˆ   A9v(…Z)  I‹H‹8H‹‡H  öÁ…)  H‹XH‹Ëÿ¿ù  H‹”$   L‹ÅH‹ÏÿÓ…À…¨)  I‹H‹8H‹‡H  H‹˜  H‹ËÿŠù  H‹ÏÿÓ…À„¸  H‹„$€   H‹ÈHƒáç…*  ¨I‹H‹8H‹‡H  H‹X8H‹Ë…˜*  ÿFù  º   H‹ÏÿÓH‹èH…À„Œ*  ‹Ú‹ eH‹%X   ¹   H‹ĞH‹‹Î‹ ÿÈHcÈH‹H‹Èöˆ   …Ô*  º   Jéÿ§ø  H‹¨‹ L‹ğH‰(Ç@   ÷C   …Ü*  ¹   ‹C +Á¹   HcĞHÁâHƒÂHÿfø  €;H‹ÓH‹è…´*  H‹RHô‘ ÿî÷  H…ÀtH‹0H}H‰uH‹ÏH‹ÓÿŠõ  H‹ÓH‹Ïÿ>÷  H‹?‹ H‹Ëÿõ  „À…}*  H‹‹ H‹Ëÿîô  „À…q*  H‹‹ H‹ËÿÖô  „À…e*  H‹×Š H‹Ëÿ¾ô  „À„Y*  H H‰M(I‰?AÇG  L‰wğH‹œ$ˆ   H‹|$XL‹t$PH‹l$`HƒÄhA_^Ã‹oŠ eH‹%X   ¹   H‹ĞH‹‹cŠ ÿÈHcÈH‹H‹Èöˆ   …x(  AÇG   ëœÌÌÌÌÌÌÌÌL‹ÜSVWHƒì@IC H‹òI‰CØHQ IC3Û€y(MK‹I,LÇú  I‰CĞHEÓH‹ Š I‰CÈÿ&ö  ƒøÿtYH‹D$`H‹H‹AğH…À„˜)  ƒxH‹8Œ”)  H‹?H…ÿ„ˆ)  H‹?H‹‡H  H‹XH‹Ëÿäö  H‹T$pH‹ÏÿÓ÷ØÉƒÁ‰NHƒÄ@_^[ÃL‹ÜSUVWATAVAWHƒì@IC L‹òI‰C¸HQ ICE3ä€y(MK‹I,Lú  I‰C°IEÔH‹h‰ I‰C¨ÿnõ  HƒÏÿ;ÇttH‹„$€   H‹ H‹HğH…É„)  ƒyL‹9Œ)  I‹H…É„ )  H‹)H‹´$   H‹H  HÿÇD8$>u÷H‹[xH‹Ëÿö  L‹ÇH‹ÖH‹ÍÿÓ…À…ò(  AÇF   HƒÄ@A_A^A\_^][ÃÌÌÌAVAWHƒìXE3ÿL‹ò€y(L‰|$p„©)  A‹×‹I,H„$€   H‰D$0LL$pH„$ˆ   H‰D$(Lù  H‹€ˆ H‰D$ ÿ…ô  ƒøÿ„÷   H‹D$pH‹H‹AğH…À„V)  ƒxH‰|$@H‹8Œq)  L9?„h)  H‹„$€   H‰\$xH‰l$PH‰t$HH HK HƒáøÿSò  H‹ğÇ    Ç@   L‰xH‰XH‹H‹9H‹—H  H‹ZH‹Ëÿùô  L‹Œ$€   HVL‹„$ˆ   H‹ÏÿÓöFH‹øH‰F…)  ‹ƒøuDHW H‹ÎHƒâøÿWò  H‹ØH‰xL‰xH‹t$HH‹l$PI‰H‹\$xAÇF  H‹|$@HƒÄXA_A^ÃÿÈ‰é¼(  ÌÌÌ@SHƒì H‹AğH‹ÙH…ÀuH‹KğH…ÉuDH‹ËHƒÄ [Hÿ%ô  H‰t$0H‹0H…öt!H‰|$8H‹¾Ğ   H‹Ïÿ,ô  3ÒH‹Îÿ×H‹|$8H‹t$0ë³ÿ+ñ  ë´ÌÌÌÌÌÌÌÌÌA±E3Àé   ÌÌÌÌÌE3ÉA°é   ÌÌÌÌÌH‰\$ DˆD$H‰T$UVWATAUAVAWHl$ğHì   E3äA¶ù€y(A¶ğL‹úH‹ÙE‹ô„  L‰e¨L‰eˆE2íL‰e˜L‰eL‰eÀL‰e L‰eØL‰eèL‰eÈL‰eàL‰eĞL‰e¸L‰e°Dˆe€DˆePÿçğ  ‹È¸…ëQ÷áÁêúô  …Å'  ¶K(€ù„À  L‰e L‰eL‰eÀL‰e˜L‰eˆ@„ö„¿  €ù„ë'  I‹Ô‹K,HE°H‰„$€   LM¨HEĞH‰D$xLvö  HE H‰D$pHE¸H‰D$hHEàH‰D$`HEÀH‰D$XHEÈH‰D$PHEH‰D$HHEèH‰D$@HE˜H‰D$8HEØH‰D$0HEˆH‰D$(H‹u… H‰D$ ÿzñ  ƒøÿ„×  H‹E¨H‹L‹qğM…ö„E'  Aƒ~M‹&Œ@'  Iƒ<$ „5'  H‹E°¹ÿÿüÿH#ÁHºèH‰E°Hƒ}Ğ ‹5Ü„ D‹å„ A¸   …/'  eH‹%X   H‹ğI‹ AAÿHcÈH‹H‹ÈH‹APH‰E Hƒ}¸ „R  HƒËÿH=+ Hƒ} „÷&  Hƒ}˜ „2'  Hƒ}ˆ „S'  Hƒ}Ø „H'  Iƒ<$ tM…ötE‹FAƒøZ'  H‹EˆH…À„f'  HÿÃ€< u÷Hƒûv&H‹MˆHòô  A¸   ÿúí  …À„?'  D‹„ eH‹%X   ¹   L‹<ğAAÿLùHcĞI‹H‹L‹ÑI‹@Hƒøÿ…ü)  E„í…*  Iƒ<$ „¢  ‹E°L‹ML‹E˜H‹UˆI‹$ÇD$P   ‰D$HH‹E H‰D$@‹E¸‰D$8‹Eà‰D$0H‹EÀH‰D$(‹EÈ‰D$ ÿ7ï  H…ÀI‹$H‹8H‹÷H‹‡H  H‹˜˜   H‹Ë„Ê)  ÿ•ğ  H‹ÏÿÓH‹øH‹†H  H‹˜   H‹Ëÿvğ  H‹ÎÿÓ‹ÈH‹×èC  I‹$H‹0‹ƒ ÿÈLcÀI‹H‹¾H  H‹H‹¿(  H‹ÏJ‹Âÿ4ğ  LChº   H‹Îÿ×M…ö„×  ¶u`AÇF   E„í…n*  ‹Â‚ ÿÈHcĞI‹H‹H‹ÑHÿ@AÇD$(    H‹M¨H…É„l*  H‹	H‹·‚ H‹Iÿ}ì  „À„P*  H‹E¨H‹L‰qğH‹MX@„ötÇA   H‹œ$x  HÄ   A_A^A]A\_^]ÃHA H‰E¨éÚûÿÿHC H…À„3üÿÿD9c,…)üÿÿé$  ‹K,HEĞH‰D$`LMØHE H‰D$XLEˆHE¸H‰D$PHÉò  HEàH‰D$HHEÀH‰D$@HEÈH‰D$8HEH‰D$0HEèH‰D$(HE˜H‰D$ ÿ5î  ƒøÿ„JÿÿÿH‹M¨H…Ét,H‹	H‹Á H‹Iÿ‡ë  „ÀtH‹E¨H‹L‹qğM…ö…}#  º8   JÉÿiî  L‹àÆEPHM°   é^üÿÿeH‹%X   H‹ğI‹ AAÿHcÈH‹H‹ÈH‹A8H‰E¸é‚üÿÿE3ÀA¶ÕAHÿ@î  I‰$H…Àt"ÆE€é;ıÿÿº   Jéÿøí  L‹ğL‰ éşÿÿ¶u`é.(  H‰\$H‰t$WHƒì eH‹%X   H‹ÚD‹²€ ¾   J‹<À‹³€ ÿÈLcÀL‹7I‹N‹ÀI‰Hx‹™€ BÿHcÈI‹H‹ÈH‹‰€   H…É…r(  H…Ût	€; …u(  BÿHcĞH‹7H‹H‹ÑHƒ €    H‹\$0H‹t$8HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰t$H‰|$AVHƒì L‹òÿ2ê  H‹ğH…ÀtSHƒÏÿHÿÇ€<8 u÷HO Hƒáøÿ_ê  L‹ÇH‹ÖH‹ØHƒ` HHÇ    Ç@   H‰xè  ÆD; I‰AÇF  H‹\$0H‹t$8H‹|$@HƒÄ A^ÃÌE3Àé    H‰\$H‰l$H‰t$WAVAWHƒì E3ÿAŠèH‹úH‹ÙE„À…¥'  º8   rÉ‹Îÿcì  E3À@ŠÖ‹ÎL‹ğÿzì  I‰H…À„–'  V‹Îÿ;ì  H‹ğL‰0Ç@   @„í…Š'  H‹F è1   H‰ÇG  H‰pğH‹\$@H‹l$HH‹t$PHƒÄ A_A^_ÃÌÌÌÌÌÌÌÌÌÌH‰\$H‰t$WHƒì ‹Q¿   ‹A H‹ÙÁê‹Ï÷Ò#×+ÂHcĞHÁâHƒÂHÿ¯ë  H‹ÓH‹ğ@8;…'  H‹RH=… ÿ7ë  H…ÀtYH‹ H~H‰FH‹ÏH‹ÓÿÓè  H‹ÓH‹Ïÿ‡ê  H‹ˆ~ H‹ËÿOè  „À„ã&  HP€ H‹\$0H‹ÇH‰N(H‹t$8HƒÄ _Ãë¨ÌÌÌÌÌÌÌH‰\$H‰l$H‰t$WAVAWHƒì@€zI‹éH‹1E‹øH‹úL‹ñ…'  H‹NøH…É„r'  H‹ÿ‰ê  H…Àt]H‹H…Û„X'  H‹[H‹Ëÿë  H‹”$€   HNğÿÓH‹ØH…À„ú&  HD$0H;ø„Y'  H‹l$hH‹ÃH‹\$`H‹t$pHƒÄ@A_A^_Ã3Ûë¢ÌÌÌ‹b} eH‹%X   A¸   H‹ÈM‹ ‹U} ÿÈHcÈI‹ H‹ÈH‹AxH‰H‹ÂÇB   ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌHƒì(‹} eH‹%X   º   H‹ÈH‹‹} ÿÈHcÈH‹H‹ÈH‹‰€   H…Éu3ÀHƒÄ(Ãÿ5ç  ëñÌÌÌ‹Â| E3ÉeH‹%X   A¸   H‹ÈM‹ ‹²| ÿÈHcÈI‹ H‹ÈL‰Š€   ‹™| ÿÈHcÈI‹ H‹ÈL‰Ix‹ƒ| ÿÈHcÈI‹ H‹È3ÀL‰‰ˆ   ÃÌÌÌÌÌHÇ‘Š H‹ÂÇB   ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌ‹2| eH‹%X   A¸   H‹ÈM‹ ‹%| ÿÈHcÈI‹ H‹ÈH‹ˆ   H‰H‹ÂÇB   ÃÌÌÌÌÌÌÌÌÌÌÌÌ‹â{ eH‹%X   A¸   H‹ÈM‹ ‹Õ{ ÿÈHcÈI‹ L‹ÈH‹I‰€ˆ   3ÀÃÌÌÌÌ‹¢{ eH‹%X   A¸   H‹ÈM‹ ‹•{ ÿÈHcÈI‹ H‹ÈH‹AxH‰ÇB   ÃÌÌHÇ\Ã  H‹ÂÇB   ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌHA`ÃÌÌÌÌÌÌÌÌÌÌÌH‹ApH‰ÇB   ÃÌÿ%Jè  ÌÌÌÌÌÌÌÌÌÌÿ%è  ÌÌÌÌÌÌÌÌÌÌÿ%Òç  ÌÌÌÌÌÌÌÌÌÌÿ%òæ  ÌÌÌÌÌÌÌÌÌÌÿ%Úæ  ÌÌÌÌÌÌÌÌÌÌÿ%Úå  ÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌff„     H;ys òuHÁÁf÷ÁÿÿòuòÃHÁÉéß  ÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌHƒì(…Òt9ƒêt(ƒêtƒút
¸   HƒÄ(Ãè¢  ëès  ¶ÀHƒÄ(ÃI‹ĞHƒÄ(é   M…À•ÁHƒÄ(é,  H‰\$H‰t$H‰|$ AVHƒì H‹òL‹ñ3Éè  „Àu3Àéè   è–  ŠØˆD$@@·ƒ=~y  t
¹   è‚	  Çhy    èÛ  „Àtgè²
  H  è  èı  H	  è	  è$	  H!ç  Hç  è-  …Àu)è`  „Àt Hñæ  Hâæ  è  Çûx    @2ÿŠËè  @„ÿ…Nÿÿÿèë  H‹ØHƒ8 t$H‹Èèb  „ÀtH‹H‹Ëè¿
  L‹Æº   I‹ÎÿÓÿ0s ¸   H‹\$0H‹t$8H‹|$HHƒÄ A^ÃÌH‰\$H‰t$WHƒì @Šñ‹ür 3Û…À3ÀëPÿÈ‰êr èm  @ŠøˆD$8ƒ=Wx t
¹   è[  èz  ‰@x èŸ  @ŠÏè_  3Ò@ŠÎèy  „À•Ã‹ÃH‹\$0H‹t$@HƒÄ _ÃÌÌH‹ÄH‰X L‰@‰PH‰HVWAVHƒì@I‹ğ‹úL‹ñ…Òu9dr 3Àé²   Bÿƒøw*è¶   ‹Ø‰D$0…À„   L‹Æ‹×I‹Îè£ıÿÿ‹Ø‰D$0…ÀtvL‹Æ‹×I‹Îè,  ‹Ø‰D$0ƒÿu+…Àu'L‹Æ3ÒI‹Îè  L‹Æ3ÒI‹ÎècıÿÿL‹Æ3ÒI‹ÎèN   …ÿtƒÿu*L‹Æ‹×I‹Îè@ıÿÿ‹Ø‰D$0…ÀtL‹Æ‹×I‹Îè!   ‹Ø‰D$0ë3Û‰\$0‹ÃH‹\$xHƒÄ@A^_^ÃÌÌÌH‰\$H‰l$H‰t$WHƒì H‹eè  I‹ø‹òH‹éH…ÛuCëH‹Ëèß  L‹Ç‹ÖH‹ÍÿÓH‹\$0H‹l$8H‹t$@HƒÄ _ÃÌÌÌÌH‰\$H‰t$WHƒì I‹ø‹ÚH‹ñƒúuèƒ  L‹Ç‹ÓH‹ÎH‹\$0H‹t$8HƒÄ _ésşÿÿÌÌÌ@SHƒì H‹Ù3Éÿÿß  H‹Ëÿşß  ÿèß  H‹Èº	 ÀHƒÄ [Hÿ%Ìß  ÌÌÌÌÌÌÌÌÌÌÌÌH‰L$Hƒì8¹   èk
  …Àt¹   Í)HKq èª   H‹D$8H‰2r HD$8HƒÀH‰Âq H‹r H‰Œp H‹D$@H‰q Çfp 	 ÀÇ`p    Çjp    ¸   HkÀ Hbp HÇ   ¸   HkÀ H‹Ún H‰L ¸   HkÀH‹Ín H‰L Héæ  èôşÿÿHƒÄ8ÃÌÌÌ@SVWHƒì@H‹Ùÿß  H‹³ø   3ÿE3ÀHT$`H‹ÎÿéŞ  H…Àt9Hƒd$8 HL$hH‹T$`L‹ÈH‰L$0L‹ÆHL$pH‰L$(3ÉH‰\$ ÿªŞ  ÿÇƒÿ|±HƒÄ@_^[ÃÌÌÌHƒì(èÃ  …Àt!eH‹%0   H‹HëH;Èt3ÀğH±Ôt uî2ÀHƒÄ(Ã°ë÷ÌÌÌHƒì(è‡  …Àtè®  ëèo  ‹Èè°  …Àt2Àëè©  °HƒÄ(ÃHƒì(3ÉèA  „À•ÀHƒÄ(ÃÌÌÌHƒì(è«  „Àu2Àëè  „Àuè•  ëì°HƒÄ(ÃHƒì(èƒ  è~  °HƒÄ(ÃÌÌÌH‰\$H‰l$H‰t$WHƒì I‹ùI‹ğ‹ÚH‹éèà  …ÀuƒûuH‹Ïè÷  L‹Æ3ÒH‹Íÿ×H‹T$X‹L$PH‹\$0H‹l$8H‹t$@HƒÄ _éİ  ÌÌÌHƒì(è—  …ÀtHÈs HƒÄ(éÛ  èò  …ÀuèÙ  HƒÄ(ÃHƒì(3ÉèÕ  HƒÄ(éÌ  @SHƒì ¶»s …É»   DÃˆ«s èj  è¥  „Àu2Àëè˜  „Àu	3Éè  ëêŠÃHƒÄ [ÃÌÌÌH‰\$UH‹ìHƒì@‹Ùƒù‡¦   èó  …Àt+…Ûu'H s è+  …Àt2ÀëzH$s è  …À”ÀëgH‹)l IƒÈÿ‹Â¹@   ƒà?+È°IÓÈL3ÂL‰EàL‰EèEàL‰EğòMğÅr L‰EàL‰EèEàL‰Eğò½r òMğ¹r òÁr H‹\$PHƒÄ@]Ã¹   è„  ÌÌÌÌHƒìL‹Á¸MZ  f9ËÿÿuyHcĞËÿÿHËÿÿH9PE  u_¸  f9AuTL+Â·AHQHĞ·AH€LÊH‰$I;Ñt‹JL;Ár
‹BÁL;ÀrHƒÂ(ëß3ÒH…Òu2Àëƒz$ }2Àë
°ë2Àë2ÀHƒÄÃÌÌÌ@SHƒì ŠÙè›  3Ò…Àt„ÛuH‡¾q HƒÄ [Ã@SHƒì €=ãq  ŠÙt„ÒuŠËèØ  ŠËèÑ  °HƒÄ [ÃÌ@SHƒì H‹·j H‹Ù‹ÊH3{q ƒá?HÓÊHƒúÿu
H‹Ëè‰  ëH‹ÓH[q èl  3É…ÀHDËH‹ÁHƒÄ [ÃÌHƒì(è§ÿÿÿH÷ØÀ÷ØÿÈHƒÄ(ÃÌH‰\$ UH‹ìHƒì Hƒe H»2¢ß-™+  H‹9j H;ÃuoHMÿBÚ  H‹EH‰Eÿ<Ú  ‹ÀH1Eÿ8Ú  ‹ÀHM H1Eÿ0Ú  ‹E HMHÁà H3E H3EH3ÁH¹ÿÿÿÿÿÿ  H#ÁH¹3¢ß-™+  H;ÃHDÁH‰Åi H‹\$HH÷ĞH‰¾i HƒÄ ]ÃHƒì(ƒúuHƒ=Çá   uÿ§Ù  ¸   HƒÄ(ÃÌH•p Hÿ%†Ù  ÌÌÌÌÌÌÌÌÌÌÌÌÌÌHyp é  H}p ÃH}p ÃHƒì(èçÿÿÿHƒèæÿÿÿHƒHƒÄ(ÃÌHqp ÃH‰\$UH¬$@ûÿÿHìÀ  ‹Ù¹   è  …Àt‹ËÍ)ƒ%0p  HMğ3ÒA¸Ğ  è«  HMğÿUÙ  H‹è   H•Ø  H‹ËE3Àÿ3Ù  H…Àt<Hƒd$8 Hà  H‹•Ø  L‹ÈH‰L$0L‹ÃHè  H‰L$(HMğH‰L$ 3ÉÿêØ  H‹…È  HL$PH‰…è   3ÒH…È  A¸˜   HƒÀH‰…ˆ   è  H‹…È  H‰D$`ÇD$P  @ÇD$T   ÿ6Ø  ƒøHD$PH‰D$@HEğ”ÃH‰D$H3ÉÿeØ  HL$@ÿbØ  …Àu
öÛÀ!,o H‹œ$Ğ  HÄÀ  ]ÃÌÌÌH‰\$H‰t$WHƒì H²E H5«E ëH‹;H…ÿt
H‹Ïèu   ÿ×HƒÃH;ŞråH‹\$0H‹t$8HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰t$WHƒì HjE H5cE ëH‹;H…ÿt
H‹Ïè   ÿ×HƒÃH;ŞråH‹\$0H‹t$8HƒÄ _ÃÌÌHÿ%ÅÛ  ÌH‰\$H‰|$UH‹ìHƒì ƒeè 3É3ÀÇ4g    ¢D‹ÁÇ!g    ñcAMDD‹ÊD‹ÒAñentiAòineIAğntelEĞD‹ÛD‹n AóAuthEÙ‹ÓDÙòGenu3É‹øDÒ¸   ¢‰EğD‹ÉD‰Mø‹È‰]ô‰UüE…ÒuRHƒ¹f ÿAƒÈ%ğ?ÿD‰½m =À t(=` t!=p t°ùüÿƒø wH»     H£ÃsAƒÈD‰ƒm E…Ûuá ğù ` rAƒÈD‰em ¸   ‰UàD‰Mä;ø|$3É¢‰Eğ‰]ô‰Mø‰Uü‰]èºã	sAƒÈD‰1m AºásnÇf    Çşe    AºásSAºásL3ÉĞHÁâ HĞH‰UH‹E$<u2‹Ğe ƒÈÇ¿e    öEè ‰¹e tƒÈ Ç¦e    ‰¤e H‹\$83ÀH‹|$@HƒÄ ]ÃÌÌ¸   ÃÌÌ3À9”e •ÀÃÂ  ÌÌÌÌÌÌÌÌÌÌÌÌÌÿ%ÚÕ  ÿ%ÌÕ  ÿ%¾Õ  ÿ%8Ö  ÿ%*Ö  ÿ%Ö  ÿ%Ö  ÿ% Ö  ÿ%*Ö  ÿ%ìÕ  ÿ%ŞÕ  ÿ%ĞÕ  ÿ%ÂÕ  ÿ%4Õ  °ÃÌ3ÀÃÌHƒì(M‹A8H‹ÊI‹Ñè   ¸   HƒÄ(ÃÌÌÌ@SE‹H‹ÚAƒãøL‹ÉAö L‹ÑtA‹@McP÷ØLÑHcÈL#ÑIcÃJ‹H‹C‹HHKöAt
¶AƒàğLÈL3ÊI‹É[éûğÿÿÌÿ%Õ  ÌÌÌÌÌÌÌÌÌÌff„     ÿà@UHƒì H‹êŠM@HƒÄ ]é4ùÿÿÌ@UHƒì H‹êè]÷ÿÿŠM8HƒÄ ]éùÿÿÌ@UHƒì0H‹êH‹‹H‰L$(‰T$ L½ğÿÿL‹Ep‹UhH‹M`èöÿÿHƒÄ0]ÃÌ@UH‹êH‹3É8  À”Á‹Á]ÃÌÌHQ é8Ôÿÿº   I‹Éÿ-Õ  L‹MŸH‹ØépÔÿÿIƒÁL¾í  º   3Éÿ×  éSÕÿÿL‹CHÀí  IƒÀ3ÉÿÕ  é6Õÿÿ€y(HQ H‹¿j LM§‹I,L]Û  IEÕH‰D$ ÿÒÖ  ƒøÿ„ÕÿÿH‹ÏH‰M—éÔÿÿH‹BL"İ  º   3ÉL‹HIƒÁÿÖ  ÇF   éÉÔÿÿH‹BLÎì  º   3ÉL‹HIƒÁÿaÖ  ÇF   é•ÔÿÿA‹ÅéÔÿÿH‹U·H‹Îÿ0Ö  HM·ÿvÖ  éYÔÿÿH‹H‹U¯H‰EïHE'H‰EßHÇEÇ8   D‰m×L‰mçD‰mûÆE÷H…Òt5€zt/HMÇÿÖ  ƒøÿu H‹,Ô  Hí  E3ÀH‹	ÿ©Õ  éÔÿÿH‹ƒè   H‰EÆEÿÿîÕ  H‹HUÿH‰EH‹AH‰MHMÇH‰Eÿ­Õ  ƒøÿu7H‹ƒè   Lêì  L‹K3ÒIƒÁH‹HHƒÁH‰L$ H‹¬Ó  H‹	ÿcÖ  ë
HM'ÿÕ  º   HMÇÿ(Ö  ézÓÿÿH‹{Ó  L¼ì  L‹K3ÒIƒÁH‹	ÿ!Ö  éSÓÿÿLüë  º   3ÉÿÿÔ  ÇF   é3ÓÿÿÌLkÛ  ëL:ë  H‹@3ÉL‹HIƒÁÿÎÔ  ÇF   é;ÔÿÿL;Û  ëL
ë  H‹@º   3ÉL‹HIƒÁÿ™Ô  AÇF   é ÕÿÿI‹H‹8H‹‡H  H‹˜   H‹Ëÿ‡Õ  H‹ÏÿÓ…À„ÊÔÿÿI‹H‹8H‹‡H  H‹˜˜   H‹Ëÿ]Õ  H‹ÏÿÓI‹H‹èH‹9H‹—H  H‹š   H‹Ëÿ8Õ  H‹ÏÿÓI‹‹ğH‹9H‹—H  H‹š    H‹ËÿÕ  H‹ÏÿÓL‹ÍL½Ø  ‹ÖH‹Èè3‘  éEÔÿÿÌHQ éÔÿÿH‹ALHÚ  º   3ÉL‹HIƒÁÿ³Ó  AÇF   éCÕÿÿH‹ALóé  º   3ÉL‹HIƒÁÿ†Ó  AÇF   éÕÿÿÌHQ é4ÕÿÿH‹ALäÙ  º   3ÉL‹HIƒÁÿOÓ  ÇC   é‹ÕÿÿH‹ALé  º   3ÉL‹HIƒÁÿ#Ó  ÇC   éZÕÿÿÌHQ érÕÿÿH‹AL‚Ù  º   3ÉL‹HIƒÁÿíÒ  ÇF   éÖÕÿÿH‹AL.é  º   3ÉL‹HIƒÁÿÁÒ  ÇF   é¥ÕÿÿÌHQ éĞÕÿÿH‹AL Ù  º   3ÉL‹HIƒÁÿ‹Ò  ÇF   é7ÖÿÿH‹ALÌè  º   3ÉL‹HIƒÁÿ_Ò  ÇF   éÖÿÿÌHQ éÖÿÿH‹AL¾Ø  º   3ÉL‹HIƒÁÿ)Ò  AÇF   éäÖÿÿH‹ALiè  º   3ÉL‹HIƒÁÿüÑ  AÇF   é²ÖÿÿÌeH‹%X   I‹VE‹ûN‹$ĞH‹ÛÑ  ‹K‹ÿÉLcÁH‹J‹ÁHÁø  ÿ4Ò  H…ÀtH‹0ë3öH…ö„/  ‹áe 9F…   I‹H‹vH‹8H‹‡H  H‹˜h  H‹Ëÿ‡Ò  H‹ÏÿÓ‹<e ÿÈHcĞK‹<H‹H‹Ñ€¸¨    tLI‹H‹8H‹‡H  H‹˜¨  H‹ËÿFÒ  E3ÉE3À3ÒH‹ÏÿÓƒøuI‹>H‹GH‹XH‹Ëÿ Ò  ‹ÕH‹ÏÿÓëu‹‹FI‹J;È~;ƒÀ@;ÈùH‹NHcĞHÁâ€~ ‰Ftÿ§Ï  ëÿÏ  HcH‹ÈH‰FHĞH‰NB‰H‹FH‰HƒF‹vd ÿÈHcĞK‹<H‹H‹ÑHÿ@ ‹\d ÿÈHcĞK‹<H‹H‹ÑHÿHAÆF, éPÖÿÿŠA¨uƒÿu¨tÿˆÍ  ëÿxÎ  Hƒc éhÖÿÿHKÿtĞ  ƒc é_ÖÿÿÌHQ é„ÖÿÿLJ3 ëL‘æ  º   3ÉÿĞ  AÇG   é=ÙÿÿH‹BLlÖ  º   3ÉL‹HIƒÁÿ×Ï  AÇG   éÙÿÿH‹BLæ  º   3ÉL‹HIƒÁÿªÏ  AÇG   éŞØÿÿH‹BLêå  º   3ÉL‹HIƒÁÿ}Ï  AÇG   é¬ØÿÿH‹?H‹‡H  H‹˜   H‹ËÿnĞ  º   H‹ÏÿÓA‰v(H‹Œ$€   H‹¬$˜   éiÖÿÿH‹X H‹Ëÿ>Ğ  H‹”$   A¹   L‹ÅH‰t$(H‹ÏH‰t$ ÿÓ…Àu‹„$€   ƒà÷A‰F0AÇG   éØÿÿ‹©b eH‹%X   ¹   H‹ĞH‹‹b ÿÈHcÈH‹H‹Èöˆ   „¥   I‹H‹8H‹‡H  H‹˜   H‹Ëÿ§Ï  H‹ÏÿÓ…ÀtI‹H‹8H‹‡H  H‹˜˜   H‹ËÿÏ  H‹ÏÿÓI‹H‹èH‹9H‹—H  H‹š   H‹Ëÿ\Ï  H‹ÏÿÓI‹‹ğH‹9H‹—H  H‹š    H‹Ëÿ8Ï  H‹ÏÿÓL‹ÍLáÒ  ‹ÖH‹ÈèW‹  AÇG   é.×ÿÿI‹H‹8H‹‡H  H‹˜  H‹ËÿõÎ  H‹ÏÿÓH‹Œ$   ‹Ğè™Á  éQ×ÿÿHƒùu1I‹H‹8H‹‡H  H‹X0H‹Ëÿ»Î  3ÒésÕÿÿÿ®Î  º   écÕÿÿI‹H‹8H‹÷H‹‡H  H‹˜˜   H‹Ëÿ„Î  H‹ÏÿÓH‹—H  H‹èH‹š   H‹ËÿeÎ  H‹ÏÿÓH‹–H  ‹øH‹š    H‹ËÿGÎ  H‹ÎÿÓH‹ÈLğÑ  L‹Í‹×èfŠ  AÇG   é=ÖÿÿI‹H‹8H‹‡H  H‹˜  H‹ËÿÎ  H‹ÏÿÓH‹Œ$   ‹Ğè¨À  éõÔÿÿ‹Îé"ÕÿÿH‹BH…À„?Õÿÿ€8H‹Ğuëé2ÕÿÿHŠb éÆÕÿÿH^a éºÕÿÿHRc é®ÕÿÿH‹v` H‹ËÿMÊ  „ÀHd Híd HDÊé…ÕÿÿLİÒ  ëL¬â  H‹Aº   3ÉL‹HIƒÁÿ;Ì  ÇF   éwÖÿÿÌL§Ò  ëLvâ  H‹@º   3ÉL‹HIƒÁÿÌ  AÇF   é×ÿÿ‹º_ eH‹%X   º   H‹ÈH‹‹®_ ÿÈHcÈH‹H‹Èöˆ   „¥   I‹H‹8H‹‡H  H‹˜   H‹Ëÿ¸Ì  H‹ÏÿÓ…ÀtI‹H‹8H‹‡H  H‹˜˜   H‹Ëÿ’Ì  H‹ÏÿÓI‹H‹èH‹9H‹—H  H‹š   H‹ËÿmÌ  H‹ÏÿÓI‹‹ğH‹9H‹—H  H‹š    H‹ËÿIÌ  H‹ÏÿÓL‹ÍLòÏ  ‹ÖH‹Èèhˆ  AÇF   é)ÖÿÿÌHQ éQÖÿÿH‹ALvÑ  º   3ÉL‹HIƒÁÿáÊ  AÇF   é_×ÿÿH‹AL!á  º   3ÉL‹HIƒÁÿ´Ê  AÇF   é-×ÿÿHO HƒáøÿÙÈ  LGH‹ØHVHHÇ    Ç@   L‰xH‰xè†òÿÿéÖÖÿÿÿÉ  A¹\Ã  LG, º   ‰D$ 3ÉÿFÊ  éØÿÿ@„ÿ„ØÿÿD¶ÇI‹×H‹Ëè`Şÿÿé˜ÛÿÿM‹&M…ä…ŒÜÿÿérÜÿÿHS éØÿÿLĞ  ëLPà  H‹Aº   3ÉL‹HIƒÁÿßÉ  AÇG   éHÛÿÿHƒ}  …íØÿÿéÁØÿÿeH‹%X   H‹ğI‹ AAÿHcÈH‹H‹ÈH‹AXH‹ÏH…ÀH‰EHEÈH‹ÃHÿÀ€< u÷H‰EÈéÃØÿÿeH‹%X   H‹ğI‹ AAÿHcÈH‹H‹ÈH‹AHH‰E˜é¢ØÿÿeH‹%X   H‹ğI‹ AAÿHcÈH‹H‹ÈH‹A@H‰EˆéŒØÿÿº   I‹ÌèÎÿÿD‹è\ éØÿÿH‹Çé“ØÿÿHƒEˆeH‹%X   ¹   L‹<ğ‹¾\ LùÿÈHcĞI‹H‹H‹ÑHƒx0 uL+ º   3Éÿ¹È  éqØÿÿL‹×H‹×H‹ÏL‹ÏAµEˆl$,H‹EH…ÀLEĞH‹EÀH…ÀL‰T$8HEĞH‹E˜H…ÀH‰T$0Hñ* HEÈH‹E H…ÀH‰L$(LEÈH‹EˆH…ÀHEøH‹E¸L‹ÇH‰D$ 3Éÿ®Æ  I‰D$H‹øH‹7È  ‹I‹ÿÉHcÑH‹H‹ÑH‹×HÁø  ÿÈ  H…À„]  H‹H…Û„Q  ‹;\ 9C…¨×ÿÿH‹[H‹ËÿıÆ  …À„“×ÿÿHƒCøÿH‹CH‹I‰$‹•[ ÿÈHcĞI‹H‹H‹ÑHÿH I‹$H‹8H‹‡H  H‹˜ˆ   H‹Ëÿ¤È  H‹EÈH‹ÏL‹MÀL‹EH‹U˜H‰D$(Dˆl$ ÿÓ…À…‘   I‹$H‹8H‹‡H  H‹˜`  H‹Ëÿ_È  H‹ÏÿÓ‹[ ÿÈHcĞI‹H‹H‹ÑHÿ@I‹$H‹8H‹÷H‹‡H  H‹˜˜   H‹Ëÿ È  H‹ÏÿÓH‹øH‹†H  H‹˜   H‹ËÿÈ  H‹ÎÿÓ‹ÈH‹×èÎÙÿÿéÆ×ÿÿI‹<$H‹GH‹XH‹Ëÿ×Ç  º   H‹ÏÿÓIÇ$    éfÖÿÿ‹êZ º    ‰EüJáÿÑÃ  H‹ÈH‰E ²ÿÒÅ  H‹GH‰EH‹{Æ  ‹I‹ÿÉHcÑH‹H‹Ñ¹   öƒ   tÿÅ  ëÿ†Ä  EğHWL‹EòM LMğ H‹ø  H‰EğòHÇEø   ÿyÅ  éÉÕÿÿM‹HL;ÈŒ÷ÕÿÿL( º   3ÉÿêÅ  éÏØÿÿI‹P(Hƒúÿ„ØÕÿÿM‹H I‹@LÈL;ÊŒÄÕÿÿLs( ëÂÿËÆ  H‹ÏÿÓH‹øH‹†H  H‹˜   H‹Ëÿ¬Æ  H‹ÎÿÓ‹ÈH‹×èyØÿÿI‹$H‹8H‹÷H‹‡H  H‹˜˜   H‹Ëÿ|Æ  H‹ÏÿÓL‹ğH‹‡H  H‹˜   H‹Ëÿ]Æ  H‹ÏÿÓ‹øH‹†H  H‹˜    H‹Ëÿ?Æ  H‹ÎÿÓH‹ÈLèÉ  M‹Î‹×è^‚  ¶u`@„öu'I‹<$H‹GH‹XH‹ËÿÆ  º   H‹ÏÿÓIÇ$    I‹L$H…Ét.¶A¨uƒÿu¨tÿõÁ  ëÿåÂ  IÇD$    AÆD$, @„öu@8uPt	I‹ÌÿÂÂ  H‹MXÇA   éøÕÿÿ€}€ u	@„ö„ƒÕÿÿ‹EX ÿÈHcĞI‹H‹H‹ÑHÿ@éeÕÿÿH‹NX è9ÙÿÿH‹MXH‰ÇA  L‰pğéœÕÿÿÿ\Â  ‹şW é}×ÿÿH‹ËÿpÃ  ‹êW ÿÉLcÁH‹7H‹J‹ÂH‰€   éy×ÿÿ€y(HA IEÇH‹ L9xğ…ªØÿÿé=ØÿÿI‹ÎÿÿÁ  ÇG   éØÿÿ€{(HC IEÇH‹ éxØÿÿÌH‹JH…É„ÙØÿÿH‹Ñ@89uëéÌØÿÿH‹…W H‹ËÿTÁ  „ÀtH9X éÙÿÿH‹uW H‹Ëÿ4Á  „ÀtHZ éäØÿÿH‹-W H‹ËÿÁ  „ÀtHÙ\ éÄØÿÿH‹W H‹ËÿôÀ  „ÀH»Z H”[ HDÊé›ØÿÿÌ‹GH‹H‰T$0‰D$8ºà
sÿ€|$8tHL$0ÿÃ  H|$0é¸Øÿÿ‹™V eH‹%X   A¸   H‹ÈM‹ H‹»Â  ‹I‹ ÿÉHcÑH‹ĞéÏØÿÿÿJÃ  H‹X H‹Ëÿ¥Ã  H‹„$€   L‹ÍE‹ÇH‰D$ H‹×I‹ÎÿÓH‹Øé™ØÿÿöG	„ØÿÿH‹ƒ ÿ…‘ØÿÿH‹ÿóÂ  é‚ØÿÿÌÌÌÌÌH‹	Hÿ%n¿  ÌÌÌÌÌÌHùN ÃH‹ÄH‰XH‰hH‰pH‰x AVHƒìPH‹éI‹ñIH I‹øHƒáøH‹ÚÿµÀ  L‹ÇH‹ÓL‹ğHƒ` HHÇ    Ç@  H‰xèãéÿÿH‹„$€   H  H…öAÆD> LD$ L‰t$0HEÎÇD$(   H‰L$8H…ÀHp  I‹ÖHEÈHƒd$  H‰L$@H‹Íÿİ¿  H‹ØH…Àt/öE¹   tÿÀ  ëÿ”¿  D$0H‰òL$@ òHAŠF¨uAƒÿuI‹Î¨tÿ[¾  ëÿK¿  H‹\$`H‹l$hH‹t$pH‹|$xHƒÄPA^Ã@SHƒì €yH‹ÙuBH‹	H‹çT H‹Iÿ­¾  3Ò„Àt(H‹H‹HğH…Éu$H‹@L@Ç  QL‹HIƒÁÿ¯À  3ÀHƒÄ [ÃH‹H…ÀtH‹H‹ÂëêÌÌÌÌÌÌÌéËÅÿÿÌÌÌÌÌÌÌÌÌÌÌH‹ÄH‰XH‰hH‰pH‰x ATAVAWHƒìPH‹L‹á¹8   L‹òH‹Xøÿ‹¾  ‹SE3ÉL‹~À  ÿÂH‹ÈH‹ğÿ¾  H‹{‹[HÁãHßH;û„š   ‹-ÛS eL‹<%X   € txH‹HT$0H‹H‰L$0ŠA$öØHD$@ÉH‰D$ á ìÿÿE3ÉÁ  ‰L$8I‹ÌEAèrÕÿÿI‹ïA¹   M‹	H‹µ¿  ‹I‹	ÿÊLcÂJ;ÁtH‹T$0L‹ÀH‹Îÿô½  HƒÇ H;û…uÿÿÿL\$PAÇ   I‹[ H‹ÆI‹k(I‹s0I‹{8I‹ãA_A^A\ÃÌÌÌÌÌÌH‹ÄH‰XH‰hH‰pH‰x AVHƒì@L‹ñH‹òH‹	I‹éH‹A‹ø3ÛH‹Iøÿ™¿  H…ÀtH‹ ëH‹ÃH…À„	  …ÿ„š   ƒït(ƒÿtLBÅ  º   3Éÿİ¾  é  »   é÷   HD$0L‹ÍA¸   H‰D$ H‹ÖI‹Îè`Ôÿÿ‹rR H‹øeH‹%X   ¹   H‹ĞH‹H‹’¾  ‹H‹ÿÉHcÉH;<È„Ÿ   H‹Ïÿı¾  €”ÃéŠ   HD$0L‹ÍA¸   H‰D$ H‹ÖI‹ÎèóÓÿÿ‹R L‹ÀeH‹%X   ¹   H‹ĞH‹H‹%¾  ‹H‹ÿÉHcÉL;Èt6A€xI‹È•Ãÿ<¾  ë#ÿ¤¾  H‹XXH‹Ëÿÿ¾  L‹ÍD‹ÇH‹ÖI‹ÎÿÓ‹ØH‹l$X‹ÃH‹\$PH‹t$`H‹|$hHƒÄ@A^ÃÌÌÌÌÌÌÌÌÌÌHƒì(HÍÃ  3Éÿ½»  3ÀHƒÄ(ÃÌÌÌÌÌÌ@SHƒì H‹ÙH‹IğH…ÉtH‹	H…Étèa  H‹ËHƒÄ [é¤ÂÿÿÌÌÌÌH‰\$WHƒì H‹YğH‹ùH…ÛtHƒ; tH‹Kè‡´  Hƒ# H‹ÏH‹\$0HƒÄ _éeÂÿÿÌÌÌÌÌHƒì(HUÃ  3Éÿ-»  ƒÈÿHƒÄ(ÃÌÌÌÌÌL‹ÜI‰[I‰kI‰sWAVAWHƒì0€zM‹ùI‹èH‹úL‹ñt-‹GH‹I‰SØ‰D$(ºà
sÿ€|$(tHL$ ÿŞ¼  H|$ I‹6H‹NøH…Ét2H‹ÿ½  H…ÀtH‹ë3ÛH…ÛtH‹[H‹Ëÿ½  H‹ÕHNğÿÓë!ÿ½  H‹X(H‹Ëÿp½  M‹ÏL‹ÅH‹×I‹ÎÿÓHD$ H;øuöG	tH‹ƒ ÿu	H‹ÿÑ¼  H‹\$PH‹l$XH‹t$`HƒÄ0A_A^_ÃH‰\$H‰t$WHƒì H‹1H‹ùH…öt2H‹FH‹X`H‹Ëÿı¼  ²H‹ÎÿÓ…ÀtLÓÁ  º   3ÉÿÆ»  ëH‹OHH…Étÿå¹  H‹ÏÿÜ¹  H‹\$0H‹t$8HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌ@SHƒì H‹YH…Ût%E3ÀH'   H‹Ëÿf¼  H‹Ëÿí»  H‹Ëÿ”¸  HƒÄ [ÃÌÌÌÌÌÌH‰\$WHƒì H‹AH‹ùH‹XH‹ËÿJ¼  º   H‹ÏH‹ÃH‹\$0HƒÄ _HÿàÌÌH‰\$H‰l$H‰t$WHƒì H‹ZHI‹ğH‹ùH‹êH‹Ëƒæÿ¼  A¹   L‹Ç‹ÖH‹ÍH‹ÃH‹\$0H‹l$8H‹t$@HƒÄ _HÿàÌÌÌH‰\$H‰l$ VWATAVAWHƒì0D‹A,E3ÿH‹òL‰|$pH‹éEgEwE+Ät8E;Ätÿğº  é5  L‹tN HD$pLD$`H‰D$ HÖĞ  A‹Îÿ­º  ëL‹LN LD$`HÌ¾  A‹Ìÿº  ƒøÿ„è   H‹D$`H‹ H‹HğH…Éu'L–À  H‹@A‹Ö3ÉL‹HIƒÁÿÿ¹  D‰fé°   ƒyH‹9}	L>Ğ  ëÎH‹?H…ÿtïH‹D$pH…Àt4I;ÄtLFĞ  A‹Ö3Éÿ»¹  ëBH‹?H‹‡H  H‹X0H‹Ëÿºº  3ÒëH‹?H‹‡H  H‹X8H‹ËÿŸº  A‹ÖH‹ÏÿÓH‹ØH…ÀuD‰vë+º   I‹Ìÿ;º  HM H‰Ç@   €}(IEÏH‹	H‰AğH‹\$hH‹l$xHƒÄ0A_A^A\_^ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$UWATAVAWHƒì0L‹òL‹ù‹Q,ƒê„à   ƒútÿb¹  éÏ  L‹æL HD$pH‰D$(LD$`HD$x¹   H^½  H‰D$ ÿ¹  ƒøÿ„”  H‹D$`3íH‹ H‹HğH…É„¸   ƒyH‹}LãÎ  éª   H9+tïºP   J±ÿK¹  H‹L‹àH‹9H‹—H  H‹ZPH‹Ëÿn¹  H‹ÏÿÓI‰$H‹øH…À„Õ   H‹HH‹H‹ËÿI¹  L‹D$pH‹ÏH‹T$xÿÓé±   L‹L LD$`H–¼  ¹   ÿW¸  ƒøÿ„Ø   H‹D$`3íH‹ H‹HğH…Éu-L\¾  H‹@º   3ÉL‹HIƒÁÿÃ·  AÇF   é˜   ƒyH‹ŒÿÿÿH9+„ÿÿÿºP   J±ÿj¸  L‹àH‹H‹8H‹‡H  H‹XPH‹Ëÿ¸  H‹ÏÿÓI‰$I9,$uI‹Ìÿµ  AÇF   ë5º   Jéÿ¸  ‹ÍL‰ Aƒ,•ÁƒÁ‰HIO A€(HEÍH‹	H‰AğH‹\$hHƒÄ0A_A^A\_]ÃÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰l$H‰t$WHƒì ‹®J 3öeH‹%X   H‹Ù½   H‹<ĞH94/u
ÿ·  H‰/H‹l$8HƒÈÿH‰CH‰C(¸   H‰sH‰sH‰s H‰s8H‰s@H‰sHH‰sXH‰sPH‰s`H‰³ˆ   H‰³   H‰³    @ˆ³¨   H‹t$@H‰C0H‰ChH‹\$0HƒÄ _ÃÌH‰\$H‰t$WAVAWHƒìPH‹ÆB H3ÄH‰D$@H‹ùÿu´  A¿   L¸Ë  A‹ÏH¶Ë  ÿ´  ÿò³  H³Ë  A‹ÏL‹Àÿ·¶  eH‹%X   Aw‹¤I L©Ë  A¾   ‹ÖH‹È‹›I ÿÈLcÈJ‹3H‹N‹ÉHL$ M‹Iÿ´  LD$ A‹ÏHuË  ÿW¶  ‹aI LVË  ÿÈ‹ÖLcÈJ‹3H‹N‹ÉHL$ M‹I ÿÁ³  LD$ A‹ÏHJË  ÿ¶  ‹I LË  ÿÈHL$ LcĞ‹ÖJ‹3L‹O‹ÑM‹Iÿ~³  LD$ A‹ÏH'Ë  ÿÑµ  ÿ{µ  H‹Ïÿâ´  H‹L$@H3ÌèıÍÿÿL\$PI‹[(I‹s0I‹ãA_A^_ÃÌÌÌÌÌÌÌ@SHƒì HKI ‹Úÿ‹´  HTJ ÿ~´  H÷H ÿq´  HúM ÿd´  HH ÿW´  H O ÿJ´  ‹ËÿÊ²  3ÀHƒÄ [ÃÌÌÌÌÌÌÌÌÌÌH‹ÄH‰XH‰pH‰xUATAUAVAWH¨˜üÿÿHì@  ‹òÿâ´  ‹ÖHiÌ  H‹Øÿ8²  ÿÊ´  L%£ ÿÿº€   IŒ$ğ« E3ö HI@ A H0I0@@A@HPIP@`A`HÊ@pHÂAğ	@AH I @0A0H@I@@PHÎ¸ÿÿAPH‰L IŒ$0¨ ÇñK    HÉÿÿH‰L H\öÿÿH‰ıK L‰5æK H‹C@H‰L HìóÿÿH‰L H®òÿÿH‰_L I„$ğ«  H@ IH0A @@I0HPA@@`IPHpA`HÊHÂIğ H@ IA H0@@I0HPHòÿÿA@IPIŒ$P© H‰_G I„$ğ«  H@ IH0A @@I0HPA@@`IPHpHÂA`HÊ IğH@ IH0A @@I0HPHî¶ÿÿA@IPIŒ$­ H‰ïG I„$ğ«  H@ IH0A @@I0HPA@@`IPHpA`HÊHÂIğ H@ IH0A @@I0HPH.¾ÿÿA@IPH‰'K IŒ$0ª I„$ğ«  H@ DjE3ÀIEŠÍH0A @@I0HPA@@`IPHpHÂA`HÊ IğH@ IH0A @@I0HPHdóÿÿA@IPIŒ$« H‰¥G I„$ğ«  H@ IH0A @@I0HPA@@`IPHpHÂA`HÊ3Ò IğH@ IH0A @@I0HPHóÿÿA@IPHÓJ H‰ôG ÿ®  D‹ÎL¸  H¥ôÿÿ3Éÿõ­  AN0‰kD ÿİ®  A¿  H‹øD‰(D‰xL‰pHÇ@   â·  @‹è·  ‰H(H‹Î­  Dˆp,H‹H‹Ëÿî°  H‹ÏÿÓH‹ª­  H@  WÀH‰…H  WÉL‰µ(  H@í  f…0  L‰µÈ  L‰µØ  L‰µà  f…`  L‰µp  f@  f…P  L‰µx  f€  L‰µè  L‰µğ  L‰µP  L‰µø  L‰µ  f…  L‰µ   L‰µĞ  L‰µ  L‰µ(  H‰…   H‹ÿÆ¬  E3ÉÇD$    EFH‰ØB Hé¶  H‹ÈƒHÿä¬  H‹½B LÖ¶  EFÇD$    HË¶  ÿ•¯  AN(ÿk­  H‹øD‰(D‰xL‰pHÇ@   ò­¶  ò@‹ª¶  ‰H Š¥¶  ˆH$H‹W¬  Dˆp%H‹H‹Ëÿw¯  H‹ÏÿÓWÀH‰D$HfE0L‰u(WÉL‰µØ   H°ë  L‰µà   H‰…   HL$@HÃÿÿfE`H‰…È   L‰upfM@fEPL‰uxf€   L‰µè   L‰µğ   L‰t$PL‰µø   L‰µ  f…  L‰µ   L‰µĞ   L‰µ   L‰µ(  ÿõ­  L=.B EŠÍL\ëÿÿH‰A I‹Ï3ÒH‹Øÿ_«  A‹ÎH­µ  A‹şJ‹„!(/ N‹Œ! / N‹„!/ I‹ÏH‰D$ è-ëÿÿIıH‹ÏHÁáJ‹”!/ H…ÒuÄzA¿   E‹ÏH[µ  D‹ÇH‹ËÿG«  E‹ÏDGHQµ  H‹Ëÿ0«  E‹ÏDGHJµ  H‹Ëÿ«  E‹ÏDGıHCµ  H‹Ëÿ«  E‹ÏDGşH<µ  H‹Ëÿëª  E‹ÏH9µ  D‹ÇH‹ËÿÕª  H‹SH*A ƒKLD$0HG H‰D$0ÇD$8   ÿçª  Oÿ6«  H‹øD‰(Ç@  L‰pHÇ@   ‹ã´  ‰H·İ´  f‰HH‹*ª  DˆpH‹H‹ËÿJ­  H‹ÏÿÓWÀH‰D$HWÉfE0fE`fM@fEPf€   f…  L‰u(L‰µØ   L‰µà   L‰upL‰uxL‰µè   L‰µğ   L‰t$PL‰µø   L‰µ  L‰µ   L‰µĞ   L‰µ   L‰µ(  H Ì  H‰…   HL$@H}ÀÿÿH‰…È   ÿÈ«  EŠÍL6éÿÿH? H‰h? 3ÒH‹Øÿ5©  I‹ÎHë³  I‹şJ‹„!ˆH N‹Œ!€H N‹„!xH HA? H‰D$ èÿèÿÿIıH‹ÏHÁáJ‹”!pH H…ÒuÀDBE‹ÏHš³  H‹Ëÿ!©  ¿   H³  D‹ÇE‹ÏH‹Ëÿ©  E‹ÏDGH³  H‹Ëÿï¨  E‹ÏDGHa³  H‹ËÿØ¨  E‹ÏDGHZ³  H‹ËÿÁ¨  E‹ÏDGúHS³  H‹Ëÿª¨  E‹ÏDGúHD³  H‹Ëÿ“¨  E‹ÏDGÿH5³  H‹Ëÿ|¨  E‹ÏH2³  D‹ÇH‹Ëÿf¨  E‹ÏDGşH(³  H‹ËÿO¨  E‹ÏDGùH³  H‹Ëÿ8¨  E‹ÏDGşH³  H‹Ëÿ!¨  E‹ÏH³  D‹ÇH‹Ëÿ¨  E‹ÏDGH³  H‹Ëÿô§  E‹ÏDGùHş²  H‹Ëÿİ§  E‹ÏDGıH·±  H‹ËÿÆ§  E‹ÏDGHØ²  H‹Ëÿ¯§  E‹ÏDGşHÙ²  H‹Ëÿ˜§  E‹ÏDGHÒ²  H‹Ëÿ§  H‹SH^= LD$0H‰D$0H½C ÇD$8   ÿ—§  Oÿæ§  H‹øD‰(Ç@  L‰pHÇ@   ò…²  ò@‹‚²  ‰H ·|²  f‰H$H‹Í¦  Dˆp&H‹H‹Ëÿí©  H‹ÏÿÓWÀH‰D$HWÉL‰u(Hzá  fE0H‰…   HL$@H’½ÿÿL‰µØ   H‰…È   L‰µà   fE`L‰upfM@fEPL‰uxf€   L‰µè   L‰µğ   L‰t$PL‰µø   L‰µ  f…  L‰µ   L‰µĞ   L‰µ   L‰µ(  ÿk¨  EŠÍLÙåÿÿHrA H‰û; 3ÒH‹ØƒHÿÔ¥  I‹ÎI‹şH±  J‹„!ÈK N‹Œ!ÀK N‹„!¸K H0A H‰D$ èåÿÿIıH‹ÏHÁáJ‹”!°K H…ÒuÀDBE‹ÏH1±  H‹ËÿÀ¥  E‹ÏH¯  A¸   H‹Ëÿ§¥  E‹ÏH=°  A¸   H‹Ëÿ¥  H‹SH»@ LD$0H‰D$0HÊA ÇD$8   ÿ¤¥  ¹(   ÿñ¥  H‹øD‰(Ç@  L‰pHÇ@   ò¨°  ò@‹¥°  ‰H Š °  ˆH$H‹Ú¤  Dˆp%H‹H‹Ëÿú§  H‹ÏÿÓWÀH‰D$HWÉL‰u(HçÅ  fE0H‰…   HL$@HŸ»ÿÿL‰µØ   H‰…È   L‰µà   fE`L‰upfM@fEPL‰uxf€   L‰µè   L‰µğ   L‰t$PL‰µø   L‰µ  f…  L‰µ   L‰µĞ   L‰µ   L‰µ(  ÿx¦  EŠÍLæãÿÿH¿; H‰ø9 3ÒH‹Øÿå£  I‹ÎH«¯  I‹şJ‹„!K N‹Œ! K N‹„!øJ H; H‰D$ è¯ãÿÿIıH‹ÏHÁáJ‹”!ğJ H…ÒuÀDBE‹ÏHZ¯  H‹ËÿÑ£  E‹ÏH‡®  A¸   H‹Ëÿ¸£  E‹ÏH>¯  A¸   H‹ËÿŸ£  E‹ÏH-¯  A¸   H‹Ëÿ†£  E‹ÏH ¯  A¸   H‹Ëÿm£  H‹9 H÷™  L‹0£  A‹ÕH‰  H§2 H‰P  M‹ ÿ7¥  H‹SH¤: LD$0H‰D$0Hs? ÇD$8   ÿM£  ¹(   ÿš£  H‹øD‰(Ç@  L‰pHÇ@   ò‘®  ò@·®  f‰H Š…®  ˆH"H‹¢  Dˆp#H‹H‹Ëÿ¡¥  H‹ÏÿÓWÀH‰D$HWÉfE0fE`fM@fEPf€   f…  L‰u(L‰µØ   L‰µà   L‰upL‰uxL‰µè   L‰µğ   L‰t$PL‰µø   L‰µ  L‰µ   HŒÀ  L‰µĞ   H‰…   HL$@Hâ¸ÿÿL‰µ   H‰…È   L‰µ(  ÿ¤  EŠÍLáÿÿH8 H‰Ç7 3ÒH‹øÿŒ¡  I‹ÎHB¬  I‹ŞJ‹„!(G N‹Œ! G N‹„!G HØ7 H‰D$ èVáÿÿIİH‹ËHÁáJ‹”!G H…ÒuÀDBE‹ÏHñ«  H‹Ïÿx¡  E‹ÏHV¬  A¸	   H‹Ïÿ_¡  A¾   Hê¬  E‹ÆE‹ÏH‹ÏÿC¡  A^E‹ÏD‹ÃHò¬  H‹Ïÿ)¡  E‹ÏHß«  D‹ÃH‹Ïÿ¡  EfıE‹ÏE‹ÄH¢«  H‹Ïÿù   E‹ÏH—«  E‹ÄH‹Ïÿã   E‹ÏEFH…«  H‹ÏÿÌ   E‹ÏHªª  E‹ÆH‹Ïÿ¶   E‹ÏEFúHt¬  H‹ÏÿŸ   H‹WH¼6 LD$0H‰D$0HÛ< ÇD$8   ÿµ   A^û‰t$ EnD‹ËA‹ÕH,¬  E‹Äÿ‹¢  Ef‰t$ A‹ÔEFüD‹ËH*¬  ÿl¢  D‹Ë‰t$ E3ÀSH0¬  ÿR¢  E‹Æ‰t$ D‹ËH9¬  EpA‹Öÿ4¢  D{‰t$ A‹×H2¬  D‹ËD‹Ãÿ¢  {‰t$ ‹×H.¬  D‹ËA¸Ê   ÿ÷¡  D‹Ë‰t$ _A¸Ë   ‹ÓH&¬  ÿØ¡  EMê‰t$ A¸É   H+¬  ‹Óÿ»¡  _å‰t$ D‹ËWEEüH+¬  ÿ¡  D‹Ë‰t$ EEA‹ÔH8¬  ÿ‚¡  A¼   ‰t$ E‹ÄWóD‹ËH8¬  ÿb¡  D‹Ë‰t$ WøEEH5¬  ÿG¡  D‹Ë‰t$ »   H4¬  D‹ÃA‹Õÿ(¡  Wü‰t$ EMêA¸   H0¬  ÿ
¡  EMê‰t$ EE÷A‹ÖH4¬  ÿî   ‰t$ EuêE‹ÎWúEEéH-¬  ÿÏ   E‹Î‰t$ WA¸   @H0¬  ÿ²   E‹Î‰t$ WEE'H=¬  ÿ—   E‹Î‰t$ WA¸  @ HP¬  ÿz   Eo
‰t$ E‹ÅW	E‹ÎHb¬  ÿ\   E‹Î‰t$ E3ÀHs¬  A‹×ÿB   E‹Î‰t$ WóEGîHm¬  ÿ'   E‹Î‰t$ EwùA‹ÖEGõHf¬  ÿ   EOğ‰t$ EGıA‹ÕHZ¬  ÿìŸ  Eoğ‰t$ E‹ÍEGîA‹ÖH[¬  ÿÍŸ  E‹Í‰t$ WìEGïHP¬  ÿ²Ÿ  E‹Í‰t$ E‹ÅWíHF¬  ÿ˜Ÿ  E‹Í‰t$ E3ÀWH<¬  ÿ~Ÿ  ‰t$ E‹ÍEGîEo	A‹ÕHE¬  ÿ_Ÿ  EOğ‰t$ E3ÀHM¬  A‹ÕÿDŸ  EOğ‰t$ EGîA‹ÕHN¬  ÿ(Ÿ  Wÿ‰t$ EOğEGïHR¬  ÿŸ  Wÿ‰t$ EOğEGñHV¬  ÿğ  EOğ‰t$ EGï‹×H[¬  ÿÕ  A‰t$ ‹×EOğEGîH\¬  ÿ¶  EOğ‰t$ EGïA‹×HX¬  ÿš  E~÷‰t$ E‹ÏWEFøHQ¬  ÿ{  E‹Ï‰t$ WEFüHN¬  ÿ`  Gü‰t$ D‹ÀHW¬  ‹ĞE‹ÏÿD  E‹Ï‰t$ EF‹×HP¬  ÿ*  E‹Ï‰t$ EF4‹×HN¬  ÿ  E‹Ï‰t$ WA¸   HI¬  ÿó  E‹Ï‰t$ D‹ÃWHO¬  ÿÙ  A~‰t$ ‹×HP¬  E‹ÏE‹Äÿ¼  » €  ‰t$ D‹ÃHA¬  E‹Ï‹×ÿ  E‹Ï‰t$ WA¸ @  H/¬  ÿ  E‹Ï‰t$ D‹ÃWH-¬  ÿg  E‹Ï‰t$ WA¸   H(¬  ÿJ  Ef‰t$ A‹ÔEFtE‹ÏH!¬  ÿ+  E‹Ï‰t$ A¿   H¬  E‹ÇA‹Õÿ  W
‰t$ EN÷A¸    H¬  ÿíœ  _‰t$ ‹ÓEN÷E3ÀH¬  ÿĞœ  W‰t$ EN÷EFõH¬  ÿ´œ  W‰t$ EN÷EFöH¬  ÿ˜œ  Gô‰t$ D‹ÈWD‹ÀH¬  ÿ{œ  W‰t$ EN÷EFøH¬  ÿ_œ  EN÷‰t$ EFùA‹ÔH	¬  ÿCœ  W‰t$ EN÷EFúH¬  ÿ'œ  W‰t$ EN÷EFûH¬  ÿœ  W‰t$ EN÷EFüHı«  ÿï›  W‰t$ EN÷EFıHù«  ÿÓ›