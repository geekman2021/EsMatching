/* Copyright (c) 2005, 2013, Oracle and/or its affiliates
   Copyright (C) 2009, 2017, MariaDB

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

#ifndef MYSQL_PLUGIN_INCLUDED
#define MYSQL_PLUGIN_INCLUDED

/*
  On Windows, exports from DLL need to be declared
  Also, plugin needs to be declared as extern "C" because MSVC 
  unlike other compilers, uses C++ mangling for variables not only
  for functions.
*/
#ifdef MYSQL_DYNAMIC_PLUGIN
  #ifdef _MSC_VER
    #define MYSQL_DLLEXPORT _declspec(dllexport)
  #else
    #define MYSQL_DLLEXPORT
  #endif
#else
  #define MYSQL_DLLEXPORT
#endif

#ifdef __cplusplus
  #define MYSQL_PLUGIN_EXPORT extern "C" MYSQL_DLLEXPORT
#else
  #define MYSQL_PLUGIN_EXPORT MYSQL_DLLEXPORT
#endif

#ifdef __cplusplus
class THD;
class Item;
#define MYSQL_THD THD*
#else
#define MYSQL_THD void*
#endif

typedef char my_bool;
typedef void * MYSQL_PLUGIN;

#include <mysql/services.h>

#define MYSQL_XIDDATASIZE 128
/**
  struct st_mysql_xid is binary compatible with the XID structure as
  in the X/Open CAE Specification, Distributed Transaction Processing:
  The XA Specification, X/Open Company Ltd., 1991.
  http://www.opengroup.org/bookstore/catalog/c193.htm

  @see XID in sql/handler.h
*/
struct st_mysql_xid {
  long formatID;
  long gtrid_length;
  long bqual_length;
  char data[MYSQL_XIDDATASIZE];  /* Not \0-terminated */
};
typedef struct st_mysql_xid MYSQL_XID;

/*************************************************************************
  Plugin API. Common for all plugin types.
*/

/* MySQL plugin interface version */
#define MYSQL_PLUGIN_INTERFACE_VERSION 0x0104

/* MariaDB plugin interface version */
#define MARIA_PLUGIN_INTERFACE_VERSION 0x010e

/*
  The allowable types of plugins
*/
#define MYSQL_UDF_PLUGIN             0  /* not implemented              */
#define MYSQL_STORAGE_ENGINE_PLUGIN  1
#define MYSQL_FTPARSER_PLUGIN        2  /* Full-text parser plugin      */
#define MYSQL_DAEMON_PLUGIN          3
#define MYSQL_INFORMATION_SCHEMA_PLUGIN  4
#define MYSQL_AUDIT_PLUGIN           5
#define MYSQL_REPLICATION_PLUGIN     6
#define MYSQL_AUTHENTICATION_PLUGIN  7
#define MYSQL_MAX_PLUGIN_TYPE_NUM    12  /* The number of plugin types   */

/* MariaDB plugin types */
#define MariaDB_PASSWORD_VALIDATION_PLUGIN  8
#define MariaDB_ENCRYPTION_PLUGIN 9
#define MariaDB_DATA_TYPE_PLUGIN  10
#define MariaDB_FUNCTION_PLUGIN 11

/* We use the following strings to define licenses for plugins */
#define PLUGIN_LICENSE_PROPRIETARY 0
#define PLUGIN_LICENSE_GPL 1
#define PLUGIN_LICENSE_BSD 2

#define PLUGIN_LICENSE_PROPRIETARY_STRING "PROPRIETARY"
#define PLUGIN_LICENSE_GPL_STRING "GPL"
#define PLUGIN_LICENSE_BSD_STRING "BSD"

/* definitions of code maturity for plugins */
#define MariaDB_PLUGIN_MATURITY_UNKNOWN 0
#define MariaDB_PLUGIN_MATURITY_EXPERIMENTAL 1
#define MariaDB_PLUGIN_MATURITY_ALPHA 2
#define MariaDB_PLUGIN_MATURITY_BETA 3
#define MariaDB_PLUGIN_MATURITY_GAMMA 4
#define MariaDB_PLUGIN_MATURITY_STABLE 5

/*
  Macros for beginning and ending plugin declarations.  Between
  mysql_declare_plugin and mysql_declare_plugin_end there should
  be a st_mysql_plugin struct for each plugin to be declared.
*/


