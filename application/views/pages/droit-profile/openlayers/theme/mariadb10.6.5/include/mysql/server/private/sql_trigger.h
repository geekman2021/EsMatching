ns.  Between
  mysql_declare_plugin and mysql_declare_plugin_end there should
  be a st_mysql_plugin struct for each plugin to be declared.
*/


#ifndef MYSQL_DYNAMIC_PLUGIN
#define __MYSQL_DECLARE_PLUGIN(NAME, VERSION, PSIZE, DECLS)                   \
MYSQL_PLUGIN_EXPORT int VERSION= MYSQL_PLUGIN_INTERFACE_VERSION;                                  \
MYSQL_PLUGIN_EXPORT int PSIZE= sizeof(struct st_mysql_plugin);                                    \
MYSQL_PLUGIN_EXPORT struct st_mysql_plugin DECLS[]= {
#else
#define __MYSQL_DECLARE_PLUGIN(NAME, VERSION, PSIZE, DECLS)                   \
MYSQL_PLUGIN_EXPORT int _mysql_plugin_interface_version_= MYSQL_PLUGIN_INTERFACE_VERSION;         \
MYSQL_PLUGIN_EXPORT int _mysql_sizeof_struct_st_plugin_= sizeof(struct st_mysql_plugin);          \
MYSQL_PLUGIN_EXPORT struct st_mysql_plugin _mysql_plugin_declarations_[]= {
#endif

#define mysql_declare_plugin(NAME) \
__MYSQL_DECLARE_PLUGIN(NAME, \
                 builtin_ ## NAME ## _plugin_interface_version, \
                 builtin_ ## NAME ## _sizeof_struct_st_plugin, \
                 builtin_ ## NAME ## _plugin)

#define mysql_declare_plugin_end ,{0,0,0,0,0,0,0,0,0,0,0,0,0}}

/**
  Declarations for SHOW STATUS support in plugins
*/
enum enum_mysql_show_type
{
  SHOW_UNDEF, SHOW_BOOL,
  SHOW_INT,        ///< shown as _unsigned_ int
  SHOW_LONG,       ///< shown as _unsigned_ long
  SHOW_LONGLONG,   ///< shown as _unsigned_ longlong
  SHOW_CHAR, SHOW_CHAR_PTR,
  SHOW_ARRAY, SHOW_FUNC, SHOW_DOUBLE
#ifdef MYSQL_SERVER
  /*
    This include defines server-only values of the enum.
    Using them in plugins is not supported.
  */
  #include "sql_plugin_enum.h"
#endif
};

/**
  Status variable scope.
  Only GLOBAL status variable scope is available in plugins.
*/
enum enum_mysql_show_scope
{
  SHOW_SCOPE_UNDEF,
  SHOW_SCOPE_GLOBAL
#ifdef MYSQL_SERVER
  /* Server-only values. Not supported in plugins. */
  ,
  SHOW_SCOPE_SESSION,
  SHOW_SCOPE_ALL
#endif
};

/**
  SHOW STATUS Server status variable
*/
struct st_mysql_show_var
{
  const char *name;
  char *value;
  enum enum_mysql_show_type type;
  enum enum_mysql_show_scope scope;
};

#define SHOW_VAR_MAX_NAME_LEN 64
#define SHOW_VAR_FUNC_BUFF_SIZE 1024
typedef int (*mysql_show_var_func)(MYSQL_THD, struct st_mysql_show_var*, char *);


/*
  Constants for plugin flags.
 */

#define PLUGIN_OPT_NO_INSTALL   1UL   /* Not dynamically loadable */
#define PLUGIN_OPT_NO_UNINSTALL 2UL   /* Not dynamically unloadable */


/*
  declarations for server variables and command line options
*/


#define PLUGIN_VAR_BOOL         0x0001
#define PLUGIN_VAR_INT          0x0002
#define PLUGIN_VAR_LONG         0x0003
#define PLUGIN_VAR_LONGLONG     0x0004
#define PLUGIN_VAR_STR          0x0005
#define PLUGIN_VAR_ENUM         0x0006
#define PLUGIN_VAR_SET          0x0007
#define PLUGIN_VAR_DOUBLE       0x0008
#define PLUGIN_VAR_UNSIGNED     0x0080
#define PLUGIN_VAR_THDLOCAL     0x0100 /* Variable is per-connection */
#define PLUGIN_VAR_READONLY     0x0200 /* Server variable is read only */
#define PLUGIN_VAR_NOSYSVAR     0x0400 /* Not a server variable */
#define PLUGIN_VAR_NOCMDOPT     0x0800 /* Not a command line option */
#define PLUGIN_VAR_NOCMDARG     0x1000 /* No argument for cmd line */
#define PLUGIN_VAR_RQCMDARG     0x0000 /* Argument required for cmd line */
#define PLUGIN_VAR_OPCMDARG     0x2000 /* Argument optional for cmd line */
#define PLUGIN_VAR_NODEFAULT    0x4000 /* SET DEFAULT is prohibited */
#define PLUGIN_VAR_MEMALLOC     0x8000 /* String needs memory allocated */
#define PLUGIN_VAR_INVISIBLE    0x10000 /* Variable should not be shown */

struct st_mysql_sys_var;
struct st_mysql_value;

/*
  SYNOPSIS
    (*mysql_var_check_func)()
      thd               thread handle
      var               dynamic variable being altered
      save              pointer to temporary storage
      value             user provided value
  RETURN
    0   user provided value is OK and the update func may be called.
    any other value indicates error.
  
  This function should parse the user provided value and store in the
  provided temporary storage any data as required by the update func.
  There is sufficient space in the temporary storage to store a double.
  Note that the update func may not be called if any other error occurs
  so any memory allocated should be thread-local so that it may be freed
  automatically at the end of the statement.
*/

typedef int (*mysql_var_check_func)(MYSQL_THD thd,
                                    struct st_mysql_sys_var *var,
                                    void *save, struct st_mysql_value *value);

/*
  SYNOPSIS
    (*mysql_var_update_func)()
      thd               thread handle
      var               dynamic variable being altered
      var_ptr           pointer to dynamic variable
      save              pointer to temporary storage
   RETURN
     NONE
   
   This function should use the validated value stored in the temporary store
   and persist it in the provided pointer to the dynamic variable.
   For example, strings may require memory to be allocated.
*/
typedef void (*mysql_var_update_func)(MYSQL_THD thd,
                                      struct st_mysql_sys_var *var,
                                      void *var_ptr, const void *save);


/* the following declarations are for internal use only */


#define PLUGIN_VAR_MASK \
        (PLUGIN_VAR_READONLY | PLUGIN_VAR_NOSYSVAR | \
         PLUGIN_VAR_NOCMDOPT | PLUGIN_VAR_NOCMDARG | \
         PLUGIN_VAR_OPCMDARG | PLUGIN_VAR_RQCMDARG | PLUGIN_VAR_MEMALLOC | \
         PLUGIN_VAR_NODEFAULT | PLUGIN_VAR_INVISIBLE)

#define MYSQL_PLUGIN_VAR_HEADER \
  int flags;                    \
  const char *name;             \
  const char *comment;          \
  mysql_var_check_func check;   \
  mysql_var_update_func update

#define MYSQL_SYSVAR_NAME(name) mysql_sysvar_ ## name
#define MYSQL_SYSVAR(name) \
  ((struct st_mysql_sys_var *)&(MYSQL_SYSVAR_NAME(name)))

/*
  for global variables, the value pointer is the first
  element after the header, the default value is the second.
  for thread variables, the value offset is the first
  element after the header, the default value is the second.
*/
   

#define DECLARE_MYSQL_SYSVAR_BASIC(name, type) struct { \
  MYSQL_PLUGIN_VAR_HEADER;      \
  type *value;                  \
  const type def_val;           \
} MYSQL_SYSVAR_NAME(name)

#define DECLARE_MYSQL_SYSVAR_SIMPLE(name, type) struct { \
  MYSQL_PLUGIN_VAR_HEADER;      \
  type *value; type def_val;    \
  type min_val; type max_val;   \
  type blk_sz;                  \
} MYSQL_SYSVAR_NAME(name)

#define DECLARE_MYSQL_SYSVAR_TYPELIB(name, type) struct { \
  MYSQL_PLUGIN_VAR_HEADER;      \
  type *value; type def_val;    \
  TYPELIB *typelib;             \
} MYSQL_SYSVAR_NAME(name)

