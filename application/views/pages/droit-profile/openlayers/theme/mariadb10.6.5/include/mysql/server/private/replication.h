                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 Failure
  */
  int (*after_reset_master)(Binlog_transmit_param *param);
} Binlog_transmit_observer;

/**
   Binlog relay IO flags
*/
enum Binlog_relay_IO_flags {
  /** Binary relay log was sync:ed */
  BINLOG_RELAY_IS_SYNCED = 1
};


/**
  Replication binlog relay IO observer parameter
*/
typedef struct Binlog_relay_IO_param {
  uint32 server_id;

  /* Master host, user and port */
  char *host;
  char *user;
  unsigned int port;

  char *master_log_name;
  my_off_t master_log_pos;

  MYSQL *mysql;                        /* the connection to master */
} Binlog_relay_IO_param;

/**
   Observes and extends the service of slave IO thread.
*/
typedef struct Binlog_relay_IO_observer {
  uint32 len;

  /**
     This callback is called when slave IO thread starts

     @param param Observer common parameter

     @retval 0 Sucess
     @retval 1 Failure
  */
  int (*thread_start)(Binlog_relay_IO_param *param);

  /**
     This callback is called when slave IO thread stops

     @param param Observer common parameter

     @retval 0 Sucess
     @retval 1 Failure
  */
  int (*thread_stop)(Binlog_relay_IO_param *param);

  /**
     This callback is called before slave requesting binlog transmission from master

     This is called before slave issuing BINLOG_DUMP command to master
     to request binlog.

     @param param Observer common parameter
     @param flags binlog dump flags

     @retval 0 Sucess
     @retval 1 Failure
  */
  int (*before_request_transmit)(Binlog_relay_IO_param *param, uint32 flags);

  /**
     This callback is called after read an event packet from master

     @param param Observer common parameter
     @param packet The event packet read from master
     @param len Length of the event packet read from master
     @param event_buf The event packet return after process
     @param event_len The length of event packet return after process

     @retval 0 Sucess
     @retval 1 Failure
  */
  int (*after_read_event)(Binlog_relay_IO_param *param,
                          const char *packet, unsigned long len,
                          const char **event_buf, unsigned long *event_len);

  /**
     This callback is called after written an event packet to relay log

     @param param Observer common parameter
     @param event_buf Event packet written to relay log
     @param event_len Length of the event packet written to relay log
     @param flags flags for relay log

     @retval 0 Sucess
     @retval 1 Failure
  */
  int (*after_queue_event)(Binlog_relay_IO_param *param,
                           const char *event_buf, unsigned long event_len,
                           uint32 flags);

  /**
     This callback is called after reset slave relay log IO status
     
     @param param Observer common parameter

     @retval 0 Sucess
     @retval 1 Failure
  */
  int (*after_reset_slave)(Binlog_relay_IO_param *param);
} Binlog_relay_IO_observer;


/**
   Register a transaction observer

   @param observer The transaction observer to register
   @param p pointer to the internal plugin structure

   @retval 0 Sucess
   @retval 1 Observer already exists
*/
int register_trans_observer(Trans_observer *observer, void *p);

/**
   Unregister a transaction observer

   @param observer The transaction observer to unregister
   @param p pointer to the internal plugin structure

   @retval 0 Sucess
   @retval 1 Observer not exists
*/
int unregister_trans_observer(Trans_observer *observer, void *p);

/**
   Register a binlog storage observer

   @param observer The binlog storage observer to register
   @param p pointer to the internal plugin structure

   @retval 0 Sucess
   @retval 1 Observer already exists
*/
int register_binlog_storage_observer(Binlog_storage_observer *observer, void *p);

/**
   Unregister a binlog storage observer

   @param observer The binlog storage observer to unregister
   @param p pointer to the internal plugin structure

   @retval 0 Sucess
   @retval 1 Observer not exists
*/
int unregister_binlog_storage_observer(Binlog_storage_observer *observer, void *p);

/**
   Register a binlog transmit observer

   @param observer The binlog transmit observer to register
   @param p pointer to the internal plugin structure

   @retval 0 Sucess
   @retval 1 Observer already exists
*/
int register_binlog_transmit_observer(Binlog_transmit_observer *observer, void *p);

/**
   Unregister a binlog transmit observer

   @param observer The binlog transmit observer to unregister
   @param p pointer to the internal plugin structure

   @retval 0 Sucess
   @retval 1 Observer not exists
*/
int unregister_binlog_transmit_observer(Binlog_transmit_observer *observer, void *p);

/**
   Register a binlog relay IO (slave IO thread) observer

   @param observer The binlog relay IO observer to register
   @param p pointer to the internal plugin structure

   @retval 0 Sucess
   @retval 1 Observer already exists
*/
int register_binlog_relay_io_observer(Binlog_relay_IO_observer *observer, void *p);

/**
   Unregister a binlog relay IO (slave IO thread) observer

   @param observer The binlog relay IO observer to unregister
   @param p pointer to the internal plugin structure

   @retval 0 Sucess
   @retval 1 Observer not exists
*/
int unregister_binlog_relay_io_observer(Binlog_relay_IO_observer *observer, void *p);

/**
   Connect to master

   This function can only used in the slave I/O thread context, and
   will use the same master information to do the connection.

   @code
   MYSQL *mysql = mysql_init(NULL);
   if (rpl_connect_master(mysql))
   {
     // do stuff with the connection
   }
   mysql_close(mysql); // close the connection
   @endcode
   
   @param mysql address of MYSQL structure to use, pass NULL will
   create a new one

   @return address of MYSQL structure on success, NULL on failure
*/
MYSQL *rpl_connect_master(MYSQL *mysql);

/**
   Get the value of user variable as an integer.

   This function will return the value of variable @a name as an
   integer. If the original value of the variable is not an integer,
   the value will be converted into an integer.

   @param name     user variable name
   @param value    pointer to return the value
   @param null_value if not NULL, the function will set it to true if
   the value of variable is null, set to false if not

   @retval 0 Success
   @retval 1 Variable not found
*/
int get_user_var_int(const char *name,
                     long long int *value, int *null_value);

/**
   Get the value of user variable as a double precision float number.

   This function will return the value of variable @a name as real
   number. If the original value of the variable is not a real number,
   the value will be converted into a real number.

   @param name     user variable name
   @param value    pointer to return the value
   @param null_value if not NULL, the function will set it to true if
   the value of variable is null, set to false if not

   @retval 0 Success
   @retval 1 Variable not found
*/
int get_user_var_real(const char *name,
                      double *value, int *null_value);

/**
   Get the value of user variable as a string.

   This function will return the value of variable @a name as
   string. If the original value of the variable is not a string,
   the value will be converted into a string.

   @param name     user variable name
   @param value    pointer to the value buffer
   @param len      length of the value buffer
   @param precision precision of the value if it is a float number
   @param null_value if not NULL, the function will set it to true if
   the value of variable is null, set to false if not

   @retval 0 Success
   @retval 1 Variable not found
*/
int get_user_var_str(const char *name,
                     char *value, unsigned long len,
                     unsigned int precision, int *null_value);

  

#ifdef __cplusplus
}
#endif
#endif /* REPLICATION_H */
