�PNG

   IHDR         �w=�  �IDATx�����G�ݝd����{�g۶m�����s�ٶ��^{���IҼ�����T����Nz��n|��_���)$�"���u0e|ϗ��#6���o~�[w�K0!> �s��	ȤӦ�yB�w��ĥ���~!��C�Ҏe�N�n�¯G�F]ԧv�U` ��7p���O�​�t��04>��\-��Ybf�����T4����	:55U:}��?����B���&+����;B�Rp��9�q�8>�z|t"r�nv3��(�����Ǔ�c������U�g����������>�+��Rꪆѭ2[G������9��G=ƿ���^y圊
c L��\�Gʿ�s�K��?�����&�2�]�qtҘ���Ý+[�w��>
��G����X�h̲,�M}ƥ>�"���O�.�[��.923��mU�H��tݬꓫ�����0|�#@����g�QHp&�2�\z\�e�h?Tʰ��*
�a�a�`,n�p"����ۻ���_� �-��8���x��5��+ʹ����ׁ��`���h�R A�@J�&䊶G׸�2��ym�`�����2��#�\��
 �O�F̂��
���4t)�S�{��m�9�NA� @�`�4�H�mԴ�b�+���Ҁ�
�D�ױ��=��\I�v��^�ˆ�2V�9���n�u~����Z[��7ބ��Y��>��G�p���P��N�5������F�+N�k��i�X>R�h��;%8x�{�{����✹1=��Ҟ��LH=����D��ˤ��J��7�����6��Vy��\]��t~�mO~��� n�,//C4v"�4�w�Mƙ�T�1!�A�0�>���Y*!6�
G���g�&�����3����I�-����W�j���y,�4i'kq�/L����b9U��</+���(	=�i���$<��d\�yɏ��5_�kKh��O�R��F�k���߲�^u�v*9��]w*�盥�ǋ+G����ziw~����@oo�{F�)�`������<W��R���_���4�|>_*���Ǘ����x�3��^�:nkfn.�6����_OύW��tHf:����
#,U�n����]7!pO����Z�v��j��;���ׅ�C��F�����p�7nA_H:j�M<2�����+Ju���QZ��j�Z�v��r�io�R^�0-h*r�HDB0���qC'�Čs�s��_oڶ{�����Vܮܷ#!��d�
���{� ǌDXG�$����+7[<v���M�u7<rT[ ]wdU�lw��zS ƃP��z�X���o�t����G��H����u�lk���屗N`�Z]�P�:���V��gx��"�Geq�}�U?��c����F>�k/--�9�% P;hxt,E ��ժ�z�
!
J�K ����o��_u+�:��,�9    IEND�B`�                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               in the scope of the element (including
    subqueries and specifications of other with elements).
  */ 
  uint references;

  /*
    true <=> this With_element is referred in the query in which the
    element is defined
  */
  bool referenced;

  /*
    true <=> this With_element is needed for the execution of the query
    in which the element is defined
  */
  bool is_used_in_query;

  /* 
    Unparsed specification of the query that specifies this element.
    It's used to build clones of the specification if they are needed.
  */
  LEX_CSTRING unparsed_spec;
  /* Offset of the specification in the input string */
  my_ptrdiff_t unparsed_spec_offset;

  /* True if the with element is used a prepared statement */
  bool stmt_prepare_mode;

  /* Return the map where 1 is set only in the position for this element */
  table_map get_elem_map() { return (table_map) 1 << number; }
 
public:
  /*
    Contains the name of the defined With element and the position of
    the subchain of the tables references used by its definition in the
    global chain of TABLE_LIST objects created for the whole query.
  */
  With_element_head *head;

  /*
    Optional list of column names to name the columns of the table introduced
    by this with element. It is used in the case when the names are not
    inherited from the query that specified the table. Otherwise the list is
    always empty.
  */
  List <Lex_ident_sys> column_list;
  List <Lex_ident_sys> *cycle_list;
  /* The query that specifies the table introduced by this with element */
  st_select_lex_unit *spec;
  /* 
    Set to true is recursion is used (directly or indirectly)
    for the definition of this element
  */
  bool is_recursive;
  /*
    For a simple recursive CTE: the number of references to the CTE from
    outside of the CTE specification.
    For a CTE mutually recursive with other CTEs : the total number of
    references to all these CTEs outside of their specification.
    Each of these mutually recursive CTEs has the same value in this field.
  */
  uint rec_outer_references;
  /*
    Any non-recursive select in the specification of a recursive
    with element is a called anchor. In the case mutually recursive
    elements the specification of some them may be without any anchor.
    Yet at least one of them must contain an anchor.
    All anchors of any recursivespecification are moved ahead before
    the prepare stage.
  */  
  /* Set to true if this is a recursive element with an anchor */ 
  bool with_anchor;
  /* 
    Set to the first recursive select of the unit specifying the element
    after all anchor have been moved to the head of the unit.
  */
  st_select_lex *first_recursive;
  
  /* 
    The number of the last performed iteration for recursive table
    (the number of the initial non-recursive step is 0, the number
     of the first iteration is 1).
  */
  uint level;

  /* 
    The pointer to the object used to materialize this with element
    if it's recursive. This object is built at the end of prepare 
    stage and is used at the execution stage.
  */
  select_union_recursive *rec_result;

  /* List of Item_subselects containing recursive references to this CTE */
  SQL_I_List<Item_subselect> sq_with_rec_ref;
  /* List of derived tables containing recursive references to this CTE */
  SQL_I_List<TABLE_LIST> derived_with_rec_ref;

  With_element(With_element_head *h,
               List <Lex_ident_sys> list,
               st_select_lex_unit *unit)
    : next(NULL), base_dep_map(0), derived_dep_map(0),
      sq_dep_map(0), work_dep_map(0), mutually_recursive(0),
      top_level_dep_map(0), sq_rec_ref(NULL),
      next_mutually_recursive(NULL), references(0), 
      referenced(false), is_used_in_query(false),
      head(h), column_list(list), cycle_list(0), spec(unit),
      is_recursive(false), rec_outer_references(0), with_anchor(false),
      level(0), rec_result(NULL)
  { unit->with_element= this; }

  LEX_CSTRING *get_name() { return head->query_name; }
  const char *get_name_str() { return get_name()->str; }

  void set_tables_start_pos(TABLE_LIST **pos)
  { head->tables_pos.set_start_pos(pos); }
  void set_tables_end_pos(TABLE_LIST **pos)
  { head->tables_pos.set_end_pos(pos); }

  bool check_dependencies_in_spec();
  
  void check_dependencies_in_select(st_select_lex *sl, st_unit_ctxt_elem *ctxt,
                                    bool in_subq, table_map *dep_map);
      
  void check_dependencies_in_unit(st_select_lex_unit *unit,
                                  st_unit_ctxt_elem *ctxt,
                                  bool in_subq,
                                  table_map *dep_map);

  void check_dependencies_in_with_clause(With_clause *with_clause, 
                                         st_unit_ctxt_elem *ctxt,
                                         bool in_subq,
                                         table_map *dep_map);

  void  set_dependency_on(With_element *with_elem)
  { base_dep_map|= with_elem->get_elem_map(); }

  bool check_dependency_on(With_element *with_elem)
  { return base_dep_map & with_elem->get_elem_map(); }

  TABLE_LIST *find_first_sq_rec_ref_in_select(st_select_lex *sel);

  bool set_unparsed_spec(THD *thd, const char *spec_start, const char *spec_end,
                         my_ptrdiff_t spec_offset);

  st_select_lex_unit *clone_parsed_spec(LEX *old_lex, TABLE_LIST *with_table);

  bool is_referenced() { return referenced; }

  bool is_hanging_recursive() { return is_recursive && !rec_outer_references; }

  void inc_references() { references++; }

  bool process_columns_of_derived_unit(THD *thd, st_select_lex_unit *unit);

  bool prepare_unreferenced(THD *thd);

  bool check_unrestricted_recursive(st_select_lex *sel,
                                    table_map &unrestricted,
                                    table_map &encountered);

  void print(THD *thd, String *str, enum_query_type query_type);

  With_clause *get_owner() { return owner; }

  bool contains_sq_with_recursive_reference()
  { return sq_dep_map & mutually_recursive; }

  bool no_rec_ref_on_top_level()
  { return !(top_level_dep_map & mutually_recursive); }

  table_map get_mutually_recursive() { return mutually_recursive; }

  With_element *get_next_mutually_recursive()
  { return next_mutually_recursive; }

  TABLE_LIST *get_sq_rec_ref() { return sq_rec_ref; }

  bool is_anchor(st_select_lex *sel);

  void move_anchors_ahead(); 

  bool is_unrestricted();

  bool is_with_prepared_anchor();

  void mark_as_with_prepared_anchor();

  bool is_cleaned();

  void mark_as_cleaned();

  void reset_recursive_for_exec();

  void cleanup_stabilized();

  void set_as_stabilized();

  bool is_stabilized();

  bool all_are_stabilized();

  bool instantiate_tmp_tables();

  void prepare_for_next_iteration();

  void set_cycle_list(List<Lex_ident_sys> *cycle_list_arg);

  friend class With_clause;

  friend
  bool LEX::resolve_references_to_cte(TABLE_LIST *tables,
                                      TABLE_LIST **tables_last);
  friend
  bool LEX::resolve_references_to_cte_in_hanging_cte();
};

const uint max_number_of_elements_in_with_clause= sizeof(table_map)*8;

/**
  @class With_clause
  @brief Set of with_elements

  It has a reference to the first with element from this with clause.
  This reference allows to navigate through all the elements of the with clause.
  It contains a reference to the unit to which this with clause is attached.
  It also contains a flag saying whether this with clause was specified as recursive.
*/ 

class With_clause : public Sql_alloc
{
private:
  st_select_lex_unit *owner; // the unit this with clause attached to

  /* The list of all with elements from this with clause */
  SQL_I_List<With_element> with_list; 
  /*
    The with clause immediately containing this with clause if there is any,
    otherwise NULL. Now used  only at parsing.
  */
  With_clause *embedding_with_clause;
  /*
    The next with the clause of the chain of with clauses encountered
    in the current statement
  */
  With_clause *next_with_clause;
  /* Set to true if dependencies between with elements have been checked */
  bool dependencies_are_checked;
  /* 
    The bitmap of all recursive with elements whose specifications
    are not complied with restrictions imposed by the SQL standards
    on recursive specifications.
  */ 
  table_map unrestricted;
  /* 
    The bitmap of all recursive with elements whose anchors
    has been already prepared.
  */
  table_map with_prepared_anchor;
  table_map cleaned;
  /* 
    The bitmap of all recursive with elements that
    has been already materialized
  */
  table_map stabilized;

public:
 /* If true the specifier RECURSIVE is present in the with clause */
  bool with_recursive;

  With_clause(bool recursive_fl, With_clause *emb_with_clause)
    : owner(NULL), embedding_with_clause(emb_with_clause),
      next_with_clause(NULL), dependencies_are_checked(false), unrestricted(0),
      with_prepared_anchor(0), cleaned(0), stabilized(0),
      with_recursive(recursive_fl)
  { }

  bool add_with_element(With_element *elem);

  /* Add this with clause to the list of with clauses used in the statement */
  void add_to_list(With_clause ** &last_next)
  {
    *last_next= this;
    last_next= &this->next_with_clause;
  }

  st_select_lex_unit *get_owner() { return owner; }

  void set_owner(st_select_lex_unit *unit) { owner= unit; }

  void attach_to(st_select_lex *select_lex);

  With_clause *pop() { return embedding_with_clause; }
      
  bool check_dependencies();

  bool check_anchors();

  void move_anchors_ahead();

  With_element *find_table_def(TABLE_LIST *table, With_element *barrier);

  With_element *find_table_def_in_with_clauses(TABLE_LIST *table);

  bool prepare_unreferenced_elements(THD *thd);

  void add_unrestricted(table_map map) { unrestricted|= map; }

  void print(THD *thd, String *str, enum_query_type query_type);

  friend class With_element;

  friend
  bool LEX::check_dependencies_in_with_clauses();

  friend
  bool LEX::resolve_references_to_cte_in_hanging_cte();
};

inline
bool With_element::is_unrestricted() 
{
  return owner->unrestricted & get_elem_map();
}

inline
bool With_element::is_with_prepared_anchor() 
{
  return owner->with_prepared_anchor & get_elem_map();
}

inline
void With_element::mark_as_with_prepared_anchor() 
{
  owner->with_prepared_anchor|= mutually_recursive;
}


inline
bool With_element::is_cleaned() 
{
  return owner->cleaned & get_elem_map();
}


inline
void With_element::mark_as_cleaned() 
{
  owner->cleaned|= get_elem_map();
}


inline
void With_element::reset_recursive_for_exec()
{
  DBUG_ASSERT(is_recursive);
  level= 0;
  owner->with_prepared_anchor&= ~mutually_recursive;
  owner->cleaned&= ~get_elem_map();
  cleanup_stabilized();
  spec->columns_are_renamed= false;
}



inline
void With_element::cleanup_stabilized()
{
  owner->stabilized&= ~mutually_recursive;
}


inline
void With_element::set_as_stabilized()
{
  owner->stabilized|= get_elem_map();
}


inline
bool With_element::is_stabilized()
{
  return owner->stabilized & get_elem_map();
}


inline
bool With_element::all_are_stabilized()
{
  return (owner->stabilized & mutually_recursive) == mutually_recursive;
}


inline
void With_element::prepare_for_next_iteration()
{
  With_element *with_elem= this;
  while ((with_elem= with_elem->get_next_mutually_recursive()) != this)
  {
    TABLE *rec_table= with_elem->rec_result->first_rec_table_to_update;
    if (rec_table)
      rec_table->reginfo.join_tab->preread_init_done= false;        
  }
}


inline
void With_clause::attach_to(st_select_lex *select_lex)
{
  for (With_element *with_elem= with_list.first;
       with_elem;
       with_elem= with_elem->next)
  {
    select_lex->register_unit(with_elem->spec, NULL);
  }
}


inline
void st_select_lex::set_with_clause(With_clause *with_clause)
{
  master_unit()->with_clause= with_clause;
  if (with_clause)
    with_clause->set_owner(master_unit());
}

#endif /* SQL_CTE_INCLUDED */
