ruct st_typelib *typelib;           /**< Pointer to possible values */
  ulong     var_type;                   /**< GET_BOOL, GET_ULL, etc */
  enum get_opt_arg_type arg_type;       /**< e.g. REQUIRED_ARG or OPT_ARG */
  longlong   def_value;                 /**< Default value */
  longlong   min_value;                 /**< Min allowed value (for numbers) */
  ulonglong  max_value;                 /**< Max allowed value (for numbers) */
  longlong   sub_size;                  /**< Unused                          */
  long       block_size;                /**< Value should be a mult. of this (for numbers) */
  void       *app_type;                 /**< To be used by an application */
};


typedef my_bool (*my_get_one_option)(int, const struct my_option *, char *);
/**
  Used to retrieve a reference to the object (variable) that holds the value
  for the given option. For example, if var_type is GET_UINT, the function
  must return a pointer to a variable of type uint. A argument is stored in
  the location pointed to by the returned pointer.
*/
typedef void *(*my_getopt_value)(const char *, size_t, const struct my_option *,
                                 int *);


extern char *disabled_my_option;
extern my_bool my_getopt_print_errors;
extern my_bool my_getopt_skip_unknown;
extern my_error_reporter my_getopt_error_reporter;

extern int handle_options (int *argc, char ***argv, 
			   const struct my_option *longopts, my_get_one_option);
extern int my_handle_options (int *argc, char ***argv,
                              const struct my_option *longopts,
                              my_get_one_option,
                              const char **command_list, my_bool ignore_unknown_option);
extern void print_cmdline_password_warning();
extern void my_cleanup_options(const struct my_option *options);
extern void my_cleanup_options(const struct my_option *options);
extern void my_print_help(const struct my_option *options);
extern void my_print_variables(const struct my_option *options);
extern void my_print_variables_ex(const struct my_option *options, FILE* file);
extern void my_getopt_register_get_addr(my_getopt_value);

ulonglong getopt_ull_limit_value(ulonglong num, const struct my_option *optp,
              