#define DECLARE_THDVAR_FUNC(type) \
  type *(*resolve)(MYSQL_THD thd, int offset)

#define DECLARE_MYSQL_THDVAR_BASIC(name, type) struct { \
  MYSQL_PLUGIN_VAR_HEADER;      \
  int offset;                   \
  const type def_val;           \
  DECLARE_THDVAR_FUNC(type);    \
} MYSQL_SYSVAR_NAME(name)

#define DECLARE_MYSQL_THDVAR_SIMPLE(name, type) struct { \
  MYSQL_PLUGIN_VAR_HEADER;      \
  int offset;                   \
  type def_val; type min_val;   \
  type max_val; type blk_sz;    \
  DECLARE_THDVAR_FUNC(type);    \
} MYSQL_SYSVAR_NAME(name)

#define DECLARE_MYSQL_THDVAR_TYPELIB(name, type) struct { \
  MYSQL_PLUGIN_VAR_HEADER;      \
  int offset;                   \
  type def_val;                 \
  DECLARE_THDVAR_FUNC(type);    \
  TYPELIB *typelib;             \
} MYSQL_SYSVAR_NAME(name)


/*
  the following declarations are for use by plugin implementors
*/

#define MYSQL_SYSVAR_BOOL(name, varname, opt, comment, check, update, def) \
DECLARE_MYSQL_SYSVAR_BASIC(name, char) = { \
  PLUGIN_VAR_BOOL | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def}

