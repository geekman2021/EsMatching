INDX( 	 ؒ,�          (   �
  �       r  �  �            k�    p Z     b�    �P�7z�� (�_�X��P�7z���P�7z��       V              F I L T E R ~ 1 . P H P       l�    � n     b�    (��7z�� (�_�X�(��7z��(��7z��       �              F u n c t i o n E x p r e s s i o n . p h p   l�    p Z     b�    (��7z�� (�_�X�(��7z��(��7z��       �              F U N C T I ~ 1 . P H P       m�    � l     b�    (��7z�� (�_�X�(��7z��(��7z��       6              G e t A t t  E x p r e s s i o n . p h p    m�    p Z     b�    (��7z�� (�_�X�(��7z��(��7z��       6              G E T A T T ~ 1 . P H P       n�    p `     b�    (��7z�� (�_�X�(��7z��(��7z��       �              I n l i n e P r i n t . p h p n�    p Z     b�    (��7z�� (�_�X�(��7z��(��7z��       �              I N L I N E ~ 1 . P H P       o�    � r     b�    (��7z�� (�_�X�(��7z��(��7z��       L              M e t h o d C a l l E x p r e s s i o n . p h p       o�    p Z     b�    (��7z�� (�_�X�(��7z��(��7z��       L              M E T H O D ~ 1 . P H P       p�    x f     b�    (��7z�� (�_�X�(��7z��(��7z��       �              N a m e E x p r e s s i o n . p h p   p�    p Z     b�    (��7z�� (�_�X�(��7z��(��7z��       �              N A M E E X ~ 1 . P H P       q�    � v     b�    (��7z�� (�_�X�(��7z��(��7z��       �              N u l l C o a l e s c e E x p r e s s i o n . p h p   q�    p Z     b�    (��7z�� (�_�X (��7z��(��7z��       �              N U L L C O ~ 1 . P H P       r�    � j     b�    (��7z�� (�_�X�(��7z��(��7z��       �              P a r e n t E x p r e s s i o n . p h p      r�    p Z     b�    (��7z�� (�_�X�(��7z��(��7z��       �              P A R E N T ~ 1 . P H P       s�    � n     b�    (��7z�� (�_�X�(��7z��(��7z��       �              T e m p N a m e E x p r e s s i o n . p h p   s�    p Z     b�    (��7z�� (�_�X�(��7z��(��7z��       �             T E M P N A ~ 1 . P H P       ��    ` J     b�    �<�7z��۞�7z��۞�7z��k��Q���                       T e s t E x p t�    x f     b�    k�7z�� (�_�X�k�7z��k�7z��       �              T e s t E x p r e s s i o n . p h p   t�    p Z     b�    k�7z�� (�_�X�k�7z��k�7z��       �              T E S T E X ~ 1 . P H P       ��    ` L     b�    ۞�7z��8�7z��8�7z��k��Q���                       U n a r y d i u�    � n     b�    k�7z�� (�_�X�k�7z� k�7z���      �              V a r i a d i c E x p r e s s i o n . p h p   u�    p Z     b�    k�7z�� (�_�X�k�7z��k�7z���      �              V A R I A D ~ 1 . P H P                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  = 0;
  DBUG_ASSERT(thd->wsrep_next_trx_id() != WSREP_UNDEFINED_TRX_ID);
  DBUG_ASSERT(thd->wsrep_cs().mode() == Wsrep_client_state::m_local);
  if (thd->wsrep_trx().active() == false)
  {
    ret= wsrep_start_transaction(thd, thd->wsrep_next_trx_id());
  }
  return ret;
}

/*
  Called after each row operation.

  Return zero on succes, non-zero on failure.
 */
static inline int wsrep_after_row_internal(THD* thd)
{
  if (thd->wsrep_cs().state() != wsrep::client_state::s_none  &&
      wsrep_thd_is_local(thd))
  {
    if (wsrep_check_pk(thd))
    {
      return 1;
    }
    else if (wsrep_streaming_enabled(thd))
    {
      return thd->wsrep_cs().after_row();
    }
  }
  return 0;
}

/*
  Helper method to determine whether commit time hooks
  should be run for the transaction.

  Commit hooks must be run in the following cases:
  - The transaction is local and has generated write set and is committing.
  - The transaction has been BF aborted
  - Is running in high priority mode and is ordered. This can be replayer,
    applier or storage access.
 */
static inline bool wsrep_run_commit_hook(THD* thd, bool all)
{
  DBUG_ENTER("wsrep_run_commit_hook");
  DBUG_PRINT("wsrep", ("Is_active: %d is_real %d has_changes %d is_applying %d "
                       "is_ordered: %d",
                       wsrep_is_active(thd), wsrep_is_real(thd, all),
                       wsrep_has_changes(thd), wsrep_thd_is_applying(thd),
                       wsrep_is_ordered(thd)));
  /* Is MST commit or autocommit? */
  bool ret= wsrep_is_active(thd) && wsrep_is_real(thd, all);
  /* Do not commit if we are aborting */
  ret= ret && (thd->wsrep_trx().state() != wsrep::transaction::s_aborting);
  if (ret && !(wsrep_has_changes(thd) ||  /* Has generated write set */
               /* Is high priority (replay, applier, storage) and the
                  transaction is scheduled for commit ordering */
               (wsrep_thd_is_applying(thd) && wsrep_is_ordered(thd))))
  {
    mysql_mutex_lock(&thd->LOCK_thd_data);
    DBUG_PRINT("wsrep", ("state: %s",
                         wsrep::to_c_string(thd->wsrep_trx().state())));
    /* Transaction is local but has no changes, the commit hooks will
       be skipped and the wsrep transaction is terminated in
       wsrep_commit_empty() */
    if (thd->wsrep_trx().state() == wsrep::transaction::s_executing)
    {
      ret= false;
    }
    mysql_mutex_unlock(&thd->LOCK_thd_data);
  }
  DBUG_PRINT("wsrep", ("return: %d", ret));
  DBUG_RETURN(ret);
}

/*
  Called before the transaction is prepared.

  Return zero on succes, non-zero on failure.
 */
static inline int wsrep_before_prepare(THD* thd, bool all)
{
  DBUG_ENTER("wsrep_before_prepare");
  WSREP_DEBUG("wsrep_before_prepare: %d", wsrep_is_real(thd, all));
  int ret= 0;
  DBUG_ASSERT(wsrep_run_commit_hook(thd, all));
  if ((ret= thd->wsrep_cs().before_prepare()) == 0)
  {
    DBUG_ASSERT(!thd->wsrep_trx().ws_meta().gtid().is_undefined());
    wsrep_xid_init(&thd->wsrep_xid,
                   thd->wsrep_trx().ws_meta().gtid(),
                   wsrep_gtid_server.gtid());
  }
  DBUG_RETURN(ret);
}

/*
  Called after the transaction has been prepared.

  Return zero on succes, non-zero on failure.
 */
static inline int wsrep_after_prepare(THD* thd, bool all)
{
  DBUG_ENTER("wsrep_after_prepare");
  WSREP_DEBUG("wsrep_after_prepare: %d", wsrep_is_real(thd, all));
  DBUG_ASSERT(wsrep_run_commit_hook(thd, all));
  int ret= thd->wsrep_cs().after_prepare();
  DBUG_ASSERT(ret == 0 || thd->wsrep_cs().current_error() ||
              thd->wsrep_cs().transaction().state() == wsrep::transaction::s_must_replay);
  DBUG_RETURN(ret);
}

/*
  Called before the transaction is committed.

  This function must be called from both client and
  applier contexts before commit.

  Return zero on succes, non-zero on failure.
 */
static inline int wsrep_before_commit(THD* thd, bool all)
{
  DBUG_ENTER("wsrep_before_commit");
  WSREP_DEBUG("wsrep_before_commit: %d, %lld",
              wsrep_is_real(thd, all),
              (long long)wsrep_thd_trx_seqno(thd));
  int ret= 0;
  DBUG_ASSERT(wsrep_run_commit_hook(thd, all));
  if ((ret= thd->wsrep_cs().before_commit()) == 0)
  {
    DBUG_ASSERT(!thd->wsrep_trx().ws_meta().gtid().is_undefined());
    if (!thd->variables.gtid_seq_no && 
        (thd->wsrep_trx().ws_meta().flags() & wsrep::provider::flag::commit))
    {
        uint64 seqno= 0;
        if (thd->variables.wsrep_gtid_seq_no &&
            thd->variables.wsrep_gtid_seq_no > wsrep_gtid_server.seqno())
        {
          seqno= thd->variables.wsrep_gtid_seq_no;
          wsrep_gtid_server.seqno(thd->variables.wsrep_gtid_seq_no);
        }
        else
        {
          seqno= wsrep_gtid_server.seqno_inc();
        }
        thd->variables.wsrep_gtid_seq_no= 0;
        thd->wsrep_current_gtid_seqno= seqno;
        if (mysql_bin_log.is_open() && wsrep_gtid_mode)
        {
          thd->variables.gtid_seq_no= seqno;
          thd->variables.gtid_domain_id= wsrep_gtid_server.domain_id;
          thd->variables.server_id= wsrep_gtid_server.server_id;
        }
    }

    wsrep_xid_init(&thd->wsrep_xid,
                   thd->wsrep_trx().ws_meta().gtid(),
                   wsrep_gtid_server.gtid());
    wsrep_register_for_group_commit(thd);
  }
  DBUG_RETURN(ret);
}

/*
  Called after the transaction has been ordered for commit.

  This function must be called from both client and
  applier contexts after the commit has been ordered.

  @param thd Pointer to THD
  @param all 
  @param err Error buffer in case of applying error

  Return zero on succes, non-zero on failure.
 */
static inline int wsrep_ordered_commit(THD* thd, bool all)
{
  DBUG_ENTER("wsrep_ordered_commit");
  WSREP_DEBUG("wsrep_ordered_commit: %d", wsrep_is_real(thd, all));
  DBUG_ASSERT(wsrep_run_commit_hook(thd, all));
  DBUG_RETURN(thd->wsrep_cs().ordered_commit());
}

/*
  Called after the transaction has been committed.

  Return zero on succes, non-zero on failure.
 */
static inline int wsrep_after_commit(THD* thd, bool all)
{
  DBUG_ENTER("wsrep_after_commit");
  WSREP_DEBUG("wsrep_after_commit: %d, %d, %lld, %d",
              wsrep_is_real(thd, all),
              wsrep_is_active(thd),
              (long long)wsrep_thd_trx_seqno(thd),
              wsrep_has_changes(thd));
  DBUG_ASSERT(wsrep_run_commit_hook(thd, all));
  if (thd->internal_transaction())
    DBUG_RETURN(0);
  int ret= 0;
  if (thd->wsrep_trx().state() == wsrep::transaction::s_committing)
  {
    ret= thd->wsrep_cs().ordered_commit();
  }
  wsrep_unregister_from_group_commit(thd);
  thd->wsrep_xid.null();
  DBUG_RETURN(ret || thd->wsrep_cs().after_commit());
}

/*
  Called before the transaction is rolled back.

  Return zero on succes, non-zero on failure.
 */
static inline int wsrep_before_rollback(THD* thd, bool all)
{
  DBUG_ENTER("wsrep_before_rollback");
  int ret= 0;
  if (wsrep_is_active(thd))
  {
    if (!all && thd->in_active_multi_stmt_transaction())
    {
      if (wsrep_emulate_bin_log)
      {
        wsrep_thd_binlog_stmt_rollback(thd);
      }

      if (thd->wsrep_trx().is_streaming() &&
          (wsrep_fragments_certified_for_stmt(thd) > 0))
      {
        /* Non-safe statement rollback during SR multi statement
           transaction. A statement rollback is considered unsafe, if
           the same statement has already replicated one or more fragments.
           Self abort the transaction, the actual rollback and error
           handling will be done in after statement phase. */
        WSREP_DEBUG("statement rollback is not safe for streaming replication");
        wsrep_thd_self_abort(thd);
        ret= 0;
      }
    }
    else if (wsrep_is_real(thd, all) &&
             thd->wsrep_trx().state() != wsrep::transaction::s_aborted)
    {
      /* Real transaction rolling back and wsrep abort not completed
         yet */
      /* Reset XID so that it does not trigger writing serialization
         history in InnoDB. This needs to be avoided because rollback
         may happen out of order and replay may follow. */
      thd->wsrep_xid.null();
      ret= thd->wsrep_cs().before_rollback();
    }
  }
  DBUG_RETURN(ret);
}

/*
  Called after the transaction has been rolled back.

  Return zero on succes, non-zero on failure.
 */
static inline int wsrep_after_rollback(THD* thd, bool all)
{
  DBUG_ENTER("wsrep_after_rollback");
  DBUG_RETURN((wsrep_is_real(thd, all) && wsrep_is_active(thd) &&
               thd->wsrep_cs().transaction().state() !=
               wsrep::transaction::s_aborted) ?
              thd->wsrep_cs().after_rollback() : 0);
}

static inline int wsrep_before_statement(THD* thd)
{
  return (thd->wsrep_cs().state() != wsrep::client_state::s_none &&
          !thd->internal_transaction() ?
	  thd->wsrep_cs().before_statement() : 0);
}

static inline
int wsrep_after_statement(THD* thd)
{
  DBUG_ENTER("wsrep_after_statement");
  WSREP_DEBUG("wsrep_after_statement for %lu client_state %s "
              " client_mode %s trans_state %s",
              thd_get_thread_id(thd),
              wsrep::to_c_string(thd->wsrep_cs().state()),
              wsrep::to_c_string(thd->wsrep_cs().mode()),
              wsrep::to_c_string(thd->wsrep_cs().transaction().state()));
  DBUG_RETURN((thd->wsrep_cs().state() != wsrep::client_state::s_none &&
               thd->wsrep_cs().mode() == Wsrep_client_state::m_local) &&
              !thd->internal_transaction() ?
              thd->wsrep_cs().after_statement() : 0);
}

static inline void wsrep_after_apply(THD* thd)
{
  DBUG_ASSERT(wsrep_thd_is_applying(thd));
  WSREP_DEBUG("wsrep_after_apply %lld", thd->thread_id);
  if (!thd->internal_transaction())
    thd->wsrep_cs().after_applying();
}

static inline void wsrep_open(THD* thd)
{
  DBUG_ENTER("wsrep_open");
  if (WSREP_ON_)
  {
    /* WSREP_PROVIDER_EXISTS_ cannot be set if WSREP_ON_ is not set */
    DBUG_ASSERT(WSREP_PROVIDER_EXISTS_);
    thd->wsrep_cs().open(wsrep::client_id(thd->thread_id));
    thd->wsrep_cs().debug_log_level(wsrep_debug);
    if (!thd->wsrep_applier && thd->variables.wsrep_trx_fragment_size)
    {
      thd->wsrep_cs().enable_streaming(
        wsrep_fragment_unit(thd->variables.wsrep_trx_fragment_unit),
        size_t(thd->variables.wsrep_trx_fragment_size));
    }
  }
  DBUG_VOID_RETURN;
}

static inline void wsrep_close(THD* thd)
{
  DBUG_ENTER("wsrep_close");
  if (thd->wsrep_cs().state() != wsrep::client_state::s_none &&
      !thd->internal_transaction())
  {
    thd->wsrep_cs().close();
  }
  DBUG_VOID_RETURN;
}

static inline void wsrep_cleanup(THD* thd)
{
  DBUG_ENTER("wsrep_cleanup");
  if (thd->wsrep_cs().state() != wsrep::client_state::s_none)
  {
    thd->wsrep_cs().cleanup();
  }
  DBUG_VOID_RETURN;
}

static inline void
wsrep_wait_rollback_complete_and_acquire_ownership(THD *thd)
{
  DBUG_ENTER("wsrep_wait_rollback_complete_and_acquire_ownership");
  if (thd->wsrep_cs().state() != wsrep::client_state::s_none &&
      !thd->internal_transaction())
  {
    thd->wsrep_cs().wait_rollback_complete_and_acquire_ownership();
  }
  DBUG_VOID_RETURN;
}

static inline int wsrep_before_command(THD* thd, bool keep_command_error)
{
  return (thd->wsrep_cs().state() != wsrep::client_state::s_none &&
          !thd->internal_transaction() ?
          thd->wsrep_cs().before_command(keep_command_error) : 0);
}

static inline int wsrep_before_command(THD* thd)
{
  return wsrep_before_command(thd, false);
}

/*
  Called after each command.

  Return zero on success, non-zero on failure.
*/
static inline void wsrep_after_command_before_result(THD* thd)
{
  if (thd->wsrep_cs().state() != wsrep::client_state::s_none &&
      !thd->internal_transaction())
  {
    thd->wsrep_cs().after_command_before_result();
  }
}

static inline void wsrep_after_command_after_result(THD* thd)
{
  if (thd->wsrep_cs().state() != wsrep::client_state::s_none &&
      !thd->internal_transaction())
  {
    thd->wsrep_cs().after_command_after_result();
  }
}

static inline void wsrep_after_command_ignore_result(THD* thd)
{
  wsrep_after_command_before_result(thd);
  DBUG_ASSERT(!thd->wsrep_cs().current_error());
  wsrep_after_command_after_result(thd);
}

static inline enum wsrep::client_error wsrep_current_error(THD* thd)
{
  return thd->wsrep_cs().current_error();
}

static inline enum wsrep::provider::status
wsrep_current_error_status(THD* thd)
{
  return thd->wsrep_cs().current_error_status();
}

#endif /* WSREP_TRANS_OBSERVER */