#ifndef MYSQL_DYNAMIC_PLUGIN
#define __MYSQL_DECLARE_PLUGIN(NAME, VERSION, PSIZE, DECLS)                   \
int VERSION= MYSQL_PLUGIN_INTERFACE_VERSION;                                  \
int PSIZE= sizeof(struct st_mysql_plugin);                                    \
struct st_mysql_plugin DECLS[]= {

#                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                S)             */
  struct st_mysql_show_var *status_vars;
  struct st_mysql_sys_var **system_vars;
  void * __reserved1;   /* reserved for dependency checking             */
  unsigned long flags;  /* flags for plugin */
};

/*
  MariaDB extension for plugins declaration structure.

  It also copy current MySQL plugin fields to have more independency
  in plugins extension
*/

struct st_maria_plugin
{
  int type;             /* the plugin type (a MYSQL_XXX_PLUGIN value)   */
  void *info;           /* pointer to type-specific plugin descriptor   */
  const char *name;     /* plugin name                                  */
  const char *author;   /* plugin author (for SHOW PLUGINS)             */
  const char *descr;    /* general descriptive text (for SHOW PLUGINS ) */
  int license;          /* the plugin license (PLUGIN_LICENSE_XXX)      */
  int (*init)(void *);  /* the function to invoke when plugin is loaded */
  int (*deinit)(void *);/* the function to invoke when plugin is unloaded */
  unsigned int version; /* plugin version (for SHOW PLUGINS)            */
  struct st_mysql_show_var *status_vars;
  struct st_mysql_sys_var **system_vars;
  const char *version_info;  /* plugin version string */
  unsigned int maturity; /* MariaDB_PLUGIN_MATURITY_XXX */
};

/*************************************************************************
  API for Full-text parser plugin. (MYSQL_FTPARSER_PLUGIN)
*/
#include "plugin_ftparser.h"

/*************************************************************************
  API for Storage Engine plugin. (MYSQL_DAEMON_PLUGIN)
*/

/* handlertons of different MySQL releases are incompatible */
#define MYSQL_DAEMON_INTERFACE_VERSION (MYSQL_VERSION_ID << 8)

/*
  Here we define only the descriptor structure, that is referred from
  st_mysql_plugin.
*/

struct st_mysql_daemon
{
  int interface_version;
};


/*************************************************************************
  API for I_S plugin. (MYSQL_INFORMATION_SCHEMA_PLUGIN)
*/

/* handlertons of different MySQL releases are incompatible */
#define MYSQL_INFORMATION_SCHEMA_INTERFACE_VERSION (MYSQL_VERSION_ID << 8)

/*
  Here we define only the descriptor structure, that is referred from
  st_mysql_plugin.
*/

struct st_mysql_information_schema
{
  int interface_version;
};


/*************************************************************************
  API for Storage Engine plugin. (MYSQL_STORAGE_ENGINE_PLUGIN)
*/

/* handlertons of different MySQL releases are incompatible */
#define MYSQL_HANDLERTON_INTERFACE_VERSION (MYSQL_VERSION_ID << 8)

/*
  The real API is in the sql/handler.h
  Here we define only the descriptor structure, that is referred from
  st_mysql_plugin.
*/

struct st_mysql_storage_engine
{
  int interface_version;
};

struct handlerton;


/*
  API for Replication plugin. (MYSQL_REPLICATION_PLUGIN)
*/
 #define MYSQL_REPLICATION_INTERFACE_VERSION 0x0200
 
 /**
    Replication plugin descriptor
 */
 struct Mysql_replication {
   int interface_version;
 };

/*************************************************************************
  st_mysql_value struct for reading values from mysqld.
  Used by server variables framework to parse user-provided values.
  Will be used for arguments when implementing UDFs.

  Note that val_str() returns a string in temporary memory
  that will be freed at the end of statement. Copy the string
  if you need it to persist.
*/

#define MYSQL_VALUE_TYPE_STRING 0
#define MYSQL_VALUE_TYPE_REAL   1
#define MYSQL_VALUE_TYPE_INT    2

struct st_mysql_value
{
  int (*value_type)(struct st_mysql_value *);
  const char *(*val_str)(struct st_mysql_value *, char *buffer, int *length);
  int (*val_real)(struct st_mysql_value *, double *realbuf);
  int (*val_int)(struct st_mysql_value *, long long *intbuf);
  int (*is_unsigned)(struct st_mysql_value *);
};


