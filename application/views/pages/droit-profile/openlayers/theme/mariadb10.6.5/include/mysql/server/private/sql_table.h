/* Copyright (c) 2012, 2021, Oracle and/or its affiliates.

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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#ifndef MYSQL_PLUGIN_VALIDATE_PASSWORD_INCLUDED
#define MYSQL_PLUGIN_VALIDATE_PASSWORD_INCLUDED

/* API for validate_password plugin. (MYSQL_VALIDATE_PASSWORD_PLUGIN) */

#include <mysql/plugin.h>
#define MYSQL_VALIDATE_PASSWORD_INTERFACE_VERSION 0x0100

/*  
  The descriptor structure for the plugin, that is referred from
  st_mysql_plugin.
*/

typedef void* mysql_string_handle;

struct st_mysql_validate_password
{
  int interface_version;
  /*  
    This function retuns TRUE for passwords which satisfy the password
    policy (as choosen by plugin variable) and FALSE for all other
    password
  */
  int (*validate_password)(mysql_string_handle password);
  /*  
    This function returns the password strength (0-100) depending
    upon the policies
  */
  int (*get_password_strength)(mysql_string_handle password);
};
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              r
    example in

       CREATE TABLE t1 (a int(5) NOT NUL) SELECT b+10 as a FROM t2;

    the list in alter_info->create_list will have two fields `a`.

  - ALTER TABLE, that creates a temporary table #sql-xxx, which will be later
    renamed to replace the original table.

  - ALTER TABLE as above, but which only modifies the frm file, it only
    creates an frm file for the #sql-xxx, the table in the engine is not
    created.

  - Assisted discovery, CREATE TABLE statement without the table structure.

  These situations are distinguished by the following "create table mode"
  values, where a CREATE ... SELECT is denoted by any non-negative number
  (which should be the number of fields in the SELECT ... part), and other
  cases use constants as defined below.
*/
#define C_CREATE_SELECT(X)        ((X) > 0 ? (X) : 0)
#define C_ORDINARY_CREATE         0
#define C_ALTER_TABLE            -1
#define C_ALTER_TABLE_FRM_ONLY   -2
#define C_ASSISTED_DISCOVERY     -3

int mysql_create_table_no_lock(THD *thd,
                               DDL_LOG_STATE *ddl_log_state,
                               DDL_LOG_STATE *ddl_log_state_rm,
                               const LEX_CSTRING *db,
                               const LEX_CSTRING *table_name,
                               Table_specification_st *create_info,
                               Alter_info *alter_info, bool *is_trans,
                               int create_table_mode, TABLE_LIST *table);

handler *mysql_create_frm_image(THD *thd,
                                const LEX_CSTRING &db,
                                const LEX_CSTRING &table_name,
                                HA_CREATE_INFO *create_info,
                                Alter_info *alter_info,
                                int create_table_mode,
                                KEY **key_info,
                                uint *key_count,
                                LEX_CUSTRING *frm);

int mysql_discard_or_import_tablespace(THD *thd,
                                       TABLE_LIST *table_list,
                                       bool discard);

bool mysql_prepare_alter_table(THD *thd, TABLE *table,
                               HA_CREATE_INFO *create_info,
                               Alter_info *alter_info,
                               Alter_table_ctx *alter_ctx);
bool mysql_trans_prepare_alter_copy_data(THD *thd);
bool mysql_trans_commit_alter_copy_data(THD *thd);
bool mysql_alter_table(THD *thd, const LEX_CSTRING *new_db,
                       const LEX_CSTRING *new_name,
                       HA_CREATE_INFO *create_info,
                       TABLE_LIST *table_list,
                       Alter_info *alter_info,
                       uint order_num, ORDER *order, bool ignore,
                       bool if_exists);
bool mysql_compare_tables(TABLE *table,
                          Alter_info *alter_info,
                          HA_CREATE_INFO *create_info,
                          bool *metadata_equal);
bool mysql_recreate_table(THD *thd, TABLE_LIST *table_list, bool table_copy);
bool mysql_create_like_table(THD *thd, TABLE_LIST *table,
                             TABLE_LIST *src_table,
                             Table_specification_st *create_info);
bool mysql_rename_table(handlerton *base, const LEX_CSTRING *old_db,
                        const LEX_CSTRING *old_name, const LEX_CSTRING *new_db,
                        const LEX_CSTRING *new_name, LEX_CUSTRING *id,
                        uint flags);
bool mysql_backup_table(THD* thd, TABLE_LIST* table_list);
bool mysql_restore_table(THD* thd, TABLE_LIST* table_list);

template<typename T> class List;
void fill_checksum_table_metadata_fields(THD *thd, List<Item> *fields);
bool mysql_checksum_table(THD* thd, TABLE_LIST* table_list,
                          HA_CHECK_OPT* check_opt);
bool mysql_rm_table(THD *thd,TABLE_LIST *tables, bool if_exists,
                    bool drop_temporary, bool drop_sequence,
                    bool dont_log_query);
int mysql_rm_table_no_locks(THD *thd, TABLE_LIST *tables,
                            const LEX_CSTRING *db,
                            DDL_LOG_STATE *ddl_log_state,
                            bool if_exists,
                            bool drop_temporary, bool drop_view,
                            bool drop_sequence,
                            bool dont_log_query, bool dont_free_locks);
bool log_drop_table(THD *thd, const LEX_CSTRING *db_name,
                    const LEX_CSTRING *table_name, const LEX_CSTRING *handler,
                    bool partitioned, const LEX_CUSTRING *id,
                    bool temporary_table);
bool quick_rm_table(THD *thd, handlerton *base, const LEX_CSTRING *db,
                    const LEX_CSTRING *table_name, uint flags,
                    const char *table_path=0);
void close_cached_table(THD *thd, TABLE *table);
void sp_prepare_create_field(THD *thd, Column_definition *sql_field);
bool mysql_write_frm(ALTER_PARTITION_PARAM_TYPE *lpt, uint flags);
int write_bin_log(THD *thd, bool clear_error,
                  char const *query, ulong query_length,
                  bool is_trans= FALSE);
int write_bin_log_with_if_exists(THD *thd, bool clear_error,
                                 bool is_trans, bool add_if_exists);

void promote_first_timestamp_column(List<Create_field> *column_definitions);

/*
  These prototypes where under INNODB_COMPATIBILITY_HOOKS.
*/
uint explain_filename(THD* thd, const char *from, char *to, uint to_length,
                      enum_explain_filename_mode explain_mode);


extern MYSQL_PLUGIN_IMPORT const LEX_CSTRING primary_key_name;

bool check_engine(THD *, const char *, const char *, HA_CREATE_INFO *);

#endif /* SQL_TABLE_INCLUDED */