#define MYSQL_SYSVAR_STR(name, varname, opt, comment, check, update, def) \
DECLARE_MYSQL_SYSVAR_BASIC(name, char *) = { \
  PLUGIN_VAR_STR | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def}

#define MYSQL_SYSVAR_INT(name, varname, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYSQL_SYSVAR_SIMPLE(name, int) = { \
  PLUGIN_VAR_INT | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def, min, max, blk }

#define MYSQL_SYSVAR_UINT(name, varname, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYSQL_SYSVAR_SIMPLE(name, unsigned int) = { \
  PLUGIN_VAR_INT | PLUGIN_VAR_UNSIGNED | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def, min, max, blk }

#define MYSQL_SYSVAR_LONG(name, varname, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYSQL_SYSVAR_SIMPLE(name, long) = { \
  PLUGIN_VAR_LONG | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def, min, max, blk }

#define MYSQL_SYSVAR_ULONG(name, varname, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYSQL_SYSVAR_SIMPLE(name, unsigned long) = { \
  PLUGIN_VAR_LONG | PLUGIN_VAR_UNSIGNED | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def, min, max, blk }

#define MYSQL_SYSVAR_LONGLONG(name, varname, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYSQL_SYSVAR_SIMPLE(name, long long) = { \
  PLUGIN_VAR_LONGLONG | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def, min, max, blk }

#define MYSQL_SYSVAR_ULONGLONG(name, varname, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYSQL_SYSVAR_SIMPLE(name, unsigned long long) = { \
  PLUGIN_VAR_LONGLONG | PLUGIN_VAR_UNSIGNED | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def, min, max, blk }

#define MYSQL_SYSVAR_ENUM(name, varname, opt, comment, check, update, def, typelib) \
DECLARE_MYSQL_SYSVAR_TYPELIB(name, unsigned long) = { \
  PLUGIN_VAR_ENUM | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def, typelib }

#define MYSQL_SYSVAR_SET(name, varname, opt, comment, check, update, def, typelib) \
DECLARE_MYSQL_SYSVAR_TYPELIB(name, unsigned long long) = { \
  PLUGIN_VAR_SET | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def, typelib }

#define MYSQL_SYSVAR_DOUBLE(name, varname, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYSQL_SYSVAR_SIMPLE(name, double) = { \
  PLUGIN_VAR_DOUBLE | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def, min, max, blk }

#define MYSQL_THDVAR_BOOL(name, opt, comment, check, update, def) \
DECLARE_MYSQL_THDVAR_BASIC(name, char) = { \
  PLUGIN_VAR_BOOL | PLUGIN_VAR_THDLOCAL | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, NULL}

#define MYSQL_THDVAR_STR(name, opt, comment, check, update, def) \
DECLARE_MYSQL_THDVAR_BASIC(name, char *) = { \
  PLUGIN_VAR_STR | PLUGIN_VAR_THDLOCAL | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, NULL}

#define MYSQL_THDVAR_INT(name, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYSQL_THDVAR_SIMPLE(name, int) = { \
  PLUGIN_VAR_INT | PLUGIN_VAR_THDLOCAL | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, min, max, blk, NULL }

#define MYSQL_THDVAR_UINT(name, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYSQL_THDVAR_SIMPLE(name, unsigned int) = { \
  PLUGIN_VAR_INT | PLUGIN_VAR_THDLOCAL | PLUGIN_VAR_UNSIGNED | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, min, max, blk, NULL }

#define MYSQL_THDVAR_LONG(name, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYSQL_THDVAR_SIMPLE(name, long) = { \
  PLUGIN_VAR_LONG | PLUGIN_VAR_THDLOCAL | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, min, max, blk, NULL }

#define MYSQL_THDVAR_ULONG(name, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYSQL_THDVAR_SIMPLE(name, unsigned long) = { \
  PLUGIN_VAR_LONG | PLUGIN_VAR_THDLOCAL | PLUGIN_VAR_UNSIGNED | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, u