/*************************************************************************
  Miscellaneous functions for plugin implementors
*/

#ifdef __cplusplus
extern "C" {
#endif

int thd_in_lock_tables(const MYSQL_THD thd);
int thd_tablespace_op(const MYSQL_THD thd);
long long thd_test_options(const MYSQL_THD thd, long long test_options);
int thd_sql_command(const MYSQL_THD thd);
struct DDL_options_st;
struct DDL_options_st *thd_ddl_options(const MYSQL_THD thd);
void thd_storage_lock_wait(MYSQL_THD thd, long long value);
int thd_tx_isolation(const MYSQL_THD thd);
int thd_tx_is_read_only(const MYSQL_THD thd);
/**
  Create a temporary file.

  @details
  The temporary file is created in a location specified by the mysql
  server configuration (--tmpdir option).  The caller does not need to
  delete the file, it will be deleted automatically.

  @param prefix  prefix for temporary file name
  @retval -1    error
  @retval >= 0  a file handle that can be passed to dup or my_close
*/
int mysql_tmpfile(const char *prefix);

/**
  Return the thread id of a user thread

  @param thd  user thread connection handle
  @return  thread id
*/
unsigned long thd_get_thread_id(const MYSQL_THD thd);

/**
  Get the XID for this connection's transaction

  @param thd  user thread connection handle
  @param xid  location where identifier is stored
*/
void thd_get_xid(const MYSQL_THD thd, MYSQL_XID *xid);

/**
  Invalidate the query cache for a given table.

  @param thd         user thread connection handle
  @param key         databasename\\0tablename\\0
  @param key_length  length of key in bytes, including the NUL bytes
  @param using_trx   flag: TRUE if using transactions, FALSE otherwise
*/
void mysql_query_cache_invalidate4(MYSQL_THD thd,
                                   const char *key, unsigned int key_length,
                                   int using_trx);


/**
  Provide a handler data getter to simplify coding
*/
void *thd_get_ha_data(const MYSQL_THD thd, const struct handlerton *hton);


/**
  Provide a handler data setter to simplify coding

  @details
  Set ha_data pointer (storage engine per-connection information).

  To avoid unclean deactivation (uninstall) of storage engine plugin
  in the middle of transaction, additional storage engine plugin
  lock is acquired.

  If ha_data is not null and storage engine plugin was not locked
  by thd_set_ha_data() in this connection before, storage engine
  plugin gets locked.

  If ha_data is null and storage engine plugin was locked by
  thd_set_ha_data() in this connection before, storage engine
  plugin lock gets released.

  If handlerton::close_connection() didn't reset ha_data, server does
  it immediately after calling handlerton::close_connection().
*/
void thd_set_ha_data(MYSQL_THD thd, const struct handlerton *hton,
                     const void *ha_data);


/**
  Signal that the first part of handler commit is finished, and that the
  committed transaction is now visible and has fixed commit ordering with
  respect to other transactions. The commit need _not_ be durable yet, and
  typically will not be when this call makes sense.

  This call is optional, if the storage engine does not call it the upper
  layer will after the handler commit() method is done. However, the storage
  engine may choose to call it itself to increase the possibility for group
  commit.

  In-order parallel replication uses this to apply different transaction in
  parallel, but delay the commits of later transactions until earlier
  transactions have committed first, thus achieving increased performance on
  multi-core systems while still preserving full transaction consistency.

  The storage engine can call this from within the commit() method, typically
  after the commit record has been written to the transaction log, but before
  the log has been fsync()'ed. This will allow the next replicated transaction
  to proceed to commit before the first one has done fsync() or similar. Thus,
  it becomes possible for multiple sequential replicated transactions to share
  a single fsync() inside the engine in group commit.

  Note that this method should _not_ be called from within the commit_ordered()
  method, or any other place in the storage engine. When commit_ordered() is
  used (typically when binlog is enabled), the transaction coordinator takes
  care of this and makes group commit in the storage engine possible without
  any other action needed on the part of the storage engine. This function
  thd_wakeup_subsequent_commits() is only needed when no transaction
  coordinator is used, meaning a single storage engine and no binary log.
*/
void thd_wakeup_subsequent_commits(MYSQL_THD thd, int wakeup_error);

#ifdef __cplusplus
}
#endif

#endif

