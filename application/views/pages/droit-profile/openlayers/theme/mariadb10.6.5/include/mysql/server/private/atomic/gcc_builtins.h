_handlers(size_t count);

  const Sql_condition_info *raised_condition() const
  {
    return m_handler_call_stack.elements() ?
      (*m_handler_call_stack.back())->sql_condition : NULL;
  }

  /// Handle current SQL condition (if any).
  ///
  /// This is the public-interface function to handle SQL conditions in
  /// stored routines.
  ///
  /// @param thd            Thread handle.
  /// @param ip[out]        Instruction pointer to the first handler
  ///                       instruction.
  /// @param cur_spi        Current SP instruction.
  ///
  /// @retval true if an SQL-handler has been activated. That means, all of
  /// the following conditions are satisfied:
  ///   - the SP-instruction raised SQL-condition(s),
  ///   - and there is an SQL-handler to process at least one of those
  ///     SQL-conditions,
  ///   - and that SQL-handler has been activated.
  /// Note, that the return value has nothing to do with "error flag"
  /// semantics.
  ///
  /// @retval false otherwise.
  bool handle_sql_condition(THD *thd,
                            uint *ip,
                            const sp_instr *cur_spi);

  /// Remove latest call frame from the handler call stack.
  ///
  /// @param da Diagnostics area containing handled conditions.
  ///
  /// @return continue instruction pointer of the removed handler.
  uint exit_handler(Diagnostics_area *da);

  /////////////////////////////////////////////////////////////////////////
  // Cursors.
  /////////////////////////////////////////////////////////////////////////

  /// Push a cursor to the cursor stack.
  ///
  /// @param cursor The cursor
  ///
  void push_cursor(sp_cursor *cur);

  void pop_cursor(THD *thd);
  /// Pop and delete given number of sp_cursor instance from the cursor stack.
  ///
  /// @param count Number of cursors to pop & delete.
  void pop_cursors(THD *thd, size_t count);

  void pop_all_cursors(THD *thd)
  { pop_cursors(thd, m_ccount); }

  sp_cursor *get_cursor(uint i) const
  { return m_cstack[i]; }

  /////////////////////////////////////////////////////////////////////////
  // CASE expressions.
  /////////////////////////////////////////////////////////////////////////

  /// Set CASE expression to the specified value.
  ///
  /// @param thd             Thread handler.
  /// @param case_expr_id    The CASE expression identifier.
  /// @param case_expr_item  The CASE expression value
  ///
  /// @return error flag.
  /// @retval false on success.
  /// @retval true on error.
  ///
  /// @note The idea is to reuse Item_cache for the expression of the one
  /// CASE statement. This optimization takes place when there is CASE
  /// statement inside of a loop. So, in other words, we will use the same
  /// object on each iteration instead of creating a new one for each
  /// iteration.
  ///
  /// TODO
  ///   Hypothetically, a type of CASE expression can be different for each
  ///   iteration. For instance, this can happen if the expression contains
  ///   a session variable (something like @@VAR) and its type is changed
  ///   from one iteration to another.
  ///
  ///   In order to cope with this problem, we check type each time, when we
  ///   use already created object. If the type does not match, we re-create
  ///   Item.  This also can (should?) be optimized.
  bool set_case_expr(THD *thd, int case_expr_id, Item **case_expr_item_ptr);

  Item *get_case_expr(int case_expr_id) const
  { return m_case_expr_holders[case_expr_id]; }

  Item ** get_case_expr_addr(int case_expr_id) const
  { return (Item**) m_case_expr_holders.array() + case_expr_id; }

private:
  /// Internal function to allocate memory for arrays.
  ///
  /// @param thd Thread handle.
  ///
  /// @return error flag: false on success, true in case of failure.
  bool alloc_arrays(THD *thd)