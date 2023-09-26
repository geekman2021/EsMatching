GIF89a  �  L�<�ޤ|�td�\�֌L�D\�T�҄t�l��|�ڜT�Ll�dL�D��t�ڔd�Tt�lT�LL�<�����tl�\�ڌT�Dd�T�ք��|�ޜt�d|�l\�L                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                !�   ,      � )H����&<H0�8<Xxp@� h�@��D�*p�p@Á��P��0c:p`@�*$p������8� <���!B�C)@ �,t����
QthP��� @��!l	W%�� Ä��b@k� ڽQ�v�P���<&���p3X�p���S.��"�ϠC ;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    OP,
  SQLCOM_SHOW_EXPLAIN, SQLCOM_SHUTDOWN,
  SQLCOM_CREATE_ROLE, SQLCOM_DROP_ROLE, SQLCOM_GRANT_ROLE, SQLCOM_REVOKE_ROLE,
  SQLCOM_COMPOUND,
  SQLCOM_SHOW_GENERIC,
  SQLCOM_ALTER_USER,
  SQLCOM_SHOW_CREATE_USER,
  SQLCOM_EXECUTE_IMMEDIATE,
  SQLCOM_CREATE_SEQUENCE,
  SQLCOM_DROP_SEQUENCE,
  SQLCOM_ALTER_SEQUENCE,
  SQLCOM_CREATE_PACKAGE,
  SQLCOM_DROP_PACKAGE,
  SQLCOM_CREATE_PACKAGE_BODY,
  SQLCOM_DROP_PACKAGE_BODY,
  SQLCOM_SHOW_CREATE_PACKAGE,
  SQLCOM_SHOW_CREATE_PACKAGE_BODY,
  SQLCOM_SHOW_STATUS_PACKAGE,
  SQLCOM_SHOW_STATUS_PACKAGE_BODY,
  SQLCOM_SHOW_PACKAGE_BODY_CODE,
  SQLCOM_BACKUP, SQLCOM_BACKUP_LOCK,

  /*
    When a command is added here, be sure it's also added in mysqld.cc
    in "struct show_var_st com_status_vars[]= {" ...
  */
  /* This should be the last !!! */
  SQLCOM_END
};


class Storage_engine_name
{
protected:
  LEX_CSTRING m_storage_engine_name;
public:
  Storage_engine_name()
  {
    m_storage_engine_name.str= NULL;
    m_storage_engine_name.length= 0;
  }
  Storage_engine_name(const LEX_CSTRING &name)
   :m_storage_engine_name(name)
  { }
  Storage_engine_name(const LEX_STRING &name)
  {
    m_storage_engine_name.str= name.str;
    m_storage_engine_name.length= name.length;
  }
  bool resolve_storage_engine_with_error(THD *thd,
                                         handlerton **ha,
                                         bool tmp_table);
  bool is_set() { return m_storage_engine_name.str != NULL; }
};


/**
  @class Sql_cmd - Representation of an SQL command.

  This class is an interface between the parser and the runtime.
  The parser builds the appropriate derived classes of Sql_cmd
  to represent a SQL statement in the parsed tree.
  The execute() method in the derived classes of Sql_cmd contain the runtime
  implementation.
  Note that this interface is used for SQL statements recently implemented,
  the code for older statements tend to load the LEX structure with more
  attributes instead.
  Implement new statements by sub-classing Sql_cmd, as this improves
  code modularity (see the 'big switch' in dispatch_command()), and decreases
  the total size of the LEX structure (therefore saving memory in stored
  programs).
  The recommended name of a derived class of Sql_cmd is Sql_cmd_<derived>.

  Notice that the Sql_cmd class should not be confused with the
  Statement class.  Statement is a class that is used to manage an SQL
  command or a set of SQL commands. When the SQL statement text is
  analyzed, the parser will create one or more Sql_cmd objects to
  represent the actual SQL commands.
*/
class Sql_cmd : public Sql_alloc
{
private:
  Sql_cmd(const Sql_cmd &);         // No copy constructor wanted
  void operator=(Sql_cmd &);        // No assignment operator wanted

public:
  /**
    @brief Return the command code for this statement
  */
  virtual enum_sql_command sql_command_code() const = 0;

  /**
    Execute this SQL statement.
    @param thd the current thread.
    @retval false on success.
    @retval true on error
  */
  virtual bool execute(THD *thd) = 0;

  virtual Storage_engine_name *option_storage_engine_name()
  {
    return NULL;
  }

protected:
  Sql_cmd()
  {}

  virtual ~Sql_cmd()
  {
    /*
      Sql_cmd objects are allocated in thd->mem_root.
      In MySQL, the C++ destructor is never called, the underlying MEM_ROOT is
      simply destroyed instead.
      Do not rely on the destructor for any cleanup.
    */
    DBUG_ASSERT(FALSE);
  }
};

class Sql_cmd_show_slave_status: public Sql_cmd
{
protected:
  bool show_all_slaves_status;
public:
  Sql_cmd_show_slave_status()
    :show_all_slaves_status(false)
  {}

  Sql_cmd_show_slave_status(bool status_all)
    :show_all_slaves_status(status_all)
  {}

  enum_sql_command sql_command_code() const { return SQLCOM_SHOW_SLAVE_STAT; }

  bool execute(THD *thd);
  bool is_show_all_slaves_stat() { return show_all_slaves_status; }
};


class Sql_cmd_create_table_like: public Sql_cmd,
                                 public Storage_engine_name
{
public:
  Storage_engine_name *option_storage_engine_name() { return this; }
  bool execute(THD *thd);
};

class Sql_cmd_create_table: public Sql_cmd_create_table_like
{
public:
  enum_sql_command sql_command_code() const { return SQLCOM_CREATE_TABLE; }
};

class Sql_cmd_create_sequence: public Sql_cmd_create_table_like
{
public:
  enum_sql_command sql_command_code() const { return SQLCOM_CREATE_SEQUENCE; }
};


/**
  Sql_cmd_call represents the CALL statement.
*/
class Sql_cmd_call : public Sql_cmd
{
public:
  class sp_name *m_name;
  const class Sp_handler *m_handler;
  Sql_cmd_call(class sp_name *name, const class Sp_handler *handler)
   :m_name(name),
    m_handler(handler)
  {}

  virtual ~Sql_cmd_call()
  {}

  /**
    Execute a CALL statement at runtime.
    @param thd the current thread.
    @return false on success.
  */
  bool execute(THD *thd);

  virtual enum_sql_command sql_command_code() const
  {
    return SQLCOM_CALL;
  }
};

#endif // SQL_CMD_INCLUDED
