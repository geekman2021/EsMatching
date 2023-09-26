�˂1-Q���!��m�0U�0� 0U��0U%0
+0y+m0k0$+0�http://ocsp.digicert.com0C+0�7http://cacerts.digicert.com/DigiCertAssuredIDRootCA.crt0��Uz0x0:�8�6�4http://crl4.digicert.com/DigiCertAssuredIDRootCA.crl0:�8�6�4http://crl3.digicert.com/DigiCertAssuredIDRootCA.crl0PU I0G08
`�H��l 0*0(+https://www.digicert.com/CPS0	`�H��l0	*�H�� � q��Q�Vi����|�cz�x�7@��K�����ŧ� B,�w�+!Z*�4j���'�f�	�`�v�:�����|b�^�%ĺ{,!��[,5Ch��u�rǘ�<d�H_�8aMbF$�c��R,�)��x����Gp�uՓ��~��g�Sa���k���Xi|���(u��c�"�a�y�~Y���o�&�l��Q!�X`��-+m��~��%+G|I���|BB(�V�A4�f]�t{�V3"/"6�K�J�����R1��0��0��0r10	UUS10U
DigiCert Inc10Uwww.digicert.com110/U(DigiCert SHA2 Assured ID Timestamping CABJ�:��`@!� ��0	`�He ���0	*�H��	1*�H��	0	*�H��	1210907094141Z0+*�H��	1000�ׂ�ᑾ�k�������$��0/	*�H��	1" �hD|]�����x�&� ZS�v, o�J����N07*�H��	/1(0&0$0" ���
ك�W)�DA\��)+aO&�L�5q���0	*�H�� � O��*SL��7N��}��zT69��Pk����h�vyi��� �A�.ʆXTB-D��~��������i�hƏa��^��&V?�#C��3ݍD7J�)%i��N�u�&���^)B~Uy��9xm���K>mrw]k�Z%-+?��  ���F�����4��@�Ó����Y?�����j��Y��4�����zZ�k�� ��3��!���+�)r��9��]`c�L�f�O��>F��~�]in                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         nfo shape) { b_states[shape]= 1; }
  void clear_b_state(gcalc_shape_info shape) { b_states[shape]= 0; }
  int get_state(gcalc_shape_info shape)
    { return i_states[shape] | b_states[shape]; }
  int get_i_state(gcalc_shape_info shape) { return i_states[shape]; }
  int get_b_state(gcalc_shape_info shape) { return b_states[shape]; }
  int count()
    { return count_internal(function_buffer.ptr(), 0, 0); }
  int count_last()
    { return count_internal(function_buffer.ptr(), 1, 0); }
  void clear_i_states();
  void clear_b_states();
  void reset();

  int check_function(Gcalc_scan_iterator &scan_it);
};


/*
  Gcalc_operation_transporter class extends the Gcalc_shape_transporter.
  In addition to the parent's functionality, it fills the Gcalc_function
  object so it has the function that determines the proper shape.
  For example Multipolyline will be represented as an union of polylines.
*/

class Gcalc_operation_transporter : public Gcalc_shape_transporter
{
protected:
  Gcalc_function *m_fn;
  gcalc_shape_info m_si;
public:
  Gcalc_operation_transporter(Gcalc_function *fn, Gcalc_heap *heap) :
    Gcalc_shape_transporter(heap), m_fn(fn) {}

  int single_point(double x, double y);
  int start_line();
  int complete_line();
  int start_poly();
  int complete_poly();
  int start_ring();
  int complete_ring();
  int add_point(double x, double y);
  int start_collection(int n_objects);
  int empty_shape();
};


/*
   When we calculate the result of an spatial operation like
   Union or Intersection, we receive vertexes of the result
   one-by-one, and probably need to treat them in variative ways.
   So, the Gcalc_result_receiver class designed to get these
   vertexes and construct shapes/objects out of them.
   and to store the result in an appropriate format
*/

class Gcalc_result_receiver
{
  String buffer;
  uint32 n_points;
  Gcalc_function::shape_type common_shapetype;
  bool collection_result;
  uint32 n_shapes;
  uint32 n_holes;

  Gcalc_function::shape_type cur_shape;
  uint32 shape_pos;
  double first_x, first_y, prev_x, prev_y;
  double shape_area;
public:
Gcalc_result_receiver() :
    n_points(0),
    common_shapetype(Gcalc_function::shape_point),
    collection_result(FALSE), n_shapes(0), n_holes(0),
    cur_shape(Gcalc_function::shape_point), shape_pos(0)
    {}
  int start_shape(Gcalc_function::shape_type shape);
  int add_point(double x, double y);
  int complete_shape();
  int single_point(double x, double y);
  int done();
  void reset();

  const char *result() { return buffer.ptr(); }
  uint length() { return buffer.length(); }
  int get_nshapes() { return n_shapes; }
  int get_nholes() { return n_holes; }
  int get_result_typeid();
  uint32 position() { return buffer.length(); }
  int move_hole(uint32 dest_position, uint32 source_position,
                uint32 *position_shift);
};


/*
  Gcalc_operation_reducer class incapsulates the spatial
  operation functionality. It analyses the slices generated by
  the slicescan and calculates the shape of the result defined
  by some Gcalc_function.
*/

