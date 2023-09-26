                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ner to store info the set of elements in the filter */
  Rowid_filter_container *container;

  Rowid_filter_tracker *tracker;

public:
  Rowid_filter(Rowid_filter_container *container_arg)
    : container(container_arg) {}

  /*
    Build the filter :
    fill it with info on the set of elements placed there
  */
  virtual bool build() = 0;

  /*
    Check whether an element is in the filter.
    Returns false is the elements is definitely not in the filter.
  */
  virtual bool check(char *elem) = 0;

  virtual ~Rowid_filter() {}

  Rowid_filter_container *get_container() { return container; }

  void set_tracker(Rowid_filter_tracker *track_arg) { tracker= track_arg; }
  Rowid_filter_tracker *get_tracker() { return tracker; }
};


/**
  @class Rowid_filter_container

  The implementation of the Rowid_interface used for pk-filters
  that are filled when performing range index scans.
*/

class Range_rowid_filter: public Rowid_filter
{
  /* The table for which the rowid filter is built */
  TABLE *table;
  /* The select to perform the range scan to fill the filter */
  SQL_SELECT *select;
  /* The cost info on the filter (used for EXPLAIN/ANALYZE) */
  Range_rowid_filter_cost_info *cost_info;

public:
  Range_rowid_filter(TABLE *tab,
                     Range_rowid_filter_cost_info *cost_arg,
                     Rowid_filter_container *container_arg,
                     SQL_SELECT *sel)
    : Rowid_filter(container_arg), table(tab), select(sel), cost_info(cost_arg)
  {}

  ~Range_rowid_filter();

  bool build() { return fill(); }

  bool check(char *elem)
  {
    bool was_checked= container->check(table, elem);
    tracker->increment_checked_elements_count(was_checked);
    return was_checked;
  }

  bool fill();

  SQL_SELECT *get_select() { return select; }
};


/**
  @class Refpos_container_sorted_array

  The wrapper class over Dynamic_array<char> to facilitate operations over
  array of elements of the type char[N] where N is the same for all elements
*/

class Refpos_container_sorted_array : public Sql_alloc
{
  /* 
    Maximum number of elements in the array
    (Now is used only at the initialization of the dynamic array)
  */
  uint max_elements;
  /* Number of bytes allocated for an element */
  uint elem_size;
  /* The dynamic array over which the wrapper is built */
  Dynamic_array<char> *array;

public:

 Refpos_container_sorted_array(uint max_elems, uint elem_sz)
    :  max_elements(max_elems), elem_size(elem_sz), array(0) {}

  ~Refpos_container_sorted_array()
  {
    delete array;
    array= 0;
  }

  bool alloc()
  {
    array= new Dynamic_array<char> (PSI_INSTRUMENT_MEM,
                                    elem_size * max_elements,
                                    elem_size * max_elements/sizeof(char) + 1);
    return array == NULL;
  }

  bool add(char *elem)
  {
    for (uint i= 0; i < elem_size; i++)
    {
      if (array->append(elem[i]))
	return true;
    }
    return false;
  }

  char *get_pos(uint n)
  {
    return array->get_pos(n * elem_size);
  }

  uint elements() { return (uint) (array->elements() / elem_size); }

  void sort (int (*cmp) (void *ctxt, const void *el1, const void *el2),
                         void *cmp_arg)
  {
    my_qsort2(array->front(), array->elements()/elem_size,
              elem_size, (qsort2_cmp) cmp, cmp_arg);
  }
};


/**
  @class Rowid_filter_sorted_array

  The implementation of the Rowid_filter_container interface as
  a sorted array container of rowids / primary keys
*/

class Rowid_filter_sorted_array: public Rowid_filter_container
{
  /* The dynamic array to store rowids / primary keys */
  Refpos_container_sorted_array refpos_container;
  /* Initially false, becomes true after the first call of (check() */
  bool is_checked;

public:
  Rowid_filter_sorted_array(uint elems, uint elem_size)
    : refpos_container(elems, elem_size), is_checked(false) {}

  Rowid_filter_container_type get_type()
  { return SORTED_ARRAY_CONTAINER; }

  bool alloc() { return refpos_container.alloc(); }

  bool add(void *ctxt, char *elem) { return refpos_container.add(elem); }

  bool check(void *ctxt, char *elem);
};

/**
  @class Range_rowid_filter_cost_info

  An objects of this class is created for each potentially usable
  range filter. It contains the info that allows to figure out
  whether usage of the range filter promises some gain.
*/

class Range_rowid_filter_cost_info : public Sql_alloc
{
  /* The table for which the range filter is to be built (if needed) */
  TABLE *table;
  /* Estimated number of elements in the filter */
  ulonglong est_elements;
  /* The cost of building the range filter */
  double b;
  /*
     a*N-b yields the gain of the filter
     for N key tuples of the index key_no
  */
  double a;
  /* The value of N where the gain is  0 */
  double cross_x;
  /* Used for pruning of the potential range filters */
  key_map abs_independent;

  /*
    These two parameters are used to choose the best range filter
    in the function TABLE::best_range_rowid_filter_for_partial_join
  */
  double a_adj;
  double cross_x_adj;

public:
  /* The type of the container of the range filter */
  Rowid_filter_container_type container_type;
  /* The index whose range scan would be used to build the range filter */
  uint key_no;
  /* The selectivity of the range filter */
  double selectivity;

  Range_rowid_filter_cost_info() : table(0), key_no(0) {}

  void init(Rowid_filter_container_type cont_type,
            TABLE *tab, uint key_no);

  double build_cost(Rowid_filter_container_type container_type);

  inline double lookup_cost(Rowid_filter_container_type cont_type);

  inline double
  avg_access_and_eval_gain_per_row(Rowid_filter_container_type cont_type);

  inline double avg_adjusted_gain_per_row(double access_cost_factor);

  inline void set_adjusted_gain_param(double access_cost_factor);

  /* Get the gain that usage of filter promises for r key tuples */
  inline double get_gain(double r)
  {
    return r * a - b;
  }

  /* Get the adjusted gain that usage of filter promises for r key tuples */
  inline double get_adjusted_gain(double r)
  {
    return r * a_adj - b;
  }

  /*
    The gain promised by usage of the filter for r key tuples
    due to less condition evaluations
  */
  inline double get_cmp_gain(double r)
  {
    return r * (1 - selectivity) / TIME_FOR_COMPARE;
  }

  Rowid_filter_container *create_container();

  double get_a() { return a; }

  void trace_info(THD *thd);

  friend
  void TABLE::prune_range_rowid_filters();

  friend
  void TABLE::init_cost_info_for_usable_range_rowid_filters(THD *thd);

  friend
  Range_rowid_filter_cost_info *
  TABLE::best_range_rowid_filter_for_partial_join(uint access_key_no,
                                                  double records,
                                                  double access_cost_factor);
};

#endif /* ROWID_FILTER_INCLUDED */
