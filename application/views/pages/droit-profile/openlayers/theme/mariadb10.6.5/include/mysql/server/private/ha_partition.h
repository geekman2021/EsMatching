#ifndef HA_PARTITION_INCLUDED
#define HA_PARTITION_INCLUDED

/*
   Copyright (c) 2005, 2012, Oracle and/or its affiliates.
   Copyright (c) 2009, 2021, MariaDB Corporation.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1335  USA */

#include "sql_partition.h"      /* part_id_range, partition_element */
#include "queues.h"             /* QUEUE */

struct Ordered_blob_storage
{
  String blob;
  bool set_read_value;
  Ordered_blob_storage() : set_read_value(false)
  {}
};

#define PAR_EXT ".par"
#define PARTITION_BYTES_IN_POS 2
#define ORDERED_PART_NUM_OFFSET sizeof(Ordered_blob_storage **)
#define ORDERED_REC_OFFSET (ORDERED_PART_NUM_OFFSET + PARTITION_BYTES_IN_POS)


/** Struct used for partition_name_hash */
typedef struct st_part_name_def
{
  uchar *partition_name;
  uint length;
  uint32 part_id;
  my_bool is_subpart;
} PART_NAME_DEF;

/** class where to save partitions Handler_share's */
class Parts_share_refs
{
public:
  uint num_parts;                              /**< Size of ha_share array */
  Handler_share **ha_shares;                   /**< Storage for each part */
  Parts_share_refs()
  {
    num_parts= 0;
    ha_shares= NULL;
  }
  ~Parts_share_refs()
  {
    uint i;
    for (i= 0; i < num_parts; i++)
      delete ha_shares[i];
    delete[] ha_shares;
  }
  bool init(uint arg_num_parts)
  {
    DBUG_ASSERT(!num_parts && !ha_shares);
    num_parts= arg_num_parts;
    /* Allocate an array of Handler_share pointers */
    ha_shares= new Handler_share *[num_parts];
    if (!ha_shares)
    {
      num_parts= 0;
      return true;
    }
    memset(ha_shares, 0, sizeof(Handler_share*) * num_parts);
    return false;
  }
};

class ha_partition;

/* Partition Full Text Search info */
struct st_partition_ft_info
{
  struct _ft_vft        *please;
  st_partition_ft_info  *next;
  ha_partition          *file;
  FT_INFO               **part_ft_info;
};


#ifdef HAVE_PSI_MUTEX_INTERFACE
extern PSI_mutex_key key_partition_auto_inc_mutex;
#endif

/**
  Partition specific Handler_share.
*/
class Partition_share : public Handler_share
{
public:
  bool auto_inc_initialized;
  mysql_mutex_t auto_inc_mutex;                /**< protecting auto_inc val */
  ulonglong next_auto_inc_val;                 /**< first non reserved value */
  /**
    Hash of partition names. Initialized in the first ha_partition::open()
    for the table_share. After that it is read-only, i.e. no locking required.
  */
  bool partition_name_hash_initialized;
  HASH partition_name_hash;
  const char *partition_engine_name;
  /** Storage for each partitions Handler_share */
  Parts_share_refs partitions_share_refs;
  Partition_share()
    : auto_inc_initialized(false),
    next_auto_inc_val(0),
    partition_name_hash_initialized(false),
    partition_engine_name(NULL),
    partition_names(NULL)
  {
    mysql_mutex_init(key_partition_auto_inc_mutex,
                    &auto_inc_mutex,
                    MY_MUTEX_INIT_FAST);
  }

  ~Partition_share()
  {
    mysql_mutex_destroy(&auto_inc_mutex);
    if (partition_names)
    {
      my_free(partition_names);
    }
    if (partition_name_hash_initialized)
    {
      my_hash_free(&partition_name_hash);
    }
  }
  
  bool init(uint num_parts);

  /**
    Release reserved auto increment values not used.
    @param thd             Thread.
    @param table_share     Table Share
    @param next_insert_id  Next insert id (first non used auto inc value).
    @param max_reserved    End of reserved auto inc range.
  */
  void release_auto_inc_if_possible(THD *thd, TABLE_SHARE *table_share,
                                    const ulonglong next_insert_id,
                                    const ulonglong max_reserved);

  /** lock mutex protecting auto increment value next_auto_inc_val. */
  inline void lock_auto_inc()
  {
    mysql_mutex_lock(&auto_inc_mutex);
  }
  /** unlock mutex protecting auto increment value next_auto_inc_val. */
  inline void unlock_auto_inc()
  {
    mysql_mutex_unlock(&auto_inc_mutex);
  }
  /**
    Populate partition_name_hash with partition and subpartition names
    from part_info.
    @param part_info  Partition info containing all partitions metadata.

    @return Operation status.
      @retval false Success.
      @retval true  Failure.
  */
  bool populate_partition_name_hash(partition_info *part_info);
  /** Get partition name.

  @param part_id  Partition id (for subpartitioned table only subpartition
                  names will be returned.)

  @return partition name or NULL if error.
  */
  const char *get_partition_name(size_t part_id) const;
private:
  const uchar **partition_names;
  /**
    Insert [sub]partition name into  partition_name_hash
    @param name        Partition name.
    @param part_id     Partition id.
    @param is_subpart  True if subpartition else partition.

    @return Operation status.
      @retval false Success.
      @retval true  Failure.
  */
  bool insert_partition_name_in_hash(const char *name,
                                     uint part_id,
                                     bool is_subpart);
};


/*
  List of ranges to be scanned by ha_partition's MRR implementation

  This object is
   - A KEY_MULTI_RANGE structure (the MRR range)
   - Storage for the range endpoints that the KEY_MULTI_RANGE has pointers to
   - list of such ranges (connected through the "next" pointer).
*/

typedef struct st_partition_key_multi_range
{
  /*
    Number of the range. The ranges are numbered in the order RANGE_SEQ_IF has
    emitted them, starting from 1. The numbering in used by ordered MRR scans.
  */
  uint id;
  uchar *key[2];
  /*
    Sizes of allocated memory in key[]. These may be larger then the actual
    values as this structure is reused across MRR scans
  */
  uint length[2];

  /*
    The range.
    key_multi_range.ptr is a pointer to the this PARTITION_KEY_MULTI_RANGE
    object
  */
  KEY_MULTI_RANGE key_multi_range;

  // Range id from the SQL layer
  range_id_t ptr;

  // The next element in the list of MRR ranges.
  st_partition_key_multi_range *next;
} PARTITION_KEY_MULTI_RANGE;


/*
  List of ranges to be scanned in a certain [sub]partition

  The idea is that there's a list of ranges to be scanned in the table
  (formed by PARTITION_KEY_MULTI_RANGE structures),
  and for each [sub]partition, we only need to scan a subset of that list.

     PKMR1 --> PKMR2 --> PKMR3 -->... // list of PARTITION_KEY_MULTI_RANGE
       ^                   ^
       |                   |
     PPKMR1 ----------> PPKMR2 -->... // list of PARTITION_PART_KEY_MULTI_RANGE

  This way, per-partition lists of PARTITION_PART_KEY_MULTI_RANGE have pointers
  to the elements of the global list of PARTITION_KEY_MULTI_RANGE.
*/

typedef struct st_partition_part_key_multi_range
{
  PARTITION_KEY_MULTI_RANGE *partition_key_multi_range;
  st_partition_part_key_multi_range *next;
} PARTITION_PART_KEY_MULTI_RANGE;


class ha_partition;

/*
  The structure holding information about range sequence to be used with one
  partition.
  (pointer to this is used as seq_init_param for RANGE_SEQ_IF structure when
   invoking MRR for an individual partition)
*/

typedef struct st_partition_part_key_multi_range_hld
{
  /* Owner object */
  ha_partition *partition;

  /* id of the the partition this structure is for */
  uint32 part_id;

  /* Current range we're iterating through */
  PARTITION_PART_KEY_MULTI_RANGE *partition_part_key_multi_range;
} PARTITION_PART_KEY_MULTI_RANGE_HLD;


extern "C" int cmp_key_part_id(void *key_p, uchar *ref1, uchar *ref2);
extern "C" int cmp_key_rowid_part_id(void *ptr, uchar *ref1, uchar *ref2);

class ha_partition final :public handler
{
private:
  enum partition_index_scan_type
  {
    partition_index_read= 0,
    partition_index_first= 1,
    partition_index_last= 3,
    partition_index_read_last= 4,
    partition_read_range = 5,
    partition_no_index_scan= 6,
    partition_read_multi_range = 7,
    partition_ft_read= 8
  };
  /* Data for the partition handler */
  int  m_mode;                          // Open mode
  uint m_open_test_lock;                // Open test_if_locked
  uchar *m_file_buffer;                 // Content of the .par file
  char *m_name_buffer_ptr;		// Pointer to first partition name
  MEM_ROOT m_mem_root;
  plugin_ref *m_engine_array;           // Array of types of the handlers
  handler **m_file;                     // Array of references to handler inst.
  uint m_file_tot_parts;                // Debug
  handler **m_new_file;                 // Array of references to new handlers
  handler **m_reorged_file;             // Reorganised partitions
  handler **m_added_file;               // Added parts kept for errors
  LEX_CSTRING *m_connect_string;
  partition_info *m_part_info;          // local reference to partition
  Field **m_part_field_array;           // Part field array locally to save acc
  uchar *m_ordered_rec_buffer;          // Row and key buffer for ord. idx scan
  st_partition_ft_info *ft_first;
  st_partition_ft_info *ft_current;
  /*
    Current index.
    When used in key_rec_cmp: If clustered pk, index compare
    must compare pk if given index is same for two rows.
    So normally m_curr_key_info[0]= current index and m_curr_key[1]= NULL,
    and if clustered pk, [0]= current index, [1]= pk, [2]= NULL
  */
  KEY *m_curr_key_info[3];              // Current index
  uchar *m_rec0;                        // table->record[0]
  const uchar *m_err_rec;               // record which gave error
  QUEUE m_queue;                        // Prio queue used by sorted read

  /*
    Length of an element in m_ordered_rec_buffer. The elements are composed of

      [part_no] [table->record copy] [underlying_table_rowid]

    underlying_table_rowid is only stored when the table has no extended keys.
  */
  size_t m_priority_queue_rec_len;

  /*
    If true, then sorting records by key value also sorts them by their
    underlying_table_rowid.
  */
  bool m_using_extended_keys;

  /*
    Since the partition handler is a handler on top of other handlers, it
    is necessary to keep information about what the underlying handler
    characteristics is. It is not possible to keep any handler instances
    for this since the MySQL Server sometimes allocating the handler object
    without freeing them.
  */
  enum enum_handler_status
  {
    handler_not_initialized= 0,
    handler_initialized,
    handler_opened,
    handler_closed
  };
  enum_handler_status m_handler_status;

  uint m_reorged_parts;                  // Number of reorganised parts
  uint m_tot_parts;                      // Total number of partitions;
  uint m_num_locks;                       // For engines like ha_blackhole, which needs no locks
  uint m_last_part;                      // Last file that we update,write,read
  part_id_range m_part_spec;             // Which parts to scan
  uint m_scan_value;                     // Value passed in rnd_init
                                         // call
  uint m_ref_length;                     // Length of position in this
                                         // handler object
  key_range m_start_key;                 // index read key range
  enum partition_index_scan_type m_index_scan_type;// What type of index
                                                   // scan
  uint m_top_entry;                      // Which partition is to
                                         // deliver next result
  uint m_rec_length;                     // Local copy of record length