class Gcalc_operation_reducer : public Gcalc_dyn_list
{
public:
  enum modes
  {
    /* Numeric values important here - careful with changing */
    default_mode= 0,
    prefer_big_with_holes= 1,
    polygon_selfintersections_allowed= 2,  /* allowed in the result */
    line_selfintersections_allowed= 4      /* allowed in the result */
  };

  Gcalc_operation_reducer(size_t blk_size=8192);
  Gcalc_operation_reducer(const Gcalc_operation_reducer &gor);
  void init(Gcalc_function *fn, modes mode= default_mode);
  Gcalc_operation_reducer(Gcalc_function *fn, modes mode= default_mode,
		       size_t blk_size=8192);
  GCALC_DECL_TERMINATED_STATE(killed)
  int count_slice(Gcalc_scan_iterator *si);
  int count_all(Gcalc_heap *hp);
  int get_result(Gcalc_result_receiver *storage);
  void reset();

#ifndef GCALC_DBUG_OFF
  int n_res_points;
#endif /*GCALC_DBUG_OFF*/
  class res_point : public Gcalc_dyn_list::Item
  {
  public:
    int intersection_point;
    union
    {
      const Gcalc_heap::Info *pi;
      res_point *first_poly_node;
    };
    union
    {
      res_point *outer_poly;
      uint32 poly_position;
    };
    res_point *up;
    res_point *down;
    res_point *glue;
    Gcalc_function::shape_type type;
    Gcalc_dyn_list::Item **prev_hook;
#ifndef GCALC_DBUG_OFF
    int point_n;
#endif /*GCALC_DBUG_OFF*/
    void set(const Gcalc_scan_iterator *si);
    res_point *get_next() { return (res_point *)next; }
  };

  class active_thread : public Gcalc_dyn_list::Item
  {
  public:
    res_point *rp;
    res_point *thread_start;

    const Gcalc_heap::Info *p1, *p2;
    res_point *enabled() { return rp; }
    active_thread *get_next() { return (active_thread *)next; }
  };

  class poly_instance : public Gcalc_dyn_list::Item
  {
  public:
    uint32 *after_poly_position;
    poly_instance *get_next() { return (poly_instance *)next; }
  };

  class line : public Gcalc_dyn_list::Item
  {
  public:
    active_thread *t;
    int incoming;
    const Gcalc_scan_iterator::point *p;
    line *get_next() { return (line *)next; }
  };

  class poly_border : public Gcalc_dyn_list::Item
  {
  public:
    active_thread *t;
    int incoming;
    int prev_state;
    const Gcalc_scan_iterator::point *p;
    poly_border *get_next() { return (poly_border *)next; }
  };

  line *m_lines;
  Gcalc_dyn_list::Item **m_lines_hook;
  poly_border *m_poly_borders;
  Gcalc_dyn_list::Item **m_poly_borders_hook;
  line *new_line() { return (line *) new_item(); }
  poly_border *new_poly_border() { return (poly_border *) new_item(); }
  int add_line(int incoming, active_thread *t,
               const Gcalc_scan_iterator::point *p);
  int add_poly_border(int incoming, active_thread *t, int prev_state,
                      const Gcalc_scan_iterator::point *p);

protected:
  Gcalc_function *m_fn;
  Gcalc_dyn_list::Item **m_res_hook;
  res_point *m_result;
  int m_mode;

  res_point *result_heap;
  active_thread *m_first_active_thread;

  res_point *add_res_point(Gcalc_function::shape_type type);
  active_thread *new_active_thread() { return (active_thread *)new_item(); }

  poly_instance *new_poly() { return (poly_instance *) new_item(); }

private:
  int start_line(active_thread *t, const Gcalc_scan_iterator::point *p,
                 const Gcalc_scan_iterator *si);
  int end_line(active_thread *t, const Gcalc_scan_iterator *si);
  int connect_threads(int incoming_a, int incoming_b,
                      active_thread *ta, active_thread *tb,
                      const Gcalc_scan_iterator::point *pa,
                      const Gcalc_scan_iterator::point *pb,
                      active_thread *prev_range,
                      const Gcalc_scan_iterator *si,
                      Gcalc_function::shape_type s_t);
  int add_single_point(const Gcalc_scan_iterator *si);
  poly_border *get_pair_border(poly_border *b1);
  int continue_range(active_thread *t, const Gcalc_heap::Info *p,
                     const Gcalc_heap::Info *p_next);
  int continue_i_range(active_thread *t,
                       const Gcalc_heap::Info *ii);
  int end_couple(active_thread *t0, active_thread *t1, const Gcalc_heap::Info *p);
  int get_single_result(res_point *res, Gcalc_result_receiver *storage);
  int get_result_thread(res_point *cur, Gcalc_result_receiver *storage,
			int move_upward, res_point *first_poly_node);
  int get_polygon_result(res_point *cur, Gcalc_result_receiver *storage,
                         res_point *first_poly_node);
  int get_line_result(res_point *cur, Gcalc_result_receiver *storage);

  void free_result(res_point *res);
};

#endif /*GCALC_TOOLS_INCLUDED*/

