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
  key_BIMZ�       ��  �       @                                     � �	�!�L�!This program cannot be run in DOS mode.
$       u���1���1���1���8�L�7���
���3���
���3���
���:���
���;����^�3������2���1��������:������0���� �0������0���Rich1���                PE  d� �&�]        � "      �       0        �                        �    �k  `A                                    � l   l� �    �  	   � \           � �  P T                   H (   � �             �                          .text   ��                          `.rdata  Č     �                @  @.data   (   �     �             @  �.pdata  \   �     �             @  @.tls        �     �             @  �.rsrc    	   �  
   �             @  @.reloc  �   �     �             @  B                                                                                                                                                                                                                        E3�E3��   �����A�   E���   ��H�\$UVWATAUH�l$�H���   E3�Ic�L�m�E��H��A��E���{  �y(L�m���+  A�ՋI,H�E�H�D$0L�M�H�E�H�D$(L�k H�Ė H�D$ �� ����  L�M�M����+  H�\ H�H����+  �C���+  �   H�M�H�E�H�H�B�H����+  �xL��$  L�8��+  H�A�H���S-  L��$  D��A������+  �   I�HD�H���Q A�   L��A��I����L��$  E��t[�~uUH�E3�H�E�H�ӋFH�ΉE��� D9k ��+  L9�  ��+  H�H�E�H�A L9��   ��+  L9m���,  L��$  H��$   H���   A]A\_^]�E����*  �y(H�E�   t:I�ՋI,H�E�H�D$(L�M�H�P� L�� H�D$ �n ���t�H�M�����H�Q ������������L��I�[I�kI�s WH��0H�%� M�KH��I�C�H�Q 3�y(L�� �I,HE�� ���tpH�D$@�UH� H�H�H����+  H�99Q��+  H9/��+  D�A3�H���$  H�D$@H�H�B��hH�D$@H�H�K����  H��H�k����  �F   H�\$HH�l$PH�t$XH��0_���L��I�[I�kVWATAVAWH��0H�R� M�KL��I�C�H�Q 3��y(L�� �I,HE��9  �����   H�D$`H� H�H�H���I+  �yL�9�E+  I�?H���9+  H�?H��H  H�XhH����  H���Ӌ�� Lc�eH�%X   �   H��H���� ��Hc�H�H�����   �+  I�����A�FH�\$hH�l$pH��0A_A^A\_^������@UAVH��83�L��y(H�l$P�x+  ��H�K� L�L$P�I,L�� H�D$ �A�  �����   H�D$PH�H�A�H���@+  �xH�|$0H�8�[+  H�?H���O+  H�?H�\$XH�t$`H��H  H���   H�����  H����H��H��tRH���H��@8,u�H�K H������  L��H��H��H�H�    �@   H�hH�X�&  @�lI�>A�F  H�t$`H�\$XH�|$0H��8A^]������������@SH��0�y(H����*  3�H�� L�L$@�I,L�� H�D$ �*�  ���t]H�D$@H�H�A�H����*  �xH�|$HH�8��*  H���   H�����  3�H����H�D$@H�H�K����  H�C�    H�|$HH��0[���@VH��0�y(H����*  3�H��� L�L$@�I,L� H�D$ ���  ���tjH�D$@H�H�A�H���S*  �xH�|$PH�8�m*  H�?H���a*  H�?H�\$HH��H  H�XHH���>�  H����H�\$H��u�   �FH�|$PH��0^ø   ����������������@VH��0�y(H���)*  3�H�Ԑ L�L$@�I,L�Q H�D$ ���  ���tmH�D$@H�H�A�H����)  �xH�|$PH�8�*  H�?H���*  H�?H�\$HH��H  H���   H���{�  H����H�\$H��H��F   H�|$PH��0^���AVH��0�y(L����)  3�H�$� L�L$@�I,L��  H�D$ ��  �����   H�D$@H�H�A�H����)  �xH�|$XH�8��)  H�?H����)  H�?H�\$HH�t$PH��H  H���   H�����  H����H��H��tVH���H�À< u�H�K H������  L��H��H��H�H�    �@   H�@    H�X�m#  �D I�>A�F  H�t$PH�\$HH�|$XH��0A^��H�\$H�t$WH�� H�y�H��H��t1H�H��t)H�; tD�G3�H���a   H���  H����  �g H��H�\$0H�t$8H�� _�   �������������@SH�� H��H�I�H��t���  H��H�� [H�%i�  �H��H�XH�hH�pH�x ATAVAWH�� D�<� ��A�   L��A��~'eH�%X   J��M��$� ��Hc�I�H��H�JA�~, �C(  I�>H�GH�XH���5�  ��H����I�& I��H�\$@H�l$HH�t$PH�|$XH�� A_A^A\�    @SH�� H��H�IH���p)  �{ ��)  H�� [�������H��VAWH��h3�L���y(H�pH�p�u)  �֋I,H��$�   H�D$8L�L$@H��$�   H�D$0L��  H��$�   H�D$(H�`� H�D$ �e�  �����  H�l$`H��$�   H���)  H��$�   H��H���H��tH��������(  H�D$@H�H�B�H���)  �xL�t$PL�0�#)  H�|$XI�>H���?)  H��$�   A9v(�Z)  I�H�8H��H  ����)  H�XH�����  H��$�   L��H���Ӆ���)  I�H�8H��H  H��  H�����  H���Ӆ���  H��$�   H��H�����*  �I�H�8H��H  H�X8H����*  �F�  �   H����H��H����*  �ڋ eH�%X   �   H��H��΋ ��Hc�H�H�����   ��*  �   �J����  H��� L��H�(�@   �C   ��*  �   �C +��   Hc�H��H��H�f�  �;H��H����*  H�RH��� ���  H��tH�0H�}H�uH��H�����  H��H���>�  H�?� H����  ���}*  H�� H�����  ���q*  H�� H�����  ���e*  H�׊ H�����  ���Y*  H�� H�M(I�?A�G  L�w�H��$�   H�|$XL�t$PH�l$`H��hA_^Ëo� eH�%X   �   H��H��c� ��Hc�H�H�����   �x(  A�G   ���������L��SVWH��@I�C H��I�C�H�Q I�C3ۀy(M�K�I,L���  I�C�HE�H� � I�C��&�  ���tYH�D$`H�H�A�H����)  �xH�8��)  H�?H����)  H�?H��H  H�XH�����  H�T$pH������Ƀ��NH��@_^[�L��SUVWATAVAWH��@I�C L��I�C�H�Q I�CE3�y(M�K�I,L��  I�C�IE�H�h� I�C��n�  H���;�ttH��$�   H� H�H�H���)  �yL�9�)  I�H��� )  H�)H��$�   H��H  H��D8$>u�H�[xH����  L��H��H���Ӆ���(  A�F   H��@A_A^A\_^][����AVAWH��XE3�L��y(L�|$p��)  A�׋I,H��$�   H�D$0L�L$pH��$�   H�D$(L��  H��� H�D$ ���  �����   H�D$pH�H�A�H���V)  �xH�|$@H�8�q)  L9?�h)  H��$�   H�\$xH�l$PH�t$HH� H�K H����S�  H���    �@   L�xH�XH�H�9H��H  H�ZH�����  L��$�   H�VL��$�   H�����FH��H�F�)  ���uDH�W H��H����W�  H��H�xL�xH�t$HH�l$PI�H�\$xA�F  H�|$@H��XA_A^��ȉ�(  ���@SH�� H�A�H��H��uH�K�H��uDH��H�� [H�%�  H�t$0H�0H��t!H�|$8H���   H���,�  3�H����H�|$8H�t$0��+�  ����������A�E3��   �����E3�A��   �����H�\$ D�D$H�T$UVWATAUAVAWH�l$�H��   E3�A���y(A��L��H��E���  L�e�L�e�E2�L�e�L�e�L�e�L�e�L�e�L�e�L�e�L�e�L�e�L�e�L�e�D�e�D�eP���  �ȸ��Q�������  ��'  �K(����  L�e�L�e�L�e�L�e�L�e�@����  ����'  I�ԋK,H�E�H��$�   L�M�H�E�H�D$xL�v�  H�E�H�D$pH�E�H�D$hH�E�H�D$`H�E�H�D$XH�E�H�D$PH�E�H�D$HH�E�H�D$@H�E�H�D$8H�E�H�D$0H�E�H�D$(H�u� H�D$ �z�  �����  H�E�H�L�q�M���E'  A�~M�&�@'  I�<$ �5'  H�E������H#�H��H�E�H�}� �5܄ D�� A�   �/'  eH�%X   H��I� A�A�Hc�H�H��H�APH�E�H�}� �R  H���H�=�+ H�}� ��&  H�}� �2'  H�}� �S'  H�}� �H'  I�<$ tM��tE�FA���Z'  H�E�H���f'  �H�À< u�H��v&H�M�H���  A�   ���  ���?'  D�� eH�%X   �   L�<�A�A�L�Hc�I�H�L��I�@H�����)  E���*  I�<$ ��  �E�L�M�L�E�H�U�I�$�D$P   �D$HH�E�H�D$@�E��D$8�E��D$0H�E�H�D$(�EȉD$ �7�  H��I�$H�8H��H��H  H���   H����)  ���  H����H��H��H  H���   H���v�  H���Ӌ�H���C  I�$H�0�� ��Lc�I�H��H  H�H��(  H��J���4�  L�Ch�   H����M����  �u`A�F   E���n*  � ��Hc�I�H�H��H�@A�D$(    H�M�H���l*  H�	H��� H�I�}�  ���P*  H�E�H�L�q�H�MX@��t�A   H��$x  H��   A_A^A]A\_^]�H�A H�E������H�C H���3���D9c,�)����$  �K,H�E�H�D$`L�M�H�E�H�D$XL�E�H�E�H�D$PH���  H�E�H�D$HH�E�H�D$@H�E�H�D$8H�E�H�D$0H�E�H�D$(H�E�H�D$ �5�  ����J���H�M�H��t,H�	H��� H�I���  ��tH�E�H�L�q�M���}#  �8   �J��i�  L���EPH�M�   �^���eH�%X   H��I� A�A�Hc�H�H��H�A8H�E�����E3�A��A�H�@�  I�$H��t"�E��;����   �J����  L��L� �����u`�.(  H�\$H�t$WH�� eH�%X   H��D��� �   J�<���� ��Lc�L�7I�N��I�Hx��� �B�Hc�I�H��H���   H���r(  H��t	�; �u(  �B�Hc�H�7H�H��H���    H�\$0H�t$8H�� _��������������H�\$H�t$H�|$AVH�� L���2�  H��H��tSH���H�ǀ<8 u�H�O H����_�  L��H��H��H�` H�H�    �@   H�x�  �D; I�A�F  H�\$0H�t$8H�|$@H�� A^��E3��    H�\$H�l$H�t$WAVAWH�� E3�A��H��H��E����'  �8   �rɋ��c�  E3�@�֋�L���z�  I�H����'  �V���;�  H��L�0�@   @����'  H�F �1   H��G  H�p�H�\$@H�l$HH�t$PH�� A_A^_�����������H�\$H�t$WH�� �Q�   �A H��������#�+�Hc�H��H��H���  H��H��@8;�'  H�RH�=� �7�  H��tYH� H�~H�FH��H�����  H��H�����  H��~ H���O�  ����&  H�P� H�\$0H��H�N(H�t$8H�� _���������H�\$H�l$H�t$WAVAWH��@�zI��H�1E��H��L���'  H�N�H���r'  H����  H��t]H�H���X'  H�[H����  H��$�   H�N���H��H����&  H�D$0H;��Y'  H�l$hH��H�\$`H�t$pH��@A_A^_�3����̋b} eH�%X   A�   H��M� �U} ��Hc�I� H��H�AxH�H���B   ����������������H��(�} eH�%X   �   H��H��} ��Hc�H�H��H���   H��u3�H��(��5�  ����̋�| E3�eH�%X   A�   H��M� ��| ��Hc�I� H��L���   ��| ��Hc�I� H��L�Ix��| ��Hc�I� H��3�L���   ������H��� H���B   ��������������̋2| eH�%X   A�   H��M� �%| ��Hc�I� H��H���   H�H���B   ������������̋�{ eH�%X   A�   H��M� ��{ ��Hc�I� L��H�I���   3�����̋�{ eH�%X   A�   H��M� ��{ ��Hc�I� H��H�AxH��B   ���H�\�  H���B   ���������������H�A`������������H�ApH��B   ���%J�  �����������%�  �����������%��  �����������%��  �����������%��  �����������%��  ����������������ff�     H;ys �uH��f�����u��H����  ���������������H��(��t9��t(��t��t
�   H��(��  ��s  ��H��(�I��H��(�   M����H��(�,  H�\$H�t$H�|$ AVH�� H��L��3��  ��u3���   �  �؈D$@@��=~y  t
�   �	  �hy    ��  ��tg�
  H�  �  ��  H�	  �	  �$	  H�!�  H��  �-  ��u)�`  ��t H���  H���  �  ��x    @2����  @���N�����  H��H�8 t$H���b  ��tH�H���
  L�ƺ   I�����0s �   H�\$0H�t$8H�|$HH�� A^��H�\$H�t$WH�� @���r 3ۅ�3��P�ȉ�r �m  @���D$8�=Wx t
�   �[  �z  �@x �  @���_  3�@���y  ���Ë�H�\$0H�t$@H�� _���H��H�X L�@�PH�HVWAVH��@I����L���u9dr 3��   �B���w*�   �؉D$0����   L�Ƌ�I�������؉D$0��tvL�Ƌ�I���,  �؉D$0��u+��u'L��3�I���  L��3�I���c���L��3�I���N   ��t��u*L�Ƌ�I���@����؉D$0��tL�Ƌ�I���!   �؉D$0�3ۉ\$0��H�\$xH��@A^_^����H�\$H�l$H�t$WH�� H�e�  I����H��H��u�C�H����  L�ǋ�H����H�\$0H�l$8H�t$@H�� _�����H�\$H�t$WH�� I����H���u�  L�ǋ�H��H�\$0H�t$8H�� _�s������@SH�� H��3����  H�����  ���  H�Ⱥ	 �H�� [H�%��  ������������H�L$H��8�   �k
  ��t�   �)H�Kq �   H�D$8H�2r H�D$8H��H��q H�r H��p H�D$@H��q �fp 	 ��`p    �jp    �   Hk� H�bp H�   �   Hk� H��n H�L �   Hk�H��n H�L H���  �����H��8����@SVWH��@H����  H���   3�E3�H�T$`H�����  H��t9H�d$8 H�L$hH�T$`L��H�L$0L��H�L$pH�L$(3�H�\$ ���  �ǃ�|�H��@_^[����H��(��  ��t!eH�%0   H�H�H;�t3��H��t u�2�H��(ð�����H��(�  ��t�  ��o  ���  ��t2���  �H��(�H��(3��A  ����H��(����H��(�  ��u2���  ��u�  ��H��(�H��(�  �~  �H��(����H�\$H�l$H�t$WH�� I��I����H����  ��u��uH����  L��3�H����H�T$X�L$PH�\$0H�l$8H�t$@H�� _��  ���H��(�  ��tH��s H��(��  ��  ��u��  H��(�H��(3���  H��(��  @SH�� ��s �ɻ   DÈ�s �j  �  ��u2���  ��u	3��  ���H�� [����H�\$UH��H��@�ك���   ��  ��t+��u'H� s �+  ��t2��zH�$s �  �����gH�)l I����¹@   ��?+ȰI��L3�L�E�L�E�E�L�E��M��r L�E�L�E�E�L�E���r �M��r ��r H�\$PH��@]ù   �  ����H��L���MZ  f9����uyHc����H�����H��9PE  u_�  f9AuTL+��AH�QH��AH��L��H�$I;�t�JL;�r
�B�L;�rH��(��3�H��u2���z$ }2��
��2��2�H������@SH�� ���  3҅�t��uH��q H�� [�@SH�� �=�q  ��t��u����  ����  �H�� [��@SH�� H��j H�ً�H3{q ��?H��H���u
H���  �H��H�[q �l  3Ʌ�HD�H��H�� [��H��(����H�������H��(��H�\$ UH��H�� H�e H�2��-�+  H�9j H;�uoH�M�B�  H�EH�E�<�  ��H1E�8�  ��H�M H1E�0�  �E H�MH�� H3E H3EH3�H�������  H#�H�3��-�+  H;�HD�H��i H�\$HH��H��i H�� ]�H��(��uH�=��   u���  �   H��(��H��p H�%��  ��������������H�yp �  H�}p �H�}p �H��(�����H������H�H��(��H�qp �H�\$UH��$@���H���  �ٹ   �  ��t���)�%0p  H�M�3�A��  �  H�M��U�  H���   H���  H��E3��3�  H��t<H�d$8 H���  H���  L��H�L$0L��H���  H�L$(H�M�H�L$ 3����  H���  H�L$PH���   3�H���  A��   H��H���   �  H���  H�D$`�D$P  @�D$T   �6�  ��H�D$PH�D$@H�E���H�D$H3��e�  H�L$@�b�  ��u
���!,o H��$�  H���  ]����H�\$H�t$WH�� H��E H�5�E �H�;H��t
H���u   ��H��H;�r�H�\$0H�t$8H�� _���������������H�\$H�t$WH�� H�jE H�5cE �H�;H��t
H���   ��H��H;�r�H�\$0H�t$8H�� _���H�%��  �H�\$H�|$UH��H�� �e� 3�3��4g    �D���!g    ��cAMDD��D��A��entiA��ineIA��ntelE�D��D�n A��AuthEً�Dف�Genu3ɋ�DҸ   ��E�D��D�M��ȉ]�U�E��uRH��f �A��%�?�D��m =� t(=` t!=p t������ wH�     H��sA��D��m E��u�� ��� ` rA��D�em �   �U�D�M�;�|$3���E��]�M��U��]���	sA��D�1m A��sn�f    ��e    A��sSA��sL3��H�� H�H�UH�E$<u2��e ����e    �E� ��e t�� ��e    ��e H�\$83�H�|$@H�� ]��̸   ���3�9�e ����  ��������������%��  �%��  �%��  �%8�  �%*�  �%�  �%�  �% �  �%*�  �%��  �%��  �%��  �%��  �%4�  ���3���H��(M�A8H��I���   �   H��(����@SE�H��A���L��A� L��tA�@McP��L�Hc�L#�Ic�J�H�C�HHK�At
�A���L�L3�I��[�������%�  ����������ff�     ��@UH�� H��M@H�� ]�4����@UH�� H���]����M8H�� ]�����@UH��0H��H��H�L$(�T$ L�����L�Ep�UhH�M`�����H��0]��@UH��H�3Ɂ8  �����]���H�Q �8����   I���-�  L�M�H���p���I��L���  �   3���  ��S���L�CH���  I��3���  ��6����y(H�Q H��j L�M��I,L�]�  IE�H�D$ ���  �������H��H�M�����H�BL�"�  �   3�L�HI�����  �F   �����H�BL���  �   3�L�HI���a�  �F   ����A������H�U�H���0�  H�M��v�  ��Y���H�H�U�H�E�H�E'H�E�H�E�8   D�m�L�m�D�m��E�H��t5�zt/H�M���  ���u H�,�  H��  E3�H�	���  �����H���   H�E�E����  H�H�U�H�EH�AH�MH�M�H�E���  ���u7H���   L���  L�K3�I��H�HH��H�L$ H���  H�	�c�  �
H�M'���  �   H�M��(�  ��z���H�{�  L���  L�K3�I��H�	�!�  ��S���L���  �   3����  �F   �3����L�k�  �L�:�  H�@3�L�HI�����  �F   �;���L�;�  �L�
�  H�@�   3�L�HI�����  A�F   � ���I�H�8H��H  H���   H�����  H���Ӆ������I�H�8H��H  H���   H���]�  H����I�H��H�9H��H  H���   H���8�  H����I���H�9H��H  H���   H����  H����L��L���  ��H���3�  ��E����H�Q ����H�AL�H�  �   3�L�HI�����  A�F   �C���H�AL���  �   3�L�HI�����  A�F   �����H�Q �4���H�AL���  �   3�L�HI���O�  �C   ����H�AL���  �   3�L�HI���#�  �C   �Z����H�Q �r���H�AL���  �   3�L�HI�����  �F   �����H�AL�.�  �   3�L�HI�����  �F   �����H�Q �����H�AL� �  �   3�L�HI�����  �F   �7���H�AL���  �   3�L�HI���_�  �F   �����H�Q ����H�AL���  �   3�L�HI���)�  A�F   �����H�AL�i�  �   3�L�HI�����  A�F   �����eH�%X   I�VE��N�$�H���  �K���Lc�H�J��H���  �4�  H��tH�0�3�H���/  ��e 9F�   I�H�vH�8H��H  H��h  H�����  H���Ӌ<e ��Hc�K�<H�H�р��    tLI�H�8H��H  H���  H���F�  E3�E3�3�H���Ӄ�uI�>H�GH�XH��� �  ��H�����u��FI��J;�~;��@;��H�NHc�H���~ �Ft���  ���  HcH��H�FH��H�N�B�H�FH�H�F�vd ��Hc�K�<H�H��H�@ �\d ��Hc�K�<H�H��H�HA�F, �P����A�u��u�t���  ��x�  H�c �h���H�K�t�  �c �_����H�Q ����L�J3 �L���  �   3���  A�G   �=���H�BL�l�  �   3�L�HI�����  A�G   ����H�BL��  �   3�L�HI�����  A�G   �����H�BL���  �   3�L�HI���}�  A�G   ����H�?H��H  H��   H���n�  �   H����A�v(H��$�   H��$�   �i���H�X H���>�  H��$�   A�   L��H�t$(H��H�t$ �Ӆ�u��$�   ���A�F0A�G   ������b eH�%X   �   H��H���b ��Hc�H�H�����   ��   I�H�8H��H  H���   H�����  H���Ӆ�tI�H�8H��H  H���   H�����  H����I�H��H�9H��H  H���   H���\�  H����I���H�9H��H  H���   H���8�  H����L��L���  ��H���W�  A�G   �.���I�H�8H��H  H��  H�����  H����H��$�   ����  ��Q���H��u1I�H�8H��H  H�X0H�����  3��s������  �   �c���I�H�8H��H��H  H���   H�����  H����H��H  H��H���   H���e�  H����H��H  ��H���   H���G�  H����H��L���  L�͋��f�  A�G   �=���I�H�8H��H  H��  H����  H����H��$�   ����  ���������"���H�BH���?����8H��u��2���H��b �����H�^a ����H�Rc ����H�v` H���M�  ��H�d H��d HD�����L���  �L���  H�A�   3�L�HI���;�  �F   �w����L���  �L�v�  H�@�   3�L�HI����  A�F   ������_ eH�%X   �   H��H���_ ��Hc�H�H�����   ��   I�H�8H��H  H���   H�����  H���Ӆ�tI�H�8H��H  H���   H�����  H����I�H��H�9H��H  H���   H���m�  H����I���H�9H��H  H���   H���I�  H����L��L���  ��H���h�  A�F   �)����H�Q �Q���H�AL�v�  �   3�L�HI�����  A�F   �_���H�AL�!�  �   3�L�HI�����  A�F   �-���H�O H������  L�GH��H�VH�H�    �@   L�xH�x������������  A�\�  L�G, �   �D$ 3��F�  �����@������D��I��H���`��������M�&M��������r���H�S ����L���  �L�P�  H�A�   3�L�HI�����  A�G   �H���H�}� ����������eH�%X   H��I� A�A�Hc�H�H��H�AXH��H��H�E�HE�H��H���< u�H�E������eH�%X   H��I� A�A�Hc�H�H��H�AHH�E�����eH�%X   H��I� A�A�Hc�H�H��H�A@H�E������   I������D��\ ����H������H�E�eH�%X   �   L�<���\ L���Hc�I�H�H��H�x0 uL�+ �   3����  ��q���L��H��H��L��A�E�l$,H�E�H��LE�H�E�H��L�T$8HE�H�E�H��H�T$0H��* HE�H�E�H��H�L$(LE�H�E�H��HE�H�E�L��H�D$ 3����  I�D$H��H�7�  �I���Hc�H�H��H��H���  ���  H���]  H�H���Q  �;\ 9C�����H�[H�����  �������H�C��H�CH�I�$��[ ��Hc�I�H�H��H�H I�$H�8H��H  H���   H�����  H�E�H��L�M�L�E�H�U�H�D$(D�l$ �Ӆ���   I�$H�8H��H  H��`  H���_�  H���Ӌ[ ��Hc�I�H�H��H�@I�$H�8H��H��H  H���   H��� �  H����H��H��H  H���   H����  H���Ӌ�H�������������I�<$H�GH�XH�����  �   H����I�$    �f�����Z �    �E��J����  H��H�E ����  H�GH�EH�{�  �I���Hc�H�H�ѹ   ��   t��  ����  E�H�WL�E�M L�M� H���  H�E��H�E�   �y�  ������M�HL;������L��( �   3����  ������I�P(H��������M�H I�@L�L;������L�s( �����  H����H��H��H  H���   H�����  H���Ӌ�H���y���I�$H�8H��H��H  H���   H���|�  H����L��H��H  H���   H���]�  H���Ӌ�H��H  H���   H���?�  H����H��L���  M�΋��^�  �u`@��u'I�<$H�GH�XH����  �   H����I�$    I�L$H��t.�A�u��u�t���  ����  I�D$    A�D$, @��u@8uPt	I�����  H�MX�A   ������}� u	@��������EX ��Hc�I�H�H��H�@�e���H�NX �9���H�MXH��A  L�p������\�  ��W �}���H���p�  ��W ��Lc�H�7H�J��H���   �y����y(H�A IE�H� L9x�������=���I�����  �G   �����{(H�C IE�H� �x����H�JH�������H��@89u������H��W H���T�  ��tH�9X ����H�uW H���4�  ��tH�Z �����H�-W H����  ��tH��\ �����H�W H�����  ��H��Z H��[ HD�����̋GH�H�T$0�D$8��
s��|$8tH�L$0��  H�|$0������V eH�%X   A�   H��M� H���  �I� ��Hc�H��������J�  H�X H�����  H��$�   L��E��H�D$ H��I����H�������G	�����H�� ������H����  ����������H�	H�%n�  ������H��N �H��H�XH�hH�pH�x AVH��PH��I��I�H I��H���H�����  L��H��L��H�` H�H�    �@  H�x�����H��$�   H�  H��A�D> L�D$ L�t$0HE��D$(   H�L$8H��H�p  I��HE�H�d$  H�L$@H���ݿ  H��H��t/�E�   t��  ����  D$0H��L$@ �HA�F�uA��uI�Ψt�[�  ��K�  H�\$`H�l$hH�t$pH�|$xH��PA^�@SH�� �yH��uBH�	H��T H�I���  3҄�t(H�H�H�H��u$H�@L�@�  �QL�HI�����  3�H�� [�H�H��tH�H���������������������������H��H�XH�hH�pH�x ATAVAWH��PH�L��8   L��H�X����  �SE3�L�~�  ��H��H����  H�{�[H��H�H;���   �-�S eL�<%X   � txH�H�T$0H�H�L$0�A$��H�D$@�H�D$ �� ���E3Ɂ�  �L$8I��E�A�r���I��A�   M�	H���  �I�	��Lc�J;�tH�T$0L��H�����  H�� H;��u���L�\$PA�   I�[ H��I�k(I�s0I�{8I��A_A^A\�������H��H�XH�hH�pH�x AVH��@L��H��H�	I��H�A��3�H�I����  H��tH� �H��H���	  ����   ��t(��tL�B�  �   3��ݾ  �  �   ��   H�D$0L��A�   H�D$ H��I���`����rR H��eH�%X   �   H��H�H���  �H���Hc�H;<���   H�����  ����   H�D$0L��A�   H�D$ H��I��������R L��eH�%X   �   H��H�H�%�  �H���Hc�L;�t6A�xI�����<�  �#���  H�XXH�����  L��D��H��I���Ӌ�H�l$X��H�\$PH�t$`H�|$hH��@A^�����������H��(H���  3����  3�H��(�������@SH�� H��H�I�H��tH�	H��t�a  H��H�� [��������H�\$WH�� H�Y�H��H��tH�; tH�K致  H�# H��H�\$0H�� _�e��������H��(H�U�  3��-�  ���H��(������L��I�[I�kI�sWAVAWH��0�zM��I��H��L��t-�GH�I�S؉D$(��
s��|$(tH�L$ �޼  H�|$ I�6H�N�H��t2H���  H��tH��3�H��tH�[H�����  H��H�N����!��  H�X(H���p�  M��L��H��I����H�D$ H;�u�G	tH�� �u	H��Ѽ  H�\$PH�l$XH�t$`H��0A_A^_�H�\$H�t$WH�� H�1H��H��t2H�FH�X`H�����  �H���Ӆ�tL���  �   3��ƻ  �H�OHH��t��  H���ܹ  H�\$0H�t$8H�� _�������������@SH�� H�YH��t%E3�H�'   H���f�  H����  H�����  H�� [�������H�\$WH�� H�AH��H�XH���J�  �   H��H��H�\$0H�� _H����H�\$H�l$H�t$WH�� H�ZHI��H��H��H�˃���  A�   L�ǋ�H��H��H�\$0H�l$8H�t$@H�� _H�����H�\$H�l$ VWATAVAWH��0D�A,E3�H��L�|$pH��E�gE�wE+�t8E;�t��  �5  L�tN H�D$pL�D$`H�D$ H���  A�����  �L�LN L�D$`H�̾  A�����  �����   H�D$`H� H�H�H��u'L���  H�@A��3�L�HI�����  D�f�   �yH�9}	L�>�  ��H�?H��t�H�D$pH��t4I;�tL�F�  A��3����  �BH�?H��H  H�X0H�����  3��H�?H��H  H�X8H�����  A��H����H��H��uD�v�+�   I���;�  H�M H��@   �}(IE�H�	H�A�H�\$hH�l$xH��0A_A^A\_^����������������H�\$UWATAVAWH��0L��L���Q,����   ��t�b�  ��  L��L H�D$pH�D$(L�D$`H�D$x�   H�^�  H�D$ ��  �����  H�D$`3�H� H�H�H����   �yH�}L���  �   H9+t�P   �J��K�  H�L��H�9H��H  H�ZPH���n�  H����I�$H��H����   H�HH�H���I�  L�D$pH��H�T$x���   L�L L�D$`H���  �   �W�  �����   H�D$`3�H� H�H�H��u-L�\�  H�@�   3�L�HI���÷  A�F   �   �yH�����H9+�����P   �J��j�  L��H�H�8H��H  H�XPH�����  H����I�$I9,$uI�����  A�F   �5�   �J���  ��L� A�,�����HI�O A�(HE�H�	H�A�H�\$hH��0A_A^A\_]�������������H�\$H�l$H�t$WH�� ��J 3�eH�%X   H�ٽ   H�<�H94/u
��  H�/H�l$8H���H�CH�C(�   H�sH�sH�s H�s8H�s@H�sHH�sXH�sPH�s`H���   H���   H���   @���   H�t$@H�C0H�ChH�\$0H�� _��H�\$H�t$WAVAWH��PH��B H3�H�D$@H���u�  A�   L���  A��H���  ��  ��  H���  A��L�����  eH�%X   A�w��I L���  A�   ��H�ȋ�I ��Lc�J�3H�N��H�L$ M�I��  L�D$ A��H�u�  �W�  �aI L�V�  �ȋ�Lc�J�3H�N��H�L$ M�I ���  L�D$ A��H�J�  ��  �I L��  ��H�L$ LcЋ�J�3L�O��M�I�~�  L�D$ A��H�'�  �ѵ  �{�  H����  H�L$@H3������L�\$PI�[(I�s0I��A_A^_��������@SH�� H�KI �����  H�TJ �~�  H��H �q�  H��M �d�  H��H �W�  H� O �J�  ���ʲ  3�H�� [�����������H��H�XH�pH�xUATAUAVAWH������H��@  ����  ��H�i�  H���8�  �ʴ  L�%������   I��$� E3� HI@ A H0I0@@A@HPIP@`A`H�@pH�A�	@AH I @0A0H@I@@PH�θ��APH�L I��$0� ��K    H����H�L H�\���H��K L�5�K H�C@H�L H�����H�L H�����H�_L I��$�  H@ IH0A @@I0HPA@@`IPHpA`H�H�I� H@ IA H0@@I0HPH����A@IPI��$P� H�_G I��$�  H@ IH0A @@I0HPA@@`IPHpH�A`H� I�H@ IH0A @@I0HPH����A@IPI��$� H��G I��$�  H@ IH0A @@I0HPA@@`IPHpA`H�H�I� H@ IH0A @@I0HPH�.���A@IPH�'K I��$0� I��$�  H@ D�j�E3�IE��H0A @@I0HPA@@`IPHpH�A`H� I�H@ IH0A @@I0HPH�d���A@IPI��$� H��G I��$�  H@ IH0A @@I0HPA@@`IPHpH�A`H�3� I�H@ IH0A @@I0HPH����A@IPH��J H��G ��  D��L��  H�����3����  A�N0�kD �ݮ  A�  H��D�(D�xL�pH�@   �  @��  �H(H�έ  D�p,H�H����  H����H���  H��@  W�H��H  W�L��(  H�@�  f�0  L���  L���  L���  f�`  L��p  f�@  f�P  L��x  f��  L���  L���  L��P  L���  L��  f�  L��   L���  L���  L��(  H��   H��Ƭ  E3��D$    E�FH��B H��  H�ȃH��  H��B L�ֶ  E�F�D$    H�˶  ���  A�N(�k�  H��D�(D�xL�pH�@   ���  �@���  �H ���  �H$H�W�  D�p%H�H���w�  H����W�H�D$HfE0L�u(W�L���   H���  L���   H��   H�L$@H����fE`H���   L�upfM@fEPL�uxf��   L���   L���   L�t$PL���   L��  f�  L��   L���   L���   L��(  ���  L�=.B E��L�\���H��A I��3�H���_�  A��H���  A��J��!(/ N��! / N��!/ I��H�D$ �-���I�H��H��J��!/ H��učzA�   E��H�[�  D��H���G�  E��D�GH�Q�  H���0�  E��D�GH�J�  H����  E��D�G�H�C�  H����  E��D�G�H�<�  H����  E��H�9�  D��H���ժ  H�SH�*A �KL�D$0H�G H�D$0�D$8   ��  �O�6�  H��D�(�@  L�pH�@   ��  �H�ݴ  f�HH�*�  D�pH�H���J�  H����W�H�D$HW�fE0fE`fM@fEPf��   f�  L�u(L���   L���   L�upL�uxL���   L���   L�t$PL���   L��  L��   L���   L���   L��(  H���  H��   H�L$@H�}���H���   �ȫ  E��L�6���H�? H�h? 3�H���5�  I��H��  I��J��!�H N��!�H N��!xH H�A? H�D$ �����I�H��H��J��!pH H��u�D�BE��H���  H���!�  �   H��  D��E��H����  E��D�GH��  H����  E��D�GH�a�  H���ب  E��D�GH�Z�  H�����  E��D�G�H�S�  H�����  E��D�G�H�D�  H�����  E��D�G�H�5�  H���|�  E��H�2�  D��H���f�  E��D�G�H�(�  H���O�  E��D�G�H��  H���8�  E��D�G�H��  H���!�  E��H��  D��H����  E��D�GH��  H�����  E��D�G�H���  H���ݧ  E��D�G�H���  H���Ƨ  E��D�GH�ز  H�����  E��D�G�H�ٲ  H�����  E��D�GH�Ҳ  H�����  H�SH�^= L�D$0H�D$0H��C �D$8   ���  �O��  H��D�(�@  L�pH�@   ���  �@���  �H �|�  f�H$H�ͦ  D�p&H�H����  H����W�H�D$HW�L�u(H�z�  fE0H��   H�L$@H�����L���   H���   L���   fE`L�upfM@fEPL�uxf��   L���   L���   L�t$PL���   L��  f�  L��   L���   L���   L��(  �k�  E��L�����H�rA H��; 3�H�؃H�ԥ  I��I��H��  J��!�K N��!�K N��!�K H�0A H�D$ ����I�H��H��J��!�K H��u�D�BE��H�1�  H�����  E��H���  A�   H�����  E��H�=�  A�   H�����  H�SH��@ L�D$0H�D$0H��A �D$8   ���  �(   ��  H��D�(�@  L�pH�@   ���  �@���  �H ���  �H$H�ڤ  D�p%H�H�����  H����W�H�D$HW�L�u(H���  fE0H��   H�L$@H�����L���   H���   L���   fE`L�upfM@fEPL�uxf��   L���   L���   L�t$PL���   L��  f�  L��   L���   L���   L��(  �x�  E��L�����H��; H��9 3�H����  I��H���  I��J��!K N��! K N��!�J H��; H�D$ ����I�H��H��J��!�J H��u�D�BE��H�Z�  H���ѣ  E��H���  A�   H�����  E��H�>�  A�   H�����  E��H�-�  A�   H�����  E��H� �  A�   H���m�  H�9 H���  L�0�  A��H���  H��2 H��P  M� �7�  H�SH��: L�D$0H�D$0H�s? �D$8   �M�  �(   ���  H��D�(�@  L�pH�@   ���  �@���  f�H ���  �H"H���  D�p#H�H�����  H����W�H�D$HW�fE0fE`fM@fEPf��   f�  L�u(L���   L���   L�upL�uxL���   L���   L�t$PL���   L��  L��   H���  L���   H��   H�L$@H����L���   H���   L��(  ��  E��L�����H�8 H��7 3�H�����  I��H�B�  I��J��!(G N��! G N��!G H��7 H�D$ �V���I�H��H��J��!G H��u�D�BE��H��  H���x�  E��H�V�  A�	   H���_�  A�   H��  E��E��H���C�  A�^E��D��H��  H���)�  E��H�߫  D��H����  E�f�E��E��H���  H�����  E��H���  E��H����  E��E�FH���  H���̠  E��H���  E��H�����  E��E�F�H�t�  H�����  H�WH��6 L�D$0H�D$0H��< �D$8   ���  A�^��t$ E�nD��A��H�,�  E�����  E�f�t$ A��E�F�D��H�*�  �l�  D�ˉt$ E3��SH�0�  �R�  E�Ɖt$ D��H�9�  E�pA���4�  D�{�t$ A��H�2�  D��D����  �{�t$ ��H�.�  D��A��   ���  D�ˉt$ �_A��   ��H�&�  �ء  E�M�t$ A��   H�+�  �����  �_�t$ D�ˍWE�E�H�+�  ���  D�ˉt$ E�EA��H�8�  ���  A�   �t$ E�čW�D��H�8�  �b�  D�ˉt$ �W�E�EH�5�  �G�  D�ˉt$ �   H�4�  D��A���(�  �W��t$ E�M�A�   H�0�  �
�  E�M�t$ E�E�A��H�4�  ��  �t$ E�u�E�΍W�E�E�H�-�  �Ϡ  E�Ήt$ �WA�   @H�0�  ���  E�Ήt$ �WE�E'H�=�  ���  E�Ήt$ �WA�  @ H�P�  �z�  E�o
�t$ E�ōW	E��H�b�  �\�  E�Ήt$ E3�H�s�  A���B�  E�Ήt$ �W�E�G�H�m�  �'�  E�Ήt$ E�w�A��E�G�H�f�  ��  E�O��t$ E�G�A��H�Z�  ��  E�o��t$ E��E�G�A��H�[�  �͟  E�͉t$ �W�E�G�H�P�  ���  E�͉t$ E�ōW�H�F�  ���  E�͉t$ E3��WH�<�  �~�  �t$ E��E�G�E�o	A��H�E�  �_�  E�O��t$ E3�H�M�  A���D�  E�O��t$ E�G�A��H�N�  �(�  �W��t$ E�O�E�G�H�R�  ��  �W��t$ E�O�E�G�H�V�  ��  E�O��t$ E�G��H�[�  �՞  A��t$ ��E�O�E�G�H�\�  ���  E�O��t$ E�G�A��H�X�  ���  E�~��t$ E�ύWE�F�H�Q�  �{�  E�ωt$ �WE�F�H�N�  �`�  �G��t$ D��H�W�  ��E���D�  E�ωt$ E�F��H�P�  �*�  E�ωt$ E�F4��H�N�  ��  E�ωt$ �WA�   H�I�  ��  E�ωt$ D�ÍWH�O�  �ٝ  A�~�t$ ��H�P�  E��E�����  � �  �t$ D��H�A�  E�ϋ����  E�ωt$ �WA� @  H�/�  ���  E�ωt$ D�ÍWH�-�  �g�  E�ωt$ �WA�   H�(�  �J�  E�f�t$ A��E�FtE��H�!�  �+�  E�ωt$ A�   H��  E��A����  �W
�t$ E�N�A�    H��  ��  �_�t$ ��E�N�E3�H��  �М  �W�t$ E�N�E�F�H��  ���  �W�t$ E�N�E�F�H��  ���  �G�t$ D�ȍWD��H��  �{�  �W�t$ E�N�E�F�H��  �_�  E�N��t$ E�F�A��H�	�  �C�  �W�t$ E�N�E�F�H��  �'�  �W�t$ E�N�E�F�H��  ��  �W�t$ E�N�E�F�H���  ��  �W�t$ E�N�E�F�H���  �ӛ