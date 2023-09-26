 PLUGIN_VAR_THDLOCAL | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, min, max, blk, NULL }

#define MYSQL_THDVAR_ULONGLONG(name, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYSQL_THDVAR_SIMPLE(name, unsigned long long) = { \
  PLUGIN_VAR_LONGLONG | PLUGIN_VAR_THDLOCAL | PLUGIN_VAR_UNSIGNED | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, min, max, blk, NULL }

#define MYSQL_THDVAR_ENUM(name, opt, comment, check, update, def, typelib) \
DECLARE_MYSQL_THDVAR_TYPELIB(name, unsigned long) = { \
  PLUGIN_VAR_ENUM | PLUGIN_VAR_THDLOCAL | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, NULL, typelib }

#define MYSQL_THDVAR_SET(name, opt, comment, check, update, def, typelib) \
DECLARE_MYSQL_THDVAR_TYPELIB(name, unsigned long long) = { \
  PLUGIN_VAR_SET | PLUGIN_VAR_THDLOCAL | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, NULL, typelib }

#define MYSQL_THDVAR_DOUBLE(name, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYSQL_THDVAR_SIMPLE(name, double) = { \
  PLUGIN_VAR_DOUBLE | PLUGIN_VAR_THDLOCAL | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, min, max, blk, NULL }

/* accessor macros */

#define SYSVAR(name) \
  (*(MYSQL_SYSVAR_NAME(name).value))

/* when thd == null, result points to global value */
#define THDVAR(thd, name) \
  (*(MYSQL_SYSVAR_NAME(name).resolve(thd, MYSQL_SYSVAR_NAME(name).offset)))


/*
  Plugin description structure.
*/

struct st_mysql_plugin
{
  int type;             /* the plugin type (a MYSQL_XXX_PLUGIN value)   */
  void *info;           /* pointer to type-specific plugin descriptor   */
  const char *name;     /* plugin name                                  */
  const char *author;   /* plugin author (for I_S.PLUGINS)              */
  const char *descr;    /* general descriptive text (for I_S.PLUGINS)   */
  int license;          /* the plugin license (PLUGIN_LICENSE_XXX)      */
  int (*init)(MYSQL_PLUGIN);  /* the function to invoke when plugin is loaded */
  int (*deinit)(MYSQL_PLUGIN);/* the function to invoke when plugin is unloaded */
  unsigned int version; /* plugin version (for I_S.PLUGINS)             */
  struct st_mysql_show_var *status_vars;
  struct st_mysql_sys_var **system_vars;
  void * __reserved1;   /* reserved for dependency checking             */
  unsigned long flags;  /* flags for plugin */
};

/*************************************************************************
  API for Full-text parser plugin. (MYSQL_FTPARSER_PLUGIN)
*/
#define MYSQL_FTPARSER_INTERFACE_VERSION 0x0101

/*************************************************************************
  API for Query Rewrite plugin. (MYSQL_QUERY_REWRITE_PLUGIN)
*/

#define MYSQL_REWRITE_PRE_PARSE_INTERFACE_VERSION 0x0010
#define MYSQL_REWRITE_POST_PARSE_INTERFACE_VERSION 0x0010

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
 #define MYSQL_REPLICATION_INTERFACE_VERSION 0x0400

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
const char *set_thd_proc_info(MYSQL_THD thd, const char *info,
                              const char *calling_func,
                              const char *calling_file,
                              const unsigned int calling_line);
void **thd_ha_data(const MYSQL_THD thd, const struct handlerton *hton);
void thd_storage_lock_wait(MYSQL_THD thd, long long value);
int thd_tx_isolation(const MYSQL_THD thd);
int thd_tx_is_read_only(const MYSQL_THD thd);
MYSQL_THD thd_tx_arbitrate(MYSQL_THD requestor, MYSQL_THD holder);
int thd_tx_priority(const MYSQL_THD thd);
int thd_tx_is_dd_trx(const MYSQL_THD thd);
char *thd_security_context(MYSQL_THD thd, char *buffer, size_t length,
                           size_t max_query_len);
/* Increments the row counter, see THD::row_count */
void thd_inc_row_count(MYSQL_THD thd);
int thd_allow_batch(MYSQL_THD thd);


/**
  Mark transaction to rollback and mark error as fatal to a
  sub-statement if in sub statement mode.

  @param thd  user thread connection handle
  @param all  if all != 0, rollback the main transaction
*/

void thd_mark_transaction_to_rollback(MYSQL_THD thd, int all);

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
  Check the killed state of a connection

  @details
  In MySQL support for the KILL statement is cooperative. The KILL
  statement only sets a "killed" flag. This function returns the value
  of that flag.  A thread should check it often, especially inside
  time-consuming loops, and gracefully abort the operation if it is
  non-zero.

  @param thd  user thread connection handle
  @retval 0  the connection is active
  @retval 1  the connection has been killed
*/
int thd_killed(const MYSQL_THD thd);

/**
  Set the killed status of the current statement.

  @param thd  user thread connection handle
*/
void thd_set_kill_status(const MYSQL_THD thd);

/**
  Get binary log position for latest written entry.

  @note The file variable will be set to a buffer holding the name of
  the file name currently, but this can change if a rotation
  occur. Copy the string if you want to retain it.

  @param thd Use thread connection handle
  @param file_var Pointer to variable that will hold the file name.
  @param pos_var Pointer to variable that will hold the file position.
 */
void thd_binlog_pos(const MYSQL_THD thd,
                    const char **file_var,
          