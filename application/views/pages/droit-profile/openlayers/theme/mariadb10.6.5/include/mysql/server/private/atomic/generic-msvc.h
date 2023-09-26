Item-adapter (Item_field) for each SP-var field.
  ///
  /// param thd Thread handle.
  ///
  /// @return error flag.
  /// @retval false on success.
  /// @retval true on error.
  bool init_var_items(THD *thd, List<Spvar_definition> &defs);

  /// Create an instance of appropriate Item_cache class depending on the
  /// specified type in the callers arena.
  ///
  /// @note We should create cache items in the callers arena, as they are
  /// used between in several instructions.
  ///
  /// @param thd   Thread handler.
  /// @param item  Item to get the expression type.
  ///
  /// @return Pointer to valid object on success, or NULL in case of error.
  Item_cache *create_case_expr_holder(THD *thd, const Item *item) const;

  Virtual_tmp_table *virtual_tmp_table_for_row(uint idx);

private:
  /// Top-level (root) parsing context for this runtime context.
  const sp_pcontext *m_root_parsing_ctx;

  /// Virtual table for storing SP-variables.
  Virtual_tmp_table *m_var_table;

  /// Collection of Item_field proxies, each of them points to the
  /// corresponding field in m_var_table.
  Bounds_checked_array<Item_field *> m_var_items;

  /// This is a pointer to a field, which should contain return value for
  /// stored functions (only). For stored procedures, this pointer is NULL.
  Field *m_return_value_fld;

  /// Indicates whether the return value (in m_return_value_fld) has been
  /// set during execution.
  bool m_return_value_set;

  /// Flag to tell if the runtime context is created for a sub-statement.
  bool m_in_sub_stmt;

  /// Stack of visible handlers.
  Dynamic_array<sp_instr_hpush_jump *> m_handlers;

  /// Stack of caught SQL conditions.
  Dynamic_array<Handler_call_frame *> m_handler_call_stack;

  /// Stack of cursors.
  Bounds_checked_array<sp_cursor *> m_cstack;

  /// Current number of cursors in m_cstack.
  uint m_ccount;

  /// Array of CASE expression holders.
  Bounds_checked_array<Item_cache *> m_case_expr_holders;
}; // class sp_rcontext : public Sql_alloc

#endif /* _SP_RCONTEXT_H_ */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      