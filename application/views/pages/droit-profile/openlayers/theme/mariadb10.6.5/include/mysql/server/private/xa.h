/// Text message.
    char *message;

    /// The constructor.
    ///
    /// @param _sql_condition  The SQL condition.
    /// @param arena           Query arena for SP
    Sql_condition_info(const Sql_condition *_sql_condition,
                       Query_arena *arena)
      :Sql_condition_identity(*_sql_condition)
    {
      message= strdup_root(arena->mem_root, _sql_condition->get_message_text());
    }
  };

private:
  /// This class represents a call frame of SQL-handler (one invocation of a
  /// handler). Basically, it's needed to store continue instruction pointer for
  /// CONTINUE SQL-handlers.
  class Handler_call_frame : public Sql_alloc
  {
  public:
    /// SQL-condition, triggered handler activation.
    const Sql_condition_info *sql_condition;

    /// Continue-instruction-pointer for CONTINUE-handlers.
    /// The attribute contains 0 for EXIT-handlers.
    uint continue_ip;

    /// The constructor.
    ///
    /// @param _sql_condition SQL-condition, triggered handler activation.
    /// @param _continue_ip   Continue instruction pointer.
    Handler_call_frame(const Sql_condition_info *_sql_condition,
                       uint _continue_ip)
     :sql_condition(_sql_condition),
      continue_ip(_continue_ip)
    { }
 };

public:
  /// Arena used to (re) allocate items on. E.g. reallocate INOUT/OUT
  /// SP-variables when they don't fit into prealloced items. This is common
  /// situation with String items. It is used mainly in sp_eval_func_item().
  Query_arena *callers_arena;

  /// Flag to end an open result set before start executing an SQL-handler
  /// (if one is found). Otherwise the client will hang due to a violation
  /// of the client/server protocol.
  bool end_partial_result_set;
  bool pause_state;
  bool quit_func;
  uint instr_ptr;

  /// The st