  bool m_ordered;                        // Ordered/Unordered index scan
  bool m_create_handler;                 // Handler used to create table
  bool m_is_sub_partitioned;      MZ       ÿÿ  ¸       @                                     º ´	Í!¸LÍ!This program cannot be run in DOS mode.
$       d×¹Ñ ¶×‚ ¶×‚ ¶×‚¾‚&¶×‚Ñp‚"¶×‚Ñp‚"¶×‚Ñp‚1¶×‚Ñp‚"¶×‚ÜÁn‚$¶×‚FX‚#¶×‚ ¶Ö‚À¶×‚FX‚%¶×‚FX‚!¶×‚p‚!¶×‚FX‚!¶×‚Rich ¶×‚                        PE  d† ò6\        ð "   ¸   |      dÂ        @                             u¿  `  €                                  "     d    € L   p l            Œ  `×  T                           0 p            Ð                             .text   ›·      ¸                    `.rdata  R   Ð   T   ¼              @  @.data   Œ3   0                  @  À.pdata  l   p                  @  @.rsrc   L   €     $             @  @.reloc  f        4             @  B                                                                                                                                                                                                                                                        Hÿ%Â  ÌÌÌÌÌÌÌÌÌHƒì(ÿnÂ  HƒÀ0HƒÄ(ÃÌÌÌÌÌÌÌÌÌÌÌÌÌHƒì(ÿNÂ  HƒÀ`HƒÄ(ÃÌÌÌÌÌÌÌÌÌÌÌÌÌHÿ%iÂ  ÌÌÌÌÌÌÌÌÌHÿ%¹Á  ÌÌÌÌÌÌÌÌÌHÿ%IÁ  ÌÌÌÌÌÌÌÌÌHÿ%¡Á  ÌÌÌÌÌÌÌÌÌ@SHƒì ¸ €  €úb» @  DØÿ|Á  ‹Ó‹ÈHƒÄ [Hÿ%DÁ  ÌÌÌÌƒ=Q!  „>  H,ÿÿÿÇ:!     H‰C H$ÿÿÿH‰= H6ÿÿÿH‰7 H‹(Á  H‰1 H‹BÁ  H‰+ H‹Á  H‰% H‹ÞÀ  H‰ HPÿÿÿH‰ HÿÿÿH‰ H‹lÀ  H‰ H‹ÖÀ  H‰ H‹`À  H‰ H‹rÀ  H‰û H‹äÀ  H‰õ H¾þÿÿH‰ï HÀþÿÿH‰é HÂþÿÿH‰ã H‹ÜÀ  H‰Ý H‹nÀ  H‰× H‹ø¿  H‰Ñ H‹ú¿  H‰Ë H‹¬¿  H‰Å H ÃÌÌÌÌÌÌÌÌÌÌÌÌÌH	/ ÃÌÌÌÌÌÌÌÌHƒì(H‹Q HAÇ  ÿÓÃ  3ÀHƒÄ(ÃÌÌÌÌH‹	H‹H‹PH‹IH‹R H‹I Hÿ%ó½  ÌÌÌ@SHì   E3ÀH‹ÙHL$0AP2A±ÿßÃ  H‹hÃ  LL$pHL$0E3ÀÇD$ ¨   ÿ}Â  H‹Ä  LÿÿÿHL$0E3ÉH‰\$ ÿôÃ  HUÿÿÿHL$0L‹ÃÿïÂ  HL$0ÿ´Â  HÄ   [ÃÌÌÌHƒì(H‹HrÆ  ÿÃ  3ÀHƒÄ(ÃÌÌÌÌÌL‹H‹H‹@I‹PH+Ð¶:u
HÿÀ„Éuñ3ÀÃÀƒÈÃÌÌÌÌÌÌÌH‰\$WHƒì H‹Ï- ‹ÚH‹ùH…ÀtÿÐHƒøÿw%ÿ¿  ¹ @  º   ;ÙLH0BËD‹ÁH‹Ïÿx¾  H‹\$0HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰l$H‰t$WHƒì I‹ð‹ÚH‹éH‹ù…Òu3ÀësH‹Z- H…ÀtÿÐƒøÿ`…ÛtWfH‹9- H…ÀtL‹Æ‹ÓH‹ÏÿÐHƒøÿw%ÿv¾  A¸ @  º   A;ØLH0H‹ÏDBÃÿß½  H…Àt	Hø+Øu³ëÿÁ  H+ý‹ÇH‹\$0H‹l$8H‹t$@HƒÄ _ÃÌHƒì(ÿ¾  HH0ÿ¾  ƒøÿuÿñ½  ƒ8	tHƒÄ(Hÿ%ÁÀ  HƒÄ(ÃÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰t$WHƒì0HÎÄ  H‹úH‹ñH‰D$XH‹°Á  ÿH‹ HƒËÿH‹Ã„     HÿÀ€< u÷HL$P‰D$PD‹ÈH‹¤À  H‰|$(H‰L$ LÛ HtÄ  ¹   ÿø   …ÀtH‹¾ HWÄ  L‹ÏL‹Æÿ£À  D‹L$PHD$PH‰|$(H‰D$ H‹HÀ  L‰ H2Ä  ¹   ÿø   …ÀtH‹l HÄ  L‹ÏL‹ÆÿQÀ  H‹B HÿÃ€< u÷HD$PH‰|$(L( H‰D$ H‹ä¿  HåÃ  D‹Ë¹   ‰\$Pÿø   …ÀtH‹ø HÁÃ  L‹ÏL‹Æÿí¿  D‹L$PHD$PH‰|$(H‰D$ H‹’¿  LÃ HœÃ  ¹   ÿø   …ÀtH‹¦ HÃ  L‹ÏL‹Æÿ›¿  HD$PE3ÉH‰|$(H‰D$ H‹B¿  LD$XH^Ã  AIÇD$P    ÿø   …ÀtH‹T$XH<Ã  L‹ÏL‹ÆÿH¿  H‹\$@H‹t$HHƒÄ0_ÃÌÌÌÌÌÌÌÌ@SHƒì H‹ÙÿÉ»  HºÂ  HH`L‹ÃHƒÄ [Hÿ%W»  ÌÌÌÌÌÌÌH‰\$WHƒì H‹Ùÿ•»  HH0ÿ{»  H‹=L¿  H‹HcH‹TÐøH‹JxH…Étÿ’»  HcH‹H‹LÈøHÇAx    3ÀH‹\$0HƒÄ _ÃÌÌ3ÀÃÌÌÌÌÌÌÌÌÌÌÌÌÌ¸   ÃÌÌÌÌÌÌÌÌÌÌÂ  ÌÌÌÌÌÌÌÌÌÌÌÌÌHƒì(E3À3Òÿ©¾  3Éƒøÿ•ÁAÿHƒÄ(ÃÌH‰\$WHƒì`3ÿH‹ÙH2Â  W‰|$P@ˆ|$Uÿ½  ÇD$0   LL$@DGHÂ  H‹ËH‰|$(H‰D$@ÇD$H   ÆD$TÇD$    ÿ0½  WHòÁ  ‰|$P@ˆ|$Uÿ¯¼  ÇD$0   LL$@DGHÏÁ  H‹ËH‰|$(H‰D$@ÇD$H   ÆD$TÇD$    ÿ×¼  H‹\$pHƒÄ`_ÃÌÌÌÌH‰\$H‰t$H‰|$UATAUAVAWHl$ÉHìÀ   H‹Ù¹    ÿ½  E3ÿAO L‹ðL‰xÇ@   Dˆxÿç¼  AO L‰xÇ@   DˆxH‹ðÿË¼  H˜É  H•É  E3ÉE3ÀH‰\$(L‰xÇ@   DˆxH‹øL‰|$ ÿ-½  HrÉ  HoÉ  E3ÉE3ÀH‰\$(L‹àL‰|$ ÿ½  HKÉ  HXÉ  E3ÉE3ÀH‰\$(L‹èL‰|$ ÿß¼  L‹øM…ä„v  M…í„m  H…À„d  A‹L$pAÆFº   A‰A‹MpÆF‰‹@pÆG‰I‹HÉ  H‰E×I‹FL‰%›' H‰EßI‹FÇEï   H‰Eçÿúº  HM×H‹ÓHÇEÿ   H‰E÷ÿñº  H‹H¿È  H‰E§H‹Fº   H‰E¯H‹FÇE¿   H‰E·ÿ±º  HM§H‹ÓHÇEÏ   H‰EÇÿ¨º  H‹H~È  H‰EH‹Gº   H‰EH‹GÇE   H‰Eÿhº  HMH‹ÓHÇE/   H‰E'ÿ_º  I÷FüÿÿÿtH‹ÓI‹Îÿ1º  I‹ÎÿÈº  H÷FüÿÿÿtH‹ÓH‹Îÿº  H‹Îÿ©º  H÷GüÿÿÿtH‹ÓH‹Ïÿó¹  H‹ÏÿŠº  éŸ   I÷FüÿÿÿtH‹ÓI‹ÎÿÏ¹  I‹Îÿfº  H÷FüÿÿÿtH‹ÓH‹Îÿ°¹  H‹ÎÿGº  H÷GüÿÿÿtH‹ÓH‹Ïÿ‘¹  H‹Ïÿ(º  M…ätL‹Ãº   I‹Ìÿ¢¹  M…ítL‹Ãº   I‹ÍÿŒ¹  M…ÿtL‹Ãº   I‹Ïÿv¹  Lœ$À   I‹[0I‹s8I‹{@I‹ãA_A^A]A\]ÃÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰t$WHƒì H‹úI‹ðH‹ÙAÇ    Ç   HÇA    ÆAp H”Æ  M‹ÁH‹Ïÿ¹  H‰CH…Àu#HÇ  H‹×ÿº  ƒÈÿH‹\$0H‹t$8HƒÄ _ÃH‹ÈH‰{ÿK¶  ƒø#unH‹Kÿ<¶  ƒø!u_ƒøtƒøÿtH‹Kÿ#¶  ƒø
uçƒøu)H‹Kÿ¶  ƒø
tH‹Kÿµ  H‹KE3ÀPÿÿ`µ  Ç   3ÀH‹\$0H‹t$8HƒÄ _ÃH‹Kÿþµ  H‹\$0H‹t$83ÀHƒÄ _ÃÌÌÌÌH‰\$ UVWATAUAVAWH¬$ üÿÿHìà  H‹* H3ÄH‰…Ð  E3ÿÇD$X   ‰ML‰|$hL‰}èH‰UHEèL‰EÇD$T   H‰E(H‹z¸  L‰}€HcI‹ D‰|$xH‹TÈøHÀ  D‰|$PH‹‚è  L‰}8ÇEð   H‰E@H…À  L‰}øH‰‚è  H‹ÔL‰}ÐL‰} L‰}L‰} L‰}àD‰}ˆD‰|$tL‰|$`D‰}Àè’©  H‹}…À…E  H‹i·  ‹]L‹mHcH‹ÇD$0   H‹LÈøH‹Ð·  LL$hDˆ¹Y  HcH‹H‹LÈøHD$XL=É  L‰y I‹Õ‹ËD‰|$(H‰}0‰\$|H‰D$ ÿÔ¶  ƒøÿtU€    ƒøi„â  ƒøm„'  ƒøv„Ë   HD$XÇD$0   LL$hLÜÈ  I‹Õ‹ËD‰|$(H‰D$ ÿ¶  ƒøÿu²H‹ë·  ÇD$0   LL$hHcH‹L¡È  H‹TÈø‹Eð‹ËƒŠL  ‰D$XH‹E8H‰D$hHD$XI‹ÕD‰|$(H‰D$ ÿ#¶  ‹\$TL‹e€H‹u L‹t$`L3áÿÿƒøÿ„b  L‹MøL‹E ‹Uˆ‹|$|L‰MØL‰EÈ‰T$péN  ÿ¶  L‹í LÄ  H‰D$0Hnº  HÄ  H‰D$(HïÃ  HÄ  H‰D$ ÿÝ¶  H‹Ïÿ´·  é±  H‹Ïÿ·  ƒøÿ„  ÇD$x   HÄ  ÿ¨¶  H‹ÏèøòÿÿHÄ  ÿ“¶  H‹Ü¶  HÐ   ÿ/¶  H€óÿÿHÐ   L‹ÇL‰½ð   ÿI·  HBóÿÿHÐ   L‹ÇÿB·  HÐ   ÿý¶  HÊÃ  ÿ0¶  H‹Ïÿ¿µ  D‰|$Péÿ  H‹ÏÿT¶  ƒøÿ„]  ÇD$x   H‹×HƒÉÿÿ&¶  H‹Ïÿ…µ  D‰|$PéÅ  fff„     ‹T$pƒÀöƒøp‡˜  H˜A¶„¼.  A‹Œ‚p.  IÊÿá…Ò…x  ƒûtL5<Â  L‰t$`éb  ¸   ‰D$p‰EˆéQ  ƒûuL9}ÐuH…öt'L5Ã  L‰t$`é0  ƒûtL5ôÁ  L‰t$`é  H‹t$h»   H‰u é  CúƒøvŒH…ötL5ãÂ  L‰t$`éé  H‹t$hH‰u éÛ  ƒû…Ò  »   éÄ  ƒûuM…ÉuH…öt"L5ÂÂ  L‰t$`é¨  ƒû…,ÿÿÿ…Ò…$ÿÿÿH‹D$h»   H‰EØH‰Eøé|  ƒûu L9}Ð…0ÿÿÿH…öt'L5-Â  L‰t$`é[  ƒûtL5Á  L‰t$`éE  H‹D$h»   H‰EÐé.  ƒûuM…Àt"L5QÂ  L‰t$`é  ƒû…›þÿÿ…Ò…“þÿÿH‹D$h»   H‰EÈH‰E éë   ƒûuL9}t"L5-Â  L‰t$`éÓ   ƒû…Wþÿÿ…Ò…OþÿÿH‹D$h»   H‰Eé«   Cúƒøv
»   é™   L5Â  L‰t$`éŒ   Cúƒøv»   ëyL5Â  L‰t$`ëoH‹L$hÿL³  ëbÇEÀ   ëYL‹d$h»   L‰e€ëEL‹d$h»	   L‰e€ë5L‹d$h»
   L‰e€ë%L‹d$h»   L‰e€ëL‹d$h»   L‰e€ë»   ‰\$THD$XÇD$0   LL$hLÄ  I‹Õ‹ÏD‰|$(H‰D$ ÿÂ±  L‹EÈL‹MØLÛÜÿÿƒøÿ…ýÿÿH‹}0‹D$pëH‹EøH‰EØH‹E H‰EÈ‹Eˆ‰D$pM…öt1HƒÎÿ„     HÿÆE8<6u÷H‹ÖL‹ÇI‹Îÿ˜±  ÇD$P   éå  …ÀtH Á  ÿº®  ÿì®  HH0ÿÒ®  H‹[±  D‹t$|HcH‹H‹LÈø‹D$p‰È  HcL$XD;ñ~DH…öuDCúƒøv7I‹TÍø¶:àÀ  u¶B:ÕÀ  u¶B:ÊÀ  tI‹tÍ ÿÁH‰u ‰L$XH…öt^LD$tHMPL‹ÏH‹Öèr÷ÿÿ…À…4  H•À  L‹ÇH‹Îÿo²  H…ÀtHÀ  ÿ-¬  H‰5v L‹ÈH‰Eàë&L‹MàH‰5b ëHQÀ  H‰EXÿÿ­  L‹MàH‰EhH‹EXL‹´±  ÇEP   H‰> L‰}`Dˆ½À   H‹IcA‹ÖH‹LÈø+T$XÿÂ‰‘Œ   HcD$XH‹UXH4ÅøÿÿÿI‹DÅøH‰EèH‹IcH‹LÈøIõM…ÉIEÑH‰u(H‰Q(HcL$XH‹EXI‰DÍøHcD$XIcHÅøÿÿÿH‹H‹LÈøIÕH‰‘   H‹Ïÿ±  ƒøÿuEH‹EèH‰H‹Mhÿs¬  H|¿  L‹Çº   ÿ¦¯  H‹Ïÿ…±  H‹Ïÿd¯  ÇD$P   éq  H‹x¯  ÇD$x   HcH‹H‹TÈø‹D$t‰‚Ì  H‹EèH‰HƒÎÿD9}Àt1Aƒþ~+MuD‹l$|AÿÍI‹L‹ÆIÿÀF8<u÷3ÒèJ¡  IƒÆIÿÍuáHÒ±  L‹Çº   ÿ¯  H‹-¯  HcH‹H‹LÈøCÿDˆ¹²  ƒø‡´  HÐÙÿÿH˜‹Œ‚0/  HÊÿáH‹MX¶:…¾  u¶A:z¾  tH‹ÏèÿñÿÿD9|$ptH‹Ñ H…ÀtH‹ÏÿÐ‰D$Pé[  HMPH‹×ÿ±®  H‹ú®  HcH‹H‹LÈø‹ø  ‰D$Pé-  HMPH‹×ÿ3¯  ‰D$P‹D$PH‹UX…ÀH‹Î­  uH¾  ÿH‹}é÷  H+¾  ÿH‹}éå  HMPH‹×ÿ«¯  …À…í  H‹ÏÿJ®  éß  HMPH‹×ÿˆ¯  …À…Ê  H  ÿ3®  H  H‹×ÿ;¯  é¨  H‹ÏèñÿÿH‹MØLË½  3ÒH‰|$ DJÿ:­  ƒøÿ…^  ÇD$Pþ   éQ  H‹ÏM‹çèÉðÿÿH‹EÈH…Àt*L¡½  A¹   3ÒH‹ÈH‰|$ ÿó¬  ƒøÿuÇD$Pþ   ¹    ÿÃ­  ÇD$0   LM¨H‰E¨L‰xH‹E¨Æ@H‹E¨Hd½  D‰8H‹E¨A¸   Ç@   H‹E¨L‰|$(Dˆ`H‹l­  ÇD$    HcH‹H‹LÈøHÁh  ÿä¬  ‹D$P…À…ª  H‹}H‹í E3ÉE3À3ÒH‰|$ ÿ2®  L‹ðH…À„„  H‹Þ HÿÃD8<u÷H…ÛtfB¶D3ÿHÿË<
t<uFˆ<3H…ÛuæHÿË¹    ÿà¬  HÿÃI‹ÎH‰E˜L‰xH‹E˜Æ@H‹E˜‹Ó‰Xÿó­  H‹M˜H‰H‹E˜ÇD$0   Ç@   H‹E˜LM˜DˆxH‹†¬  HW¼  HcH‹A¸   H‹LÈøL‰|$(ÇD$    HÁh  ÿì«  H‹E¨H‹MÐIÿÄD‰ H…Ét)L¼  A¹   3ÒH‰|$ ÿG«  ƒøÿuwÇD$Pþ   ëmH‹U H…ÒtdLD$tHMPL‹Ïèòÿÿ…Àt
ÇD$P   ëEH‹a«  HcH‹H‹TÈø‹D$tHMP‰‚Ì  H‹×ÿw«  H‹À«  HcH‹H‹LÈø‹ø  ‰D$PI‹Îÿj«  ‹D$P…À„VþÿÿH‹}H‹MH…É„Ò  Lv»  éNýÿÿM‹÷ƒët(ÿËtÿËtƒûuAH‹U«  ë5H‹lª  ë,H‹«¬  ë#HR»  I‹Ìÿ9¨  H…ÀH‹W«  uH‹Ž«  L‹0¹    ÿ «  H‰E¸L‰xH‹E¸Ç@   H‹E¸DˆxH‹]¸HÿÆE8<4u÷‰sA;÷}Hùº  ¹   ÿf«  ‹SI‹Ìÿ
¬  ¹    ÆCH‰ÿÀª  L‹ÇH‰E°L‰xH‹M°I‹Öÿ¸«  H‹E°H0  Ç@   H‹E°3ÒA¸ˆ   Dˆxèœ  H‹rª  H‰|$HHcH‹L‰|$@L‹DÈøH…0  Lº  I‰€Ø  M†  HM°I‹ I‹ÖH‰…8  H‹E¸H‰D$8ÇD$0   L‰|$(ÇD$    ÿ„ª  H‹ª  HcH‹H‹LÈøH‹™0  H…ÛtyH‹Ïÿª  Lº  A¹   H‹ÓH‹ÈH‰|$(Dˆ|$ ÿÊ¨  Hº  H‹H‹Á¨  ÿH‹¨©  HcH‹H‹LÈøHÁ0  ÿH©  H‹‰©  HcH‹H‹LÈøL‰¹0  ëDH‹^©  H‹E°H‰|$HH‹L‰|$@H‰D$8ÇD$0   L¡¹  E3À3ÉL‰|$(ÇD$    ÿ©©  HM°ÿß¨  HM¸ÿÕ¨  ée  „     HÿÆE8<4u÷‹ÖI‹Ìÿ|©  H‹e©  DFLM H‹ÐH‹Øÿá©  ƒøÿu`H5¹  D  C¶<IÿÇB:D9ÿuIƒÿuë3Éÿ‹¨  H‹Ëÿr¨  éò   H‹®§  Hÿ¸  I‹ÔÿH‹ËÇD$P   ÿI¨  éÉ   H‹M H‹×ÿw©  H‹Ëÿ.¨  é®   Hâ¸  ÿ”¤  H…ÀtHÐ¸  ÿ‚¤  ëH-¬  H‹ÐH‹?§  HÀ¸  ÿH‹w¨  HØ¸  H‹H‹§  H‹ÓH…ÉHEÑHÅ¸  ÿH‹Ü§  H‹ÓH‹H‹÷¦  H…ÉHEÑHÉ¸  ÿH‹Ð¦  H‹H‹Ö¦  H…ÉHEÙHÐ¸  H‹ÓÿH‹¬§  HcH‹H‹TÈøH‹E@H‰‚è  ‹D$x…Àt3ÉÿÞ¨  H‹MàH…Étÿ—¤  ‹D$P…Àu H‹h§  HcH‹EH‹H‹DÑø‹ˆø  ‰L$P‹D$PH‹Ð  H3Ìè|  H‹œ$8  HÄà  A_A^A]A\_^]Ã"  Ÿ"  ¯"  ¿"  Ï"  ß"  ¶!  ù!  —   †"  h!  h   á   !   !  9"  \"  y"  è"   	
 >&  '  Ý-  °&  ø&  R'  Œ'  *  *  *  €,  *  /-  ÌÌÌÌÌÌÌÌÌÌÌÌH‰\$UVWATAUAVAWH¬$@ÿÿÿHìÀ  H‹j  H3ÄH‰…°   H‹ÂH‰T$XH‰U˜3ÒH‰S2 H=4 D‹â‰T$P‰T$h‰T$TH‰T$H‰T$x‹ÚH‰T$pD‹òH‰T$`‹ò‰T$@‰U„Hº  º   ‰Mˆ‰M€‰3 ‹ÊE3ÉE3ÀÇD$|   L‹ÿH‰}H‰æ ÿ¸¦  3Ò3Éÿ.¦  L‹èH‰E H‹Ø¡  Ç  €  ÿ<¢  H‹ÈÿÓ¡  º €  ‹Èÿž¡  ÿ ¢  HH0ÿ¶¡  º €  ‹Èÿ¡  ÿ¢  HH`ÿ™¡  º €  ‹Èÿd¡  ÇD$0   ‰\$(H‹T$X‹MˆHD$|LL$HL_¶  H‰D$ ÿ¤  ƒøÿ„â  ƒÀÁLqº  HÏÿÿƒø7‡S  H˜¶„˜6  ‹”x6  HÑÿâH…Ût	H‹Ëÿ¡  H‹L$HÿjŸ  ÇD$0   D‰d$(H‹ØH‰D$pépÿÿÿÇD$0   ÇE„   D‰d$(éWÿÿÿH‹L$HHƒËÿfD  HÿÃD8$u÷º=   ÿ¡  H‹øH…À„+  ¾OHÿÇÿÓ   …À…ª   ¶<'wH¹   „   H£Á‚   3I‹ÎHcÐHƒÂÿ%   H‹T$HL‹ÇHcÎL+ÂL‹ðHÈH‰D$`èI•  ‹ÇLcÃ+D$HL+ÇH‹×ðHcÆÿÆBÆ0"LD$HHcÎIÎè•  ‹D$HÇD$0   +ÇD‰d$(ÃH‹\$pðHcÆƒÆBÇ0"
  ‰t$@éiþÿÿóI‹ÎHcÖHƒÂÿ—Ÿ  H‹T$HHcûHc\$@L‹ÇL‹ðHH‰D$`è¸”  ·W´  HH‹\$pfB‰1¶D´  ÿÆÇD$0   BˆD1‰t$@D‰d$(éýýÿÿóI‹ÎHcÖHƒÂÿ+Ÿ  H‹T$HHcûHc\$@L‹ÇL‹ðHH‰D$`èL”  ‹ð³  HH‹\$pB‰1¶à³  ƒÆÇD$0   BˆD1‰t$@D‰d$(é’ýÿÿL=ÿ  ÇD$0   L‰  L‰}D‰d$(énýÿÿÇD$x   ÇD$0   D‰d$(éTýÿÿH‹D$Xº/   H‹ÿ¡ž  H…ÀtHÿÀëHø¦  H‰D$8H‰D$0H‰D$(Hò¦  L‹ÈL‹ÀH‹ÐH‰D$ ÿ®ž  éh  L%Bý  M‹üL‰eëL%2ý  HËãÿÿI‹ÏI‰‡   I‰Ÿ    AÇ‡     AÇ‡Ô      ÿŽ¢  ‹E„A‰‡Ð   H‹D$X¿   H‹ ‰|$TI‰‡È   M;ü…   M…ö„˜   HcÖI‹ÎHƒÂpÿ²  L‹ðH‰D$`FINnLcÀI‹Öÿgž  (8µ  A(=µ  AN(Aµ  AF (Eµ  AN0(Iµ  AF@(Mµ  ANPòPµ  òAF`‹Lµ  A‰Fh·Eµ  fA‰Flë_¹p   ÿ×  (À´  L‹ðH‰D$` (¾´  H(Ã´  @ (È´  H0(Í´  @@(Ò´  HP(×´  @`I‹ÏM‰·  AÿWƒøÿuD‹ç‰|$Pé×   ƒ|$x ‰|$htH‹ºŸ  HcI‹E H‹TÈø	º  I‹E H‹%   Hc
H‹LÈøHÇè      Hc
I‹E H‹TÈøHM°H‹‚è  H‰D$XHE°H‰‚è  H‹Ôèq‘  L‹m …Àu,H‹U˜‹M€M‹ÅL9euèææÿÿD‹à‰D$PëèØd  D‹à‰D$PëD‹d$PH‹£Ÿ  L‹t$`H‹\$pHcI‹E H‹TÈøH‹D$XH‰‚è  ‹|$TH…Ût	H‹Ëÿœ  M…öt	I‹Îÿœ  ƒ|$h t	I‹ÍÿÙž  …ÿtÿ¯ž  ÿéŸ  Hƒ=±+  tHÇ¤+     Ç¦,     A‹ÌÿA›  ÌT3  3  1  Q1  :3  §3  81  B3    @SHƒì@H‹ÚH‹T$xHD$0LL$8L‰»  HÇD$8    H‰D$ ÿ…  ƒøÿtNH‹L$8è†  …ÀuÇ   ÆCHƒÄ@[Ã‹Èèƒ…  H‹T$xLG»  A¸   3ÉH‰D$ ÿ4  Ç    ÆCHƒÄ@[ÃÌÌÌÌ@SHƒì0H‹ÚH‹T$hL¢  ÿ  ƒøÿt{Hƒ=€*  u¹   ë#Hx* º  ÿ˜  ‹ÀH‰l+ H…Àu1Hè÷„  H‹T$hLãº  A¸   3ÉH‰D$ ÿ¨œ  ÆC HƒÄ0[ÃH'* ‹Ð‰CÿÜž  ÆCH‰HƒÄ0[ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‹	Hÿ%–š  ÌÌÌÌÌÌD‹‹
D;Á}ƒÈÿÃ3ÀD;ÁŸÀÃÌÌÌÌÌÌÌÌÌ@USWATAUAVH‹ìHƒìxH‹¨÷  H3ÄH‰EðE…ÀE‹è¸È   DDèHƒ9 E‹áD‹òH‹ÙD‰m¸uSHÇA    HÇAN   3É¿   P‡E…É„    ÿ>™  H…À…š   ÿÏ™  Hº  HH`ÿf™  Oýÿå˜  ÌH‹yHƒÇH;yrnH‡€   H‰AH=ÿÿÿrH¬¬  ¹   ÿ  H‹SH‹HÿÂE…ät.ÿÌ˜  H…Àu,ÿa™  H¢¹  HH`ÿø˜  ¹   ÿu˜  ÌE3Àÿãš  H‰H‹KH‹ÇHTTPHƒ; H‰{uR¿   3ÉHÇC    HÇCN   WNE…ä„š   ÿW˜  H…À…”   ÿè˜  H)¹  HH`ÿ˜  ‹Ïÿÿ—  ÌHÿÇH;{rnH‡€   H‰CH=ÿÿÿrHË«  ¹   ÿ8œ  H‹SH‹HÿÂE…ät.ÿë—  H…Àu,ÿ€˜  HÁ¸  HH`ÿ˜  ¹   ÿ”—  ÌE3Àÿš  H‰H‹H‰´$¸   L‰|$pH‰{ÆD8ÿ/¸…ëQHuïÆEï A÷îD‹úAÁÿA‹ÇÁèDøE‹Ç ¸gfffHÿÎA÷èÁú‹ÂÁèÐ¶ÂÀàÉD*ÁA€À0DˆD‹Â…ÒÏHƒ; H}ïuoH+þHÇC    HƒÿNsºN   ëH‹×HÿÿÿsH—€   H‰S3ÉHÿÂE…ä„¤   ÿú–  H…À…ž   ÿ‹—  HÌ·  HH`ÿ"—  ¹   ÿŸ–  ÌH‹CH+ÆHøH;{rnH‡€   H‰CH=ÿÿÿrHdª  ¹   ÿÑš  H‹SH‹HÿÂE…ät.ÿ„–  H…Àu,ÿ—  HZ·  HH`ÿ°–  ¹   ÿ-–  ÌE3Àÿ›˜  H‰H‹KLEïH‹ÖHL+Æè|‹  Hƒ; H‰{uR¿   3ÉHÇC    HÇCN   WNE…ä„š   ÿ–  H…À…”   ÿ˜–  HÙ¶  HH`ÿ/–  ‹Ïÿ¯•  ÌHÿÇH;{rnH‡€   H‰CH=ÿÿÿrH{©  ¹   ÿè™  H‹SH‹HÿÂE…ät.ÿ›•  H…Àu,ÿ0–  Hq¶  HH`ÿÇ•  ¹   ÿD•  ÌE3Àÿ²—  H‰H‹EkÿdH‰{HuïÆEï ÆD8ÿ.E+÷@ ¸gfffHÿÎA÷îÁú‹ÂÁèÐ¶ÂÀàÉD*ñA€Æ0Dˆ6D‹ò…ÒÏHƒ; H}ïuvE3öH+þL‰sHƒÿNsAVNëH‹×HÿÿÿsH—€   H‰S3ÉHÿÂE…ät.ÿÐ”  H…Àu,ÿe•  H¦µ  HH`ÿü”  ¹   ÿy”  ÌE3Àÿç–  H‰é   H‹CH+ÆHøH;{rnH‡€   H‰CH=ÿÿÿrH-¨  ¹   ÿš˜  H‹SH‹HÿÂE…ät.ÿM”  H…Àu,ÿâ”  H#µ  HH`ÿy”  ¹   ÿö“  ÌE3Àÿd–  H‰E3öH‹KLEïH‹ÖHL+ÆèB‰  Hƒ; H‰{uN¿   3ÉL‰sHÇCN   WNE…ä„š   ÿÑ“  H…À…”   ÿb”  H£´  HH`ÿù“  ‹Ïÿy“  ÌHÿÇH;{rnH‡€   H‰CH=ÿÿÿrHE§  ¹   ÿ²—  H‹SH‹HÿÂE…ät.ÿe“  H…Àu,ÿú“  H;´  HH`ÿ‘“  ¹   ÿ“  ÌE3Àÿ|•  H‰H‹H‰{HuïE‹ÅÆEï ÆD8ÿ f¸gfffHÿÎA÷èÁú‹ÂÁèÐ¶ÂÀàÉD*ÁA€À0DˆD‹Â…ÒÏHƒ; H}ïukH+þL‰sHƒÿNsºN   ëH‹×HÿÿÿsH—€   H‰S3ÉHÿÂE…ä„¤   ÿž’  H…À…ž   ÿ/“  Hp³  HH`ÿÆ’  ¹   ÿC’  ÌH‹CH+ÆHøH;{rnH‡€   H‰CH=ÿÿÿrH¦  ¹   ÿu–  H‹SH‹HÿÂE…ät.ÿ(’  H…Àu,ÿ½’  Hþ²  HH`ÿT’  ¹   ÿÑ‘  ÌE3Àÿ?”  H‰H‹KLEïH‹ÖHL+Æè ‡  Hƒ; H‰{uN¿   3ÉL‰sHÇCN   WNE…ä„š   ÿ¯‘  H…À…”   ÿ@’  H²  HH`ÿ×‘  ‹ÏÿW‘  ÌHÿÇH;{rnH‡€   H‰CH=ÿÿÿrH#¥  ¹   ÿ•  H‹SH‹HÿÂE…ät.ÿC‘  H…Àu,ÿØ‘  H²  HH`ÿo‘  ¹   ÿì  ÌE3ÀÿZ“  H‰H‹D‰mÀL-9÷ÿÿA¹   H‰{ÆD8ÿ Hƒ; L‰uÈHó  EAHMÀL‰l$ …–   L‰sÿV  L=7»  H…ÀtH‹@ëI‹ÇHƒÎÿH‹þfHÿÇ€<8 u÷HƒÿNsºN   ëH‹×HÿÿÿsH—€   H‹H‰SHÿÂE…ä„É   ÿf  H…À…Ã   ÿ÷  H8±  HH`ÿŽ  ¹   ÿ  ÌÿÄ  L=¥º  H…ÀtH‹@ëI‹ÇHƒÎÿH‹þHÿÇ€<8 u÷H{H;{rnH‡€   H‰CH=ÿÿÿrH«£  ¹   ÿ”  H‹SH‹HÿÂE…ät.ÿË  H…Àu,ÿ`  H¡°  HH`ÿ÷  ¹   ÿt  ÌE3Àÿâ‘  H‰‹E¸A¹   H¯ñ  HMÀEAL‰uÈ‰EÀL‰l$ ÿýŽ  H…ÀtL‹pëM‹÷‹E¸A¹   Hwñ  HMÐEAHÇEØ    ‰EÐL‰l$ ÿÁŽ  H…ÀtL‹xHÿÆA€<6 uöH‹KL‹ÆI‹×HèN„  Hƒ; H‰{uS¿   3ÉHÇC    HÇCN   WME…ä„œ   ÿÙŽ  H…À…–   ÿj  H«¯  HH`ÿ  Oÿÿ€Ž  ÌHƒÇH;{rnH‡€   H‰CH=ÿÿÿrHK¢  ¹   ÿ¸’  H‹SH‹HÿÂE…ät.ÿkŽ  H…Àu,ÿ   HA¯  HH`ÿ—Ž  ¹   ÿŽ  ÌE3Àÿ‚  H‰H‹KH‹H‹´$¸   L‹|$pº
  f‰H‰{H‹MðH3Ìèòy  HƒÄxA^A]A\_[]ÃÌÌÌÌH‰\$H‰l$VWATAVAWHƒì H‹ÙHŠÐ   E‹ðHb¸  LL$XA¸   ÿe’  E3ä…À…×  L9#uM3ÉL‰cxHÇCN   POE…ö„    ÿŠ  H…À…š   ÿŽ  H\®  HH`ÿ²  Oýÿ1  ÌH‹{HƒÇH;{rnH‡€   H‰CH=ÿÿÿrHø   ¹   ÿe‘  H‹SH‹HÿÂE…öt.ÿ  H…Àu,ÿ­  Hî­  HH`ÿD  ¹   ÿÁŒ  ÌE3Àÿ/  H‰H‹KH‹A¿   ÇHostH‰{L9#uO3ÉL‰cA‹ïHÇCN   AWME…ö„   ÿ£Œ  H…À…—   ÿ4  Hu­  HH`ÿËŒ  AOÿÿIŒ  ÌHoH;krnH…€   H‰CH=ÿÿÿrH   ¹   ÿ  H‹SH‹HÿÂE…öt.ÿ4Œ  H…Àu,ÿÉŒ  H
­  HH`ÿ`Œ  ¹   ÿÝ‹  ÌE3ÀÿKŽ  H‰H‹KH‹HƒÎÿº:   H‹þf‰H‹D$XH‰kL9#utL‰cH‹HÿÇD8$9u÷HƒÿNsºN   ëH‹×HÿÿÿsH—€   H‰S3ÉHÿÂE…ö„©   ÿ“‹  H…À…£   ÿ$Œ  He¬  HH`ÿ»‹  ¹   ÿ8‹  ÌH‹HÿÇD8$9u÷HýH;{rnH‡€   H‰CH=ÿÿÿrHøž  ¹   ÿe  H‹SH‹HÿÂE…öt.ÿ‹  H…Àu,ÿ­‹  Hî«  HH`ÿD‹  ¹   ÿÁŠ  ÌE3Àÿ/  H‰H‹D$XH‹@ HÿÆD8$2u÷H‹KL‹ÆHè€  H‰{L9#uN3ÉL‰cHÇCN   ºO   E…ö„ž   ÿ•Š  H…À…˜   ÿ&‹  Hg«  HH`ÿ½Š  ¹   ÿ:Š  ÌLL;{rnI‡€   H‰CH=ÿÿÿrHž  ¹   ÿrŽ  H‹SH‹HÿÂE…öt.ÿ%Š  H…Àu,ÿºŠ  Hûª  HH`ÿQŠ  ¹   ÿÎ‰  ÌE3Àÿ<Œ  H‰H‹KH‹A¸
  fD‰L‰{L9#uO¿   3ÉL‰cHÇCN   W<E…ö„    ÿ±‰  H…À…š   ÿBŠ  Hƒª  HH`ÿÙ‰  OîÿX‰  ÌH‹{HƒÇH;{rnH‡€   H‰CH=ÿÿÿrH  ¹   ÿŒ  H‹SH‹HÿÂE…öt.ÿ?‰  H…Àu,ÿÔ‰  Hª  HH`ÿk‰  ¹   ÿèˆ  ÌE3ÀÿV‹  H‰H‹SH‹ ³  H‹l$`HH‰H‹–³  H‰B·“³  f‰B¶Š³  ˆBH‰{H‹\$PHƒÄ A_A^A\_^ÃÌÌÌÌÌÌÌH‰\$H‰t$WHƒì H‹úí  H‹úH‹ñHíí  H…Àt7„     IƒÈÿIÿÀB€<  uöL;ÇuH‹ÖH‹Èè¦}  …ÀtH‹CHƒÃH…ÀuÑ3ÀH‹\$0H‹t$8HƒÄ _ÃH‹CH‹t$8H‹\$0HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌH‰\$WHƒìPH‹œ$ˆ   H‹úL¨  H‹Óÿ£Š  ƒøÿ„Ï   H‹[Œ  HcH‹H‹LÈøH‹HÃ  H‹Ëÿ¥‰  H‹Ï‹Ðÿ’‰  HT$8H‹Ëÿô‰  LD$8HT$@H‹ËÿŠ  …Àuzfffff„     HD$8LD$0HT$HH‰D$(E3ÉH‹ËÆD$  ÿ#‰  L‹L$@D‹D$0H‹T$HM‹	H‹ÏÇD$    ÿh‰  HT$8H‹Ëÿj‰  LD$8HT$@H‹Ëÿ—‰  …Àt“H‹\$`HƒÄP_ÃÌÌÌÌÌÌÌÌ@UATAUAVAWHƒì@Hl$0H‰]@H‰uHH‰}PH‹
å  H3ÅH‰E ƒy L‹êL‹ñ†÷   H‹	º:   ÿe‡  ‹ØH‹øA+H…À„Ø   …ÛŽÐ   I‹HcËHPÿf¶
< t<	u
HÿÉÿËH…Éê…Û„¦   D{Aÿ €  AŸÄE„ätIcÏÿŠ  H‹ðë*IcÇHHH;Èw
H¹ðÿÿÿÿÿÿHƒáðH‹Áè.{  H+áHt$0I‹HcÛH‹ÎL‹Ãèo{  Æ3 ¶GHÿÇ< tõ<	tñA‹L‹ÏE‹Ç+ÇH‹ÖI‹ÍAFÇD$(   ‰D$ ÿˆ  E„ät	H‹ÎÿT‰  H‹M H3ÍèÀq  H‹]@H‹uHH‹}PHeA_A^A]A\]ÃÌÌÌÌÌÌH‰\$WHƒì H‹|$XH‹ÚL;  H‹×ÿ6ˆ  ƒøÿtcH‹ò‰  HcH‹H‹TÈøHÂ    u‰ÆCH‹\$0HƒÄ _Ã3ÒH‹Ëÿ"‡  H‹»‰  H4þÿÿHcH‹L‹ÏH‹LÈøL‹ÃHÁ    ÿ®†  H‹\$0HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌ3Àf‰ÃÌÌÌÌÌÌÌÌÌÌH‰\$WHƒì E3ÉI‹Ø‹úL×ÿÿÿHˆ AQÿÎ†  Hï   L‹Ã‹×ÿ¤†  H‹\$03ÀHƒÄ _ÃÌÌÌÌÌÌÌHƒì(‹ÊI‹Ðÿ1‡  3ÀHƒÄ(ÃÌÌÌÌÌÌÌÌÌÌHÿ%ˆ  ÌÌÌÌÌÌÌÌÌHƒì(Håå  A¸   ÿ±ˆ  3Éƒøÿ•ÁAÿHƒÄ(ÃÌÌÌÌÌÌÌÌÌH‹¹ˆ  M‹ÈL‹ÑLc I‹N‹DÀøI‹H…Éu3ÀÃD‹ÂI‹Òéb'  ÌÌH‰\$WHƒì H‹Ù3Ò3Éÿ‰ˆ  H‹øH…Ût=H‹jˆ  HcH‹H‹TÑø€ºù    u"H‹Èÿ]†  H‹H‹Cˆ  Hc	H‹LÈøÆù   H‹\$0HƒÄ _ÃÌÌÌÌÌÌÌÌÌL‹ÜUVWATAVAWHƒìHH‹	ˆ  E3ö3öLc H‹L‹úN‹TÀøL‹áI‰s¨I‹*M‰s°A‹þH‰¬$ˆ   M‰s¸H…í„S  A8rI…I  I‹‚è   I‰[M‰kÈDnH…À„I  HƒÍÿH‹ÝH…öu>HÿÃ@84u÷HƒûNs¿N   3ÉH‰|$0ë\H‹ûHûÿÿsH»€   H‰|$03Éë@D  HÿÃ@84u÷IÞI;Þr?H»€   H‰|$0HÿÿÿÿrH‹–  ¹   ÿø†  H‹ÎHWE3Àÿø„  H‰D$ H‹ðH‹	‡  HcI‹H‹LÈøH‹‘è   €    HÿÅ€<* u÷I6L‹Åèµw  H…öu3ÛI‹íHÇD$0N   3ÉVOë7HkH;ïr?H½€   H‰|$0HÿÿÿÿrHô•  ¹   ÿa†  HWH‹ÎE3Àÿa„  H‰D$ H‹ð¸
  H‰l$(H‹¬$ˆ   f‰3ëE‹‚Ø   ‹U|HL$ E3Éè:èÿÿHL$ E3ÀH‹ÕèÚóÿÿH”$   I‹Ìÿñƒ  H‹\$0L‹t$(H‹t$ L‹øH…À„  ½
  f„     A‹O…É„Ò   H…öu)E3ö‹ùHƒùNs^N3ÉëEH‹ÙHùÿÿsH™€   3Éë.I<H;ûr5HŸ€   HûÿÿÿrHþ”  ¹   ÿk…  H‹ÎHSE3Àÿkƒ  H‹ðE‹GI‹I6èRv  H…öu3ÿM‹õ^N3ÉVOë2LwL;ór5Iž€   HûÿÿÿrHœ”  ¹   ÿ	…  HSH‹ÎE3Àÿ	ƒ  H‹ðf‰,7H”$   I‹ÌÿQ‚  L‹øH…À…ÿÿÿH‹¬$ˆ   H…öu
E3öVO3Éë2MnL;ër5I€   HûÿÿÿrH&”  ¹   ÿ“„  HSH‹ÎE3Àÿ“‚  H‹ð¸
  M‹ÅH‹ÖH‹ÍfA‰6èh#  L‹l$@H‹œ$€   H…öt	H‹Îÿ…ƒ  ¸   HƒÄHA_A^A\_^]ÃÌÌÌÌÌÌÌÌÌÌHƒì(H‹U„  H‹ÑLL$0HcH‹H«  H‹LÈøA¸   H‹	HÁÐ   ÿe„  ƒøÿu3ÀHƒÄ(ÃH‹D$0H‹ HƒÄ(ÃÌÌÌÌÌÌÌÌÌÌÌÌ@WHƒì H‹óƒ  M‹ÈL‹ÑLc I‹N‹DÀøI‹8H‹‡P  H…Àt@L‹GpH‰\$0H‹ŸX  ‹ÊI IÈH;ËHBÙI‹ÊI+ØL‹Ãÿ€  H_p‹ÃH‹\$0HƒÄ _ÃHƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌÌM…ÀtH‰\$H‰t$WHƒì@I‹ÙH‹úH‹ñIƒÉÿL‰D$0IÿÁC€< uöHD$`H‰\$(LD$0H‰D$ H‹…‚  ¹   ÿø   …ÀtD‹D$`H‹T$0L‹ÎH‹ÏH‰\$ ÿ‹€  H‹\$PH‹t$XHƒÄ@_óÃÌÌÌÌÌÌÌÌÌÌ@VATAVAWHƒìHH‹´$   M‹!L‹ú‹VL‹ñ…Ò„   H‹H‰\$pH‰l$xH‰|$@ÿ0ƒ  3ÛH‹è9^v+fD  H<+‹Ë¶€ù-uÆ_ë¾Éÿœ}  ˆÿÃ;^rÛL˜¨  LE©  HL$03ÒH‰l$ ÿk  H4©  H‹Íè2s  HƒËÿ…ÀtH-©  H‹Íès  …ÀuqI‹H‰„$ˆ   H…ÀtaL‹ËIÿÁB€< uöH„$   L‰|$(L„$ˆ   H‰D$ H‹C  H‹Õ¹   ÿø   …Àt!D‹„$   H‹”$ˆ   M‹ÌH‹ÍL‰|$ ÿ@  I‹H‹|$0H‰„$ˆ   H…Àt`HÿÃ€< u÷H„$   L‰|$(L„$ˆ   H‰D$ H‹Ñ€  L‹ËH‹×¹   ÿø   …Àt!D‹„$   H‹”$ˆ   M‹ÌH‹ÏL‰|$ ÿË~  H‹ÍÿJ€  H‹L$0ÿ?€  H‹|$@H‹l$xH‹\$p3ÀHƒÄHA_A^A\^ÃÌÌÌÌÌÌÌÌÌÌH‰\$UVWAVAWHl$ÉHì    H‹¡Ú  H3ÄH‰E'H‹ã€  H‹ÚH‹ñHcH‹HƒÏÿH‹TÐøL‹2I‹H‹ˆH   H‰M×H…Ét^L‹ÏD  IÿÁB€<	 uöHEÇH‰\$(LE×H‰D$ H‹Ý  Hþƒ  ¹   ÿø   …ÀtD‹EÇH‹U×Hàƒ  L‹ÎH‰\$ ÿÚ}  I‹N º:   ÿ|  L‹øH…À„  HPHM×A¸   ÿc{  I‹V M‹ÇHMçL+ÂÆEÞ ÿK{  M+~ Iƒÿ@ƒÒ  HEçBÆD=ç H‰EÏHEçH…ÀtdHEçL‹Ï€    IÿÁB€< uöHEÇH‰\$(LEÏH‰D$ H‹  H¶¦  ¹   ÿø   …ÀtD‹EÇH‹UÏH˜¦  L‹ÎH‰\$ ÿ
}  HE×H‰EÏHE×H…À„¯   HE×L‹ÏIÿÁB€< uöHEÇH‰\$(LEÏH‰D$ H‹›~  HT¦  ¹   ÿø   …ÀtoH>¦  ëPI‹F H‰EÏH…ÀtYL‹ÏIÿÁB€< uöHEÇH‰\$(LEÏH‰D$ H‹I~  Hò¥  ¹   ÿø   …ÀtHÜ¥  D‹EÇH‹UÏL‹ÎH‰\$ ÿF|  L£‹  LØ¥  HM×3Òÿ¼{  H‹M×H‰MÏH…Ét]L‹ÏIÿÁB€<	 uöHEÇH‰\$(LEÏH‰D$ H‹É}  H²¥  ¹   ÿø   …ÀtD‹EÇH‹UÏH”¥  L‹ÎH‰\$ ÿÆ{  H‹M×ÿD}  A‹N|¸…ëQL|¥  ÷éÁú‹ÂÁèÐ‹ÂD‹Ê3ÒkÀd+È‰L$ HM×ÿ{  H‹M×H‰MÏH…ÉtcL‹ÏfD  IÿÁB€<	 uöHEÇH‰\$(LEÏH‰D$ H‹}  H&¥  ¹   ÿø   …ÀtD‹EÇH‹UÏH¥  L‹ÎH‰\$ ÿ{  H‹M×ÿ˜|  I‹H‹ˆ8   H‰MÏH…ÉtYL‹ÏIÿÁB€<	 uöHEÇH‰\$(LEÏH‰D$ H‹§|  HÀ¤  ¹   ÿø   …ÀtD‹EÇH‹UÏH¢¤  L‹ÎH‰\$ ÿ¤z  I‹L–¤  HM×D‹ˆ@   3Òÿz  H‹M×H‰MÏH…ÉtdL‹Ï€    IÿÁB€<	 uöHEÇH‰\$(LEÏH‰D$ H‹|  HF¤  ¹   ÿø   …ÀtD‹EÇH‹UÏH(¤  L‹ÎH‰\$ ÿz  H‹M×ÿ˜{  I‹†€   H‰EÏH…ÀtYL‹ÏIÿÁB€< uöHEÇH‰\$(LEÏH‰D$ H‹ª{  Hã£  ¹   ÿø   …ÀtD‹EÇH‹UÏHÅ£  L‹ÎH‰\$ ÿ§y  H‹ |  HcH‹H‹LÈøH‹AH‰EÏH…Àt^L‹ÏD  IÿÁB€< uöHEÇH‰\$(LEÏH‰D$ H‹-{  Hv£  ¹   ÿø   …ÀtD‹EÇH‹UÏHX£  L‹ÎH‰\$ ÿ*y  I‹†   H‰EÏH…Àt`L‹Ï€    IÿÁB€< uöHEÇH‰\$(LEÏH‰D$ H‹½z  H®~  ¹   ÿø   …ÀtD‹EÇH‹UÏH~  L‹ÎH‰\$ ÿºx  H‹3{  HcH‹H‹LÈøH‹A(H…ÀtgH‰EÏL‹Ï„     IÿÁB€< uöHEÇH‰\$(LEÏH‰D$ H‹=z  H>~  ¹   ÿø   …ÀtmD‹EÇH‹UÏH ~  L‹ÎH‰\$ ÿ:x  ëNM‹I‹X   H…Àt?M‹‰H   LK¢  HMÏ3ÒH‰D$ ÿšw  L‹EÏH×}  L‹ËH‹ÎèôöÿÿH‹MÏÿry  I‹†°   H…ÀtdH‰EÏL‹Ï€    IÿÁB€< uöHEÇH‰\$(LEÏH‰D$ H‹}y  HÞ¡  ¹   ÿø   …ÀtD‹EÇH‹UÏHÀ¡  L‹ÎH‰\$ ÿzw  Iƒ¾¸    „ž   I‹†°   M‹Ž   L›¡  HM×3ÒH‰D$ ÿÖv  H‹M×H‰MÏH…ÉtcL‹ÏfD  IÿÁB€<	 uöHEÇH‰\$(LEÏH‰D$ H‹Ýx  H¾|  ¹   ÿø   …ÀtD‹EÇH‹UÏH |  L‹ÎH‰\$ ÿÚv  H‹M×ÿXx  ëiI‹†   H‰EÏH…ÀtYL‹ÏIÿÁB€< uöHEÇH‰\$(LEÏH‰D$ H‹hx  HI|  ¹   ÿø   …ÀtD‹EÇH‹UÏH+|  L‹ÎH‰\$ ÿev  I‹†À   H…ÀtaH‰EÏD  HÿÇ€<8 u÷HEÇH‰\$(LEÏH‰D$ H‹þw  Hw   L‹Ï¹   ÿø   …ÀtD‹EÇH‹UÏHV   L‹ÎH‰\$ ÿøu  IŽÐ   LzõÿÿA¹   H‹ÓH‰t$ ÿy  H‹M'H3ÌèÊ_  H‹œ$à   HÄ    A_A^_^]Ãèî`  ÌÌÌÌÌÌÌÌÌÌÌÌÌÌ@SHƒìpH‹ÃÑ  H3ÄH‰D$hH‹ÙHL$ ÿ-s  LD$ HL$0º4   ÿs  HL$0…ÀtA¸   H²Ÿ  ë4HƒÈÿHÿÀ€< u÷H…ÀtHÿÈHƒø4sQÆD0 ëA¸   H¡Ÿ  HL$0ÿþs  ÿÈs  LD$0HŒŸ  HH`L‹ËÿWs  H‹L$hH3Ìèê^  HƒÄp[Ãè`  ÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰|$3ÀIcø‹ÚL‹É÷Â   tUD‹D‹ÐE…ÛtJf„     A‹ÊI9|Ét
AÿÂE;Órîë-AKÿD;Ñs! EBA‹ÒK‹LÁE‹ÐI‰LÑA‹	ÿÉD;ÁrâAÿ	öÃthA‹‘  …Òt]I9¼Á  tÿÀ;ÂrðH‹\$H‹|$ÃJÿ;Ás4fffff„     D@‹ÐK‹„Á  I‰„Ñ  A‹‰  A‹ÀÿÉD;ÁrÙAÿ‰  H‹\$H‹|$ÃÌÌÌÌÌÌÌH‰\$UVW¸0   èÎf  H+àH‹ôÏ  H3ÄH‰„$    D‹‰  3öH‹êL‹ÑH\$ M…Ét+L  f„     I‹ ÇC   HƒÃH‰CðM@IÿÉuåE‹Š  H‹ËM…ÉtVM‚   fffffff„     I‹LT$ HD$ L;ÑsH9uƒHHƒÀH;ÁrîH;ÁuÇC   H‰HƒÃIƒÀIÿÉuÀHD$ H|$ H;Ãs6L‰´$P   AƒÎÿD  D‹G‹H‹Íè¢6  …ÀAEöHƒÇH;ûrãL‹´$P   ‹ÆH‹Œ$    H3Ìè¹\  H‹œ$`   HÄ0   _^]ÃÌÌÌÌÌÌH‹L‹ÂH…ÀtD  L‹ÀH‹ H…ÀuõH‹AH…ÀuH‰L‰AÃH‰L‰AÃÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰t$WHƒì H‹ñ¹(   I‹ØH‹úÿq  H…Àt$3ÉH‰pH‹t$8H‰X H‹\$0‰HH‰H‰xHƒÄ _ÃÿÆp  H‘  HH`ÿ]p  ¹   ÿÚo  ÌÌ@SHƒì H‹ÙHƒÁ(ÿ­p  H…Àt3ÉHP(H‰@H‰P‰HH‰H‰X HƒÄ [Ãÿlp  H­  HH`ÿp  ¹   ÿ€o  ÌÌÌÌÌÌÌÌH‰\$H‰l$H‰t$WATAUAVAWHƒì H‹E3íM‹øL‹âL‹ñA‹õH…Û„½   f„     D‹CH‹;E…Àt>AÿÈ…“   D‹CH‹SE3ÉI‹Ìÿop  Hcè…Àˆ¢   H‹CH;èt1HkH+ÅH‰Cë[D‹C H‹SE3ÉI‹Ìÿ:p  Hcè…ÀxqH‹C H;èu,D9kuH‹KH;Ëtÿ­o  H‹Ëÿ¤o  I‰>H…ÿuM‰nëHkH+ÅH‰C HõH‹ßH…ÿ…LÿÿÿI‰73ÀH‹\$PH‹l$XH‹t$`HƒÄ A_A^A]A\_ÃI‰7ÿÑo  ëØÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰l$H‰t$WHƒì H‹Ù¹(  I‹ð‹êÿ	o  H‹øH…À„ë   3ÀHW(H‰HÇG    H‰W‰GH‰D‹G ‹Íÿ}n  HcÐ…Àywÿ˜n  ‹ÿÀm  H‹ÈÿŸl  3Ò3ÉH‹Øÿjr  H“š  H‹ÐL‹ÃèX  H‹ËÿŸn  ƒ uH‹OH;ÏtÿŠn  H‹Ïÿn  ¸   H‹\$0H‹l$8H‹t$@HƒÄ _ÃH‰W H‹H‹ÏH…Àt„     H‹ÈH‹ H…ÀuõH‹CH…ÀuH‰;H‰KH‰ë´H‰8H‰H‰K3Àë¦ÿôm  H5Ž  HH`ÿ‹m  ¹   ÿm  ÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰t$WHƒì@H®t  I‹ñI‹ØH‰D$hH‰D$0H‹Kp  LcH‹@2ÿN‹DÐøD‹ÊIƒ¸À   t%A‹¸  ÿÊtƒêtƒêtƒê0túÀ   u@·H‹€   L]™  3ÒH‰D$(D‰L$ L‹I HL$Pÿ
n  Hƒ|$P „æ   H…Ût)L6™  HL$hL‹Ë3Òÿâm  Hƒ|$h u
H‹L$Pé²   @„ÿteH‹œo  L™  3ÒHcH‹L‹LÈøHL$0A‹Ð  ‰D$(I‹È  M‹‰À  H‰D$ ÿ‡m  H‹D$0H…ÀuH‹L$Pÿjo  H…ÛtUH‹L$hëHH‹D$0L‹L$hL‹D$PH¸˜  H‹ÖH‰D$ èC   H‹L$Pÿ0o  H…ÛtH‹L$hÿ o  @„ÿtH‹L$0ÿo  H‹\$XH‹t$`HƒÄ@_ÃÌÌÌÌÌÌÌÌH‰T$L‰D$L‰L$ Hƒì8L‹ÁLL$PHL$ 3ÒHÇD$     ÿOm  H‹L$ H…Ét&H‹þn  L‹€ˆ   M…ÀtH‹T$HAÿÐH‹L$ ÿ—n  HƒÄ8ÃÌÌÌÌÌÌÌÌÌÌL‰L$ D‰D$SUVATAUHƒìPL‹„$°   L‹¬$¨   L‹âL‰D$ ƒÎÿLD$0VM‹Í3í3Ûÿkl  …ÀuÆHƒÄPA]A\^][ÃH‰¼$€   H‹|$0L‰t$HL‰|$@H9„¯  L‹¼$    H…ÛtH‹ËÿRk  3ÛH‹E3À·APÿ•k  H‹ðƒøÿ„7  L‹7A·ƒøtxƒøtHcÎAÇ    ÿpk  é  ¹   ÿðj  H‹ØH…À„{  A‹A·$‰A‹F‰CA‹F‰CA‹F‰CA‹F‰CA‹F‰CA‹F‰Cÿâj  f‰CAÇ   ëG¹   ÿ’j  H‹ØH…À„—  A‹A·$‰A‹F‰CA‹F‰CA‹F‰Cÿ™j  f‰CAÇ   LcöLŒ$   ºÿÿ  A¸   I‹ÎÇ„$      ÇD$    ÿtj  E‹H‹ÓI‹Îÿuj  ƒøÿ…Æ   ÿ–j  ‹è=&'  t@=@'  t9I‹Îÿ]j  ƒÎÿHƒÇHƒ? …ŒþÿÿƒþÿtHcÎºÿÿÿÿIj  …ÀtÿOj  ‹èH…Ût	H‹Ëÿ¿i  H‹|$0H…ÿt	H‹ÏÿÄj  …í„È   …öx	HcÎÿ÷i  M…ítE3À3Ò‹Íÿýj  I‰E ƒÈÿéŸ   ÿKi  HŒ‰  HH`ÿâh  ¹   ÿ_h  ÌH‹Œ$˜   ·3í‰A9,$…QÿÿÿHcÎM‹ÇH‹Óÿfi  …À…Qÿÿÿ·ƒøt	ƒø…)ÿÿÿ·Kÿ;i  ·ÀA‰$éÿÿÿÿÑh  H‰  HH`ÿhh  ¹   ÿåg  Ì‹ÆL‹t$HH‹¼$€   L‹|$@HƒÄPA]A\^][ÃÌÌÌÌÌÌÌÌÌÌÌÌH‰\$WHƒì 3ÿH‹ÙLúÍÿÿ‰yH‰yH‰yH‰yH‰y H‰y(H‰y0H‰y8H‰y@H‰yHH‰yPHƒÁXA±3Òÿ l  H‹˜   A±E3À3Òÿl  H‰»Ø   H‰»à   H‰»è   H‰»ð   H‹\$03ÀHƒÄ _ÃÌÌ@SHƒì H‹ÙH‹IH…Étÿh  H‹KH…Étÿùg  H‹K(H…Étÿêg  H‹K8H…ÉtÿÛg  H‹KHH…ÉtÿÌg  HKXÿºj  H‹˜   ÿ­j  H‹‹Ø   H…ÉtHƒÄ [Hÿ%g  HƒÄ [ÃÌÌÌÌÌÌÌ@SUVWATAUAVHƒìpH‹ÊÄ  H3ÄH‰D$`H‹úH‹Q I‹ØH‹éIƒÀ¹   ÿi  E3íH‹ðE‹åE‹õD‰l$ H…À„v  L‹ÃH‹×H‹ÈL‰¼$È   ÿ-g  H<3L‹ÿH‰|$(L9m vH‹E€8/tÆ\HÿÇH‹UH‹M HÊH;Êv@ ¶HÿÉ<.tH;ÊwñëÇD$    L‹E H‹ÏÿÒf  H‹M HÏ €9/uÆ\H‹ÁHÿÉH;ÇwíH} H‹ßDˆ'H;þ†0  ff„     HT$0H‹Îÿ2e  …Àt@M…ätI‹Ìÿpf  Æ\H;ÞvHÿË€;\uóL‹÷H‹ËL+óA‹Öÿ^h  Dˆ+L‹àH;Þw³é¤   ¸ @  f…D$6„”   €{ÿ\L=kÇ  tÆ\HÿÃL9-\Ç  tWfI‹HƒÏÿHÿÇD8,:u÷LGH‹Ëÿûe  HT$0H‹Îÿ•d  …Àu¸ €  f…D$6uIƒÇM9/u¼ëHßM9/tD9l$ tM…ät	I‹Ìÿ­e  H‹Îÿ¤e  éÑ   L‹|$(M…ät+L‰u@M…ötK&„     €xÿ\H@ÿuÆ /IÿÎuîL‰e8H‹Mÿ`e  ‹ÓI‹ÏA+×ÿbg  H‰u(H‰EH‹ÃH+ÞI+ÇH‰]0H‰E H…ÀtI‹ÍH‹E€<\uÆ/AÿÅA‹ÍH;M ræH‹D$0H‰…ø   H‹D$8H‰…   H‹D$@H‰…  H‹D$HH‰…  H‹D$PH‰…  H‹D$XH‰…   L‹¼$È   H‹L$`H3ÌèÙO  HƒÄpA^A]A\_^][ÃÌÌÌÌÌÌÌÌÌÌH‰\$ ATAVAWHƒì L‹âL‹ùHÇ    A‹ÑI‹ÈI‹Ùÿ…f  L‹ðH…À„’  ‹ÓH‹ÈH‰t$HH‰|$Pÿe  I‹ÎHcðIö€9\uÆ/¶HÿÁ„ÀuîI‹þL;öƒ?  A€>/u3I‹Þ@ €;/uHÿÃH;ÞróI~L‹ÆH‹ÓH‹ÏL+Ãÿd  H‹ÇH+ÃHðH;þƒý   H‰l$@fD  H‹ßH;þs€;/tHÿÃH;ÞróH‹ÃH+ÇHƒøuN€?.uI€.uCI;þv$HÿÏI;þt€    HÿÏ€?/t
I;þuó€?/uHÿÇH;Þs3fD  €;/u(HÿÃH;ÞróëHƒøu5€?.u0H;Þs€;/uHÿÃH;ÞróL‹ÆH‹ÓH‹ÏL+ÃÿIc  H‹ÇH+ÃHðë:H;Þs2H‹ë€} /uHÿÅH;îròH{L‹ÆH‹ÕH‹ÏL+Åÿc  H‹ÇH+ÅHðëH‹ûH;þ‚ÿÿÿH‹l$@H‹|$PÆ I+öI‰4$H‹t$HM‰7H‹\$XHƒÄ A_A^A\ÃÌÌÌÌÌÌÌ@SHƒì0H‹YM‹ÈL‹ÂHT$XHL$@èþÿÿH‹D$@H‰ƒ   H‹D$XH‰ƒ˜   3ÀHƒÄ0[ÃÌH‰\$WHƒì H‹YH‹ÂA‹ÐH‹ÈI‹øÿpd  H‰»È   H‰ƒÀ   H‹\$03ÀHƒÄ _ÃÌÌÌÌÌH‰\$WHƒì H‹Y¶AL‹ÊA‹ÐI‹ÉI‹ø‰Cxÿ)d  H‰»ˆ   H‰ƒ€   H‹\$03ÀHƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰t$WHƒì H‹YI‹øH‹òöChtH‹KXÿÇa  ƒchþH‰sXH‹t$8H‰{`H‹\$03ÀHƒÄ _ÃÌÌÌÌÌÌÌÌÌ@VHƒì@H‹qH‹ÂA‹ÐH‹Èÿ—c  H‰D$PH…Àu¸   HƒÄ@^Ã‹V`H‹NXH‰\$XH‰|$`ÿ{e  ‹V`H‹NXH‹øÿ«d  D‹F`ÇD$0   HŽÐ   LL$PAÿÀH‹ÐHÇD$(    H‹ØÇD$    ÿ‹c  D‹F`ÇD$0   HŽ  LL$PAÿÀH‹×HÇD$(    ÇD$    ÿVc  H‹Ëÿ}c  H‹Ïÿtc  öFhH‹|$`H‹\$XtH‹NXÿ²`  ƒfhþ3ÀHƒÄ@^ÃÌÌÌÌÌÌ@SHƒì H‹YöChtH‹KXÿ†`  ƒchþ3ÀH‰CXHƒÄ [ÃÌÌÌÌÌÌH‰\$H‰t$WHƒì H‹YI‹øH‹òHƒ»P   u!H‹Iÿ3`  H…ÀtdH‰ƒP  HÇƒX      H‹“X  H‹‹P  H×ÿM_  H…ÀtYH‹‹X  L‹ÇH‹ÖHÈH‰ƒP  ÿû_  H»X  H‹\$0H‹t$83ÀHƒÄ _Ãÿ¬_  Hí  HH`ÿC_  ¹   ÿÀ^  Ìÿ‰_  HÊ  HH`ÿ _  ¹   ÿ^  ÌÌÌÌÌ@SHƒì ·Q·AH‹YkÒdÐHKx‰S|H‹L‹‚P   H‹’H   èÌ÷ÿÿH‹‹   H‹“˜   HÑHÇƒh      H‹ÂH‰“`  H;Ñv.HÿÈ€8.tH;ÁwóƒKP3ÀHƒÄ [ÃHÿÀH+ÐH‰ƒ`  H‰“h  ƒKP3ÀHƒÄ [ÃÌÌÌÌÌÌÌH‰t$ W¸0@  è S  H+àH‹&¼  H3ÄH‰„$ @  öAPH‹òH‹ùt
¸   éø   H‹IHT$ E3ÉA¸ÿ?  ÿå^  …Ày,ÿ_  =3'  u3ÀéÈ   E3À3Ò‹Èÿð_  H‰ƒÈÿé°   uH¤Š  ÿž_  H‰ƒÈÿé–   H‰œ$P@  HcØHO0LD$ L‹ËH‰Hèã(  H;Ãt)¶D$ „ÀxH†Š  <uHcŠ  ÿM_  H‰ƒÈÿë@HƒX t3H‹O`º   D‹Âÿ´_  H‹ØH…ÀtÚL‹G`H‹WXH‹ÈÿÛ]  ƒOhH‰_X‹GPƒàH‹œ$P@  H‹Œ$ @  H3ÌèÍH  H‹´$X@  HÄ0@  _ÃÌÌÌÌÌÌÌÌÌÌÌÌH‰\$ UVWAVAWHƒì@H‹Éº  H3ÄH‰D$0I‹ðH‹êH‹ùI‹ØD  H‹OH‹ÕE3ÉH+ÓD‹ÃHÖÿ¯]  HcÈ…ÀyLÿº]  =3'  u4H‹GHL$ º   A¸'  ÇD$(   H‰D$ ÿ´^  …À~	¿D$*…Àÿ™_  H‹ÃëH+ÙH…ÛŒH‹ÆH‹L$0H3ÌèüG  H‹œ$ˆ   HƒÄ@A_A^_^]ÃÌÌÌÌÌÌÌÌ@SHƒì HcAxH‹ÚH¬Ä  H‹ÂLL$0H‰  H‰‹A|HÁÐ   ‰ƒ   H‹A°H‰C(H‹AÐH‰C H‹AðH‰C‹ˆ   ‰C3ÀD@H‰ChH‰C`H‰CXÿ0`  …ÀuH‹D$0H‹H‰K8HƒÄ [ÃÌÌÌÌÌÌÌÌÌÌL‹ÜI‰[WHƒì@IcÀ3ÿH‰‹T$pH‰AH‹D$xM‹ÑL‰II‰CèH‹Ù‰QI‰{àMK(MCI‹ÊI‰{ØI‰{‰|$pÿu]  ‹T$pH‹L$PÿÞ]  H‹L$PH‰C HcD$pH‰C(ÿ^^  fÇC0 ‰{4f‰{FƒcPþƒchþHKxH‰{XH‰{`H‰{pèÜòÿÿƒøÿuÀH‹\$XHƒÄ@_Ãƒ£   þÇƒ¸  ÿÿÿÿH‹\$X3ÀHƒÄ@_ÃÌÌÌÌÌÌÌÌÌÌÌÌ@SHƒì H‹ÙHƒÁxèóÿÿ‹‹¸  …Éxÿ.Z  Çƒ¸  ÿÿÿÿH‹Kÿ[  H‹K ÿ[  öƒ   tEH‹›¨  H…Ût9H‰|$0fD  ƒ{ H‹;uH‹KH;ËtÿØZ  H‹ËÿÏZ  H‹ßH…ÿu×H‹|$0HƒÄ [ÃÌÌÌÌÌÌÌÌÌÌÌÌ@SUVWATAUAVAWHì¨   H‹å·  H3ÄH‰„$   3ÛI‹éL‰L$hE‹èH‹úH‰T$HH‰L$`D‰D$pHÔ¿ÿÿDKDC/Hµ»  HL$pH‰\$xH‰D$ ÿY  H…ÀtH‹pëH5Þƒ  º   L‹ÃL5o¾  HJÿHBIÀHÑèH;ÁHBÈH‹ÁHÀE‹ÆE;Í~H‹Ñë}L‹ÁI;ÐuÍL‹óëHÉM‹tÎÿ™Y  ‹ÿÁX  H‹Èÿ W  H‰l$8L§¨  I‰$I‰\$H‹—ˆ   H‹€   ƒ   ˆ\$0LD$XE3ÉH‰\$(H‰D$PÇD$    ÿ[  L‹øH‰D$@HƒÉÿ8\HIuöHi3HM(ÿFY  H‹øH…À„c  HH(L_u  E‹ÍH‹ÕL‰|$(‰XH‰HH‰H‰@H‰h H‰t$ ÿÏZ  H‹OHƒÈÿHÿÀ8uøH‰G H‹H‹ÏH…Àt„     H‹ÈH‹ H…ÀuõI‹D$H…ÀuI‰<$ëH‰8I‰L$A¿(   A‹Ïÿ®X  H…À„«  Hžr  Ç@   H‰H‰HHÇ@[  I‹L$H…ÉuI‰$ëH‰I‹ÏI‰D$ÿgX  H…À„A  H'r  Ç@   H‰H‰HHÇ@   I‹L$H…ÉuI‰$ëH‰I‰D$HƒÉÿD  HÿÁA8u÷HƒÊÿ HÿÂ8uøH‹D$XHƒÀHÈHÊè4çÿÿH‹l$@H‹øH…À„˜  H‹P H‹HL‹ÎM‹ÆH‰l$ ÿ’Y  H‹GHƒÉÿfHÿÁ8uøH‰O H‹H‹ÏH…Àtf„     H‹ÈH‹ H…ÀuõI‹D$H…ÀuI‰<$ëH‰8I‰L$I‹ÏÿtW  H…À„+  HTq  H‹ÐÇ@   H‰HI‹ÌH‰HÇ@   èÓåÿÿH‹|$H‹W|3ÀHL$pDHE‹ÅH‰\$pH‰D$xH‰„$€   è¶¼ÿÿH9\$p„À  HL$pA¸   H‹×èHÈÿÿH‹|$pH…ÿu>WO3ÉL‹ówNÿ%V  H‹øH…À…   ÿ³V  Hôv  HH`ÿJV  N³ÿÉU  ÌL‹t$xH‹´$€   M~(L;þrXI·€   HþÿÿÿrH‹i  ¹   ÿøY  HVH‹Ïÿ³U  H‹øH…Àu!ÿEV  H†v  HH`ÿÜU  Oÿ[U  ÌH‹Û‚  I‰>H‹Ø‚  I‰D>H‹Ô‚  I‰D>H‹Ð‚  I‰D>H‹Ì‚  I‰D> H…ÿ…Œ   WO3ÉL‹ûwNÿ5U  H‹øH…ÀtPnÂH‹«‚  L‹ÃI‰?H‹¥‚  I‰D?I‹$L¼$   H…É„¿    ‹Q…Ò„ž   ÿÊ„   H‹Ãé’   ÿwU  H¸u  HH`ÿU  ¹   ÿ‹T  ÌIoH;îr‡Hµ€   HþÿÿÿrHZh  ¹   ÿÇX  HVH‹Ïÿ‚T  H‹øH…À…LÿÿÿÿU  HQu  HH`ÿ§T  Oÿ&T  ÌH‹AëH‹A H‹	LÀH…É…Dÿÿÿˆœ$   I¹ÍÌÌÌÌÌÌÌ IÿÏI‹ÁI÷àHÁê¶ÂÀàÉD*ÁA€À0EˆL‹ÂH…ÒuÖL´$   M+÷H…ÿuZH‹ëIƒþNsrNëI‹öIþÿÿsI¶€   HV3ÉÿÁS  H‹øH…À…   ÿOT  Ht  HH`ÿæS  OÿeS  ÌLõL;örXI¶€   HþÿÿÿrH5g  ¹   ÿ¢W  HVH‹Ïÿ]S  H‹øH…Àu!ÿïS  H0t  HH`ÿ†S  OÿS  ÌL„$   H/I‹×M+Çè_H  ½   H…ÿuAUM3ÉL‹óD‹ýuLÿÿR  H‹øH…À…‚   ÿS  HÎs  HH`ÿ$S  Mÿÿ£R  ÌM~L;þrXI·€   HþÿÿÿrHrf  ¹   ÿßV  HVH‹ÏÿšR  H‹øH…Àu!ÿ,S  Hms  HH`ÿÃR  OÿBR  Ì¸
  fA‰>H…ÿu;WO3ÉL‹ûÿTR  H‹øH…À…‚   ÿâR  H#s  HH`ÿyR  OÿøQ  ÌIoH;îr]Hµ€   HþÿÿÿrHÇe  ¹   ÿ4V  HVH‹ÏÿïQ  H‹øH…Àu!ÿR  HÂr  HH`ÿR  Oÿ—Q  Ì¸
  L‹ÅH‹×H‹ÏfA‰?è6áÿÿH‹ÐH…ÀuVH…ÿt	H‹Ïÿ`R  H‹l$@H‹D$PH…Àt	H‹ÈÿHR  H‹ÍÿçT  ƒÈÿH‹Œ$   H3ÌèL=  HÄ¨   A_A^A]A\_^][ÃH‹ÈH‹ H…ÀtH‹ÈH‹ H…ÀuõI‹$H‰I9\$uI‰L$H‹|$PH‹t$HL‹L$hI‰$L©~  H…ÿA‹ÕH‹ÎLEÇèÓãÿÿL‹L$`LcFA‹‰  …Ét‹ÃM9„Á  tÿÃ;Ùrî;Ùuù   s‹ÃM‰„Á  Aÿ  M;(   vM‰(   H…ÿt	H‹ÏÿbQ  H‹L$@ÿÿS  3Àéÿÿÿÿ"Q  Hcq  HH`ÿ¹P  ¹   ÿ6P  ÌÿÿP  H@q  HH`ÿ–P  ¹   ÿP  ÌÿÜP  Hq  HH`ÿsP  ¹   ÿðO  Ìÿ¹P  Húp  HH`ÿPP  ¹   ÿÍO  ÌÌÌÌÌ@SHìÐ  H‹ ®  H3ÄH‰„$À  M‹ÈL‰D$(L‹‚    H‰T$ HƒÈÿHÿÀA€<  uöH;‚¨   tA¸  è¿õÿÿéÎ   H‹T  3Ò‰T$@HcI‹H‹LÈøH‹A(H‰T$Xˆ”$°   H‰D$HH‹S  H‰T$PHcI‹H‹TÈøHŒ$À   H‹‚è  H‰D$0H„$À   H‰‚è  H‹ÔèiD  H‹\$(…ÀuHL$@H‹ÓÿtR  H‹½R  L‹ËE3ÀHcH‹H‹TÈøH‹D$0H‰‚è  H‹YS  HcH‹H‹TÈøH‹L$ ‹’Ø   è®áÿÿ3ÀH‹Œ$À  H3Ìè¬:  HÄÐ  [ÃÌÌÌ@SWAVHì€   H‹½¬  H3ÄH‰D$PH‹ùH‰L$(H‹Š    I‹ØL‹òH‰\$ H…É„	  HƒÈÿHÿÀ€< u÷H;‚¨   tL‹ËA¸  H‹Ïèkôÿÿé  H…É„Ö  H‹‚¨   ¶Tÿ€ú.t€ú uL‹ËA¸ô  I‹ÖH‹Ïè3ôÿÿéã  H…É„ž  3ÒÿÅN  …ÀˆŽ  H‰¬$¸   3íL‰|$`I‰®¨  I‰®°  I‹–h  I‹Ž`  AƒŽ   A‰†¸  3ÀH‰l$0H‰D$8H‰D$@èõÄÿÿA‹V|H…ÀL=?{  DMHL$0A¸È   LEøèß³ÿÿH9l$0uL‹ËE3Àºô  I‹ÎèEàÿÿƒÈÿé%  HL$0A¸   I‹ÖH‰t$xL‰d$pèP¿ÿÿH‹\$0H…ÛuBSO3ÉH‹õ{NDcÿ)M  H‹ØH…À…   ÿ·M  Høm  HH`ÿNM  O³ÿÍL  ÌH‹t$8H‹|$@LfL;çrYI¼$€   HÿÿÿÿrH‘`  ¹   ÿþP  HWH‹Ëÿ¹L  H‹ØH…Àu!ÿKM  HŒm  HH`ÿâL  KÿaL  ÌH‹Iz  H‰‹Gz  ‰D·@z  f‰DHƒÎÿH…ÛueL‹åHÿÆA8,7u÷HƒþNs¿N   ëH‹þHþÿÿsH¾€   HW3Éÿ.L  H‹ØH…À…Š   ÿ¼L  Hýl  HH`ÿSL  KÿÒK  ÌHÿÆA8,7u÷IôH;÷rXH¾€   HÿÿÿÿrH™_  ¹   ÿP  HWH‹ËÿÁK  H‹ØH…Àu!ÿSL  H”l  HH`ÿêK  KÿiK  ÌIƒÈÿIÿÀC8,u÷II‹×èÁ@  HHy  A¸   I‹Ïÿ)K  …À…×   H…ÛuBPO3ÉH‹õxNDxÿGK  H‹ØH…À…‚   ÿÕK  Hl  HH`ÿlK  O³ÿëJ  ÌL~L;ÿrXI¿€   HÿÿÿÿrHº^  ¹   ÿ'O  HWH‹ËÿâJ  H‹ØH…Àu!ÿtK  Hµk  HH`ÿK  KÿŠJ  ÌH‹Šx  H‰‹ˆx  ‰D·x  f‰D¶wx  ˆDI‹÷L‰l$hA½   H…ÛuASO3ÉH‹õE‹ý{NÿfJ  H‹ØH…À…‚   ÿôJ  H5k  HH`ÿ‹J  O³ÿ
J  ÌL~L;ÿrXI¿€   HÿÿÿÿrHÙ]  ¹   ÿFN  HWH‹ËÿJ  H‹ØH…Àu!ÿ“J  HÔj  HH`ÿ*J  Kÿ©I  Ì¸
  f‰H…ÛuBSO3ÉL‹ý{NDcÿµI  H‹ØH…À…ƒ   ÿCJ  H„j  HH`ÿÚI  O³ÿYI  ÌMgL;çrYI¼$€   HÿÿÿÿrH']  ¹   ÿ”M  HWH‹ËÿOI  H‹ØH…Àu!ÿáI  H"j  HH`ÿxI  Kÿ÷H  ÌH‹§v  @ˆl$OI¹ÍÌÌÌÌÌÌÌI‰H‹•v  I‰DMc†„  L|$Offff„     IÿÏI‹ÁI÷àHÁê¶ÂÀàÉD*ÁA€À0EˆL‹ÂH…ÒuÖHt$OI+÷H…ÛuZL‹åHƒþNszNëH‹þHþÿÿsH¾€   HW3Éÿ„H  H‹ØH…À…   ÿI  HSi  HH`ÿ©H  Kÿ(H  ÌIôH;÷rXH¾€   HÿÿÿÿrHø[  ¹   ÿeL  HWH‹Ëÿ H  H‹ØH…Àu!ÿ²H  Hóh  HH`ÿIH  KÿÈG  ÌLD$OII‹×M+Çè%=  H…ÛuASO3ÉH‹õM‹ý{NÿÊG  H‹ØH…À…‚   ÿXH  H™h  HH`ÿïG  O³ÿnG  ÌL~L;ÿrXI¿€   HÿÿÿÿrH=[  ¹   ÿªK  HWH‹ËÿeG  H‹ØH…Àu!ÿ÷G  H8h  HH`ÿŽG  KÿG  ÌA¼
  fD‰$H…Ûu;SO3ÉL‹ýÿG  H‹ØH…À…‚   ÿ¬G  Híg  HH`ÿCG  KÿÂF  ÌMoL;ïrXI½€   HÿÿÿÿrH‘Z  ¹   ÿþJ  HWH‹Ëÿ¹F  H‹ØH…Àu!ÿKG  HŒg  HH`ÿâF  KÿaF  ÌM‹ÅH‹ÓH‹ËfE‰$èÖÿÿH‹ÐH…Àu[H…Ût	H‹Ëÿ/G  L‹L$ E3Àºô  I‹Îè*ÙÿÿƒÈÿL‹l$hL‹d$pH‹t$xH‹¬$¸   L‹|$`H‹L$PH3Ìè2  HÄ€   A^_[ÃH‹ÈH‹ H…Àt€    H‹ÈH‹ H…ÀuõI‹†°  H…Àu	I‰–¨  ëH‰L‹L$ I‰Ž°  E3ÀºÈ   I‹Îè¡ØÿÿL‹L$(McFA‹‰  …Ét‹ÅM9„Á  tÿÅ;érî;éuù   s‹ÅM‰„Á  Aÿ  M;(   vM‰(   3Àé ÿÿÿL‹ËA¸”  I‹ÖH‹Ïèsëÿÿé#ÿÿÿÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰L$WHƒì H‹šI  H‹ÚI‹øHcI‹ H‹TÈøH‹ËHƒÂèéÿÿH‹Ð   LL$0Hêr  A¸   ÿžI  …ÀuH‹L$0H‹×H‹	ÿáF  H‹BI  HcH‹H‹LÈøÇØ   È   H‹Ïÿ,I  ƒøÿuÀH‹\$8HƒÄ _ÃH‹óG  H‹\$8HcH‹H‹LÈø3ÀÆ²   HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌ@UWAVHìð  Hl$0H‰è  H‰µø  H‹j¢  H3ÅH‰…°  ‹$E3öI‹øH‹ÙL‰ED‰uDˆu ¸  H+àHL$0‹º  H‰MDˆ1ÿoE  D‰u0H‹ƒX   H‰E8H‹¡G  L‰uHDˆµ    L‰u@HcH‹H‹TÈøH°   H‹‚è  H‰E H…°   H‰‚è  HUÐèì8  H‹u…ÀuT»   HE0LEKH‹ÖD‹ËL‰uH‰D$ ÿ)E  …Àu&H‹EH…Àt"€xuD90tA‹ÞHM‰]ÿÁF  ë‰]ë‹]H‹øF  H‹}HcH‹L‹DÈøH‹E I‰€è  D87tH‹ÖH‹Ïÿ}E  D8u t	H‹Ïÿ†F  ‹ÃH‹°  H3Íèí.  H‹è  H‹µø  H¥À  A^_]ÃÌÌÌH‰\$H‰l$H‰t$H‰|$ AVHƒì0H‹/G  E3öI‹ØLcI‹ H‹úN‹LÈøH‹ñA‹îI‰Hƒºh  u(H‹Š`  ·f;K  u¶A:K  u	H9ª    u½   L9¶X   u…íuBèýÿÿƒøÿu8H‹½F  E3À3ÒHcH‹ÇD$    H‹LÈøL‰1D‹OHŽp   ÿ‘C  éŽ  L9¶X   tL‹ÃH‹Îè˜ýÿÿ…Àt[…í…¥   L‹ÃH‹×H‹Îèþñÿÿ…ÀtL‹ËA¸ô  H‹×H‹Îèöçÿÿ…À„+  H‹7F  HcH‹H‹TÈøºØ   0  uDˆ²Ü   3Éÿ©F  D‹OHŽp   E3À3ÒÇD$    ÿóB  H‹H‹éE  Hc
H‹LÈøL‰1Hc
H‹H‹LÈøL‰±@  éÉ   L9¶X   t{H‹E  H‹B`H‰\Ñ  H‹¥E  HcH‹H‹LÈøH“†ÿÿDˆ±Ü   3ÉH‰B`ÿF  H‹ÉD  H‹"Ñ  H‹kE  H‰H`H‹Hc
H‹LÈøÆÜ   Hc
H‹H‹LÈøL‰±@  L‹ÃH‹×H‹Îèòÿÿ…ÀtD‹OHŽp   E3À3ÒÇD$    ÿB  H‹E  HcH‹H‹LÈøL‰1H‹\$@H‹l$HH‹t$PH‹|$X3ÀHƒÄ0A^ÃÌ@SHƒì H‹ÙH‹	H‹IÿŠA  H‹E3ÉLcXH‹ A‹ÑD‹@E…Àt?L9\Ðt	ÿÂA;Ðròë.AHÿ;Ñs#„     DBJ‹LÀH‰LÐ‹HA‹ÐÿÉD;ÁråÿH‹  …ÒtU„     N9œÈ  t
AÿÁD;Êrîë9JÿD;És+@ EAA‹ÑJ‹ŒÀ  E‹ÈH‰ŒÐ  ‹ˆ  ÿÉD;ÁrÙÿˆ  H‹èåÿÿH‹HƒÄ [Hÿ%\@  ÌÌÌÌÌÌÌÌÌÌÌÌH‹ÄH‰XH‰h H‰HVWATAVAWHƒìP3ÿIƒÌÿ€:[I‹éM‹øH‹ÚH‰xÈD‹÷H‰xÇ@¸  …ï   HJÿÙ=  H‹ðH…ÀuƒÈÿé¶   º]   H‹Èÿ³?  H‰„$ˆ   H…ÀtV@ˆ8H‹„$ˆ   HÿÀH‰„$ˆ   ¶€ù:…’   HHH”$ˆ   A¸
   ÿ|>  ‰„$€   ÿÈ=þÿ  †»   H‰¼$ˆ   ÿR?  H£l  L‹ÃHH`ÿæ>  H‹ÎƒÏÿÿZ?  M…öt	I‹ÎÿL?  Iƒüÿv	I‹Ìÿ?  ‹ÇL\$PI‹[@I‹kHI‹ãA_A^A\_^Ã„ÉtYë”H‹Êÿë<  H‹ðH…À„ÿÿÿº:   H‹ÈÿÉ>  H‰„$ˆ   H…À„hÿÿÿ@ˆ8H‹Œ$ˆ   HÿÁH‰Œ$ˆ   éÿÿÿH‹„$ˆ   H…À„<ÿÿÿH‹œ$    HD$@LŒÍ  H‰\$0H‰D$(HŸÍ  H”$€   H‹ÎH‰D$ è¢ÒÿÿLcàIƒüÿuBH‹L$@D‹Œ$€   H9k  H…ÉL‹ÆH‹ÓHEÁH™k  H‰D$ è÷ÑÿÿH‹L$@ÿä@  éÑþÿÿHƒÈÿLìüÿÿH5Í  L;àA±L‰%¸¬  IGÄ3Ò‰=¼´  H‰ÍÌ  ‹„$€   L‰%§¬  Ç•¬     H‰5¾Ì  ‰ÀÌ  ÿšA  HƒËÿfD  HÿÃA8<u÷‹ÓI‹ÏÿÌ?  L‹ðH…À„JþÿÿH‰‘Ì  H‰’Ì  H…ít>HƒËÿHÿÃ@8<+u÷‹ÓH‹Íÿ•?  H…À„þÿÿH‰mÌ  H‰nÌ  Ç4Ì     é"þÿÿH‰=PÌ  H‰=QÌ  ÇÌ     éþÿÿÌÌH‰\$H‰l$H‰t$WHƒì0H‹úH‹ñHT$X3ÛH‹ÏI‹èH‰\$XèÞÿÿ…ÀyIL‹L$XL‹G Hkj  H‹Õè£ÐÿÿH‹L$Xÿ?  D‹OHŽp   E3À3ÒÇD$    ÿj=  ƒÈÿé‰   ƒøu>ƒxuL‹ÍA¸õ  H‹×H‹ÎèóáÿÿëhD‹GHNº   èßÈÿÿL‹ÅH‹×H‹ÎèÑøÿÿëD‹NLcG…Ét@ ‹ÃL9DÆtÿÃ;Ùrñ;Ùuù   s
‹ÃL‰DÆÿFL;†(   vL‰†(   3ÀH‹\$@H‹l$HH‹t$PHƒÄ0_ÃÌÌÌÌH‰\$H‰t$WHƒì0ö‚   H‹ÚH‹ù„Å   ‹’¸  …ÒxjH‹¨  Hƒ9 u]LD$HèÌÿÿ…Àt1D‹KHp   E3À3ÒÇD$    ÿ[<  ƒÈÿH‹\$@H‹t$PHƒÄ0_ÃHƒ|$H u‹‹¸  ÿ”:  Çƒ¸  ÿÿÿÿH‹SLD$HH‹¨  èËÿÿ…Àt=3'  u‘Hƒ»¨   u'ƒ»¸   }D‹KHp   E3À3ÒÇD$    ÿÙ;  H‹\$@H‹t$P3ÀHƒÄ0_ÃÌÌÌÌÌÌÌH‰\$UVWHƒì0H‹YH‹1HcÂA‹èH‹ùH9…N  Hcƒh   HÇD$P    H‹È‰D$XÿÎ:  H‹øH…À„  H‹LD$XH‹Ðÿ9;  L‹Æ3Ò‹ÈH‹èÿY;  …À…•   ¹À  ÿŽ:  H…À„¢   ‹L$XH‰t$(L‹Ï‰L$ D‹ÅH‹ÓH‹ÈH‰D$PèBÞÿÿƒøÿtJH‹p   LD$PA¹   ‹ÕÇD$(   HÇD$     ÿÒ:  H‹T$PL‹ÆH‹Ëèòüÿÿ3ÀH‹\$`HƒÄ0_^]ÃH¤g  H‹ÖèÄÍÿÿH‹Ïÿ:  H‹Íÿb:  3ÀH‹\$`HƒÄ0_^]ÃÿÅ9  HZ  HH`ÿ\9  ¹   ÿÙ8  Ìÿ¢9  HãY  HH`ÿ99  ¹   ÿ¶8  ÌH‹p   LD$Pÿó:  …Àu0÷Å   tH‹T$PL‹ÆH‹ËH‹ÿW@öÅtH‹T$PL‹ÆH‹ËH‹ÿW3ÀH‹\$`HƒÄ0_^]ÃÌÌÌÌÌÌÌÌH‹ÄH‰HSWHƒìX3ÛH‹ú9È  „  H‰hH‰pL‰pèL5ÿüÿÿH5¸§  H-ÑûÿÿH±§  Hº·  A¸  HÇD$p   ÿõ8  H–¯  HŸ¿  A¸  ÿÛ8  ‹•Ç  HD$pL¿  Hr·  ÿÁE3ÉH‰D$ ÿ¢9  …À~'HT$0HB§  H‰|$0H‰t$8H‰l$@L‰t$HèÁÅÿÿëtÿ÷8  ='  u'9:Ç  …Tÿÿÿ‹ÃH‹´$€   H‹l$xL‹t$PHƒÄX_[ÃE3À3Ò‹Èÿ´9  Hd  H‹×L‹ÀH‹Øè×ËÿÿH‹ËÿÆ:  ƒÈÿë·‹ÃHƒÄX_[ÃÇÖÆ      ÃÌÌÌÌÌ@SUVWATAVAWHìÀ  H‹7•  H3ÄH‰„$°  E3äI‹èHD$@ÇD$0   L2L  LL$HD‰d$(L‹ò‹ñL‰d$HÇD$@   A‹üH‰D$ A‹ÜE‹üÿº9  ƒøÿ„æ   ƒøStƒøtuH‹\$HëH‹|$HHD$@ÇD$0   LL$HLÊK  I‹Ö‹ÎD‰d$(H‰D$ ÿm9  ƒøÿu·H…Û„”   HT$`H‹ËÿÙ5  …Àt$ÿ÷6  HÈd  L‹ÃHH`ÿ‹6  ¸   éŠ  ·D$f¹ ð  f#Á¹ @  f;Át$ÿ¼6  H­d  L‹ÃHH`ÿP6  ¸   éO  H”$    L‹ÅH‹Ëÿº:  H…Àt6Hœ$    ë,HŒ$    L‹Åº  ÿ%7  Hnd  Hœ$    H…ÀHDÙHcD$@;ð~M‹<ÆM‹ÏL‹ÃH‹×H‰l$ èöÿÿƒøÿ„CÿÿÿH‹79  HL$PD‰   ÿ5  LD$PHL$`º4   ÿà4  …ÀtH•a  HL$`A¸   ÿ6  LD$`H|F  Hác  L‹ÏH‰\$ ÿƒ5  HÌýÿÿ¹   ÿ¹4  H‹ÕèyüÿÿHÒÄ  ÿ´8  H‹U¤  ÿ6  H‹€Ä  H…Étÿ5  H‹~Ä  H…Étÿ‹5  H‹|Ä  H…Étÿy5  3ÀH‹Œ$°  H3Ìè‡   HÄÀ  A_A^A\_^][ÃÌÌÌÌÌL‰L$ H‰T$UVWATAUAVAWHƒìP‹A¶iD¶iD¶aI‹ùL‹ñ3É‰„$˜   I‹ðD‹ÑH‰L$(D‹ÙH‰L$0H‰L$8D‹ÉH‰L$ ‹ÑH‰Œ$   D‹ùH…ÿu(ƒý6uH‹‘Q  H…ÀtI‹ÎÿÐ3ÀHƒÄPA_A^A]A\_^]ÃH‰\$Hƒý)u
M‹ÐL‰D$(ëNƒý+u
M‹ØL‰D$0ë?ƒýuL‰D$8ë0ƒýu
M‹ÈL‰D$ ë!ƒýuI‹ÐH‰”$   ëƒýw¹  ð£ésM‹øI‹ØLÇL‰„$    I;ð„J  Haÿÿ¶;ƒý-Aö uÿÀ‰„$˜   = @ ‡¨  Eþƒø4‡œ  H˜‹Œäº  HOaÿÿHÈÿá@€ÿ„í  @€ÿ
„ã  A€&IÇFÿÿÿÿH‹CP  H…ÀtI‹ÎÿÐ…À…S  L‹„$    @€ÿHu!½   H‹”$   L‹L$ L‹T$(L‹\$0é  A€&üéœ  @€ÿTuA€&ýA€½   én  @€ÿE…ô   A€&ü½   AÆFDeðéM  A€&IÇFÿÿÿÿH‹­O  H…ÀtI‹ÎÿÐ…À…½   L‹„$    @€ÿ
„kÿÿÿ@€ÿ„aÿÿÿ@€ÿH…’   ½   éMÿÿÿ½   éî  ½   éä  ½	   éÚ  GÏ<wc@¾Ç½
   fƒè0fA‰Fé¼  @€ÿ.u
½   é¬  @€ÿ0|6@€ÿ90A·N·ÁfÁàfÈ@¾ÇfÉfƒé0fÈ¸ç  fA‰Nf;È†p  AÆFH+ÞH‹Ãé€  GÐ<	wé@¾Ç½   fƒè0fA‰FéB  @€ÿ u
½   é2  @€ÿ0|¼@€ÿ9¶A·N·ÁfÁàfÈ@¾ÇfÉfƒé0fÈ¸ç  fA‰Nf;ÈwŠéõ  GÐ<	w@¾Ç½   fƒè0fA‰Fé×  @€ÿ …]ÿÿÿéÈ  GÐ<	w5A·N·ÁfÁàfÈ@¾ÇfÉfƒé0fÈ¸ç  fA‰Nf;È‡!ÿÿÿéŒ  @€ÿ
„(  @€ÿt@€ÿ …ÿÿÿ½   éh  @€ÿu
½   éX  @€ÿ
‹¼$˜   H™^ÿÿ…N  ½(   éD  @€ÿ„,  @€ÿ
„"  A€&IÇFÿÿÿÿH‹‚M  H…ÀtI‹ÎÿÐ…À…’þÿÿL‹„$    G¿<‡zþÿÿ@¾ÇAÆF A¼   ƒÀ½ƒø‡×   H^ÿÿH˜‹Œ‚¸»  HÊÿáAÆF½   é ýÿÿAÆF ½   éñüÿÿEˆf½   éãüÿÿAÆF½   éÔüÿÿAÆF
½   éÅüÿÿAÆF½   é¶üÿÿAÆF½   é§üÿÿAÆF½   é˜üÿÿAÆF½   é‰üÿÿAÆF½   ézüÿÿAÆF½   éküÿÿAÆF½   é\üÿÿAÆF½   éMüÿÿAÆF½   é>üÿÿ@„ÿ„pýÿÿA¶NH‹”È°: @€ÿ u(€ùtA‹Ä€< tAÆFH‹”$   ½   AÿÄé¥  €ù„Ñ  A‹Ä@:<„Ä  €ùuNAƒüu@€ÿH…ª  H‹”$   AÆFAÿÄée  Aƒü…‹  @€ÿP…  H‹”$   AÆF	AÿÄé<  €ù…©   AƒüuU@€ÿOuH‹”$   AÆFAÿÄé  @€ÿEuH‹”$   AÆFAÿÄé÷  @€ÿ-…  H‹”$   AÆFAÿÄéØ  Aƒüu@€ÿA…ø   H‹”$   AÆFAÿÄé³  Aƒü…Ù   @€ÿA…Ï   H‹”$   AÆFAÿÄéŠ  Aƒüuh€ù…«   @€ÿRuH‹”$   AÆFAÿÄé`  €ù…‡   @€ÿUuH‹”$   AÆFAÿÄé<  €ùug@€ÿAuaH‹”$   AÆFAÿÄé  Aƒüu €ùuA@€ÿSu;H‹”$   AÆFAÿÄéö  Aƒüu €ùu@€ÿPuH‹”$   AÆFAÿÄéÐ  AÆFAÿÄH‹”$   é»  @€ÿ „±  @€ÿ/t%@€ÿ*t@€Ï @€ïa@€ÿ‡#ûÿÿL‹û½   é†  L‹ûH‹ÓH‰œ$   ½   én  @¶Ç ,a<†^  @€ÿ:u
½   éN  @€ÿ.u
½   é>  @€ï0@€ÿ	‡Àúÿÿ½   é&  ½   é  @¶Ç ,a<†  @€ÿ/t@€ÿ,~
@€ÿ:Œö  @€ÿ t@€ÿ/„cÿÿÿ@€ÿ:…lúÿÿ½   éÒ  M…ÿt*H‹QI  H…ÀtL‹ÃI‹×I‹ÎM+ÇÿÐ…À…@úÿÿL‹„$    3ÀD‹øhéëøÿÿGÐ<	†‹  @€ÿ t³@€ÿ/…úÿÿH‹ÓH‰œ$   ½   éf  H²Yÿÿ@¶Ç€¼pñ   …M  @€ÿ
„Õ  @€ÿ„_  @€ÿ „®   @€ÿ#tY@€ÿ?…¯ùÿÿH…Òt'H‹ŽH  H…ÀtL‹ÃI‹ÎL+ÂÿÐ…À…ùÿÿL‹„$    L‹L$ L‹T$(L‹\$03À‹ÐH‰„$   héÐ   H…Òt'H‹?H  H…ÀtL‹ÃI‹ÎL+ÂÿÐ…À…AùÿÿL‹„$    L‹L$ L‹T$(L‹\$03À‹ÐH‰„$   hé   M…ÿt"H‹ H  H…ÀtL‹ÃI‹×I‹ÎM+ÇÿÐ…À…ïøÿÿH‹”$   3ÉD‹ùH…Òt!H‹¼G  H…ÀtL‹ÃI‹ÎL+ÂÿÐ…À…¾øÿÿ3É½   H‹ÑH‰Œ$   L‹L$ L‹T$(L‹\$0L‹„$    HLXÿÿ‹¼$˜   HÿÃI;Ø„ö  ‹„$˜   é öÿÿM…ÿt"H‹YG  H…ÀtL‹ÃI‹×I‹ÎM+ÇÿÐ…À…HøÿÿH‹”$   3ÉD‹ùH…Òt!H‹G  H…ÀtL‹ÃI‹ÎL+ÂÿÐ…À…øÿÿ3ÉAÇF  	 ½'   éLÿÿÿM…ÿt"H‹íF  H…ÀtL‹ÃI‹×I‹ÎM+ÇÿÐ…À…Ü÷ÿÿH‹”$   3ÉD‹ùH…Òt!H‹©F  H…ÀtL‹ÃI‹ÎL+ÂÿÐ…À…«÷ÿÿ3ÉAÇF  	 ½(   éàþÿÿHNWÿÿ@¶Ç€¼pñ   tL‹ËH‰\$ ½   éÛþÿÿ@€ÿ
ts@€ÿt)@€ÿ „óüÿÿ@€ÿ#t@€ÿ?…E÷ÿÿé°þÿÿ½   é¦þÿÿM…ÿt*H‹%F  H…ÀtL‹ÃI‹×I‹ÎM+ÇÿÐ…À…÷ÿÿL‹„$    3ÉAÇF  	 i'D‹ùé·õÿÿM…ÿt*H‹áE  H…ÀtL‹ÃI‹×I‹ÎM+ÇÿÐ…À…ÐöÿÿL‹„$    3ÉAÇF  	 i(D‹ùésõÿÿHjVÿÿ@¶Ç€¼pñ   …þÿÿ@€ÿ
„b  @€ÿ„ß   @€ÿ th@€ÿ#t@€ÿ?…köÿÿéÖýÿÿM…Ét*H‹ME  H…ÀtL‹ÃI‹ÑI‹ÎM+ÁÿÐ…À…DöÿÿL‹„$    H‹”$   L‹T$(L‹\$03ÀD‹ÈH‰D$ héƒýÿÿM…ÿt"H‹E  H…ÀtL‹ÃI‹×I‹ÎM+ÇÿÐ…À…ñõÿÿH‹T$ 3ÉD‹ùH…Òt!H‹ÉD  H…ÀtL‹ÃI‹ÎL+ÂÿÐ…À…Ãõÿÿ3ÉH‹”$   L‹ÉH‰L$ ½   é ýÿÿM…ÿt"H‹‘D  H…ÀtL‹ÃI‹×I‹ÎM+ÇÿÐ…À…€õÿÿH‹T$ 3ÉD‹ùH…Òt!H‹XD  H…ÀtL‹ÃI‹ÎL+ÂÿÐ…À…Rõÿÿ3ÉH‹”$   L‹ÉH‰L$ AÇF  	 ½'   é‡üÿÿM…ÿt"H‹D  H…ÀtL‹ÃI‹×I‹ÎM+ÇÿÐ…À…õÿÿH‹T$ 3ÉD‹ùH…Òt!H‹ßC  H…ÀtL‹ÃI‹ÎL+ÂÿÐ…À…Ùôÿÿ3ÉH‹”$   L‹ÉH‰L$ AÇF  	 ½(   éüÿÿHlTÿÿ@¶Ç€¼pñ   tH‰\$8½   éüûÿÿ@€ÿ
„ýÿÿ@€ÿ„Býÿÿ@€ÿ „úÿÿ@€ÿ#„Ôûÿÿ@€ÿ?…ZôÿÿH‰\$8½   é»ûÿÿHTÿÿ@¶Ç€¼pñ   …©ûÿÿ@€ÿ
„  @€ÿ„Ž   @€ÿ t@€ÿ#„…ûÿÿ@€ÿ?…ôÿÿévûÿÿM…ÿt"H‹îB  H…ÀtL‹ÃI‹×I‹ÎM+ÇÿÐ…À…ÝóÿÿH‹L$83ÿD‹ÿH…Ét"H‹ÅB  H…ÀtH‹ÑL‹ÃL+ÁI‹ÎÿÐ…À…¬óÿÿH‹”$   H‰|$8½   ééúÿÿM…ÿt"H‹B  H…ÀtL‹ÃI‹×I‹ÎM+ÇÿÐ…À…nóÿÿH‹L$83ÀD‹øH…Ét"H‹VB  H…ÀtH‹ÑL‹ÃL+ÁI‹ÎÿÐ…À…=óÿÿH‹”$   3ÉAÇF  	 i'H‰L$8érúÿÿM…ÿt"H‹B  H…ÀtL‹ÃI‹×I‹ÎM+ÇÿÐ…À…÷òÿÿH‹L$83ÿD‹ÿH…Ét"H‹ßA  H…ÀtH‹ÑL‹ÃL+ÁI‹ÎÿÐ…À…ÆòÿÿH‹”$   H‰|$8AÇF  	 ½(   éûùÿÿ@€ÿ „úÿÿ@€ÿH…Žòÿÿ½   éôùÿÿ½    éêùÿÿ½!   éàùÿÿ½"   éÖùÿÿ½#   éÌùÿÿGÏ<‡Qòÿÿ@¾Ç½$   fƒè0fA‰Féªùÿÿ@€ÿ.…ôñÿÿ½%   é–ùÿÿGÐ<	‡òÿÿ@¾Ç½&   fƒè0fA‰Fétùÿÿ@€ÿu
½'   édùÿÿ@€ÿ
…(òÿÿ½(   éPùÿÿ@€ÿ
…Öñÿÿ½(   é<ùÿÿ@€ÿu
½-   é,ùÿÿ@€ÿ
„Ö  HnQÿÿ@¶Ç¶Œpï  „É„—ñÿÿ3ÿ¾ÉL‹Óo)H‰\$(D‹çƒéct>ƒét.ƒétÿÉH/QÿÿtD‹ïéÙøÿÿA½   éÎøÿÿA½   é¼øÿÿA½   é±øÿÿA½   é¦øÿÿHòPÿÿ@¶Ç¶”pï  „Ò„'  AEÿƒø‡ß   H˜‹Œ¼  HÁPÿÿHÈÿá3ÀAÿÄ€úoH‹”$   D‹è¸   DDèéHøÿÿ3ÀAÿÄ€únH‹”$   D‹è¸   DDèé'øÿÿ‹¼$˜   AÿÄHiPÿÿ€únt€útH‹”$   A½   ¸    DEèé øÿÿH‹”$   A½   éí÷ÿÿAÿÄH(PÿÿAƒü
w+A:”  u!H‹”$   ‹¼$˜   Aƒü	…º÷ÿÿE‹ìé²÷ÿÿ3ÀD‹èH‹”$   é™÷ÿÿAÿÄHÛOÿÿAƒüwÞA:”0 uÔH‹”$   ‹¼$˜   Aƒü…m÷ÿÿEl$úéc÷ÿÿAÿÄHžOÿÿAƒüw¡A:”H u—H‹”$   ‹¼$˜   Aƒü…0÷ÿÿEl$ýé&÷ÿÿAÿÄHaOÿÿAƒü‡`ÿÿÿA:”X …RÿÿÿH‹”$   ‹¼$˜   Aƒü…ëöÿÿEl$ûéáöÿÿAÿÄHOÿÿAƒü‡ÿÿÿA:”p …ÿÿÿH‹”$   ‹¼$˜   Aƒü…¦öÿÿEl$éœöÿÿH‹”$   @€ÿ ‹¼$˜   HÇNÿÿ„|öÿÿ3ÀD‹èéröÿÿ@€ÿ:uZM…Òt1H‹í=  H…Àt%L‹ÃI‹ÒI‹ÎM+ÂÿÐ…À…ÌîÿÿL‹„$    H{NÿÿH‹”$   L‹L$ L‹\$03ÀD‹ÐH‰D$(h*éöÿÿ@€ÿu\½,   M…Òt1H‹ˆ=  H…Àt%L‹ÃI‹ÒI‹ÎM+ÂÿÐ…À…gîÿÿL‹„$    HNÿÿH‹”$   L‹L$ L‹\$03ÀD‹ÐH‰D$(é©õÿÿ@€ÿ
…(îÿÿM…Òt1H‹'=  H…Àt%L‹ÃI‹ÒI‹ÎM+ÂÿÐ…À…îÿÿL‹„$    HµMÿÿH‹”$   L‹L$ L‹\$03ÀD‹ÐH‰D$(h(éEõÿÿ@€ÿ „4õÿÿ3É@¶×L‹Û€Ê H‰\$0½+   D‹á@€ÿuPH‹±<  H…ÀtE3ÀH‹ÓI‹ÎÿÐ…À…‹íÿÿL‹„$    3ÉH‹”$   L‹L$ L‹T$(L‹ÙH‰L$0D‹é½,   éÅôÿÿ@€ÿ
uMH‹[<  H…ÀtE3ÀH‹ÓI‹ÎÿÐ…À…5íÿÿL‹„$    3ÉH‹”$   L‹L$ L‹T$(L‹ÙH‰L$0½(   érôÿÿA‹Íƒé	t}ÿÉtTÿÉt2H‹”$   ‹¼$˜   ÿÉHLÿÿt
3ÀD‹èéLôÿÿA€@3ÀD‹èé>ôÿÿ3À€úcH‹”$   D‹è¸   DDèéôÿÿGÐ<	‡—ìÿÿH‹”$   @¾Çƒè0H˜I‰Féíóÿÿ‹¼$˜   H2Lÿÿ€úkuH‹”$   A½   éÕóÿÿ3À€úcH‹”$   D‹è¸   DDèé·óÿÿD¶ÇA€È @€ÿuKM…Ût"H‹2;  H…ÀtL‹ÃI‹ÓI‹ÎM+ÃÿÐ…À…	ìÿÿH‹”$   L‹L$ L‹T$(3ÀD‹ØH‰D$0h,éHóÿÿ@€ÿ
…ƒ   M…Ût"H‹Ý:  H…ÀtL‹ÃI‹ÓI‹ÎM+ÃÿÐ…À…´ëÿÿL‹„$    3ÀD‹ØH‰D$0H‹”$   L‹L$ L‹T$(A‹Í½(   ƒé„•  ÿÉ„„  ‹¼$˜   ÿÉH%Kÿÿ…ÚòÿÿA€éÑòÿÿAƒý‡L  IcÅHKÿÿ‹Œ4¼  HôJÿÿHÈÿá@€ÿ „ŠòÿÿGÐ<	‡ëÿÿ@¾Çƒè0HcÐI‹FH€HJH‹”$   I‰FéXòÿÿAÿÄH©JÿÿAƒüw-E:„x u#‹¼$˜   L‹„$    Aƒü…;òÿÿEl$
é1òÿÿL‹„$    3ÀD‹èéòÿÿAÿÄHZJÿÿAƒü
wÞE:„€ uÔ‹¼$˜   L‹„$    Aƒü	…ìñÿÿEl$éâñÿÿAÿÄHJÿÿAƒüw¡E:„Œ u—‹¼$˜   L‹„$    Aƒü…¯ñÿÿEl$é¥ñÿÿL‹„$    @€ÿ ‹¼$˜   HÐIÿÿ„…ñÿÿ3ÀD‹èé{ñÿÿ3À½+   D‹èéVñÿÿA€éUñÿÿA€éLñÿÿA¶Àè¨t7H‹õ8  H…ÀtI‹ÎÿÐ…À…½éÿÿL‹„$    Aö½   ¸   Dèé_èÿÿ3É‹ù‰Œ$˜   ¨uA€~uAÆFH‹“8  H…ÀtI‹ÎÿÐ…ÀtÿÈ…géÿÿA€€L‹„$    A8~…  A¶¶ÁÀè¨ u3¨t½.   ëZI‹FH…Àt~½5   ëHöÁtI‹ÎèW  …Àuh6ë2H‹/8  H…ÀtI‹ÎÿÐ…À…÷èÿÿAöL‹„$    ½   ¸   DèH‹”$   L‹L$ L‹T$(L‹\$0H~Hÿÿé4ðÿÿI‹~I‹ÀH+ÃH;ÇHBøH…ÿ„ðÿÿH‹¸7  H…ÀtL‹ÇH‹ÓI‹ÎÿÐL‹„$    HÿËHßI)~…2çÿÿH‹‘7  H…ÀtI‹ÎÿÐ…À…YèÿÿAöH‹”$   ½   ¸   DèéïÿÿI‹øH+û„šïÿÿH‹F7  H…ÀtL‹ÇH‹ÓI‹ÎÿÐL‹„$    HÿËHßéÅæÿÿH¼Gÿÿ@¶Ç¶Œpð  €ùÿ„äçÿÿH¾Á½/   I‰FéBïÿÿHŽGÿÿ@€ÿu
½0   é2ïÿÿ@¶Ç¶”pð  €úÿu"@€ÿ;t
@€ÿ …šçÿÿH‹”$   ½1   éÿîÿÿI‹NH¾ÂH‹”$   HÁáHÈI‰NéØîÿÿ@€ÿ‹¼$˜   HGÿÿ…Îîÿÿ½0   éÄîÿÿIƒ~ ‹¼$˜   HöFÿÿuA€ ½(   é¡îÿÿ½2   é—îÿÿI‹~I‹ÀH+ÃH;ÇHBøH…ÿt%H‹6  H…ÀtL‹ÇH‹ÓI‹ÎÿÐL‹„$    HÿËHßI‹F¹3   H;øDéH+ÇI‰Féˆåÿÿ½4   é)îÿÿ½.   éîÿÿH‹Ó5  H…À„¤æÿÿI‹ÎÿÐ…À„—æÿÿH+ÞH‹Ãé  ‹¼$˜   M…Òt"H‹5  H…ÀtL‹ÃI‹ÒI‹ÎM+ÂÿÐ…À…^æÿÿH‹T$0H…ÒtH‹[5  H…ÀtL‹ÃI‹ÎL+ÂÿÐ…À…5æÿÿH‹L$8H…Ét"H‹"5  H…ÀtH‹ÑL‹ÃL+ÁI‹ÎÿÐ…À…	æÿÿH‹T$ H…ÒtH‹æ4  H…ÀtL‹ÃI‹ÎL+ÂÿÐ…À…àåÿÿH‹”$   H…ÒtH‹²4  H…ÀtL‹ÃI‹ÎL+ÂÿÐ…À…´åÿÿM…ÿt"H‹ž4  H…ÀtL‹ÃI‹×I‹ÎM+ÇÿÐ…À…åÿÿH‹„$¨   AˆnEˆnEˆfA‰~H‹\$HHƒÄPA_A^A]A\_^]Ãf¶ž  'Ÿ  `Ÿ  5Ÿ  µŸ  ¿Ÿ  ÉŸ  ÓŸ  ñŸ  M   k   ¸   å   E¡  S®  w¡  Ä¢  ò¤  ?¥  ‡¥  e¥  ‘¥  ¦  G¦  «¨  ©  «  ò«  ›­  ¹­  Ã­  Í­  ×­  á­  ®  ®  9®  ]®  q®  ¯  o²  ´   ´  a¶  =¸  k¸  ÷¸  Õ¸  $¹  z¹  „¹  ‡·  ¸  ó¡  ¢  µ¢  µ¢  ¢  ¢  µ¢  µ¢  µ¢  .¢  =¢  L¢  [¢  j¢  µ¢  y¢  ˆ¢  —¢  ¦¢  D¯  e¯  †¯  Î¯  °  X°  •°  Ú°  ±  ±  ±  ±  ¥§  @¶  @¶  @¶  @¶  @¶  @¶  @¶  @¶  ¥§  µ  ¥§  @¶  Mµ  œµ  Ùµ  ¶  ¶  ¶  fƒy vfƒy v¶öÐ¶ÀÁèƒàÃ¶ÁèƒàÃÌÌÌÌÌÌÌÌÌHƒì(…ÉtiÿÉtYÿÉtIÿÉt9ÿÉtHAF  HƒÄ(Ãÿ6  HF  H8¦  D‹Àÿg  H(¦  HƒÄ(ÃHÜE  HƒÄ(ÃH°E  HƒÄ(ÃH„E  HƒÄ(ÃHpE  HƒÄ(ÃÌÌÌHƒì(Hƒ=Ì¤   u
¸   HƒÄ(ÃH‹ÑH‰\$ H¹¤  H‹ËA¸  ÿ"  Æ¦¥   HƒÈÿHÿÀ€< u÷H‹ËH‰”¥  ÿ~  H‹\$ 3É…Àº   DÊ‹ÁHƒÄ(Ãÿ%˜  ÿ%r  ÿ%  ÿ%¾  ÿ%à  ÌÌÌÌÌÌff„     H;)r  uHÁÁf÷ÁÿÿuóÃHÁÉéM   Ì@SHƒì H‹Ùÿ  ¹   ‰æ‚  èg  H‹Ëèe  ƒ=Ò‚   u
¹   èL  ¹	 ÀHƒÄ [éI  ÌÌÌH‰L$Hƒì8¹   èå  …Àt¹   Í)H¿}  è(  H‹D$8H‰¦~  HD$8HƒÀH‰6~  H‹~  H‰ }  H‹D$@H‰~  ÇÚ|  	 ÀÇÔ|     ÇÞ|     ¸   HkÀ HÖ|  HÇ   ¸   HkÀ H‹q  H‰L ¸   HkÀH‹q  H‰L Hµ  èèþÿÿHƒÄ8ÃÌÌÌHƒì(¹   è   HƒÄ(ÃÌ‰L$Hƒì(¹   èþ  …Àt‹D$0‹ÈÍ)H×|  è:  H‹D$(H‰¾}  HD$(HƒÀH‰N}  H‹§}  H‰|  Çþ{  	 ÀÇø{     Ç|     ¸   HkÀ Hú{  ‹T$0H‰H  è6þÿÿHƒÄ(ÃÌHƒì(¸MZ  f90@ÿÿt3Éë8Hc_@ÿÿH@ÿÿHÁ8PE  uã¹  f9HuØ3Éƒ¸„   v	9ˆø   •Á‰  ¹   ÿñ  HƒÉÿÿÿ  H‹è  H‰9£  H‰:£  ‹è€  ‰H‹{  ‹Í€  ‰è:  è‘  ƒ=ªo   uH  ÿ{  ƒ=˜o  ÿu	ƒÉÿÿq  3ÀHƒÄ(ÃÌÌHƒì8H1  èÜ  ‹‚€  D‹w€  ‰i€  Hb€  LO€  H@€  H1€  H‰D$ ÿ>  ‰$€  …Ày
¹   èü  HƒÄ8ÃÌ@WHƒì eH‹%0   H‹H3ÿ3ÀðH±L¢  tH;Áu¿   ëëå‹>¢  ƒøu
Hè³  ë?‹)¢  …Àu+Ç¢     H  HÝ  è¤  …Àt¸ÿ   éð   Ç„     ‹ê¡  ƒøuH¦  H  èt  ÇÈ¡     …ÿu	3ÀH‡³¡  Hƒ=Ë¡   t"HÂ¡  è  …ÀtE3ÀAP3ÉH‹©¡  ÿÐH‹0  H‹é  H‰L‹  H‹  ‹  èmmÿÿ‰ï~  ƒ=   u‹Èÿž  ƒ=Û~   uÿ×  ‹É~  ë-‰Á~  ƒ=Ú~   u	‹ÈÿÀ  Ìƒ=¬~   uÿ¨  ‹š~  HƒÄ _ÃHƒì(èc  HƒÄ(é~þÿÿÿ%Ô  ÿ%Æ  ÿ%¸  ÿ%ª  ÿ%œ  Hƒì(H‹8csmàuƒxu‹H àúlæƒøvù @™t3ÀHƒÄ(ÃèK  ÌHƒì(H½ÿÿÿè@  3ÀHƒÄ(ÃÌÿ%>  ÿ%0  3ÀÃÌÌÌÌÌÌÌÌÌÌÌÌÌLcA<E3ÉL‹ÒLÁA·@E·XHƒÀIÀE…Ût‹PL;Òr
‹HÊL;ÑrAÿÁHƒÀ(E;Ërâ3ÀóÃÌÌÌÌÌÌÌÌÌÌÌH‰\$WHƒì H‹ÙH=œ<ÿÿH‹Ïè4   …Àt"H+ßH‹ÓH‹Ïè‚ÿÿÿH…Àt‹@$Áè÷Ðƒàë3ÀH‹\$0HƒÄ _ÃÌÌÌH‹Á¹MZ  f9t3ÀÃHcH<HÈ3À9PE  uº  f9Q”ÀóÃÌH‰\$ UH‹ìHƒì H‹l  Hƒe H»2¢ß-™+  H;ÃuoHMÿ6  H‹EH‰Eÿð  ‹ÀH1Eÿ,  HM ‹ÀH1Eÿ$  ‹E HÁà HMH3E H3EH3ÁH¹ÿÿÿÿÿÿ  H#ÁH¹3¢ß-™+  H;ÃHDÁH‰™k  H‹\$HH÷ÐH‰’k  HƒÄ ]Ã@SHƒì Hƒ=îž   u6º   Jÿ  H‹ÈH‹Øÿ‚  H‰Ëž  H‰¼ž  H…ÛuCëHƒ# 3ÀHƒÄ [ÃÌÌ@SHƒì H‹ÙH‹œž  ÿN  H‰D$8HƒøÿuH‹Ëÿš  ë~¹   è   H‹nž  ÿ   H‰D$8H‹Tž  ÿ  H‰D$@H‹Ëÿø
  H‹ÈLD$@HT$8èì   H‹ØH‹L$8ÿØ
  H‰!ž  H‹L$@ÿÆ
  H‰ž  ¹   èµ   H‹ÃHƒÄ [ÃHƒì(èGÿÿÿH÷ØÀ÷ØÿÈHƒÄ(ÃÌH‰\$WHƒì Hc>  H=\>  ëH‹H…ÀtÿÐHƒÃH;ßríH‹\$0HƒÄ _ÃH‰\$WHƒì H;>  H=4>  ëH‹H…ÀtÿÐHƒÃH;ßríH‹\$0HƒÄ _Ãÿ%Þ
  ÿ%Ð
  ÿ%Â
  ÿ%¤
  ÿ%–
  ÿ%ˆ
  ÿ%z
  ÿ%d
  ÿ%æ	  ÌÌHƒì(M‹A8H‹ÊI‹Ñè   ¸   HƒÄ(ÃÌÌÌ@SHƒì E‹H‹ÚL‹ÉAƒãøAö L‹ÑtA‹@McP÷ØLÑHcÈL#ÑIcÃJ‹H‹C‹HHKöAt¶AƒàðH˜LÈL3ÊI‹ÉHƒÄ [é÷ÿÿÌÌÌÌÌÌÌff„     HƒìL‰$L‰\$M3ÛLT$L+ÐMBÓeL‹%   M;ÓsfAâ ðM› ðÿÿAÆ M;ÓuðL‹$L‹\$HƒÄÃÿ%D	  ÿ%F	  ÿ%H	  ÿ%’  ÿ%D	  ÌÌÌÌ@UHƒì H‹êH‹H‹Ñ‹èŽûÿÿHƒÄ ]ÃÌÌÌ@UHƒì H‹êH‹3É8  À”Á‹ÁHƒÄ ]ÃÌ@UHƒì H‹ê¹   è•þÿÿHƒÄ ]ÃÌ                                                                                                     d!     Ð      ä      ô      !     $!     ”!     z!     ¼      N!     4!             ¼!     È!     Ò!     Ü!     ð!     ²!     ²      ¤      –      Œ      „      `      L      @      4                        î     Ø     Î     Æ     ´     ¤     –     ˆ     j     L     4               ì     Þ     Ô     Æ     ¼     °     ¦     œ     ’     †     |     r     j     ^     T     J     B     8     .     $                    ü     ò     è     Þ     Ô     Ì     Ä     ¼     ²     ª           ˜     Ž     „     z     l     b     X     N     F     >     4     ,     æ!             	      €      €      €      €      €      €      €      €      €      €      €o      €        *     T     f     „     œ     ¨     Ê     à     ð          .     H     h     ˆ     –     ¤     ¾     Ú     ö           ô     Ü     Ð     ¸     ¢     Š     n     T     .          î     ú     Þ     ´     ”     ˆ     t     b     D     4                ð     Ú     Ì     °     š     |     l     N     4     &          ü     ì     Ü     È     ²     ž     ˆ     r     V     D     2          
     ê     à     Ò     ¼           ˆ     v     V     @     4     &          ü     ä     Æ     ´     ž     Œ     t     ^     L     2                ô     æ     Ø     Â     ¤     ”     Š     x     f     P     >     ,          ö     æ     Ô     Â     ª     ’     €     f     R     D     2     &                è     Ð     ¾     ¬          ˜                     À @                   À¿ @   |Ä @   ÌÂ @               ò6\       I      ï      ò6\    
      ü üï      ò6\          ì ìï      €;@    <@   _SERVER title   interactive     process-begin   no-chdir        php-ini define  process-end     profile-info    process-file    file    help    info    syntax-check    modules no-php-ini      no-header       process-code    hide-args   run syntax-highlight        syntax-highlighting server      docroot strip   usage   version zend-extension  rf      rfunction   rc  rclass  re      rextension  rz  rzendextension  ri      rextinfo    ini %s
     PHP_SELF        SCRIPT_NAME     SCRIPT_FILENAME PATH_TRANSLATED DOCUMENT_ROOT   0       report_zend_debug   1   display_errors  cli     Command Line Interface  extension_filename  dl  cli_set_process_title   cli_get_process_title   php             Usage: %s [options] [-f] <file> [--] [args...]
   %s [options] -r <code> [--] [args...]
   %s [options] [-B <begin_code>] -R <code> [-E <end_code>] [--] [args...]
   %s [options] [-B <begin_code>] -F <file> [-E <end_code>] [--] [args...]
   %s [options] -S <addr>:<port> [-t docroot]
   %s [options] -- [args...]
   %s [options] -a

  -a               Run interactively
  -c <path>|<file> Look for php.ini file in this directory
  -n               No php.ini file will be used
  -d foo[=bar]     Def